
#ifndef _OTA_LPUART0_H
#define _OTA_LPUART0_H

#include "NordicCmdPackage.h"

//协议函数结构
typedef void (*pOtaHandlerFn)(uint8_t);
typedef struct
{
	uint8_t OtaCmd;
	pOtaHandlerFn pFn;
} OTA_DATA_CMD_T;

void Ota_LpUart0_Init(void);

void OtaProgress(uint8_t Pro);
void OtaResult(uint8_t Res);
void Enable_LpUart0_IRQ(void);
void Disable_LpUart0_IRQ(void);

#endif

