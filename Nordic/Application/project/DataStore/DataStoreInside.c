#include "headfile.h"

/******************************************************************************************
*函数名:		StoreDataInit
*参数:			
*返回值:		
*描述:			初始化运动记录缓存数据
*编写:			statdream 
*版本信息:	    v1.0
******************************************************************************************/
static AncsFlash gAncsFlash[7]; 		/*4个类型8个扇区: 1个类型2个扇区(8k): 256*32=8k 最多16条*/
static AncsFlash gSocialFlash[9]; 		/*4个类型8个扇区: 1个类型2个扇区(8k): 256*32=8k 最多16条*/
uint32 Ota_Base=0x392000UL;
uint32 Ota_Hr_Base=0x392000UL;				
uint32 Ota_Tp_Base=0x39B000UL;				
uint32 Ota_Kl17_Base=0x3A7000UL;				
uint32 Ota_Nor_Base=0x3C8000UL;					
void StoreDataInit(void)
{
	Force_NordicFlash();
	/************************************Sector 1 存储 (重要标志)************************************/	
	uint8 UserFlag_Buf[USER_FLAG_MAX_LEN]={0x00};
	READ_USER_FLAG(UserFlag_Buf,0,USER_FLAG_MAX_LEN);
	
	//初始化标志
	if(UserFlag_Buf[INIT_MARK_LOC]==0xFF)
		gInitMark=0;
	else
		gInitMark = UserFlag_Buf[INIT_MARK_LOC];
	
	//BC5重连标志
	if(UserFlag_Buf[BC5_RECONNECTED_LOC]==0xFF)
		BC5_POWERON=false;
	else
		BC5_POWERON = UserFlag_Buf[BC5_RECONNECTED_LOC];

	/************************************Sector 2 存储 (用户设置)************************************/
	uint8  UserData_Buf[USER_PARAMETER_MAX_LEN]={0x00};
	READ_USER_PARAMETER(UserData_Buf,0,USER_PARAMETER_MAX_LEN);	

	//个人信息设置
	if(UserData_Buf[USER_INFO_LOC]==0xFF)
	{
		UserInfoData.UserSex = 0; 
		UserInfoData.UserAge = 20;	
		UserInfoData.UserHeight = 160;
		UserInfoData.UserWeight = 6000;	
	}
	else
	{
		UserInfoData.UserSex = UserData_Buf[USER_INFO_LOC];
		UserInfoData.UserAge = UserData_Buf[USER_INFO_LOC+1];
		UserInfoData.UserHeight = UserData_Buf[USER_INFO_LOC+2];
		UserInfoData.UserWeight = (((uint16)(UserData_Buf[USER_INFO_LOC+4]))<<8)|UserData_Buf[USER_INFO_LOC+3];		
	}
	
	//一级界面设置
	if(UserData_Buf[MENU_ORDER_LOC]==0xFF){
		MenuOrder[0]=1;
		MenuOrder[1]=2;
		MenuOrder[2]=3;
		MenuOrder[3]=4;
		MenuOrder[4]=5;
		MenuOrder[5]=6;
		MenuOrder[6]=7;	
		MenuOrder[7]=0;
		MenuOrder[8]=0;			
	}
	else
		memcpy(MenuOrder,UserData_Buf+MENU_ORDER_LOC,MENU_ORDER_LEN);

	//振子设置
	if(UserData_Buf[MOTO_ORDER_LOC]==0xFF){
		MotoOrder[0]=0x04;
		MotoOrder[1]=0x04;
		MotoOrder[2]=0x07;
		MotoOrder[3]=0x04;
		MotoOrder[4]=0x04;
		MotoOrder[5]=0x04;
		MotoOrder[6]=0x04;
		MotoOrder[7]=0x04;
		MotoOrder[8]=0x04;
		MotoOrder[9]=0x04;		
	}
	else
		memcpy(MotoOrder,UserData_Buf+MOTO_ORDER_LOC,MOTO_ORDER_LEN);
	

	//时间设置
	if(UserData_Buf[TIME_ORDER_LOC]==0xFF){
		TimeOrder.DateFormat=0x05;
		TimeOrder.TimeFormat=0x02;
		TimeOrder.BatFormat=0x01;
		TimeOrder.LunarFormat=0x00;
		TimeOrder.RotateFormat=0x00;
		TimeOrder.BackgroundStyle=0x00;
		TimeOrder.HeartRateFormat=0x01;
		TimeOrder.UserNameFormat=0x00;
	}		
	else
		memcpy((uint8 *)&TimeOrder,UserData_Buf+TIME_ORDER_LOC,TIME_ORDER_LEN);	
	
	//时间界面参数
	TimerMenuFlag = TimeOrder.BackgroundStyle+1;
#if 0
	if(UserData_Buf[TIMER_MENU_LOC]==0xFF){
		TimerMenuFlag=0x01;
	}
	else
		TimerMenuFlag=UserData_Buf[TIMER_MENU_LOC];
#endif
	
	//设置值
	if(UserData_Buf[SET_VALUE_LOC]==0xFF)
	{
		SetValue.Backlight=0x00;		
	}		
	else
	{
		memcpy((uint8 *)&SetValue.Backlight,UserData_Buf+SET_VALUE_LOC,1);
	}
	
	if(UserData_Buf[SET_VALUE_LOC + 1]==0xFF)
	{
		SetValue.Volume=0x05;	
	}		
	else
	{
		memcpy((uint8 *)&SetValue.Volume,UserData_Buf+SET_VALUE_LOC + 1,1);
	}
	
	if(UserData_Buf[SET_VALUE_LOC + 2]==0xFF)
	{
		SetValue.Language=0x00;	
	}		
	else
	{
		memcpy((uint8 *)&SetValue.Language,UserData_Buf + SET_VALUE_LOC + 2,1);
	}
	
	if(UserData_Buf[SET_VALUE_LOC + 3]==0xFF)
	{
		SetValue.DistanceFormat=0x00;		
	}		
	else
	{
		memcpy((uint8 *)&SetValue.DistanceFormat,UserData_Buf + SET_VALUE_LOC + 3,1);
	}
	
	if(UserData_Buf[SET_VALUE_LOC + 4]==0xFF)
	{
		SetValue.UserHabit=0x00;		
	}		
	else
	{
		memcpy((uint8 *)&SetValue.UserHabit,UserData_Buf + SET_VALUE_LOC + 4,1);
	}
	

	//目标值
	if(UserData_Buf[DAILY_GOAL_LOC]==0xFF&&UserData_Buf[DAILY_GOAL_LOC+3]==0xFF&&UserData_Buf[DAILY_GOAL_LOC+6]==0xFF&&UserData_Buf[DAILY_GOAL_LOC+12]==0xFF)
	{
		DailyGoal[0].Goal = 70;
		DailyGoal[0].MainGoalFlag = 0;
		DailyGoal[1].Goal = 350;
		DailyGoal[1].MainGoalFlag = 0;
		DailyGoal[2].Goal = 5;
		DailyGoal[2].MainGoalFlag = 0;
		DailyGoal[3].Goal = 8;
		DailyGoal[3].MainGoalFlag = 0;	
		DailyGoal[4].Goal = 30;
		DailyGoal[4].MainGoalFlag = 0;			
	}
	else{
		DailyGoal[0].Goal = UserData_Buf[DAILY_GOAL_LOC]|UserData_Buf[DAILY_GOAL_LOC+1]<<8;
		DailyGoal[0].MainGoalFlag = UserData_Buf[DAILY_GOAL_LOC+2];
		DailyGoal[1].Goal = UserData_Buf[DAILY_GOAL_LOC+3]|UserData_Buf[DAILY_GOAL_LOC+4]<<8;
		DailyGoal[1].MainGoalFlag = UserData_Buf[DAILY_GOAL_LOC+5];
		DailyGoal[2].Goal = UserData_Buf[DAILY_GOAL_LOC+6]|UserData_Buf[DAILY_GOAL_LOC+7]<<8;
		DailyGoal[2].MainGoalFlag = UserData_Buf[DAILY_GOAL_LOC+8];
		DailyGoal[3].Goal = UserData_Buf[DAILY_GOAL_LOC+9]|UserData_Buf[DAILY_GOAL_LOC+10]<<8;
		DailyGoal[3].MainGoalFlag = UserData_Buf[DAILY_GOAL_LOC+11];	
		DailyGoal[4].Goal = UserData_Buf[DAILY_GOAL_LOC+12]|UserData_Buf[DAILY_GOAL_LOC+13]<<8;
		DailyGoal[4].MainGoalFlag = UserData_Buf[DAILY_GOAL_LOC+14];	
	}
	//自动睡眠
	if(UserData_Buf[AUTO_SLEEP_LOC]==0xFF){
		AutoSleepTime.BeginHour= 23;
		AutoSleepTime.BeginMinute= 0;
		AutoSleepTime.EndHour= 7;
		AutoSleepTime.EndMinute= 0;
		AutoSleepTime.SleepCycle=0;	
	}		
	else
		memcpy((uint8 *)&AutoSleepTime,UserData_Buf+AUTO_SLEEP_LOC,AUTO_SLEEP_LEN);

	//开关
	gSwitchSetting.u32PMD =  UserData_Buf[SWITCH_LOC]|UserData_Buf[SWITCH_LOC+1]<<8|UserData_Buf[SWITCH_LOC+2]<<16|UserData_Buf[SWITCH_LOC+3]<<24;
	SWITCH_ENTER_SLEEP = false;

	//快捷设置参数
	if (UserData_Buf[SHORTCUT_LOC]==0xFF)
	{
		shortcut_info.find_me = 1;
	} 
	else 
		memcpy((uint8 *)&shortcut_info.find_me,UserData_Buf+SHORTCUT_LOC,1);

	if (UserData_Buf[SHORTCUT_LOC + 1]==0xFF)
	{
		shortcut_info.airplane_mode = 0;	
	} 
	else 
		memcpy((uint8 *)&shortcut_info.airplane_mode,UserData_Buf+SHORTCUT_LOC + 1,1);

	if (UserData_Buf[SHORTCUT_LOC + 2]==0xFF)
	{
		shortcut_info.vibration_level= MOTOR_LEVEL_NORMAL;
	} 
	else 
		memcpy((uint8 *)&shortcut_info.vibration_level,UserData_Buf+SHORTCUT_LOC + 2,1);

	if (UserData_Buf[SHORTCUT_LOC + 3]==0xFF)
	{
		shortcut_info.battery_level = 5;		
	} 
	else 
		memcpy((uint8 *)&shortcut_info.battery_level,UserData_Buf+SHORTCUT_LOC + 3,1);

	if (UserData_Buf[SHORTCUT_LOC + 4]==0xFF)
	{
		shortcut_info.battery_percent = 100;	
	} 
	else 
		memcpy((uint8 *)&shortcut_info.battery_percent,UserData_Buf+SHORTCUT_LOC + 4,1);

	if (UserData_Buf[SHORTCUT_LOC + 5]==0xFF)
	{
		shortcut_info.brightness_level = 8;		
	} 
	else 
		memcpy((uint8 *)&shortcut_info.brightness_level,UserData_Buf+SHORTCUT_LOC + 5,1);
	
	ShowShortCut(0, 0);	//no display, no flash write
	ShowBrightness(0, 0);//no display, no flash write

	//睡眠显示参数
	if (UserData_Buf[SLEEP_LOC]==0xFF)
	{
		sleep_info.total_sleep = 0x00;
		sleep_info.fell_sleep = 0x00;
		sleep_info.light_sleep = 0x00;
		sleep_info.deep_sleep = 0x00;
		sleep_info.awake_sleep = 0x00;
		sleep_info.awake = 0x00;
	} 
	else
		memcpy((uint8 *)&sleep_info,UserData_Buf+SLEEP_LOC,SLEEP_LEN);

	//设置参数
	if (UserData_Buf[SETTING_LOC]==0xFF)
	{
		setting_info.sounds = 0x00;
		setting_info.wrist = 0x01;
	}
	else
		memcpy((uint8 *)&setting_info.sounds,UserData_Buf+SETTING_LOC,1);	

	if(UserData_Buf[SETTING_LOC+1]==0xFF)
	{
		setting_info.wrist = 0x01;
	} 
	else 
		memcpy((uint8 *)&setting_info.wrist,UserData_Buf+SETTING_LOC+1,1);	

	//用户间通信ID
	memset(mName,0,13);
	if (UserData_Buf[DEVICE_COMMID_LOC]!=0xFF&&UserData_Buf[DEVICE_COMMID_LOC]!=0x00)
	{
		memcpy(mName,(UserData_Buf+DEVICE_COMMID_LOC),DEVICE_COMMID_LEN);
	}
		
	//升级前运动数据保存
	if(UserData_Buf[UPDATE_STORE_LOC]==0x01)
	{
		uint8 OtaFlagTemp =0x00;
		gSportDisplay.SportSteps = UserData_Buf[UPDATE_STORE_LOC+1]|UserData_Buf[UPDATE_STORE_LOC+2]<<8|UserData_Buf[UPDATE_STORE_LOC+3]<<16|UserData_Buf[UPDATE_STORE_LOC+4]<<24;
		gSportDisplay.SportKcal = UserData_Buf[UPDATE_STORE_LOC+5]|UserData_Buf[UPDATE_STORE_LOC+6]<<8|UserData_Buf[UPDATE_STORE_LOC+7]<<16|UserData_Buf[UPDATE_STORE_LOC+8]<<24;
		gSportDisplay.SportKcal = UserData_Buf[UPDATE_STORE_LOC+9]|UserData_Buf[UPDATE_STORE_LOC+10]<<8|UserData_Buf[UPDATE_STORE_LOC+11]<<16|UserData_Buf[UPDATE_STORE_LOC+12]<<24;
		gSportDisplay.SportDuration = UserData_Buf[UPDATE_STORE_LOC+13]|UserData_Buf[UPDATE_STORE_LOC+14]<<8|UserData_Buf[UPDATE_STORE_LOC+15]<<16|UserData_Buf[UPDATE_STORE_LOC+16]<<24;
		WRITE_USER_PARAMETER(&OtaFlagTemp,UPDATE_STORE_LOC,1);
	}
	
	/*静坐参数*/
	if(UserData_Buf[SIT_REMIAND_LOC]==0xFF)
	{
		gSitRemind.SrSwitch =0;
		gSitRemind.SrDayFrequency = 0;
		gSitRemind.SrFrequency = 15;
		gSitRemind.SrBeginHour = 0;
		gSitRemind.SrBeginMinute = 0;
		gSitRemind.SrEndHour = 0;
		gSitRemind.SrEndMinute = 0;
		gSitRemind.SrStepThresholdL = 0;
		gSitRemind.SrStepThresholdH = 0;		
	}
	else
		memcpy((uint8 *)&gSitRemind,UserData_Buf+SIT_REMIAND_LOC,SIT_REMIAND_LEN);	
	ClearSitRemindEvent();

	/*基础卡路里*/
	if(UserData_Buf[BASE_KCAL_TYPE_LOC]==0xFF)
	{
		Base_Kcal_type = 0;
	}
	else
		memcpy((uint8 *)&Base_Kcal_type,UserData_Buf+BASE_KCAL_TYPE_LOC,BASE_KCAL_TYPE_LEN);		
	
	if(UserData_Buf[LAST_LANGUAGE_LOC]==0xFF)
	{
		LastLanguage = 0;
	}
	else
		memcpy((uint8 *)&LastLanguage,UserData_Buf+LAST_LANGUAGE_LOC,LAST_LANGUAGE_LEN);	
	
	if(UserData_Buf[PHONE_BOOK_LOC]==0xFF)
	{
		PhoneBoookSave = 0;
	}
	else
		memcpy((uint8 *)&PhoneBoookSave,UserData_Buf+PHONE_BOOK_LOC,PHONE_BOOK_LEN);

	/*通话记录标志位*/
#if 0	
	if(UserData_Buf[PHONE_RECORD_LOC]==0xFF)
	{
		gAncsFlash[5].wPos=0;
		gAncsFlash[5].wMap=0;
		gAncsFlash[5].rMap=0;
	}
	else
	{
		gAncsFlash[5].wPos=UserData_Buf[PHONE_RECORD_LOC];
		gAncsFlash[5].wMap=uint32_decode(UserData_Buf+PHONE_RECORD_LOC+1);
		gAncsFlash[5].rMap=uint32_decode(UserData_Buf+PHONE_RECORD_LOC+5);
	}
#endif	
	if((UserData_Buf[USER_UID_LOC] == 0xFF)&&(UserData_Buf[USER_UID_LOC+1]==0xFF)&&(UserData_Buf[USER_UID_LOC+2]==0xFF)&&(UserData_Buf[USER_UID_LOC+3]==0xFF))
	{
		UserData_Buf[USER_UID_LOC]=0x00;
		UserData_Buf[USER_UID_LOC+1]=0x00;
		UserData_Buf[USER_UID_LOC+2]=0x00;
		UserData_Buf[USER_UID_LOC+3]=0x00;
	}
	memcpy((uint8 *)&UserID,UserData_Buf+USER_UID_LOC,USER_UID_LEN);
	/************************************Sector 3 存储 (运动/休眠总条数)************************************/
	uint32 SportDataSum=0,SleepDataSum=0;

       /*存储的运动总条数*/
	READ_STEP_SLEEP_SUM(&SportDataSum,SPORT_TOTAL_LOC,SPORT_TOTAL_LEN);
	if(SportDataSum==0xFFFFFFFF)
	{
		Step_Store_Logs=0;
		WRITE_STEP_SLEEP_SUM(&Step_Store_Logs,SPORT_TOTAL_LOC,SPORT_TOTAL_LEN);
	}
	else
		Step_Store_Logs = SportDataSum;
	
        /*存储的睡眠总条数*/
	READ_STEP_SLEEP_SUM(&SleepDataSum,SLEEP_TOTAL_LOC,SLEEP_TOTAL_LEN);
	if(SleepDataSum==0xFFFFFFFF)
	{
		Sleep_Store_Logs=0;
		WRITE_STEP_SLEEP_SUM(&Sleep_Store_Logs,SLEEP_TOTAL_LOC,SLEEP_TOTAL_LEN);
	}
	else
		Sleep_Store_Logs =SleepDataSum;

	uint32 TempData=0;
	/*存储的心率总条数*/
	READ_STEP_SLEEP_SUM(&TempData, HEARTRATE_RECORDS_TOTAL_LOC,HEARTRATE_RECORDS_TOTAL_LEN);	 
	if(TempData==0xFFFFFFFF)
	{
		TempData=0;
		//WriteStepSleepSum(&TempData, HEARTRATE_RECORDS_TOTAL_LOC);
		WRITE_STEP_SLEEP_SUM(&TempData,HEARTRATE_RECORDS_TOTAL_LOC,HEARTRATE_RECORDS_TOTAL_LEN);
	}
	HeartRate_Store_Logs = TempData;
	 
	/*计时运动记录总条数*/
	READ_STEP_SLEEP_SUM(&TempData, RECORD_WORK_LOC,RECORD_WORK_LEN);	 
	if(TempData==0xFFFFFFFF)
	{
		TempData=0;
		WRITE_STEP_SLEEP_SUM(&TempData,RECORD_WORK_LOC,RECORD_WORK_LEN);
	}
	RecordWork_Store_Logs = TempData;
	
	/*存储的心率门限*/
	READ_STEP_SLEEP_SUM(&TempData,HEARTRATE_ALARM_LOC,HEARTRATE_ALARM_LEN);	 
	if(TempData==0xFFFFFFFF)
	{
		hr_alarm.Alarm_en = 0;
		hr_alarm.HeartRateAlaLower = 50;
		hr_alarm.HeartRateAlaUpper = 180;
		//WriteStepSleepSum((uint32*)&hr_alarm, HEARTRATE_ALARM_LOC);
		WRITE_STEP_SLEEP_SUM(&TempData,HEARTRATE_ALARM_LOC,HEARTRATE_ALARM_LEN);
	}
	else
	{
		memcpy((void*)&hr_alarm, &TempData, sizeof(hr_alarm));
	}
	
	/************************************Sector 5存储 (睡眠详细数据图表)**********************************/
	//ReadDeviceComm_Friends();
	InitSleepDetailForDisplay();
	/************************************Sector 9存储 (运动显示保存)***************************************/	
	GetDisplySportData();
		
	GetOTALanguageInfo();
	GetAndroid_iosFalg();
	Free_NordicFlash();
	Set_OTALanguageInfo();
}




/************************************Sector 0 存储 (Watch ID)************************************/
void ReadWatchId(uint8 *dat)
{
	SpiFlash_Read(WATCH_ID_BASE_ADDR,dat,WATCH_ID_LEN);
}
void WriteWatchId(uint8 *dat)
{
	SpiFlash_4KSector_Erase(WATCH_ID_BASE_ADDR);
	SpiFlash_Write(WATCH_ID_BASE_ADDR,dat,WATCH_ID_LEN);
}

/************************************Sector 1 存储 (重要标志)************************************/
void READ_USER_FLAG(uint8 *data,uint8 Cnt,uint8 CLen)
{
	if(data != NULL)
		SpiFlash_Read(USER_FLAG_BASE_ADDR+Cnt, (uint8*)data, CLen);
}
void WRITE_USER_FLAG(uint8 *data,uint8 Cnt, uint8 CLen)
{
	if(data != NULL)
	{
		uint8 BufferForFlash[USER_FLAG_MAX_LEN]={0x00};
		SpiFlash_Read(USER_FLAG_BASE_ADDR, BufferForFlash, USER_FLAG_MAX_LEN);
		osal_memcpy(&BufferForFlash[Cnt],data,CLen);
		SpiFlash_4KSector_Erase(USER_FLAG_BASE_ADDR);
		SpiFlash_Write(USER_FLAG_BASE_ADDR,BufferForFlash,USER_FLAG_MAX_LEN);		
	}
}

/************************************Sector 2 存储 (用户设置)************************************/
void READ_USER_PARAMETER(uint8 *data,uint8 Cnt,uint8 CLen)
{
	if(data != NULL)
		SpiFlash_Read(USER_PARAMETER_BASE_ADDR+Cnt, (uint8*)data, CLen);
}
void WRITE_USER_PARAMETER(uint8 *data,uint8 Cnt, uint8 CLen)
{
	if(data != NULL)
	{
		uint8 BufferForFlash[USER_PARAMETER_MAX_LEN]={0x00};
		SpiFlash_Read(USER_PARAMETER_BASE_ADDR, BufferForFlash, USER_PARAMETER_MAX_LEN);
		osal_memcpy(&BufferForFlash[Cnt],data,CLen);
		SpiFlash_4KSector_Erase(USER_PARAMETER_BASE_ADDR);
		SpiFlash_Write(USER_PARAMETER_BASE_ADDR,BufferForFlash,USER_PARAMETER_MAX_LEN);		
	}
}

uint8 SetUserInfo(void)
{
	uint8 uInfo[USER_INFO_LEN];
	uInfo[0] = UserInfoData.UserSex;
	uInfo[1] = UserInfoData.UserAge;
	uInfo[2] = UserInfoData.UserHeight;
	uInfo[3] = UserInfoData.UserWeight & 0xff;
	uInfo[4] = (UserInfoData.UserWeight>>8)&0xff;
	WRITE_USER_PARAMETER(uInfo,USER_INFO_LOC,USER_INFO_LEN);
	return (1);
}

//静坐参数
uint8 ReadSitRemiandSetting(SIT_REMIAND_DATA_T *dat)
{
	if(dat != NULL)
	{
		uint8 sRinfo[SIT_REMIAND_LEN];
		READ_USER_PARAMETER(sRinfo,SIT_REMIAND_LOC,SIT_REMIAND_LEN);
		dat->SrSwitch = sRinfo[0];
		dat->SrDayFrequency = sRinfo[1];
		dat->SrFrequency = sRinfo[2];
		dat->SrBeginHour = sRinfo[3];
		dat->SrBeginMinute = sRinfo[4];
		dat->SrEndHour = sRinfo[5];
		dat->SrEndMinute = sRinfo[6];
		dat->SrStepThresholdL = sRinfo[7];
		dat->SrStepThresholdH = sRinfo[8];
	}
	return (1);
}

uint8 WriteSitRemiandSetting(uint8 *dat)
{
	WRITE_USER_PARAMETER(dat,SIT_REMIAND_LOC,SIT_REMIAND_LEN);
	return (1);
}

//用户间通信ID
uint8 ReadDeviceCommID(uint8 *dat)
{
	READ_USER_PARAMETER(dat,DEVICE_COMMID_LOC,DEVICE_COMMID_LEN);
	return (1);
}

uint8 WriteDeviceCommID(uint8 *dat)
{
	WRITE_USER_PARAMETER(dat,DEVICE_COMMID_LOC,DEVICE_COMMID_LEN);
	return (1);
}

/************************************Sector 3 存储 (运动/休眠总条数)************************************/
void READ_STEP_SLEEP_SUM(uint32 *data,uint8 Cnt,uint8 CLen)
{
	if(data != NULL)
		SpiFlash_Read(STEP_SLEEP_SUM_BASE_ADDR+Cnt, (uint8*)data, CLen);
}
void WRITE_STEP_SLEEP_SUM(uint32 *data,uint8 Cnt, uint8 CLen)
{
	if(data != NULL)
	{
		uint8 BufferForFlash[STEP_SLEEP_SUM_MAX_LEN]={0x00};
		SpiFlash_Read(STEP_SLEEP_SUM_BASE_ADDR, BufferForFlash, STEP_SLEEP_SUM_MAX_LEN);
		osal_memcpy(&BufferForFlash[Cnt],(uint8*)data,CLen);
		SpiFlash_4KSector_Erase(STEP_SLEEP_SUM_BASE_ADDR);
		SpiFlash_Write(STEP_SLEEP_SUM_BASE_ADDR,BufferForFlash,STEP_SLEEP_SUM_MAX_LEN);		
	}
}

/************************************Sector 4存储 (提醒) *********************************************/
//提醒:
uint8 GetRemindData(uint8 *remin, uint8 items)
{
	SpiFlash_Read(REMINDER_BASE_ADDR+PRE_REMINDER_BYTES*items, (unsigned char *)remin, PRE_REMINDER_BYTES);   
	return (1);
}

uint8 GetRemindHead(uint8 *remin, uint8 items)
{
	SpiFlash_Read(REMINDER_BASE_ADDR+PRE_REMINDER_BYTES*items, (unsigned char *)remin, sizeof(REMIND_DATA_T));          
	return 1;
}

bool GetRemindFreePage(uint8* page)
{  
	uint8 i;
	REMIND_DATA_T TempRemind;
	for(i=0;i<REMINDER_MAX_CNT;i++)
	{
		GetRemindHead((uint8*)&TempRemind,i);
		if(TempRemind.Id!=0xFF)
			continue;
		else
		{
			*page=i;		  
			return true;
		}
	}
	return false;
}

uint8 SetRemindData(uint8 *remin, uint8 items)
{
	uint8 RemindBuf[300]={0x00};
	SpiFlash_Read(REMINDER_BASE_ADDR,RemindBuf,300);
	osal_memcpy(&RemindBuf[PRE_REMINDER_BYTES*items],remin,PRE_REMINDER_BYTES);

	SpiFlash_4KSector_Erase(REMINDER_BASE_ADDR);
	SpiFlash_Write(REMINDER_BASE_ADDR,RemindBuf,300);
	return (1);
}

void ClearRemindData(void)
{
	SpiFlash_4KSector_Erase(REMINDER_BASE_ADDR);
}

/************************************Sector 5存储 (设备间通信好友信息)**********************************/
//设备间通信好友信息:
uint8 ResetDeviceComm_Friends(void)
{
	uint8 Dcf[FRIENDS_MAX*6]={0x00};
	SpiFlash_4KSector_Erase(DEVICES_COMM_BASE_ADDR);
	SpiFlash_Write(DEVICES_COMM_BASE_ADDR,Dcf,FRIENDS_MAX*6);	
	return (1);
}

uint8  WriteDeviceComm_Friends(void)
{
	uint8 Dcf[FRIENDS_MAX*6]={0x00};
	for(int8 i =0; i<FRIENDS_MAX;i++)
		memcpy((Dcf+i*6),mFriends[i],6);	
	SpiFlash_4KSector_Erase(DEVICES_COMM_BASE_ADDR);
	SpiFlash_Write(DEVICES_COMM_BASE_ADDR,Dcf,FRIENDS_MAX*6);
	return (1);
}

uint8  ReadDeviceComm_Friends(void)
{
	uint8 Dcf[FRIENDS_MAX*6]={0x00};
	SpiFlash_Read(DEVICES_COMM_BASE_ADDR,Dcf,FRIENDS_MAX*6);
	if(Dcf[0]==0xff)
		return (0);
	for(int8 i =0; i<FRIENDS_MAX;i++)
		memcpy(mFriends[i],(Dcf+i*6),6);
	return (1);
}
/************************************Sector 6存储 (OTA存储区域)**************************************/
void READ_OTA_INFO(uint8 *data,uint8 Cnt,uint8 CLen)
{
	if(data != NULL)
		SpiFlash_Read(OTA_INFO_BASE_ADDR+Cnt, data, CLen);
}
void WRITE_OTA_INFO(uint8 *data,uint8 Cnt, uint8 CLen)
{
	if(data != NULL)
	{
		uint8 BufferForFlash[OTA_INFO_MAX_LEN]={0x00};
		SpiFlash_Read(OTA_INFO_BASE_ADDR, BufferForFlash, OTA_INFO_MAX_LEN);
		osal_memcpy(&BufferForFlash[Cnt],data,CLen);
		SpiFlash_4KSector_Erase(OTA_INFO_BASE_ADDR);
		SpiFlash_Write(OTA_INFO_BASE_ADDR,BufferForFlash,OTA_INFO_MAX_LEN);		
	}
}
void Get_Ota_Addr(void)
{
	uint32_t Ota_Hr_Base_t=0x00000000UL,Ota_Tp_Base_t=0x00000000UL,Ota_Kl17_Base_t=0x00000000UL;				
	uint8_t BufferForFlash[OTA_INFO_MAX_LEN]={0x00};
	SpiFlash_Read(OTA_INFO_BASE_ADDR, BufferForFlash, OTA_INFO_MAX_LEN);
	Ota_Hr_Base_t = (BufferForFlash[OTA_HR_ADDR]<<24) | (BufferForFlash[OTA_HR_ADDR+1]<<16) |(BufferForFlash[OTA_HR_ADDR+2]<<8)|(BufferForFlash[OTA_HR_ADDR+3]);
	Ota_Tp_Base_t = (BufferForFlash[OTA_TP_ADDR]<<24) | (BufferForFlash[OTA_TP_ADDR+1]<<16) |(BufferForFlash[OTA_TP_ADDR+2]<<8)|(BufferForFlash[OTA_TP_ADDR+3]);
	Ota_Kl17_Base_t = (BufferForFlash[OTA_KL17_ADDR_LOC]<<24) | (BufferForFlash[OTA_KL17_ADDR_LOC+1]<<16) |(BufferForFlash[OTA_KL17_ADDR_LOC+2]<<8)|(BufferForFlash[OTA_KL17_ADDR_LOC+3]);
	if((Ota_Hr_Base_t!=0xFFFFFFFFUL)&&(Ota_Hr_Base_t!=0x00000000UL))
		Ota_Hr_Base=Ota_Hr_Base_t;
	if((Ota_Tp_Base_t!=0xFFFFFFFFUL)&&(Ota_Tp_Base_t!=0x00000000UL))
		Ota_Tp_Base=Ota_Tp_Base_t;
	if((Ota_Kl17_Base_t!=0xFFFFFFFFUL)&&(Ota_Kl17_Base_t!=0x00000000UL))
		Ota_Kl17_Base=Ota_Kl17_Base_t;
}

void Ota_Prepare(uint8 * otaD,uint8 len)
{
	uint8 BufferForFlash[OTA_INFO_MAX_LEN]={0x00};
	if(len>=15)
	{
		BufferForFlash[OTA_KL17_ADDR_LOC] = 	otaD[3];
		BufferForFlash[OTA_KL17_ADDR_LOC+1] = otaD[2];
		BufferForFlash[OTA_KL17_ADDR_LOC+2] = otaD[1];
		BufferForFlash[OTA_KL17_ADDR_LOC+3] = otaD[0];
		BufferForFlash[OTA_KL17_ADDR_LOC+4] = otaD[4];
		
		BufferForFlash[OTA_TP_ADDR] = 	   otaD[8];
		BufferForFlash[OTA_TP_ADDR+1] = otaD[7];
		BufferForFlash[OTA_TP_ADDR+2] = otaD[6];
		BufferForFlash[OTA_TP_ADDR+3] = otaD[5];
		BufferForFlash[OTA_TP_ADDR+4] = otaD[9];	
		
		BufferForFlash[OTA_HR_ADDR] =    otaD[13];
		BufferForFlash[OTA_HR_ADDR+1] = otaD[12];
		BufferForFlash[OTA_HR_ADDR+2] = otaD[11];
		BufferForFlash[OTA_HR_ADDR+3] = otaD[10];
		BufferForFlash[OTA_HR_ADDR+4] = otaD[14];	
		if(len==20)
		{
			BufferForFlash[OTA_NOR_ADDR] =    otaD[18];
			BufferForFlash[OTA_NOR_ADDR+1] = otaD[17];
			BufferForFlash[OTA_NOR_ADDR+2] = otaD[16];
			BufferForFlash[OTA_NOR_ADDR+3] = otaD[15];
			BufferForFlash[OTA_NOR_ADDR+4] = otaD[19];				
		}
		else
		{
			BufferForFlash[OTA_NOR_ADDR] = (Ota_Nor_Base>>24)&0xff;
			BufferForFlash[OTA_NOR_ADDR+1] = (Ota_Nor_Base>>16)&0xff;
			BufferForFlash[OTA_NOR_ADDR+2] = (Ota_Nor_Base>>8)&0xff;
			BufferForFlash[OTA_NOR_ADDR+3] = (Ota_Nor_Base)&0xff;
			BufferForFlash[OTA_NOR_ADDR+4] = Ota_Nor_SectorNum;	
		}
	}
	else
	{
		BufferForFlash[OTA_KL17_ADDR_LOC] = (Ota_Kl17_Base>>24)&0xff;
		BufferForFlash[OTA_KL17_ADDR_LOC+1] = (Ota_Kl17_Base>>16)&0xff;
		BufferForFlash[OTA_KL17_ADDR_LOC+2] = (Ota_Kl17_Base>>8)&0xff;
		BufferForFlash[OTA_KL17_ADDR_LOC+3] = (Ota_Kl17_Base)&0xff;
		BufferForFlash[OTA_KL17_ADDR_LOC+4] = Ota_Kl17_SectorNum;
		
		BufferForFlash[OTA_TP_ADDR] = 	(Ota_Tp_Base>>24)&0xff;
		BufferForFlash[OTA_TP_ADDR+1] = (Ota_Tp_Base>>16)&0xff;
		BufferForFlash[OTA_TP_ADDR+2] = (Ota_Tp_Base>>8)&0xff;
		BufferForFlash[OTA_TP_ADDR+3] = (Ota_Tp_Base)&0xff;
		BufferForFlash[OTA_TP_ADDR+4] = Ota_Tp_SectorNum;
		
		BufferForFlash[OTA_HR_ADDR] = (Ota_Hr_Base>>24)&0xff;
		BufferForFlash[OTA_HR_ADDR+1] = (Ota_Hr_Base>>16)&0xff;
		BufferForFlash[OTA_HR_ADDR+2] = (Ota_Hr_Base>>8)&0xff;
		BufferForFlash[OTA_HR_ADDR+3] = (Ota_Hr_Base)&0xff;
		BufferForFlash[OTA_HR_ADDR+4] = Ota_Hr_SectorNum;	

		BufferForFlash[OTA_NOR_ADDR] = (Ota_Nor_Base>>24)&0xff;
		BufferForFlash[OTA_NOR_ADDR+1] = (Ota_Nor_Base>>16)&0xff;
		BufferForFlash[OTA_NOR_ADDR+2] = (Ota_Nor_Base>>8)&0xff;
		BufferForFlash[OTA_NOR_ADDR+3] = (Ota_Nor_Base)&0xff;
		BufferForFlash[OTA_NOR_ADDR+4] = Ota_Nor_SectorNum;			
	}
	SpiFlash_4KSector_Erase(OTA_INFO_BASE_ADDR);
	SpiFlash_Write(OTA_INFO_BASE_ADDR,BufferForFlash,15);
	SpiFlash_Write(OTA_INFO_BASE_ADDR+OTA_NOR_ADDR,BufferForFlash+OTA_NOR_ADDR,5);
}

//空中升级成功标志
uint8  EraseOtaSuccessFlag(void)
{	
	if(OtaFlag!=0xFF)
	{
		SpiFlash_4KSector_Erase(OTA_INFO_BASE_ADDR);
	}
	return (1);
}

uint8  ReadOtaSuccessFlag(void)
{
	uint8 result =0;
	READ_OTA_INFO(&result,OTA_FLAG_LOC,OTA_FLAG_LEN);
	return result;
}

/************************************运动\睡眠等存储区域********************************************/
//运动数据
bool GetStepData(uint32 *Logs,SPORT_DATA_T *pData)
{
	uint32 GetTempData[5]={0x00};
	if((*Logs)>=STORE_STEP_DATA_MAX)
		return false;	
	
	SpiFlash_Read(SPORT_STORE_ADDR+PRE_SPORT_SAMPLE_BYTES*(*Logs), (uint8*)GetTempData, PRE_SPORT_SAMPLE_BYTES);

	pData->SportTimeStamp = GetTempData[0];
	pData->SportSteps = GetTempData[1];
	pData->SportKcal = GetTempData[2];
	pData->SportDistance = GetTempData[3];
	pData->SportDuration = GetTempData[4];
	return true;     
}


bool StoreStepData(uint32 *Logs,SPORT_DATA_T* tData)
{
	uint32 SetTempData[5]={0x00};			//用于设置存储数据
	
	SetTempData[0]=tData->SportTimeStamp;	
	SetTempData[1]=tData->SportSteps;		
	SetTempData[2]=tData->SportKcal;				  
	SetTempData[3]=tData->SportDistance;		
	SetTempData[4]=tData->SportDuration;	
	if((*Logs) == 0)
	{
		SpiFlash_4KSector_Erase(SPORT_STORE_ADDR);
	}
	SpiFlash_Write(SPORT_STORE_ADDR+PRE_SPORT_SAMPLE_BYTES*(*Logs) , (uint8*)SetTempData, PRE_SPORT_SAMPLE_BYTES);
	return true;    
}


//睡眠数据
bool GetSleepData(SLEEP_DATA_T *uData,uint32 *LogCnt)
{
	uint32 DataTemp[2]={0};
	if((*LogCnt)>=STORE_SLEEP_DATA_MAX)
		return false;
	SpiFlash_Read(SLEEP_STORE_ADDR+PRE_SLEEP_SAMPLE_BYTES*(*LogCnt), (uint8*)DataTemp, PRE_SLEEP_SAMPLE_BYTES);
	uData->SleepTimeStamp=DataTemp[0];
	uData->SleepType=DataTemp[1];
	return true;
}
bool StoreSleepData(SLEEP_DATA_T *uData,uint32 *LogCnt)
{
	uint32 DataTemp[2]={0};

	if((*LogCnt)>=STORE_SLEEP_DATA_MAX){
		SetSleepMemoryFullFlag(true);
		return false;
	}
	DataTemp[0]=uData->SleepTimeStamp;
	DataTemp[1]=uData->SleepType;
	
	if((*LogCnt) == 0)
	{
		SpiFlash_4KSector_Erase(SLEEP_STORE_ADDR);
	}	
	SpiFlash_Write(SLEEP_STORE_ADDR+PRE_SLEEP_SAMPLE_BYTES*(*LogCnt), (uint8*)DataTemp, PRE_SLEEP_SAMPLE_BYTES);
	return true;
}

//心率数据
bool GetHeartRateData(HEARTRATE_DATA_T *uData,uint32 *LogCnt)
{
	uint32 DataTemp[2]={0};
	if((*LogCnt)>=STORE_HEARTRATE_DATA_MAX)
		return false;
	SpiFlash_Read(HEARTRATE_STORE_ADDR+PRE_HEARTRATE_SAMPLE_BYTES*(*LogCnt), (uint8*)DataTemp, PRE_HEARTRATE_SAMPLE_BYTES);
	uData->HeartRateTimeStamp=DataTemp[0];
	uData->HeartRateVal=DataTemp[1];
	return true;
}

bool StoreHeartRateData(HEARTRATE_DATA_T *uData,uint32 *LogCnt)
{
	uint32 DataTemp[2]={0};

	if((*LogCnt)>=STORE_HEARTRATE_DATA_MAX){
		SetHeartRateMemoryFullFlag(true);
		return false;
	}
	DataTemp[0]=uData->HeartRateTimeStamp;
	DataTemp[1]=uData->HeartRateVal;
	
	if((*LogCnt) == 0)
	{
		SpiFlash_4KSector_Erase(HEARTRATE_STORE_ADDR);
		SpiFlash_4KSector_Erase(HEARTRATE_STORE_ADDR+0x1000UL);
	}
	
	SpiFlash_Write(HEARTRATE_STORE_ADDR+PRE_HEARTRATE_SAMPLE_BYTES*(*LogCnt), (uint8*)DataTemp, PRE_HEARTRATE_SAMPLE_BYTES);
	
	return true;
}

//计时运动记录数据
bool GetRecordWorkData(uint32 *Logs,REC_WORK_DATA_T *pData)
{
	uint32 GetTempData[6]={0x00};
	if((*Logs)>=RECORD_WORK_DATA_MAX)
		return false;	
	
	SpiFlash_Read(RECORD_WORK_STORE_ADDR+PRE_RECORD_WORK_BYTES*(*Logs), (uint8*)GetTempData, PRE_RECORD_WORK_BYTES);

	pData->TimeStamp = GetTempData[0];
	pData->Steps = GetTempData[1];
	pData->Kcal = GetTempData[2];
	pData->Distance = GetTempData[3];
	pData->Duration = GetTempData[4];
	pData->Hr = GetTempData[5];
	return true;     
}

bool StoreRecordWorkData(uint32 *Logs,REC_WORK_DATA_T* tData)
{
	uint32 SetTempData[6]={0x00};			//用于设置存储数据
	
	SetTempData[0]=tData->TimeStamp;	
	SetTempData[1]=tData->Steps;		
	SetTempData[2]=tData->Kcal;				  
	SetTempData[3]=tData->Distance;		
	SetTempData[4]=tData->Duration;	
	SetTempData[5]=tData->Hr;
	if((*Logs) == 0)
	{
		SpiFlash_4KSector_Erase(RECORD_WORK_STORE_ADDR);
		SpiFlash_4KSector_Erase(RECORD_WORK_STORE_ADDR+0x1000);
	}
	SpiFlash_Write(RECORD_WORK_STORE_ADDR+PRE_RECORD_WORK_BYTES*(*Logs) , (uint8*)SetTempData, PRE_RECORD_WORK_BYTES);
	return true;    
}

//保存显示数据
void SetDisplySportData(void)
{
	if(!UPDATE_FONTLIB)
	{
		uint8 SetBuf =0x01;
		uint8 TimerBuf[16]={0x00};
		uint32_encode(gSportDisplay.SportSteps,TimerBuf);
		uint32_encode(gSportDisplay.SportKcal,TimerBuf+4);
		uint32_encode(gSportDisplay.SportDistance,TimerBuf+8);
		uint32_encode(gSportDisplay.SportDuration,TimerBuf+12);
		if(SportDisplySaveIndex>=240)
		{
			SportDisplySaveIndex=0;
			SpiFlash_4KSector_Erase(SPORT_DIPLAY_SAVE_BASE_ADDR);	
		}	
		SpiFlash_Write(SPORT_DIPLAY_SAVE_BASE_ADDR+SportDisplySaveIndex,&SetBuf,1);
		SpiFlash_Write((SPORT_DIPLAY_SAVE_BASE_ADDR+256+SportDisplySaveIndex*16),TimerBuf,16);	
		gSportDisplayStore.SportSteps=    gSportDisplay.SportSteps;
		gSportDisplayStore.SportKcal=      gSportDisplay.SportKcal;
		gSportDisplayStore.SportDistance=gSportDisplay.SportDistance;
		gSportDisplayStore.SportDuration=gSportDisplay.SportDuration;		
		SportDisplySaveIndex++;
	}
}


void NeedSetDisplySportData(uint8 ui_index)
{
	bool NeedSet = false;
	switch(ui_index)
	{
		case 1:
			if(gSportDisplay.SportSteps!=gSportDisplayStore.SportSteps)
				NeedSet =true;
			break;
		case 2:
			if(gSportDisplay.SportDistance!=gSportDisplayStore.SportDistance)
				NeedSet =true;
			break;	
		case 3:
			if(gSportDisplay.SportKcal!=gSportDisplayStore.SportKcal)
				NeedSet =true;
			break;		
		case 4:
			if(gSportDisplay.SportDuration!=gSportDisplayStore.SportDuration)
				NeedSet =true;
			break;		
		default:
			break;
	}	
	if(NeedSet)
	{
		Force_NordicFlash();
		StoreSportDataToFlash(0);
		Free_NordicFlash();	
	}
}

uint16 GetEmptyDisplySportDataIndex(void)
{
	uint8 Dt[256]={0x00};
	SpiFlash_Read(SPORT_DIPLAY_SAVE_BASE_ADDR,Dt,256);
	for(uint8 i=0;i<240;i++)
	{
		if(Dt[i]==0xFF)
			return i;
	}
	return 240;
}
void GetDisplySportData(void)
{
	SportDisplySaveIndex = GetEmptyDisplySportDataIndex();
	if(SportDisplySaveIndex==0)
	{
		gSportDisplay.SportSteps=0;
		gSportDisplay.SportKcal=0;
		gSportDisplay.SportDistance=0;
		gSportDisplay.SportDuration=0;
	}
	else
	{
		uint8 TimerBuf[16]={0x00};
		SpiFlash_Read((SPORT_DIPLAY_SAVE_BASE_ADDR+256+(SportDisplySaveIndex-1)*16),TimerBuf,16);
		gSportDisplay.SportSteps=uint32_decode(TimerBuf);
		gSportDisplay.SportKcal=uint32_decode(TimerBuf+4);
		gSportDisplay.SportDistance=uint32_decode(TimerBuf+8);
		gSportDisplay.SportDuration=uint32_decode(TimerBuf+12);
		if(gSportDisplay.SportSteps>0xffffffUL ||gSportDisplay.SportKcal>0xffffffUL ||gSportDisplay.SportDistance>0xffffffUL||gSportDisplay.SportDuration>0xffffffUL )
		{
			gSportDisplay.SportSteps=0;
			gSportDisplay.SportKcal=0;
			gSportDisplay.SportDistance=0;
			gSportDisplay.SportDuration=0;			
		}
	}
	gSportDisplayStore.SportSteps=    gSportDisplay.SportSteps;
	gSportDisplayStore.SportKcal=      gSportDisplay.SportKcal;
	gSportDisplayStore.SportDistance=gSportDisplay.SportDistance;
	gSportDisplayStore.SportDuration=gSportDisplay.SportDuration;
}

/////////////////////////////////////////////////////////////ANCS Support Functions/////////////////////////////////////////////////////
uint8 SocailClass[32][2];

void UpdateSocialCount_SOCIAL(void)
{
	uint8 rm=ForAncsHandlerEnd(Ancs_Social);	
	if(rm!=0xff)
		gSocialCount=rm;
	else if(gSocialCount>0)
		gSocialCount--;		
}

void UpdateSocialCount(uint8 off)
{
	ANCS_SAVE_TYPE AstC =(ANCS_SAVE_TYPE)(off+Ancs_Social_Facebook);
	uint8 rm=ForAncsHandlerEnd_SOCIAL(AstC);
	if(rm!=0xff)
		SocialClassCount[(uint8)(AstC-Ancs_Social_Facebook)] = rm;
	else if(SocialClassCount[(uint8)(AstC-Ancs_Social_Facebook)]>0)
		SocialClassCount[(uint8)(AstC-Ancs_Social_Facebook)]--; 
}

void SavePhoneRecordFlag(void)
{
	uint8 Spr[9]={0x00};
	Spr[0]=gAncsFlash[6].wPos;
	uint32_encode(gAncsFlash[6].wMap,Spr+1);
	uint32_encode(gAncsFlash[6].rMap,Spr+5);
	WRITE_USER_PARAMETER(Spr,PHONE_RECORD_LOC,PHONE_RECORD_LEN);
}
void CleanWpos(ANCS_SAVE_TYPE ast,uint8 index)
{
	if(ast>=Ancs_Social_Facebook)
	{
		if(index==15)
		{
			for(uint8 i=16;i<32;i++)
			{
				SocailClass[i][0]=0;
				SocailClass[i][1]=0;
			}	
		}
		else if(index==31)
		{
			for(uint8 i=0;i<16;i++)
			{
				SocailClass[i][0]=0;
				SocailClass[i][1]=0;
			}	
		}
	}
}
void CleanWpos_SOCIAL(ANCS_SAVE_TYPE ast,uint8 index)
{
	uint8 find =(uint8)ast;
	if(index==15)
	{
		for(uint8 i=0;i<32;i++)
		{
			if(SocailClass[i][0]==find)
			{
				if(SocailClass[i][1]>15)
					SocailClass[i][1]=0;
			}
		}
	}
	else if(index==31)
	{
		for(uint8 i=0;i<32;i++)
		{
			if(SocailClass[i][0]==find)
			{
				if(SocailClass[i][1]<=15)
					SocailClass[i][1]=0;
			}
		}	
	}
}

void SetReadPos(ANCS_SAVE_TYPE ast,uint8 index)
{
	uint8 offset =0;
	uint8 wpos=0;
	offset = SocailClass[index][0];
	wpos = SocailClass[index][1];
	if(offset>=Ancs_Social_Facebook)
	{
		offset = (offset-Ancs_Social_Facebook);
		if((gSocialFlash[offset].rMap&(1UL<<wpos))>0)
		{
			gSocialFlash[offset].rMap&=(~(1UL<<wpos));		/*设置已读*/
			UpdateSocialCount(offset);
		}
	}
}

void SetReadPos_SOCIAL(ANCS_SAVE_TYPE ast,uint8 index)
{
	uint8 find =(uint8)ast;
	for(uint8 i=0;i<32;i++)
	{
		if(SocailClass[i][0]==find&&SocailClass[i][1]==index)
		{
			if((gAncsFlash[3].rMap&(1UL<<i))>0)
			{
				gAncsFlash[3].rMap&=(~(1UL<<i));		/*设置已读*/
				UpdateSocialCount_SOCIAL();
			}
		}
	}
	
}

void SetDeletePos(ANCS_SAVE_TYPE ast,uint8 index)
{
	uint8 offset =0;
	uint8 wpos=0;
	offset = SocailClass[index][0];
	wpos = SocailClass[index][1];
	if(offset>=Ancs_Social_Facebook)
	{
		offset = (offset-Ancs_Social_Facebook);
		gSocialFlash[offset].wMap &= ~(0x00000001UL<<wpos);
		gSocialFlash[offset].rMap &= ~(0x00000001UL<<wpos);	
	}
}

void SetDeletePos_SOCIAL(ANCS_SAVE_TYPE ast,uint8 index)
{
	uint8 find =(uint8)ast;
	for(uint8 i=0;i<32;i++)
	{
		if(SocailClass[i][0]==find&&SocailClass[i][1]==index)
		{
			gAncsFlash[3].wMap &= ~(0x00000001UL<<i);
			gAncsFlash[3].rMap &= ~(0x00000001UL<<i);	
		}
	}
	
}


uint8 Total_Count_SOCIAL(void)
{
	uint8 total=0;
	for(uint8 i=0;i<9;i++)
	{
		total+=SocialClassCount[i];
		if(total>99)
		{
			total=99;
			break;
		}
		
	}
	return total;
}
void Add_Count_SOCIAL(ANCS_SAVE_TYPE ast)
{
	uint8 offset = (uint8)(ast-Ancs_Social_Facebook);
	if(SocialClassCount[offset] < 99)	
		SocialClassCount[offset]+=1;
}

void Del_Count_SOCIAL(ANCS_SAVE_TYPE ast)
{
	uint8 offset = (uint8)(ast-Ancs_Social_Facebook);
	if(SocialClassCount[offset]>0)
		SocialClassCount[offset]-=1;
}

void SaveAncs_SOCIAL(ANCS_SAVE_TYPE ast, uint8 *dat, uint8 len, uint8 off)
{
	uint32 addr = 0;
	uint8 offset = (uint8)(ast-Ancs_Social_Facebook);
	addr = ANCS_SOCIAL_BASE_ADDR +(0x00002000UL*offset)+ (gSocialFlash[offset].wPos)*256;
	Force_NordicFlash();
	SpiFlash_Write(addr, (uint8*)dat,len);
	if(gSocialFlash[offset].wPos==15)
	{
		gSocialFlash[offset].wMap &= 0x0000FFFFUL;
		gSocialFlash[offset].rMap &= 0x0000FFFFUL;
		SpiFlash_4KSector_Erase(ANCS_SOCIAL_BASE_ADDR+(0x00002000UL*offset+0x00001000UL));	
		CleanWpos_SOCIAL(ast,15);
	}
	else if(gSocialFlash[offset].wPos==31)
	{
		gSocialFlash[offset].wMap &= 0xFFFF0000UL;
		gSocialFlash[offset].rMap &= 0xFFFF0000UL;
		SpiFlash_4KSector_Erase(ANCS_SOCIAL_BASE_ADDR+(0x00002000UL*offset));
		CleanWpos_SOCIAL(ast,31);
	}
	
	gSocialFlash[offset].wMap |= (1UL << gSocialFlash[offset].wPos);  	/*设置wMap*/
	gSocialFlash[offset].rMap |= (1UL << gSocialFlash[offset].wPos);  	/*设置rMap*/
	SocailClass[off][0]=(uint8)ast;
	SocailClass[off][1]=gSocialFlash[offset].wPos;
	gSocialFlash[offset].wPos++;
	if(gSocialFlash[offset].wPos>31)
		gSocialFlash[offset].wPos =0;
	Free_NordicFlash();
}

bool ReadAncsMsg_ToKl17_SOCIAL(ANCS_SAVE_TYPE ast,uint8 index, uint8 detail, uint8 line, DetailPageDir dir)
{
	uint32 addr = 0;
	uint8 layout[ANCS_LAYOUT_MAX] ={0};
	uint8 offset = (uint8)(ast-Ancs_Social_Facebook);
	uint8 rPos=0,RPosfind= 0,find_Pos=0;
	uint8 ReadOffset=0;
		
	memset(layout,0x40,ANCS_LAYOUT_MAX);
	addr = ANCS_SOCIAL_BASE_ADDR+(0x00002000UL*offset);
	if(gSocialFlash[offset].wPos==0)
		rPos=31; 
	else
		rPos=(gSocialFlash[offset].wPos-1); 	
	RPosfind = rPos; 
	
	if(detail<0x40)		/* index*line+detail 为相对第一个的偏移，detail=0x30代表第一个,detail=0x40代表预读界面*/						
	{
		if(detail<line)	
			ReadOffset = index*line+detail;
		else
			ReadOffset = 0;
	}			
	
	/*遍历表,寻找可读信息*/
	do{
		if(gSocialFlash[offset].wMap&(1UL<<RPosfind))		/*找到*/
		{		
			if(layout!=NULL)
			{
				if((detail<0x40) && (ReadOffset==find_Pos))
				{
					gSocialFlash[offset].rMap&=(~(1UL<<RPosfind));		/*设置已读*/
					SetReadPos_SOCIAL(ast,RPosfind);
				}
				if(gSocialFlash[offset].rMap&(1UL<<RPosfind))
					layout[find_Pos++]=RPosfind |0x20;		 /*未读信息*/
				else	
					layout[find_Pos++]=RPosfind;			 /*已读信息*/				
			}
			
		}
		if(RPosfind==0)
			RPosfind =31;
		else
			RPosfind--;
	}while((find_Pos<ANCS_LAYOUT_MAX)&&(RPosfind!=rPos));

	if(find_Pos>0)
	{
		Flash_Display_Ancs_LayOut(addr,layout,index,detail,dir);
		return true;
	}
	return false;
}

bool GetAncsMsgLayOut_SOCIAL(ANCS_SAVE_TYPE ast,uint8*Lo)
{
	uint8 offset = (uint8)(ast-Ancs_Social_Facebook);
	uint8 rPos=0,RPosfind= 0,find_Pos=0;
	bool findLayOut=false;	
	if(gSocialFlash[offset].wPos==0)
		rPos=31; 
	else
		rPos=(gSocialFlash[offset].wPos-1); 	
	RPosfind = rPos; 
	do{
		if(gSocialFlash[offset].wMap&(1UL<<RPosfind))
		{
			if(Lo!=NULL)
			{
				if(gSocialFlash[offset].rMap&(1UL<<RPosfind))
					Lo[find_Pos++]=RPosfind |0x20;		 /*未读信息*/
				else	
					Lo[find_Pos++]=RPosfind;			 /*已读信息*/			
			}
			findLayOut =true;
		}
		if(RPosfind==0)
			RPosfind =31;
		else
			RPosfind--;
	}while((find_Pos<ANCS_LAYOUT_MAX)&& (RPosfind!=rPos));
	return findLayOut;
}

uint8 ForAncsHandlerEnd_SOCIAL(ANCS_SAVE_TYPE ast)
{
	uint8 offset = (uint8)(ast-Ancs_Social_Facebook);
	uint8 rPos=0,RPosfind= 0,find_Pos=0,noRead=0;	
	if(gSocialFlash[offset].wPos==0)
		rPos=31; 
	else
		rPos=(gSocialFlash[offset].wPos-1); 	
	RPosfind = rPos;  
	do{
		if(gSocialFlash[offset].wMap&(1UL<<RPosfind))
		{
			if(gSocialFlash[offset].rMap&(1UL<<RPosfind))
				noRead++;
			find_Pos++;
		}
		if(RPosfind==0)
			RPosfind =31;
		else
			RPosfind--;
	}while((find_Pos<ANCS_LAYOUT_MAX)&& (RPosfind!=rPos));
	if(find_Pos<ANCS_LAYOUT_MAX)
		return noRead;
	else
		return 0xff;
}
uint8 DeleteAncsMsg_SOCIAL(ANCS_SAVE_TYPE ast,uint8 index)
{
	uint8 offset =(uint8)(ast-Ancs_Social_Facebook);
	uint8 rPos=0,RPosfind= 0,find_Pos=0;
	if(gSocialFlash[offset].wPos==0)
		rPos=31; 
	else
		rPos=(gSocialFlash[offset].wPos-1); 	
	RPosfind = rPos;  
	do{
		if(gSocialFlash[offset].wMap&(1UL<<RPosfind))
		{
			if(index==find_Pos)
			{
				gSocialFlash[offset].wMap &= ~(0x00000001UL<<RPosfind);
				gSocialFlash[offset].rMap &= ~(0x00000001UL<<RPosfind);
				SetDeletePos_SOCIAL(ast,RPosfind);
			}
			find_Pos++;
		}
		if(RPosfind==0)
			RPosfind =31;
		else
			RPosfind--;
	}while((find_Pos<ANCS_LAYOUT_MAX)&& (RPosfind!=rPos));
	return find_Pos;	
}


void SaveAncsMsg(ANCS_SAVE_TYPE ast, uint8 *dat, uint8 len)
{
	uint32 addr = 0;
	uint8 offset = (uint8)ast;
	offset =offset-1;
	if(offset>6)//offset=7 Ancs_Social_Gmail
	{
		if(ast>=Ancs_Social_Facebook)
		{
			SaveAncs_SOCIAL(ast,dat,len,gAncsFlash[3].wPos);
			Add_Count_SOCIAL(ast);
		}
		offset =3;
	}
	addr = ANCS_BASE_ADDR +(0x00002000UL*offset)+ (gAncsFlash[offset].wPos)*256;
	Force_NordicFlash();
	SpiFlash_Write(addr, (uint8*)dat,len);
	if(gAncsFlash[offset].wPos==15)
	{
		gAncsFlash[offset].wMap &= 0x0000FFFFUL;
		gAncsFlash[offset].rMap &= 0x0000FFFFUL;
		SpiFlash_4KSector_Erase(ANCS_BASE_ADDR+(0x00002000UL*offset+0x00001000UL));	
		CleanWpos(ast,15);
	}
	else if(gAncsFlash[offset].wPos==31)
	{
		gAncsFlash[offset].wMap &= 0xFFFF0000UL;
		gAncsFlash[offset].rMap &= 0xFFFF0000UL;
		SpiFlash_4KSector_Erase(ANCS_BASE_ADDR+(0x00002000UL*offset));
		CleanWpos(ast,31);
	}
	
	gAncsFlash[offset].wMap |= (1UL << gAncsFlash[offset].wPos);  /*设置wMap*/
	gAncsFlash[offset].rMap |= (1UL << gAncsFlash[offset].wPos);  	/*设置rMap*/
	gAncsFlash[offset].wPos++;
	if(gAncsFlash[offset].wPos>31)
		gAncsFlash[offset].wPos =0;
	//if(offset==5)
		//SavePhoneRecordFlag();
	Free_NordicFlash();
}

bool ReadAncsMsg_ToKl17(ANCS_SAVE_TYPE ast,uint8 index, uint8 detail, uint8 line, DetailPageDir dir)
{
	uint32 addr = 0;
	uint8 layout[ANCS_LAYOUT_MAX] ={0};
	uint8 offset = (uint8)ast;
	uint8 rPos=0,RPosfind= 0,find_Pos=0;
	uint8 ReadOffset=0;
	offset =offset-1;
	if(offset>6)
		offset =3;		
	memset(layout,0x40,ANCS_LAYOUT_MAX);
	addr = ANCS_BASE_ADDR+(0x00002000UL*offset);
	if(gAncsFlash[offset].wPos==0)
		rPos=31; 
	else
		rPos=(gAncsFlash[offset].wPos-1); 	
	RPosfind = rPos; 
	
	if(detail<0x40)		/* index*line+detail 为相对第一个的偏移，detail=0x30代表第一个,detail=0x40代表预读界面*/						
	{
		if(detail<line)	
			ReadOffset = index*line+detail;
		else
			ReadOffset = 0;
	}			
	
	/*遍历表,寻找可读信息*/
	do{
		if(gAncsFlash[offset].wMap&(1UL<<RPosfind))		/*找到*/
		{		
			if(layout!=NULL)
			{
				if((detail<0x40) && (ReadOffset==find_Pos))
				{
					gAncsFlash[offset].rMap&=(~(1UL<<RPosfind));		/*设置已读*/
					SetReadPos(ast,RPosfind);
					
				}
				if(gAncsFlash[offset].rMap&(1UL<<RPosfind))
					layout[find_Pos++]=RPosfind |0x20;		 /*未读信息*/
				else	
					layout[find_Pos++]=RPosfind;			 /*已读信息*/				
			}
			
		}
		if(RPosfind==0)
			RPosfind =31;
		else
			RPosfind--;
	}while((find_Pos<ANCS_LAYOUT_MAX)&&(RPosfind!=rPos));

	if(find_Pos>0)
	{
		Flash_Display_Ancs_LayOut(addr,layout,index,detail,dir);
		return true;
	}
	return false;
}

uint8 DeleteAncsMsg(ANCS_SAVE_TYPE ast,uint8 index)
{
	uint8 offset = (uint8)ast;
	uint8 rPos=0,RPosfind= 0,find_Pos=0;
	offset =offset-1;
	if(offset>6)
		offset =3;	
	if(gAncsFlash[offset].wPos==0)
		rPos=31; 
	else
		rPos=(gAncsFlash[offset].wPos-1); 	
	RPosfind = rPos;  
	do{
		if(gAncsFlash[offset].wMap&(1UL<<RPosfind))
		{
			if(index==find_Pos)
			{
				gAncsFlash[offset].wMap &= ~(0x00000001UL<<RPosfind);
				gAncsFlash[offset].rMap &= ~(0x00000001UL<<RPosfind);
				SetDeletePos(ast,RPosfind);
			}
			find_Pos++;
		}
		if(RPosfind==0)
			RPosfind =31;
		else
			RPosfind--;
	}while((find_Pos<ANCS_LAYOUT_MAX)&& (RPosfind!=rPos));
	return find_Pos;	
}

uint8 ForAncsHandlerEnd(ANCS_SAVE_TYPE ast)
{
	uint8 offset = (uint8)ast;
	uint8 rPos=0,RPosfind= 0,find_Pos=0,noRead=0;
	offset =offset-1;
	if(offset>6)
		offset =3;	
	if(gAncsFlash[offset].wPos==0)
		rPos=31; 
	else
		rPos=(gAncsFlash[offset].wPos-1); 	
	RPosfind = rPos;  
	do{
		if(gAncsFlash[offset].wMap&(1UL<<RPosfind))
		{
			if(gAncsFlash[offset].rMap&(1UL<<RPosfind))
				noRead++;
			find_Pos++;
		}
		if(RPosfind==0)
			RPosfind =31;
		else
			RPosfind--;
	}while((find_Pos<ANCS_LAYOUT_MAX)&& (RPosfind!=rPos));
	if(find_Pos<ANCS_LAYOUT_MAX)
		return noRead;
	else
		return 0xff;
}

bool GetAncsMsgLayOut(ANCS_SAVE_TYPE ast,uint8*Lo)
{
	uint8 offset = (uint8)ast;
	uint8 rPos=0,RPosfind= 0,find_Pos=0;
	bool findLayOut=false;
	offset =offset-1;
	if(offset>6)
		offset =3;	
	if(gAncsFlash[offset].wPos==0)
		rPos=31; 
	else
		rPos=(gAncsFlash[offset].wPos-1); 	
	RPosfind = rPos; 
	do{
		if(gAncsFlash[offset].wMap&(1UL<<RPosfind))
		{
			if(Lo!=NULL)
			{
				if(gAncsFlash[offset].rMap&(1UL<<RPosfind))
					Lo[find_Pos++]=RPosfind |0x20;		 /*未读信息*/
				else	
					Lo[find_Pos++]=RPosfind;			 /*已读信息*/			
			}
			findLayOut =true;
		}
		if(RPosfind==0)
			RPosfind =31;
		else
			RPosfind--;
	}while((find_Pos<ANCS_LAYOUT_MAX)&& (RPosfind!=rPos));
	return findLayOut;
}

uint32 ReadAncsMsg(ANCS_SAVE_TYPE ast,uint8 index)
{
	uint32 addr = 0;
	uint8 layout[ANCS_LAYOUT_MAX] ={0};
	uint8 offset = (uint8)ast;
	offset =offset-1;
	if(offset>6)
		offset =3;	
	memset(layout,0x40,ANCS_LAYOUT_MAX);
	addr = ANCS_BASE_ADDR+(0x00002000UL*offset);

	if(GetAncsMsgLayOut(ast,layout))
	{
		if(layout[index]!=0x40)
			return (addr+256*(layout[index]&0x1F));
	}
	return 0;
}


void ClearAncsFlash(void)
{
	Force_NordicFlash();
	SpiFlash_4KSector_Erase(ANCS_BASE_ADDR);
	SpiFlash_4KSector_Erase(ANCS_BASE_ADDR+0x00001000UL);
	
	SpiFlash_4KSector_Erase(ANCS_BASE_ADDR+0x00002000UL);
	SpiFlash_4KSector_Erase(ANCS_BASE_ADDR+0x00003000UL);
	
	SpiFlash_4KSector_Erase(ANCS_BASE_ADDR+0x00004000UL);
	SpiFlash_4KSector_Erase(ANCS_BASE_ADDR+0x00005000UL);
	
	SpiFlash_4KSector_Erase(ANCS_BASE_ADDR+0x00006000UL);
	SpiFlash_4KSector_Erase(ANCS_BASE_ADDR+0x00007000UL);
	
	SpiFlash_4KSector_Erase(ANCS_BASE_ADDR+0x00008000UL);
	SpiFlash_4KSector_Erase(ANCS_BASE_ADDR+0x00009000UL);	

	SpiFlash_4KSector_Erase(ANCS_BASE_ADDR+0x0000A000UL);
	SpiFlash_4KSector_Erase(ANCS_BASE_ADDR+0x0000B000UL);	
	
	SpiFlash_4KSector_Erase(ANCS_BASE_ADDR+0x0000C000UL);
	SpiFlash_4KSector_Erase(ANCS_BASE_ADDR+0x0000D000UL);		


	for(uint8 i=0;i<9;i++)
	{
		SpiFlash_4KSector_Erase(ANCS_SOCIAL_BASE_ADDR+0x00000000UL+0x00002000UL*i);
		SpiFlash_4KSector_Erase(ANCS_SOCIAL_BASE_ADDR+0x00001000UL+0x00002000UL*i);
	}
	Free_NordicFlash();

	memset(gAncsFlash,0,sizeof(gAncsFlash));
	memset(gSocialFlash,0,sizeof(gSocialFlash));
	for(uint8 i=0;i<32;i++)
	{
		SocailClass[i][0]=0;
		SocailClass[i][1]=0;
	}
	for(uint8 i=0;i<9;i++)
	{	
		SocialClassCount[i]=0;
	}
	gShortMsgCount=0;
	gIncomingCallCount=0;
	gMissedCallCount=0;
	gEMailCount=0;
	gSocialCount=0;
	gScheduleCount =0;
	gShortMsgCountT=0;
	gIncomingCallCountT=0;
	gMissedCallCountT=0;
	gEMailCountT=0;
	gScheduleCountT=0;
	gSocialCountT=0;

}

void ReflashAncsCount(void)
{
	if(!GetAncsMsgLayOut(Ancs_MissedCall,NULL))
		gMissedCallCount=0;
	if(!GetAncsMsgLayOut(Ancs_Sms,NULL))
		gShortMsgCount=0;		
	if(!GetAncsMsgLayOut(Ancs_Email,NULL))
		gEMailCount=0;
	if(!GetAncsMsgLayOut(Ancs_Schedule,NULL))
		gScheduleCount=0;
}

void ClearPhoneRecordFlash(void)
{
	Force_NordicFlash();
	SpiFlash_4KSector_Erase(ANCS_BASE_ADDR+0x0000C000UL);
	SpiFlash_4KSector_Erase(ANCS_BASE_ADDR+0x0000D000UL);		
	Free_NordicFlash();
}

void GetOTALanguageInfo(void)
{
	uint8 Oli[4]={0x00};
	SpiFlash_Read(Font_Base, Oli, 4);
	OTALanguagetype=Oli[0];
	OTALanguageVersion[0]=Oli[2];
	OTALanguageVersion[1]=Oli[3];
	if(SetValue.Language!=OTALanguagetype)
		SetValue.Language = LastLanguage;
}


//////////////////////PhoneBook Support Functions////////////////////////////////////////
static PHFlash gPHFlash; 		/*10个扇区: 50*800=10k 最多800条*/
#define	PH_ID_LEN			1
#define	PH_CNT_LEN			2
#define	PH_INF_LEN			46
#define MAX_CNT_NUMNAME	3
#define MAX_CNT_GROUP		7

bool	PhB_Save = false;
uint32 InfAddr_Curr =0;
uint32 GrpAddr_Curr =0;
uint8 GrpIndex_Curr =0;
uint8 InfCnt_Curr =0;
phonebook_data_t  PH_Data;
uint8 IsFirst = 0;
uint8 Last_ID = 0;
bool Android_ios = false;

void SetPhBok_Flag(uint8 IsSave)
{
	PhoneBoookSave = IsSave;
	WRITE_USER_PARAMETER(&PhoneBoookSave,PHONE_BOOK_LOC,1);
}

uint8 GetPhBok_Flag(void)
{
	return PhoneBoookSave;
}

void ResetWpos(void)
{
	gPHFlash.WPos = 0;
}

void ResetGWpos(void)
{
	gPHFlash.G_WPos = 0;
}
void Cnt_Wpos(void)
{
	gPHFlash.WPos++;
	if(gPHFlash.WPos>800)
		gPHFlash.WPos = 800;
}

void Cnt_GWpos(void)
{
	gPHFlash.G_WPos++;
	if(gPHFlash.G_WPos>30)
		gPHFlash.G_WPos = 30;
}

void SetMaxCnt(void)
{
	uint8 Buf[3]={0x00,0x00,0x00};
	uint32 addr = PH_INF_MAXCNT_BASE_ADDR;
	uint16_encode(gPHFlash.WPos, Buf);
	Buf[2] = gPHFlash.G_WPos;
	SpiFlash_Write(addr, (uint8*)Buf,3);
}

void SetAndroid_iosFalg(bool IsApk_App) // android is true, ios is false
{
	uint8 ApkApp_Flag = IsApk_App;
	SpiFlash_Write(PH_ANDRIOD_IOS_BASE_ADDR, &ApkApp_Flag,1);
	Android_ios = IsApk_App;
}


uint16 GetInfMaxCnt(void)
{
	uint8 Buf[2]={0x00,0x00};
	uint32 addr = PH_INF_MAXCNT_BASE_ADDR;
	uint16 MaxInfCnt = 0;
	SpiFlash_Read(addr, (uint8*)Buf,2);
	MaxInfCnt = uint16_decode(Buf);
	return MaxInfCnt;
}

uint8 GetGrpMaxCnt(void)
{
	uint32 addr = PH_GRP_MAXCNT_BASE_ADDR;
	uint8 MaxGrpCnt = 0;
	SpiFlash_Read(addr, &MaxGrpCnt,1);
	return MaxGrpCnt;
}

void GetAndroid_iosFalg(void) // android is true, ios is false
{
	uint8 IsApk_App = 0;
	SpiFlash_Read(PH_ANDRIOD_IOS_BASE_ADDR, &IsApk_App,1);
	if(IsApk_App == 0xFF)
		IsApk_App = 0;
	Android_ios = (bool)IsApk_App;
}

void ReadPBook_id_toFlash(uint8 *dat, uint8 len)
{
	uint32 addr = PH_GROUP_ID_BASE_ADDR + len* 1;
	Force_NordicFlash();
	SpiFlash_Read(addr, (uint8*)dat,1);
	Free_NordicFlash();
	
}

void ReadPBook_CNT_toFlash(uint8 *dat, uint8 len)
{
	uint32 addr = PH_GROUP_CNT_BASE_ADDR + len* 2;
	Force_NordicFlash();
	SpiFlash_Read(addr, (uint8*)dat,2);
	Free_NordicFlash();
	
}

void ReadPBook_INF_toFlash(uint8 *dat, uint8 len)
{
	uint32 addr = PHONEBOOK_BASE_ADDR + len * 46;
	Force_NordicFlash();
	SpiFlash_Read(addr, (uint8*)dat,46);
	Free_NordicFlash();
	
}

void GetPhBook_idAddr(uint32 *PIdaddr, uint8 Grp_Index)
{
	*PIdaddr = PH_GROUP_ID_BASE_ADDR + Grp_Index* PH_ID_LEN;	
}

void GetPBook_Cnt(uint8 *dat, uint8 Grp_Index)
{
	uint32 addr = PH_GROUP_CNT_BASE_ADDR + Grp_Index* 2;
	SpiFlash_Read(addr, (uint8*)dat,2);
	
}
void GetPhBook_InfAddr(uint32 *PInfAddr, uint16 Show_Index)
{
	*PInfAddr = PHONEBOOK_BASE_ADDR + Show_Index * PH_INF_LEN;	
}


void SaveCurrGrpAddrIndex(uint32 Inf_addr, uint8 Grp_Index)
{
	GrpIndex_Curr = Grp_Index;
	InfAddr_Curr = Inf_addr;
}


bool GetChooseGrp_Addr(uint32 *PInf_addr,uint8 *PInf_Cnt,uint32 *PGrp_addr,uint8* PGrp_Cnt, uint16 ChoseIndex, uint16 *PtrShowIndex)
{
	Force_NordicFlash();
	uint16 Grp_Cnt_Pre= 0;
	uint8 Buf[2] = {0x00, 0x00};
	uint16 Max_InfCnt = GetInfMaxCnt() + 1;
	uint8 Max_GrpCnt = GetGrpMaxCnt() + 1;
	uint8 PreGrp_Index = 0;
	uint8 Grp_Index = GrpIndex_Curr + (uint8)ChoseIndex;	
	
	if(Grp_Index == 0)
	{
		PreGrp_Index = 0; 
	}
	else
		PreGrp_Index = Grp_Index - 1;
	
	memset(Buf,0,2);
	GetPBook_Cnt(Buf,PreGrp_Index);
	Grp_Cnt_Pre = uint16_decode(Buf);
	
	if(Grp_Cnt_Pre == 0xffff )
	{
		Free_NordicFlash();
		return false;
	}	
	GetPhBook_idAddr(PGrp_addr,Grp_Index);
	
	if(Grp_Index == 0)
	{
		*PInf_addr = PHONEBOOK_BASE_ADDR;
	}		
	else
	{
		*PInf_addr = PHONEBOOK_BASE_ADDR + Grp_Cnt_Pre*PH_INF_LEN;
	}
	
	SaveCurrGrpAddrIndex(*PInf_addr,Grp_Index);
	
	 *PtrShowIndex = Grp_Cnt_Pre;
	 	
	if((Max_InfCnt -Grp_Cnt_Pre)>3)
		*PInf_Cnt = 3;
	else
		*PInf_Cnt = (uint8)(Max_InfCnt -Grp_Cnt_Pre);
	if((Max_GrpCnt -Grp_Index)>7)
		*PGrp_Cnt = 7;
	else
		*PGrp_Cnt = Max_GrpCnt -Grp_Index;
	InfCnt_Curr = *PInf_Cnt;
	Free_NordicFlash();
	return true;
}
bool GetChooseInfNum(uint32 *InfAddr_Chs,uint8 ChooseIndex)
{
	if((ChooseIndex + 1) > InfCnt_Curr)
		return false;

	*InfAddr_Chs = InfAddr_Curr + ChooseIndex * PH_INF_LEN;
	
	if(*InfAddr_Chs< PHONEBOOK_BASE_ADDR)
		*InfAddr_Chs = PHONEBOOK_BASE_ADDR;
	if(*InfAddr_Chs>PhoneBook_End)
		*InfAddr_Chs = PhoneBook_End;
	
	return true;

}

bool GetPhBook_GrpIdAddr(uint32 *PGrp_addr,uint8 *PGrp_Index,uint16 Show_Index)
{
	uint16 Grp_cnt = 0;
	uint8 Grp_Index = 0;
	bool IsFind = false;
	uint8 MaxGrpCnt = GetGrpMaxCnt();
	uint8 Buf_Cnt[2] = {0x00,0x00};
	for(uint8 i = 0; i <= MaxGrpCnt; i++)
	{
		memset(Buf_Cnt, 0, 2);
		GetPBook_Cnt(Buf_Cnt,i);
		Grp_cnt = uint16_decode(Buf_Cnt);
		if(Grp_cnt == 0XFFFF)
			break;
		if(Show_Index < Grp_cnt)
		{
			Grp_Index = i ;
			IsFind = true;
			break;
		}		 
	}
	if(IsFind)
	{
		
		*PGrp_Index = Grp_Index;
		GetPhBook_idAddr(PGrp_addr,Grp_Index);	
		GrpIndex_Curr = Grp_Index;
		return true;
	}	
	return false;
}


bool GetPhoneBookInf(uint32 *PInf_addr,uint8 *PInf_Cnt,uint32 *PGrp_addr,uint8* PGrp_Cnt, uint16 Show_Index)
{
	Force_NordicFlash();
	uint16 Max_InfCnt = GetInfMaxCnt() + 1;
	uint8 Max_GrpCnt = GetGrpMaxCnt()+ 1;
	uint8 Grp_Index = 0;
	GetPhBook_InfAddr(PInf_addr,Show_Index);
	GetPhBook_GrpIdAddr(PGrp_addr,&Grp_Index,Show_Index);
	InfAddr_Curr = *PInf_addr;
	if((Max_InfCnt -Show_Index)>3)
		*PInf_Cnt = 3;
	else
		*PInf_Cnt = (uint8)(Max_InfCnt -Show_Index);
	InfCnt_Curr = *PInf_Cnt;
	if((Max_GrpCnt -Grp_Index)>7)
		*PGrp_Cnt = 7;
	else
		*PGrp_Cnt = Max_GrpCnt -Grp_Index;
	
	Free_NordicFlash();
	return true;
}


void SavePBook_ID_toFlash(uint8 *dat, uint8 len)
{
	uint32 addr = PH_GROUP_ID_BASE_ADDR +  gPHFlash.G_WPos * len;
	SpiFlash_Write(addr, (uint8*)dat,len);

}

void SavePBook_Cnt_toFlash(uint8 *dat, uint8 len)
{
	uint32 addr = PH_GROUP_CNT_BASE_ADDR +  gPHFlash.G_WPos* len;
	SpiFlash_Write(addr, (uint8*)dat,len);
}

void SavePBook_NumName_toFlash(uint8 *dat, uint8 len)
{
	uint32 addr = PHONEBOOK_BASE_ADDR + gPHFlash.WPos * len;
	SpiFlash_Write(addr, (uint8*)dat,len);
}

void SavePBookto_Flash(PHONEBOOK_SAVE_ID PH_Id, uint8 *dat, uint8 len)
{
	switch(PH_Id)
	{
		case ID_ID:
			SavePBook_ID_toFlash(dat, len);
			break;
		case ID_CNT:
			SavePBook_Cnt_toFlash(dat, len);
			break;
		case ID_NUM_NAME:
			SavePBook_NumName_toFlash(dat, len);			
			Cnt_Wpos();
			break;
		default:
			break;
	}

}


void InitPhoneBookBuf(void)
{
	memset(PH_Data.PH_Inf,0,46);
	PH_Data.Group_ID =0;
	PH_Data.Group_Cnt = 0;
	ResetWpos();
	ResetGWpos();
}

void CNT_GroupId(void)
{
	PH_Data.Group_Cnt ++;
}

void Reset_GrpCnt(void)
{
	PH_Data.Group_Cnt = 1;
}


void SavePhoneBookToBuf(uint8* Data, bool save)
{
	uint8 Curr_ID = *(Data + 1);
	uint8 Buf_CNT[4] = {0x00};
	memset(Buf_CNT,0,4);
	memset(PH_Data.PH_Inf,0,46);
	if(*Data == 0X00)
		SetAndroid_iosFalg(false);
	else if( *Data == 0XA0)
		SetAndroid_iosFalg(true);
	if(*Data == 0X00 || *Data == 0XA0)
	{
		Last_ID = Curr_ID;
		CNT_GroupId();
	}
	else if(*Data == 0X01 || *Data == 0XA1)
	{
		if(Last_ID != Curr_ID) 
		{
			PH_Data.Group_ID = Last_ID;
			SavePBookto_Flash(ID_ID,&PH_Data.Group_ID,1);
			uint16_encode(PH_Data.Group_Cnt,Buf_CNT);
			SavePBookto_Flash(ID_CNT,Buf_CNT,2);
			Last_ID = Curr_ID;
			Cnt_GWpos();	
		}
		CNT_GroupId();
	}
	else if(*Data == 0X02 || *Data == 0XA2)
	{
		if(Last_ID != Curr_ID) 
		{
			SavePBookto_Flash(ID_ID,&Last_ID,1);
			uint16_encode(PH_Data.Group_Cnt,Buf_CNT);
			SavePBookto_Flash(ID_CNT,Buf_CNT,2);
			Cnt_GWpos();
			
		}
		CNT_GroupId();
		SavePBookto_Flash(ID_ID,&Curr_ID,1);
		uint16_encode(PH_Data.Group_Cnt,Buf_CNT);
		SavePBookto_Flash(ID_CNT,Buf_CNT,2);
		SetMaxCnt();
	}
	else
	{
		PH_Data.Group_Cnt = 1;
		ResetWpos();
		ResetGWpos();
		uint16_encode(PH_Data.Group_Cnt,Buf_CNT);
		SavePBookto_Flash(ID_ID,(Data + 1),1);
		SavePBookto_Flash(ID_CNT,Buf_CNT,2);
		SetMaxCnt();
	}
	
	if(save)
	{		
		memcpy(PH_Data.PH_Inf,Data+4,*(Data+2));
		memcpy(&PH_Data.PH_Inf[16],Data+4+*(Data+2),*(Data+3));
		SavePBookto_Flash(ID_NUM_NAME,PH_Data.PH_Inf,46);			
	}	
}

uint8 ShowedCntPercent(uint16 ShowedCnt)
{
	uint8 Percent_Showed = 0;
	Force_NordicFlash();
	uint16 Max_InfCnt = GetInfMaxCnt() + 1;
	Free_NordicFlash();
	if((Max_InfCnt - ShowedCnt) > 3)
		Percent_Showed = (ShowedCnt*10)/Max_InfCnt;
	else
		Percent_Showed = 10;
		
	return Percent_Showed;
}

bool ReadChosePhoneBook_ToKl17(uint8 ChsGrpIndex, uint16 *PtrShowIndex)
{
		uint32 Inf_addr = 0;
		uint32 Grp_addr = 0;
		uint8 Inf_Cnt = 0;	
		uint8 Grp_Cnt = 0;
		uint8 Percent_Showed = 0;
		if(GetChooseGrp_Addr(&Inf_addr,&Inf_Cnt,&Grp_addr,&Grp_Cnt,ChsGrpIndex, PtrShowIndex))
		{
			if(Grp_Cnt == 0 ||Inf_Cnt == 0)
				return false;
			Percent_Showed = ShowedCntPercent(*PtrShowIndex + 3);
			Flash_Display_PhBook_Inf(Inf_addr,Inf_Cnt,Grp_addr,Grp_Cnt,Percent_Showed);
			return true;
		}
		return false;

}
bool ReadPhoneBook_ToKl17(uint8 Percent_Showed,uint16 Show_Index)
{
	uint32 Inf_addr = 0;
	uint32 Grp_addr = 0;
	uint8 Inf_Cnt = 0;	
	uint8 Grp_Cnt = 0;	
	if(GetPhoneBookInf(&Inf_addr,&Inf_Cnt,&Grp_addr,&Grp_Cnt,Show_Index))
	{
		if(Grp_Cnt == 0 ||Inf_Cnt == 0)
			return false;
		Flash_Display_PhBook_Inf(Inf_addr,Inf_Cnt,Grp_addr,Grp_Cnt,Percent_Showed);
		return true;
	}	
	return false;
}


void ClearPhoneBookFlash(void)
{
	SpiFlash_4KSector_Erase(PHONEBOOK_BASE_ADDR);
	SpiFlash_4KSector_Erase(PHONEBOOK_BASE_ADDR+0x00001000UL);
	
	SpiFlash_4KSector_Erase(PHONEBOOK_BASE_ADDR+0x00002000UL);
	SpiFlash_4KSector_Erase(PHONEBOOK_BASE_ADDR+0x00003000UL);
	
	SpiFlash_4KSector_Erase(PHONEBOOK_BASE_ADDR+0x00004000UL);
	SpiFlash_4KSector_Erase(PHONEBOOK_BASE_ADDR+0x00005000UL);
	
	SpiFlash_4KSector_Erase(PHONEBOOK_BASE_ADDR+0x00006000UL);
	SpiFlash_4KSector_Erase(PHONEBOOK_BASE_ADDR+0x00007000UL);

	SpiFlash_4KSector_Erase(PHONEBOOK_BASE_ADDR+0x00008000UL);
	SpiFlash_4KSector_Erase(PHONEBOOK_BASE_ADDR+0x00009000UL);
	
}

void ClearAncs_IncomingCall(void)
{
	Force_NordicFlash();
	SpiFlash_4KSector_Erase(ANCS_BASE_ADDR);						//Ancs_IncomingCall
	SpiFlash_4KSector_Erase(ANCS_BASE_ADDR+0x00001000UL);	
	Free_NordicFlash();
	gIncomingCallCount=0;
	gIncomingCallCountT=0;
	gAncsFlash[0].wMap= 0;
	gAncsFlash[0].rMap= 0;
	gAncsFlash[0].wPos = 0;
}
void ClearAncs_MissedCall(void)
{
	Force_NordicFlash();
	SpiFlash_4KSector_Erase(ANCS_BASE_ADDR+0x00002000UL);			//Ancs_MissedCall
	SpiFlash_4KSector_Erase(ANCS_BASE_ADDR+0x00003000UL);
	Free_NordicFlash();
	gMissedCallCount=0;
	gMissedCallCountT=0;
	gAncsFlash[1].wMap= 0;
	gAncsFlash[1].rMap= 0;
	gAncsFlash[1].wPos = 0;
}

void ClearAncs_Sms(void)
{
	Force_NordicFlash();
	SpiFlash_4KSector_Erase(ANCS_BASE_ADDR+0x00004000UL);			//Ancs_Sms
	SpiFlash_4KSector_Erase(ANCS_BASE_ADDR+0x00005000UL);
	Free_NordicFlash();
	gShortMsgCount=0;
	gShortMsgCountT=0;
	gAncsFlash[2].wMap= 0;
	gAncsFlash[2].rMap= 0;
	gAncsFlash[2].wPos = 0;
}


void ClearAncs_Social(void)
{
	Force_NordicFlash();	
	SpiFlash_4KSector_Erase(ANCS_BASE_ADDR+0x00006000UL);			//Ancs_Social
	SpiFlash_4KSector_Erase(ANCS_BASE_ADDR+0x00007000UL);
	Free_NordicFlash();
	gSocialCount=0;
	gSocialCountT=0;
	gAncsFlash[3].wMap= 0;
	gAncsFlash[3].rMap= 0;
	gAncsFlash[3].wPos = 0;
}

void ClearAncs_Email(void)
{
	Force_NordicFlash();	
	SpiFlash_4KSector_Erase(ANCS_BASE_ADDR+0x00008000UL);			//Ancs_Email
	SpiFlash_4KSector_Erase(ANCS_BASE_ADDR+0x00009000UL);	
	Free_NordicFlash();
	gEMailCount=0;
	gSocialCountT=0;
	gAncsFlash[4].wMap= 0;
	gAncsFlash[4].rMap= 0;
	gAncsFlash[4].wPos = 0;
}

void ClearAncs_PhoneRecord(void)
{
	ClearPhoneRecordFlash();
	gAncsFlash[6].wMap= 0;
	gAncsFlash[6].rMap= 0;
	gAncsFlash[6].wPos = 0;
}


void BindEndDealWith(void)
{
	WRITE_USER_PARAMETER((uint8 *)&UserID,USER_UID_LOC, USER_UID_LEN); 
	if(!IsSameUser)
	{
			uint8 Temp[AUTO_SLEEP_LEN]={0x00};
			uint8 SetBuf[4]={0x00};
			memset(&AutoSleepTime,0,sizeof(AutoSleepTime));	
			WRITE_USER_PARAMETER(Temp,AUTO_SLEEP_LOC,AUTO_SLEEP_LEN);
			CleanAutoSleepFlag();
			gSwitchSetting.PMD.PMD3 = 0;
			SetBuf[0]=gSwitchSetting.u32PMD & 0xFF;
			SetBuf[1]=(gSwitchSetting.u32PMD>>8) & 0xFF;
			SetBuf[2]=(gSwitchSetting.u32PMD>>16) & 0xFF;
			SetBuf[3]=(gSwitchSetting.u32PMD>>24) & 0xFF;
			WRITE_USER_PARAMETER(SetBuf,SWITCH_LOC,SWITCH_LEN);
	}
}

