#ifndef __LCD_H
#define __LCD_H

#include "nrf_gpio.h"


void LCD_Init(void);
void DisplayMap(const uint8_t *Map,uint8_t x,uint8_t y,uint8_t width,uint8_t heigh);
void ClearScreen(void);
void ClearLcdDisplay(uint8_t x,uint8_t y,uint8_t width,uint8_t heigh);
void SetLcdDisplay(uint8_t x,uint8_t y,uint8_t width,uint8_t heigh);
void showUpgrade(void);
void showUpgradeSuccess(void);
void showUpgradeFail(void);

#endif


