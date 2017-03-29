#include "smartcard.h"

volatile uint8_t rxBuff[64]={0};
volatile uint8_t SendBuffer[512];
volatile uint8_t rx_flag = 0;
volatile uint8_t TS_CHECK = 1;
volatile uint32_t index_t;

void UART2_FLEXIO_IRQHandler(void)
{
	UART_Type*  base  = UART2;

	PRINTF("IRQ\r\n");
//	if (UART_HAL_GetISO7816InterruptStatus(base, kUartIntIso7816InitialCharDetected))
//	{
//		PRINTF("TS\r\n");
//		UART_HAL_ClearISO7816InterruptStatus(base, kUartIntIso7816InitialCharDetected);
//		UART_HAL_ReceiveDataPolling(base,(uint8_t *)rxBuff, 22);
//		rx_flag = 1;
//	}
	
	if (UART_HAL_GetStatusFlag(base, kUartRxDataRegFull))
	{
		UART_HAL_Getchar(base, (uint8_t *)(rxBuff + index_t));
		PRINTF("RX FULL:0x%x\r\n", rxBuff[index_t]);
		index_t++;
	}
	if ((index_t == 23) && (TS_CHECK))
	{
		TS_CHECK = 0;
		index_t = 0;
		rx_flag = 1;
	}
}

void BufferClear(void)
{
	index_t = 0;
	memset((char *)rxBuff, 0, UART_BUFSIZE);
}

//dir=true, UART Single-Wire mode TXDIR output
//dir=false,UART Single-Wire mode TXDIR input
void SetTransmitterDir(UART_Type*	base, bool dir)
{
	if (dir)
	{
		UART_HAL_SetTransmitterDir(base, kUartSinglewireTxdirOut);
	}
	else
	{
		UART_HAL_SetTransmitterDir(base, kUartSinglewireTxdirIn);
	}
}

void pwm_init()
{
    tpm_general_config_t driverInfo;
    tpm_pwm_param_t param = {
            .mode              = kTpmEdgeAlignedPWM,
            .edgeMode          = kTpmHighTrue,
            .uFrequencyHZ      = CLK_7816,
            .uDutyCyclePercent = 50u
    };

    // Prepare memory for initialization.
    memset(&driverInfo, 0, sizeof(driverInfo));
    // Init TPM.
    TPM_DRV_Init(BOARD_PWM_INSTANCE, &driverInfo);

    // Set clock for TPM.
    TPM_DRV_SetClock(BOARD_PWM_INSTANCE, kTpmClockSourceModuleHighFreq, kTpmDividedBy2);

	// Init PWM module with updated configuration.
	TPM_DRV_PwmStart(BOARD_PWM_INSTANCE, &param, BOARD_PWM_CHANNEL);
}

void gpio_init()
{
    // Define gpio output pin config structure POWER AND RESET.
    gpio_output_pin_user_config_t outputPin[] = {
        {
            .pinName              = RESET,
            .config.outputLogic   = 0,
#if FSL_FEATURE_PORT_HAS_SLEW_RATE
            .config.slewRate      = kPortFastSlewRate,
#endif
#if FSL_FEATURE_PORT_HAS_DRIVE_STRENGTH
            .config.driveStrength = kPortHighDriveStrength,
#endif
        },
        {
            .pinName              = POWER,
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

    // Init POWER and RESET
    GPIO_DRV_Init(NULL, outputPin);
	
    GPIO_DRV_ClearPinOutput(RESET);
	//GPIO_DRV_SetPinOutput(RESET);
	
		
    GPIO_DRV_ClearPinOutput(POWER);
	//GPIO_DRV_SetPinOutput(POWER);
}

void C7816_init(UART_Type*	base)
{
	uint32_t UART_CLK = 0;
	uart_status_t status;
	
	UART_CLK = CLOCK_SYS_GetUartFreq(0);
	PRINTF("UART_CLK: %d\r\n", UART_CLK);
	status = UART_HAL_SetISO7816Etu(base, UART_CLK, CLK_7816, Fi, Di);
	if (status == kStatus_UART_Success) 
	{
		PRINTF("UART_HAL_SetISO7816Etu Done.\r\n");
	}
	
	UART_HAL_SetWorkWaitTimeInteger(base, 2);	//set the WT = 480 *2 =960
	//C1
	UART_HAL_SetLoopCmd(base, true);	//LOOPS=1
	UART_HAL_SetReceiverSource(base, kUartSingleWire);	//RSRC=1
	UART_HAL_SetBitCountPerChar(base, kUart9BitsPerChar);	//M=1
	UART_HAL_SetParityMode(base, kUartParityEven);	//set Even parity
	
	//S2	
	UART_BWR_S2_RWUID(base, true);
	UART_HAL_SetSendMsbFirstCmd(base, false);	//LSB is first bit
	UART_BWR_S2_RXINV(base, false);		//Receive data is not inverted.
	
	//C3 
	UART_HAL_SetIntMode(base, kUartIntRxOverrun, true);	
	UART_HAL_SetIntMode(base, kUartIntNoiseErrFlag, true);
	UART_HAL_SetIntMode(base, kUartIntFrameErrFlag, true);
	UART_HAL_SetIntMode(base, kUartIntParityErrFlag, true);
	
	//C4
    UART_BWR_C4_MAEN1(base, 0);
    UART_BWR_C4_MAEN2(base, 0);
	
	//C5
	UART_HAL_SetTxDmaCmd(base, false);
	
	//C7816
	//UART_HAL_ConfigureInitialCharacterDetection(base, kUartIso7816InitdEnable);
	UART_HAL_SetTransferProtocolType(base, kUartIso7816TransfertType0);
	UART_HAL_ConfigureNackOnError(base, kUartIso7816AnackEnable);
	UART_HAL_ConfigureNackOnOverflow(base, kUartIso7816OnackEnable);	//modified fsl_uart_hal.h
	UART_HAL_EnableISO7816(base);
	
	//IE7816
	UART_HAL_SetISO7816IntMode(base, kUartIntIso7816InitialCharDetected, true);
	
	//ET7816
	//TBD
	
	//C2
	UART_HAL_SetIntMode(base, kUartIntIdleLine , false);	//ILIE=0
	UART_HAL_EnableReceiver(base);	//RE=1
	UART_HAL_EnableTransmitter(base);	//TE=1
	UART_HAL_PutReceiverInNormalMode(base);	//RWU=0
	UART_HAL_SetBreakCharCmd(base, false);	//SBK=0
	UART_HAL_SetIntMode(base, kUartIntRxDataRegFull, true);	//RIE
	
	/* Enable UART interrupt on NVIC level. */
	INT_SYS_EnableIRQ(UART2_FLEXIO_IRQn);
}

void ATR(UART_Type*	base)
{  
	POWER_ON;
	UART_HAL_SetTransmitterDir(base, kUartSinglewireTxdirIn);	//RX input
	RST_HIGH;     		 //set smart card out of reset
}

/*****************************************************************************
 ** Function name:		TAToBaud
 **
 ** Descriptions:		Converts TA byte to corresponding baudrate.
 **
 ** parameters:			ta - byte value TA
 ** Returned value:	baudrate
 **                  *index - pointer to index in baud tables (if not null)
 **
 *****************************************************************************/
/*static uint32_t TAToBaud(uint8_t ta, int * index)
{
	int i;

	for (i = 0;i < sizeof(TABaudTable) / sizeof(uint8_t);i++)
		if (TABaudTable[i] == ta)
		{
			if (index) *index = i;
			return BaudTable[i];
		}

	return BaudTable[0];
}  */

/*****************************************************************************
 ** Function name:		BaudToTA
 **
 ** Descriptions:		Converts baudrate to corresponding TA byte.
 **
 ** parameters:			baudrate - baud rate to convert
 ** Returned value:	0 if no matches, otherwise TA byte value
 **                  *index - pointer to index in baud tables (if not null)
 **
 *****************************************************************************/
/*static uint8_t BaudToTA(uint32_t baudrate, int * index)
{
	int i;

	for (i = 0;i < sizeof(TABaudTable) / sizeof(uint8_t);i++)
		if (BaudTable[i] == baudrate)
		{
			if (index) *index = i;
			return TABaudTable[i];
		}

	return 0;
} */

void smartcardInit(void)
{
	/* Turn off clock to device, wait in reset, make sure RX cannot receive garbage */
//	USARTIgnore(); /* We don't want random noise from insertion */
//	CardOff();
//	USARTConfig(BaudTable[0], 8, PARITY_EVEN, 1, 4);
	
	// Init pwm
	pwm_init();
	
	//Init POWER and RESET GPIO
	gpio_init();
	
	// Init 7816 Controller
	C7816_init(UART2);
}

int smartcardReset(void)
{
	/* Indicate that the card has been inserted */
//	USARTClear();						/* Clear USART buffer */
//	ShowActivity();
//	CardPowerOn(); 						/* Power on */
//	delay32Ms(TIME_SOURCE, T_Ta); 		/* Wait for power to settle */
//	ShowActivity();
//	CardClockOn(); 						/* Start clock at Ta */
//	delay32Ms(TIME_SOURCE, T_Ta_Tb); 	/* Wait until Tb */
//	ShowActivity();

//	USARTAccept();						/* Come out of reset, get ready for ATR */
//	memset((void *)USARTBuffer, 0, sizeof(USARTBuffer));

//	CardDeassertReset();				/* Release reset to start the ATR */

//	delay32Ms(TIME_SOURCE, T_Tb_Tc + (32 * T_BYTE_TIME)); /* Wait for 32 bytes to be sent */
//	ShowActivity();

//	if (buflen > USARTCount)			/* only copy what is in the USART buffer */
//		buflen = USARTCount;

//	memcpy(ATRBuf, (const char *)USARTBuffer, buflen);

//	return buflen;
	ATR(UART2);
	while(!rx_flag);
	return 0;
}


void smartcardWaitForInsertion()
{
//	while(1)
//	{
//		while (!isCardInserted()) ; /* Wait for card to be inserted */
//		delay32Ms(TIME_SOURCE, T_INSERTION_DEBOUNCE); /* Verify the card is still there */
//		if (isCardInserted())
//			return;
//	}
}

void smartcardWaitForRemoval()
{
	/* Turn off clock to device, wait in reset */
//	CardClockOff(); /* reset and stop clock */
//	USARTIgnore(); /* Make sure RX interrupt does not respond to garbage on data line */
//	CardOff();
//	while (isCardInserted()) ; /* Wait for card to be extracted */
}

void smartcardCommandDelay(int bytecnt)
{
	/* Wait for command to complete */
	for(int i=0; i<80000; i++)
	{
		__asm("nop");
	}
}

//Transmits APDU to Card and returns the response
APDU_STATUS SendAPDU(
	unsigned char CLA,
	unsigned char INS,
	unsigned char P1,
	unsigned char P2,
	unsigned char P3,
	unsigned char writeFlag,
	unsigned char *DataBuff,
	unsigned char *SW1,
	unsigned char *SW2)
{
	unsigned char procedureByte;
	int index = 0;
	int dataLen = P3?P3:256;
	UART_Type*  base  = UART2;
	uint8_t buffer[12] = {0};

	PRINTF("COS -1\r\n");
	if (CLA == 0xff)
		return APDU_STATUS_INVALID_CMD;

	if ((INS >> 4) == 6 || (INS >> 4) == 9)
		return APDU_STATUS_INVALID_INS;

	PRINTF("COS 0\r\n");
	BufferClear();
	SetTransmitterDir(base, true);
	buffer[0] = CLA;
	buffer[1] = INS;
	buffer[2] = P1;
	buffer[3] = P2;
	buffer[4] = P3;
	UART_HAL_SendDataPolling(base,(const uint8_t *)buffer, 5);
	

	// Grab procedure bytes from the smart card in response to the command
	while(1)
	{
		SetTransmitterDir(base, false);
		smartcardCommandDelay(1);
		procedureByte = rxBuff[index++];
		if (procedureByte == 0x60)                                  // NULL, wait for another procedure byte
		{
			PRINTF("COS 1\r\n");
			continue;
		}
		if ((procedureByte >> 4) == 6 || (procedureByte >> 4) == 9) // SW1
		{
			PRINTF("COS 2\r\n");
			*SW1 = procedureByte;
			smartcardCommandDelay(1);
			*SW2 = rxBuff[index++];
			switch(*SW1)
			{
			case 0x90:
				PRINTF("0x90\r\n");
				return APDU_STATUS_SUCCESS;
			case 0x6e:
				PRINTF("0x6e\r\n");
				return APDU_STATUS_INVALID_CMD;
			case 0x6d:
				PRINTF("0x6d\r\n");
				return APDU_STATUS_INVALID_INS;
			case 0x6b:
				PRINTF("0x6b\r\n");
				return APDU_STATUS_INVALID_P1P2;
			case 0x67:
				PRINTF("0x67\r\n");
				return APDU_STATUS_INVALID_P3;
			case 0x6f:
				PRINTF("0x6f\r\n");
				return APDU_STATUS_UNKNOWN_ERROR;
			}
		}
		if (procedureByte == INS)                                    // ACK, copy all data
		{
			//PRINTF("COS 4\r\n");
			SetTransmitterDir(base, true);
			if (writeFlag)
				UART_HAL_SendDataPolling(base, DataBuff, dataLen);
			else
			{
				smartcardCommandDelay(dataLen);
				memcpy(DataBuff, (const char *)&rxBuff[index], dataLen);
				index += dataLen;
			}
			DataBuff += dataLen;
			dataLen = 0;
			continue;
		}

		if (procedureByte == (INS ^ 0xff))                           // ACK, copy one byte of data
		{
			PRINTF("COS 5\r\n");
			if (!dataLen)
				return APDU_STATUS_INVALID_PROCEDURE_BYTE;
			SetTransmitterDir(base, true);
			if (writeFlag)
			{
				UART_HAL_SendDataPolling(base, DataBuff, 1);
				DataBuff++;
			}
			else
			{
				smartcardCommandDelay(1);
				*DataBuff++ = rxBuff[index++];
			}
			dataLen--;
			continue;
		}
		return APDU_STATUS_INVALID_PROCEDURE_BYTE;
	}
}

void smartcardParseATR(int buflen, unsigned char *ATRBuf)
{
//	int i;
//	uint8_t *buffer_ptr;
//	uint8_t TS, T0, TA[15], TB[15], TC[15], TD[15];
//	uint8_t InterfaceBytesBitmap, InterfaceBytesSet;

//	//printf("ATR: ");
//	for(i = 0; i < buflen; i++)
//	{}
//		//printf("%02x ", ATRBuf[i]);
//	//printf("\n");

//	buffer_ptr = ATRBuf;
//	TS = *buffer_ptr++;
//	//printf("TS = 0x%02x\n", TS);
//	T0 = InterfaceBytesBitmap = *buffer_ptr++;
//	//printf("T0 = 0x%02x\n", T0);
//	InterfaceBytesSet = 0;
//	while((InterfaceBytesBitmap & 0xf0) && (InterfaceBytesSet < sizeof(TA)))
//	{
//		if (InterfaceBytesBitmap & 0x10)
//		{
//			TA[InterfaceBytesSet] = *buffer_ptr++;
//			//printf("TA%d = 0x%02x\n", InterfaceBytesSet, TA[InterfaceBytesSet]);
//		}

//		if (InterfaceBytesBitmap & 0x20)
//		{
//			TB[InterfaceBytesSet] = *buffer_ptr++;
//			//printf("TB%d = 0x%02x\n", InterfaceBytesSet, TB[InterfaceBytesSet]);
//		}

//		if (InterfaceBytesBitmap & 0x40)
//		{
//			TC[InterfaceBytesSet] = *buffer_ptr++;
//			//printf("TC%d = 0x%02x\n", InterfaceBytesSet, TC[InterfaceBytesSet]);
//		}

//		if (InterfaceBytesBitmap & 0x80)
//		{
//			TD[InterfaceBytesSet] = InterfaceBytesBitmap = *buffer_ptr++;
//			//printf("TD%d = 0x%02x\n", InterfaceBytesSet, TD[InterfaceBytesSet]);
//			InterfaceBytesSet++;
//		}
//		else
//			InterfaceBytesBitmap = 0;
//	}
//	//printf("HistoricalBytes: ");
//	for(i = 0; i < buflen - (buffer_ptr - ATRBuf); i++)
//	{
//	}
//	//	printf("%02x ", *(buffer_ptr + i));
//	//printf("\n");

//	//printf("\nTransmission protocol: T%d\n", T0 & 0x0f);

}

int Test_ADPU(void)
{
	APDU_STATUS APDUStatus;
	unsigned char CLA, INS, P1, P2, P3, SW1, SW2, writeFlag;

	BufferClear();
	CLA = 0x00;
	INS = 0x00;
	P1  = 0x00;
	P2  = 0x00;
	P3  = 0x04;
	writeFlag = 1;

	APDUStatus = SendAPDU(CLA, INS, P1, P2, P3, writeFlag, (unsigned char *)SendBuffer, &SW1, &SW2);

	if (APDU_STATUS_SUCCESS == APDUStatus)
	{
		PRINTF("COS CARD_STATUS_SUCCESS\r\n");
		return 0;
	}
	else
	{
		PRINTF("COS_CARD_STATUS_ERROR\r\n");
	 	return -1;
	}
}


