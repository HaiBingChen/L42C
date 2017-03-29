/* Copyright (c) 2013 Nordic Semiconductor. All Rights Reserved.
 *
 * The information contained herein is property of Nordic Semiconductor ASA.
 * Terms and conditions of usage are described in detail in NORDIC
 * SEMICONDUCTOR STANDARD SOFTWARE LICENSE AGREEMENT.
 *
 * Licensees are granted free, non-transferable use of the information. NO
 * WARRANTY of ANY KIND is provided. This heading must NOT be removed from
 * the file.
 *
 */
#include "headfile.h"
#include "app_fifo.h"
#include "nrf.h"
#include "nrf_drv_gpiote.h"
#include "nrf_gpio.h"
#include "app_error.h"
#include "app_util.h"
#include <string.h>
#include <stdarg.h>
bool Update_flags= false;
app_fifo_t  app_fifo;

//开始
#define XMODEM_SOH                1
//发送完成
#define XMODEM_EOT                4
//成功
#define XMODEM_ACK                6
//失败，要求重发
#define XMODEM_NAK                21
//无条件终止传输
#define XMODEM_CAN                24
//发送表示等待接收
#define XMODEM_NCG                67

#define XMODEM_DATA_SIZE          128

typedef struct
{
  uint8 header;       //头 SOH
  uint8 packetNumber; //信息包序号，从 01 开始以发送一包将加1，加到FF hex 将循环。
  uint8 packetNumberC;//信息包序号的补码。
  uint8 data[XMODEM_DATA_SIZE];//128数据
  uint8 crcHigh;//CRC16 高字节
  uint8 crcLow; //CRC16 低字节
  
  uint8 padding; /* Padding to make sure data is 32 bit aligned. */
  
} XMODEM_packet;

uint16 UartRevValueOffset=0;
uint8 UartDataRevBuf[DATA_REV_MAX]={0};


void InitUartBuffer(void)
{
	memset(UartDataRevBuf,0,DATA_REV_MAX);
	UartRevValueOffset=0;
}

uint16 CRC_calc(uint8 *start, uint8 *end)
{
	uint16 crc = 0x0;
	uint8  *data=start;

	for (data = start; data < end; data++)
	{
		crc  = (crc >> 8) | (crc << 8);
		crc ^= *data;
		crc ^= (crc & 0xff) >> 4;
		crc ^= crc << 12;
		crc ^= (crc & 0xff) << 5;
	}
	return crc;
}
uint16 cur_crc;
int VerifyPacketChecksum(XMODEM_packet *pkt)
{
	uint16 packetCRC;
	uint16 calculatedCRC;	
	if (pkt->packetNumber + pkt->packetNumberC != 255)
	{
		return -1;
	}
	if(pkt->header !=XMODEM_SOH)//?????
	{
		return -1;
	}
	calculatedCRC = CRC_calc((uint8 *) pkt->data, (uint8 *) &(pkt->crcHigh));
	packetCRC     = pkt->crcHigh << 8 | pkt->crcLow;
	if (calculatedCRC != packetCRC)
	{
		return -1;
	}
	//CrcTotalValue+=packetCRC;
	cur_crc = packetCRC;
	return 0;
}

#define RETRY_TIMES 3
uint8 XmodemRevPktToSpiFlash(void)
{
	uint8 try_time;
	uint8 buf[XMODEM_DATA_SIZE];
	uint8 answer=XMODEM_NAK;
	XMODEM_packet *pkt=(XMODEM_packet *)UartDataRevBuf;
	if(VerifyPacketChecksum(pkt)==0)//??????
	{
		try_time = 0;
		do
		{
			SpiFlash_Write(SpiFlash_Addr, pkt->data, XMODEM_DATA_SIZE);

			memset(buf, 0, XMODEM_DATA_SIZE);
			SpiFlash_Read(SpiFlash_Addr, buf, XMODEM_DATA_SIZE);
			if(0==memcmp(pkt->data, buf, XMODEM_DATA_SIZE))
			{
				answer = XMODEM_ACK;
				SpiFlash_Addr+=XMODEM_DATA_SIZE;
				CrcTotalValue+=cur_crc;
				break;
			}
		}while(++try_time <= RETRY_TIMES);
	}
	return answer;
}



void Uart0PacketParser(void)
{
	if(BC5_UART_FLAG)
	{
		Bc5DataParseHandler(&UartDataRevBuf);
		UartRevValueOffset=0;
	}
	else
	{
		uint8 DataRsp=DataParseHandler(UartDataRevBuf); 

		if(DataRsp !=E_RSP_NULL)
		SendRspCmd(CurrentDataParseCmd,(RSP_VAULE_E)DataRsp);

		UartRevValueOffset=0;
		osal_memset(UartDataRevBuf,0,DATA_REV_MAX);
	}
}

/**@brief Function for the UART Interrupt handler.
 *
 * @details UART interrupt handler to process TX Ready when TXD is available, RX Ready when a byte
 *          is received, or in case of error when receiving a byte.
 */
void UART0_IRQHandler(void)
{
	if (Update_flags)
	{
		if (NRF_UART0->EVENTS_RXDRDY != 0)
		{
			NRF_UART0->EVENTS_RXDRDY = 0; // Clear UART RX event flag
			app_fifo_put(&app_fifo, (uint8)NRF_UART0->RXD);
		}
		return;
	}

	if (NRF_UART0->EVENTS_RXDRDY != 0)
	{
		uint8 data=0;
		uint8 RetData = XMODEM_ACK;

		NRF_UART0->EVENTS_RXDRDY = 0; // Clear UART RX event flag
		data = (uint8)NRF_UART0->RXD;

		if(UartRevValueOffset<DATA_REV_MAX)
			UartDataRevBuf[UartRevValueOffset++] = data;

		if(UPDATE_FONTLIB)							/*XModem模式*/
		{
			if(UartRevValueOffset==1) 					/*数据头*/
			{
				switch(data)
				{
					case XMODEM_SOH:				/*包头*/
						break;	

					case XMODEM_EOT:				/*传输完成*/
						RetData = XMODEM_ACK;
						UART_Send(&RetData, 1);	
						osal_stop_timerEx(GetAppTaskId(), MSG_UPDATE_FONT_LIB_FAIL);									
						SendMsg(MSG_UPDATE_FONT_LIB_SUCCESS);		
						UartRevValueOffset =0 ;
						break;

					case 'v':							/*校验下载内容*/
						{
							char vstr[20]={0x00};
							UART_Send("v\r\n", 3);
							sprintf(vstr, "Value:%ld\r\n", (long)CrcTotalValue);
							UART_Send((uint8*)vstr, strlen(vstr));
							UartRevValueOffset =0;
						}
						break;

					case 'r':						       /*从XModem模式返回协议接收模式*/	
						UART_Send("r\r\n", 3);
						osal_start_timerEx(GetAppTaskId(), MSG_UPDATE_FONT_LIB_TIMEOUT, 2000);	
						UartRevValueOffset =0;
						break;

					default:
						UartRevValueOffset =0;
						break;
				}
			}
			if(UartRevValueOffset>=133)					 /*包数据解析*/				
			{
				UPDATE_FONTLIB_SEND_C =false;			
				RetData = XmodemRevPktToSpiFlash();
				UART_Send(&RetData, 1);
				osal_start_timerEx(GetAppTaskId(), MSG_UPDATE_FONT_LIB_FAIL,3000);	
				UartRevValueOffset =0;
			}
		} 
		else if(BC5_UART_FLAG)
		{
			if(data == 0x0D)
			{			
				if(UartRevValueOffset>=2 && UartDataRevBuf[UartRevValueOffset-2]==0x0A)
				{	 
					osal_set_event(GetHwTaskId(), HW_UART0_POLL);
				}
			}
		}
		else 										/*协议接收模式*/
		{
			osal_stop_timerEx(GetHwTaskId(),HW_UART0_POLL);
			if(data == 0x8F)							
			{
				osal_start_timerEx(GetHwTaskId(), HW_UART0_POLL,200);  
			}
		}

	}    
}


void UartInitial(void)
{
    InitUartBuffer();	
    /* Power on */
    NRF_UART0->POWER = (UART_POWER_POWER_Enabled << UART_POWER_POWER_Pos);

    /* initialize TXD */
    NRF_UART0->PSELTXD = UART_TX_PIN;//UART_InitStruct.txd_pin_number;

    /* initialize RXD */
    NRF_UART0->PSELRXD = UART_RX_PIN;//UART_InitStruct.rxd_pin_number;

   

    /* set baudrate */
    NRF_UART0->BAUDRATE = UART_BAUDRATE_BAUDRATE_Baud115200;
    
    /* set parity */
    NRF_UART0->CONFIG &= ~(UART_CONFIG_PARITY_Excluded << UART_CONFIG_PARITY_Pos);
    NRF_UART0->CONFIG |= (UART_CONFIG_PARITY_Excluded << UART_CONFIG_PARITY_Pos);
    

    NRF_UART0->ENABLE           = (UART_ENABLE_ENABLE_Enabled << UART_ENABLE_ENABLE_Pos);
    NRF_UART0->TASKS_STARTTX    = 1;
    NRF_UART0->TASKS_STARTRX    = 1;
    NRF_UART0->EVENTS_RXDRDY    = 0;

    /* Enable UART interrupt */
    NRF_UART0->INTENCLR = 0xFFFFFFFFUL; // clear all
    NRF_UART0->INTENSET = UART_INTENSET_RXDRDY_Set << UART_INTENSET_RXDRDY_Pos;

    NVIC_ClearPendingIRQ(UART0_IRQn);
    NVIC_SetPriority(UART0_IRQn, 3);
    NVIC_EnableIRQ(UART0_IRQn);
    UART0_OFF_FLAG =false;	
}


void UartInit_ChoosePin(uint8 cPin)
{
	switch (cPin)
	{
		case USB_PINS:
    			NRF_UART0->PSELTXD = UART_TX_PIN;
    			NRF_UART0->PSELRXD = UART_RX_PIN;
    			BC5_UART_FLAG =false;
			InitUartBuffer();	
			break;
#if 0
		case BC5_PINS:
    			NRF_UART0->PSELTXD = UART_TX_BC5_PIN;
    			NRF_UART0->PSELRXD = UART_RX_BC5_PIN;
    			BC5_UART_FLAG =true;	
			InitUartBuffer();	
			break;
#endif
		case KL17_PINS:
			break;
		default:
			break;
	}
	//nrf_delay_ms(1);
}


void  BootloaderUartInitial(void)
{
    /* Power on */
    NRF_UART0->POWER = (UART_POWER_POWER_Enabled << UART_POWER_POWER_Pos);

    /* initialize TXD */
    NRF_UART0->PSELTXD = UART_TX_KL17_PIN;//UART_InitStruct.txd_pin_number;

    /* initialize RXD */
    NRF_UART0->PSELRXD = UART_RX_KL17_PIN;//UART_InitStruct.rxd_pin_number;

   

    /* set baudrate */
    NRF_UART0->BAUDRATE = UART_BAUDRATE_BAUDRATE_Baud115200;
    
    /* set parity */
    NRF_UART0->CONFIG &= ~(UART_CONFIG_PARITY_Excluded << UART_CONFIG_PARITY_Pos);
    NRF_UART0->CONFIG |= (UART_CONFIG_PARITY_Excluded << UART_CONFIG_PARITY_Pos);
    

    NRF_UART0->ENABLE           = (UART_ENABLE_ENABLE_Enabled << UART_ENABLE_ENABLE_Pos);
    NRF_UART0->TASKS_STARTTX    = 1;
    NRF_UART0->TASKS_STARTRX    = 1;
    NRF_UART0->EVENTS_RXDRDY    = 0;

    /* Enable UART interrupt */
    NRF_UART0->INTENCLR = 0xFFFFFFFFUL; // clear all
    NRF_UART0->INTENSET = UART_INTENSET_RXDRDY_Set << UART_INTENSET_RXDRDY_Pos;

    NVIC_ClearPendingIRQ(UART0_IRQn);
    NVIC_SetPriority(UART0_IRQn, 3);
    NVIC_EnableIRQ(UART0_IRQn);

}


void UartPowerOff(void)
{
    /* Power off */
	UART0_OFF_FLAG =true;
	NRF_UART0->POWER = (UART_POWER_POWER_Disabled << UART_POWER_POWER_Pos);	
}
void UartPowerOn(void)
{
	UartInitial();
	if(BC5_UART_FLAG)
		UartInit_ChoosePin(BC5_PINS);	
}

void UART_Send( uint8_t *buf, int size)
{
	if(UART0_OFF_FLAG)
		return;
	for (int i = 0; i < size; i++)
	{
		NRF_UART0->TXD = buf[i];

		while (NRF_UART0->EVENTS_TXDRDY != 1)
		{
			/* do nothing */
		}
		NRF_UART0->EVENTS_TXDRDY = 0;
	}    
}


uint32_t app_uart_get(uint8_t * p_byte)
{
        return NRF_SUCCESS;
}


uint32_t app_uart_put(uint8_t byte)
{
        return NRF_SUCCESS;
}


#if defined (DEBUG_ENABLE)
void TRACE(const char *fmt, ...)
{
	int nLen;
	va_list args;
	char szBuffer[64];

	va_start(args, fmt);
	nLen = vsprintf(szBuffer, fmt, args);
	va_end(args);

	UART_Send((uint8_t*)szBuffer, nLen);
}

#else
void TRACE(const char *fmt, ...)
{
}
#endif









