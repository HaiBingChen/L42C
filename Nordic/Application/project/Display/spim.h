#ifndef _SPIM_H
#define _SPIM_H

#include "nrf_gpio.h"
#include "app_error.h"
#include "app_util_platform.h"
#include "spi_master.h"
#include "bsp.h"
#include "app_timer.h"
#include "nordic_common.h"

void HwSpi1IoPortInit(void);
void HwSpi1IoPortUnInit(void);
void hw_SpiWriteByte(uint8_t data);
uint8_t hw_SpiReadByte(void);
void SpiMultipleByteWrite(uint8_t *pData, uint16_t nByte);
void SpiMultipleByteWriteZero(uint8_t Data, uint16_t nByte);








#endif







