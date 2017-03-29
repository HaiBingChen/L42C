#include "headfile.h"






uint8 XPos=0;
uint8 XPosOff=0;
uint8 YPos=0;
DISPLAY_MODE FDisMode = UNROLL;
uint8 Utf8UseLen =0;
uint8 UnicodeUseLen =0;
uint16* ptchRollingTxtOrg = NULL;
uint16* ptchRollingTxtCurr = NULL;
uint16* ptchRollingTxtStart = NULL;
bool CanNotDisplayOneLine = false;
bool RecordNextRollPos = false;

uint16 FontBuf[130]={0x00};
uint16 DevComFontBuf[20]={0x00};
uint8 FontLen = 0x00;

void Get_Ascii_Dot(uint8*Dot,uint16 Unicode)
{
	uint32 addr =0;
	Unicode-=0x0020;
	addr = DOT_ASCII_LIB_START + 0x12UL*(uint32)Unicode;
	if(addr<=DOT_ASCII_LIB_END)
	{
		SpiFlash_Read(EXT_SPI_FLASH_OFFSET+addr,Dot,18);
	}
}
#ifdef FONT_LARTIN
void Get_Lartin_Dot(uint8*Dot,uint16 Unicode)
{
	uint32 addr =0;
	Unicode-=0x00C0;
	addr = DOT_LARTIN_LIB_START + 0x12UL*(uint32)Unicode;
	if(addr>=DOT_LARTIN_LIB_START && addr<=DOT_LARTIN_LIB_END)
	{
		SpiFlash_Read(EXT_SPI_FLASH_OFFSET+addr,Dot,18);
	}
}
#endif

#ifdef FONT_RUSSION
void Get_Russion_Dot(uint8*Dot,uint16 Unicode,uint16 Unitcode_attach)
{
	uint32 addr =0;
	if (Unicode== 0x0415 && Unitcode_attach == 0x0308) 
	{
		addr = DOT_RUSSION_LIB_PART2_START;
		UnicodeUseLen = 2;
	}
	else if (Unicode == 0x0418 && Unitcode_attach== 0x0306) 
	{
		addr = DOT_RUSSION_LIB_PART2_START+ 0x12UL;
		UnicodeUseLen = 2;
	}
	else if (Unicode== 0x0419 && Unitcode_attach== 0x0306) 
	{
		addr = DOT_RUSSION_LIB_PART2_START+ 0x12UL*2;
		UnicodeUseLen = 2;
	}
	else if (Unicode== 0x0435 && Unitcode_attach== 0x0308) 
	{
		addr = DOT_RUSSION_LIB_PART2_START+ 0x12UL*3;
		UnicodeUseLen = 2;
	}
	else if (Unicode== 0x0438 && Unitcode_attach == 0x0306) 
	{
		addr = DOT_RUSSION_LIB_PART2_START+ 0x12UL*4;
		UnicodeUseLen = 2;
	}
	else if (Unicode== 0x0439 && Unitcode_attach == 0x0306) 
	{
		addr = DOT_RUSSION_LIB_PART2_START+ 0x12UL*5;
		UnicodeUseLen = 2;
	}
	else if (Unicode== 0x039B) 
		addr = DOT_RUSSION_LIB_PART2_START+ 0x12UL*6;
	else
	{
		Unicode-=0x0410;
		addr = DOT_RUSSION_LIB_PART1_START + 0x12UL*(uint32)Unicode;
	}
	if(addr>=DOT_RUSSION_LIB_PART1_START && addr<=DOT_RUSSION_LIB_END)
	{
		SpiFlash_Read(EXT_SPI_FLASH_OFFSET+addr,Dot,18);
	}
}
#endif	

#ifdef FONT_CHINESE
void Get_Chinese_Dot(uint8*Dot,uint16 Unicode)
{
	uint8 index0=0,index1=0,buf=0,gb[2];
	uint16 len =0,GB2312=0;
	uint32 addr =0;	
	index0=((Unicode>>8)&0xff)-0x4e;
	index1=Unicode&0xff;
	len = (Gb2312_AddrTable[index0+1]-Gb2312_AddrTable[index0])/4;
	if(len>0)
	{
		for(int16 i=0;i<len;i++)
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
					addr=DOT_CN_LIB_START+(0xBC0UL*(uint32)gb[1])+(0x20UL*(uint32)gb[0]);
				}
				if(addr>=DOT_CN_LIB_START && addr<=DOT_CN_LIB_END)
					SpiFlash_Read(EXT_SPI_FLASH_OFFSET+addr,Dot,32);
			}					
		}
	}			
}
#endif	

#ifdef FONT_JAPANESE
void Get_Japanese_Dot(uint8*Dot,uint16 Unicode)
{
	uint32 addr =0;	
	if((Unicode>=0x3041)&&(Unicode<=0x30FF))
	{
		Unicode -= 0x3041;
		addr = DOT_JP_LIB_PART1_START + 0x20UL*(uint32)Unicode;
	}
	else if((Unicode>=0x31F0)&&(Unicode<=0x31FF))
	{
		Unicode -= 0x31F0;
		addr = DOT_JP_LIB_PART2_START + 0x20UL*(uint32)Unicode;
	}
	if(addr>=DOT_JP_LIB_PART1_START && addr<=DOT_JP_LIB_END)
		SpiFlash_Read(EXT_SPI_FLASH_OFFSET+addr,Dot,32);
	
}
#endif

#ifdef FONT_THAI
void Get_Thai_Dot(uint8*Dot,uint16 Unicode)
{
	uint32 addr =0;	
	if((Unicode>=0x0E00)&&(Unicode<=0x0E7F))
	{
		Unicode -= 0x0E00;
		addr = DOT_TA_LIB_START + 0x20UL*(uint32)Unicode;
	}
	if(addr>=DOT_TA_LIB_START && addr<=DOT_TA_LIB_END)
		SpiFlash_Read(EXT_SPI_FLASH_OFFSET+addr,Dot,32);	
}
#endif

#ifdef FONT_KOREAN
void Get_Korean_Dot(uint8*Dot,uint16 Unicode)
{
	uint32 addr =0;	
	if((Unicode>=0x1100)&&(Unicode<=0x11ff))
	{
		Unicode -= 0x1100;
		addr = DOT_KO_LIB_PART1_START + 0x20UL*(uint32)Unicode;
	}
	else if((Unicode>=0x3130)&&(Unicode<=0x318f))
	{
		Unicode -= 0x3130;
		addr = DOT_KO_LIB_PART2_START + 0x20UL*(uint32)Unicode;
	}
	else if((Unicode>=0xAC00)&&(Unicode<=0xD7AF))
	{
		Unicode -= 0xAC00;
		addr = DOT_KO_LIB_PART3_START + 0x20UL*(uint32)Unicode;
	}
	if(addr>=DOT_KO_LIB_PART1_START && addr<=DOT_KO_LIB_END)
		SpiFlash_Read(EXT_SPI_FLASH_OFFSET+addr,Dot,32);	
}
#endif

uint8 CalculateDataLen(uint8 utf8)
{
    uint8 ret =1;                              //ASCII码
    if((utf8& 0xf0 )== 0xe0)           //中文日文
        ret =3;
    else if((utf8& 0xf0 )== 0xc0|| (utf8 & 0xf0 )== 0xd0)  //法文俄文
        ret =2;   
    else if((utf8& 0xf0 )== 0xf0)   //其他
        ret= 4;
    return ret;
}

bool Utf8ToUnitcode(uint8*utf8,uint8 offset, uint8 len, uint16* unitcode, uint16* unitcode_attach)
{
	if((utf8[offset] & 0x80 )== 0) {       //ASCII码
		*unitcode= utf8[offset];
		Utf8UseLen = 1;
		return true;
	}
	else if((utf8[offset] & 0xf0 )== 0xe0) {  //中文日文
		if(offset<(len-2)) {
			*unitcode =((((utf8[offset] & 0x0f) <<4) + ((utf8[offset+1] & 0x3c) >>2))<<8)+(((utf8[offset+1] & 0x03) <<6) + (utf8[offset+2]& 0x3f));
			Utf8UseLen =3;
			return true;
		}
		else 
			return false;
	}
	else if((utf8[offset] & 0xf0 )== 0xc0) {  //法文
		if(offset<(len-1)){	
			*unitcode =((((utf8[offset] & 0x1f) >>2) &0x07)<<8)+(((utf8[offset] & 0x03) <<6) + (utf8[offset+1]& 0x3f));
			Utf8UseLen = 2;	
			return true;
		}                                      
		else 
			return false; 
	}
	else if((utf8[offset] & 0xf0 )== 0xd0) { //俄文
		if(offset<(len-3)){	
			*unitcode =((((utf8[offset] & 0x1f) >>2) &0x07)<<8)+(((utf8[offset] & 0x03) <<6) + (utf8[offset+1]& 0x3f));
			*unitcode_attach = ((((utf8[offset+2] & 0x1f) >>2) &0x07)<<8)+(((utf8[offset+2] & 0x03) <<6) + (utf8[offset+3]& 0x3f));
			Utf8UseLen = 2;
			return true;
		}                                      
		else 
			return false; 
	}
	else if((utf8[offset] & 0xf0 )== 0xf0){  //其他
		if(offset<(len-4)){	
			*unitcode = 0xffff;
			Utf8UseLen = 4;
			return true;
		}
		else
			return false;	
	}
	return false;
}

uint16 UnknownUnicodeConversion(uint16 c)
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
                    c = 0x005C;
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
		      c = 0x0020;
                    break;                 
        }       
        return c;        
}

void	DisplaySingleInit(uint8 x)
{
	DisplayUnicode_Stop();
	XPos = x;
}

bool DisplaySingleUnicode(uint16 unicode, uint16 unitcode_attach,uint8 y)
{
	UnicodeUseLen=1;
	if(IS_ASCII(unicode)){		
		if(XPos+FONT_WIDTH_ASCII>XPOSMAX)
		{
			ClearLcdDisplay(XPos,y,(XPOSMAX-XPos),16);
			return false;
		}
		uint8 AsDot[18]={0x00};
		Get_Ascii_Dot(AsDot,unicode);
		DisplayMap(AsDot,XPos,y,9,16);
		XPos+=FONT_WIDTH_ASCII;				
	}
	
#ifdef FONT_LARTIN
	else	 if(IS_LARTIN(unicode)){
		if(XPos+FONT_WIDTH_LARTIN>XPOSMAX)
		{
			ClearLcdDisplay(XPos,y,(XPOSMAX-XPos),16);
			return false;
		}
		uint8 LaDot[18]={0x00};
		Get_Lartin_Dot(LaDot,unicode);
		DisplayMap(LaDot,XPos,y,9,16);
		XPos+=FONT_WIDTH_LARTIN;	
	}
#endif

#ifdef FONT_RUSSION
	else	 if(IS_RUSSION(unicode)){	
		if(XPos+FONT_WIDTH_RUSSION>XPOSMAX)
		{
			ClearLcdDisplay(XPos,y,(XPOSMAX-XPos),16);
			return false;
		}
		uint8 Rus[18]={0x00};
		Get_Russion_Dot(Rus,unicode,unitcode_attach);
		DisplayMap(Rus,XPos,y,9,16);
		XPos+=FONT_WIDTH_RUSSION;
		Utf8UseLen = UnicodeUseLen*2;	
	}
#endif

#ifdef FONT_CHINESE
	else	 if(IS_CHINESE(unicode)){
		if(XPos+FONT_WIDTH_CHINESE>XPOSMAX)
		{
			ClearLcdDisplay(XPos,y,(XPOSMAX-XPos),16);
			return false;
		}
		uint8 CnDot[32]={0x00};
		Get_Chinese_Dot(CnDot,unicode);
		DisplayMap(CnDot,XPos,y,16,16);
		XPos+=FONT_WIDTH_CHINESE;		
	}
#endif

#ifdef FONT_JAPANESE
	else	 if(IS_JAPANESE(unicode)){
		if(XPos+FONT_WIDTH_JANPANSE>XPOSMAX)
		{
			ClearLcdDisplay(XPos,y,(XPOSMAX-XPos),16);
			return false;
		}
		uint8 JpDot[32]={0x00};
		Get_Japanese_Dot(JpDot,unicode);
		DisplayMap(JpDot,XPos,y,16,16);
		XPos+=FONT_WIDTH_JANPANSE;		
	}
#endif

#ifdef FONT_THAI
	else	 if(IS_THAI(unicode)){
		if(XPos+FONT_WIDTH_THAI>XPOSMAX)
		{
			ClearLcdDisplay(XPos,y,(XPOSMAX-XPos),16);
			return false;
		}
		uint8 TaDot[32]={0x00};
		Get_Thai_Dot(TaDot,unicode);
		DisplayMap(TaDot,XPos,y,16,16);
		XPos+=FONT_WIDTH_THAI;		
	}
#endif

#ifdef FONT_KOREAN
	else	 if(IS_KOREAN(unicode)){
		if(XPos+FONT_WIDTH_KOREAN>XPOSMAX)
		{
			ClearLcdDisplay(XPos,y,(XPOSMAX-XPos),16);
			return false;
		}
		uint8 KoDot[32]={0x00};
		Get_Korean_Dot(KoDot,unicode);
		DisplayMap(KoDot,XPos,y,16,16);
		XPos+=FONT_WIDTH_KOREAN;		
	}
#endif
	else{	//不识别的字符
		unicode = UnknownUnicodeConversion(unicode);
		if(XPos+FONT_WIDTH_ASCII>XPOSMAX)
		{
			ClearLcdDisplay(XPos,y,(XPOSMAX-XPos),16);
			return false;
		}
		uint8 AsDot[18]={0x00};
		Get_Ascii_Dot(AsDot,unicode);
		DisplayMap(AsDot,XPos,y,9,16);
		XPos+=FONT_WIDTH_ASCII;		
	}
	return true;
}

uint8 GetUtf8UserLen(void)
{
	return Utf8UseLen;
}

void DisplayUnicode_Stop(void)
{
	osal_stop_timerEx(GetAppTaskId(), MSG_FONT_DISPLAY_NEXT_LETTER);
	osal_clear_event(GetAppTaskId(), MSG_FONT_DISPLAY_NEXT_LETTER);
	osal_stop_timerEx(GetAppTaskId(), MSG_FONT_BUF_DISPLAY_NEXT_LETTER);
	osal_clear_event(GetAppTaskId(), MSG_FONT_BUF_DISPLAY_NEXT_LETTER);	
}


uint8 _tcslen(uint16* pStr)
{
	uint8 Len = 0;
	while (*pStr ++) Len ++;
	return Len;
}


void DisplayLineUnicode(void)
{
	uint16 NextTime = LETTER_PERIOD;
	if(*ptchRollingTxtCurr!=0x0000)
	{
		if(DisplaySingleUnicode(*ptchRollingTxtCurr,*(ptchRollingTxtCurr+1),YPos))
		{
			if(ptchRollingTxtCurr==ptchRollingTxtStart&&RecordNextRollPos)
			{	
				RecordNextRollPos =false;
				ptchRollingTxtStart+=UnicodeUseLen;
			}
			ptchRollingTxtCurr+=UnicodeUseLen;
			if(*ptchRollingTxtCurr==0x0000)	/*全部Unicode 显示完毕*/
			{
				if(FDisMode==ROLL && CanNotDisplayOneLine)
				{
					ptchRollingTxtCurr = ptchRollingTxtOrg;
					ptchRollingTxtStart = ptchRollingTxtOrg;
					RecordNextRollPos =true;
					if(XPos<XPOSMAX)
						ClearLcdDisplay(XPos,YPos,(XPOSMAX-XPos),16);/*清除残影*/
					XPos =XPosOff;
					NextTime = ROLL_PERIOD;
				}
				else
					NextTime= 0;
			}
		}
		else    /*一行显示完毕*/
		{
			CanNotDisplayOneLine = true;
		 	if(FDisMode==ROLL)
		 	{
				ptchRollingTxtCurr = ptchRollingTxtStart;
				RecordNextRollPos =true;
				XPos =XPosOff;
				NextTime = ROLL_PERIOD;
		 	}
			else
				NextTime= 0;
		}
	}
	else
		NextTime= 0;	
	if(NextTime>0)
		osal_start_timerEx(GetAppTaskId(), MSG_FONT_DISPLAY_NEXT_LETTER, NextTime);
}

uint8 _RollingText_CalcXpos(uint16* pStr,uint8 xoffset)
{
	uint8 x =xoffset;
	uint16 width = 0;
	uint8 Len =_tcslen(pStr);
	for(uint8 i = 0; i < Len; i ++) {
		if(IS_CHINESE(pStr[i]))
			width += FONT_WIDTH_CHINESE;
		else if (IS_THAI(pStr[i]))
			width += FONT_WIDTH_THAI;
		else if (IS_KOREAN(pStr[i]))
			width += FONT_WIDTH_KOREAN;
		else if (IS_LARTIN(pStr[i]))
			width += FONT_WIDTH_LARTIN;
		else if (IS_RUSSION(pStr[i]))
		{
			if((i+1)<Len)
			{
				if ((*pStr == 0x0415 && *(pStr + 1) == 0x0308) ||(*pStr == 0x0418 && *(pStr + 1) == 0x0306) ||(*pStr == 0x0419 && *(pStr + 1) == 0x0306) ||
					(*pStr == 0x0435 && *(pStr + 1) == 0x0308) ||(*pStr == 0x0438 && *(pStr + 1) == 0x0306) ||(*pStr == 0x0439 && *(pStr + 1) == 0x0306))
				{
					i++;
				}				
			}
			width += FONT_WIDTH_RUSSION;
		}
		else if (IS_JAPANESE(pStr[i]))
			width += FONT_WIDTH_JANPANSE;
		else
			width += FONT_WIDTH_ASCII;
	}
	if((width+xoffset) < LCD_SIZE_W)
	{
		if(xoffset>0)
			x = LCD_SIZE_W - width;			/*右对齐*/
		else
			x = (LCD_SIZE_W - width)/2;		/*居中*/
	}
	return x;
}

void	DisplayLine(uint16* unicode, uint8 x,uint8 y, DISPLAY_MODE mode)
{
	DisplayUnicode_Stop();
	XPosOff = x;
	XPos = _RollingText_CalcXpos(unicode,x);
	YPos = y;
	FDisMode =mode;
	ptchRollingTxtOrg = unicode;
	ptchRollingTxtCurr = unicode;
	ptchRollingTxtStart = unicode;
	CanNotDisplayOneLine = false;
	RecordNextRollPos = true;
	DisplayLineUnicode();
}


uint8 CalculateCallerIDLen(uint8* CallerID, uint8 Len)
{
     uint8 CallIDlen =0;
     for(uint8 i=0;i<Len;i++)
     {
           if((CallerID[i] & 0xf0 )== 0xe0){           //中文
               if(i<(Len-2))                                     //计算结束
	            i+=2;      
		 else
		     break;	              
	    }else if((CallerID[i] & 0xf0 )== 0xc0|| (CallerID[i] & 0xf0 )== 0xd0){  //法文
               if(i<(Len-1))                                     //计算结束
	            i+=1;  
		 else
		     break;             
	    }
	    CallIDlen++;
     }
     return CallIDlen;
}

void CallerIDUtf8ToUnicode(uint8*ut,uint8 Len,uint16* uc)
{
    uint8 CallIDlen =0; 
    for(uint8 i=0;i<Len;i++)
    {
	 if((ut[i] & 0x80 )== 0)
        {     
            uc[CallIDlen] = ut[i];
	 }
        else if((ut[i] & 0xf0 )== 0xe0)
        {
            if(i<(Len-2)){
                uc[CallIDlen] =((((ut[i] & 0x0f) <<4) + ((ut[i+1] & 0x3c) >>2))<<8)+(((ut[i+1] & 0x03) <<6) + (ut[i+2]& 0x3f));
		  i+=2; 	
	     }    	         
	     else
		 break;
	 }
        else if((ut[i] & 0xf0 )== 0xc0 || (ut[i] & 0xf0 )== 0xd0)
        {
            if(i<(Len-1)){	
                uc[CallIDlen] =((((ut[i] & 0x1f) >>2) &0x07)<<8)+(((ut[i] & 0x03) <<6) + (ut[i+1]& 0x3f));
		  i+=1;   
	     }                                      
	     else 
	         break;
	 }
        CallIDlen++;
     }     
}



void DisplayFontLineUnicode(void)
{
	uint16 NextTime = LETTER_PERIOD;
	while(NextTime)
	{
		if(*ptchRollingTxtCurr!=0x0000)
		{
			if(DisplaySingleUnicode(*ptchRollingTxtCurr,*(ptchRollingTxtCurr+1),YPos))
			{
				if(ptchRollingTxtCurr==ptchRollingTxtStart&&RecordNextRollPos)
				{	
					RecordNextRollPos =false;
					ptchRollingTxtStart+=UnicodeUseLen;
				}
				ptchRollingTxtCurr+=UnicodeUseLen;
				if(*ptchRollingTxtCurr==0x0000)	/*全部Unicode 显示完毕*/
				{
					if(FDisMode==ROLL && CanNotDisplayOneLine)
					{
						ptchRollingTxtCurr = ptchRollingTxtOrg;
						ptchRollingTxtStart = ptchRollingTxtOrg;
						RecordNextRollPos =true;
						if(XPos<XPOSMAX)
							ClearLcdDisplay(XPos,YPos,(XPOSMAX-XPos),16);/*清除残影*/
						XPos =XPosOff;
						NextTime = ROLL_PERIOD;
					}
					else
						NextTime= 0;
				}
			}
			else    /*一行显示完毕*/
			{
				CanNotDisplayOneLine = true;
			 	if(FDisMode==ROLL)
			 	{
					ptchRollingTxtCurr = ptchRollingTxtStart;
					RecordNextRollPos =true;
					XPos =XPosOff;
					NextTime = ROLL_PERIOD;
			 	}
				else
					NextTime= 0;
			}
		}
		else
			NextTime= 0;	
		if(NextTime==ROLL_PERIOD)
		{
			osal_start_timerEx(GetAppTaskId(), MSG_FONT_BUF_DISPLAY_NEXT_LETTER, NextTime);
			break;
		}
	}
}

void	DisplayFontLine(uint16* unicode, uint8 x,uint8 y, DISPLAY_MODE mode)
{
	DisplayUnicode_Stop();
	XPosOff = x;
	XPos = _RollingText_CalcXpos(unicode,x);
	YPos = y;
	FDisMode =mode;
	ptchRollingTxtOrg = unicode;
	ptchRollingTxtCurr = unicode;
	ptchRollingTxtStart = unicode;
	CanNotDisplayOneLine = false;
	RecordNextRollPos = true;
	DisplayFontLineUnicode();
}

void WriteFontBuf(uint8* Fname, uint8 Flen) 
{
	if(Flen>128)
		Flen = 128;
	FontLen = CalculateCallerIDLen(Fname,Flen);
	CallerIDUtf8ToUnicode(Fname,Flen,FontBuf);
	FontBuf[FontLen]=0x0000;
}
void ClearFontBuf(void)
{
	FontLen =0 ;
}

void DisplayFontBuf(void)
{
	if(FontLen>0)
		DisplayFontLine(FontBuf,0,32,ROLL);	 
}

void DevComWriteFontBuf(uint8* Fname, uint8 Flen) 
{
	if(Flen>20)
		Flen = 20;
	FontLen = CalculateCallerIDLen(Fname,Flen);
	CallerIDUtf8ToUnicode(Fname,Flen,DevComFontBuf);
	DevComFontBuf[FontLen]=0x0000;
}
void DevComClearFontBuf(void)
{
	FontLen =0 ;
}

void DevComDisplayFontBuf(void)
{
	if(FontLen>0)
		DisplayFontLine(DevComFontBuf,0,32,ROLL);	 
}



