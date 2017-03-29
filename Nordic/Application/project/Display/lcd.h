#ifndef __LCD_H
#define __LCD_H

#include "nrf_gpio.h"


#define LCD_SIZE_W		240
#define LCD_SIZE_H		240

#define LCD_ENTER_SLEEP	0x00
#define LCD_EXIT_SLEEP		0x01



void DisplayMap(const uint8_t *Map,uint8_t x,uint8_t y,uint8_t width,uint8_t heigh);
void ClearScreen(void);
void LCD_Sleep(unsigned char mode);
void ClearLcdDisplay(uint8_t x,uint8_t y,uint8_t width,uint8_t heigh);
void SetLcdDisplay(uint8_t x,uint8_t y,uint8_t width,uint8_t heigh);

#endif


