
#ifndef DISPLAY_FONT_H__
#define DISPLAY_FONT_H__

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include "seeed_lcd.h"

#define NEW_CHAR_LIB 

#define WHITE				0XFFFF
#define RED					0X1F00      
#define GREEN				0XE007
#define BLUE					0X00F8
#define GRAY					0X7BCF
#define BLACK				0X0000


extern uint8_t PhoneCallID[64];
extern uint8_t PhoneCallNumber[32];
extern uint8_t	SpecialFrenchFont; 

//字库函数结构
typedef uint8_t (*pFontFn)(uint16_t,uint16_t,uint8_t,uint8_t);
typedef struct
{
	uint8_t Cmd;
	pFontFn pFn;
}FONT_CMD_T;

typedef struct 
{
	uint8_t Width;
	uint8_t Len;
}RETURN_TYPE;

typedef struct 
{
    uint8_t X;
    uint8_t Y;	
    uint8_t MaxW;
    uint8_t MaxH;
}SIZE_FONT;

typedef enum
{	
	D_MODE0=0,					/*方式0: 居左; 超出屏幕则停止显示, 返回下一行起始指针偏移量(不超出则返回0)*/
	D_MODE1,					/*方式1: 居中; 超出屏幕则停止显示, 返回下一行起始指针偏移量(不超出则返回0)*/
	D_MODE2,					/*方式2: 居左; 超出屏幕则停止显示, 返回本行下一滚动位置指针偏移量(不超出则返回0)*/
	D_MODE3,					/*方式3: 居中; 超出屏幕则停止显示, 返回本行下一滚动位置指针偏移量(不超出则返回0)*/
	D_MODE4					/*方式4: 居左; 超出屏幕则显示省略号, 返回0*/
}DISPLAY_MODE;

typedef struct 
{
	uint8_t *Rowdata;
	uint8_t *data;
	DISPLAY_MODE dm;	
	uint16_t colour;	
	uint8_t x;
	uint8_t y;
	uint8_t w;
	uint8_t h;	
}LINE_ROLL_PARAMETER;

/******************************滚动参数*******************************/
#define TIMER_PERIOD_ROLL		1 
#define TIMER_PERIOD_REROLL	2 
#define DISPLAY_ROLL_TIME 		1000000u 		/*1秒*/
extern uint8_t DisplayRoll;
extern uint8_t DisplayRollPeriod;
extern uint8_t FlashDisable;
extern uint8_t OtaFontType; 					/*可OTA的类型*/
extern bool IsNormalOta;							/*是否正常OTA*/
extern uint8_t FailOta_OtaFontType;					/*升级失败OTA语言*/
/********************************************************************/
#define UNKNOWN_NUMBER   0xFF

#if defined (NEW_CHAR_LIB)	
uint8_t DisplayFont_1(uint16_t Unicode,uint16_t color,uint8_t x,uint8_t y);
uint8_t DisplayFont_2(uint16_t Unicode,uint16_t color,uint8_t x,uint8_t y);
uint8_t DisplayFont_3(uint16_t Unicode,uint16_t color,uint8_t x,uint8_t y);
uint8_t DisplayFont_4(uint16_t Unicode,uint16_t color,uint8_t x,uint8_t y);
uint8_t DisplayFont_5(uint16_t Unicode,uint16_t color,uint8_t x,uint8_t y);
uint8_t DisplayFont_6(uint16_t Unicode,uint16_t color,uint8_t x,uint8_t y);
uint8_t DisplayFont_7(uint16_t Unicode,uint16_t color,uint8_t x,uint8_t y);
#else
uint8_t DisplayFont_1(uint16_t Unicode,uint16_t Unitcode_attach,uint16_t color,uint8_t x,uint8_t y);
uint8_t DisplayFont_2(uint16_t Unicode,uint16_t Unitcode_attach,uint16_t color,uint8_t x,uint8_t y);
#endif
uint8_t DisplayFont(uint8_t cmd,uint16_t Unicode,uint16_t Unitcode_attach,uint16_t color,uint8_t x,uint8_t y);
uint16_t UnknownUnicodeConversion(uint16_t c);
bool InitDisplayFont(uint8_t* Ftype);
uint8_t CalculateAncsLen(uint8_t* dat, uint8_t Len);
void StartRoll(void);
void StopRoll(void);
void SetRollParameter(uint8_t *Rowdat, uint8_t *dat, DISPLAY_MODE dm,uint16_t colour, uint8_t x, uint8_t y, uint8_t w, uint8_t h);
void DisplayLineRoll(void);
uint8_t DisplayLine(uint8_t *dat, DISPLAY_MODE dm,uint16_t colour, uint8_t x, uint8_t y, uint8_t w, uint8_t h);
void DisplayStringNumber8x16(uint8_t * str, uint16_t color,uint8_t x, uint8_t y);

void ShowMultipleLanRollOne(DISPLAY_MODE dm,uint16_t colour,uint8_t *val,uint8_t Val_Len,
							uint8_t x,uint8_t y,uint8_t win_w,uint8_t win_h);


#endif

