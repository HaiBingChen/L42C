#include "kl17_dfu.h"
#include "bootloader.h"
#include "nrf51_bitfields.h"
#include "app_util.h"
#include "dfu.h"
#include "dfu_transport.h"
#include "app_error.h"
#include "nrf51.h"
#include "nrf_delay.h"
#include "dfu_bank_internal.h"

uint8_t CustomUpdate;								/*全局变量*/
uint32_t  CustomNordic_Addr =0;						/*Nordic的地址*/
uint32_t  CustomNordic_Size =0;						/*Nordic的大小*/
uint32_t  CustomNordic_Crc =0;						/*Nordic的crc值*/
uint16_t  OtaResult=0;									/*OTA的结果*/

static custom_pkt_type_t Custom_Pkt_Type;				/*数据包类型*/
static uint32_t  Custom_Total_Size =0;					/*所有数据的总大小，用于计算进度条*/
static uint32_t  Res_Addr =0;							/*字库地址*/
static uint32_t  CustomType_Addr =0;					/*升级种类的地址*/
static uint32_t  CustomType_Size =0;					/*升级种类的大小*/
static uint32_t  CustomType_Tag_Addr = 0;				/*升级种类存储TAG的地址*/
static uint32_t  CustomType_Size_Addr =0;				/*升级种类存储SIZE的地址*/
static uint32_t  CustomType_Crc =0;						/*升级种类的crc值*/

static uint32_t  PineIndexGetSize=0;					/*进度条获取的总大小*/
static uint32_t  PineIndexSize=0;						/*进度条步进的大小*/
static uint32_t  PineID=0;								/*进度条步进的格数*/


static uint16_t  Custom_pkt_notif_target;				/*记录是否需要回传*/
static uint16_t  Custom_pkt_notif_target_cnt;			/*记录几包回传*/
static uint32_t  Custom_num_of_firmware_bytes_rcvd;  	/*接收到多少字节的数据了*/	

/*************************************串口驱动*************************************/
void UartInitial(void)
{
	/* Power on */
	NRF_UART0->POWER = (UART_POWER_POWER_Enabled << UART_POWER_POWER_Pos);

	/* initialize TXD */
	NRF_UART0->PSELTXD = 11;//UART_InitStruct.txd_pin_number;

	/* initialize RXD */
	NRF_UART0->PSELRXD = 10;//UART_InitStruct.rxd_pin_number;

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


void UART_Send( uint8_t *buf, int size)
{
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

void OtaSendCmd(OtaCmd cmd,uint8_t data)
{
	uint8_t Sdata[5];
	Sdata[0]=0xff;
	Sdata[1]=(uint8_t)cmd;
	Sdata[2]=data;
	Sdata[3]=0x0a;
	Sdata[4]=0x0d;
	UART_Send(Sdata,5);
}
/*************************************SPI Flash驱动*************************************/
void Select_Flash_CS(void)
{
	FLASH_CS_L;
}

void Release_Flash_CS(void)
{
	FLASH_CS_H;
}

void SPI_Write(uint8_t data)
{
	for(uint8_t i=0;i<8;i++)
	{  
		FLASH_CLK_L;
		if(data&0x80)
			FLASH_DI_H;
		else
			FLASH_DI_L;
 
		FLASH_CLK_H;
  
		data<<=1;
	}  
}

uint8_t SPI_Read(void)
{
	uint8_t data=0;    
	for(int i=7;i>=0;i--)
	{
		FLASH_CLK_L;

		FLASH_CLK_H;
		if(FLASH_DO_DATA)
			data|=(0x1<<i);
	}  
	return data;
}
void Power_Suspend(void)
{
	Select_Flash_CS();
       SPI_Write(E_DPD);
	Release_Flash_CS();	
}    

void Power_Resume(void)
{
	Select_Flash_CS();
       SPI_Write(E_RDPD);
	Release_Flash_CS();	
}    

void WriteEnable(void)
{
	Select_Flash_CS();
       SPI_Write(E_WREN);
	Release_Flash_CS();	
}
void WriteDisable(void)
{
        Select_Flash_CS();
        SPI_Write(E_WRDI);
        Release_Flash_CS();	
}

void UnlockGlobalBlockProtection(void)
{
	Power_Resume();
	WriteEnable();
	Select_Flash_CS();
       SPI_Write(E_ULBPR);
	Release_Flash_CS();		
	Power_Suspend();
}

void WaitForStatusRegister(uint8_t status)
{
	uint8_t FLASH_Status = 0;
	uint32_t ECount=0;
	Select_Flash_CS();
	SPI_Write(E_RDSR); 
	FLASH_DO_IN;
	do
	{
		FLASH_Status =  SPI_Read();
		nrf_delay_us(10); 
		ECount++;
	}	
	while ((FLASH_Status & status)&&(ECount<300000));//若没有写完则一直等待，保持CS=0
	FLASH_DO_OUT;
	Release_Flash_CS();
}

void SpiFlash_4KSector_Erase(uint32_t EraseAddr)
{
	Power_Resume();
	WriteEnable();
	Select_Flash_CS();
    	SPI_Write(E_SE);
	SPI_Write((uint8_t)(EraseAddr >> 16));
	SPI_Write((uint8_t)(EraseAddr>> 8));
	SPI_Write((uint8_t)(EraseAddr));	
	Release_Flash_CS();
	WaitForStatusRegister(SPI_FLASH_BUSY);
	Power_Suspend();
}
void SpiFlash_64KBlock_Erase(uint32_t EraseAddr)
{
	Power_Resume();
	WriteEnable();
	Select_Flash_CS();
    	SPI_Write(E_BE);
	SPI_Write((uint8_t)(EraseAddr >> 16));
	SPI_Write((uint8_t)(EraseAddr>> 8));
	SPI_Write((uint8_t)(EraseAddr));	
	Release_Flash_CS();
	WaitForStatusRegister(SPI_FLASH_BUSY);
	Power_Suspend();
}  

void SpiFlash_PageWrite(uint32_t addr,uint8_t* pBuf,uint16_t len)
{  
	WriteEnable();
	Select_Flash_CS();
	SPI_Write(E_PP);//页写指令
	SPI_Write((uint8_t)(addr >> 16));
	SPI_Write((uint8_t)(addr>> 8));
	SPI_Write((uint8_t)(addr));
	while(len--)
	{
		SPI_Write(*pBuf);
		pBuf++;
	}
	Release_Flash_CS();	
	WaitForStatusRegister(SPI_FLASH_BUSY);
}

void SpiFlash_Write(uint32_t addr,uint8_t*pBuf, uint16_t len)
{
	uint8_t NumOfPage = 0, NumOfSingle = 0, Addr = 0, count = 0, temp = 0;
	Addr = addr%EFLASH_PAGE_SIZE;					/*	确定地址在单页内位置*/
	count = EFLASH_PAGE_SIZE - Addr;					/*	确定地址在单页剩余可写字节数*/
	NumOfPage =  len/EFLASH_PAGE_SIZE;				/*	确定要写入数据需要多少整页*/
	NumOfSingle = len%EFLASH_PAGE_SIZE;				/*	确定要写入数据除去整页的剩余字节数*/
	Power_Resume();
	if (Addr == 0) 									/* 写入的地址是页对齐*/
	{
		if (NumOfPage == 0)							/* 写入数据量小于一页*/
			SpiFlash_PageWrite(addr, pBuf, len);
		else 										/* 写入数据量大于一页*/
		{
			while (NumOfPage--)						/* 写整页的*/
			{
				SpiFlash_PageWrite(addr,pBuf, EFLASH_PAGE_SIZE);
				addr +=  EFLASH_PAGE_SIZE;
				pBuf += EFLASH_PAGE_SIZE;
			}
			SpiFlash_PageWrite(addr, pBuf,NumOfSingle);  	/* 写剩余的*/
		}
	}
	else 											/* 写入的地址不是页对齐*/
	{
		if (NumOfPage == 0) 							/* 写入数据量小于一页*/
		{
			if (NumOfSingle > count)					/* 写入数据量大于本页剩余字节数*/
			{ 				
				temp = NumOfSingle - count;
				SpiFlash_PageWrite(addr,pBuf, count);		/* 先写完本页剩余*/
				addr +=  count;
				pBuf += count;
				SpiFlash_PageWrite(addr, pBuf, temp);		/* 剩下的写到下一页*/
			}
			else
				SpiFlash_PageWrite(addr, pBuf,len);	 	/* 本页就能装下*/
		}
		else 										/* 写入数据量大于一页*/
		{
			len -= count;
			NumOfPage =  len / EFLASH_PAGE_SIZE;
			NumOfSingle = len % EFLASH_PAGE_SIZE;
			SpiFlash_PageWrite(addr, pBuf, count);		/* 先写完本页剩余，后面就按页对齐的来*/	
			addr +=  count;
			pBuf += count;
			while (NumOfPage--)
			{
				SpiFlash_PageWrite(addr,pBuf, EFLASH_PAGE_SIZE);
				addr +=  EFLASH_PAGE_SIZE;
				pBuf += EFLASH_PAGE_SIZE;
			}
			if (NumOfSingle != 0)
				SpiFlash_PageWrite(addr,pBuf,NumOfSingle);
		}
	}
	Power_Suspend();
}

void SpiFlash_Read( uint32_t addr,uint8_t*pBuf , uint16_t len)
{
	Power_Resume();
	Select_Flash_CS();
	SPI_Write(E_READ);//写读取数据指令    
	SPI_Write((uint8_t)(addr >> 16));
	SPI_Write((uint8_t)(addr>> 8));
	SPI_Write((uint8_t)(addr));
	FLASH_DO_IN;
	while (len--) 
	{
		*pBuf = SPI_Read();
		pBuf++;
	}
	FLASH_DO_OUT;
	Release_Flash_CS(); 
	Power_Suspend();
}


void SpiFlash_Init(void)
{
	FLASH_CS_OUT;          
	FLASH_CLK_OUT;        
	FLASH_DI_OUT;       
	FLASH_CS_H;
	FLASH_CLK_H;
	FLASH_DI_H;
	UnlockGlobalBlockProtection();
}

void ReadWatchId(uint8_t *dat)
{			
	SpiFlash_Read(UserData_Base,dat,20);
}

/*************************************OTA代码*************************************/
void Custom_Ota_Init(void)
{
	CustomType_Size =0;
	CustomType_Crc =0;
	Custom_pkt_notif_target =0;
	Custom_pkt_notif_target_cnt =0;
	Custom_num_of_firmware_bytes_rcvd =0;
}

uint16_t Custom_crc16_compute(void)
{
    uint32_t i;
    uint8_t p_data[128]={0x00};	    	
    uint16_t crc =0xffff;
    uint8_t size =0;	
    bool CrcEnd =true;	
    uint32_t  CrcAddrOffset = 0;	
    while (CrcEnd)
    {
    	    SpiFlash_Read((CustomType_Addr+CrcAddrOffset),p_data,128);
	    if((CrcAddrOffset+128)>=CustomType_Size)
	    {
			size = CustomType_Size-CrcAddrOffset ;
			CrcEnd =false;
	    }		
	    else
	    {
			size = 128;
			CrcAddrOffset+=128;	
	    }
	    for (i = 0; i < size; i++)
	    {
	        crc  = (unsigned char)(crc >> 8) | (crc << 8);
	        crc ^= p_data[i];
	        crc ^= (unsigned char)(crc & 0xff) >> 4;
	        crc ^= (crc << 8) << 4;
	        crc ^= ((crc & 0xff) << 4) << 1;
	    }
    }
    return crc;
}

void Custom_set_tag(void)
{
	uint8_t CUSTOM_TAG[4]={0x00};
	if(CustomUpdate==UPDATE_KL17)
	{
		uint8_t Ctag[4]="KL17";
		memcpy(CUSTOM_TAG,Ctag,4);
	}
	else if(CustomUpdate==UPDATE_TP)
	{
		uint8_t Ctag[4]="UPTP";
		memcpy(CUSTOM_TAG,Ctag,4);
	}
	else if(CustomUpdate==UPDATE_HR)
	{
		uint8_t Ctag[4]="UPHR";
		memcpy(CUSTOM_TAG,Ctag,4);
	}	
	if(CUSTOM_TAG[0]!=0x00)
	{
		uint8_t Csize[4]={0x00}; 
		Csize[0] = (CustomType_Size>>24)&0xff;
		Csize[1] = (CustomType_Size>>16)&0xff;
		Csize[2] = (CustomType_Size>>8)&0xff;
		Csize[3] = CustomType_Size&0xff;
		SpiFlash_Write(CustomType_Tag_Addr,CUSTOM_TAG,4);  
		SpiFlash_Write(CustomType_Size_Addr,Csize,4); 	
	}
	if(CustomUpdate!=UPDATE_NORDIC)
		OtaResult &= (~(CustomUpdate));

}

static void Custom_dfu_pkt_write(ble_dfu_t * p_dfu, ble_dfu_evt_t * p_evt)
{
	uint32_t  err_code;
	uint8_t * C_data = p_evt->evt.ble_dfu_pkt_write.p_data;
	
	switch(Custom_Pkt_Type)
	{
		case CUSTOM_PKT_TYPE_USER:				/*09的数据*/
			{
				uint32_t TotalSize = uint32_decode(C_data);
				if(TotalSize>0)
				{
					Custom_Total_Size = TotalSize;
					PineIndexGetSize=0;
					PineID=1;
					OtaSendCmd(O_PROGRESS,0);	/*进度条*/
				}
				Res_Addr = uint32_decode(C_data+4);
				err_code = ble_dfu_response_send(p_dfu, BLE_DFU_USER_PROCEDURE, BLE_DFU_RESP_VAL_SUCCESS);		/*返回 10 09 01——>1531*/			
				APP_ERROR_CHECK(err_code);
			}
			break;
			
		case CUSTOM_PKT_TYPE_START:			/*01的数据*/
			CustomType_Size = uint32_decode(C_data + 8);	
			if(CustomUpdate==UPDATE_EXFLASH)
			{
				uint16_t UsePage =0;
				UsePage = CustomType_Size/4096;
				if(CustomType_Size%4096!=0)
					UsePage++;
				for(uint16_t i=0;i<UsePage;i++)
					SpiFlash_4KSector_Erase(CustomType_Addr+i*4096);					
			}
			else if(CustomUpdate==UPDATE_NORDIC)
			{
				 if(!IS_WORD_SIZED(CustomType_Size))
				 {
					err_code=ble_dfu_response_send(p_dfu, BLE_DFU_START_PROCEDURE, BLE_DFU_RESP_VAL_DATA_SIZE);  	 /*返回 10 01 04——>1531*/
					APP_ERROR_CHECK(err_code);		
					break; 
				 }
			}
			err_code=ble_dfu_response_send(p_dfu, BLE_DFU_START_PROCEDURE, BLE_DFU_RESP_VAL_SUCCESS);  	 		/*返回 10 01 01——>1531*/
			APP_ERROR_CHECK(err_code);		
			break;
			
		case CUSTOM_PKT_TYPE_INIT:				/*02的数据*/
			CustomType_Crc = uint16_decode(C_data + 12);
			if(CustomUpdate==UPDATE_NORDIC)
			{
				CustomNordic_Addr =CustomType_Addr;
				CustomNordic_Size =CustomType_Size;
				CustomNordic_Crc   =CustomType_Crc;					
			}
			break;

		case CUSTOM_PKT_TYPE_FIRMWARE_DATA:	/*03的数据*/
			{
				uint32_t  length = p_evt->evt.ble_dfu_pkt_write.len;
				dfu_timer_restart();
				SpiFlash_Write(CustomType_Addr+Custom_num_of_firmware_bytes_rcvd,C_data,length);
				Custom_num_of_firmware_bytes_rcvd+= length;
				PineIndexGetSize+=length;
				/*进度条*/
				if(PineIndexGetSize>=(PineID*PineIndexSize-60))
				{
					OtaSendCmd(O_PROGRESS,PineID);
					PineID++;
				}
				
				if(Custom_num_of_firmware_bytes_rcvd<CustomType_Size)
				{
					if(Custom_pkt_notif_target>0)		/*需要回传*/
					{
						Custom_pkt_notif_target_cnt--;
						if(Custom_pkt_notif_target_cnt == 0)
						{
							err_code=ble_dfu_pkts_rcpt_notify(p_dfu, Custom_num_of_firmware_bytes_rcvd);	/*返回 11 长度——>1531*/
							APP_ERROR_CHECK(err_code);	
							Custom_pkt_notif_target_cnt = Custom_pkt_notif_target;
						}
					}
				}
				else if(Custom_num_of_firmware_bytes_rcvd == CustomType_Size)
				{
					err_code=ble_dfu_response_send(p_dfu, BLE_DFU_RECEIVE_APP_PROCEDURE, BLE_DFU_RESP_VAL_SUCCESS);		/*返回 10 03 01——>1531*/
					APP_ERROR_CHECK(err_code);
				}
				else
				{
					err_code=ble_dfu_response_send(p_dfu, BLE_DFU_RECEIVE_APP_PROCEDURE, BLE_DFU_RESP_VAL_DATA_SIZE);		/*返回 10 03 04——>1531*/
					APP_ERROR_CHECK(err_code);
				}
			}
			break;

		default:
			break;	
	}
}

void Custom_OTA_handler(ble_dfu_t * p_dfu, ble_dfu_evt_t * p_evt)
{
	uint32_t  err_code;
	ble_dfu_resp_val_t resp_val;
	switch (p_evt->ble_dfu_evt_type)
	{
		case BLE_DFU_USER: 						/*09*/	
			m_update_status = BOOTLOADER_UPDATING;
			Custom_Pkt_Type=CUSTOM_PKT_TYPE_USER;
		       break;	
			   
		case BLE_DFU_START:  					/*01 xx*/
			Custom_Ota_Init();
			Custom_Pkt_Type =CUSTOM_PKT_TYPE_START;
			break;

		case BLE_DFU_RECEIVE_INIT_DATA:   		/*02 00 */  /*02 01 */
			Custom_Pkt_Type =CUSTOM_PKT_TYPE_INIT;
			if ((uint8_t)p_evt->evt.ble_dfu_pkt_write.p_data[0] == 0x01)
			{
				err_code=ble_dfu_response_send(p_dfu, BLE_DFU_INIT_PROCEDURE, BLE_DFU_RESP_VAL_SUCCESS);   			/*返回 10 02 01——>1531*/
				APP_ERROR_CHECK(err_code);
			}			
			break;	
			
		case BLE_DFU_PKT_RCPT_NOTIF_ENABLED:	/*08 xx  xx*/
			Custom_pkt_notif_target =  p_evt->evt.pkt_rcpt_notif_req.num_of_pkts;
			Custom_pkt_notif_target_cnt = p_evt->evt.pkt_rcpt_notif_req.num_of_pkts;
			break;	
		case BLE_DFU_PKT_RCPT_NOTIF_DISABLED:	/*08 00 00*/
			Custom_pkt_notif_target =  0;
			break;	

		case BLE_DFU_RECEIVE_APP_DATA:			/*03*/
			Custom_Pkt_Type =CUSTOM_PKT_TYPE_FIRMWARE_DATA;
			break;
			
		case BLE_DFU_VALIDATE:					/*04*/
			{
				uint16_t image_crc;				
				image_crc = Custom_crc16_compute();			
				if(image_crc == CustomType_Crc)
				{
					resp_val = BLE_DFU_RESP_VAL_SUCCESS;
					Custom_set_tag();	
				}
				else		
					resp_val = BLE_DFU_RESP_VAL_CRC_ERROR;		
				err_code = ble_dfu_response_send(p_dfu, BLE_DFU_VALIDATE_PROCEDURE, resp_val);  		/*返回 10 04 01——>1531*/	
				APP_ERROR_CHECK(err_code);
			}
			break;

		case BLE_DFU_ACTIVATE_N_RESET:			/*05*/
			err_code = dfu_transport_close();
            		APP_ERROR_CHECK(err_code);							
			if((OtaResult&0x0004)!=0x00)			/*升级了Nordic*/
				Appscomm_Erase_Nordic();
			else	
           			dfu_reset();
			break;

			
		case BLE_DFU_PACKET_WRITE:				/*数据包传输*/
			Custom_dfu_pkt_write(p_dfu,p_evt);
			break;		
			
		default:
			break;		
	}
}



uint8_t IsCustomDFU(uint8_t type)
{
	uint8_t Buffer[5]={0x00};
	uint8_t Pos0=0,Pos1=0,Pos2=0;
	uint8_t PinexIndex=9;
	uint32_t AddrTemp=0,PageCount=0;
	CustomUpdate = type;
	OtaResult |= UPDATE_DFU_FLAG;
	OtaResult |= type;
	switch(type)
	{
		case UPDATE_NORDIC:
			Pos0 =40; Pos1=45;Pos2=49;
			break;
			
		case UPDATE_KL17:
			Pos0 =0; Pos1=16;Pos2=20;
			break;	
			
		case UPDATE_TP:
			Pos0 =5; Pos1=24;Pos2=28;
			PinexIndex=4;
			break;
			
		case UPDATE_HR:
			Pos0 =10; Pos1=32;Pos2=36;
			break;	
			
		case UPDATE_EXFLASH:					/*大小后面给,所以后面再清Flash*/
			CustomType_Addr = Res_Addr;
			if(PineIndexSize==0)
				PineIndexSize = Custom_Total_Size/9;
			return CustomUpdate;
			
		default:
			return CustomUpdate;
	}
	SpiFlash_Read((UserData_Base+0x6000UL+Pos0),Buffer,5);
	AddrTemp= (Buffer[0] << 24) | (Buffer[1] << 16) | (Buffer[2] << 8) | (Buffer[3]); 
	PageCount = Buffer[4];
	if(CustomUpdate==UPDATE_NORDIC)			
	{
		if((AddrTemp>0x003FFFFFUL)||(AddrTemp==0x00000000UL))
		{
			AddrTemp = 0x3C8000UL;
			PageCount= 36;
		}
	}
	if(PageCount!=0xFF)
	{
		CustomType_Addr         = AddrTemp;
		CustomType_Tag_Addr = UserData_Base+0x6000UL+Pos1;
		CustomType_Size_Addr = UserData_Base+0x6000UL+Pos2;	
		for(uint8_t i =0;i<PageCount; i++)
			SpiFlash_4KSector_Erase(CustomType_Addr+(i*0x1000UL));		
	}
	if(PineIndexSize==0)
		PineIndexSize = Custom_Total_Size/PinexIndex;
	return CustomUpdate;
}


void StoreOtaResult(void)
{
	uint8_t Result =0;
	if(OtaResult==UPDATE_DFU_FLAG)
		Result =1;
	else if((OtaResult&0x0004)!=0x00)
	{
		OtaSendCmd(O_RESULT,2);
		return;
	}
	if(OtaResult>0)
		SpiFlash_Write(UserData_Base+0x6000UL+15,&Result,1);
}


