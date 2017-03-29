#include "spi.h"
#include "timer.h"
#include "fsl_interrupt_manager.h"
#include "fsl_delay.h"
#include "booster.h"
#include "7816.h"

char SPI_INT_FLAG = 0;
char NFC_INT_FLAG = 0;
char NFC_UPDATE_FLAG = 0;
char NFC_Field_FLAG = 0;
extern uint8_t mutex_flag;

spi_buffer_t spi_buffer;
spi_ctx_t m_spi_ctx;
spi_t m_spis;

static void gpio_init()
{
	// Define gpio input pin config structure SW.
	gpio_input_pin_user_config_t inputPin[] = {
		{
		.pinName                       = NFC_INT,
		.config.isPullEnable           = true,
#if FSL_FEATURE_PORT_HAS_PULL_SELECTION
		.config.pullSelect             = kPortPullUp,
#endif
#if FSL_FEATURE_PORT_HAS_PASSIVE_FILTER
		.config.isPassiveFilterEnabled = false,
#endif
#if FSL_FEATURE_PORT_HAS_DIGITAL_FILTER
		.config.isDigitalFilterEnabled = false,
#endif
		.config.interrupt              = kPortIntRisingEdge,
		},
		
		{
		.pinName = GPIO_PINS_OUT_OF_RANGE,
		}
	};

	// Define gpio output pin config structure POWER AND RESET.
	gpio_output_pin_user_config_t outputPin[] = {
		{
		.pinName              = SPI_CS,
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
	GPIO_DRV_Init(inputPin, outputPin);
	GPIO_DRV_SetPinOutput(SPI_CS);
}


void SPI_INT_CFG_OUTPUT(void)
{
		// Define gpio input pin config structure SW.
		gpio_input_pin_user_config_t inputPin[] = {
			{
				.pinName					   = SPI_INT,
				.config.isPullEnable		   = true,
#if FSL_FEATURE_PORT_HAS_PULL_SELECTION
				.config.pullSelect			   = kPortPullUp,
#endif
#if FSL_FEATURE_PORT_HAS_PASSIVE_FILTER
				.config.isPassiveFilterEnabled = false,
#endif
#if FSL_FEATURE_PORT_HAS_DIGITAL_FILTER
				.config.isDigitalFilterEnabled = false,
#endif
				.config.interrupt			   = kPortIntDisabled,
			},
			{
				.pinName = GPIO_PINS_OUT_OF_RANGE,
			}
		};
		
		// Init SPI CS
		GPIO_DRV_Init(inputPin, 0);

	
    // Define gpio output pin config structure POWER AND RESET.
    gpio_output_pin_user_config_t outputPin[] = {
        {
            .pinName              = SPI_INT,
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

    // Init SPI INT 
    GPIO_DRV_Init(0, outputPin);
}


void SPI_INT_CFG_OUTPUT_HIGH(void)
{
	GPIO_DRV_SetPinOutput(SPI_INT);
}

void SPI_INT_CFG_OUTPUT_LOW(void)
{
	GPIO_DRV_ClearPinOutput(SPI_INT);
}

void SPI_INT_CFG_INPUT(void)
{
	// Define gpio input pin config structure SW.
	gpio_input_pin_user_config_t inputPin[] = {
		{
		.pinName                       = SPI_INT,
		.config.isPullEnable           = true,
#if FSL_FEATURE_PORT_HAS_PULL_SELECTION
		.config.pullSelect             = kPortPullUp,
#endif
#if FSL_FEATURE_PORT_HAS_PASSIVE_FILTER
		.config.isPassiveFilterEnabled = false,
#endif
#if FSL_FEATURE_PORT_HAS_DIGITAL_FILTER
		.config.isDigitalFilterEnabled = false,
#endif
		.config.interrupt              = kPortIntRisingEdge,
		},
		
		{
		.pinName = GPIO_PINS_OUT_OF_RANGE,
		}
	};
	GPIO_DRV_Init(inputPin, 0);
}



void SPI_Init(void)
{
	SPI_Type * spiBaseAddr = g_spiBase[SPI_MASTER_INSTANCE];

	uint32_t spiSourceClock;
	//SPI_Type * spiBaseAddr = g_spiBase[SPI_MASTER_INSTANCE];;

	//SPI_HAL_Disable(spiBaseAddr);

	// set CS pin as GPIO
	gpio_init();	

	// Enable clock for SPI
	CLOCK_SYS_EnableSpiClock(SPI_MASTER_INSTANCE);

	// configure the run-time state struct with the source clock value
	spiSourceClock = CLOCK_SYS_GetSpiFreq(SPI_MASTER_INSTANCE);

	// Reset the SPI module to it's default state, which includes SPI disabled
	SPI_HAL_Init(spiBaseAddr);

	// Set SPI to master mode
	SPI_HAL_SetMasterSlave(spiBaseAddr, kSpiMaster);

	// Set slave select to automatic output mode
	SPI_HAL_SetSlaveSelectOutputMode(spiBaseAddr, kSpiSlaveSelect_AutomaticOutput);

	// Set the SPI pin mode to normal mode
	SPI_HAL_SetPinMode(spiBaseAddr, kSpiPinMode_Normal);

#if FSL_FEATURE_SPI_FIFO_SIZE
	if (g_spiFifoSize[SPI_MASTER_INSTANCE] != 0)
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
	SPI_HAL_SetBaud(spiBaseAddr, TRANSFER_BAUDRATE, spiSourceClock);
	// Setup format as same as slave
	SPI_HAL_SetDataFormat(spiBaseAddr, kSpiClockPolarity_ActiveHigh, kSpiClockPhase_FirstEdge, kSpiMsbFirst);	

	PRINTF("SPI1 Init Done\r\n");
}

void SPI_WriteData(SPI_Type * spiBaseAddr, uint8_t * TxBuffer, uint8_t * RxBuffer, uint32_t SIZE)
{
	volatile uint8_t dummy_data = 0;
	int i;
	
	// Disable module to clear the shift register
	SPI_HAL_Disable(spiBaseAddr);
	SPI_HAL_Enable(spiBaseAddr);

	CS_LOW();
	
	for (i = 0; i < SIZE; i++)
	{
#if FSL_FEATURE_SPI_16BIT_TRANSFERS
		SPI_HAL_WriteDataBlocking(spiBaseAddr, kSpi8BitMode, 0, TxBuffer[i]);
#else
		// Write data to slave
		SPI_HAL_WriteDataBlocking(spiBaseAddr, TxBuffer[i]);
#endif
		// Wait for slave send data back
		while(SPI_HAL_IsReadBuffFullPending(spiBaseAddr)==0){}
			
		// Read data register
#if FSL_FEATURE_SPI_16BIT_TRANSFERS
		RxBuffer[i] = SPI_HAL_ReadDataLow(spiBaseAddr);
#else
		RxBuffer[i] = SPI_HAL_ReadData(spiBaseAddr);
#endif
	}
	
	CS_HIGH();	
}

void SPI_ReadData(SPI_Type * spiBaseAddr, uint8_t * Buffer, uint32_t SIZE)
{
	int i;
	uint8_t dummy_data = 0x5A;
	

	// Disable module to clear the shift register
	SPI_HAL_Disable(spiBaseAddr);
	SPI_HAL_Enable(spiBaseAddr);

	CS_LOW();
	for (i = 0; i < SIZE; i++)
	{
#if FSL_FEATURE_SPI_16BIT_TRANSFERS
		// Send dummy byte to receive data from slave
		SPI_HAL_WriteDataBlocking(spiBaseAddr, kSpi8BitMode, 0, dummy_data);
#else
		// Send dummy byte to receive data from slave
		SPI_HAL_WriteDataBlocking(spiBaseAddr, dummy_data);
#endif
		// Wait for slave send data back
		while(SPI_HAL_IsReadBuffFullPending(spiBaseAddr)==0){}
			
		// Read data register
#if FSL_FEATURE_SPI_16BIT_TRANSFERS
		Buffer[i] = SPI_HAL_ReadDataLow(spiBaseAddr);
#else
		Buffer[i] = SPI_HAL_ReadData(spiBaseAddr);
#endif

	}
	CS_HIGH();

}


static void spi_receive_packet(void)
{
	uint8_t quotients = 0;
	uint8_t remainders = 0;
	SPI_Type * spiBaseAddr = g_spiBase[SPI_MASTER_INSTANCE];
	
	SPI_ReadData(spiBaseAddr, &spi_buffer.buf_command_response[(spi_buffer.buf_command_response_read_idx)*BLOCK_SIZE], BLOCK_SIZE);
	//printf_buf(&spi_buffer.buf_command_response[(spi_buffer.buf_command_response_read_idx)*BLOCK_SIZE],BLOCK_SIZE);
	if (spi_buffer.buf_command_response_read_idx == 0)
	{
		m_spis.io.mode = SPI_IO_RX_CMD_MODE;
		m_spi_ctx.rx_done = false;
		if(((spi_command_t*) spi_buffer.buf_command_response)->header.tags!=0xff)
		{
			PRINTF("Head err,return\r\n");
			return;
		}
		if(((spi_command_t*) spi_buffer.buf_command_response)->header.len>SPI_COMMAND_PAYLOAD_MAX_SIZE)
		{
			PRINTF("Len err,return\r\n");
			return;
		}	
		if (((spi_command_t*) spi_buffer.buf_command_response)->header.len <= 16)		//always one package
		{
			m_spi_ctx.last_num = 0;
		} 
		else {	// more than one package
			quotients = (((spi_command_t*) spi_buffer.buf_command_response)->header.len - 16)/20;	//quotients
			remainders = (((spi_command_t*) spi_buffer.buf_command_response)->header.len - 16)%20;	//remainders
			if (remainders)
			{
				m_spi_ctx.last_num = quotients + 1;
			}
			else {
				m_spi_ctx.last_num = quotients;
			}
		}
	}
	//PRINTF("spi_receive_packet:%d,%d\r\n",spi_buffer.buf_command_response_read_idx,m_spi_ctx.last_num);
	if (spi_buffer.buf_command_response_read_idx == m_spi_ctx.last_num)		//finished all package
	{
		spi_buffer.buf_command_response_read_idx = 0;
		m_spi_ctx.last_num = 0;
		m_spi_ctx.rx_done = true;
		
	} else {
		spi_buffer.buf_command_response_read_idx++;		//continue receiving package
	}
	
}

#define CHECK_PATTERN	0x7F
static bool wait_for_send_done(uint8_t * buf, uint8_t pattern, uint16_t size)
{
	uint32_t i = 0;
	for(i=0; i<size; i++)
	{
		if (buf[i] != pattern)
		{
			return false;
		}
	}
	return true;
}


static bool spi_send_packet(uint8_t * P_buf, uint16_t offset, uint16_t len)
{
	uint8_t quotients = 0;
	uint8_t remainders = 0;
	uint8_t temp[BLOCK_SIZE]={0};
	bool status = false;
	SPI_Type * spiBaseAddr = g_spiBase[SPI_MASTER_INSTANCE];

	#if 0
	status = spi_send_data_init();
	if (!status)
		PRINTF("data ini fail\r\n");
		
	#endif
	
	fsl_delay_ms(10);
	
	if (spi_buffer_temp.buf_command_response_write_idx == 0)
	{
		if (len <= 20)		//always send one package
		{
			m_spi_ctx.last_num = 0;
		}
		else {		//more than one package
			quotients = (len - 20)/20;
			remainders = (len -20)%20;
			if (remainders)
			{
				m_spi_ctx.last_num = quotients + 1;
			} 
			else {
				m_spi_ctx.last_num = quotients;
			}
			
		}
	}

	if (spi_buffer_temp.buf_command_response_write_idx == m_spi_ctx.last_num)
	{
		spi_buffer_temp.buf_command_response_write_idx = 0;
		m_spi_ctx.last_num = 0;
	}
	else {
		spi_buffer_temp.buf_command_response_write_idx++;
	}

	SPI_WriteData(spiBaseAddr, &P_buf[offset],temp, BLOCK_SIZE);
	status = wait_for_send_done(temp, CHECK_PATTERN, BLOCK_SIZE);
	return status;
}

bool spi_send_packets(uint8_t * P_buf,  uint16_t len)
{
	bool status = false;
	
	do {
		status = spi_send_packet(P_buf, spi_buffer_temp.buf_command_response_write_idx*BLOCK_SIZE,(((spi_command_t*) spi_buffer_temp.buf_command_response)->header.len + 4) );
		if (!status)
		{
			spi_buffer_temp.buf_command_response_write_idx = 0;
			return false;
		}
	}while(spi_buffer_temp.buf_command_response_write_idx);

	return true;
}


/*!
 * @brief Interrupt service fuction of switch.
 *
 * This function toogles LED1
 */
void PORTCD_IRQHandler(void)
{
	if (GPIO_DRV_IsPinIntPending(SPI_INT))
	{
		// Clear external interrupt flag.g
		GPIO_DRV_ClearPinIntFlag(SPI_INT);
		spi_receive_packet();
	}
	if (GPIO_DRV_IsPinIntPending(NFC_INT))
	{
		// Clear external interrupt flag.
		GPIO_DRV_ClearPinIntFlag(NFC_INT);
		if ((NFC_INT_FLAG == 0) && (NFC_UPDATE_FLAG == 0) && (mutex_flag == 0))
		{
			#if EMV_TEST
			BoosterIrqDisable();
			#endif
			BoosterIrqClear();
			NFC_INT_FLAG = 1;
			NFC_Field_FLAG =1;
			timer_start();
		}
	}
}

