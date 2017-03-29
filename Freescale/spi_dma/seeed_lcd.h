/* Copyright (c) 2015 Nordic Semiconductor. All Rights Reserved.
 *
 * The information contained herein is property of Nordic Semiconductor ASA.
 * Terms and conditions of usage are described in detail in NORDIC
 * SEMICONDUCTOR STANDARD SOFTWARE LICENSE AGREEMENT.
 *
 * Licensees are granted free, non-transferable use of the information. NO
 * WARRANTY of ANY KIND is provided. This heading must NOT be removed from
 * the file.
 *
 */

#ifndef SEEED_LCD_H__
#define SEEED_LCD_H__

#include <stdint.h>
#include <stdbool.h>
/**
 * @ingroup seeed_lcd_panel_demo
 * @defgroup seeed_lcd_panel_demo_header LCD code header
 *
 * Function prototypes for the code presenting SEEED LCD driving
 * @{
 */

#define LCD_SIZE_W 240
#define LCD_SIZE_H 240

	
#define CHAR_16X24		1
#define CHAR_20X24		2
#define CHAR_12X16		3
#define CHAR_10X16		4

#define CHAR_7X14		5
#define CHAR_11X20		6
#define CHAR_12X24		0

#define DIS_CENTER 		0
#define DIS_R_ALIGNED	1
#define DIS_L_ALIGNED	2


typedef enum
{
	D_INIT= 0x00,
	D_ADD
}DISPLAYADDRADD;

typedef enum
{
	POSIOTN_0DGR = 0x00,
	POSIOTN_90DGR,
	POSIOTN_180DGR,
	POSIOTN_270DGR,
}LCD_XY_POSITION;

extern bool CleanDisplayMode;
extern bool NeedFillData;
extern bool NeedBackground;
extern int Dis_Offset;
void Seeed_lcd_init (void);
void Dma_GetFlashData(void);
void Dma_DataParse(void);
void ClearLcdDisplay(uint8_t x,uint8_t y,uint8_t w,uint8_t h);
void ClearFullScreen(void);
void SetFullScreen(uint16_t colour);
void SetScreen(uint8_t posx, uint8_t posy, uint8_t posw,uint8_t posh,uint16_t colour);
void seeed_lcd_put_backgnd (void);
void seeed_lcd_put_window (uint8_t qtr_pos_x, uint8_t qtr_pos_y, uint8_t blend);
void wr_cmd (uint8_t cmd);
void wr_dat (uint8_t *data, uint32_t len);
void wait_ms(volatile uint32_t number_of_ms);
void DisplayMap(uint8_t *Map, uint8_t posx, uint8_t posy, uint32_t width, uint32_t height);
void LCD_InitPins2(void);
void _clear_lcd(void);
void DisplayDotFont(const uint8_t *Map,uint16_t color,uint8_t x,uint8_t y,uint8_t width,uint8_t height);
void DisplayDotMap(const uint8_t *Map,uint16_t color,uint8_t x,uint8_t y,uint8_t width,uint8_t height);
void DrawOneLine(uint16_t colour,uint8_t y);
void DisplayCharCleanAdd(DISPLAYADDRADD ad, char*CString, uint16_t CharType, uint16_t color, uint8_t posx, uint8_t posy);
void DisplayFontCleanAdd(DISPLAYADDRADD ad, char*CString, uint32_t FontType,uint8_t posx, uint8_t posy);
void DisplayFontCleanAdd_DisOffset(DISPLAYADDRADD ad, char*CString, uint32_t FontType,uint8_t posx, uint8_t posy,uint8_t DisOffset);
void DisplayMapCleanAdd(DISPLAYADDRADD ad,uint32_t addr, uint8_t posx, uint8_t posy, uint32_t width, uint32_t height);
void DisplayMap_flash_WithCleanSreen(void);
void DisplayMap_flash(uint32_t addr, uint8_t posx, uint8_t posy, uint32_t width, uint32_t height);
void DisplayFontWithoutCls(char*CString, uint32_t FontType,uint8_t StrLen,uint8_t posx, uint8_t posy,uint8_t width,uint8_t height);
void DisplayFontWithoutCls_DisOffset(uint8_t Disffset,char*CString, uint32_t FontType,uint8_t StrLen,uint8_t posx, uint8_t posy,uint8_t width,uint8_t height);

void Lcd_Test0(void);
void Lcd_Test1(void);
void LCD_Sleep(uint8_t state);
uint8_t GetCharDisXpos(char *str, uint16_t CharType, uint8_t xpos, uint8_t DisMode);
/** @} */

#endif
