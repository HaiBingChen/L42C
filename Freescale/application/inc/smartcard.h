/****************************************************************************
 *
 *   Description:
 *     This file contains a smartcard interface that sits on top of the USART
 *
 *   Copyright(C) 2011, NXP Semiconductor
 *   All rights reserved.
 *
 ****************************************************************************
 * Software that is described herein is for illustrative purposes only
 * which provides customers with programming information regarding the
 * products. This software is supplied "AS IS" without any warranties.
 * NXP Semiconductors assumes no responsibility or liability for the
 * use of the software, conveys no license or title under any patent,
 * copyright, or mask work right to the product. NXP Semiconductors
 * reserves the right to make changes in the software without
 * notification. NXP Semiconductors also make no representation or
 * warranty that such application will be suitable for the specified
 * use without further testing or modification.
 ****************************************************************************/
#ifndef __SMARTCARD__
#define __SMARTCARD__
 
#include <string.h>
#include "fsl_uart_hal.h"
#include "fsl_tpm_driver.h"
#include "fsl_gpio_driver.h"
#include "fsl_interrupt_manager.h"
#include "fsl_debug_console.h"
#include "fsl_clock_manager.h"

#define MAX_ATR_LEN (32)

/*! @brief Pin names */
enum _7816_pins_pinNames{
  RESET = GPIO_MAKE_PIN(GPIOD_IDX, 1U),
  POWER = GPIO_MAKE_PIN(GPIOC_IDX, 8U),
};

/* The TPM instance/channel used for board */
#define BOARD_PWM_INSTANCE              0
#define BOARD_PWM_CHANNEL               0

/*7816 clock*/
#define CLK_7816						2000000
#define Fi								372
#define Di								1


#define RST_HIGH						GPIO_DRV_SetPinOutput(RESET);
#define RST_LOW							GPIO_DRV_ClearPinOutput(RESET);

#define POWER_ON    					GPIO_DRV_SetPinOutput(POWER);
#define POWER_OFF						GPIO_DRV_ClearPinOutput(POWER);

#define UART_BUFSIZE					64

typedef enum
{
	APDU_STATUS_SUCCESS,
	APDU_STATUS_INVALID_CMD,
	APDU_STATUS_INVALID_INS,
	APDU_STATUS_INVALID_P1P2,
	APDU_STATUS_INVALID_P3,
	APDU_STATUS_UNKNOWN_ERROR,
	APDU_STATUS_INVALID_PROCEDURE_BYTE,
	APDU_STATUS_TIMEOUT,
}APDU_STATUS;

void smartcardInit(void);
//int smartcardReset(int buflen, unsigned char *ATRBuf);
int smartcardReset(void);
void smartcardWaitForInsertion(void);
void smartcardWaitForRemoval(void);
void smartcardParseATR(int buflen, unsigned char *ATRBuf);
int Test_ADPU(void);
APDU_STATUS SendAPDU(
		unsigned char CLA,
		unsigned char INS,
		unsigned char P1,
		unsigned char P2,
		unsigned char P3,
		unsigned char writeFlag,
		unsigned char *DataBuff,
		unsigned char *SW1,
		unsigned char *SW2);
		
#endif
