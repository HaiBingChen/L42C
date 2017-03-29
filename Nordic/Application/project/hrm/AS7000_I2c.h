#ifndef __HR_AS7000_I2C_H
#define __HR_AS7000_I2C_H

#include "Typedef.h"

#ifdef __cplusplus
extern "C" {
#endif

extern void HRM_UnlockI2c(void);  
extern void HRM_Comms_init(void);



extern bool HRM_I2cMultipleByteRead(uint8 Address,uint8 RegAddress,uint8 *Value,uint16 nByte);
extern bool HRM_I2cMultipleByteWrite(uint8 Address,uint8 RegAddress,uint8 *Data,uint16 nByte);
#ifdef __cplusplus
}
#endif

#endif

/********************************************************************************************************
  END FILE
********************************************************************************************************/


