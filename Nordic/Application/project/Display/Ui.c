
#include "headfile.h"

uint8 G_HOUR_FORMAT =1;
uint8 display_index = 0;
uint8 STflag=0;
uint8 LastStepLen=0,LastCalorLen=0,LastDistanceLen=0;
shortcut_t shortcut_info;
PhoneSound_t PhoneSound;
PhoneMic_t PhoneMic;

uint8 BleDisable= 0;
sleep_info_t sleep_info;
sleep_overview_t sleep_overview_1,sleep_overview_2,sleep_overview_3;
sleep_overview_t sleep_overview_4,sleep_overview_5,sleep_overview_6,sleep_overview_7;
sleep_overview_t sleep_overview_detail;
sleep_info_t	sleep_info_1,sleep_info_detail;

setting_info_t setting_info;
watch_info_t watch_info;


/******************************************************************************************
时间显示
******************************************************************************************/
void InterfaceBatTimeDis(MainDataI MenuData)
{
	uint8 buf[5]={0x00};

	UTCTimeStruct uTime;
	gOS_GetUTCTime(&uTime); 
	RefreshBatteryInfo();
	
	buf[0] = uTime.hour;
	buf[1] = uTime.minutes;
	buf[2] = TimeOrder.TimeFormat;	
	buf[3] = shortcut_info.battery_level;
	buf[4] = shortcut_info.battery_percent;

	LCD_MainDataInterface(MenuData,buf,5);
}

void InterfaceDataWithBatTimeDis(MainDataI MenuData, uint8_t * Databuf, uint8_t Datalen)
{
	if(Datalen>120)
		Datalen=120;
	
	uint8 buf[128]={0x00};
	uint8_t len = Datalen;

	UTCTimeStruct uTime;
	gOS_GetUTCTime(&uTime); 
	RefreshBatteryInfo();
	
	for(uint8_t i=0;i<len;i++)
		buf[i] = *(Databuf+i);
	buf[len++] = uTime.hour;
	buf[len++] = uTime.minutes;
	buf[len++] = TimeOrder.TimeFormat;	
	buf[len++] = shortcut_info.battery_level;
	buf[len++] = shortcut_info.battery_percent;

	LCD_MainDataInterface(MenuData,buf,len);
}

void DisplayStringNumber_7x16(char * str, uint8 x, uint8 y)
{
	uint16 StringLen=0;
	uint16 posx = x, posy = y;
	if(str != NULL) {
		StringLen=strlen(str);  
		for(uint8 i=0;i<StringLen;i++)
		{
			if(str[i]=='-')
			{
				DisplayMap(&NumberMapRes7x16[10][0], posx, posy, 5, 16);
				posx += 5;
			}			
			else if(str[i]>='0' && str[i]<='9')
			{
				DisplayMap(&NumberMapRes7x16[str[i]-'0'][0], posx, posy, 7, 16); 
				posx += 7;
			}
			if(posx+7>LCD_SIZE_W)
				break;
		}
	}
}

int CalclateWeeks(int year, int month, int day)
{
	uint8 i;
	uint8  monthDays[12] = {31,28,31,30,31,30,31,31,30,31,30,31};
	uint16 days = 1;

	days  = (year - 1) * 365;
	days += year/4;

	for(i=0;i<month;i++)
	{
		days += monthDays[i];
	}
	days -= (monthDays[month-1] - day);
	if(IsLeapYear(2000+year))
	{
		if((month<2) || (month == 2 && day<29)) 
			days--;
	}

	return (days)%7;
}
APMformat HourFormat(uint8 * hour)
{
	if(*hour == 0) 
	{
		*hour = 12;
		return AM_FORMAT;
	}                
	else if(*hour < 12) 
	{
		return AM_FORMAT;
	}
	else if(*hour == 12) 
	{
		return PM_FORMAT;
	}
	else
	{
		*hour = *hour - 12;      
		return PM_FORMAT;
	}            
}

void DisplayWatchAPM(APMformat ampmformat,uint8 x,uint8 y)
{
	if(ampmformat==AM_FORMAT)
		DisplayMap(&APMRes7x24[0][0],x,y,7,24);              /*显示AM*/ 
	else
		DisplayMap(&APMRes7x24[1][0],x,y,7,24);               /*显示PM*/
}

void DisplayWatchTime(uint8 Hours,uint8 Minutes,uint8 x,uint8 y)
{
	uint16 StringLen=0;
	char TempShowNumber[10];
	uint8 OledX = 0;
    	if((9 == G_HOUR_FORMAT)||(10 == G_HOUR_FORMAT))
		sprintf(TempShowNumber,"%.2d-%.2d",Hours,Minutes);
	else
		sprintf(TempShowNumber,"%.2d:%.2d",Hours,Minutes);
	StringLen=strlen(TempShowNumber);
	for(uint8 i=0;i<StringLen;i++)
	{
		if(TempShowNumber[i]==':')
		{
			DisplayMap(&NumberMapRes9x24[10][0],x+OledX,y,4,24);
			OledX += 4;
		}
		else if(TempShowNumber[i]=='-')
		{
			DisplayMap(&NumberMapRes9x24[12][0],x+OledX,y,4,24);
			OledX += 4;
		}		
		else if (TempShowNumber[i]>='0' && TempShowNumber[i]<='9' )
		{
			DisplayMap(&NumberMapRes9x24[TempShowNumber[i]-'0'][0],x+OledX,y,9,24); 
			OledX += 9;
		}
		if(x+OledX>LCD_SIZE_W)
			break;
	}
}

void DisplaySecondTimerTime(uint8 Minutes,uint8 Seconds,uint8 mSeconds,uint8 x,uint8 y,uint8 Sy)
{
	uint16 StringLen=0;
	char TempShowNumber[10];
	uint8 OledX = 0;

	sprintf(TempShowNumber,"%.2d:%.2d.%.1d",Minutes,Seconds,mSeconds);
	StringLen=strlen(TempShowNumber);
	for(uint8 i=0;i<StringLen-2;i++)
	{
		if(TempShowNumber[i]==':')
		{
			DisplayMap(&NumberMapRes9x24[10][0],x+OledX,y,4,24);
			OledX += 4;
		}
		else if(TempShowNumber[i]=='-')
		{
			DisplayMap(&NumberMapRes9x24[12][0],x+OledX,y,4,24);
			OledX += 4;
		}		
		else if (TempShowNumber[i]>='0' && TempShowNumber[i]<='9' )
		{
			DisplayMap(&NumberMapRes9x24[TempShowNumber[i]-'0'][0],x+OledX,y,9,24); 
			OledX += 9;
		}
		if(x+OledX>LCD_SIZE_W)
			break;
	}
	for(uint8 i=StringLen-2;i<StringLen;i++)
	{
		if(TempShowNumber[i]==':')
		{
			DisplayMap(&NumberMapRes7x16[11][0],x+OledX,Sy,5,16);
			OledX += 5;
		}
		else if(TempShowNumber[i]=='.')
		{
			DisplayMap(&NumberMapRes7x16[13][0],x+OledX,Sy,5,16);
			OledX += 5;
		}		
		else if (TempShowNumber[i]>='0' && TempShowNumber[i]<='9' )
		{
			DisplayMap(&NumberMapRes7x16[TempShowNumber[i]-'0'][0],x+OledX,Sy,7,16); 
			OledX += 7;
		}
		if(x+OledX>LCD_SIZE_W)
			break;
	}
}

void DisplayWatchDate(uint8 Month,uint8 Day,uint8 x,uint8 y)
{
	char TempShowNumber[16];
	sprintf(TempShowNumber, "%.2d-%.2d", Month, Day);
	DisplayStringNumber_7x16(TempShowNumber, x, y);
}

void DisplayWatchWeek(uint8 Year,uint8 Month,uint8 Day,uint8 x,uint8 y)
{
	int week = 0;
	week=CalclateWeeks(Year, Month, Day);
	DisplayMap(&WeekRes17x16[week][0],x,y,17,16);		
}


void ShowTime(UTCTimeStruct uUtcTime, uint8 is_clear)
{	
	uint8 hour=uUtcTime.hour;
	uint8 minute=uUtcTime.minutes;
	if(is_clear == 1)
		ClearLcdDisplay(0, 0, 64, 48); 
	G_HOUR_FORMAT = 1;
	
       if(TimeOrder.DateFormat==0x04 && TimeOrder.TimeFormat==0x02 && TimeOrder.BatFormat==0x01 )
	   	G_HOUR_FORMAT =1;
	else if(TimeOrder.DateFormat==0x04 && TimeOrder.TimeFormat==0x01 && TimeOrder.BatFormat==0x01) 
		G_HOUR_FORMAT =2;
	else if(TimeOrder.DateFormat==0x00 && TimeOrder.TimeFormat==0x02 && TimeOrder.BatFormat==0x00) 
		G_HOUR_FORMAT =3;
	else if(TimeOrder.DateFormat==0x00 && TimeOrder.TimeFormat==0x01 && TimeOrder.BatFormat==0x00) 
		G_HOUR_FORMAT =4;
	else if(TimeOrder.DateFormat==0x04 && TimeOrder.TimeFormat==0x02 && TimeOrder.BatFormat==0x00) 
		G_HOUR_FORMAT =5;
	else if(TimeOrder.DateFormat==0x04 && TimeOrder.TimeFormat==0x01 && TimeOrder.BatFormat==0x00) 
		G_HOUR_FORMAT =6;
	else if(TimeOrder.DateFormat==0x00 && TimeOrder.TimeFormat==0x02 && TimeOrder.BatFormat==0x01) 
		G_HOUR_FORMAT =7;
	else if(TimeOrder.DateFormat==0x00 && TimeOrder.TimeFormat==0x01 && TimeOrder.BatFormat==0x01) 
		G_HOUR_FORMAT =8;

	switch(G_HOUR_FORMAT)
	{
		case 1:
			DisplayWatchAPM(HourFormat(&hour),49,16);								/*显示AM_PM*/ 
			DisplayWatchTime(hour,minute,8,16);                         			/*显示时间*/
			DisplayWatchDate(uUtcTime.month,uUtcTime.day,24,0);						/*显示日期*/		
			DisplayWatchWeek(uUtcTime.year%100,uUtcTime.month,uUtcTime.day,7,0);	/*显示星期*/ 
			UpdateBatteryStatus(0,0);												/*显示电池*/ 
			break;
		case 2:
			DisplayWatchTime(hour,minute,12,16);                                  					/*显示时间*/
			DisplayWatchDate(uUtcTime.month,uUtcTime.day,24,0);						/*显示日期*/		
			DisplayWatchWeek(uUtcTime.year%100,uUtcTime.month,uUtcTime.day,7,0);	/*显示星期*/ 
			UpdateBatteryStatus(0,0);												/*显示电池*/ 
			break;
		case 3:
			DisplayWatchAPM(HourFormat(&hour),49,16);  								/*显示AM_PM*/ 
			DisplayWatchTime(hour,minute,8,16);                                  					/*显示时间*/
			break;
		case 4:
			DisplayWatchTime(hour,minute,8,16);                                  					/*显示时间*/	
			break;
		case 5:
			DisplayWatchAPM(HourFormat(&hour),49,24);								/*显示AM_PM*/ 
			DisplayWatchTime(hour,minute,8,24);                         							/*显示时间*/
			DisplayWatchDate(uUtcTime.month,uUtcTime.day,24,8);						/*显示日期*/		
			DisplayWatchWeek(uUtcTime.year%100,uUtcTime.month,uUtcTime.day,7,8);	/*显示星期*/ 			
			break;
		case 6:
			DisplayWatchTime(hour,minute,12,24);                         						/*显示时间*/
			DisplayWatchDate(uUtcTime.month,uUtcTime.day,24,8);						/*显示日期*/		
			DisplayWatchWeek(uUtcTime.year%100,uUtcTime.month,uUtcTime.day,7,8);	/*显示星期*/ 		
			break;
		case 7:
			DisplayWatchAPM(HourFormat(&hour),49,8);  								/*显示AM_PM*/ 
			DisplayWatchTime(hour,minute,8,8);                                  						/*显示时间*/
			UpdateBatteryStatus(0,0);			
			break;
		case 8:
			DisplayWatchTime(hour,minute,12,8);                                  					/*显示时间*/
			UpdateBatteryStatus(0,0);												/*显示电池*/ 		
			break;
		default:
			break;
	}
}

void ShowStandby(UTCTimeStruct* uUtcTime, uint8 type)		/*To KL17*/
{
	uint8 buf[12]={0x00};
	uint16_encode(uUtcTime->year,buf);
	buf[2] = uUtcTime->month;
	buf[3] = uUtcTime->day;
	buf[4] = uUtcTime->hour;
	buf[5] = uUtcTime->minutes;
	buf[6] = GetMonthWeekDays(uUtcTime->year,uUtcTime->month,uUtcTime->day);  //0~6(日~六)//	
	buf[7] = type+0x30;
	buf[8] = TimerMenuFlag;

/*	
	buf[9] = SetValue.DistanceFormat;	//0公里 1英里	
	uint32_encode(gSportDisplay.SportDistance,(buf+10));		//距离单位:米
	uint32_encode(gSportDisplay.SportDuration,(buf+14));	//运动时长单位:分钟
*/	
	RefreshBatteryInfo();
	buf[9] = TimeOrder.TimeFormat;	
	buf[10] = shortcut_info.battery_level;
	buf[11] = shortcut_info.battery_percent;
	
	LCD_MainDataInterface(MI_TIME,buf,12);

}

void RefreshTimeUIFun(uint8 TimerMenu_Index)
{
	uint8 buf[6]={0x00};
	uint8 DisOption =0;
	uint8 DisState =0;    //0: OFF; 1: ON
	uint32 Value = 0;
	switch(gTimeDisMode%5)
		{
			case HR_OPTION:
				DisOption =HR_OPTION;
				DisState =HrIsRunning();
				Value = gHeartrateDisplay.HeartRateVal;				
				break;			
			case STEP_OPTION:
				DisOption =STEP_OPTION;
				DisState =0;
				Value = gSportDisplay.SportSteps;						
				break;			
			case CALOR_OPTION:
				DisOption =CALOR_OPTION;
				DisState =0;
				Value = gSportDisplay.SportKcal/1000;				
				break;			
			case DURATION_OPTION:
				DisOption =DURATION_OPTION;
				DisState =0;
				Value = gSportDisplay.SportDuration;				
				break;
			case DISTANCE_OPTION:
				DisOption =DISTANCE_OPTION;
				DisState =SetValue.DistanceFormat;	//0公里 1英里;
				Value = gSportDisplay.SportDistance;				
				break;
			default:
				break;
		}

	buf[0] = DisOption;	
	buf[1] = DisState;
	buf[2] = TimerMenu_Index;
	uint32_encode(Value,(buf+3));
	//buf[6] = TimerMenu_Index;
	LCD_MainDataInterface(MI_REFRESH_TIME_UI_FUN,buf,7);
}


void ShowMusicControl(uint8 Mu)					/*To KL17*/	
{
	uint8 buf[3] = {0x00};
	buf[0] = Mu;
	buf[1] = MusicVol/10;
	InterfaceDataWithBatTimeDis(MI_MUSIC_CONTROL,buf,2);
	if((MusicSet)Mu!=MS_RESUME)
		osal_start_timerEx(GetAppTaskId(), MSG_MUSIC_CONTROL_TIMEOUT, 1000);
}

void ShowMusic(uint8 Mu, bool ForceRefresh)			/*To KL17*/	
{
	uint8 len = 1;
	uint8 buf[129] = {0x00};
	if(ForceRefresh)
	{
		if(MusicStates==MS_PLAY||MusicStates==MS_STOP)
			MusicBuf[1]=1;
		else if(MusicStates==MS_CONNECT)
		{
			memset((char*)(MusicBuf),0,128);
			MusicSetting(MS_CHECK);
		}
	}
	MusicBuf[0]=Mu;
	if(MusicBuf[1]==1)
		len = 3+MusicBuf[2];
	else
		len = 2;
	memcpy(buf,MusicBuf,len);
	buf[len] = MusicVol/10;
	len = len + 1;
	InterfaceDataWithBatTimeDis(MI_MUSIC,buf,len);
}

void ShowCamera(uint8 Ca)							/*To KL17*/	
{	
	InterfaceDataWithBatTimeDis(MI_CAMERA,&Ca,1);
}

void ShowSearch(uint8 Su)							/*To KL17*/	
{
	//LCD_MainDataInterface(MI_SEARCH,&Su,1);
	InterfaceDataWithBatTimeDis(MI_SEARCH,&Su,1);
}

void ShowSearching(uint8 Su)							/*To KL17*/	
{
	//LCD_MainDataInterface(MI_SEARCHING,&Su,1);
	InterfaceDataWithBatTimeDis(MI_SEARCHING,&Su,1);
}

void ShowPaySetUp(void)
{
	LCD_PictureInterface(P_PAY_SET_UP);
}

void ShowPayProgress(void)
{
	LCD_PictureInterface(P_PAY_PROGRESS);
}


void ShowToolApps(uint8 index)					/*To KL17*/				
{	
	LCD_PictureInterface((PictureUI)(P_TOOL_APP1+(index-1)));
}

void ShowSettingApps(uint8 index)					/*To KL17*/				
{	
	uint8 buf[60] = {0x00};
	uint8 len = 0;
	if(index==1)
	{
		buf[0] = setting_info.wrist;		
		len = 1;
	}
	else if(index==2)
	{
		ReadWatchId(watch_info.WatchId);
		memcpy(watch_info.version, &SoftWareVersion[1], 3);	
		buf[0] = shortcut_info.vibration_level;
		buf[1] = shortcut_info.brightness_level;
		buf[2] = setting_info.wrist;
		memcpy(&buf[3],watch_info.WatchId, sizeof(watch_info));	
		len = sizeof(watch_info)+3;
	}
	LCD_MainDataInterface((MainDataI)(MI_SETTING_APP1+(index-1)),buf,len);
}

void ShowAppBox(void)					/*To KL17*/				
{	
	LCD_PictureInterface((PictureUI)(P_APP_BOX));
}

void ShowLanguageApps(uint8 index)					/*To KL17*/ 			
{	
	LCD_PictureInterface((PictureUI)(P_LANGUAGE_APP1+(index-1)));
}


void ShowBluetoothOnApps(void)						/*To KL17*/
{
	LCD_PictureInterface((PictureUI)P_BLUETOOTH_ON_STATE);
}

void ShowBluetoothOffApps(void)						/*To KL17*/
{
	LCD_PictureInterface((PictureUI)P_BLUETOOTH_OFF_STATE);
}

void ShowVirateOnApps(void)						/*To KL17*/
{
	LCD_PictureInterface((PictureUI)P_VIRATE_ON_STATE);
}

void ShowVirateOffApps(void)						/*To KL17*/
{
	LCD_PictureInterface((PictureUI)P_VIRATE_OFF_STATE);
}

void ShowWristOnApps(void)						/*To KL17*/
{
	LCD_PictureInterface((PictureUI)P_WRIST_ON_STATE);
}

void ShowWristOffApps(void)						/*To KL17*/
{
	LCD_PictureInterface((PictureUI)P_WRIST_OFF_STATE);
}
void ShowWristStateChange(uint8 state)
{
	uint8 buf[1]={0x00};	
	buf[0] = state;
	LCD_MainDataInterface(MI_WRIST_STATE_CHANGE,buf, 1);
}
void ShowCurrencyApps(void)						/*To KL17*/
{
	LCD_PictureInterface((PictureUI)P_CURRENCY_TYPE);
}

void ShowTimerMenu(uint8 index)					/*To KL17*/				
{	
	LCD_PictureInterface((PictureUI)(P_TIMER_MENU_ONE+(index-1)));
}

void ChooseLanguage(uint8 index)						/*To KL17*/				
{	
	LCD_PictureInterface((PictureUI)(P_LANGUAGE_CHOOSE_ONE+index));
}

void ShowTimerMenuUpdated(void)
{
	LCD_PictureInterface((PictureUI)P_TIMER_MENU_UPDATED);
}

void ShowSoundApps(void) 						/*To KL17*/
{
	LCD_PictureInterface((PictureUI)P_SOUND_STATE);
}

void ShowSwitchState(uint8 state, bool flash_write)		/*To KL17*/
{
	uint8 buf[2]={0x00};

	
	buf[0]= state;
	LCD_MainDataInterface(MI_SWITCH_ON_OFF, buf, 1);

	if (flash_write)
		WRITE_USER_PARAMETER((uint8 *)&setting_info, SETTING_LOC, sizeof(setting_info));
}

void ShowTool(uint8 index)							/*To KL17*/
{	
	UTCTimeStruct uTime;
	gOS_GetUTCTime(&uTime);
	uint8 buf[6]={0x00}; 
	buf[0] = index;
	buf[1] = uTime.hour;
	buf[2] = uTime.minutes;
	buf[3] = uTime.month;
	buf[4] = uTime.day;
	buf[5] = CalclateWeeks(uTime.year, uTime.month, uTime.day);
	LCD_MainDataInterface(MI_TOOL,buf, 6);
}

void ChangeGoals(void)							/*To KL17*/
{
	LCD_MainDataInterface(MI_CHANGE_GOALS,NULL, 0);	
}

void ShowSportOverview(uint8 ui_index)							/*To KL17*/
{
	uint8 buf[32]={0x00};
	buf[0] = ui_index;
	buf[1] = 0;
	uint32_encode(gSportDisplay.SportSteps,(buf+2));		//步数单位:步
	uint16_encode(DailyGoal[0].Goal,(buf+6));				//步数目标单位:百步
	uint32_encode(gSportDisplay.SportDistance,(buf+8));		//距离单位:米
	uint16_encode(DailyGoal[2].Goal,(buf+12));				//距离目标单位:千米	
	uint32_encode(gSportDisplay.SportKcal,(buf+14));		//卡路里单位:卡	
	uint16_encode(DailyGoal[1].Goal,(buf+18));				//步数目标单位:千卡
	uint32_encode(gSportDisplay.SportDuration,(buf+20));	//运动时长单位:分钟
	uint16_encode(DailyGoal[4].Goal,(buf+24));				//运动时长目标单位:分钟
	buf[26] = SetValue.DistanceFormat;	//0公里 1英里	
	LCD_MainDataInterface(MI_SPORT_OVER_VIEW,buf, 27);	
}

void ShowSportTotal(void)
{
	uint8 buf[8]={0x00};
	uint32 uSteps = gSportDisplay.SportSteps;
	uint32 uDistance = gSportDisplay.SportDistance;
	uint32 uCalor = gSportDisplay.SportKcal;
	uint32 uActDuration = gSportDisplay.SportDuration;
	UTCTimeStruct uTime;
	gOS_GetUTCTime(&uTime);
	CalculatePercent(uSteps,0,&buf[0]);
	CalculatePercent(uDistance,2,&buf[1]);
	CalculatePercent(uCalor,1,&buf[2]);
	CalculatePercent(uActDuration,4,&buf[3]);
	buf[4] = CalclateWeeks(uTime.year, uTime.month, uTime.day);
	buf[5] = uTime.month;
	buf[6] = uTime.day;
	
	LCD_MainDataInterface(MI_SPORTTOTAL,buf, 7);	
}


void ShowActivityDuration(uint32 uActDiration)		/*To KL17*/
{
	uint8 buf[10]={0x00};
	uint8 Min_ROP = 0;
	CalculatePercent(uActDiration, 4, &Min_ROP);
	uint32_encode(uActDiration,buf);
	buf[4] = Min_ROP;
	UTCTimeStruct uTime;
	gOS_GetUTCTime(&uTime);
	buf[5] = CalclateWeeks(uTime.year, uTime.month, uTime.day);
	buf[6] = uTime.month;
	buf[7] = uTime.day;
	LCD_MainDataInterface(MI_ACT_DIRATION,buf, 8);
}

void ShowSteps(uint32 uSteps)						/*To KL17*/
{
	uint8 buf[10]={0x00};
	uint8 Steps_ROP =0;
	CalculatePercent(uSteps, 0, &Steps_ROP);
	uint32_encode(uSteps,buf);
	buf[4] = Steps_ROP;
	UTCTimeStruct uTime;
	gOS_GetUTCTime(&uTime);
	buf[5] = CalclateWeeks(uTime.year, uTime.month, uTime.day);
	buf[6] = uTime.month;
	buf[7] = uTime.day;
	LCD_MainDataInterface(MI_SPORT,buf, 8);
}

void ShowCalor(uint32 uCalor)						/*To KL17*/
{
	uint8 buf[10]={0x00};
	uint8 Calor_ROP = 0;
	CalculatePercent(uCalor, 1, &Calor_ROP);
	uint32_encode((uCalor/1000),buf);
	buf[4] = Calor_ROP;
	UTCTimeStruct uTime;
	gOS_GetUTCTime(&uTime);
	buf[5] = CalclateWeeks(uTime.year, uTime.month, uTime.day);
	buf[6] = uTime.month;
	buf[7] = uTime.day;
	LCD_MainDataInterface(MI_CAL,buf, 8);
}

void ShowDistance(uint32 uDistance)				/*To KL17*/
{
	uint8 buf[10]={0x00};
	uint8 Distance_ROP = 0;
	CalculatePercent(uDistance, 2, &Distance_ROP);
	uint32_encode(uDistance,buf);
	buf[4] = Distance_ROP;	
	buf[5] = SetValue.DistanceFormat;	//0公里 1英里
	UTCTimeStruct uTime;
	gOS_GetUTCTime(&uTime);
	buf[6] = CalclateWeeks(uTime.year, uTime.month, uTime.day);
	buf[7] = uTime.month;
	buf[8] = uTime.day;
	LCD_MainDataInterface(MI_DISTANCE,buf, 9);
}

void ShowOtaLogo(void) 							/*To KL17*/
{
	LCD_PictureInterface((PictureUI)P_OTA_LOGO);
}

void ShowOtaResult(uint8 flag, uint8 result) 			/*To KL17*/
{
	if(result==0xff)
		return;
	if(flag==0 && (!UpdateKl17Success))
	{
		OtaSendCmd(O_PROGRESS,10);
	}
	else if(flag==1)
	{
		LCD_MainDataInterface(MI_OTA_RESULT,&result,1);
		nrf_delay_ms(3000);
	}
}

void ShowAncsSend(uint8 type) 						/*To KL17*/
{
	LCD_MainDataInterface(MI_ANCS_SEND,&type,1);
}

void ShowSmsReply(uint8 type) 						/*To KL17*/
{
	LCD_MainDataInterface(MI_ANCS_REPLY,&type,1);
}

void ShowLcdTest(uint8 Lcd)								/*To KL17*/
{
	LCD_MainDataInterface(MI_LCD_TEST,&Lcd,1);
}

void ShowTpTest(uint8 t_xy, uint8 t_id)						/*To KL17*/
{
	uint8 buf[2]={0x00};
	buf[0]=t_xy;
	buf[1]=t_id;
	LCD_MainDataInterface(MI_TP_TEST,buf,2);
}


void ShowBtConnect(void)								/*To KL17*/
{
	LCD_PictureInterface((PictureUI)P_BT_PAIRING);
}
void ShowBtPairSuccess(void)							/*To KL17*/
{
	LCD_PictureInterface((PictureUI)P_BT_PAIRING_SUCCESS);
}
void ShowBtPairFail(void)								/*To KL17*/
{
	LCD_PictureInterface((PictureUI)P_BT_PAIRING_FAIL);
}

void ShowVolume(void)								/*To KL17*/
{
	LCD_PictureInterface((PictureUI)P_VOL);
}
void ShowVolumeLevel(int level)						/*To KL17*/
{
	LCD_MainDataInterface(MI_VOL_LEV,(uint8*)&level,1);
}

void ShowSiri(uint8 State)								/*To KL17*/
{ 
	LCD_MainDataInterface(MI_SIRI_SWITCH,(uint8*)&State,1);
	if(State==BC5_SIRI_OPEN)
		osal_start_timerEx(GetAppTaskId(), MSG_SIRI_DISPLAY_OPEN, 500);
	else if(State==BC5_SIRI_CLOSE)
	{
		osal_stop_timerEx(GetAppTaskId(), MSG_SIRI_DISPLAY_OPEN);
        osal_clear_event(GetAppTaskId(), MSG_SIRI_DISPLAY_OPEN);
	}
}

void ShowPhoneDial(void)								/*To KL17*/  /*拨号表盘*/
{
	LCD_PictureInterface((PictureUI)P_DIAL);
}
void ShowPhoneDialNumber(uint8* number)			/*To KL17*/	/*拨号表盘拨出的电话号码，如果长度为0则不显示*/
{
	uint8 len = strlen((char*)number);
	LCD_MainDataInterface(MI_DIAL_NUM,number,len+1);	/*最后一个是0x00,方便freescale取长度*/	
}

void ShowPhoneDialNumber_Recover(uint8 number)		
{
	LCD_MainDataInterface(MI_DIAL_NUM_RECOVER,&number,1);	
}
void ShowPhoneDialNumber_Fade(uint8 number)		
{
	LCD_MainDataInterface(MI_DIAL_NUM_FADE,&number,1);	
}

void ShowPhoneDialIncallNumber_Recover(uint8 number)		
{
	LCD_MainDataInterface(MI_DIAL_INCALL_NUM_RECOVER,&number,1);	
}
void ShowPhoneDialIncallNumber_Fade(uint8 number)		
{
	LCD_MainDataInterface(MI_DIAL_INCALL_NUM_FADE,&number,1);	
}


void ShowPhoneDial_Incall(void)								/*To KL17*/  /*拨号表盘*/
{
	LCD_PictureInterface((PictureUI)P_DIAL_INCALL);
}
void ShowPhoneDialNumber_Incall(uint8* number)			/*To KL17*/	/*拨号表盘拨出的电话号码，如果长度为0则不显示*/
{
	uint8 len = strlen((char*)number);
	LCD_MainDataInterface(MI_DIAL_NUM_INCALL,number,len+1);	/*最后一个是0x00,方便freescale取长度*/	
}

/******************************************************************************************
运动/睡眠/支付显示
******************************************************************************************/
void DisplayStringNumber9x24(char * str, uint8 x, uint8 y)
{
	uint16 StringLen=0;
	uint16 posx = x, posy = y;
	if(str != NULL) {
		StringLen=strlen(str);  
		for(uint8 i=0;i<StringLen;i++)
		{
			if(str[i]=='.')
			{
				DisplayMap(&NumberMapRes9x24[11][0], posx, posy, 4, 24);
				posx += 4;
			}
			if(str[i]=='-')
			{
				DisplayMap(&NumberMapRes9x24[12][0], posx, posy, 4, 24);
				posx += 4;
			}
			else if(str[i]==':')
			{
				DisplayMap(&NumberMapRes9x24[10][0], posx, posy, 4, 24); 
				posx += 4;
			}
			else if(str[i]>='0' && str[i]<='9')
			{
				DisplayMap(&NumberMapRes9x24[str[i]-'0'][0], posx, posy, 9, 24); 
				posx += 9;
			}
			if(posx+9>LCD_SIZE_W)
				break;
		}
	}
}


void ShowSleepMonitor(uint16 quality, uint16 Minute , uint8 seconds, uint8 waiting, uint8 SleepOrNot)
{    
	uint8 buf[6]={0x00};
	uint16 hour = Minute/60;
	uint16 minius = Minute%60;	
	uint8 Sleep_ROP = 0;
	CalculatePercent(Minute,3,&Sleep_ROP);

	buf[0] = hour;
	buf[1] = minius;
	buf[2] = SleepOrNot;
	buf[3] = waiting;
	buf[4] = Sleep_ROP;
	//LCD_MainDataInterface(MI_SLEEP_TIME, buf,  5);
	InterfaceDataWithBatTimeDis(MI_SLEEP_TIME, buf, 5);
}

void ShowBalances(uint32 Balances)
{
	char TempShowNumber[10]={0x00};
	uint8 StringLen=0,posx =0;
	uint32 x = Balances/100;
	uint8 y = (Balances%100)/10; 
	//uint8 z = (Balances%100)%10; 
	sprintf(TempShowNumber,"%ld.%d",x,y);
	StringLen =strlen(TempShowNumber);
	if (StringLen > 6)
		StringLen = 6;
	posx = (LCD_SIZE_W - (StringLen-1)*9-4)/2;	

	if (display_index == 4) {
		display_index = 0;
	}
	DisplayMap(&BalancesMapRes24x24[display_index++][0],20,0,24,24);
	DisplayStringNumber9x24(TempShowNumber, posx, 24);  
	osal_start_timerEx(GetAppTaskId(), MSG_REPEAT_SCD_DISPLAY, 500);
}
void ShowCardID(uint8 * CardID)
{
}

/******************************************************************************************
ANCS推送界面
******************************************************************************************/
void DisplayEnString9x16(char * pCn,uint8 x,uint8 y)
{
	uint8 AsDot[18]={0x00};
	while (*pCn) {
		if (IS_ASCII(*pCn)) {
			Get_Ascii_Dot(AsDot,*pCn);
			DisplayMap(AsDot,x,y,9,16);
			pCn ++;
			x += FONT_WIDTH_ASCII;
		}
	}
}

void ShowIncomingcallIcon(void)
{
	DisplayMap(IncomingcallMapRes24x24, 20, 0, 24, 24);
}

void ShowNewMsg(uint16 count)
{
	char TempShowNumber[6]={0x00};
	uint8 StringLen=0,posx =0;
	if(count>=100)
	{
		count =99;	
		sprintf((char*)TempShowNumber, "%d+", count);
	}
	else
		sprintf((char*)TempShowNumber, "%d", count);
	StringLen = strlen(TempShowNumber);
	posx = (LCD_SIZE_W - (StringLen)*9-32)/2;
	ClearScreen();
	DisplayMap(AncsSms32x32, posx, 8, 32, 32);
	DisplayEnString9x16(TempShowNumber, posx+35, 16);
}


void ShowMissedCall(uint16 count)
{
	char TempShowNumber[6]={0x00};
	uint8 StringLen=0,posx =0;
	if(count>=100)
	{
		count =99;	
		sprintf((char*)TempShowNumber, "%d+", count);
	}
	else
		sprintf((char*)TempShowNumber, "%d", count);
	StringLen = strlen(TempShowNumber);
	posx = (LCD_SIZE_W - (StringLen)*9-32)/2;
	ClearScreen();
	DisplayMap(AncsMissCall32x32, posx, 8, 32, 32);
	DisplayEnString9x16(TempShowNumber, posx+32, 16);	
}


void ShowE_MailMsg(uint16 count)
{
	char TempShowNumber[6]={0x00};
	uint8 StringLen=0,posx =0;
	if(count>=100)
	{
		count =99;	
		sprintf((char*)TempShowNumber, "%d+", count);
	}
	else
		sprintf((char*)TempShowNumber, "%d", count);
	StringLen = strlen(TempShowNumber);
	posx = (LCD_SIZE_W - (StringLen)*9-32)/2;
	ClearScreen();
	DisplayMap(AncsEmail32x32, posx, 8, 32, 32);
	DisplayEnString9x16(TempShowNumber, posx+35, 16);
}


void ShowDailyScheduleMsg(uint16 count)
{
	char TempShowNumber[6]={0x00};
	uint8 StringLen=0,posx =0;
	if(count>=100)
	{
		count =99;	
		sprintf((char*)TempShowNumber, "%d+", count);
	}
	else
		sprintf((char*)TempShowNumber, "%d", count);
	StringLen = strlen(TempShowNumber);
	posx = (LCD_SIZE_W - (StringLen)*9-32)/2;
	ClearScreen();
	DisplayMap(AncsSchedule32x32, posx, 8, 32, 32);
	DisplayEnString9x16(TempShowNumber, posx+35, 16);
}

void ShowSocialMsg(uint16 count)
{
	char TempShowNumber[6]={0x00};
	uint8 StringLen=0,posx =0;
	if(count>=100)
	{
		count =99;	
		sprintf((char*)TempShowNumber, "%d+", count);
	}
	else
		sprintf((char*)TempShowNumber, "%d", count);
	StringLen = strlen(TempShowNumber);
	posx = (LCD_SIZE_W - (StringLen)*9-32)/2;
	ClearScreen();
	DisplayMap(AncsSocia32x32, posx, 8, 32, 32);
	DisplayEnString9x16(TempShowNumber, posx+35, 16);
}


/******************************************************************************************
工具界面
******************************************************************************************/
void DisplayStringLetter_6x16(char * str, uint8 x, uint8 y, bool Mid)
{
	uint16 StringLen=0;
	uint16 posx = x, posy = y;
	if(str != NULL) {
		StringLen=strlen(str);  
		if(Mid)
			posx = (LCD_SIZE_W-StringLen*6)/2;
		for(uint8 i=0;i<StringLen;i++)
		{		
			if(str[i]>='A' && str[i]<='Z')
			{
				DisplayMap(&LetterMapRes6x16[str[i]-'A'][0], posx, posy, 6, 16); 
				posx += 6;
			}
			else if(str[i]>='a' && str[i]<='z')
			{
				DisplayMap(&LetterMapRes6x16[(str[i]-'a')+27][0], posx, posy, 6, 16); 
				posx += 6;
			}			
			else if (str[i] == 0x20)
			{
				//DisplayMap(&LetterMapRes6x16[52][0], posx, posy, 6, 16);
				posx += 6;
			}
			else if (str[i] == 0x21)
			{
				DisplayMap(&LetterMapRes6x16[26][0], posx, posy, 6, 16);
				posx += 6;
			}
			if(posx+6>LCD_SIZE_W)
				break;
		}
	}
}



void ShowBack(void)
{   
	DisplayMap(Back24x24, 20, 0, 24, 24);
	DisplayStringLetter_6x16("BACK",0,32,true);
}


static void ReadSerialNum(uint8 * SerialNum)
{
	uint8 ReadWatchIdTemp[WATCH_ID_LEN]={0};
	uint8 i,temp;

	ReadWatchId(ReadWatchIdTemp);

	temp=0;
	for(i=7;i<12;i++)
	{
	    SerialNum[temp]=ReadWatchIdTemp[i];
	    temp++;
	}
	SerialNum[temp]='\0';
}

void ShowSerialNum(void)
{
	uint8 SerialNum[20]={0};

	ReadSerialNum(SerialNum);
	
	DisplayMap(HashKeyMapRes16x16,6,16,16,16);
	DisplayStringNumber_7x16((char *)&SerialNum[0], 22, 16);  
}


/******************************************************************************************
提醒界面
******************************************************************************************/
void ShowSetUp(void)
{
	ReadWatchId(watch_info.WatchId);
	memcpy(watch_info.version, &SoftWareVersion[1], 3);
	LCD_MainDataInterface(MI_SETUP,watch_info.WatchId, sizeof(watch_info));
}

void ShowReset(void)
{
	InterfaceBatTimeDis(MI_RESET);
}

void ShowResetProgress(uint8 level)
{
	LCD_MainDataInterface(MI_RESET_PROGRESS, &level, 1);
}

void ShowLogo(void)
{
	LCD_PictureInterface(P_LOGO);
}

void ShowUpdateDone(void)
{
	LCD_PictureInterface(P_UPDATE_DONE);
}

void ShowClearAll(void)
{
	LCD_PictureInterface(P_CLEARALL);
}

void ShowSleepStart(void)
{
	UTCTimeStruct uTime;
	uint8 buf[6]={0x00};
	uint8 Sleep_ROP = 0;
	gOS_GetUTCTime(&uTime);
	buf[0] = 0x00;
	buf[1] = 0x00;
	buf[2] = uTime.hour;
	buf[3] = uTime.minutes;
	buf[4] = Sleep_ROP;
	InterfaceDataWithBatTimeDis(MI_SLEEP_START, buf, 5);

}

void ShowSleepEnd(uint32 min)  //show a sun logo
{
	UTCTimeStruct uTime;
	uint8 buf[6]={0x00};
	uint8 Sleep_ROP = 0;
	CalculatePercent(min,3,&Sleep_ROP);
	gOS_GetUTCTime(&uTime);
	buf[0] = min/60;
	buf[1] = min%60;
	buf[2] = uTime.hour;
	buf[3] = uTime.minutes;
	buf[4] = Sleep_ROP;
	InterfaceDataWithBatTimeDis(MI_SLEEP_END, buf, 5);

}

void ShowSleepOverviewOne(void)
{
#if 0
	uint8 buf[21]={0x00};
	sleep_overview_1.state=0;
	sleep_overview_1.hour=8;
	sleep_overview_1.minute=20;
	sleep_overview_1.date=20;
	sleep_overview_1.month =12;	
	sleep_overview_1.year =2016;
	
	sleep_overview_2.state=1;
	sleep_overview_2.hour=7;
	sleep_overview_2.minute=35;
	sleep_overview_2.date=19;
	sleep_overview_2.month =12;	
	sleep_overview_2.year =2016;

	sleep_overview_3.state=0;
	sleep_overview_3.hour=9;
	sleep_overview_3.minute=12;
	sleep_overview_3.date=18;
	sleep_overview_3.month =12;	
	sleep_overview_3.year =2016;
	
	buf[0] = sleep_overview_1.state;
	buf[1] = sleep_overview_1.hour;
	buf[2] = sleep_overview_1.minute;
	buf[3] = sleep_overview_1.date;
	buf[4] = sleep_overview_1.month;
	uint16_encode(sleep_overview_1.year,&(buf[5]));

	buf[7] = sleep_overview_2.state;
	buf[8] = sleep_overview_2.hour;
	buf[9] = sleep_overview_2.minute;
	buf[10] = sleep_overview_2.date;
	buf[11] = sleep_overview_2.month;
	uint16_encode(sleep_overview_2.year,&(buf[12]));

	buf[14] = sleep_overview_3.state;
	buf[15] = sleep_overview_3.hour;
	buf[16] = sleep_overview_3.minute;
	buf[17] = sleep_overview_3.date;
	buf[18] = sleep_overview_3.month;
	uint16_encode(sleep_overview_3.year,&(buf[19]));

	LCD_MainDataInterface(MI_SLEEP_OVERVIEW_ONE, buf, 21);
#else
	uint8 buf[18]={0x00};
	if(SleepDayCount>0)			/*倒数第一个*/
	{
		uint16_encode(SleepDay[SleepDayCount-1].total,&(buf[0]));	/*总时间*/
		uint16_encode(SleepDay[SleepDayCount-1].year,&(buf[2]));	/*年*/
		buf[4]=SleepDay[SleepDayCount-1].month;					/*月*/
		buf[5]=SleepDay[SleepDayCount-1].day;					/*日*/
	}
	if(SleepDayCount>1)			/*倒数第二个*/
	{
		uint16_encode(SleepDay[SleepDayCount-2].total,&(buf[6]));
		uint16_encode(SleepDay[SleepDayCount-2].year,&(buf[8]));
		buf[10]=SleepDay[SleepDayCount-2].month;
		buf[11]=SleepDay[SleepDayCount-2].day;
	}
	if(SleepDayCount>2)			/*倒数第三个*/
	{
		uint16_encode(SleepDay[SleepDayCount-3].total,&(buf[12]));
		uint16_encode(SleepDay[SleepDayCount-3].year,&(buf[14]));
		buf[16]=SleepDay[SleepDayCount-3].month;
		buf[17]=SleepDay[SleepDayCount-3].day;
	}		
	LCD_MainDataInterface(MI_SLEEP_OVERVIEW_ONE, buf, 18);
#endif
}

void ShowSleepOverviewTwo(void)
{
#if 0
	uint8 buf[21]={0x00};
	sleep_overview_4.state=0;
	sleep_overview_4.hour=7;
	sleep_overview_4.minute=50;
	sleep_overview_4.date=17;
	sleep_overview_4.month =12;	
	sleep_overview_4.year =2016;
	
	sleep_overview_5.state=1;
	sleep_overview_5.hour=8;
	sleep_overview_5.minute=2;
	sleep_overview_5.date=16;
	sleep_overview_5.month =12;	
	sleep_overview_5.year =2016;

	sleep_overview_6.state=0;
	sleep_overview_6.hour=7;
	sleep_overview_6.minute=12;
	sleep_overview_6.date=15;
	sleep_overview_6.month =12;	
	sleep_overview_6.year =2016;
	
	buf[0] = sleep_overview_4.state;
	buf[1] = sleep_overview_4.hour;
	buf[2] = sleep_overview_4.minute;
	buf[3] = sleep_overview_4.date;
	buf[4] = sleep_overview_4.month;
	uint16_encode(sleep_overview_4.year,&(buf[5]));

	buf[7] = sleep_overview_5.state;
	buf[8] = sleep_overview_5.hour;
	buf[9] = sleep_overview_5.minute;
	buf[10] = sleep_overview_5.date;
	buf[11] = sleep_overview_5.month;
	uint16_encode(sleep_overview_5.year,&(buf[12]));

	buf[14] = sleep_overview_6.state;
	buf[15] = sleep_overview_6.hour;
	buf[16] = sleep_overview_6.minute;
	buf[17] = sleep_overview_6.date;
	buf[18] = sleep_overview_6.month;
	uint16_encode(sleep_overview_6.year,&(buf[19]));
	LCD_MainDataInterface(MI_SLEEP_OVERVIEW_TWO, buf, 21);
#else
	uint8 buf[18]={0x00};
	if(SleepDayCount>3)			/*倒数第四个*/
	{
		uint16_encode(SleepDay[SleepDayCount-4].total,&(buf[0]));	/*总时间*/
		uint16_encode(SleepDay[SleepDayCount-4].year,&(buf[2]));	/*年*/
		buf[4]=SleepDay[SleepDayCount-4].month;					/*月*/
		buf[5]=SleepDay[SleepDayCount-4].day;					/*日*/
	}
	if(SleepDayCount>4)			/*倒数第五个*/
	{
		uint16_encode(SleepDay[SleepDayCount-5].total,&(buf[6]));
		uint16_encode(SleepDay[SleepDayCount-5].year,&(buf[8]));
		buf[10]=SleepDay[SleepDayCount-5].month;
		buf[11]=SleepDay[SleepDayCount-5].day;
	}
	if(SleepDayCount>5)			/*倒数第六个*/
	{
		uint16_encode(SleepDay[SleepDayCount-6].total,&(buf[12]));
		uint16_encode(SleepDay[SleepDayCount-6].year,&(buf[14]));
		buf[16]=SleepDay[SleepDayCount-6].month;
		buf[17]=SleepDay[SleepDayCount-6].day;
	}		
	LCD_MainDataInterface(MI_SLEEP_OVERVIEW_TWO, buf, 18);
#endif
}
void ShowSleepOverviewThree(void)
{
#if 0
	uint8 buf[21]={0x00};
	sleep_overview_7.state=0;
	sleep_overview_7.hour=8;
	sleep_overview_7.minute=5;
	sleep_overview_7.date=14;
	sleep_overview_7.month =12;	
	sleep_overview_7.year =2016;
		
	buf[0] = sleep_overview_7.state;
	buf[1] = sleep_overview_7.hour;
	buf[2] = sleep_overview_7.minute;
	buf[3] = sleep_overview_7.date;
	buf[4] = sleep_overview_7.month;
	uint16_encode(sleep_overview_7.year,&(buf[5]));

	LCD_MainDataInterface(MI_SLEEP_OVERVIEW_THREE, buf, 7);
#else
	uint8 buf[18]={0x00};
	if(SleepDayCount>6)			/*倒数第七个*/
	{
		uint16_encode(SleepDay[SleepDayCount-7].total,&(buf[0]));	/*总时间*/
		uint16_encode(SleepDay[SleepDayCount-7].year,&(buf[2]));	/*年*/
		buf[4]=SleepDay[SleepDayCount-7].month;					/*月*/
		buf[5]=SleepDay[SleepDayCount-7].day;					/*日*/
	}
	LCD_MainDataInterface(MI_SLEEP_OVERVIEW_THREE, buf, 18);
#endif
}

void ShowSleepDetail(uint8 Which_date)
{
#if 0
	uint8 buf[20]={0x00};
		
	switch(Which_date)
	{
		case 0:
			memcpy(&sleep_overview_detail, &sleep_overview_1, sizeof(sleep_overview_t));
			memcpy(&sleep_info_detail, &sleep_info_1, sizeof(sleep_overview_t));
			break;
		case 1:
			memcpy(&sleep_overview_detail, &sleep_overview_2, sizeof(sleep_overview_t));
			memcpy(&sleep_info_detail, &sleep_info_1, sizeof(sleep_overview_t));
			break;
		case 2:
			memcpy(&sleep_overview_detail, &sleep_overview_3, sizeof(sleep_overview_t));
			memcpy(&sleep_info_detail, &sleep_info_1, sizeof(sleep_overview_t));
			break;
		case 3:
			memcpy(&sleep_overview_detail, &sleep_overview_4, sizeof(sleep_overview_t));
			memcpy(&sleep_info_detail, &sleep_info_1, sizeof(sleep_overview_t));
			break;
		case 4:
			memcpy(&sleep_overview_detail, &sleep_overview_5, sizeof(sleep_overview_t));
			memcpy(&sleep_info_detail, &sleep_info_1, sizeof(sleep_overview_t));
			break;
		case 5:
			memcpy(&sleep_overview_detail, &sleep_overview_6, sizeof(sleep_overview_t));
			memcpy(&sleep_info_detail, &sleep_info_1, sizeof(sleep_overview_t));
			break;
		case 6:
			memcpy(&sleep_overview_detail, &sleep_overview_7, sizeof(sleep_overview_t));
			memcpy(&sleep_info_detail, &sleep_info_1, sizeof(sleep_overview_t));
			break;
		default:
			break;
	}
	
		
	buf[0] = sleep_overview_detail.state;
	buf[1] = sleep_overview_detail.hour;
	buf[2] = sleep_overview_detail.minute;
	buf[3] = sleep_overview_detail.date;
	buf[4] = sleep_overview_detail.month;
	uint16_encode(sleep_overview_detail.year,&(buf[5]));
	uint16_encode(sleep_info_detail.total_sleep,&(buf[7]));
	uint16_encode(sleep_info_detail.fell_sleep,&(buf[9]));
	uint16_encode(sleep_info_detail.light_sleep,&(buf[11]));
	uint16_encode(sleep_info_detail.deep_sleep,&(buf[13]));
	uint16_encode(sleep_info_detail.awake_sleep,&(buf[15]));
	uint16_encode(sleep_info_detail.awake,&(buf[17]));
	
	InterfaceDataWithBatTimeDis(MI_SLEEP_DETAIL, buf, 19);
#else
	uint8 buf[12]={0x00};
	if(SleepDayCount>Which_date)			
	{
		uint16_encode(SleepDay[SleepDayCount-1-Which_date].total,&(buf[0]));	/*总时间*/
		uint16_encode(SleepDay[SleepDayCount-1-Which_date].year,&(buf[2]));	/*年*/
		buf[4]=SleepDay[SleepDayCount-1-Which_date].month;					/*月*/
		buf[5]=SleepDay[SleepDayCount-1-Which_date].day;					/*日*/
		uint16_encode(SleepDay[SleepDayCount-1-Which_date].Awake,&(buf[6]));/*清醒时间*/
		uint16_encode(SleepDay[SleepDayCount-1-Which_date].Light,&(buf[8]));	/*浅睡时间*/
		uint16_encode(SleepDay[SleepDayCount-1-Which_date].Deep,&(buf[10]));	/*深睡时间*/
	}		
	InterfaceDataWithBatTimeDis(MI_SLEEP_DETAIL, buf, 12);
#endif
}

void ShowSleepInfoOne(void)
{
	
	LCD_MainDataInterface(MI_SLEEP_INFO_ONE, (uint8*)&sleep_info, sizeof(sleep_info));

}

void ShowSleepInfoTwo(void)
{	
	LCD_MainDataInterface(MI_SLEEP_INFO_TWO, (uint8*)&sleep_info, sizeof(sleep_info));

}


void ShowBleBondQuery(void)
{
	InterfaceBatTimeDis(MI_BOND_QUERY);
}

void ShowBleBondOk(void)
{
	InterfaceBatTimeDis(MI_BOND_OK);
}

void ShowBleBondFail(void)
{
	InterfaceBatTimeDis(MI_BOND_FAIL);
}


void ShowSleep( uint32 min)
{
	uint8 buf[6]={0x00};

	buf[0] = min/60;
	buf[1] = min%60;
	LCD_MainDataInterface(MI_SLEEP_CHANGE, buf, 2);
}

void ShowSleeping(void)
{
	uint8_t buf = REMIND_SLEEP;
	InterfaceDataWithBatTimeDis(MI_REMIND_INDEX, &buf, 1);
}

void ShowWakeup(void)
{ 
	uint8_t buf = REMIND_WAKEUP;
	InterfaceDataWithBatTimeDis(MI_REMIND_INDEX, &buf, 1);
}


void ShowMedicine(void)
{
	uint8_t buf = REMIND_MEDICINE;
	InterfaceDataWithBatTimeDis(MI_REMIND_INDEX, &buf, 1);
}

void ShowEat(void)
{
	uint8_t buf = REMIND_EAT;
	InterfaceDataWithBatTimeDis(MI_REMIND_INDEX, &buf, 1);
}

void ShowSports(void)
{
	uint8_t buf = REMIND_MOVING;
	InterfaceDataWithBatTimeDis(MI_REMIND_INDEX, &buf, 1);
}


void ShowMeeting(void)
{
	uint8_t buf = REMIND_MEETING;
	InterfaceDataWithBatTimeDis(MI_REMIND_INDEX, &buf, 1);
}

void ShowUserRemind(void)
{
	uint8 buf[64]={0x00};
	uint8_t len = 0;
	UTCTimeStruct uTime;
	gOS_GetUTCTime(&uTime); 
	RefreshBatteryInfo();
	
	buf[0] = uTime.hour;
	buf[1] = uTime.minutes;
	buf[2] = TimeOrder.TimeFormat;	
	buf[3] = shortcut_info.battery_level;
	buf[4] = shortcut_info.battery_percent;
	GetRemindsNote(buf+5);
	len =strlen((char*)buf);
	LCD_MainDataInterface(MI_USER_REMAND,buf,len+1);	
}

void ShowMoving(void)
{
	InterfaceBatTimeDis(MI_MOVE_NOW);
}


void showUpgrade(void)
{
	ClearScreen();
	DisplayMap(upgrade32x32, 16, 8, 32, 32);
}

void showUpgradeSuccess(void){   
	
	ClearScreen();
	DisplayMap(upgradeSuccess32x32, 16, 8, 32, 32);
}

void showUpgradeFail(void){

	ClearScreen();
	DisplayMap(upgradeFail32x32, 16, 8, 32, 32);
}

void ShowSyncingIcon(void)
{      
	InterfaceBatTimeDis(MI_SYNCING);
}

void ShowBtDisconnected(void)
{
	InterfaceBatTimeDis(MI_BLE_DISCONNECTED);
}

void ShowMemFull(void)
{
	InterfaceBatTimeDis(MI_MEMORY_FULL);
}

void ShowGoalOver(void)
{
	uint8_t GoalOverMode = 0;
	InterfaceDataWithBatTimeDis(MI_GOAL_OVER, &GoalOverMode, 1);
}



void DispalyFactorySetLogo(void)
{
	ClearScreen();
	DisplayStringLetter_6x16("RESET",0,8,true); 
	DisplayStringLetter_6x16("DATA",0,24,true); 
}


void ShowLEDTime(void)
{

}



void ShowInactAlert(void)
{

	DisplayMap(MovingMapRes24x24, 18, 0, 24, 24);
	DisplayStringLetter_6x16("MOVE NOW!",0,32,true); 
}

void ShowHello(void)
{
	DisplayStringLetter_6x16("HELLO!",0,16,true); 
}


void ShowSecondsTimer(SecondTimerUI STu)
{
	ClearScreen();
	switch(STu)
	{
		case ST_PAGE:
			SecondsTimer(0);
		break;
		case ST_RUN:
			STflag=1;
			SecondsTimer(1);
		break;
		case ST_STOP:
			STflag=0;
			SecondsTimer(2);
		break;
		default:
		break;
	}
}

void ShowOnBoldIcon(void)
{
	ClearLcdDisplay(0, 0, 16, 16);
	DisplayMap(&BoldLetterMapRes6x16[2][0], 0, 0, 6, 16);
	DisplayMap(&BoldLetterMapRes6x16[1][0], 7, 0, 6, 16);
}

void ShowOffBlodIcon(void)
{
	DisplayMap(&BoldLetterMapRes6x16[2][0], 46, 0, 6, 16);
	DisplayMap(&BoldLetterMapRes6x16[0][0], 52, 0, 6, 16);	
	DisplayMap(&BoldLetterMapRes6x16[0][0], 58, 0, 6, 16);	
}

void ShowOnIcon(void)
{
	ClearLcdDisplay(0, 0, 16, 16);
	DisplayStringLetter_6x16("ON",0,0,false);
}

void ShowOffIcon(void)
{
	DisplayStringLetter_6x16("OFF",46,0, false);	
}



void ShowDevicesCom(void)
{	
	if (DevicesCom.on_off == DeviceOffStatus)
	{
		ShowOnIcon();
		ShowOffBlodIcon();
	}
	else {
		ShowOnBoldIcon();
		ShowOffIcon();
	}

	DisplayMap(DevComMapRes24x24, 20, 16, 24, 24);
}


void ShowDevicesSea(void)
{	
	if (display_index >= 3)
		display_index = 0;
	DisplayMap(&DevSeaMapRes24x24[display_index++][0],20,16,24,24);
	osal_start_timerEx(GetAppTaskId(), MSG_REPEAT_DEVICE_SEARCH_DISPLAY, 500);

}

void ShowDevicesNone(void)
{
	DisplayMap(DevNonMapRes24x24, 20, 16, 24, 24);
}

void ShowDevicesAdded(void)
{
	DisplayMap(DevAddMapRes24x24, 20, 8, 24, 24);
}

void ShowDevicesNonAdded(void)
{
	DisplayMap(DevNonAddMapRes24x24, 20, 8, 24, 24);
}

void ShowReceivedReq(void)
{
	DisplayMap(DevRejectedMapRes16x16, 2, 8, 16, 16);
	DisplayMap(DevNonAddMapRes24x24, 20, 0, 24, 24);
	DisplayMap(DevReceivedMapRes16x16, 46, 8, 16, 16);
}

void ShowSendRequest(void)
{
	DisplayMap(SenReqMapRes24x24, 20, 0, 24, 24);	

	if (display_index >= 3)
		display_index = 0;
	DisplayMap(&SenReqArrMapRes24x24[display_index++][0],20,24,24,24);
	osal_start_timerEx(GetAppTaskId(), MSG_REPEAT_DEVICE_REQUEST_DISPLAY, 500);

}

void ShowAddDevicesRecSuccess(void)
{
	DisplayMap(SenReqMapRes24x24, 20, 0, 24, 24);
	DisplayMap(EmoOK,20,24,24,24);
}

void ShowAddDevicesRecFail(void)
{
	DisplayMap(SenReqMapRes24x24, 20, 0, 24, 24);
	DisplayMap(DelDevFaiMapRes24x24, 16, 24, 24, 24);
}

	
void ShowAddDevicesSuccess(void)
{
	DisplayMap(AddDevSucMapRes24x24, 16, 8, 24, 24);	
}

void ShowAddDevicesFail(void)
{
	DisplayMap(AddDevFailMapRes24x24, 16, 8, 24, 24);	
}


void ShowEmotionStart(void)
{
	DisplayMap(&EmoStaMapRes24x24[0][0], 20, 8, 24, 24);
}

void ShowDevicesMotoRepeat(void)
{
	if (display_index >= 3)
		display_index = 0;
	DisplayMap(&EmoStaMapRes24x24[display_index++][0], 20, 8, 24, 24);
	osal_start_timerEx(GetAppTaskId(), MSG_REPEAT_DEVICE_MOTO_DISPLAY, 500);
}

void ShowEmotionOK(void)
{
	DisplayMap(EmoOK,20,24,24,24);
}
void ShowSendEmotionRepeat(void)
{
	if (display_index >= 3)
		display_index = 0;
	DisplayMap(&SenReqArrMapRes24x24[display_index++][0],20,24,24,24);
	osal_start_timerEx(GetAppTaskId(), MSG_REPEAT_DEVICE_EMOTION_DISPLAY, 500);

}

void ShowEmotionHomeMenu(void)
{
	DisplayMap(EmoHomMenMapRes24x24, 20, 0, 24, 24);
	DisplayMap(EmoDotMenMapRes26x8, 19, 40, 26, 8);
}

void ShowEmotionIdex(uint8 idx)
{
	switch(idx)
	{
		case 0:	
			ShowEmotionOne();	
			break;
		case 1:
			ShowEmotionTwo();
			break;
		case 2:
			ShowEmotionThree();
			break;
		case 3:
			ShowEmotionFour();
			break;
		case 4:
			ShowEmotionFive();
			break;
		case 5:
			ShowBack();
			break;			
	}
}
void ShowEmotionOne(void)
{
	DisplayMap(EmoHomMenMapRes24x24, 20, 0, 24, 24);
	DisplayMap(EmoDotOneMapRes26x8, 19, 40, 26, 8);
}
	
void ShowEmotionTwo(void)
{
	DisplayMap(EmoTwoMapRes24x24, 20, 0, 24, 24);
	DisplayMap(EmoDotTwoMapRes26x8, 19, 40, 26, 8);
}

void ShowEmotionThree(void)
{
	DisplayMap(EmoThreeMapRes24x24, 20, 0, 24, 24);
	DisplayMap(EmoDotThreeMapRes26x8, 19, 40, 26, 8);
}
	
void ShowEmotionFour(void)
{
	DisplayMap(EmoFourMapRes24x24, 20, 0, 24, 24);
	DisplayMap(EmoDotFourMapRes26x8, 19, 40, 26, 8);
}
	
void ShowEmotionFive(void)
{
	DisplayMap(EmoFiveMapRes24x24, 20, 0, 24, 24);
	DisplayMap(EmoDotFiveMapRes26x8, 19, 40, 26, 8);
}
	
		
void ShowDevDel(void)
{
	if (display_index >= 2)
		display_index =0;
	if(display_index==0)
		DisplayMap(DevAddMapRes24x24, 20, 8, 24, 24);
	else
		DisplayMap(DevNonAddMapRes24x24, 20, 8, 24, 24);
	display_index++;
	osal_start_timerEx(GetAppTaskId(), MSG_REPEAT_REC_DELETING_DEVICES, 500);
}
		
void ShowDevicesDel(void)
{
	DisplayMap(DevNonAddMapRes24x24, 0, 0, 24, 24);
	DisplayMap(&DevDelArrayMapRes16x16[1][0], 24, 0, 16, 16);
	DisplayMap(DevDelRubbishMapRes24x24, 40, 0, 24, 24);	
	DisplayStringLetter_6x16("NO",6,32,false);	
	DisplayStringLetter_6x16("YES",43,32,false);
}

void ShowDevicesDeleting(void)
{
	if (display_index >= 2)
		display_index = 0;
	DisplayMap(DevNonAddMapRes24x24, 0, 0, 24, 24);
	DisplayMap(&DevDelArrayMapRes16x16[display_index++][0], 24, 0, 16, 16);
	DisplayMap(DevDelRubbishMapRes24x24, 40, 0, 24, 24);	
	osal_start_timerEx(GetAppTaskId(), MSG_REPEAT_DELETING_DEVICES_DISPLAY, 500);
}
	
void ShowDevicesDelSuccess(void)
{
	DisplayMap(DevNonAddMapRes24x24, 0, 0, 24, 24);
	DisplayMap(&DevDelArrayMapRes16x16[1][0], 24, 0, 16, 16);	
	DisplayMap(DevDelRubbishMapRes24x24, 40, 0, 24, 24);
	DisplayMap(SenReqSucMapRes24x24, 16, 32, 24, 24);
}

void ShowDevicesDelFail(void)
{
	DisplayMap(DevNonAddMapRes24x24, 0, 0, 24, 24);
	DisplayMap(&DevDelArrayMapRes16x16[1][0], 24, 0, 16, 16);
	DisplayMap(DevDelRubbishMapRes24x24, 40, 0, 24, 24);
	DisplayMap(DelDevFaiMapRes24x24, 20, 24, 24, 24);
}

void ShowCharging(uint8 level, uint8 percent)
{
	uint8 buf[10]={0x00};
	UTCTimeStruct uTime;
	gOS_GetUTCTime(&uTime);
	buf[0] = uTime.hour;
	buf[1] = uTime.minutes;
	buf[2] = TimeOrder.TimeFormat; 
	buf[3] = level;
	buf[4] = percent;
	LCD_MainDataInterface(MI_CHARGING,buf, 5);
}

void ShowBatteryLow(void) 
{
	uint8 buf[6]={0x00};
	UTCTimeStruct uTime;
	gOS_GetUTCTime(&uTime);
	buf[0] = uTime.hour;
	buf[1] = uTime.minutes;
	buf[2] = TimeOrder.TimeFormat; 
	LCD_MainDataInterface(MI_LOW_BATTERY,buf, 3);
}

void ShowBatteryFull(void)
{
	uint8 buf[6]={0x00};
	UTCTimeStruct uTime;
	gOS_GetUTCTime(&uTime);
	buf[0] = uTime.hour;
	buf[1] = uTime.minutes;
	buf[2] = TimeOrder.TimeFormat; 
	LCD_MainDataInterface(MI_BATTERY_FULL,buf, 3);
}


void UpdateBleStatus(void)
{
	if (shortcut_info.airplane_mode == 1)
	{
		FLP_TurnOffBle();

	}
	else
	{
		FLP_TurnOnBle();
	}
}

void ShowShortCut(uint8 flags, bool flash_write)
{
	uint8 buf[10]={0x00};

	RefreshBatteryInfo();

	if (shortcut_info.airplane_mode == 1)
	{
		if (BC5_State != BC5_BT_POWEROFF)
		{
			osal_start_timerEx(GetAppTaskId(), MSG_SLEEP_CLOSE_BT, 300);
			osal_start_timerEx(GetAppTaskId(), MSG_BC5_DISCONNECT, 2500);
		}
		if (((BtState == BT_OPEN) || (BtState == BT_CONNECTED)|| (BtState == BT_DISCONNECT)) && (BleDisable == 1))
		{
			FLP_TurnOffBle();
			BleDisable = 0;
		}
	}
	else
	{
		if (BC5_State == BC5_BT_POWEROFF)
		{
			osal_start_timerEx(GetAppTaskId(), MSG_SLEEP_OPEN_BT, 300);
		}
		if (((BtState == BT_CLOSE) || (BtState == BT_DISCONNECT)) && (BleDisable == 1))
		{
			FLP_TurnOnBle();
			BleDisable = 0;
		}
	}
	if(BC5_State == BC5_BT_CONNECT_OK)
	{
		shortcut_info.BC5_Connect = 1;
	}
	else
		shortcut_info.BC5_Connect = 0;

	buf[0] = flags; //flags = 0, Only update data; flags = 1, Both update data and image.
	buf[1] = shortcut_info.find_me;
	buf[2] = shortcut_info.airplane_mode;
	buf[3] = shortcut_info.vibration_level;
	buf[4] = shortcut_info.battery_level;
	buf[5] = shortcut_info.battery_percent;
	buf[6] = shortcut_info.BC5_Connect;
	LCD_MainDataInterface(MI_SHORTCUT,buf, 7);

	if (flash_write)
		WRITE_USER_PARAMETER((uint8 *)&shortcut_info, SHORTCUT_LOC, SHORTCUT_LEN);
}

void ShowBrightness(uint8 flags, bool flash_write)
{
	uint8 buf[10]={0x00};

	buf[0] = flags; //flags = 0, Only update data; flags = 1, Both update data and image.
	buf[1] = shortcut_info.brightness_level;
	
	InterfaceDataWithBatTimeDis(MI_BRIGHTNESS,buf, 2);
	if (flash_write)
		WRITE_USER_PARAMETER((uint8 *)&shortcut_info, SHORTCUT_LOC, SHORTCUT_LEN);
}

void SetBrightness(uint8 Brightness)
{
	uint8 brightness_level = 0;
	brightness_level = shortcut_info.brightness_level;	
	if(Brightness==0)
	{
		brightness_level = 2;
#if 0
		if(brightness_level>3)
			brightness_level = 2;
		else
			brightness_level /= 2;
#endif
	}
	LCD_MainDataInterface_NoDisplay(MI_BRIGHTNESS_SET,&brightness_level,1);
}


void ShowTimerStatus(uint8 status, uint8 minutes, uint8 seconds, uint8 millSeconds)
{
	uint8 buf[10]={0x00};	
	buf[0] = status; 
	buf[1] = minutes;
	buf[2] = seconds;
	buf[3] = millSeconds;
	InterfaceDataWithBatTimeDis(MI_TIMER_CONTROL,buf, 4);
}	

void ShowWatchInfo(void)
{
	ReadWatchId(watch_info.WatchId);
	memcpy(watch_info.version, &SoftWareVersion[1], 3);
	LCD_MainDataInterface(MI_WATCH_INFO,watch_info.WatchId, sizeof(watch_info));
}

void ShowPaymentLock(void)
{
	uint8 buf[2]={0x00};
	
	buf[0] = PAY_LOCK; 
	LCD_MainDataInterface(MI_PAYMENT,buf, 1);
}

void ShowPaymentUnlock(void)
{
	uint8 buf[2]={0x00};
	
	buf[0] = PAY_UNLOCK; 
	LCD_MainDataInterface(MI_PAYMENT,buf, 1);
}


void ShowPaymentAnimation1(void)
{
	uint8 buf[2]={0x00};
	
	buf[0] = PAY_ANIMATION_1; 
	LCD_MainDataInterface(MI_PAYMENT,buf, 1);
}

void ShowPaymentAnimation2(void)
{
	uint8 buf[2]={0x00};
	
	buf[0] = PAY_ANIMATION_2; 
	LCD_MainDataInterface(MI_PAYMENT,buf, 1);
}

void ShowPaymentConsume(void)
{
	uint8 buf[6]={0x00};
	
	buf[0] = PAY_CONSUME; 
	buf[1] = (log_data.Amount >> 0)& 0xFF;
	buf[2] = (log_data.Amount >> 8) & 0xFF;
	buf[3] = (log_data.Amount >> 16) & 0xFF;
	buf[4] = (log_data.Amount >> 24) & 0xFF;
	LCD_MainDataInterface(MI_PAYMENT,buf, 5);
}


void ShowPaymentBalance(void)
{
	uint8 buf[6]={0x00};

	log_data.Balance= log_data.Balance - log_data.Amount;
	
	buf[0] = PAY_BALANCE; 
	buf[1] = (log_data.Balance >> 0) & 0xFF;
	buf[2] = (log_data.Balance >> 8) & 0xFF;
	buf[3] = (log_data.Balance >> 16) & 0xFF;
	buf[4] = (log_data.Balance >> 24) & 0xFF;
	LCD_MainDataInterface(MI_PAYMENT,buf, 5);	
}

void ShowPaymentLog(void)
{
	uint8 buf[15]={0x00};
	
	buf[0] = PAY_LOG;
	buf[1] = log_data.Date[0]; //date
	buf[2] = log_data.Date[1]; 
	buf[3] = log_data.Date[2]; 
	buf[4] = log_data.Date[3]; 
	buf[5] = log_data.Date[4]; 
	buf[6] = log_data.Date[5]; 
	buf[7] = log_data.Time[0];	//time
	buf[8] = log_data.Time[1];
	buf[9] = log_data.Time[2];
	buf[10] = log_data.Time[3];
	buf[11] = log_data.Time[4];
	buf[12] = log_data.Time[5];

	LCD_MainDataInterface(MI_PAYMENT,buf, 13);
}


void ShowStatus(uint8 scene_mode, uint8 sleep_mode, uint8 memory_full, uint8 bc5_connect,uint8 ble_connect)
{
}

void ShowBtConnecting(void)
{
}


void DisplayEnString(const uint8 *pCn,uint8 x,uint8 y)
{
}




void ShowHeartRate(UiHRStruct gDispalyHrData)		/*To KL17*/
{
	uint8 buf[12]={0x00};

		buf[0] =	gDispalyHrData.HR_mode;
		buf[1] =	gDispalyHrData.HR_mode;
		buf[2] =	gDispalyHrData.HR_Run_State;
		buf[3] =	gDispalyHrData.HR_Value_Val;   
		buf[4] =	gDispalyHrData.HR_Value;         
		buf[5] =	gDispalyHrData.HR_Running;
		buf[6] =	gDispalyHrData.DHM_Flag;
		buf[7] =	gDispalyHrData.DHM_Time;
		buf[8] =	gDispalyHrData.HR_Level;

	UTCTimeStruct uTime;
	gOS_GetUTCTime(&uTime);
	buf[9] = CalclateWeeks(uTime.year, uTime.month, uTime.day);
	buf[10] = uTime.month;
	buf[11] = uTime.day;
	
	
	//LCD_MainDataInterface(MI_HR,buf,12);
	InterfaceDataWithBatTimeDis(MI_HR, buf, 12);

}

void ShowHRAlarm(UiHRStruct gDispalyHrData,uint8 HR_Alarm_Value)		/*To KL17*/	
{
	uint8 buf[2]={0x00};

	buf[0] =	gDispalyHrData.HR_Value;         
	buf[1] =	HR_Alarm_Value;
	InterfaceDataWithBatTimeDis(MI_HR_ALARM, buf, 2);
}

void ShowHRAlarm_WithoutClean(UiHRStruct gDispalyHrData,uint8 HR_Alarm_Value)		/*To KL17*/	
{
	uint8 buf[2]={0x00};

	buf[0] =	gDispalyHrData.HR_Value;         
	buf[1] =	HR_Alarm_Value;
	InterfaceDataWithBatTimeDis(MI_HR_ALARM_WITHOUT_CLS, buf, 2);
}



uint8 CalcAccumulateTime(uint8* A_TotalMin, UTCTimeStruct uUtcTime_C,UTCTimeStruct uUtcTime_L)
{
	uint8 year=uUtcTime_C.year-uUtcTime_L.year;
	uint8 month=uUtcTime_C.month-uUtcTime_L.month;
	uint32 C_TotalMin=0;
	uint32 L_TotalMin=0;
	uint32 TotalMin=0;

	if((year==0)&&(month==0))
	{
		C_TotalMin = osal_ConvertUTCSecs(&uUtcTime_C)/60;
		L_TotalMin = osal_ConvertUTCSecs(&uUtcTime_L)/60;
		TotalMin = (C_TotalMin>L_TotalMin) ? (C_TotalMin-L_TotalMin) : 0;
		if(TotalMin>=1440)
		{
			*A_TotalMin = (TotalMin/1440 >= 255) ? 255 : (TotalMin/1440);//days
			return 3;
		}
		else if(TotalMin>=60)
		{
			*A_TotalMin = TotalMin/60;//hours
			return 2;
		}
		else if(TotalMin>0)
		{
			*A_TotalMin=TotalMin;//min
			return 1;
		}
		else//(TotalMin==0)
		{
			*A_TotalMin=0;
			return 0;
		}
	}
	else
	{
		*A_TotalMin=0;
		return 0;
	}
}

void CalculatePercent(uint32_t Values, uint8 Goal_Index, uint8 *Ptr_Percent)
{
	uint32_t Values_Goal = 0;
	Values_Goal = DailyGoal[Goal_Index].Goal;	
	if(Goal_Index == 0)// steps
		Values_Goal  = Values_Goal*100;
	if(Goal_Index == 1 ||Goal_Index == 2) // Distacne and calor
		Values_Goal  = Values_Goal*1000;
	if(Goal_Index == 3)
		Values_Goal  = Values_Goal*60;
	*Ptr_Percent = (Values*10)/Values_Goal;
	if(*Ptr_Percent>10)
		*Ptr_Percent = 10;

}

void RefreshBatteryInfo(void)
{
	BAT_VAULE_E CapacityTemp = BatteryTempPercent;
	shortcut_info.battery_percent = BatteryTempPercent;
	
	if(CapacityTemp >= 100)
	{
		shortcut_info.battery_level = 5;

	}
	else if(CapacityTemp >= 76)//86)
	{
		shortcut_info.battery_level = 4;
	}
	else if(CapacityTemp >= 52)//66)
	{
		shortcut_info.battery_level = 3;

	}
	else if (CapacityTemp >= 26)//41)
	{
		shortcut_info.battery_level = 2;

	}
	else if(CapacityTemp >= 11)//23)
	{
		shortcut_info.battery_level = 1;

	}
	else
	{
		shortcut_info.battery_level = 0;
	}
}

void ShowMainMenu(uint8 index)					/*To KL17*/				
{	
	InterfaceBatTimeDis((MainDataI)(MI_MAIN_MENU_ONE+(index-1)));
}

void ShowAppsMenu(uint8 index)					/*To KL17*/	
{
	LCD_PictureInterface((PictureUI)(P_APPS_MENU_ONE+(index-1)));
}

void ShowNotificationMenu(uint8 index)					/*To KL17*/	
{
	uint8 buf[3] = {0x00};
	ReflashAncsCount();
	if(index==1)
	{
		buf[0] = gMissedCallCount;
		buf[1] = gShortMsgCount;
		buf[2] = gEMailCount;
	}
	else if(index==2)
	{
		buf[0] = Total_Count_SOCIAL();
		buf[1] = gScheduleCount;
	}
	LCD_MainDataInterface((MainDataI)(MI_NOTIFICATION_MENU_ONE+(index-1)), buf, 3);
}

void ShowRestart(void)
{
	InterfaceBatTimeDis(MI_RESTART);
}

void ShowRemindList(REMIND_DATA_T* RDTableTemp)
{
	uint8 buf[20] = {0x00};
	uint8 len = 0;

	buf[1] = TimeOrder.TimeFormat;
	
	buf[2] = RDTableTemp->Id;
	buf[3] = RDTableTemp->Hour;
	buf[4] = RDTableTemp->Minute;
	buf[5] = RDTableTemp->RemindTimes;
	
	buf[6] = (RDTableTemp+1)->Id;
	buf[7] = (RDTableTemp+1)->Hour;
	buf[8] = (RDTableTemp+1)->Minute;
	buf[9] = (RDTableTemp+1)->RemindTimes;

	buf[10] = (RDTableTemp+2)->Id;
	buf[11] = (RDTableTemp+2)->Hour;
	buf[12] = (RDTableTemp+2)->Minute;
	buf[13] = (RDTableTemp+2)->RemindTimes;

	if(RDTableTemp->RdSwitch==0)
	{
		len = 1;
		buf[0] = 0;
	}
	else if((RDTableTemp+1)->RdSwitch==0)
	{
		len = 6;
		buf[0] = 1;
	}
	else if((RDTableTemp+2)->RdSwitch==0)
	{
		len = 10;
		buf[0] = 2;
	}
	else
	{
		len = 14;
		buf[0] = 3;
	}
		
	LCD_MainDataInterface(MI_REMIND_LIST, buf, len);	
}

void ShowVirateLevel(bool flash_write)						/*To KL17*/
{
	uint8 buf = 0;

	buf = shortcut_info.vibration_level;
	LCD_MainDataInterface(MI_VIRATE_LEVEL, &buf, 1);
	if (flash_write)
		WRITE_USER_PARAMETER((uint8 *)&shortcut_info, SHORTCUT_LOC, SHORTCUT_LEN);
}

void ShowCameraCountDown(uint8 Ca)							/*To KL17*/	
{	
	InterfaceDataWithBatTimeDis(MI_CAMERA_COUNTDOWN,&Ca,1);
}

void ShowBatteryInfo(void)
{
	InterfaceBatTimeDis(MI_BATTERY_INFO);
}


void ShowRecordgo(void)
{
	InterfaceBatTimeDis(MI_RECORD_GO);
}

void ShowRecordWorkoutsRUN(uint8 status,uint16 TimerMinutes, uint8 TimerSeconds, uint8 TimerMSec)
{

	uint8 buf[5]={0x00};
	buf[0] = status;	
	uint16_encode(TimerMinutes,&(buf[1]));
	buf[3] = TimerSeconds;
	buf[4] = TimerMSec;
	InterfaceDataWithBatTimeDis(MI_RECORD_RUN, buf, 5);
}

void ShowRecordResult(uint8 ui_index, uint8 DistanceFormat,uint8 Hr_value,
							uint32 uSteps,uint32 uDistance,
							uint32 uCalor,uint32 TimerMSec)
{
	uint8 buf[19]={0x00};
	buf[0] = ui_index;
	buf[1] = DistanceFormat;
	buf[2] = Hr_value;
	
	uint32_encode(uSteps,&(buf[3]));	
	uint32_encode(uDistance,&(buf[7])); 
	uint32_encode(uCalor,&(buf[11]));	
	uint32_encode(TimerMSec,&(buf[15]));
	
	InterfaceDataWithBatTimeDis(MI_RECORD_RESULT, buf, 19);
}

void ShowEarnedMeadalRemind(uint8 level)
{

	LCD_MainDataInterface(MI_EARNED_MEDAL_REMIND, &level, 1);

}
void ShowNoNewNotifications(void)
{
	LCD_PictureInterface(P_NO_NEW_NTIFICATIONS);
}


void ShowSocialClass(uint8 page)
{
	uint8 buf[4]={0x00};
	buf[0] = page;
	if(page<3)
	{
		buf[1] = SocialClassCount[page*3];
		buf[2] = SocialClassCount[page*3+1];	
		buf[3] = SocialClassCount[page*3+2];	
	}
	LCD_MainDataInterface(MI_SOCIAL_CLASS, buf, 4);
}

void ShowNoNewReminders(void)
{
	LCD_PictureInterface(P_NO_REMINDERS);
}

void ShowExitSleepMode(void)
{
	LCD_PictureInterface(P_EXITSLEEPMODE);
}

void ShowDeleteAncs(void)
{
	LCD_PictureInterface(P_DELETE_ANCS);
}

void ShowExitRecordWorkouts(void)
{
	LCD_PictureInterface(P_EXITWORKOUTS);
}

void ShowExitSleep(void)
{
	LCD_PictureInterface(P_EXITSLEEP);
}


