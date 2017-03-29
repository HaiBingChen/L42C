#include "booster.h"
#include "spi.h"
#include "fsl_delay.h"

booster_state_t booster_state;


static void booster_gpio_init()
{
    // Define gpio output pin config structure POWER AND RESET.
    gpio_output_pin_user_config_t outputPin[] = {
        {
            .pinName              = BOOSTER_CS,
            .config.outputLogic   = 0,
#if FSL_FEATURE_PORT_HAS_SLEW_RATE
            .config.slewRate      = kPortFastSlewRate,
#endif
#if FSL_FEATURE_PORT_HAS_DRIVE_STRENGTH
            .config.driveStrength = kPortHighDriveStrength,
#endif
        },
        {
            .pinName = GPIO_PINS_OUT_OF_RANGE,
        }
    };

    // Init SPI CS
    GPIO_DRV_Init(NULL, outputPin);

    GPIO_DRV_SetPinOutput(BOOSTER_CS);
}


void Booster_Init(void)
{
	// set CS pin as GPIO
	booster_gpio_init();	

	/*

	SPI_Type * spiBaseAddr = g_spiBase[SPI_BOOSTER_INSTANCE];
	uint32_t spiSourceClock;

	// set CS pin as GPIO
	booster_gpio_init();	
	
	// Enable clock for SPI
	CLOCK_SYS_EnableSpiClock(SPI_BOOSTER_INSTANCE);

	// configure the run-time state struct with the source clock value
	spiSourceClock = CLOCK_SYS_GetSpiFreq(SPI_BOOSTER_INSTANCE);

	// Reset the SPI module to it's default state, which includes SPI disabled
	SPI_HAL_Init(spiBaseAddr);

	// Set SPI to master mode
	SPI_HAL_SetMasterSlave(spiBaseAddr, kSpiMaster);

	// Set slave select to automatic output mode
	SPI_HAL_SetSlaveSelectOutputMode(spiBaseAddr, kSpiSlaveSelect_AutomaticOutput);

	// Set the SPI pin mode to normal mode
	SPI_HAL_SetPinMode(spiBaseAddr, kSpiPinMode_Normal);

#if FSL_FEATURE_SPI_FIFO_SIZE
	if (g_spiFifoSize[SPI_BOOSTER_INSTANCE] != 0)
	{
		// If SPI module contains a FIFO, disable it and set watermarks to half full/empty
		SPI_HAL_SetFifoMode(spiBaseAddr, false, kSpiTxFifoOneHalfEmpty, kSpiRxFifoOneHalfFull);
	}
#endif

	//USER CONFIGURABLE OPTION FOR 8 or 16-BIT MODE (if applicable)
#if FSL_FEATURE_SPI_16BIT_TRANSFERS
	SPI_HAL_Set8or16BitMode(spiBaseAddr, kSpi8BitMode);
#endif
	// SPI system Enable
	SPI_HAL_Enable(spiBaseAddr);

	// Configure the bus to access the provided device.
	SPI_HAL_SetBaud(spiBaseAddr, BOOSTER_TRANSFER_BAUDRATE, spiSourceClock);
	// Setup format as same as slave
	SPI_HAL_SetDataFormat(spiBaseAddr, kSpiClockPolarity_ActiveLow, kSpiClockPhase_FirstEdge, kSpiMsbFirst);	
*/

	//PRINTF("Booster SPI0 Init Done\r\n");
	
}


void Booster_SPI_Resume(void)
{
	SPI_Type * spiBaseAddr = g_spiBase[SPI_BOOSTER_INSTANCE];
	uint32_t spiSourceClock;

	// Configure the bus to access the provided device.
	SPI_HAL_SetBaud(spiBaseAddr, BOOSTER_TRANSFER_BAUDRATE, spiSourceClock);
	// Setup format as same as slave
	SPI_HAL_SetDataFormat(spiBaseAddr, kSpiClockPolarity_ActiveLow, kSpiClockPhase_FirstEdge, kSpiMsbFirst);	
}

void Booster_SPI_Sleep(void)
{
	SPI_Type * spiBaseAddr = g_spiBase[SPI_BOOSTER_INSTANCE];
	uint32_t spiSourceClock;

	// Configure the bus to access the provided device.
	SPI_HAL_SetBaud(spiBaseAddr, BOOSTER_TRANSFER_HIGH_BAUDRATE, spiSourceClock);
	// Setup format as same as slave
	SPI_HAL_SetDataFormat(spiBaseAddr, kSpiClockPolarity_ActiveHigh, kSpiClockPhase_FirstEdge, kSpiMsbFirst);	

}

void Booster_WriteData( uint8_t * Buffer,  uint32_t SIZE)
{
	volatile uint8_t dummy_data = 0;
	SPI_Type * spiBaseAddr = g_spiBase[SPI_BOOSTER_INSTANCE];
	int i;
	
	// Disable module to clear the shift register
	SPI_HAL_Disable(spiBaseAddr);
	SPI_HAL_Enable(spiBaseAddr);

	BOOSTER_CS_LOW();
	
	for (i = 0; i < SIZE; i++)
	{
#if FSL_FEATURE_SPI_16BIT_TRANSFERS
		SPI_HAL_WriteDataBlocking(spiBaseAddr, kSpi8BitMode, 0, Buffer[i]);
#else
		// Write data to slave
		SPI_HAL_WriteDataBlocking(spiBaseAddr, Buffer[i]);
#endif
		// Wait for slave send data back
		while(SPI_HAL_IsReadBuffFullPending(spiBaseAddr)==0){}
			
		// Read data register
#if FSL_FEATURE_SPI_16BIT_TRANSFERS
		dummy_data = SPI_HAL_ReadDataLow(spiBaseAddr);
#else
		dummy_data = SPI_HAL_ReadData(spiBaseAddr);
#endif
	}
	
	BOOSTER_CS_HIGH();	
}

void Booster_WriteReadData( uint8_t * Txbuf, uint8_t * Rxbuf,  uint32_t SIZE)
{
	int i;
	SPI_Type * spiBaseAddr = g_spiBase[SPI_BOOSTER_INSTANCE];
	

	// Disable module to clear the shift register
	SPI_HAL_Disable(spiBaseAddr);
	SPI_HAL_Enable(spiBaseAddr);

	BOOSTER_CS_LOW();
	for (i = 0; i < SIZE; i++)
	{
#if FSL_FEATURE_SPI_16BIT_TRANSFERS
		// Send dummy byte to receive data from slave
		SPI_HAL_WriteDataBlocking(spiBaseAddr, kSpi8BitMode, 0, Txbuf[i]);
#else
		// Send dummy byte to receive data from slave
		SPI_HAL_WriteDataBlocking(spiBaseAddr, Txbuf[i]);
#endif
		// Wait for slave send data back
		while(SPI_HAL_IsReadBuffFullPending(spiBaseAddr)==0){}
			
		// Read data register
#if FSL_FEATURE_SPI_16BIT_TRANSFERS
		Rxbuf[i] = SPI_HAL_ReadDataLow(spiBaseAddr);
#else
		Rxbuf[i] = SPI_HAL_ReadData(spiBaseAddr);
#endif

	}
	BOOSTER_CS_HIGH();

}

void BoosterIdleMode(void)
{
	uint8_t buf[2]={0};

	Booster_SPI_Resume();

	buf[0] = 0x02;
	buf[1] = 0x00;
	Booster_WriteData(buf,  2);
	booster_state.mode = BOOSTER_IDLE_MODE;

	Booster_SPI_Sleep();
}

void BoosterContactMode(void)
{
	uint8_t buf[2]={0};

	Booster_SPI_Resume();
	
	buf[0] = 0x02;
	buf[1] = 0x40;
	Booster_WriteData(buf,  2);
	booster_state.mode = BOOSTER_CONTACT_MODE;

	Booster_SPI_Sleep();
}

void BoosterContactlessMode(void)
{
	uint8_t buf[2]={0};
	
	Booster_SPI_Resume();

	buf[0] = 0x02;
	buf[1] = 0x80;
	Booster_WriteData(buf,  2);
	booster_state.mode = BOOSTER_CONTACTLESS_OF_MODE;

	Booster_SPI_Sleep();

}

void BoosterIrqEnable(void)
{
	uint8_t buf[2]={0};

	Booster_SPI_Resume();

	buf[0] = 0x0E;
	buf[1] = 0x70;
	Booster_WriteData(buf,  2);	
	
	Booster_SPI_Sleep();

}

void BoosterIrqDisable(void)
{
	uint8_t buf[2]={0};

	Booster_SPI_Resume();

	buf[0] = 0x0E;
	buf[1] = 0xF0;
	Booster_WriteData(buf,  2);

	Booster_SPI_Sleep();

}

void BoosterIrqClear(void)
{
	uint8_t buf[2]={0};

	Booster_SPI_Resume();

	buf[0] = 0x30;
	buf[1] = 0xFF;
	Booster_WriteData(buf,  2);

	Booster_SPI_Sleep();

}

uint8_t BoosterReadStatus(void)
{
	uint8_t buf[2]={0};
	uint8_t status[2] = {0};

	Booster_SPI_Resume();

	buf[0] = 0x31;
	buf[1] = 0x00;
	Booster_WriteReadData(&buf[0],status, 2);

	Booster_SPI_Sleep();

	return status[1];
}

void BoosterSetting(void)
{
	uint8_t buf[6]={0};

	Booster_SPI_Resume();

	buf[0] = 0x40;
	buf[1] = 0x00;
	buf[2] = 0x80;
	buf[3] = 0x00;
	buf[4] = 0x80;
	buf[5] = 0x02;
	Booster_WriteData(buf,  6);

	fsl_delay_ms(10);
	buf[0] = 0x40;
	buf[1] = 0x08;
	buf[2] = 0x00;
	buf[3] = 0x20;
	buf[4] = 0x4A;
	buf[5] = 0xB7;
	Booster_WriteData(buf,  6);

	fsl_delay_ms(10);
	buf[0] = 0x40;
	buf[1] = 0x10;
	buf[2] = 0x00;
	buf[3] = 0x08;
	buf[4] = 0x00;
	buf[5] = 0x00;
	Booster_WriteData(buf,  6);

	fsl_delay_ms(10);
	buf[0] = 0x40;
	buf[1] = 0x18;
	buf[2] = 0x00;
	buf[3] = 0x00;
	buf[4] = 0xF0;
	buf[5] = 0x00;
	Booster_WriteData(buf,  6);

	fsl_delay_ms(10);
	buf[0] = 0xE0;
	Booster_WriteData(buf,  1);

	fsl_delay_ms(20);
	
	Booster_SPI_Sleep();

	
}

bool BoosterSetParameter(uint8_t * buf, uint32_t len)
{
	booster_para_t booster_para;
	const uint8_t num = 6;
	uint8_t i = 0;

	Booster_SPI_Resume();


	booster_para.bNumOfCommands = buf[0];
	booster_para.abSPICommands = &buf[1];
	booster_para.length = len;

	if (booster_para.bNumOfCommands > 0)
	{
		for(i=0; i<booster_para.bNumOfCommands; i++)
		{
			fsl_delay_ms(20);
			Booster_WriteData(&booster_para.abSPICommands[i*num],  num);		
		}

		if ((booster_para.length - booster_para.bNumOfCommands * num) ==  2)
		{
			booster_para.bForceLoadCommand = buf[1+ booster_para.bNumOfCommands*num];
			if (booster_para.bForceLoadCommand == 0xE0)
			{
				fsl_delay_ms(40);
				Booster_WriteData(&booster_para.bForceLoadCommand,  1);
			}
		}
		Booster_SPI_Sleep();
		return true;
	}

	Booster_SPI_Sleep();
	return false;

}


void BoosterReadSetting(void) 
{
	uint8_t i = 0;
	uint8_t buf[18]={0};
	uint8_t buf2[18]={0};

	Booster_SPI_Resume();

	for(i=0; i<18; i++)
		buf2[i] = 0xFF;

	buf[0] = 0x20;
	Booster_WriteReadData(&buf[0],buf2, 17);
	//printf_buf(buf2,17);

	Booster_SPI_Sleep();

}

void BoosterGetParameter(uint8_t *buf, uint32_t len, uint8_t *para)
{
	uint8_t i = 0;
	
	Booster_SPI_Resume();

	for(i=0; i<(len -1); i++)
		para[i] = 0x00;

	Booster_WriteReadData(&buf[0],para, len);
	para[0] = 0x00;
	
	Booster_SPI_Sleep();

}



