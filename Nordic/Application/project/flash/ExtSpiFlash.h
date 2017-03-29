
#include "Typedef.h"

void Power_Suspend(void);
void Power_Resume(void);
/*Erase Interface*/
void SpiFlash_4KSector_Erase(uint32 EraseAddr);
void SpiFlash_64KBlock_Erase(uint32 EraseAddr);
void SpiFlash_ChipErase(void);

/*Debug Interface*/
uint8 SpiFlash_GetBusyBit(void);
uint32 SpiFlash_ReadJedecID(void);

/*Write Read Interface*/
void SpiFlash_Write(uint32 addr,uint8*pBuf, uint16 len);
void SpiFlash_Read( uint32 addr,uint8*pBuf , uint16 len);
void SpiFlash_Read_TP_Firmware( uint32 addr,uint8*pBuf , uint16 len);
void SpiFlash_Enable_IO(void);
void SpiFlash_Disable_IO(void);
void Force_NordicFlash(void);
void Free_NordicFlash(void);
void SpiFlash_Init(void);




