/*
 * Copyright (c) 2014, Freescale Semiconductor, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of Freescale Semiconductor, Inc. nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "board.h"
#include "timer.h"
#include "spi.h"
#include "7816.h"
#include "fsl_debug_console.h"
#include "7816_test.h"
#include "spi_test.h"
#include "power.h"
#include "booster.h"
#include "led.h"
#include "fsl_pit_driver.h"
#include "fsl_spi_dma.h"
#include "seeed_lcd.h"
#include "fsl_dmamux.h"
#include "ExtSpiFlash.h"
#include "tpm_timer.h"
#include "Ui.h"
#include "display_font.h"
#include "Ota_LpUart0.h"

uint8_t count_flag = 0;
uint8_t mutex_flag = 0;
uint8_t NFC_CL_FLAG = 0;
uint8_t fireware_version[2]={0,4};

extern uint8_t nfc_falag;

spi_dma_handle_t master1Handle = {0};
dma_handle_t master1TxHandle = {0};
dma_handle_t master1RxHandle = {0};
dma_handle_t memHandle= {0};
volatile bool master1Finished = false;
volatile bool memFinished = false;
/****************************DMA**************************************/
#define EXAMPLE_DMA DMA0
#define EXAMPLE_SPI1_MASTER SPI1
#define EXAMPLE_DMAMUX DMAMUX0
#define EXAMPLE_MEM_CHANNEL 0U
#define EXAMPLE_SPI1_MASTER_TX_CHANNEL 2U
#define EXAMPLE_SPI1_MASTER_RX_CHANNEL 3U
#define EXAMPLE_DMA_MEM_SOURCE 			    63U
#define EXAMPLE_DMA_SPI1_MASTER_TX_SOURCE 19U
#define EXAMPLE_DMA_SPI1_MASTER_RX_SOURCE 18U
#define EXAMPLE_SPI1_MASTER_SOURCE_CLOCK kCLOCK_BusClk
uint8_t volatile resgister_SPI1_S=0;
uint8_t volatile resgister_SPI1_BR=0;
uint8_t volatile resgister_SPI1_C2=0;
uint8_t volatile resgister_SPI1_C1=0;
uint8_t volatile resgister_SPI1_C3=0;
uint8_t volatile resgister_SPI1_MH=0;
uint8_t volatile resgister_SPI1_DL=0;
uint8_t volatile resgister_SPI1_DH=0;

void mem_Callback(dma_handle_t *handle, void *param)
{
	memFinished = true;
}
void MemDmaTransfer(uint8_t* dest ,uint8_t* src, uint32_t transferBytes)
{
	dma_transfer_config_t transferConfig;
	DMA_PrepareTransfer(&transferConfig, src, sizeof(src[0]), dest, sizeof(dest[0]), transferBytes,kDMA_MemoryToMemory);
	DMA_SubmitTransfer(&memHandle, &transferConfig, kDMA_EnableInterrupt);
	DMA_StartTransfer(&memHandle);
}

static void master1Callback(SPI_Type *base, spi_dma_handle_t *handle, status_t status, void *userData)
{
	resgister_SPI1_S = SPI1->S;
	resgister_SPI1_BR= SPI1->BR;
	resgister_SPI1_C2= SPI1->C2;
	resgister_SPI1_C1= SPI1->C1;
	resgister_SPI1_C3= SPI1->C3;
	resgister_SPI1_MH= SPI1->MH;
	resgister_SPI1_DL= SPI1->DL;
	resgister_SPI1_DH= SPI1->DH;
	
	if(CleanDisplayMode)
	{
		Release_Flash_CS(); 
		CleanDisplayMode = false;
		Dma_DataParse();
		NeedFillData=true;
	}
	else
	{
		master1Finished = true;
	}
}
static void InitDMA(void)
{
	spi_master_config_t master1Config = {0};
	uint32_t sourceClock = 0;
	/* Init DMAMUX */
       DMAMUX_Init(EXAMPLE_DMAMUX);
	DMAMUX_SetSource(EXAMPLE_DMAMUX, EXAMPLE_SPI1_MASTER_TX_CHANNEL, EXAMPLE_DMA_SPI1_MASTER_TX_SOURCE);
	DMAMUX_SetSource(EXAMPLE_DMAMUX, EXAMPLE_SPI1_MASTER_RX_CHANNEL, EXAMPLE_DMA_SPI1_MASTER_RX_SOURCE);
	DMAMUX_SetSource(EXAMPLE_DMAMUX, EXAMPLE_MEM_CHANNEL, EXAMPLE_DMA_MEM_SOURCE);
	DMAMUX_EnableChannel(EXAMPLE_DMAMUX, EXAMPLE_SPI1_MASTER_TX_CHANNEL);
	DMAMUX_EnableChannel(EXAMPLE_DMAMUX, EXAMPLE_SPI1_MASTER_RX_CHANNEL);
	DMAMUX_EnableChannel(EXAMPLE_DMAMUX, EXAMPLE_MEM_CHANNEL);
	/* Init the DMA module */
	DMA_Init(EXAMPLE_DMA);
	DMA_CreateHandle(&master1TxHandle, EXAMPLE_DMA, EXAMPLE_SPI1_MASTER_TX_CHANNEL);
	DMA_CreateHandle(&master1RxHandle, EXAMPLE_DMA, EXAMPLE_SPI1_MASTER_RX_CHANNEL);
	DMA_CreateHandle(&memHandle, EXAMPLE_DMA, EXAMPLE_MEM_CHANNEL);

	DMA_SetCallback(&memHandle, mem_Callback, NULL);
	
	SPI_MasterGetDefaultConfig(&master1Config);
	master1Config.baudRate_Bps = 24000000U; //5000000U;     // SPI1 max baudrate up to 24Mbps
	sourceClock = CLOCK_GetFreq(EXAMPLE_SPI1_MASTER_SOURCE_CLOCK);
	SPI_MasterInit(EXAMPLE_SPI1_MASTER, &master1Config, sourceClock);
	/* Create handle for SPI master */
	SPI_MasterTransferCreateHandleDMA(EXAMPLE_SPI1_MASTER, &master1Handle, master1Callback, NULL, &master1TxHandle,
 										&master1RxHandle);	
}

void SuspendDMA(void)
{
	configure_spi_pins(SPI1_INPUT);
	FlashDisable=1;
}
void ReumeDMA(void)
{
	configure_spi_pins(SPI1_IDX);
	FlashDisable=0;
}

/*********************************************************************/

void RollTimerInit(void)
{
	pit_user_config_t chn0Confg = {
		.isInterruptEnabled = true,
		.periodUs = DISPLAY_ROLL_TIME
	};	
	PIT_DRV_Init(BOARD_PIT_INSTANCE, false);
       PIT_DRV_InitChannel(BOARD_PIT_INSTANCE, 0, &chn0Confg);	
	DisplayRoll =0;   
}


int main(void)
{
	tpm_init();
	hardware_init();	
	PRINTF("KL17 main\r\n");
	
	//init  LpUart0
	Ota_LpUart0_Init();
	OtaRest=0;
	//init  SPI0
	SPI_Init();	
	SPI_INT_CFG_INPUT();	
	
	//init  SPI1 DMA
	InitDMA();
	SpiFlash_Init();
	Seeed_lcd_init();
	ClearFullScreen();
	SuspendDMA();

		
	RollTimerInit();
	//LED_INIT();   
	//init booster
	Booster_Init();
	BoosterIdleMode();
	BoosterIrqDisable();
	booster_state.block = NFC_UNBLOCK_STATE;

	timer2_init();
	
	#if (EMV_TEST == 0)
	BoosterIrqEnable();
	BoosterIrqClear();
	BoosterContactlessMode();
	booster_state.nfc_state = NFC_ACTIVE_AND_INIT;
	#endif
	
	while(1)
	{
		if (m_spi_ctx.rx_done)
		{
			PRINTF("rx done\r\n");
			m_spi_ctx.rx_done = false;
			if((m_spis.io.mode & SPI_IO_RX_CMD_MODE) > 0) {
				if((((spi_command_t*) spi_buffer.buf_command_response)->header.len + 4) >= sizeof(spi_command_header_t)) {
					process_spi_command(&spi_buffer.buf_command_response[0], (((spi_command_t*) spi_buffer.buf_command_response)->header.len + 4));
					m_spi_ctx.first_cmd = 0;
				}
			}
		}
			
		if(DisplayRoll>=DisplayRollPeriod)
		{
			DisplayLineRoll();
			DisplayRoll=0;
		}


		if (DynDisplay)
		{
			DynDisplay = 0x00;
			if (TimerFlag)
			{
				TimerFlag = 0x00;
				if(FlashDisable==0)
					ShowPaymentAnimation1();
			}
			else
			{
				TimerFlag = 0xFF;
				if(FlashDisable==0)
					ShowPaymentAnimation2();
			}
			
			
		}
		
		if ((booster_state.nfc_state == NFC_ACTIVE_AND_UNINIT) && (booster_state.mode == BOOSTER_IDLE_MODE)) {
			#if EMV_TEST
			BoosterIrqEnable();
			BoosterIrqClear();
			BoosterContactlessMode();
			booster_state.nfc_state = NFC_ACTIVE_AND_INIT;
			#endif
		}

		//MCU enter Low Power Mode
		if ((booster_state.mode == BOOSTER_IDLE_MODE) && (power_state == POWER_SUSPEND)) {		
			power_config(kDemoVlps);
		}
		
		//contactless interface
		if ((NFC_UPDATE_FLAG == 1) && (mutex_flag == 0))
		{
			PRINTF("outside\r\n");
			mutex_flag = 1;
			notify_nfc_change(NFC_NOTIFY_EVENT_OUT);
			NFC_UPDATE_FLAG = 0;
			mutex_flag = 0;	
			NFC_CL_FLAG = 0; 
		}
		if ((NFC_Field_FLAG == 1) && (mutex_flag == 0))
		{
			PRINTF("inside\r\n");
			timer2_stop();
			NFC_CL_FLAG = 1; 
			mutex_flag = 1;	
			notify_nfc_change(NFC_NOTIFY_EVENT_IN);
			NFC_Field_FLAG = 0;
			mutex_flag = 0;
		}	
		__asm("nop");
	}
}



