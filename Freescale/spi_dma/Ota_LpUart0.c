
#include "Ota_LpUart0.h"
#include "fsl_lpuart_driver.h"
#include "fsl_debug_console.h"
#include <string.h>
#include "Ui.h"
#include "pin_mux.h"

#define DATA_REV_MAX  8
uint8_t DataRevBuf[DATA_REV_MAX]={0x00};
uint8_t DataRevIndex =0;
bool Otabegin =false;
static const OTA_DATA_CMD_T OtaDataCmdTable[]={
	{0x01,OtaProgress},
	{0x02,OtaResult},		
};
const uint16_t OtaDataCmdTableLen=sizeof(OtaDataCmdTable)/sizeof(OTA_DATA_CMD_T);

void LpUart0ParseHandler(uint8_t c)
{
	if(c==0xFF)
	{
		Otabegin =true;
		DataRevIndex =0;
	}
	if(Otabegin)
	{
		DataRevBuf[DataRevIndex++]=c;
		if(DataRevIndex>=DATA_REV_MAX)
		{
			memset(DataRevBuf,0,DATA_REV_MAX);
			DataRevIndex=0;
			Otabegin = false;
			return;
		}
		if(c==0x0d && DataRevIndex==5)
		{
			if(DataRevBuf[3]==0x0a)
			{
				for(uint8_t i=0;i<OtaDataCmdTableLen;i++)	
				{
					if(DataRevBuf[1]==OtaDataCmdTable[i].OtaCmd)
					{
						OtaDataCmdTable[i].pFn(DataRevBuf[2]);
						memset(DataRevBuf,0,DATA_REV_MAX);
						DataRevIndex=0;
						Otabegin = false;
						return;
					}
				}			
			}
		}	
	}
}

void OtaProgress(uint8_t Pro)
{
	PRINTF("OtaProgress--%d--\r\n",Pro);
	ShowOtaLogoProgress(Pro);
}

void OtaResult(uint8_t Res)
{
	PRINTF("OtaResult--%d--\r\n",Res);
	ShowOtaResult(Res);
}

void Enable_LpUart0_IRQ(void)
{
	SetUart0Pins(1);
	LPUART_HAL_SetIntMode(LPUART0, kLpuartIntRxOverrun, true);
	LPUART_BWR_CTRL_RIE(LPUART0, 1U);
}

void Disable_LpUart0_IRQ(void)
{
	LPUART_BWR_CTRL_RIE(LPUART0, 0U);
	LPUART_HAL_SetIntMode(LPUART0, kLpuartIntRxOverrun, false);
	SetUart0Pins(0);
}


void Ota_LpUart0_Init(void)
{
	lpuart_state_t lpuartStatePtr;
	lpuart_user_config_t lpuartConfig = {
		.clockSource     = kClockLpuartSrcIrc48M,
		.bitCountPerChar = kLpuart8BitsPerChar,
		.parityMode      = kLpuartParityDisabled,
		.stopBitCount    = kLpuartOneStopBit,
		.baudRate        = 115200
	};
	LPUART_DRV_Init(0, &lpuartStatePtr, &lpuartConfig);
	Enable_LpUart0_IRQ();
}

void LPUART0_IRQHandler(void)
{
	uint8_t rxChar =0x00;
	if((LPUART_BRD_CTRL_RIE(LPUART0)) && (LPUART_BRD_STAT_RDRF(LPUART0)))
	{
		LPUART_HAL_Getchar(LPUART0, &rxChar);
		LpUart0ParseHandler(rxChar);
	}  
	if (LPUART_HAL_GetStatusFlag(LPUART0, kLpuartRxOverrun))
		LPUART_HAL_ClearStatusFlag(LPUART0, kLpuartRxOverrun);
}



