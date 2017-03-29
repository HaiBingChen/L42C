
#ifndef __NV_FLASH_H
#define __NV_FLASH_H

#include "Typedef.h"

#ifdef __cplusplus
extern "C" {
#endif

inline void  Select_Flash(void);
inline void  NotSelect_Flash(void);
void Mx25LxxResume(void);
void Mx25LxxSuspend(void);
uint8 NvFlashWrite( uint32 add,void *pBuf ,  uint16 len);
uint8 NvFlashRead( uint32 add,void *pBuf ,  uint16 len);
uint8 Mx25LxxFlashSendByte(uint8 byte);
void NvFlashErase( uint32 add);



  
#ifdef __cplusplus
}
#endif

#endif



/********************************************************************************************************
  END FILE
********************************************************************************************************/


