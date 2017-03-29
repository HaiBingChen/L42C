
#include "display_font.h"
#include "ExtSpiFlash.h"
#include "fsl_debug_console.h"
#include <string.h>
#include "fsl_pit_driver.h"
#include "display_ancs.h"

uint8_t PhoneCallID[64]={0x00};
uint8_t PhoneCallNumber[32]={0x00};

uint8_t OtaFontType =0; /*可OTA的类型*/
SIZE_FONT	      			SizeFont;

/*滚动参数*/
LINE_ROLL_PARAMETER		LineRollParameter;
uint8_t DisplayRoll =0;
uint8_t DisplayRollPeriod =TIMER_PERIOD_ROLL;
uint8_t FlashDisable =0;
static bool StartTimer =false;
uint8_t	SpecialFrenchFont =1;
bool IsAllChineseTran  = false;

#define LAST_LETTER_CHINESE_SIMPLIFID_ADRR 			0X2EF93C
#define LAST_LETTER_CHINESE_TANDITIONAL_ADRR 		0X315FE4
#define LAST_LETTER_KOREAN_ADRR 					0X33CC3C

#define LAST_LETTER_CHINESE_SIMPLIFID_CRC			0X1D3A
#define LAST_LETTER_CHINESE_TANDITIONAL_CRC		0XC9B4
#define LAST_LETTER_KOREAN_CRC 					0X1D55
bool IsNormalOta = true;
uint8_t FailOta_OtaFontType = LAN_CHINESE_SIMPLIFID;


const uint16_t Gb2312_AddrTable[83]={	
/*0x4Exx*/0x0000,/*0x4Fxx*/0x0258,	
/*0x50xx*/0x0438,/*0x51xx*/0x0530,/*0x52xx*/0x06c4,/*0x53xx*/0x0848,/*0x54xx*/0x0a44,/*0x55xx*/0x0c94,/*0x56xx*/0x0e40,/*0x57xx*/0x0f68,
/*0x58xx*/0x10f8,/*0x59xx*/0x11cc,/*0x5Axx*/0x1360,/*0x5Bxx*/0x145c,/*0x5Cxx*/0x15e4,/*0x5Dxx*/0x1768,/*0x5Exx*/0x1848,/*0x5Fxx*/0x19c4,
/*0x60xx*/0x1b7c,/*0x61xx*/0x1d54,/*0x62xx*/0x1e20,/*0x63xx*/0x2040,/*0x64xx*/0x21ec,/*0x65xx*/0x2314,/*0x66xx*/0x2474,/*0x67xx*/0x25b4,
/*0x68xx*/0x2774,/*0x69xx*/0x28e0,/*0x6Axx*/0x29e8,/*0x6Bxx*/0x2a6c,/*0x6Cxx*/0x2b88,/*0x6Dxx*/0x2d98,/*0x6Exx*/0x2f58,/*0x6Fxx*/0x30a8,
/*0x70xx*/0x3188,/*0x71xx*/0x3294,/*0x72xx*/0x3364,/*0x73xx*/0x34bc,/*0x74xx*/0x35e0,/*0x75xx*/0x36dc,/*0x76xx*/0x3894,/*0x77xx*/0x39f8,
/*0x78xx*/0x3b48,/*0x79xx*/0x3cac,/*0x7Axx*/0x3dcc,/*0x7Bxx*/0x3ee4,/*0x7Cxx*/0x4058,/*0x7Dxx*/0x4144,/*0x7Exx*/0x416c,/*0x7Fxx*/0x42e4,
/*0x80xx*/0x44b4,/*0x81xx*/0x46a4,/*0x82xx*/0x47f8,/*0x83xx*/0x49f0,/*0x84xx*/0x4bd0,/*0x85xx*/0x4d0c,/*0x86xx*/0x4dec,/*0x87xx*/0x4f30,
/*0x88xx*/0x5070,/*0x89xx*/0x51a4,/*0x8Axx*/0x5270,/*0x8Bxx*/0x5290,/*0x8Cxx*/0x5408,/*0x8Dxx*/0x5538,/*0x8Exx*/0x572c,/*0x8Fxx*/0x5814,
/*0x90xx*/0x599c,/*0x91xx*/0x5b6c,/*0x92xx*/0x5c98,/*0x93xx*/0x5ca8,/*0x94xx*/0x5cbc,/*0x95xx*/0x5e88,/*0x96xx*/0x6088,/*0x97xx*/0x624c,
/*0x98xx*/0x633c,/*0x99xx*/0x6420,/*0x9Axx*/0x64e8,/*0x9Bxx*/0x6618,/*0x9Cxx*/0x6670,/*0x9Dxx*/0x678c,/*0x9Exx*/0x678c,/*0x9Fxx*/0x691c,
/*0xA0xx*/0x69ac
};

const uint8_t DunHao[48] ={
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x06,0x06,0x0C,0x0C,0x00,0x00,0x00,0x00,0x00,0x00
};
const uint8_t NumberMapRes8x16[][16]=
{
//0/
{
	0x00,0xFC,0x02,0x02,0x02,0x02,0xFC,0x00,0x00,0x07,0x08,0x08,0x08,0x08,0x07,0x00
},
// 1/
{
	0x00,0x00,0x00,0x04,0xFE,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x0F,0x00,0x00,0x00
},
// 2/
{
	0x00,0x82,0x42,0x42,0x42,0x42,0x3C,0x00,0x00,0x07,0x08,0x08,0x08,0x08,0x08,0x00
},
// 3/
{
	0x00,0x42,0x42,0x42,0x42,0x42,0xFC,0x00,0x00,0x08,0x08,0x08,0x08,0x08,0x07,0x00
},
// 4/
{
	0x00,0x3E,0x40,0x40,0x40,0x40,0xFE,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x0F,0x00
},
// 5/
{
	0x00,0x3C,0x42,0x42,0x42,0x42,0x80,0x00,0x00,0x00,0x08,0x08,0x08,0x08,0x07,0x00
},
// 6/
{
	0x00,0xFC,0x42,0x42,0x42,0x42,0x80,0x00,0x00,0x07,0x08,0x08,0x08,0x08,0x07,0x00
},
// 7/
{
	0x00,0x02,0x02,0x02,0x02,0x02,0xFC,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x0F,0x00
},
// 8/
{
	0x00,0xFC,0x42,0x42,0x42,0x42,0xFC,0x00,0x00,0x07,0x08,0x08,0x08,0x08,0x07,0x00
},
// 9/
{
	0x00,0x3C,0x42,0x42,0x42,0x42,0xFC,0x00,0x00,0x00,0x08,0x08,0x08,0x08,0x07,0x00
},
// -/
{
	0x80,0x80,0x80,0x80,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
},
// :/
{
	0x00,0x00,0xA0,0x00,0x00,0x00,0x00,0x00
},
// +/
{
       0x00,0x40,0x40,0xf0,0x40,0x40,0x00,0x00,0x00,0x00,0x00,0x01,0x00,0x00,0x00,0x00
},
// ./
{
       0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x08,0x00,0x00,0x00,0x00
},
//
{
	0x00,0x00,0x00,0xc0,0x38,0x06,0x00,0x00,0x00,0x00,0x07,0x00,0x00,0x00,0x00,0x00
}
};

void DisplayStringNumber8x16(uint8_t * str, uint16_t color,uint8_t x, uint8_t y)
{
	uint16_t StringLen=0;
	uint16_t posx = x, posy = y;
	if(str != NULL) {
		StringLen=strlen((char*)str);  
		for(uint8_t i=0;i<StringLen;i++)
		{
			if(str[i]=='.')
			{
				DisplayDotFont(&NumberMapRes8x16[13][0], color,posx, posy, 8, 16);
				posx += 8;
			}
			if(str[i]=='-')
			{
				DisplayDotFont(&NumberMapRes8x16[10][0], color,posx, posy, 6, 16);
				posx += 6;
			}
			else if(str[i]==':')
			{
				DisplayDotFont(&NumberMapRes8x16[11][0], color,posx, posy, 4, 16); 
				posx += 4;
			}
			else if(str[i]=='/')
			{
				DisplayDotFont(&NumberMapRes8x16[14][0], color,posx, posy, 8, 16); 
				posx += 8;
			}
			else if(str[i]=='!')
			{
				DisplayDotFont(&NumberMapRes8x16[13][0], color,posx, posy, 8, 16); 
				posx += 8;
			}
			else if(str[i]>='0' && str[i]<='9')
			{
				DisplayDotFont(&NumberMapRes8x16[str[i]-'0'][0], color,posx, posy, 8, 16); 
				posx += 8;
			}
			if(posx+8>LCD_SIZE_W)
				break;
		}
	}
}


#if defined (NEW_CHAR_LIB)	
#define EXT_SPI_FLASH_OFFSET 				0x00000000UL
#define DOT_LIB_START						0x00272000
#define DOT_ASCII_LIB_START				(DOT_LIB_START+0x000DBD00UL)
#define DOT_CN_TABLE_START        			(DOT_LIB_START+0x00000004UL)
#define DOT_CN_LIB_START        				(DOT_LIB_START+0x00006a04UL)

#define DOT_KO_LIB_PART1_START        		(DOT_LIB_START+0x00000004UL)
#define DOT_KO_LIB_PART2_START        		(DOT_LIB_START+0x00004804UL)
#define DOT_KO_LIB_PART3_START        		(DOT_LIB_START+0x00006304UL)

#define DOT_RUSSION_LIB_START        		(DOT_LIB_START+0x000DBD00UL+0x00004800UL)
#define DOT_THAI_LIB_START        			(DOT_LIB_START+0x000DBD00UL+0x00008100UL)
#define DOT_JAPANESE_LIB_START        		(DOT_LIB_START+0x000DBD00UL+0x0000A500UL)

static const FONT_CMD_T FontCmdTable[]={
	{0x01,	DisplayFont_1},      //字母
	{0x02,	DisplayFont_2},		//简体中文
	{0x03,	DisplayFont_3},		//繁体中文
	{0x04,	DisplayFont_4},     //韩文
	{0x05,	DisplayFont_5},     //俄文
	{0x06,	DisplayFont_6},     //泰文
	{0x07,	DisplayFont_7},     //日文
};
const uint16_t FontCmdTableLen=sizeof(FontCmdTable)/sizeof(FONT_CMD_T);

uint8_t DisplayFont_1(uint16_t Unicode,uint16_t color,uint8_t x,uint8_t y)
{
	uint8_t Dot[48]={0x00};
	uint32_t addr=0;
	if(Unicode>=0x0000&&Unicode<=0x017F)
	{
		addr = DOT_ASCII_LIB_START + 0x30UL*(uint32_t)Unicode;
	}
	else if (Unicode ==0x3001)
	{
		DisplayDotFont(DunHao,color,x,y,16,24);
		return SpecialFrenchFont==1?13:16;
	}
	if(addr>0)
		SpiFlash_Read(EXT_SPI_FLASH_OFFSET+addr,Dot,48);
	DisplayDotFont(Dot,color,x,y,16,24);
	IsAllChineseTran  = false;
	return SpecialFrenchFont==1?13:16;
}

uint8_t DisplayFont_2(uint16_t Unicode,uint16_t color,uint8_t x,uint8_t y)
{
	uint8_t Dot[72]={0x00};
	uint32_t addr =0;
	uint8_t index0=0,index1=0,buf=0,gb[2];
	uint16_t len =0,GB2312=0;	
	index0=((Unicode>>8)&0xff)-0x4e;
	index1=Unicode&0xff;	
	bool IsChineseSym = false;
	len = (Gb2312_AddrTable[index0+1]-Gb2312_AddrTable[index0])/4;
	if(len>0)
	{
		for(int16_t i=0;i<len;i++)
		{
			addr=EXT_SPI_FLASH_OFFSET+DOT_CN_TABLE_START+Gb2312_AddrTable[index0]+i*4;
			SpiFlash_Read(addr,&buf,1);
			if(buf==index1)
			{
				SpiFlash_Read(addr+2,gb,2);
				GB2312=(gb[1]<<8)|gb[0];
				if((GB2312>=0xB0A1) && (GB2312<=0xF7FE))
				{
					gb[1]-=0xB0;
					gb[0]-=0xA1;
					addr=DOT_CN_LIB_START+(0x1A70UL*(uint32_t)gb[1])+(0x48UL*(uint32_t)gb[0]);
				}
				if(addr>0)
				{
					IsChineseSym = true;
					SpiFlash_Read(EXT_SPI_FLASH_OFFSET+addr,Dot,72);
				}
				break;
			}					
		}
	}
	if(IsChineseSym == false)
	{
		IsAllChineseTran  = true;
	}
	DisplayDotFont(Dot,color,x,y,24,24);
	return 24;
}

uint8_t DisplayFont_3(uint16_t Unicode,uint16_t color,uint8_t x,uint8_t y)
{
	uint8_t Dot[32]={0x00};
	uint32_t addr=0;
	if((Unicode>=0x4E00)&&(Unicode<=0x9FFF))
	{
		Unicode -= 0x4E00;
		addr = DOT_CN_TABLE_START + 0x20UL*(uint32_t)Unicode;
	}
	if(addr>0)
		SpiFlash_Read(EXT_SPI_FLASH_OFFSET+addr,Dot,32);
	DisplayDotFont(Dot,color,x,y+6,16,16);
	IsAllChineseTran  = false;
	return 16;
}

uint8_t DisplayFont_4(uint16_t Unicode,uint16_t color,uint8_t x,uint8_t y)
{
	uint8_t Dot[72]={0x00};
	uint32_t addr=0;
	if((Unicode>=0x1100)&&(Unicode<=0x11ff))
	{
		Unicode -= 0x1100;
		addr = DOT_KO_LIB_PART1_START + 0x48UL*(uint32_t)Unicode;
	}
	else if((Unicode>=0x3130)&&(Unicode<=0x318f))
	{
		Unicode -= 0x3130;
		addr = DOT_KO_LIB_PART2_START + 0x48UL*(uint32_t)Unicode;
	}
	else if((Unicode>=0xAC00)&&(Unicode<=0xD7AF))
	{
		Unicode -= 0xAC00;
		addr = DOT_KO_LIB_PART3_START + 0x48UL*(uint32_t)Unicode;
	}
	if(addr>0)
		SpiFlash_Read(EXT_SPI_FLASH_OFFSET+addr,Dot,72);
	DisplayDotFont(Dot,color,x,y,24,24);	
	IsAllChineseTran  = false;
	return 24;
}

uint8_t DisplayFont_5(uint16_t Unicode,uint16_t color,uint8_t x,uint8_t y)
{
	uint8_t Dot[48]={0x00};
	uint32_t addr=0;
	if(Unicode==0x0450)//此俄文字符由于字库原因显示不全，用法文的字符显示
	{
			Unicode = 0x00e8;

			if(Unicode>=0x0000&&Unicode<=0x017F)
			{
				addr = DOT_ASCII_LIB_START + 0x30UL*(uint32_t)Unicode;
			}
			if(addr>0)
				SpiFlash_Read(EXT_SPI_FLASH_OFFSET+addr,Dot,48);
			DisplayDotFont(Dot,color,x,y,16,24);
			IsAllChineseTran  = false;
			return SpecialFrenchFont==1?13:16;
	}
	else
	{
		if((Unicode>=0x0400)&&(Unicode<=0x052F))
		{
			Unicode -= 0x0400;
			addr = DOT_RUSSION_LIB_START + 0x30UL*(uint32_t)Unicode;				
		}
			
		if(addr>0)
			SpiFlash_Read(EXT_SPI_FLASH_OFFSET+addr,Dot,48);
		DisplayDotFont(Dot,color,x,y,16,24);
		IsAllChineseTran  = false;
		return 16;
	}	
}

uint8_t DisplayFont_6(uint16_t Unicode,uint16_t color,uint8_t x,uint8_t y)
{
	uint8_t Dot[72]={0x00};
	uint32_t addr=0;
	if((Unicode>=0x0E00)&&(Unicode<=0x0E7F))
	{
		Unicode -= 0x0E00;
		addr = DOT_THAI_LIB_START + 0x48UL*(uint32_t)Unicode;
	}
	if(addr>0)
		SpiFlash_Read(EXT_SPI_FLASH_OFFSET+addr,Dot,72);
	DisplayDotFont(Dot,color,x,y,24,24);	
	IsAllChineseTran  = false;
	return 24;
}

uint8_t DisplayFont_7(uint16_t Unicode,uint16_t color,uint8_t x,uint8_t y)
{
	uint8_t Dot[72]={0x00};
	uint32_t addr=0;
	if((Unicode>=0x3040)&&(Unicode<=0x31FF))
	{
		Unicode -= 0x3040;
		addr = DOT_JAPANESE_LIB_START + 0x48UL*(uint32_t)Unicode;
	}
	if(addr>0)
		SpiFlash_Read(EXT_SPI_FLASH_OFFSET+addr,Dot,72);
	DisplayDotFont(Dot,color,x,y,24,24);
	IsAllChineseTran  = false;
	return 24;
}

uint8_t IsFontSupport(uint16_t* Ucode, uint8_t* CmdType)
{
	uint16_t Unicode = *Ucode;
	if(Unicode>=0x0000&&Unicode<=0x017F)	 		//Ascii Lartin
	{
		*CmdType =0x01;
		return SpecialFrenchFont==1?13:16;
	} 
	else if((OtaFontType==0x01)&&Unicode>=0x4E00&&Unicode<=0x9FBF && (IsNormalOta == true))		//Chinese  JianTi
	{
		*CmdType =0x02;
		return 24;
	} 
	else if((OtaFontType==0x02)&&Unicode>=0x4E00&&Unicode<=0x9FFF && (IsNormalOta == true))		//Chinese  FanTi
	{
		*CmdType =0x03;
		return 16;
	}  
	else if((OtaFontType==0x03)&&(((Unicode>=0x1100)&&(Unicode<=0x11FF))||((Unicode>=0x3130)&&(Unicode<=0x318F))
		||((Unicode>=0xAC00)&&(Unicode<=0xD7AF))) && (IsNormalOta == true))		//Korean
	{
		*CmdType =0x04;
		return 24;
	}
	else if((Unicode>=0x0400)&&(Unicode<=0x052F))		//Russion
	{
		*CmdType =0x05;
		return 16;
	}
	else if((Unicode>=0x0E00)&&(Unicode<=0x0E7F))		//Thai
	{
		*CmdType =0x06;
		return 24;
	}  
	else if((Unicode>=0x3040)&&(Unicode<=0x31FF))		//Japanese
	{
		*CmdType =0x07;
		return 24;
	}  
	else
	{
		*Ucode=UnknownUnicodeConversion(Unicode);
		if(*Ucode==0x003F)
			*CmdType =UNKNOWN_NUMBER;
		else
			*CmdType =0x01;
		return 16;
	}
}
uint8_t GetWidth(uint8_t Bl)
{	
	if(Bl==0)
		return SpecialFrenchFont==1?13:16;
	else
		return 24;
}
void Set_SizeFont(uint8_t x,uint8_t y, uint8_t w,uint8_t h)
{
	SizeFont.X = x;
	SizeFont.Y = y;
	SizeFont.MaxW= x+w;
	SizeFont.MaxH = 24;
}

#else

#define EXT_SPI_FLASH_OFFSET 				0x00000000UL
#define DOT_ASCII_LIB_START				0x00000000UL
#define DOT_LARTIN_LIB_START        			0x00000700UL
#define DOT_RUSSION_LIB_PART1_START       	0x00001480UL
#define DOT_RUSSION_LIB_PART2_START     	0x00001900UL
#define DOT_CN_TABLE_START        			0x00001980UL
#define DOT_CN_LIB_START        				0x00008380UL
#define DOT_JP_LIB_PART1_START       			0x0003d180UL
#define DOT_JP_LIB_PART2_START     			0x0003e960UL
#define DOT_TA_LIB_START        				0x0003eb80UL
#define DOT_KO_LIB_PART1_START   			0x0003fb80UL
#define DOT_KO_LIB_PART2_START     			0x0003fda0UL
#define DOT_KO_LIB_PART3_START     			0x000409a0UL

static const FONT_CMD_T FontCmdTable[]={
	{0x01,	DisplayFont_1},
	{0x02,	DisplayFont_2},
	{0x03,	DisplayFont_3},
	{0x04,	DisplayFont_4},
};

const uint16_t FontCmdTableLen=sizeof(FontCmdTable)/sizeof(FONT_CMD_T);

uint8_t DisplayFont_1(uint16_t Unicode,uint16_t Unitcode_attach,uint16_t color,uint8_t x,uint8_t y)
{
	uint8_t Dot[18]={0x00};
	uint32_t addr=0;
	if(Unicode>=0x0020&&Unicode<=0x007E)
	{
		Unicode-=0x0020;
		addr = DOT_ASCII_LIB_START + 0x12UL*(uint32_t)Unicode;
	}
	else if(Unicode>=0x00C0&&Unicode<=0x017F)
	{
		Unicode-=0x00C0;
		addr = DOT_LARTIN_LIB_START + 0x12UL*(uint32_t)Unicode;
	}
	if(addr>0)
		SpiFlash_Read(EXT_SPI_FLASH_OFFSET+addr,Dot,18);
	DisplayDotFont(Dot,color,x,y,9,16);	
	return 9;
}

uint8_t DisplayFont_2(uint16_t Unicode,uint16_t Unitcode_attach,uint16_t color,uint8_t x,uint8_t y)
{
	uint8_t Dot[18]={0x00};
	uint32_t addr =0;
	if (Unicode== 0x0415 && Unitcode_attach == 0x0308) 
		addr = DOT_RUSSION_LIB_PART2_START;
	else if (Unicode == 0x0418 && Unitcode_attach== 0x0306) 
		addr = DOT_RUSSION_LIB_PART2_START+ 0x12UL;
	else if (Unicode== 0x0419 && Unitcode_attach== 0x0306) 
		addr = DOT_RUSSION_LIB_PART2_START+ 0x12UL*2;
	else if (Unicode== 0x0435 && Unitcode_attach== 0x0308) 
		addr = DOT_RUSSION_LIB_PART2_START+ 0x12UL*3;
	else if (Unicode== 0x0438 && Unitcode_attach == 0x0306) 
		addr = DOT_RUSSION_LIB_PART2_START+ 0x12UL*4;
	else if (Unicode== 0x0439 && Unitcode_attach == 0x0306) 
		addr = DOT_RUSSION_LIB_PART2_START+ 0x12UL*5;
	else if (Unicode== 0x039B) 
		addr = DOT_RUSSION_LIB_PART2_START+ 0x12UL*6;
	else
	{
		Unicode-=0x0410;
		addr = DOT_RUSSION_LIB_PART1_START + 0x12UL*(uint32_t)Unicode;
	}
	if(addr>0)
		SpiFlash_Read(EXT_SPI_FLASH_OFFSET+addr,Dot,18);	
	DisplayDotFont(Dot,color,x,y,9,16);		
	return 9;
}

uint8_t DisplayFont_3(uint16_t Unicode,uint16_t Unitcode_attach,uint16_t color,uint8_t x,uint8_t y)
{
	uint8_t Dot[32]={0x00};
	uint32_t addr =0;
	uint8_t index0=0,index1=0,buf=0,gb[2];
	uint16_t len =0,GB2312=0;	
	index0=((Unicode>>8)&0xff)-0x4e;
	index1=Unicode&0xff;	
	len = (Gb2312_AddrTable[index0+1]-Gb2312_AddrTable[index0])/4;
	if(len>0)
	{
		for(int16_t i=0;i<len;i++)
		{
			addr=EXT_SPI_FLASH_OFFSET+DOT_CN_TABLE_START+Gb2312_AddrTable[index0]+i*4;
			SpiFlash_Read(addr,&buf,1);
			if(buf==index1)
			{
				SpiFlash_Read(addr+2,gb,2);
				GB2312=(gb[1]<<8)|gb[0];
				if((GB2312>=0xB0A1) && (GB2312<=0xF7FE))
				{
					gb[1]-=0xB0;
					gb[0]-=0xA1;
					addr=DOT_CN_LIB_START+(0xBC0UL*(uint32_t)gb[1])+(0x20UL*(uint32_t)gb[0]);
				}
				if(addr>0)
					SpiFlash_Read(EXT_SPI_FLASH_OFFSET+addr,Dot,32);
				break;
			}					
		}
	}	
	DisplayDotFont(Dot,color,x,y,16,16);			
	return 16;	
}

uint8_t DisplayFont_4(uint16_t Unicode,uint16_t Unitcode_attach,uint16_t color,uint8_t x,uint8_t y)
{
	uint8_t Dot[32]={0x00};
	uint32_t addr =0;	
	if((Unicode>=0x3041)&&(Unicode<=0x30FF))
	{
		Unicode -= 0x3041;
		addr = DOT_JP_LIB_PART1_START + 0x20UL*(uint32_t)Unicode;
	}	
	else if((Unicode>=0x31F0)&&(Unicode<=0x31FF))
	{
		Unicode -= 0x31F0;
		addr = DOT_JP_LIB_PART2_START + 0x20UL*(uint32_t)Unicode;
	}
	else if((Unicode>=0x0E00)&&(Unicode<=0x0E7F))
	{
		Unicode -= 0x0E00;
		addr = DOT_TA_LIB_START + 0x20UL*(uint32_t)Unicode;
	}
	else if((Unicode>=0x1100)&&(Unicode<=0x11ff))
	{
		Unicode -= 0x1100;
		addr = DOT_KO_LIB_PART1_START + 0x20UL*(uint32_t)Unicode;
	}
	else if((Unicode>=0x3130)&&(Unicode<=0x318f))
	{
		Unicode -= 0x3130;
		addr = DOT_KO_LIB_PART2_START + 0x20UL*(uint32_t)Unicode;
	}
	else if((Unicode>=0xAC00)&&(Unicode<=0xD7AF))
	{
		Unicode -= 0xAC00;
		addr = DOT_KO_LIB_PART3_START + 0x20UL*(uint32_t)Unicode;
	}	
	if(addr>0)
		SpiFlash_Read(EXT_SPI_FLASH_OFFSET+addr,Dot,32);	
	DisplayDotFont(Dot,color,x,y,16,16);			
	return 16;	
}

uint8_t IsFontSupport(uint16_t* Ucode, uint8_t* CmdType)
{
	uint16_t Unicode = *Ucode;
	if((Unicode>=0x0020&&Unicode<=0x007E) || (Unicode>=0x00C0)&&(Unicode<=0x017F)) 	 	//Ascii Lartin
	{
		*CmdType =0x01;
		return 9;
	}
	else if((Unicode>=0x0410&&Unicode<=0x044F) || (Unicode==0x039B))					//Russion
	{
		*CmdType =0x02;
		return 9;		
	}
	else if(Unicode>=0x4E00&&Unicode<=0x9FBF)											//Chinese
	{
		*CmdType =0x03;
		return 16;
	}
	else if((Unicode>=0x3041&&Unicode<=0x30FF)||(Unicode>=0x31F0&&Unicode<=0x31FF)||(Unicode>=0x0E00&&Unicode<=0x0E7F)
		||(Unicode>=0x1100&&Unicode<=0x11FF)||(Unicode>=0x3130&&Unicode<=0x318F)||(Unicode>=0xAC00&&Unicode<=0xD7AF))		//japanese thai korean
	{
		*CmdType =0x04;
		return 16;
	}	
	else
	{
		*Ucode=UnknownUnicodeConversion(Unicode);
		if(*Ucode==0x003F)
			*CmdType =UNKNOWN_NUMBER;
		else
			*CmdType =0x01;
		return 9;
	}
}
uint8_t GetWidth(uint8_t Bl)
{	
	if(Bl==0)
		return 9;
	else
		return 16;
}
void Set_SizeFont(uint8_t x,uint8_t y, uint8_t w,uint8_t h)
{
	SizeFont.X = x;
	SizeFont.Y = y;
	SizeFont.MaxW= x+w;
	SizeFont.MaxH = h;
}

#endif

uint8_t DisplayFont(uint8_t cmd,uint16_t Unicode,uint16_t Unitcode_attach,uint16_t color,uint8_t x,uint8_t y)
{
	if(cmd==UNKNOWN_NUMBER)
		cmd=0x01;
	for(uint8_t i=0;i<FontCmdTableLen;i++)
	{
		if(cmd==FontCmdTable[i].Cmd)
		{
			return FontCmdTable[i].pFn(Unicode,color,x,y);
		}
	}
	return 0;
}

uint16_t crc16_compute(const uint8_t * p_data, uint32_t size)
{
    uint32_t i;
    uint16_t crc = 0xffff;
    for (i = 0; i < size; i++)
    {
        crc  = (unsigned char)(crc >> 8) | (crc << 8);
        crc ^= p_data[i];
        crc ^= (unsigned char)(crc & 0xff) >> 4;
        crc ^= (crc << 8) << 4;
        crc ^= ((crc & 0xff) << 4) << 1;
    }
    return crc;
}


bool InitDisplayFont(uint8_t* Ftype)
{
	uint8_t Un_Buffer[72]={0x00};
	uint8_t len = 72;
	uint16_t LastLetter_Crc = 0;
	memset(Un_Buffer,0,len);
	OtaFontType = Ftype[0];
	LanNow = (LanguageType)Ftype[1];
	if(OtaFontType == LAN_CHINESE_SIMPLIFID)
	{
		len =72;
		SpiFlash_Read(LAST_LETTER_CHINESE_SIMPLIFID_ADRR,Un_Buffer,len);
		LastLetter_Crc = crc16_compute(Un_Buffer,len);	
		if(LastLetter_Crc != LAST_LETTER_CHINESE_SIMPLIFID_CRC)
		{
			FailOta_OtaFontType = LAN_CHINESE_SIMPLIFID;
			IsNormalOta = false;
		}
	}
	else if(OtaFontType == LAN_CHINESE_TRADITIONAL)
	{
		len = 32;
		SpiFlash_Read(LAST_LETTER_CHINESE_TANDITIONAL_ADRR,Un_Buffer,len);
		LastLetter_Crc = crc16_compute(Un_Buffer,len);
		if(LastLetter_Crc != LAST_LETTER_CHINESE_TANDITIONAL_CRC)
		{
			FailOta_OtaFontType = LAN_CHINESE_TRADITIONAL;
			IsNormalOta = false;
		}
		
	}
	else if(OtaFontType == LAN_KOREAN)
	{
		len = 72;
		SpiFlash_Read(LAST_LETTER_KOREAN_ADRR,Un_Buffer,len);
		LastLetter_Crc = crc16_compute(Un_Buffer,len);
		if(LastLetter_Crc != LAST_LETTER_KOREAN_CRC)
		{
			FailOta_OtaFontType = LAN_KOREAN;
			IsNormalOta = false;
		};
	}
	else
	{
		IsNormalOta = false;
	}
	if((LanNow==LAN_ENGLISH)||((LanNow>=LAN_THAI)&&(LanNow<=LAN_DUTCH))){
		;
	}else if(LanNow!=OtaFontType){
		LanNow=LAN_CHINESE_SIMPLIFID;
	}
	
	if(IsNormalOta == false)
		LanNow=LAN_ENGLISH;
	
	PRINTF("LanNow = 0x%x\r\n",LanNow);
	PRINTF("IsNormalOta = 0x%x\r\n",IsNormalOta);
	return IsNormalOta;
}

uint16_t UnknownUnicodeConversion(uint16_t c)
{
        switch(c)
        {
                case 0x3002:           /*  句号        。*/
                    c = 0x002E;
                    break;
                case 0xFF1F:            /*  问号        ？*/
                    c = 0x003F;
                    break;
                case 0xFF01:            /*  感叹号  ！*/
                    c = 0x0021;
                    break;
                case 0xFF0C:            /*  逗号       ,    */
                    c = 0x002c;
                    break; 
                case 0x3001:            /*  顿号        、*/
                    c = 0x3001;
                    break;
                case 0xFF1B:            /*  分号       ；*/
                    c = 0x003B;
                    break;
                case 0xFF1A:            /*  冒号      ： */
                    c = 0x003a;
                    break;
                case 0x2018:            /*  单引号  ‘*/
                    c = 0x0027;
                    break;
                case 0x2019:           /*  单引号   ’*/
                    c = 0x0027;
                    break;
                case 0xFF02:            /*  双引号  “”*/
                    c = 0x0022;
                    break; 
                case 0xFF08:            /*  小括号  （*/
                    c = 0x0028;
                    break;
                case 0xFF09:           /*  小括号  ） */
                    c = 0x0029;
                    break;
                case 0x3010:            /*  中括号  【*/
                    c = 0x005B;
                    break;
                case 0x3011:            /*  中括号  】*/
                    c = 0x005D;
                    break;
                case 0x3014:            /*  圆括号 〔 */
                    c = 0x005B;
                    break;
                case 0x3015:            /*  圆括号  〕*/
                    c = 0x005D;
                    break;
                case 0xFF5B:            /*  大括号  ｛*/
                    c = 0x007B;
                    break;
                case 0xFF5D:            /*  大括号   ｝*/
                    c = 0x007D;
                    break;
                case 0x2014:           /*  破折号   ——*/
                    c = 0x002D;
                    break;
                case 0x2026:            /*  省略号   ……*/
                    break;
                case 0x2013:            /*  连接号   -   */
                    c = 0x002D;
                    break;
                case 0x300A:            /* 书名号   《*/
                    c = 0x003C;
                    break;
                case 0x300B:            /*  书名号    》*/
                    c = 0x003E;
                    break;               
                default:
		      c = 0x003F;
                    break;                 
        }       
        return c;        
}

uint8_t CalculateAncsLen(uint8_t* dat, uint8_t Len)
{
	uint8_t utf8Del=0;
	if(Len==0)	 
		Len = strlen((char*)dat);	 
	if(Len>2)
	{
		for(uint8_t i=0;i<Len;i++)
		{
			if((dat[i] & 0xf0 )== 0xc0||(dat[i] & 0xf0 )== 0xd0)	//法文 //俄文
			{	
				if(i<(Len-1))
					i+=1;
				else{
					utf8Del =1;
					break;
				}
			}
			else if((dat[i] & 0xf0 )== 0xe0)						//中日韩泰文	
			{	
				if(i<(Len-2))
					i+=2;
				else{
					utf8Del =(Len-i);
					break;
				}
			}
		}
	}
	return utf8Del;
}

/***************************************************************************
****|  Unicode符号范围      					|  UTF-8编码方式  
****|  (十六进制)           					| (二进制)  
----+-----------------------------------+-----------------------------------
 1 | 0000 0000 - 0000 007F 	|                                              0xxxxxxx  
 2 | 0000 0080 - 0000 07FF 	|                                     110xxxxx 10xxxxxx  
 3 | 0000 0800 - 0000 FFFF 	|                            1110xxxx 10xxxxxx 10xxxxxx  
 4 | 0001 0000 - 0010 FFFF 	|                   11110xxx 10xxxxxx 10xxxxxx 10xxxxxx  
 5 | 0020 0000 - 03FF FFFF 	|          111110xx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx  
 6 | 0400 0000 - 7FFF FFFF 	| 1111110x 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx  
 
****************************************************************************/
static RETURN_TYPE Utf8ToUnitcode(uint8_t*utf8,uint16_t*unitcode,uint16_t*attach)
{
	uint8_t len = strlen((char*)utf8);
	RETURN_TYPE Rt;
	uint16_t unitcodeTemp=0,attachTemp=0;
	Rt.Len=0;
	Rt.Width=0;
	if((utf8[0] & 0x80) ==0)			/*ASCII码 英*/
	{
		if(len>=1)
		{
			unitcodeTemp=utf8[0];
			Rt.Width= GetWidth(0);
			Rt.Len = 1;
		}
	}
	else if((utf8[0] & 0xf0)==0xc0)		/*法*/
	{
		if(len>=2)
		{
			unitcodeTemp=((((utf8[0] & 0x1f) >>2) &0x07)<<8)+(((utf8[0] & 0x03) <<6) + (utf8[1]& 0x3f));	
			Rt.Width= GetWidth(0);
			Rt.Len= 2;
		}
	}		
	else if((utf8[0] & 0xf0)==0xd0)		/*俄*/	
	{
		if(len>=2)
		{
			unitcodeTemp =((((utf8[0] & 0x1f) >>2) &0x07)<<8)+(((utf8[0] & 0x03) <<6) + (utf8[1]& 0x3f));
	
			Rt.Width=  GetWidth(0);
			Rt.Len= 2;			
		}
		if(len>=4)
		{
			attachTemp = ((((utf8[2] & 0x1f) >>2) &0x07)<<8)+(((utf8[2] & 0x03) <<6) + (utf8[3]& 0x3f));
			if(attachTemp==0x0308||attachTemp==0x0308)				
				Rt.Len= 4;	
		}
	}
	else if((utf8[0] & 0xf0)==0xe0)		/*中日泰韩*/
	{
		if(len>=3)
		{
			unitcodeTemp =((((utf8[0] & 0x0f) <<4) + ((utf8[1] & 0x3c) >>2))<<8)+(((utf8[1] & 0x03) <<6) + (utf8[2]& 0x3f));
			if(unitcodeTemp>=0x4E00&&unitcodeTemp<=0x9FFF)
			{
				if(OtaFontType==0x01)
					Rt.Width=24;
				else
					Rt.Width=16;
				Rt.Len= 3;
			} else { 
				Rt.Width= GetWidth(1);
				Rt.Len= 3;
			}
		}
	}
	else if((utf8[0] & 0xf0)==0xf0)		
	{
		if(len>=4)
		{
			unitcodeTemp= 0xffff;
			Rt.Width= GetWidth(0);	
			Rt.Len= 4;		
		}
	}
	if(unitcode!=NULL)
		*unitcode=unitcodeTemp;
	if(attach!=NULL)
		*attach = attachTemp;
	return Rt;
}


void Per_Calculation(uint8_t*RawData,uint8_t* MiddleXpos, uint8_t*DotPos, uint8_t*RollPos)
{
	RETURN_TYPE rt;
	uint8_t DotPosTemp=0,LastDotPosTemp=0;
	uint8_t xw=SizeFont.X;
	uint8_t w = SizeFont.MaxW;
	uint8_t * Pc =RawData;
	uint8_t len = strlen((char*)RawData);
	rt.Len=0;
	rt.Width=0;
	if(MiddleXpos!=NULL)
	{
		for(uint8_t i=0;i<100;i++)
		{
			Pc+=rt.Len;
			rt=Utf8ToUnitcode(Pc,NULL,NULL);
			xw+=rt.Width;
			if(xw>=w)	
			{
				*MiddleXpos =SizeFont.X;
				break;
			}			
			if(rt.Width==0||(Pc-RawData)>=len)
			{
				*MiddleXpos = SizeFont.X+(w-xw)/2;
				break;
			}
		}
		Pc =RawData;
		rt.Len=0;
		rt.Width=0;
		xw=0;
	}

	if(DotPos!=NULL)
	{
		uint8_t DotLen = 21;
		for(uint8_t i=0;i<100;i++)
		{
			LastDotPosTemp = (Pc-RawData);
			Pc+=rt.Len;
			rt=Utf8ToUnitcode(Pc,NULL,NULL);
			xw+=rt.Width;
			if((xw+DotLen)>w)	
			{
				DotPosTemp= LastDotPosTemp;	/*记录Dot位置*/
			}	
			if(xw>=w)
			{
				*DotPos =DotPosTemp;
				break;
			}
			if((rt.Width==0)||((Pc-RawData)>=len))
			{
				break;
			}
		}		
		Pc =RawData;
		rt.Len=0;
		rt.Width=0;
		xw=0;
	}
	
	if(RollPos!=NULL)
	{
		rt=Utf8ToUnitcode(RawData,NULL,NULL);
		*RollPos = rt.Len;
	}
}

bool IsAllUnKnownNumber(uint8_t *dat,uint8_t len)
{
	uint8_t FPos =0,xPos=0,cmdType=0;
	uint16_t FUnicode = 0x00, FAttach=0x00;
	RETURN_TYPE rt;
	bool IsUnKnown=true;
	for(uint8_t i=0;i<200;i++)
	{
		rt = Utf8ToUnitcode(dat+FPos,&FUnicode,&FAttach);
		if(FUnicode!=0x0020)
		{
			IsFontSupport(&FUnicode,&cmdType);
			if(cmdType!=UNKNOWN_NUMBER)
			{
				IsUnKnown=false;	
				break;
			}
		}
		xPos+= rt.Width;
		if(rt.Len==0)
			rt.Len=1;
		FPos+=rt.Len;
		if(FPos>=len)
			break;
	}	
	return IsUnKnown;
}

uint8_t DisplayLine(uint8_t *dat, DISPLAY_MODE dm,uint16_t colour, uint8_t x, uint8_t y, uint8_t w, uint8_t h)
{
	uint8_t FPos =0,xPos=0,xDotPos=0,RollOffSet=0,len =0,cmdType=0;
	uint16_t FUnicode = 0x00, FAttach=0x00;
	RETURN_TYPE rt;
	Set_SizeFont(x,y,w,h);
	rt.Len=0;
	rt.Width=0;
	xPos=SizeFont.X;
	len = strlen((char*)dat);
/*分类:*/
	if((UnknownCheck==1)&&(IsAllUnKnownNumber(dat,len)))
	{
		UnknownCheck =0;
		return UNKNOWN_NUMBER;
	}
	if(dm==D_MODE1||dm==D_MODE3)
		Per_Calculation(dat,&xPos,NULL,NULL);
	if(dm==D_MODE2||dm==D_MODE3)
		Per_Calculation(dat,NULL,NULL,&(RollOffSet));
	if(dm==D_MODE4)
	{
		Per_Calculation(dat,NULL,&(xDotPos),NULL);
		if(xDotPos>0)
			len = xDotPos;
	}
	for(uint8_t i=0;i<200;i++)
	{
		rt = Utf8ToUnitcode(dat+FPos,&FUnicode,&FAttach);
		if(IsFontSupport(&FUnicode,&cmdType)+xPos>SizeFont.MaxW)
		{
			ClearLcdDisplay(xPos,SizeFont.Y,(SizeFont.MaxW-xPos),SizeFont.MaxH);
			return (RollOffSet>0?RollOffSet:FPos);			
		}
		xPos+= DisplayFont(cmdType,FUnicode,FAttach,colour,xPos,SizeFont.Y);
		if(rt.Len==0)
			rt.Len=1;
		FPos+=rt.Len;
		if(FPos>=len)
		{
			ClearLcdDisplay(xPos,SizeFont.Y,(SizeFont.MaxW-xPos),SizeFont.MaxH);
			break;
		}
	}
	if(xDotPos>0)
	{
		uint16_t Dot = 0x002e;
		for(uint8_t i=0;i<3;i++)
		{
			DisplayFont(0x01,Dot,0,colour,xPos,SizeFont.Y);
			xPos+=7;
		}
	}
	if(IsAllChineseTran && UnknownCheck)
	{
		UnknownCheck =0;
		IsAllChineseTran = 0;
		return UNKNOWN_NUMBER;
	}
	return 0;
}

void StartRoll(void)
{	
	StartTimer =true;
	DisplayRoll =0;
	PIT_DRV_StartTimer(0, 0); 
}

void StopRoll(void)
{
	if(StartTimer)
	{
		StartTimer =false;
		PIT_DRV_StopTimer(0, 0);	
		DisplayRoll =0;
	}
}

void SetRollParameter(uint8_t *Rowdat, uint8_t *dat, DISPLAY_MODE dm,uint16_t colour, uint8_t x, uint8_t y, uint8_t w, uint8_t h)
{
	LineRollParameter.Rowdata = Rowdat;
	LineRollParameter.data = dat;
	if(dm == D_MODE3)
		dm=D_MODE2;	/*居中无法放置，需要置为滚动为居左滚动*/
	LineRollParameter.dm = dm;
	LineRollParameter.colour=colour;
	LineRollParameter.x=x;
	LineRollParameter.y=y;
	LineRollParameter.w=w;
	LineRollParameter.h=h;	
}

void DisplayLineRoll(void)
{
	uint8_t Roff=0;
	if(FlashDisable==0)
	{
		Roff =DisplayLine(LineRollParameter.data,LineRollParameter.dm,LineRollParameter.colour,LineRollParameter.x,LineRollParameter.y,LineRollParameter.w,LineRollParameter.h);
		if(Roff==0)
		{
			LineRollParameter.data = LineRollParameter.Rowdata;
			DisplayRollPeriod =TIMER_PERIOD_REROLL;
		}
		else
		{
			LineRollParameter.data+=Roff;
			DisplayRollPeriod =TIMER_PERIOD_ROLL;
		}
	}
}



void ShowMultipleLanRollOne(DISPLAY_MODE dm,uint16_t colour,uint8_t *val,uint8_t Val_Len,uint8_t x,uint8_t y,uint8_t win_w,uint8_t win_h)
{
	static uint8_t LanguageBuff[128];
	uint8_t Doffset =0;
	memset(LanguageBuff,0,128);
	memcpy(LanguageBuff,val,Val_Len);
	Doffset=DisplayLine(LanguageBuff,dm,colour,x,y,win_w,win_h);
	if(Doffset>0)
	{
		SetRollParameter(LanguageBuff,(LanguageBuff+Doffset),dm,colour,x,y,win_w,win_h);
		StartRoll();
	}

}


