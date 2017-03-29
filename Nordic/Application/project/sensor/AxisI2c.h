#ifndef __AXISI2C_H
#define __AXISI2C_H

#include "Typedef.h"

#ifdef __cplusplus
extern "C" {
#endif

extern void AxisI2cUnInit(void);
extern void AxisI2cInit(void);
extern void AxisI2cGpioInit(void);
extern bool AxisI2cSend(uint8 ucDeviceAddr, uint8 ucBufferIndex, uint8* pucData, uint32 unDataLength);
extern bool AxisI2cReceive(uint8 ucDeviceAddr, uint8 ucBufferIndex, uint8* pucData, uint32 unDataLength);
extern uint8 AxisI2cReceiveByte(bool bAck);
extern bool AxisI2cSendByte(uint8 ucData);
extern void UnlockI2c(void);  

extern bool I2cMultipleByteRead(uint8 Address,uint8 RegAddress,uint8 *Value,uint16 nByte);
extern bool I2cMultipleByteWrite(uint8 Address,uint8 RegAddress,uint8 *Data,uint16 nByte);
#ifdef __cplusplus
}
#endif

#endif

/********************************************************************************************************
  END FILE
********************************************************************************************************/


