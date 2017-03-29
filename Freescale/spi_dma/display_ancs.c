
#include "display_ancs.h"
#include "display_font.h"
#include "ExtSpiFlash.h"
#include "fsl_debug_console.h"
#include <string.h>
#include "fsl_pit_driver.h"
#include "ResMap.h"
#include "ui.h"

const uint32_t SmallMapAddr[18]={  0x00000000UL,					//Ancs_Null
								0x00000000UL,					//Ancs_IncomingCall
								0x00000000UL,					//Ancs_MissedCall
								0x00000000UL,					//Ancs_Sms
								notification_social_40x40, 			//Ancs_Social
								0x00000000UL, 					//Ancs_Email
								notification_event_40x40,			//Ancs_Schedule
								0x00000000UL, 					//Call_Record
								notification_social_40x40,  			//Ancs_Social_Gmail
								notification_40x40_facebook_L42,		//Ancs_Social_Facebook
								notification_40x40_twitter_L42,		//Ancs_Social_Twitter
								notification_40x40_qqwechat_L42,	//Ancs_Social_Wechat
								notification_40x40_snapchat_L42,		//Ancs_Social_Snapchat
								notification_40x40_instagram_L42, 	//Ancs_Social_Instagram
								notification_40x40_line_L42,			//Ancs_Social_Line
								notification_40x40_whatsapp_L42,	//Ancs_Social_WhatsApp
								notification_40x40_messenger_L42,	//Ancs_Social_Messenger
								notification_40x40_qqwechat_L42 	//Ancs_Social_QQ
							  	};

/*传入参数*/
uint32_t ANCS_READ_ADDR =0x00000000UL;
uint8_t   LayOut[5]={0x00};
uint8_t   LayOutIndex =0;
uint8_t   DetailIndex =0x40;
DetailPageDir DetailDir = De_INIT;
	
uint8_t   DetailOffset =0x00;
uint8_t   DetailPage[3]={0x00};
uint8_t   DetailPageIndex=0x00;
uint8_t   SmsFromAndroid = 0;

uint8_t   LenSave[5][4]={0x00};
static uint8_t UnKnownNumber[15]={0x55,0x4E,0x4B,0x4E,0x4F,0x57,0x4E,0x20,0x4E,0x55,0x4D,0x42,0x45,0x52,0x00};
uint8_t   UnknownCheck=0;
uint8_t   ReadEnd =0;


typedef struct 
{
	uint8_t Title[100];
	uint8_t Msg[128];
	uint8_t Date[20]; 
}ANCS_DATA;
static ANCS_DATA AD;

uint8_t Display_PhoneRecordTitle(uint8_t index, DISPLAY_MODE dm,uint16_t colour,uint8_t x, uint8_t y, uint8_t w, uint8_t h)
{
	uint8_t Doffset=0;
	uint8_t * PrBUF=NULL;
	uint8_t Numlen =LenSave[index][1];
	uint8_t Namelen =LenSave[index][3];
	uint8_t CallID[128]={0x00};
	memset(CallID,0,128);
	uint8_t Phpos =0;
	uint32_t addr = ANCS_READ_ADDR+256*(LayOut[index]&0x1F);
	memset(AD.Title,0,100);	
	memset(AD.Msg,0,128);
	if(Namelen>0)
	{
		SpiFlash_Read(addr+4+Numlen+7,AD.Msg,Namelen);
		for(uint8_t i=0;i<Namelen;i++)
		{
			if((i+2)<Namelen)
			{
				if((AD.Msg[i]==0xE2)&&(AD.Msg[i+1]==0x80))
				{
					i++;
					i++;
					continue;
				}
				else
				{
					CallID[Phpos++] = AD.Msg[i];
				}
			}
			else
			{
				if(AD.Msg[i]==0xE2)
					break;
				else
					CallID[Phpos++] = AD.Msg[i];
			}
		}
		PrBUF = CallID;
	}
	else if(Numlen>0)
	{
		SpiFlash_Read(addr+4,AD.Title,Numlen);
		PrBUF = AD.Title;
	}
	if(PrBUF==NULL)
		return 0;
	
	UnknownCheck =1;
	Doffset= DisplayLine(PrBUF,dm,colour,x,y,w,h);
	UnknownCheck =0;
	if(Doffset==UNKNOWN_NUMBER)
	{
		memset(PrBUF,0,100);
		memcpy(PrBUF,UnKnownNumber,15);
		Doffset= DisplayLine(PrBUF,dm,colour,x,y,w,h);
	}
	if(Doffset>0&&(dm==D_MODE2||dm==D_MODE3))
	{
		SetRollParameter(PrBUF,(PrBUF+Doffset),dm,colour,x,y,w,h);
		StartRoll();
	}
	return Doffset;	
}

uint8_t Display_PhoneRecordDate_Preview(uint8_t index,DISPLAY_MODE dm,uint16_t colour,uint8_t x, uint8_t y, uint8_t w, uint8_t h)
{
	 //2014-10-11 14:12:13     20141011T141230
	uint8_t Doffset=0;
	uint8_t Numlen =LenSave[index][1];
	uint8_t Datelen =LenSave[index][2];
	uint32_t addr = ANCS_READ_ADDR+256*(LayOut[index]&0x1F);	
	char TempShowNumber[10]={0};
	uint8_t Date[10]={0x00};
	if(Datelen>7)
		Datelen=7;
	SpiFlash_Read(addr+4+Numlen,Date,Datelen);
	sprintf(TempShowNumber,"%.2d:%.2d",Date[4],Date[5]);
	Doffset= DisplayLine((uint8_t*)TempShowNumber,dm,colour,x,y,w,h);
	return Doffset;	
}


/*详细内容 参考区间 50,24,140,16*/
uint8_t Display_AncsTitle(uint8_t index, DISPLAY_MODE dm,uint16_t colour,uint8_t x, uint8_t y, uint8_t w, uint8_t h)
{
	uint8_t Doffset=0;
	uint8_t tlen =LenSave[index][1];
	uint32_t addr = ANCS_READ_ADDR+256*(LayOut[index]&0x1F);
	memset(AD.Title,0,100);	
#if 1	
	SpiFlash_Read(addr+4,AD.Title,tlen);
#else
	uint8_t i=0;
	ANCS_SAVE_TYPE type = (ANCS_SAVE_TYPE)(LenSave[index][0]);
	if(type<Ancs_Social)
	{
		SpiFlash_Read(addr+4,AD.Title,tlen);
	}
	else
	{
		memset(AD.Msg,0,128);
		uint8_t mlen =LenSave[index][2];
		SpiFlash_Read(addr+4+tlen,AD.Msg,mlen);
		for(i=0;i<128;i++)
		{	
			if(AD.Msg[i]==0x3a)
				break;
		}
		memcpy(AD.Title,AD.Msg,i);
		if(LenSave[index][2]>=119)
			LenSave[index][3]=0xFF;
		LenSave[index][1]=tlen+(i+1);
		LenSave[index][2]-=(i+1);
	}
#endif
	UnknownCheck =1;
	Doffset= DisplayLine(AD.Title,dm,colour,x,y,w,h);
	UnknownCheck =0;
	if(Doffset==UNKNOWN_NUMBER)
	{
		memset(AD.Title,0,100);
		memcpy(AD.Title,UnKnownNumber,15);
		//LenSave[index][1] =14;
		Doffset= DisplayLine(AD.Title,dm,colour,x,y,w,h);
	}
	if(Doffset>0&&(dm==D_MODE2||dm==D_MODE3))
	{
		SetRollParameter(AD.Title,(AD.Title+Doffset),dm,colour,x,y,w,h);
		StartRoll();
	}
	return Doffset;	
}


/*详细内容 参考区间 20,60,200,72   (3行72)*/
uint8_t Display_AncsMsg(uint8_t index, DISPLAY_MODE dm,uint16_t colour,uint8_t x, uint8_t y, uint8_t w, uint8_t h)
{
	uint8_t HMax =h+y;
	uint8_t HUse = 24;
	uint8_t Hoffset =26;
	uint8_t Doffset=0;
	uint8_t* msgdata;
	uint8_t tlen =LenSave[index][1];
	uint8_t mlen =LenSave[index][2];
	uint8_t Dellen =0;
	uint32_t addr = ANCS_READ_ADDR+256*(LayOut[index]&0x1F);
	uint8_t LastY=0;
	memset(AD.Msg,0,128);
	SpiFlash_Read(addr+4+tlen,AD.Msg,mlen);	
	if(mlen>=113||LenSave[index][3]==0xFF)
	{
		Dellen=CalculateAncsLen(AD.Msg,113);
		memset((AD.Msg+113-Dellen),0x2e,3);
		memset((AD.Msg+113-Dellen+3),0x0,1);
	}	
	msgdata = AD.Msg+DetailOffset;
	DetailPage[DetailPageIndex]=DetailOffset;
	for(uint8_t i=0;i<50;i++)
	{
		if(i==0)
			DisplayMap_flash(ancs_backgroud_1_up_218x46,11, (y-20), 218, 46);
		else
			DisplayMap_flash(ancs_backgroud_2_center_218x26,11, (y), 218, 26);
		NeedBackground =true;
		Doffset= DisplayLine(msgdata,dm,colour,x,y,w,HUse);
		NeedBackground=false;
		LastY=y;
		if(Doffset>0)
		{
			y+=Hoffset;
			msgdata+=Doffset;
			DetailOffset+=Doffset;
		}
		else							/*全部读完*/
		{
			ReadEnd =1;
			break;
		}
		if((LastY+HUse)>HMax)
			break;
	}
	if(Doffset==0)
		DetailOffset=0xFF;
	DisplayMap_flash(ancs_backgroud_31_down_218x20,11, (LastY+24), 218, 20);
	return Doffset;
}


uint8_t Display_AncsMsg_Preview(uint8_t index, DISPLAY_MODE dm,uint16_t colour,uint8_t x, uint8_t y, uint8_t w, uint8_t h)
{
	uint8_t Doffset=0;
	uint8_t tlen =LenSave[index][1];
	uint8_t mlen =LenSave[index][2];
	uint32_t addr = ANCS_READ_ADDR+256*(LayOut[index]&0x1F);	
	memset(AD.Msg,0,128);
	SpiFlash_Read(addr+4+tlen,AD.Msg,mlen);
	Doffset= DisplayLine(AD.Msg,dm,colour,x,y,w,h);
	return Doffset;	
}

char monthStr[][4]={"Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec"};
uint8_t Display_AncsDate_Preview(uint8_t index,DISPLAY_MODE dm,uint16_t colour,uint8_t x, uint8_t y, uint8_t w, uint8_t h)
{
	 //2014-10-11 14:12:13     20141011T141230
	uint8_t Doffset=0;
	uint8_t tlen =LenSave[index][1];
	uint8_t mlen =LenSave[index][2];
	uint8_t dlen =LenSave[index][3];	
	uint32_t addr = ANCS_READ_ADDR+256*(LayOut[index]&0x1F);	
	uint8_t Date[15]={0x00};
	uint8_t month=1;
	memset(AD.Date,0,20);
	if(dlen>15)
		dlen=15;
	SpiFlash_Read(addr+4+tlen+mlen,Date,dlen);
	memcpy(AD.Date,Date+9,2);
	memset(AD.Date+2,0x3a,1);
	memcpy(AD.Date+3,Date+11,2);
	memset(AD.Date+5,0x2c,1);
	if(Date[4]>'0')
		month = 10 + (Date[5]-'0');
	else
		month = Date[5]-'0';
	if((month>12)||(month<1))
		month = 1;
	memcpy(AD.Date+6,monthStr[month-1],3);
	memset(AD.Date+9,0x20,1);
	memcpy(AD.Date+10,Date+6,2);
	memset(AD.Date+12,0x2c,1);
	memcpy(AD.Date+13,Date,4);
	Doffset= DisplayLine(AD.Date,dm,colour,x,y,w,h);
	return Doffset;	
}

uint8_t Display_AncsDate_Preview2(uint8_t index,DISPLAY_MODE dm,uint16_t colour,uint8_t x, uint8_t y, uint8_t w, uint8_t h)
{
	 //2014-10-11 14:12:13     20141011T141230
	uint8_t Doffset=0;
	uint8_t tlen =LenSave[index][1];
	uint8_t mlen =LenSave[index][2];
	uint8_t dlen =LenSave[index][3];	
	uint32_t addr = ANCS_READ_ADDR+256*(LayOut[index]&0x1F);	
	uint8_t Date[15]={0x00};
	
	SpiFlash_Read(addr+4+tlen+mlen,Date,dlen);
	memcpy(AD.Date,Date+9,2);
	memset(AD.Date+2,0x3a,1);
	memcpy(AD.Date+3,Date+11,2);

	Doffset= DisplayLine(AD.Date,dm,colour,x,y,w,h);
	return Doffset;	
}

uint8_t Display_AncsDate(uint8_t index,DISPLAY_MODE dm,uint16_t colour,uint8_t x, uint8_t y, uint8_t w, uint8_t h)
{
	 //2014-10-11 14:12:13     20141011T141230
	uint8_t Doffset=0;
	uint8_t tlen =LenSave[index][1];
	uint8_t mlen =LenSave[index][2];
	uint8_t dlen =LenSave[index][3];	
	uint32_t addr = ANCS_READ_ADDR+256*(LayOut[index]&0x1F);	
	uint8_t Date[15]={0x00};
	uint8_t month=1;
	memset(AD.Date,0,20);
	if(dlen>15)
		dlen=15;
	SpiFlash_Read(addr+4+tlen+mlen,Date,dlen);
	memcpy(AD.Date,Date+9,2);
	memset(AD.Date+2,0x3a,1);
	memcpy(AD.Date+3,Date+11,2);
	memset(AD.Date+5,0x2c,1);
	if(Date[4]>'0')
		month = 10 + (Date[5]-'0');
	else
		month = Date[5]-'0';
	if((month>12)||(month<1))
		month = 1;
	memcpy(AD.Date+6,monthStr[month-1],3);
	memset(AD.Date+9,0x20,1);
	memcpy(AD.Date+10,Date+6,2);
	memset(AD.Date+12,0x2c,1);
	memcpy(AD.Date+13,Date,4);

	Doffset= DisplayLine(AD.Date,dm,colour,x,y,w,h);
	if(Doffset>0&&(dm==D_MODE2||dm==D_MODE3))
	{
		SetRollParameter(AD.Date,(AD.Date+Doffset),dm,colour,x,y,w,h);
		StartRoll();
	}
	return Doffset;	
}

uint8_t Display_AncsDate8x16_Preview(uint8_t index,DISPLAY_MODE dm,uint16_t colour,uint8_t x, uint8_t y, uint8_t w, uint8_t h)
{
	 //2014-10-11 14:12:13     20141011T141230
	uint8_t Doffset=0;
	uint8_t tlen =LenSave[index][1];
	uint8_t mlen =LenSave[index][2];
	uint8_t dlen =LenSave[index][3];	
	uint32_t addr = ANCS_READ_ADDR+256*(LayOut[index]&0x1F);	
	uint8_t Date[15]={0x00};
	memset(AD.Date,0,20);
	if(dlen>15)
		dlen=15;
	SpiFlash_Read(addr+4+tlen+mlen,Date,dlen);
	memcpy(AD.Date,Date+9,2);
	memset(AD.Date+2,0x3a,1);
	memcpy(AD.Date+3,Date+11,2);
	DisplayStringNumber8x16(AD.Date,colour,x,y);
	return Doffset;	
}


void Display_Ancs_BigviewNum(uint8_t Num, bool clean)
{
	uint8_t posx = 167, posy =71;
	if(Num>99)
		Num=99;
	uint8_t t =Num/10;
	uint8_t n=Num%10;
	
	if(t==0)
	{
		if(clean)
		{
			DisplayMapCleanAdd(D_ADD,(remindnumber14px_9x10+n*0xB4),posx,posy,9,10);
		}
		else
		{
			DisplayMap_flash((remindnumber14px_9x10+n*0xB4),posx,posy,9,10);
		}
	}
	else
	{
		if(clean)
		{
			DisplayMapCleanAdd(D_ADD,(remindnumber14px_9x10+t*0xB4),posx,posy,9,10);		/*十位数字*/
			DisplayMapCleanAdd(D_ADD,(remindnumber14px_9x10+n*0xB4),posx+9,posy,9,10);		/*个位数字*/		
		}
		else
		{
			DisplayMap_flash((remindnumber14px_9x10+t*0xB4),posx,posy,9,10);				/*十位数字*/
			DisplayMap_flash((remindnumber14px_9x10+n*0xB4),posx+9,posy,9,10);				/*个位数字*/	
		}		
	}
}

uint8_t Display_Ancs_Bigview(MsgUI mu ,uint8_t count)
{
	uint8_t Posx = 0;
	LayOutIndex =0;
	switch(mu)
	{
		case M_INCOMINGCALL:
			break;
			
		case M_MISSEDCALL:
			DisplayMapCleanAdd(D_INIT,notification_misscalls_90x90,75, 75, 90, 90);
			ShowMainMenuTimeOne();
			Display_Ancs_BigviewNum(count,true);
			Posx = GetCharDisXpos("Missed Calls", CHAR_12X24, 0, DIS_CENTER);
			DisplayCharCleanAdd(D_ADD,"Missed Calls",CHAR_12X24,WHITE,Posx,182);		
			DisplayMap_flash_WithCleanSreen();			
			break;
			
		case M_SMS:
			DisplayMapCleanAdd(D_INIT,notification_message_90x90,75, 75, 90, 90);
			ShowMainMenuTimeOne();
			Display_Ancs_BigviewNum(count,true);
			Posx = GetCharDisXpos("Text Message", CHAR_12X24, 0, DIS_CENTER);
			DisplayCharCleanAdd(D_ADD,"Text Message",CHAR_12X24,WHITE,Posx,182);			
			DisplayMap_flash_WithCleanSreen();
			break;	
			
		case M_SOCIAL:
			DisplayMapCleanAdd(D_INIT,notification_social_90x90,75, 75, 90, 90);
			ShowMainMenuTimeOne();
			Display_Ancs_BigviewNum(count,true);
			Posx = GetCharDisXpos("Social Media", CHAR_12X24, 0, DIS_CENTER);
			DisplayCharCleanAdd(D_ADD,"Social Media",CHAR_12X24,WHITE,Posx,182);				
			DisplayMap_flash_WithCleanSreen();
			break;			

		case M_E_MAIL:
			DisplayMapCleanAdd(D_INIT,notification_email_90x68,75, 86, 90, 68);
			ShowMainMenuTimeOne();
			Display_Ancs_BigviewNum(count,true);
			Posx = GetCharDisXpos("E-Mail", CHAR_12X24, 0, DIS_CENTER);
			DisplayCharCleanAdd(D_ADD,"E-Mail",CHAR_12X24,WHITE,Posx,182);			
			DisplayMap_flash_WithCleanSreen();	
			break;

		case M_DAILY_SCHEDULE:
			DisplayMapCleanAdd(D_INIT,notification_event_90x90,75, 75, 90, 90);
			ShowMainMenuTimeOne();
			Display_Ancs_BigviewNum(count,true);
			Posx = GetCharDisXpos("Calendar", CHAR_12X24, 0, DIS_CENTER);
			DisplayCharCleanAdd(D_ADD,"Calendar",CHAR_12X24,WHITE,Posx,182);				
			DisplayMap_flash_WithCleanSreen();
			break;
		
		case M_COUNT_UPDATE:
			Display_Ancs_BigviewNum(count,false);
			break;
	}
	return 0;
}

void InitDetailParameter(void)
{
	DetailPage[0]=0x00;
	DetailPage[1]=0x00;
	DetailPage[2]=0x00;
	DetailOffset =0x00;
	DetailPageIndex =0x00;
}


uint8_t Display_Ancs_LayOut(void)
{
	uint8_t i=0;
	uint8_t NumberofPage = 3;
	uint32_t IconAddress = 0;
	ANCS_SAVE_TYPE atype=Ancs_Null;
	for(i=0;i<5;i++)
	{
		if(LayOut[i]==0x40)
			break;
		SpiFlash_Read((ANCS_READ_ADDR+256*(LayOut[i]&0x1F)),LenSave[i],4);	
	}
	atype = (ANCS_SAVE_TYPE) LenSave[0][0];
		
	if(DetailIndex==0x40)				/*显示PreView*/
	{		
		//初始化Detail参数:
		InitDetailParameter();
		if(atype==Ancs_IncomingCall)			/*来电*/
		{
			DisplayMapCleanAdd(D_INIT,remind_incomingcalls_66x88,87, 76, 66, 88);
			ShowMainMenuTimeOne();

			//DisplayCharCleanAdd(D_INIT,"incoming call",CHAR_12X16,WHITE,63,90);
			DisplayMap_flash_WithCleanSreen();
			if(LayOut[0]!=0x40)	
			{
				Display_AncsTitle(LayOutIndex*1,D_MODE3,WHITE,40,182,160,24);	
			}
		}
		
		else if(atype==Ancs_MissedCall)			/*未接来电*/
		{
			NumberofPage = 3;
			if(LayOut[LayOutIndex*NumberofPage]!=0x40)
			{
				DisplayMapCleanAdd(D_INIT,notification_misscalls_40x40 ,7,5,40,40);
			}
			//if(LayOutIndex==0)
			{	
				if(LayOut[LayOutIndex*NumberofPage+1]!=0x40)
				{
					DisplayMapCleanAdd(D_ADD,app_whiteline_240x1, 0, 80, 240, 1);			/*画直线*/
					DisplayMapCleanAdd(D_ADD,notification_misscalls_40x40 ,7,(80+5),40,40);
				}
			}
			if(LayOutIndex==0)
			{
				if(LayOut[LayOutIndex*NumberofPage+2]!=0x40)
				{
					DisplayMapCleanAdd(D_ADD,app_whiteline_240x1, 0, 160, 240, 1);	/*画直线*/
					DisplayMapCleanAdd(D_ADD,notification_misscalls_40x40 ,7,(160+5),40,40);
				}
			}
			
			DisplayMap_flash_WithCleanSreen();	
			
			if(LayOut[LayOutIndex*NumberofPage]!=0x40)
			{
				Display_AncsTitle(LayOutIndex*NumberofPage,D_MODE4,WHITE,57,12,173,24);
				Display_AncsDate_Preview(LayOutIndex*NumberofPage,D_MODE0,WHITE,7,(12+38),226,24);
			}
			//if(LayOutIndex==0)
			{			
				if(LayOut[LayOutIndex*NumberofPage+1]!=0x40)
				{
					Display_AncsTitle(LayOutIndex*NumberofPage+1,D_MODE4,WHITE,57,(80+12),173,24);
					Display_AncsDate_Preview(LayOutIndex*NumberofPage+1,D_MODE0,WHITE,7,(80+12+38),226,24);
				}
			}
			if(LayOutIndex==0)
			{
				if(LayOut[LayOutIndex*NumberofPage+2]!=0x40)
				{
					Display_AncsTitle(LayOutIndex*NumberofPage+2,D_MODE4,WHITE,57,(160+12),173,24);
					Display_AncsDate_Preview(LayOutIndex*NumberofPage+2,D_MODE0,WHITE,7,(160+12+38),226,24);
				}
			}				
						
		}
		
		else if((uint8_t)atype>=0x50 && (uint8_t)atype<=0x54)				/*通话记录*/
		{
			if(LayOut[LayOutIndex*4]!=0x40)
			{
				uint32_t PhoneIcon=dail_greenarrow_16x16;
				uint8_t  PhoneTag =LenSave[LayOutIndex*4][0];
				if(PhoneTag==0x50||PhoneTag==0x51)
					PhoneIcon=dial_redarrow_16x16_1;
				else if(PhoneTag==0x52)
					PhoneIcon=dail_redarrow_16x16;
				else if(PhoneTag==0x53)
					PhoneIcon=dail_greenarrow_16x16;
				else if(PhoneTag==0x54)
					PhoneIcon=dial_greenarrow_16x16_1;
				DisplayMapCleanAdd(D_INIT,PhoneIcon,7,22,16,16);	
			}
			if(LayOutIndex==0)
			{	
				if(LayOut[LayOutIndex*4+1]!=0x40)
				{
					uint32_t PhoneIcon=dail_greenarrow_16x16;
					uint8_t  PhoneTag =LenSave[LayOutIndex*4+1][0];
					if(PhoneTag==0x50||PhoneTag==0x51)
						PhoneIcon=dial_redarrow_16x16_1;
					else if(PhoneTag==0x52)
						PhoneIcon=dail_redarrow_16x16;
					else if(PhoneTag==0x53)
						PhoneIcon=dail_greenarrow_16x16;
					else if(PhoneTag==0x54)
						PhoneIcon=dial_greenarrow_16x16_1;
					DisplayMapCleanAdd(D_ADD,PhoneIcon,7,(60+22),16,16);
				}	
			}
			if(LayOutIndex==0)
			{
				if(LayOut[LayOutIndex*4+2]!=0x40)
				{
					uint32_t PhoneIcon=dail_greenarrow_16x16;
					uint8_t  PhoneTag =LenSave[LayOutIndex*4+2][0];
					if(PhoneTag==0x50||PhoneTag==0x51)
						PhoneIcon=dial_redarrow_16x16_1;
					else if(PhoneTag==0x52)
						PhoneIcon=dail_redarrow_16x16;
					else if(PhoneTag==0x53)
						PhoneIcon=dail_greenarrow_16x16;
					else if(PhoneTag==0x54)
						PhoneIcon=dial_greenarrow_16x16_1;
					DisplayMapCleanAdd(D_ADD,PhoneIcon,7,(120+22),16,16);
				}
			}
			if(LayOutIndex==0)
			{
				if(LayOut[LayOutIndex*4+3]!=0x40)
				{
					uint32_t PhoneIcon=dail_greenarrow_16x16;
					uint8_t  PhoneTag =LenSave[LayOutIndex*4+3][0];
					if(PhoneTag==0x50||PhoneTag==0x51)
						PhoneIcon=dial_redarrow_16x16_1;
					else if(PhoneTag==0x52)
						PhoneIcon=dail_redarrow_16x16;
					else if(PhoneTag==0x53)
						PhoneIcon=dail_greenarrow_16x16;
					else if(PhoneTag==0x54)
						PhoneIcon=dial_greenarrow_16x16_1;
					DisplayMapCleanAdd(D_ADD,PhoneIcon,7,(180+22),16,16);
				}
			}			
			DisplayMap_flash_WithCleanSreen();	
			
			if(LayOut[LayOutIndex*4]!=0x40)
			{
				Display_PhoneRecordTitle(LayOutIndex*4,D_MODE4,WHITE,30,18,140,24);
				Display_PhoneRecordDate_Preview(LayOutIndex*4,D_MODE0,GRAY,140 + 30,(18),70,24);
			}
			if(LayOutIndex==0)
			{			
				if(LayOut[LayOutIndex*4+1]!=0x40)
				{
					Display_PhoneRecordTitle(LayOutIndex*4+1,D_MODE4,WHITE,30,(60+18),140,24);
					Display_PhoneRecordDate_Preview(LayOutIndex*4+1,D_MODE0,GRAY,140 + 30,(60+18),70,24);
				}
			}
			if(LayOutIndex==0)
			{
				if(LayOut[LayOutIndex*4+2]!=0x40)
				{
					Display_PhoneRecordTitle(LayOutIndex*4+2,D_MODE4,WHITE,30,(120+18),140,24);
					Display_PhoneRecordDate_Preview(LayOutIndex*4+2,D_MODE0,GRAY,140 + 30,(120+18),70,24);
				}
			}				
			if(LayOutIndex==0)
			{
				if(LayOut[LayOutIndex*4+3]!=0x40)
				{
					Display_PhoneRecordTitle(LayOutIndex*4+3,D_MODE4,WHITE,30,(180+18),140,24);
					Display_PhoneRecordDate_Preview(LayOutIndex*4+3,D_MODE0,GRAY,140 + 30,(180+18),70,24);
				}
			}				
						
		}	
		
		else if(atype==Ancs_Sms)				/*短信*/
		{
#if 0		
			if(LayOutIndex==0)
			{
				if(LayOut[LayOutIndex*4+1]!=0x20)
				{
					DisplayMapCleanAdd(D_INIT,setting_240x1_Line, 0, 60, 240, 1);		/*画直线*/
				}
			}
			if(LayOutIndex==0)
			{
				if(LayOut[LayOutIndex*4+2]!=0x20)
				{
					DisplayMapCleanAdd(D_ADD,setting_240x1_Line, 0, 120, 240, 1);	/*画直线*/
				}
			}	
			if(LayOutIndex==0)
			{
				if(LayOut[LayOutIndex*4+3]!=0x20)
				{
					DisplayMapCleanAdd(D_ADD,setting_240x1_Line, 0, 180, 240, 1);	/*画直线*/
				}
			}			
			DisplayMap_flash_WithCleanSreen();	
#else	
			//ClearFullScreen();
			NumberofPage = 3;
			if(LayOut[LayOutIndex*NumberofPage]!=0x40)
			{
				DisplayMapCleanAdd(D_INIT,notification_message_46x40 ,7,5,46,40);
			}
			//if(LayOutIndex==0)
			{	
				if(LayOut[LayOutIndex*NumberofPage+1]!=0x40)
				{
					DisplayMapCleanAdd(D_ADD,app_whiteline_240x1, 0, 80, 240, 1);			/*画直线*/
					DisplayMapCleanAdd(D_ADD,notification_message_46x40 ,7,(80+5),46,40);
				}
			}
			if(LayOutIndex==0)
			{
				if(LayOut[LayOutIndex*NumberofPage+2]!=0x40)
				{
					DisplayMapCleanAdd(D_ADD,app_whiteline_240x1, 0, 160, 240, 1);	/*画直线*/
					DisplayMapCleanAdd(D_ADD,notification_message_46x40 ,7,(160+5),46,40);
				}
			}
			
			DisplayMap_flash_WithCleanSreen();			
#endif

			if(LayOut[LayOutIndex*NumberofPage]!=0x40)
			{
				uint16_t Ccolor=GRAY;
				if(LayOut[LayOutIndex*NumberofPage]>=0x20) /*未读*/
					Ccolor = WHITE;			
				Display_AncsTitle(LayOutIndex*NumberofPage,D_MODE4,Ccolor,63,12,167,24);
				Display_AncsMsg_Preview(LayOutIndex*NumberofPage,D_MODE4,Ccolor,7,(12+38),223,24);
				//Display_AncsDate8x16_Preview(LayOutIndex*NumberofPage,D_MODE0,WHITE,194,(6+8),36,24);
			}
			//if(LayOutIndex==0)
			{			
				if(LayOut[LayOutIndex*NumberofPage+1]!=0x40)
				{
					uint16_t Ccolor=GRAY;
					if(LayOut[LayOutIndex*NumberofPage+1]>=0x20) /*未读*/
						Ccolor = WHITE;	
					Display_AncsTitle(LayOutIndex*NumberofPage+1,D_MODE4,Ccolor,63,(80+12),167,24);
					Display_AncsMsg_Preview(LayOutIndex*NumberofPage+1,D_MODE4,Ccolor,7,(80+12+38),223,24);
					//Display_AncsDate8x16_Preview(LayOutIndex*NumberofPage+1,D_MODE0,WHITE,194,(80+6+8),36,24);
				}
			}
			if(LayOutIndex==0)
			{
				if(LayOut[LayOutIndex*NumberofPage+2]!=0x40)
				{
					uint16_t Ccolor=GRAY;
					if(LayOut[LayOutIndex*NumberofPage+2]>=0x20) /*未读*/
						Ccolor = WHITE;					
					Display_AncsTitle(LayOutIndex*NumberofPage+2,D_MODE4,Ccolor,63,(160+12),167,24);
					Display_AncsMsg_Preview(LayOutIndex*NumberofPage+2,D_MODE4,Ccolor,7,(160+12+38),223,24);
					//Display_AncsDate8x16_Preview(LayOutIndex*NumberofPage+2,D_MODE0,WHITE,194,(160+6+8),36,24);
				}
			}			
		}
		else if(atype==Ancs_Email)				/*邮件*/
		{
			NumberofPage = 3;
			IconAddress = notification_email_42x30;
			
			if(LayOut[LayOutIndex*NumberofPage]!=0x40)
			{
				//DisplayMapCleanAdd(D_INIT,SmallMapAddr[(LenSave[LayOutIndex*NumberofPage][0])],6,20,40,40);
				DisplayMapCleanAdd(D_INIT,IconAddress,6,10,42,30);
			}
			if(LayOut[LayOutIndex*NumberofPage+1]!=0x40)
			{
				DisplayMapCleanAdd(D_ADD,app_whiteline_240x1, 0, 80, 240, 1);			/*画直线*/
				//DisplayMapCleanAdd(D_ADD,SmallMapAddr[(LenSave[LayOutIndex*NumberofPage+1][0])],6,(80+20),40,40);
				DisplayMapCleanAdd(D_ADD,IconAddress,6,(80+10),42,30);
			}
			if(LayOutIndex==0)
			{
				if(LayOut[LayOutIndex*NumberofPage+2]!=0x40)
				{
					DisplayMapCleanAdd(D_ADD,app_whiteline_240x1, 0, 160, 240, 1);			/*画直线*/
					//DisplayMapCleanAdd(D_ADD,SmallMapAddr[(LenSave[LayOutIndex*NumberofPage+2][0])],6,(160+20),40,40);
					DisplayMapCleanAdd(D_ADD,IconAddress,6,(160+10),42,30);
				}
			}
			DisplayMap_flash_WithCleanSreen();	
			
			if(LayOut[LayOutIndex*NumberofPage]!=0x40)
			{
				uint16_t Ccolor=GRAY;
				if(LayOut[LayOutIndex*NumberofPage]>=0x20) /*未读*/
					Ccolor = WHITE;					
				Display_AncsTitle(LayOutIndex*NumberofPage,D_MODE4,Ccolor,63,12,167,24);
				Display_AncsMsg_Preview(LayOutIndex*NumberofPage,D_MODE4,Ccolor,7,(12+38),223,24);
			}
			if(LayOut[LayOutIndex*NumberofPage+1]!=0x40)
			{
				uint16_t Ccolor=GRAY;
				if(LayOut[LayOutIndex*NumberofPage+1]>=0x20) /*未读*/
					Ccolor = WHITE;					
				Display_AncsTitle(LayOutIndex*NumberofPage+1,D_MODE4,Ccolor,63,(80+12),167,24);
				Display_AncsMsg_Preview(LayOutIndex*NumberofPage+1,D_MODE4,Ccolor,7,(80+12+38),223,24);
			}
			if(LayOutIndex==0)
			{
				if(LayOut[LayOutIndex*NumberofPage+2]!=0x40)
				{
					uint16_t Ccolor=GRAY;
					if(LayOut[LayOutIndex*NumberofPage+2]>=0x20) /*未读*/
						Ccolor = WHITE;		
					Display_AncsTitle(LayOutIndex*NumberofPage+2,D_MODE4,Ccolor,63,(160+12),167,24);
					Display_AncsMsg_Preview(LayOutIndex*NumberofPage+2,D_MODE4,Ccolor,7,(160+12+38),223,24);
				}
			}
		}
		else if(atype==Ancs_Schedule)				/*日程*/
		{
			NumberofPage = 3;
			IconAddress = notification_event_40x40;
			
			if(LayOut[LayOutIndex*NumberofPage]!=0x40)
			{
				//DisplayMapCleanAdd(D_INIT,SmallMapAddr[(LenSave[LayOutIndex*NumberofPage][0])],6,20,40,40);
				DisplayMapCleanAdd(D_INIT,IconAddress,7,5,40,40);
			}
			if(LayOut[LayOutIndex*NumberofPage+1]!=0x40)
			{
				DisplayMapCleanAdd(D_ADD,app_whiteline_240x1, 0, 80, 240, 1);			/*画直线*/
				//DisplayMapCleanAdd(D_ADD,SmallMapAddr[(LenSave[LayOutIndex*NumberofPage+1][0])],6,(80+20),40,40);
				DisplayMapCleanAdd(D_ADD,IconAddress,7,(80+5),40,40);
			}
			if(LayOutIndex==0)
			{
				if(LayOut[LayOutIndex*NumberofPage+2]!=0x40)
				{
					DisplayMapCleanAdd(D_ADD,app_whiteline_240x1, 0, 160, 240, 1);			/*画直线*/
					//DisplayMapCleanAdd(D_ADD,SmallMapAddr[(LenSave[LayOutIndex*NumberofPage+2][0])],6,(160+20),40,40);
					DisplayMapCleanAdd(D_ADD,IconAddress,7,(160+5),40,40);
				}
			}
			DisplayMap_flash_WithCleanSreen();	
			
			if(LayOut[LayOutIndex*NumberofPage]!=0x40)
			{
				uint16_t Ccolor=GRAY;
				if(LayOut[LayOutIndex*NumberofPage]>=0x20) /*未读*/
					Ccolor = WHITE;					
				Display_AncsTitle(LayOutIndex*NumberofPage,D_MODE4,Ccolor,63,12,167,24);
				//Display_AncsMsg_Preview(LayOutIndex*NumberofPage,D_MODE4,WHITE,7,(12+38),223,24);
				Display_AncsDate_Preview(LayOutIndex*NumberofPage,D_MODE0,Ccolor,7,(12+38),223,24);
			}
			if(LayOut[LayOutIndex*NumberofPage+1]!=0x40)
			{
				uint16_t Ccolor=GRAY;
				if(LayOut[LayOutIndex*NumberofPage+1]>=0x20) /*未读*/
					Ccolor = WHITE;					
				Display_AncsTitle(LayOutIndex*NumberofPage+1,D_MODE4,Ccolor,63,(80+12),167,24);
				//Display_AncsMsg_Preview(LayOutIndex*NumberofPage+1,D_MODE4,WHITE,7,(80+12+38),223,24);
				Display_AncsDate_Preview(LayOutIndex*NumberofPage+1,D_MODE0,Ccolor,7,(80+12+38),223,24);
			}
			if(LayOutIndex==0)
			{
				if(LayOut[LayOutIndex*NumberofPage+2]!=0x40)
				{
					uint16_t Ccolor=GRAY;
					if(LayOut[LayOutIndex*NumberofPage+2]>=0x20) /*未读*/
						Ccolor = WHITE;		
					Display_AncsTitle(LayOutIndex*NumberofPage+2,D_MODE4,Ccolor,63,(160+12),167,24);
					//Display_AncsMsg_Preview(LayOutIndex*NumberofPage+2,D_MODE4,WHITE,7,(160+12+38),223,24);
					Display_AncsDate_Preview(LayOutIndex*NumberofPage+2,D_MODE0,Ccolor,7,(160+12+38),223,24);
				}
			}
		}		
		else if(atype>=Ancs_Social_Facebook)				/*社交*/
		{
			NumberofPage = 3;
			IconAddress = notification_social_40x40;
			
			if(LayOut[LayOutIndex*NumberofPage]!=0x40)
			{
				DisplayMapCleanAdd(D_INIT,SmallMapAddr[(LenSave[LayOutIndex*NumberofPage][0])],7,5,40,40);
				//DisplayMapCleanAdd(D_INIT,IconAddress,7,5,40,40);
			}
			if(LayOut[LayOutIndex*NumberofPage+1]!=0x40)
			{
				DisplayMapCleanAdd(D_ADD,app_whiteline_240x1, 0, 80, 240, 1);			/*画直线*/
				DisplayMapCleanAdd(D_ADD,SmallMapAddr[(LenSave[LayOutIndex*NumberofPage+1][0])],7,(80+5),40,40);
				//DisplayMapCleanAdd(D_ADD,IconAddress,7,(80+5),40,40);
			}
			if(LayOutIndex==0)
			{
				if(LayOut[LayOutIndex*NumberofPage+2]!=0x40)
				{
					DisplayMapCleanAdd(D_ADD,app_whiteline_240x1, 0, 160, 240, 1);			/*画直线*/
					DisplayMapCleanAdd(D_ADD,SmallMapAddr[(LenSave[LayOutIndex*NumberofPage+2][0])],7,(160+5),40,40);
					//DisplayMapCleanAdd(D_ADD,IconAddress,7,(160+5),40,40);
				}
			}
			DisplayMap_flash_WithCleanSreen();	
			
			if(LayOut[LayOutIndex*NumberofPage]!=0x40)
			{
				uint16_t Ccolor=GRAY;
				if(LayOut[LayOutIndex*NumberofPage]>=0x20) /*未读*/
					Ccolor = WHITE;					
				Display_AncsTitle(LayOutIndex*NumberofPage,D_MODE4,Ccolor,63,12,167,24);
				Display_AncsMsg_Preview(LayOutIndex*NumberofPage,D_MODE4,Ccolor,7,(12+38),223,24);
			}
			if(LayOut[LayOutIndex*NumberofPage+1]!=0x40)
			{
				uint16_t Ccolor=GRAY;
				if(LayOut[LayOutIndex*NumberofPage+1]>=0x20) /*未读*/
					Ccolor = WHITE;					
				Display_AncsTitle(LayOutIndex*NumberofPage+1,D_MODE4,Ccolor,63,(80+12),167,24);
				Display_AncsMsg_Preview(LayOutIndex*NumberofPage+1,D_MODE4,Ccolor,7,(80+12+38),223,24);
			}
			if(LayOutIndex==0)
			{
				if(LayOut[LayOutIndex*NumberofPage+2]!=0x40)
				{
					uint16_t Ccolor=GRAY;
					if(LayOut[LayOutIndex*NumberofPage+2]>=0x20) /*未读*/
						Ccolor = WHITE;		
					Display_AncsTitle(LayOutIndex*NumberofPage+2,D_MODE4,Ccolor,63,(160+12),167,24);
					Display_AncsMsg_Preview(LayOutIndex*NumberofPage+2,D_MODE4,Ccolor,7,(160+12+38),223,24);
				}
			}
		}
					
	}
	else									/*显示Detailview*/
	{
		/*翻页处理:前翻 后翻*/
		switch(DetailDir)
		{
			case De_INIT:
				DetailOffset =DetailPage[DetailPageIndex];
				break;

			case De_NEXT:				
				if(DetailPageIndex>=5||DetailOffset==0xFF)		/*只支持显示5页,显示完成不翻页*/
					return 0;
				else
					DetailPageIndex++;				
				break;

			case De_PRE:
				if(DetailPageIndex==0)
					return 0;
				else
					DetailPageIndex--;
				DetailOffset =DetailPage[DetailPageIndex];
				break;
		}
		switch(atype)
		{
			case Ancs_MissedCall:
				NumberofPage =3;
				if(LayOut[LayOutIndex*NumberofPage+DetailIndex]!=0x40)
				{
					StopRoll();
					Display_AncsTitle(LayOutIndex*NumberofPage+DetailIndex,D_MODE2,WHITE,57,(12+80*DetailIndex),173,24);
				}
				break;
				
			case Ancs_Sms:			/*短信*/
				NumberofPage =3;
				if(LayOut[LayOutIndex*NumberofPage+DetailIndex]!=0x40)
				{
					//if(SmsFromAndroid==0)		/*Iphone*/
					{
						//ClearFullScreen();
						DisplayMapCleanAdd(D_INIT,notification_message_46x40,7,10,46,40);
						DisplayMap_flash_WithCleanSreen();						
						Display_AncsTitle(LayOutIndex*NumberofPage+DetailIndex,D_MODE2,WHITE,63,18,95,24);
						Display_AncsDate_Preview2(LayOutIndex*NumberofPage+DetailIndex,D_MODE0,WHITE,168,18,72,24);
						Display_AncsMsg(LayOutIndex*NumberofPage+DetailIndex,D_MODE0,WHITE,20,80,200,100);
					}
#if 0
					else							/*Android*/
					{
						DisplayMapCleanAdd(D_INIT,SMS_114x54_reply,63,180,114,54);	/*Reply*/
						DisplayMap_flash_WithCleanSreen();	
						Display_AncsTitle(LayOutIndex*NumberofPage+DetailIndex,D_MODE3,WHITE,40,25,160,24); 
						Display_AncsMsg(LayOutIndex*NumberofPage+DetailIndex,D_MODE0,WHITE,10,65,220,115);			
					}
#endif
				}
				break;
			case Ancs_Email:			/*邮件*/
				NumberofPage =3;
				if(atype==Ancs_Email)
				IconAddress = notification_email_42x30;
				if(LayOut[LayOutIndex*NumberofPage+DetailIndex]!=0x40)
				{
					//DisplayMapCleanAdd(D_INIT,SmallMapAddr[(LenSave[LayOutIndex*3+DetailIndex][0])],100,10,40,40);
					DisplayMapCleanAdd(D_INIT,IconAddress,7,15,42,30);
					DisplayMap_flash_WithCleanSreen();	
					Display_AncsTitle(LayOutIndex*NumberofPage+DetailIndex,D_MODE2,WHITE,59,18,99,24);
					Display_AncsDate_Preview2(LayOutIndex*NumberofPage+DetailIndex,D_MODE0,WHITE,168,18,72,24);
					Display_AncsMsg(LayOutIndex*NumberofPage+DetailIndex,D_MODE0,WHITE,20,80,200,100);		
				}
				break;
			case Ancs_Schedule:			/*日程*/
				NumberofPage =3;
				if(atype==Ancs_Schedule)
				IconAddress = notification_event_40x40;
				if(LayOut[LayOutIndex*NumberofPage+DetailIndex]!=0x40)
				{
					//DisplayMapCleanAdd(D_INIT,SmallMapAddr[(LenSave[LayOutIndex*3+DetailIndex][0])],100,10,40,40);
					DisplayMapCleanAdd(D_INIT,IconAddress,7,10,40,40);
					DisplayMap_flash_WithCleanSreen();	
					//Display_AncsTitle(LayOutIndex*NumberofPage+DetailIndex,D_MODE2,WHITE,57,18,101,24);
					//Display_AncsDate_Preview2(LayOutIndex*NumberofPage+DetailIndex,D_MODE0,WHITE,168,18,72,24);
					Display_AncsDate(LayOutIndex*NumberofPage+DetailIndex,D_MODE2,WHITE,57,18,173,24);
					Display_AncsMsg(LayOutIndex*NumberofPage+DetailIndex,D_MODE0,WHITE,20,80,200,100);		
				}
				break;			
			case Ancs_Social:			/*社交*/
			case Ancs_Social_Facebook:
			case Ancs_Social_Gmail:
			case Ancs_Social_Hangouts:
			case Ancs_Social_Instagram:
			case Ancs_Social_Line:
			case Ancs_Social_Linkedin:
			case Ancs_Social_Messenger:
			case Ancs_Social_QQ:
			case Ancs_Social_Skype:
			case Ancs_Social_Snapchat:
			case Ancs_Social_Twitter:
			case Ancs_Social_Uber:
			case Ancs_Social_Viber:
			case Ancs_Social_Wechat:
			case Ancs_Social_WhatsApp:
				NumberofPage =3;
				IconAddress = notification_social_40x40;
				if(LayOut[LayOutIndex*NumberofPage+DetailIndex]!=0x40)
				{
					DisplayMapCleanAdd(D_INIT,SmallMapAddr[(LenSave[LayOutIndex*3+DetailIndex][0])],7,10,40,40);
					//DisplayMapCleanAdd(D_INIT,IconAddress,7,10,40,40);
					DisplayMap_flash_WithCleanSreen();	
					Display_AncsTitle(LayOutIndex*NumberofPage+DetailIndex,D_MODE2,WHITE,57,18,101,24);
					Display_AncsDate_Preview2(LayOutIndex*NumberofPage+DetailIndex,D_MODE0,WHITE,168,18,72,24);
					Display_AncsMsg(LayOutIndex*NumberofPage+DetailIndex,D_MODE0,WHITE,20,80,200,100);		
				}
				break;
		}
	}	
	return 0;
}


void AncsTest(void)
{
	uint8_t buf[63+4]={0x00};
	uint8_t dat0[24]={0x41,0xc3,0xa4,0xd0,0x96,0xd0,0x95,0xcc,0x88,0xe9,0x99,0x88,0xe4,0xbb,0xa3,0xe3,0x82,0x88,0xe0,0xb8,0xa2,0xec,0xa4,0x91};
	uint8_t dat1[] ="hello,how are you? hahahahah";
	uint8_t dat2[]={0x32,0x30,0x31,0x34,0x31,0x30,0x31,0x31,0x54,0x31,0x34,0x31,0x32,0x33,0x30};
	buf[1]=24;
	buf[2]=24;
	buf[3]=15;
	memcpy(buf+4,dat0,24);
	memcpy(buf+4+24,dat1,24);
	memcpy(buf+4+24+24,dat2,15);
	
	SpiFlash_4KSector_Erase(0x1B0000UL);
	SpiFlash_Write(0x1B0000UL,buf,63+4);
	
	//Display_AncsTitle(0x1B0000UL,D_MODE1,GREEN,20,20,200,20);	
	//Display_AncsTitle(0x1B0000UL,D_MODE4,WHITE,60,60,100,20);
	//Display_AncsTitle(0x1B0000UL,D_MODE2,SLEEP_COLOR,80,40,100,20);
	//Display_AncsMsg_Preview(0x1B0000UL,D_MODE4,GRAY,60,80,100,20);
	//Display_AncsDate_Preview(0x1B0000UL,D_MODE0,GRAY,170,60,60,20);
	//Display_AncsTitle(0x1B0000UL,D_MODE3,WHITE,50,24,140,16);	
	//Display_AncsMsg(0x1B0000UL,D_MODE0,WHITE,20,60,200,72);
}



