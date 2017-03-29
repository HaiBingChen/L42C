#ifndef __FONTDISPLAY_H__
#define __FONTDISPLAY_H__

#include "Typedef.h"

#define IS_ASCII(tch)		((tch>=0x0020)&&(tch<=0x007E))
#define IS_LARTIN(tch)	((tch>=0x00C0)&&(tch<=0x017F))
#define IS_RUSSION(tch)	(((tch>=0x0410)&&(tch<=0x044F))||(tch==0x039B))
#define IS_CHINESE(tch)	((tch>=0x4E00)&&(tch<=0x9FBF))
#define IS_JAPANESE(tch) (((tch>=0x3041)&&(tch<=0x30FF))||((tch>=0x31F0)&&(tch<=0x31FF)))
#define IS_THAI(tch)		((tch>=0x0E00)&&(tch<=0x0E7F))
#define IS_KOREAN(tch) 	(((tch>=0x1100)&&(tch<=0x11FF))||((tch>=0x3130)&&(tch<=0x318F))||((tch>=0xAC00)&&(tch<=0xD7AF)))
#if 0
#define IS_ARABIC(tch)	((tch>=0x0600)&&(tch<=0xFEFC))
#endif 

#define LETTER_PERIOD  5
#define ROLL_PERIOD  1000
typedef enum 
{
	ASSCII=0x00,	
	LARTIN,
	RUSSION,
	RUSSIONS,
	CHINESE,
	JAPANESE
}FONT_TYPE;

typedef enum 
{
	UNROLL=0x00,	
	ROLL
}DISPLAY_MODE;

#define FONT_WIDTH_ASCII			9
#define FONT_WIDTH_LARTIN			9
#define FONT_WIDTH_RUSSION		9
#define FONT_WIDTH_CHINESE		16
#define FONT_WIDTH_JANPANSE 		16
#define FONT_WIDTH_THAI			16
#define FONT_WIDTH_KOREAN 		16

#define XPOSMAX	64
extern uint8 XPos;

void Get_Ascii_Dot(uint8*Dot,uint16 Unicode);
uint8 GetUtf8UserLen(void);
uint8 CalculateDataLen(uint8 utf8);
bool Utf8ToUnitcode(uint8*utf8,uint8 offset, uint8 len, uint16* unitcode, uint16* unitcode_attach);

void DisplayUnicode_Stop(void);

void	DisplaySingleInit(uint8 x);
bool DisplaySingleUnicode(uint16 unicode, uint16 unitcode_attach,uint8 y);

uint8 _tcslen(uint16* pStr);
void DisplayLineUnicode(void);
void	DisplayLine(uint16* unicode, uint8 x,uint8 y, DISPLAY_MODE mode);
uint8 CalculateCallerIDLen(uint8* CallerID, uint8 Len);
void CallerIDUtf8ToUnicode(uint8*ut,uint8 Len,uint16* uc);

void WriteFontBuf(uint8* Fname, uint8 Flen);
void ClearFontBuf(void);
void DisplayFontBuf(void);
void DevComWriteFontBuf(uint8* Fname, uint8 Flen);
void DevComClearFontBuf(void);
void DevComDisplayFontBuf(void);
void DisplayFontLineUnicode(void);
#endif 

