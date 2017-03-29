#ifndef __FONTLIB_H__
#define __FONTLIB_H__

#include "Typedef.h"


#define FONT_LARTIN 
#define FONT_RUSSION 
#define FONT_CHINESE
#define FONT_JAPANESE
#define FONT_THAI
#define FONT_KOREAN


#define EXT_SPI_FLASH_OFFSET 				0x00000000UL

#define DOT_ASCII_LIB_START        			0x00000000UL
#define DOT_ASCII_LIB_END       				0x000006f0UL

#ifdef FONT_LARTIN
#define DOT_LARTIN_LIB_START        			0x00000700UL
#define DOT_LARTIN_LIB_END      				0x00001470UL
#endif

#ifdef FONT_RUSSION
#define DOT_RUSSION_LIB_PART1_START       	0x00001480UL
#define DOT_RUSSION_LIB_PART2_START     	0x00001900UL
#define DOT_RUSSION_LIB_END       			0x00001970UL
#endif

#ifdef FONT_CHINESE
#define DOT_CN_TABLE_START        			0x00001980UL
#define DOT_CN_LIB_START        				0x00008380UL
#define DOT_CN_LIB_END       					0x0003d170UL
extern const uint16 Gb2312_AddrTable[83];
#endif

#ifdef FONT_JAPANESE
#define DOT_JP_LIB_PART1_START       			0x0003d180UL
#define DOT_JP_LIB_PART2_START     			0x0003e960UL
#define DOT_JP_LIB_END       					0x0003eb70UL
#endif

#ifdef FONT_THAI
#define DOT_TA_LIB_START        				0x0003eb80UL
#define DOT_TA_LIB_END       					0x0003fb70UL
#endif

#ifdef FONT_KOREAN
#define DOT_KO_LIB_PART1_START   			0x0003fb80UL
#define DOT_KO_LIB_PART2_START     			0x0003fda0UL
#define DOT_KO_LIB_PART3_START     			0x000409a0UL
#define DOT_KO_LIB_END       					0x00097ff0UL
#endif

#endif 

