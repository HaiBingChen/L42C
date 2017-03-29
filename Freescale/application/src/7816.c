#include <string.h>
#include "7816.h"
#include "spi.h"
#include "SCD_HB.h"
#include "fsl_debug_console.h"
#include "booster.h"
#include "led.h"
#include "timer.h"
#include "NordicCmdPackage.h"

#define BUFFER_MAX_LEN			264

uint8_t  spi_tx_buffer[BUFFER_MAX_LEN]={0};
uint8_t  spi_rx_buffer[BUFFER_MAX_LEN]={0};
spi_buffer_t spi_buffer_temp;

extern uint8_t fireware_version[2];

typedef struct
{
	uint8_t cmd;
	uint16_t len;
	uint8_t * buf;
} spi_cmd_t;

spi_cmd_t spi_cmd;

void printf_buf(uint8_t * buf, uint32_t len)
{
	for(int i=0; i<len; i++)
	{
		PRINTF("buf[%d]:0x%x\r\n", i, buf[i]);
	}
}

void buffer_clear(uint8_t * buffer, uint32_t len)
{
	int i;
	
	for(i=0; i<len; i++)
	{
		buffer[i] = 0;
	}
}

int  spi_check_resopne()
{
	int i=0;
	
	SPI_Type * spiBaseAddr = g_spiBase[SPI_MASTER_INSTANCE];
	
	while(SPI_INT_FLAG != 1)
	{
		SCD_DelayMs(1);
		if (i++ == 1000)
		{
			return -1;
		}
	}	
	SPI_INT_FLAG = 0;
	SPI_ReadData(spiBaseAddr, spi_rx_buffer, CMD_LEN_8BYTE);
	SCD_DelayMs(1);  
	
	if ((spi_rx_buffer[0] == 0xFF) && (spi_rx_buffer[6] == 0x0A) && (spi_rx_buffer[7] == 0x0D))
	{
		if ((spi_rx_buffer[1] == RESP_DATA_CMD) && (spi_rx_buffer[3] == RESP_DATA_2BYTE))
		{
			if (spi_rx_buffer[4] == RESP_DATA_CMD)
			{
				if (spi_rx_buffer[5] == RESP_SUCCESS)
				{
					return RESP_SUCCESS;
				}
				else if (spi_rx_buffer[5] == RESP_FAIL)
				{
					return RESP_FAIL;
				}
				else if (spi_rx_buffer[5] == RESP_INVALID)
				{
					return RESP_INVALID;
				}
				else
				{
					return -1;
				}
			}
		} 
		else 
		{
			return -1;
		}
	}
	else 
	{
		return -1;
	}
	return 0;
}

uint8_t  spi_check_cmd(uint32_t len)
{
	SPI_Type * spiBaseAddr = g_spiBase[SPI_MASTER_INSTANCE];
	uint8_t buf[8] = {0};
	
	buf[0]  =  0xFF;
	buf[1]  =  RESP_DATA_CMD;
	buf[2]  =  0;
	buf[3]  =  RESP_DATA_2BYTE;
	buf[4]  =  0;	
	buf[5]  =  0;
	buf[6]  =  0x0A;
	buf[7]	=  0x0D;
	
	spi_cmd.cmd = spi_rx_buffer[1];
	spi_cmd.len = (spi_rx_buffer[2] << 8) | spi_rx_buffer[3];
	spi_cmd.buf = &spi_rx_buffer[4];
	
	if ((spi_rx_buffer[0] == 0xFF) && (spi_rx_buffer[len-2] == 0x0A) && (spi_rx_buffer[len-1] == 0x0D))
	{
		buf[4] = spi_rx_buffer[1];
		buf[5] = RESP_SUCCESS;
		SPI_WriteData(spiBaseAddr, buf, buf, RESP_LEN_8BYTE);
		return RESP_SUCCESS;
	} else {
		buf[4] = spi_rx_buffer[1];
		buf[5] = RESP_FAIL;	
		SPI_WriteData(spiBaseAddr, buf, buf, RESP_LEN_8BYTE);
		return RESP_FAIL;
	}
}

void SPI_Set_Len(uint16_t len)
{
	SPI_Type * spiBaseAddr = g_spiBase[SPI_MASTER_INSTANCE];
	uint8_t buf[8] = {0};
	
	buf[0]  =  0xFF;
	buf[1]  =  SET_DATA_LEN_CMD;
	buf[2]  =  0;
	buf[3]  =  LEN_DATA_2BYTE;
	buf[4]  =  len >> 8;	
	buf[5]  =  len & 0xFF;
	buf[6]  =  0x0A;
	buf[7]	=  0x0D;
	
	SPI_WriteData(spiBaseAddr, buf, buf, CMD_LEN_8BYTE);
	//SCD_DelayMs(100);  
}

void S7816_Init(void)
{
	u1 bufatr[258],len; 
	
	SCD_POWER_ON(bufatr,&len,0,0);
}

void S7816_Deinit(void)
{
	SCD_POWER_OFF(); 
}

uint8_t Process_7816(void)
{
	uint8_t status;
	u1 sta = 1;
	uint32_t i = 0;
	uint16_t len, len2, all_len;
	SPI_Type * spiBaseAddr = g_spiBase[SPI_MASTER_INSTANCE];
	
	//Clear Buffer
	buffer_clear(spi_tx_buffer, BUFFER_MAX_LEN);
	buffer_clear(spi_rx_buffer, BUFFER_MAX_LEN);

	//read cmd len
	SPI_ReadData(spiBaseAddr, spi_rx_buffer, CMD_LEN_8BYTE);
	SCD_DelayMs(1);
	status = spi_check_cmd(CMD_LEN_8BYTE);
	if (status == RESP_FAIL)
	{
		PRINTF("RESP_FAIL 1\r\n");
		return RESP_FAIL; 
	}

	len = (spi_rx_buffer[4] << 8) | spi_rx_buffer[5];

	//clear buffer
	buffer_clear(spi_rx_buffer, BUFFER_MAX_LEN);
	
	//read cmd
	while(SPI_INT_FLAG != 1)
	{
		SCD_DelayMs(1);
		if (i++ == 1000)
		{

			return RESP_FAIL;
		}
	}
	SPI_INT_FLAG = 0;
	SPI_ReadData(spiBaseAddr, spi_rx_buffer, len+6);
	SCD_DelayMs(1);  
	status = spi_check_cmd(len+6);
	if (status == RESP_SUCCESS)
	{

		//7816 send cmd
		if (NFC_INT_FLAG == 0)
		{
			sta = SCD_APDU_LYCMD((u1*)spi_cmd.buf, spi_cmd.len, (u1*)&spi_tx_buffer[4], (u2*)&len2);
		}
		if (!sta)
		{
			//set len
			all_len = len2+6;
			SPI_Set_Len(all_len);
			SCD_DelayMs(1);
			if (spi_check_resopne() != RESP_SUCCESS)
			{
				PRINTF("RESP FAIL 2\r\n");
				return RESP_FAIL;
			}
			//send data

			spi_tx_buffer[0] = 0xFF;
			spi_tx_buffer[1] = TRAN_DATA_CMD;
			spi_tx_buffer[2] = (len2) >> 8;
			spi_tx_buffer[3] = (len2)& 0xFF;
			spi_tx_buffer[len2+4] = 0x0A;
			spi_tx_buffer[len2+5] = 0x0D;

			SPI_WriteData(spiBaseAddr, spi_tx_buffer, spi_tx_buffer, all_len);
			SCD_DelayMs(1);
			if (spi_check_resopne() != RESP_SUCCESS)
			{
				PRINTF("RESP FAIL 3\r\n");
				return RESP_FAIL;	
			}
			//printf_buf(spi_tx_buffer, all_len);
		} else {
			//set len
			len2 = 6;
			all_len = len2+6;
			SPI_Set_Len(all_len);
			SCD_DelayMs(1);
			if (spi_check_resopne() != RESP_SUCCESS)
			{
				PRINTF("RESP FAIL 2\r\n");
				return RESP_FAIL;
			}
			//send data

			spi_tx_buffer[0] = 0xFF;
			spi_tx_buffer[1] = TRAN_DATA_CMD;
			spi_tx_buffer[2] = (len2) >> 8;
			spi_tx_buffer[3] = (len2)& 0xFF;
			spi_tx_buffer[4] = 0x0;
			spi_tx_buffer[5] = 0x0;
			spi_tx_buffer[6] = 0x0;
			spi_tx_buffer[7] = 0x0;
			spi_tx_buffer[8] = 0x0;
			spi_tx_buffer[9] = 0x0;
			spi_tx_buffer[len2+4] = 0x0A;
			spi_tx_buffer[len2+5] = 0x0D;

			SPI_WriteData(spiBaseAddr, spi_tx_buffer, spi_tx_buffer, all_len);
			SCD_DelayMs(1);
			if (spi_check_resopne() != RESP_SUCCESS)
			{
				PRINTF("RESP FAIL 3\r\n");
				return RESP_FAIL;	
			}
		}
	} else {
		PRINTF("RESP FAIL 4\r\n");
		return RESP_FAIL;
	}
	
	return RESP_SUCCESS;
}


uint8_t Update_balances(void)
{
	uint8_t buf[256]={0}; 
	uint32_t len=0; 
	uint32_t len2=0; 
	SPI_Type * spiBaseAddr = g_spiBase[SPI_MASTER_INSTANCE];

	//Clear Buffer
	buffer_clear(spi_tx_buffer, BUFFER_MAX_LEN);
	buffer_clear(spi_rx_buffer, BUFFER_MAX_LEN);

	//7816 send cmd
	if (NFC_INT_FLAG == 0)
		SCD_APDU_CMD((u1 *)"\x00\xA4\x04\x00\x10\xD1\x56\x00\x00\x40\x10\x00\x30\x00\x00\x00\x02\x00\x00\x00\x00",21,buf,&len);	
	else 
		return RESP_FAIL;
	
	if (NFC_INT_FLAG == 0)
		SCD_APDU_CMD((u1 *)"\x80\x5C\x00\x02\x04",5,(u1*)&spi_tx_buffer[4],&len);	
	else
		return RESP_FAIL;

	spi_tx_buffer[0] = 0xFF;
	spi_tx_buffer[1] = TRAN_BALANCES_CMD;
	spi_tx_buffer[2] = (len) >> 8;
	spi_tx_buffer[3] = (len)& 0xFF;
	spi_tx_buffer[len+4] = 0x0A;
	spi_tx_buffer[len+5] = 0x0D;
	
	len2 = len + 6;
	SPI_WriteData(spiBaseAddr, spi_tx_buffer, spi_tx_buffer, len2);
	SCD_DelayMs(1);
	return RESP_SUCCESS;	
}

//IN_OUT = 0, inside Field
//IN_OUT = 1, outside Field
uint8_t Update_NFC_Status(uint8_t IN_OUT)
{
	uint32_t len=6; 
	uint32_t len2=0; 
	SPI_Type * spiBaseAddr = g_spiBase[SPI_MASTER_INSTANCE];

	//Clear Buffer
	buffer_clear(spi_tx_buffer, BUFFER_MAX_LEN);
	buffer_clear(spi_rx_buffer, BUFFER_MAX_LEN);

	SPI_INT_CFG_OUTPUT();
	SPI_INT_CFG_OUTPUT_LOW();
	SCD_DelayMs(1);
	SPI_INT_CFG_OUTPUT_HIGH();
	SPI_INT_CFG_INPUT();

	if (IN_OUT == 0)
	{		
		spi_tx_buffer[0] = 0xFF;
		spi_tx_buffer[1] = TRAN_STATUS_CMD;
		spi_tx_buffer[2] = (len) >> 8;
		spi_tx_buffer[3] = (len)& 0xFF;
		spi_tx_buffer[4] = fireware_version[0];
		spi_tx_buffer[5] = fireware_version[1];	//version
		spi_tx_buffer[6] = 0x00;
		spi_tx_buffer[7] = 0x01;	//NFC Field inside
		spi_tx_buffer[8] = 0x00;
		spi_tx_buffer[9] = 0x01;	//Keep Balances
		spi_tx_buffer[len+4] = 0x0A;
		spi_tx_buffer[len+5] = 0x0D;
	} 
	else if (IN_OUT == 1)
	{
		spi_tx_buffer[0] = 0xFF;
		spi_tx_buffer[1] = TRAN_STATUS_CMD;
		spi_tx_buffer[2] = (len) >> 8;
		spi_tx_buffer[3] = (len)& 0xFF;
		spi_tx_buffer[4] = fireware_version[0];
		spi_tx_buffer[5] = fireware_version[1];	//version
		spi_tx_buffer[6] = 0x00;
		spi_tx_buffer[7] = 0x02;	//NFC Field outside
		spi_tx_buffer[8] = 0x00;
		spi_tx_buffer[9] = 0x02;	//Update Balances
		spi_tx_buffer[len+4] = 0x0A;
		spi_tx_buffer[len+5] = 0x0D;	
	}
	
	len2 = len + 6;
	SPI_WriteData(spiBaseAddr, spi_tx_buffer, spi_tx_buffer, len2);
	SCD_DelayMs(1);
	

	return RESP_SUCCESS;	
}


void notify_nfc_change(uint8_t event) {
	
	// Prepare response header
	((spi_command_t*) spi_buffer_temp.buf_command_response)->header.tags = 0xFF;
	((spi_command_t*) spi_buffer_temp.buf_command_response)->header.type = SE_NOTIFY_EVENT;
	((spi_command_t*) spi_buffer_temp.buf_command_response)->header.len = 1;

	// Send
	spi_buffer_temp.buf_command_response_write_idx = 0;
	spi_buffer_temp.buf_command_response_size = SPI_HEADER_COMMAND_SIZE + 1;

	//msg
	memcpy( &((spi_command_t*) spi_buffer_temp.buf_command_response)->payload[0], &event, 1);
	
	SPI_INT_CFG_OUTPUT();
	SPI_INT_CFG_OUTPUT_LOW();
	SCD_DelayMs(1);
	SPI_INT_CFG_OUTPUT_HIGH();
	SPI_INT_CFG_INPUT();
	
	// If no ongoing transmission
	if((m_spis.io.mode & SPI_IO_TX_MODE) == 0) {
		spi_send_packets(spi_buffer_temp.buf_command_response,  spi_buffer_temp.buf_command_response_size);
	}

}

uint8_t Spi_Send_MSG(uint8_t cmd, uint8_t rsp, uint8_t* data, uint16_t len) {
	
	m_spis.io.mode &= ~SPI_IO_PROCESS_MODE;
	m_spis.io.mode |= SPI_IO_RX_CMD_MODE;
	uint8_t Sdata[2]={0x00};
	if(rsp>=CCID_STATUS)
	{
		Sdata[0] = m_spi_ctx.status;
		Sdata[1] = 0x00;
		if(rsp >CCID_STATUS)
			Sdata[1] = rsp|CCID_SLOT_STATUS_FAILED;	
		
		rsp = SPI_RSP_STATUS;
		data = Sdata;
		len=2;
	}

	// Header
	((spi_command_t*) spi_buffer_temp.buf_command_response)->header.tags = 0xFF;
	((spi_command_t*) spi_buffer_temp.buf_command_response)->header.type = cmd;
	((spi_command_t*) spi_buffer_temp.buf_command_response)->header.len = len+1;
	
	//msg
	memcpy( &((spi_command_t*) spi_buffer_temp.buf_command_response)->payload[0], &rsp, 1);
	if(len>0)
		memcpy( &((spi_command_t*) spi_buffer_temp.buf_command_response)->payload[1], data, len);
	
	spi_buffer_temp.buf_command_response_write_idx = 0;
	spi_buffer_temp.buf_command_response_size = SPI_HEADER_COMMAND_SIZE + len+1;

	if((m_spis.io.mode & SPI_IO_TX_MODE) == 0) {
		spi_send_packets(spi_buffer_temp.buf_command_response, spi_buffer_temp.buf_command_response_size);
	}	
	return F_RSP_OK;
}


void switch_idle_mode(void)
{
	BoosterIrqClear();
	#if EMV_TEST
	BoosterIdleMode();
	#endif
	NFC_INT_FLAG = 0;
	booster_state.nfc_state = NFC_INACTIVE;
	timer_stop();
}

void process_spi_command(uint8_t* p_data, uint16_t p_len) 
{
	spi_command_header_t* header;
	memcpy(&spi_buffer_temp.buf_command_response[0], p_data, p_len);
	header = (spi_command_header_t*)&spi_buffer_temp.buf_command_response[0];

	#if DEBUG
		uint32_t i = 0;
		PRINTF("process_ccid_command: \r\n");
		for(i=0; i<p_len; i++) {
			PRINTF("0x%x  ",p_data[i]);
		}
		PRINTF("\r\n");
	#endif
	
	NordicDataParseHandler(header->type,&((spi_command_t*) spi_buffer_temp.buf_command_response)->payload[0],header->len);
}


