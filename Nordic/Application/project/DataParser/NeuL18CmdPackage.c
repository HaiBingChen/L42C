/******************************************************************************************
*文件名：		NeuL18CmdPackage.c
*说明：			L18通讯协议数据解析实现文件
*编写:			statdream
*版本信息:		V1.0
******************************************************************************************/
#include "headfile.h"

#define WATCH_ID_PREFIX   "FCL42A"


const uint8 SoftWareVersion[20]="N0.4K0.0R0.1T0.0H0.0";

const uint8 DeviceType[4]="L42A";

const uint8 HardWareVersion[14]="L42A_HW_V1.0.0";
const uint8 ProtocolVersion[3]    ="1.0";
const uint8 FunctionVersion[35]  ="ACC_V1.0.5_151614\nPAY_V1.0.5_151614";

uint8 Pay_Aid = 0;
uint8 pay_data[12] = {0};

uint8 CurrentDataParseCmd =0; 
uint8 CurrentDataParseAction=0;
uint16 CurrentDataParseLen=0;
uint8 AndroidSmsFlag = 0,AndroidMissCallFlag = 0,AndroidSocialFlag = 0,AndroidEmailFlag = 0,AndroidScheduleFlag = 0;	

extern uint8 SetCRCFlag(void);

//命令列表：格式{命令,数据有效长度,命令函数}
static const DATA_CMD_T DataCmdTable[]={
	{0x01,	GetCmdRespond},							/*收到手机回应*/			
	{0x02,	GetWatchIdCmdCB},							/*设备WatchID*/
	{0x03,	GetVersionMsgCmdCB},						/*设备版本信息*/
	{0x04,	DateTimeSettingCmdCB},						/*日期时间*/
	{0x05,	TimeOrderCmdCB},							/*时间界面设置*/
	{0x06,	MenuOrderCmdCB},							/*一级界面显示设置*/
	{0x07,	BacklightSettingCmdCB},						/*设备屏幕亮度*/
	{0x08,	GetBatValueCmdCB},							/*设备电池电量值*/
	{0x09,	VolumeValueCB},								/*设备音量*/
	{0x0A,	MotoOrderValueCB},							/*设备震动模式*/				
	{0x0B,	SetLanguageFormat},						/*设备语言*/
	{0x0C,	SetUnitFormat},								/*设备单位格式*/
	{0x0D,	FactorySettingCmdCB},						/*恢复出厂设置*/	
	{0x0E,	UpdateProgMsgCmdCB},						/*升级模式选择*/
	{0x10,	MotoVibrationIntensityCB},					/*设备震动强度*/	        
	{0x14,	MedalCB},									/*设备勋章显示*/
	
	{0x30, 	PersonInitCmdCB},							/*个人信息*/
	{0x31,	UserHabitCmdCB},							/*用户使用习惯设置*/

	{0x50,	DailyGoalSettingCmdCB},						/*目标设置*/
	{0x51,	GetSportModeCB},							/*设备运动/睡眠模式*/
	{0x52, 	GetSportTotalCntCB},							/*设备运动/睡眠数据总数*/
	{0x53,	ClearSportDataCB},							/*运动数据删除命令*/
	{0x54, 	UploadSportCmdCB},							/*上传运动数据*/	
	{0x55,	ClearSleepDataCB},							/*睡眠数据删除命令*/
	{0x56,	GetSleepAwakeDataCB},						/*上传睡眠数据*/
	{0x57,	GetDisplaySportDataCmdCB},					/*设备端显示的运动/睡眠值*/
	{0x58,	SetSleepModeCB},							/*自动睡眠监测*/	

	{0x59, 	GetHeartRateTotalCntCB},					/*设备心率数据总数*/
	{0x5A,	ClearHeartRateDataCB},						/*心率数据删除命令*/
	{0x5B,	GetHeartRateDataCB},						/*上传心率数据*/
	{0x5C,	AutoHeartRateCB},							/*自动开启心率时间间隔*/
	{0x5D,	HeartRateAlarmCB},							/*心率报警门限*/
	{0x5E,	SedentarinessRemindCB},					/*静坐提醒*/
	{0x60,	BaseKcalTypeCB},							/*基础卡路里*/

	{0x66, 	GetRecordWorkTotalCntCB},					/*设备计时运动记录总数*/
	{0x67, 	UploadRecordWorkCmdCB},					/*上传设备计时运动记录*/
	{0x68,	ClearRecordWorkDataCB},					/*删除设备计时运动记录*/

	{0x70,	AndroidCallCB},								/*电话姓名推送*/
	{0x71,	AndroidSmsCallCB},							/*短信息推送*/
	{0x72,	NoticeNumberCB},							/*消息条数推送*/
	{0x73,	AndroidSocialCB},							/*社交信息推送*/
	{0x74,	AndroideEmailCB},							/*邮件推送*/
	{0x75,	AndroideScheduleCB},						/*日程推送*/
        
	{0x90,	UpdateDeviceSwitchSettingCB},				/*开关设置*/
	{0x91,	ReminderNumberCmdCB},					/*提醒条数*/
	{0x92,	ReminderCmdCB},							/*提醒设置*/
	{0x93,	BindingBeginCmdCB},							/*绑定开始*/
	{0x94,	BindingEndCmdCB},							/*绑定结束*/

	{0xB0,	ReadPayAID},								/*支付：当前使用的芯片应用卡号*/
	{0xB1,	ReadPayBalance},							/*支付：读取余额*/
	{0xB2,	ReadPayRecords},							/*支付：读取交易记录*/
	{0xB3,	PayPassthroughFromApp},					/*支付：透传*/
	
	{0xD0,	MusicSettingsCmdCB},						/*音乐查询/设置*/
	{0xD1,	CameraControlCmdCB},						/*相机控制*/
	{0xD2,	SearchForMobilePhonesCmdCB},				/*寻找手机*/	
	{0xD3,	DeviceCommIDCmdCB},						/*设备间通信用户ID*/	
	{0xD5,	PhoneBookCB},								/*通讯录*/
	
	{0xF0,	SpiFlashEraseSaveFlag},						/*擦除外部Flash的Flag保存区域*/
	{0xF1,	SpiFlashUpdateFontLib},						/*升级字库*/
	{0xF2,	LEDTest},									/*测试LED*/	
	{0xF3,	NFCTest},									/*测试NFC*/ 
	{0xF4,	Get_CSN},									/*获取CSN*/ 
	{0xF5,	TestFactoryMode},							/*恢复出厂设置*/
	{0xF6, 	GetBT4AddrTest},							/*获取蓝牙4.0地址*/
	{0xF7,	KeypadTest},								/*测试按键*/
	{0xF8,	GetDateTimeTest},							/*获取当前设备时间*/
	{0xF9,	PowOFFSystemTest},							/*重启整个系统*/
	{0xFA,	MotorTest},									/*测试振子*/
	{0xFB,	TpTest},
	{0xFC,	SensorTest},								/*测试G-Sensor*/	
	{0xFE,	LCDTest},									/*测试LCD*/
	{0xFF,	WatchIDTest}								/*写WatchID*/
};
const uint16 DataCmdTableLen=sizeof(DataCmdTable)/sizeof(DATA_CMD_T);


/******************************************************************************************
*函数名:			DataParseHandler
*参数:			InData：传入需要解析的数据指针
*返回值:			返回码
*描述:			解析传入的数据是否有效，有效则返回相关：数据命令，数据长度，数据
*编写:			chenhao
*版本信息:		V1.0
******************************************************************************************/
uint8 DataParseHandler(const void *InData)
{
	uint8* Data = (uint8*)InData;
       uint8 DataParseCmd=0,DataParseAction=0,DataOffset=0;
	uint16 i=0;   
	uint16 DataParseLen=0;  
	CurrentDataParseCmd =0;
	CurrentDataParseAction=0;
	CurrentDataParseLen =0;
	/*协议头尾长度*/
	for(i=0;i<DATA_REV_MAX;i++)
	{
		if(*(Data+DataOffset)==DATA_START_CMD)
		{
			if((DataOffset+4)>=DATA_REV_MAX)
				break;
			DataParseLen = (*(Data+DataOffset+4))<<8 | (*(Data+DataOffset+3));
			if((DataOffset+DataParseLen+5)>=DATA_REV_MAX)
				break;						
			if(*(Data+DataOffset+DataParseLen+5) ==DATA_STOP_CMD)
			{
				DataParseCmd = (*(Data+DataOffset+1));
				DataParseAction =(*(Data+DataOffset+2));
			}
			break;
		}
		DataOffset++;
	}
	if(DataParseCmd==0)
	{
		CurrentDataParseCmd = 0x00; 
		return E_RSP_PARSE_ERROR;
	}

       /*是否存在解析函数,并处理*/
	for(i=0;i<DataCmdTableLen;i++)
	{
		if(DataParseCmd == DataCmdTable[i].DataCmd)
		{
			CurrentDataParseCmd =DataParseCmd;
			CurrentDataParseAction = DataParseAction;
			CurrentDataParseLen = DataParseLen;
			return DataCmdTable[i].pFn((Data+DataOffset+5));
		}
	}
	return E_RSP_PARSE_ERROR;	
}


static void BasicSendData(uint8 *pData,uint16 DataLen)
{
	uint8 TempSendData[64]={0};
	uint8 Aciton=DATA_SET_RSP;

	if(CurrentDataParseAction == DATA_GET_CMD)
		Aciton = DATA_GET_RSP;
	
	TempSendData[0]=DATA_START_CMD;
	TempSendData[1]=CurrentDataParseCmd;
	TempSendData[2]=Aciton;
	TempSendData[3]=DataLen&0xFF;
	TempSendData[4]=(DataLen>>8)&0xFF;	
	memcpy(&TempSendData[5],pData,DataLen);
	TempSendData[5+DataLen]=DATA_STOP_CMD;
	HwSendDataToBt(TempSendData,6+DataLen);
	}

void SendRspCmd(uint8 Cmd,RSP_VAULE_E uData)
{
	uint8 CmdDataArry[2];
	CmdDataArry[0]=Cmd;
	CmdDataArry[1]=uData;  
	CurrentDataParseCmd = RSP_CMD;
	BasicSendData(CmdDataArry,2);
}
/******************************************************************************************
收到手机回应
******************************************************************************************/
uint8 GetCmdRespond(uint8 *pData)
{
	if(*pData == 0xD1)		/*拍照回应*/
		SendMsg(MSG_CAMERA_NOTIC);
	else if(*pData == 0xD2)	/*相机打开回应*/
	{
		Camera_state = 1;
		SendMsg(MSG_OPEN_CAMERA_NOTIC);
	}
	else if(*pData == 0xD3)	/*相机未打开回应*/
	{
		Camera_state = 0; 
		SendMsg(MSG_OPEN_CAMERA_NOTIC);
	}
	else if(*pData == 0xD4)	/*短信回复回应*/
		SendMsg(MSG_ANCS_REPLAY_SUCCESS);		
	return E_RSP_NULL;
}

/******************************************************************************************
设备WatchID
******************************************************************************************/
uint8 GetWatchIdCmdCB(uint8 *pData)
{
	if(CurrentDataParseAction ==DATA_GET_CMD)
	{	
		uint8 ReadWatchIdTemp[WATCH_ID_LEN]={0x00};
		ReadWatchId(ReadWatchIdTemp);
		BasicSendData(ReadWatchIdTemp,WATCH_ID_LEN);
		return E_RSP_NULL;
	}
	return E_RSP_FAIL;
}

/******************************************************************************************
设备版本信息
******************************************************************************************/
uint8 GetVersionMsgCmdCB(uint8 *pData)
{
	if(CurrentDataParseAction ==DATA_GET_CMD)
	{
		switch (*pData)
		{
			case 0x00:					/*设备类型*/
			{
				uint8 Temp[5]={0};
				Temp[0]=*pData;
				memcpy(Temp+1,DeviceType,4);	
				BasicSendData(Temp,5);
				return E_RSP_NULL;
			}
			
			case 0x01:					/*软件版本*/
			{
				uint8 Temp[21]={0};
				uint8 TpVer = (reg_data_version&0xFF);
				
				Temp[0]=*pData;
				memcpy(Temp+1,SoftWareVersion,20);
				Temp[6]=0x30+KL17_VER[0];
				Temp[8]=0x30+KL17_VER[1];
				//触摸版本
				Temp[14]=0x30+(TpVer>>4);
				Temp[16]=0x30+(TpVer&0x0F);
				//心率版本
				Temp[18]=0x30+HR_VER[0];
				Temp[20]=0x30+HR_VER[1];	
				BasicSendData(Temp,21);		
				return E_RSP_NULL;				
			}	
			
			case 0x02:					/*硬件版本*/
			{
				uint8 Temp[15]={0};
				Temp[0]=*pData;
				memcpy(Temp+1,HardWareVersion,14);
				BasicSendData(Temp,15);
				return E_RSP_NULL;
			}

			case 0x03:					/*通讯协议版本*/
			{
				uint8 Temp[4]={0};
				Temp[0]=*pData;
				memcpy(Temp+1,ProtocolVersion,3);	
				BasicSendData(Temp,4);
				return E_RSP_NULL;
			}	
			
			case 0x04:					/*功能版本*/
			{
				uint8 Temp[36]={0};
				Temp[0]=*pData;
				memcpy(Temp+1,FunctionVersion,35);	
				BasicSendData(Temp,36);
				return E_RSP_NULL;
			}

			default:
				break;
		}
	}
	return E_RSP_FAIL;
}

/******************************************************************************************
日期时间
******************************************************************************************/
uint8 DateTimeSettingCmdCB(uint8 *pData)
{
	BAND_TIME_MSG_DATA_T uData;	
	uint32 TimeTemp =0;
	if(CurrentDataParseAction ==DATA_SET_CMD)
	{
		TimeTemp = GetTimeStamp();
		memcpy((BAND_TIME_MSG_DATA_T*)&uData,pData,sizeof(BAND_TIME_MSG_DATA_T));
		if((uData.Year<=VALID_YEAR_MAX)&&(uData.Year>=VALID_YEAR_MIN)
		&&(uData.Month>0)&&(uData.Month<=MAX_MONTH)
		&&(uData.Hour<=MAX_HOUR_IN_24H)&&(uData.Minute<=MAX_MINUTE)&&(uData.Second<=MAX_SECOND))
		{
			UTCTimeStruct UtcTime;	
			if((uData.Day<=GetMonthDays(uData.Year,uData.Month))&&(uData.Day>0))
				UtcTime.day=uData.Day;
			else
				return E_RSP_FAIL;  

			UtcTime.year=uData.Year;
			UtcTime.month=uData.Month;

			UtcTime.hour=uData.Hour;
			UtcTime.minutes=uData.Minute;
			UtcTime.seconds=uData.Second;

			gCurrentUtcTime.year=uData.Year;
			gCurrentUtcTime.month=uData.Month;
			gCurrentUtcTime.day=uData.Day;
			gCurrentUtcTime.hour=uData.Hour;
			gCurrentUtcTime.minutes=uData.Minute;
			gCurrentUtcTime.seconds=uData.Second;

			gOS_SetUTCTime(&UtcTime);

			gDateTimeValid = 1;
			SendMsg(MSG_DATE_TIME_UPDATE);

			gOS_GetUTCTime(&UtcTime);
			if(GetNoSameDay(UtcTime))
			{
				ClearSportData();
				ClearSleepDataWhenDateTimeSetting(TimeTemp);
			}
			return E_RSP_OK;  
		}
	}
	else if(CurrentDataParseAction ==DATA_GET_CMD)
	{
		UTCTimeStruct uTime;
		gOS_GetUTCTime(&uTime);
		uData.Year = uTime.year;
		uData.Month= uTime.month;
		uData.Day= uTime.day;
		uData.Hour= uTime.hour;
		uData.Minute= uTime.minutes;
		uData.Second= uTime.seconds;
    		BasicSendData((uint8*)&uData,7);	
		return E_RSP_NULL;
	}
	return E_RSP_FAIL;  
}
/******************************************************************************************
时间界面设置
******************************************************************************************/
uint8 TimeOrderCmdCB(uint8 *pData)
{
	if(CurrentDataParseAction ==DATA_SET_CMD)
	{
		memcpy((uint8*)&TimeOrder,pData,sizeof(TIME_ORDER));
		TimerMenuFlag = TimeOrder.BackgroundStyle+1;
		//WRITE_USER_PARAMETER((uint8 *)&TimerMenuFlag, TIMER_MENU_LOC, TIMER_MENU_LEN);
		WRITE_USER_PARAMETER((uint8 *)&TimeOrder,TIME_ORDER_LOC, TIME_ORDER_LEN);
		SendMsg(MSG_DATE_TIME_FORMAT_UPDATE);
		return E_RSP_OK; 
	}
	else if(CurrentDataParseAction ==DATA_GET_CMD)
	{
		//TimeOrder.BackgroundStyle = TimerMenuFlag -1;
		BasicSendData((uint8*)&TimeOrder,sizeof(TIME_ORDER));	
		return E_RSP_NULL;
	}
	return E_RSP_FAIL;  
}
/******************************************************************************************
一级界面显示设置
******************************************************************************************/
uint8 MenuOrderCmdCB(uint8 *pData)
{
	if(CurrentDataParseAction ==DATA_SET_CMD)
	{
		uint8*Mpos=pData;
		while(*Mpos!=0x8F)
		{
			if(*Mpos==0x00||*Mpos>sizeof(MenuOrder))
				return E_RSP_FAIL;
			Mpos++;
		};
		memset(MenuOrder,0,sizeof(MenuOrder));
		if(CurrentDataParseLen<=7)
			memcpy(MenuOrder,pData,CurrentDataParseLen);
		WRITE_USER_PARAMETER(MenuOrder,MENU_ORDER_LOC,MENU_ORDER_LEN);
		//CurrentMenuIdx=0;
		SendMsg(MSG_SSD13XX_EXIT_SLEEP);
		if(CheckpCurrentHandler(CurrentMenuIParam))
			LoadMenuHandler(MENU_STAY);
		return E_RSP_OK; 
	}
	else if(CurrentDataParseAction ==DATA_GET_CMD)
	{	
		uint8 len=strlen((char*)MenuOrder);
    		BasicSendData(MenuOrder,len);	
		return E_RSP_NULL;
	}
	return E_RSP_FAIL;  
}

/******************************************************************************************
设备屏幕亮度
******************************************************************************************/
uint8 BacklightSettingCmdCB(uint8 *pData)
{
	if(CurrentDataParseAction ==DATA_SET_CMD)
	{
		if (SetValue.Backlight!= *pData)	
		{
			SetValue.Backlight= *pData;		
			WRITE_USER_PARAMETER(pData,SET_VALUE_LOC,1);
			SendMsg(MSG_SSD13XX_EXIT_SLEEP);						
		}	
		return E_RSP_OK; 
	}
	else if(CurrentDataParseAction ==DATA_GET_CMD)
	{
    		BasicSendData(&(SetValue.Backlight),1);	
		return E_RSP_NULL;
	}
	return E_RSP_FAIL;  	
}

/******************************************************************************************
设备电池电量值
******************************************************************************************/
uint8 GetBatValueCmdCB(uint8 *pData)
{        
	if(CurrentDataParseAction ==DATA_GET_CMD)
	{	
		BasicSendData(&BatteryTempPercent,1);	
		return E_RSP_NULL;
	}
	return E_RSP_FAIL;
}


/******************************************************************************************
设备音量
******************************************************************************************/
uint8 VolumeValueCB(uint8 *pData)
{
	if(CurrentDataParseAction ==DATA_SET_CMD)
	{
		SetValue.Volume= *pData;
		WRITE_USER_PARAMETER(pData,SET_VALUE_LOC+1,1);
		SendMsg(MSG_VOL_SET);
		return E_RSP_OK; 
	}
	else if(CurrentDataParseAction ==DATA_GET_CMD)
	{
    		BasicSendData(&(SetValue.Volume),1);	
		return E_RSP_NULL;
	}
	return E_RSP_FAIL;  
}


/******************************************************************************************
设备震动模式
******************************************************************************************/
uint8 MotoOrderValueCB(uint8 *pData)
{
	if(CurrentDataParseAction ==DATA_SET_CMD)
	{
		uint8 Offset = *pData;
		if(Offset<10)
		{
			MotoOrder[Offset]= *(pData+1);
			WRITE_USER_PARAMETER(MotoOrder,MOTO_ORDER_LOC,MOTO_ORDER_LEN);
			return E_RSP_OK; 
		}
	}
	else if(CurrentDataParseAction ==DATA_GET_CMD)
	{
    		BasicSendData(MotoOrder,10);	
		return E_RSP_NULL;
	}
	return E_RSP_FAIL;  
}

/******************************************************************************************
设备语言
******************************************************************************************/
uint8 SetLanguageFormat(uint8 *pData)
{
	if(CurrentDataParseAction ==DATA_SET_CMD)
	{
		LastLanguage = SetValue.Language;
		SetValue.Language= *pData;
		Force_NordicFlash();
		WRITE_USER_PARAMETER(&LastLanguage,LAST_LANGUAGE_LOC,1);
		WRITE_USER_PARAMETER(pData,SET_VALUE_LOC+2,1);
		Free_NordicFlash();
		return E_RSP_OK; 
	}
	else if(CurrentDataParseAction ==DATA_GET_CMD)
	{
		uint8 Lf[4]={0x00};
		if(SetValue.Language==LAN_CHINESE_SIMPLIFID){
			SetValue.Language=OTALanguagetype;
		}
		Lf[0]= SetValue.Language;
		Lf[1]= OTALanguagetype;
		Lf[2]= OTALanguageVersion[0];
		Lf[3]= OTALanguageVersion[1];
    		BasicSendData(Lf,4);	
		return E_RSP_NULL;
	}
	return E_RSP_FAIL;  
}

/******************************************************************************************
设备单位格式
******************************************************************************************/
uint8 SetUnitFormat(uint8 *pData)
{
	if(CurrentDataParseAction ==DATA_SET_CMD)
	{
		uint8 OldDF =SetValue.DistanceFormat; 
		SetValue.DistanceFormat= *pData;
		WRITE_USER_PARAMETER(pData,SET_VALUE_LOC+3,1);
              if((OldDF==0 && SetValue.DistanceFormat==1) ||(OldDF==1 && SetValue.DistanceFormat==0)) /*公里->英里 或 英里->公里*/
			SetGoalOverRemind(DailyGoal,true);
		SendMsg(MSG_UNIT_CHANGED);
		return E_RSP_OK; 
	}
	else if(CurrentDataParseAction ==DATA_GET_CMD)
	{
    		BasicSendData(&(SetValue.DistanceFormat),1);	
		return E_RSP_NULL;
	}
	return E_RSP_FAIL;  
}

/******************************************************************************************
恢复出厂设置
******************************************************************************************/
uint8 FactorySettingCmdCB(uint8 *pData)
{
	if(CurrentDataParseAction ==DATA_SET_CMD)
	{
		SetFactorySettingEvent();
		SendMsg(MSG_SYSTEM_FACTORY_SET_STATE);
		return E_RSP_OK; 
	}
	return E_RSP_FAIL;
}

/******************************************************************************************
通讯录电话姓名推送
******************************************************************************************/

uint8 PhoneBookCB(uint8 *pData)
{
	
	if(CurrentDataParseAction ==DATA_SET_CMD)
	{
		uint8 CallAttr = *pData;
		switch(CallAttr)
		{
			case 0x00:
			case 0xA0:
				{
					Force_NordicFlash();	
					ClearPhoneBookFlash();
					InitPhoneBookBuf();
					SavePhoneBookToBuf(pData,true);
					Free_NordicFlash();
					return E_RSP_OK; 
				}
			case 0x01:
			case 0xA1:
				{
					Force_NordicFlash();	
					SavePhoneBookToBuf(pData,true);
					Free_NordicFlash();
					return E_RSP_OK; 
				}
			case 0x02:
			case 0xA2:
				{
					Force_NordicFlash();	
					SavePhoneBookToBuf(pData,true);
					SetPhBok_Flag(true);
					Free_NordicFlash();
					return E_RSP_OK;  
				}
			case 0x03:
			case 0xA3:
				{
					Force_NordicFlash();	
					ClearPhoneBookFlash();
					InitPhoneBookBuf();
					SavePhoneBookToBuf(pData,true);
					SetPhBok_Flag(true);
					Free_NordicFlash();
					return E_RSP_OK; 
				}
			default:
				break;
		} 
	}
	return E_RSP_FAIL; 
}


/******************************************************************************************
升级程序信息
******************************************************************************************/
void OtaProgMsg(void)
{
	Free_NordicFlash();
	ShowOtaLogo();	
	Force_NordicFlash();  
	if(SetCRCFlag())//设置升级块标志
	{
		osal_set_event(GetHwTaskId(),HW_SYSTEM_UPDATE_RESET_EVENT);
	} 	
}
uint8 UpdateProgMsgCmdCB(uint8 *pData)
{
	if(CurrentDataParseAction ==DATA_SET_CMD)
	{
	        uint8 TempData[UPDATE_STORE_LEN]={0x00};
	        pS_OsWatchDogFeed(); /*喂狗*/	 
		 Force_NordicFlash();
        	 if(FORCE_NORDIC_FLASH_FLAG)	
        	 {
		        StoreSportDataToFlash(0);/*  升级前存储数据*/
		        SetUserInfo();
			/*OTA运动数据存储*/
			TempData[0]=0x01;
			TempData[1]=(gSportDisplay.SportSteps)&0xff;
			TempData[2]=((gSportDisplay.SportSteps)>>8)&0xff;
			TempData[3]=((gSportDisplay.SportSteps)>>16)&0xff;
			TempData[4]=((gSportDisplay.SportSteps)>>24)&0xff;

			TempData[5]=(gSportDisplay.SportKcal)&0xff;
			TempData[6]=((gSportDisplay.SportKcal)>>8)&0xff;
			TempData[7]=((gSportDisplay.SportKcal)>>16)&0xff;
			TempData[8]=((gSportDisplay.SportKcal)>>24)&0xff;

			TempData[9]=(gSportDisplay.SportDistance)&0xff;
			TempData[10]=((gSportDisplay.SportDistance)>>8)&0xff;
			TempData[11]=((gSportDisplay.SportDistance)>>16)&0xff;
			TempData[12]=((gSportDisplay.SportDistance)>>24)&0xff;		
			
			TempData[13]=(gSportDisplay.SportDuration)&0xff;
			TempData[14]=((gSportDisplay.SportDuration)>>8)&0xff;
			TempData[15]=((gSportDisplay.SportDuration)>>16)&0xff;
			TempData[16]=((gSportDisplay.SportDuration)>>24)&0xff;
			WRITE_USER_PARAMETER(TempData,UPDATE_STORE_LOC,UPDATE_STORE_LEN);
			Ota_Prepare((pData+1),(CurrentDataParseLen-1));
			SendMsg(MSG_OTA);
			return E_RSP_OK;
        	}
	}
	else if(CurrentDataParseAction ==DATA_GET_CMD)
	{
		uint8 OtaResult[2] ={0x01,0x01};
		if(UpdateKl17Success)
			OtaResult[0] =0x00;
		if(UpdateTPSuccess)
			OtaResult[1] =0x00;
		BasicSendData(OtaResult,2);
		return E_RSP_NULL;
	}
	return E_RSP_FAIL;
}

/******************************************************************************************
设备震动强度
******************************************************************************************/
uint8 MotoVibrationIntensityCB(uint8 *pData)
{
	uint8 MotoLevel = Get_Motor_Level();
	if(CurrentDataParseAction ==DATA_SET_CMD)
	{
		if (MotoLevel != *pData)
		{
			MotoLevel = *pData;
			if(MotoLevel>100)
				MotoLevel=100;
			Save_Motor_Level(MotoLevel);
		}
		if(MotoLevel)
			MotoRun(MOTO_DEFAULT);
		return E_RSP_OK; 
	}
	else if(CurrentDataParseAction ==DATA_GET_CMD)
	{
		BasicSendData(&(MotoLevel),1);	
		return E_RSP_NULL;
	}
	return E_RSP_FAIL;	
}

uint8 MedalCB(uint8 *pData)
{
	if(CurrentDataParseAction ==DATA_SET_CMD)
	{
		Medal_Type = *pData;
		SendMsg(MSG_MEDAL);
		return E_RSP_OK; 
	}
	return E_RSP_FAIL;	
}

/******************************************************************************************
初始化个人信息
******************************************************************************************/
uint8 PersonInitCmdCB(uint8 *pData)
{
	if(CurrentDataParseAction ==DATA_SET_CMD)
	{
		UserInfoData.UserSex=pData[0];     //男/女？
		UserInfoData.UserAge=pData[1];      //岁数
		UserInfoData.UserHeight=pData[2];   //身高 1cm
		UserInfoData.UserWeight=((((uint16)(pData[4]))<<8)|pData[3])*10; //体重 10g
		SetUserInfo();
		return E_RSP_OK; 
	}
	else if(CurrentDataParseAction ==DATA_GET_CMD)
	{
		uint8 uData[5]={0x00};
		uData[0] = UserInfoData.UserSex;
		uData[1] = UserInfoData.UserAge;
		uData[2] = UserInfoData.UserHeight;
		uData[3] = (UserInfoData.UserWeight/10) & 0xff;
		uData[4] = ((UserInfoData.UserWeight/10) >>8)&0xff;
		BasicSendData(uData,5);
		return E_RSP_NULL;
	}
	return E_RSP_FAIL; 
}

/******************************************************************************************
用户使用习惯设置
******************************************************************************************/
uint8 UserHabitCmdCB(uint8 *pData)
{
	if(CurrentDataParseAction ==DATA_SET_CMD)
	{
		SetValue.UserHabit= *pData;
		WRITE_USER_PARAMETER(pData,SET_VALUE_LOC+4,1);
		return E_RSP_OK; 
	}
	else if(CurrentDataParseAction ==DATA_GET_CMD)
	{
		BasicSendData(&(SetValue.UserHabit),1);
		return E_RSP_NULL;
	}
	return E_RSP_FAIL; 
}

/******************************************************************************************
目标设置
******************************************************************************************/
uint8 DailyGoalSettingCmdCB(uint8 *pData)
{
	if(CurrentDataParseAction ==DATA_SET_CMD)
	{
		uint8 GoalId= *pData;
		uint8 GoalTemp[DAILY_GOAL_LEN]={0x00};
		DailyGoal[GoalId].Goal = (*(pData+2))<<8 | (*(pData+1));
		DailyGoal[GoalId].MainGoalFlag = *(pData+3);
	
		GoalTemp[0]=DailyGoal[0].Goal & 0xFF;
		GoalTemp[1]=(DailyGoal[0].Goal>>8) & 0xFF;
		GoalTemp[2]=DailyGoal[0].MainGoalFlag;
		
		GoalTemp[3]=DailyGoal[1].Goal & 0xFF;
		GoalTemp[4]=(DailyGoal[1].Goal>>8) & 0xFF;
		GoalTemp[5]=DailyGoal[1].MainGoalFlag;
		
		GoalTemp[6]=DailyGoal[2].Goal & 0xFF;
		GoalTemp[7]=(DailyGoal[2].Goal>>8) & 0xFF;
		GoalTemp[8]=DailyGoal[2].MainGoalFlag;
		
		GoalTemp[9]=DailyGoal[3].Goal & 0xFF;
		GoalTemp[10]=(DailyGoal[3].Goal>>8) & 0xFF;
		GoalTemp[11]=DailyGoal[3].MainGoalFlag;	
		
		GoalTemp[12]=DailyGoal[4].Goal & 0xFF;
		GoalTemp[13]=(DailyGoal[4].Goal>>8) & 0xFF;
		GoalTemp[14]=DailyGoal[4].MainGoalFlag;	
		
		WRITE_USER_PARAMETER(GoalTemp,DAILY_GOAL_LOC,DAILY_GOAL_LEN);
		SetGoalOverRemind(DailyGoal,false);
		SendMsg(MSG_GOAL_CHANGED);
		return E_RSP_OK; 
	}
	else if(CurrentDataParseAction ==DATA_GET_CMD)
	{
		uint8 Temp[DAILY_GOAL_LEN]={0x00};
		for(uint8 i =0 ;i<5;i++)
		{
			Temp[i*3]= DailyGoal[i].Goal & 0xFF;
			Temp[i*3+1]= (DailyGoal[i].Goal>>8)&0xFF;
			Temp[i*3+2]= DailyGoal[i].MainGoalFlag;
		}
		BasicSendData(Temp,DAILY_GOAL_LEN);
		return E_RSP_NULL;
	}
	return E_RSP_FAIL; 
}


/******************************************************************************************
设备运动/睡眠模式
******************************************************************************************/
uint8 GetSportModeCB(uint8 *pData)
{         
	if(CurrentDataParseAction ==DATA_GET_CMD)
	{
		uint8 Temp=0x00;
		if(SLEEP_MODE_FLAG)
			Temp =0x01;
		BasicSendData(&Temp,1);
		return E_RSP_NULL;
	}
	return E_RSP_FAIL; 
}




/******************************************************************************************
设备运动/睡眠数据总数
******************************************************************************************/
uint8 GetSportTotalCntCB(uint8 *pData)
{
	if(CurrentDataParseAction ==DATA_GET_CMD)
	{
		uint8 Temp[4]={0x00};
		StoreSportDataToFlash(0);
		if(Step_Store_Logs>0)
		{
			StopStep();    
			osal_start_timerEx(GetAppTaskId(), MSG_OPEN_STEP, 10000);
			gUploadSportDataState = UPLOAD_PREPARE;
		}	
		Temp[0] = Step_Store_Logs&0xFF;
		Temp[1] = (Step_Store_Logs>>8)&0xFF;
		if(SLEEP_DATA_SAVE)
		{
			Temp[2] = 0x00;
			Temp[3] = 0x00;				
		}
		else
		{
			Temp[2] = Sleep_Store_Logs&0xFF;
			Temp[3] = (Sleep_Store_Logs>>8)&0xFF;		
		}	
		BasicSendData(Temp,4);
		return E_RSP_NULL;
	}
	return E_RSP_FAIL; 
}

/******************************************************************************************
运动数据删除命令
******************************************************************************************/
uint8 ClearSportDataCB(uint8 *pData)
{
	if(CurrentDataParseAction ==DATA_SET_CMD)
	{
		Step_Store_Logs=0;
		WRITE_STEP_SLEEP_SUM(&Step_Store_Logs,SPORT_TOTAL_LOC,SPORT_TOTAL_LEN);
		SetStepMemoryFullFlag(false);
		return E_RSP_OK; 
	}
	return E_RSP_FAIL;
}

/******************************************************************************************
上传运动数据
******************************************************************************************/
void SendSportDataCmd(SPORT_DATA_T uData)
{
	uint8 Sdata[22]={0x00};
	Sdata[0]=(uData.SportIdx)&0xff;
	Sdata[1]=((uData.SportIdx)>>8)&0xff;
	
	Sdata[2]=(uData.SportTimeStamp)&0xff;
	Sdata[3]=((uData.SportTimeStamp)>>8)&0xff;
	Sdata[4]=((uData.SportTimeStamp)>>16)&0xff;
	Sdata[5]=((uData.SportTimeStamp)>>24)&0xff;
	
	Sdata[6]=(uData.SportSteps)&0xff;
	Sdata[7]=((uData.SportSteps)>>8)&0xff;
	Sdata[8]=((uData.SportSteps)>>16)&0xff;
	Sdata[9]=((uData.SportSteps)>>24)&0xff;	
	
	Sdata[10]=(uData.SportKcal)&0xff;
	Sdata[11]=((uData.SportKcal)>>8)&0xff;
	Sdata[12]=((uData.SportKcal)>>16)&0xff;
	Sdata[13]=((uData.SportKcal)>>24)&0xff;	
	
	Sdata[14]=(uData.SportDistance)&0xff;
	Sdata[15]=((uData.SportDistance)>>8)&0xff;
	Sdata[16]=((uData.SportDistance)>>16)&0xff;
	Sdata[17]=((uData.SportDistance)>>24)&0xff;

	Sdata[18]=(uData.SportDuration)&0xff;
	Sdata[19]=((uData.SportDuration)>>8)&0xff;
	Sdata[20]=((uData.SportDuration)>>16)&0xff;
	Sdata[21]=((uData.SportDuration)>>24)&0xff;		
	BasicSendData(Sdata,22);
}

uint8 UploadSportCmdCB(uint8 *pData)
{
	if(CurrentDataParseAction ==DATA_GET_CMD)
	{
		if(*pData==0)
		{
			gUploadSportDataState=UPLOADING;
			Step_UpLoad_Logs = 0;
		}		
		if(StepUploadData(*pData))
			return E_RSP_NULL;
	}
	return E_RSP_FAIL; 
}


/******************************************************************************************
睡眠数据删除命令
******************************************************************************************/
uint8 ClearSleepDataCB(uint8 *pData)
{
	if(CurrentDataParseAction ==DATA_SET_CMD)
	{
		Sleep_Store_Logs=0;
		WRITE_STEP_SLEEP_SUM(&Sleep_Store_Logs,SLEEP_TOTAL_LOC,SLEEP_TOTAL_LEN);
		SetSleepMemoryFullFlag(false);
		return E_RSP_OK; 
	}
	return E_RSP_FAIL;
}


/******************************************************************************************
上传睡眠数据
******************************************************************************************/
void SendSleepDataCmd(SLEEP_DATA_T uData)
{
	uint8 Sdata[7]={0x00};
	Sdata[0]=(uData.SleepIdx)&0xff;
	Sdata[1]=((uData.SleepIdx)>>8)&0xff;
	Sdata[2]=(uData.SleepTimeStamp)&0xff;
	Sdata[3]=((uData.SleepTimeStamp)>>8)&0xff;
	Sdata[4]=((uData.SleepTimeStamp)>>16)&0xff;
	Sdata[5]=((uData.SleepTimeStamp)>>24)&0xff;
	Sdata[6]=uData.SleepType;
	BasicSendData(Sdata,7);
}


uint8 GetSleepAwakeDataCB(uint8 *pData)
{
	if(CurrentDataParseAction ==DATA_GET_CMD)
	{
		if(*pData==0)
		{
			gSleepDataUploadState=UPLOADING;
			Sleep_UpLoad_Logs = 0;
		}		
		if(SleepUploadData(*pData))
			return E_RSP_NULL;
	}
	return E_RSP_FAIL; 
}


/******************************************************************************************
设备心率数据总数
******************************************************************************************/
uint8 GetHeartRateTotalCntCB(uint8 *pData)
{
	if(CurrentDataParseAction ==DATA_GET_CMD)
	{
		uint8 Temp[2]={0x00};
		//uint32 HeartRateLogCnt = 0 ;		
		//READ_STEP_SLEEP_SUM(&HeartRateLogCnt, HEARTRATE_RECORDS_TOTAL_LOC,HEARTRATE_RECORDS_TOTAL_LEN);
		Temp[0] = HeartRate_Store_Logs&0xFF;
		Temp[1] = (HeartRate_Store_Logs>>8)&0xFF;
		BasicSendData(Temp,2);
		return E_RSP_NULL;
	}
	return E_RSP_FAIL; 
}

/******************************************************************************************
心率数据删除命令
******************************************************************************************/
uint8 ClearHeartRateDataCB(uint8 *pData)
{
	if(CurrentDataParseAction ==DATA_SET_CMD)
	{
		HeartRate_Store_Logs=0;
		SetHeartRateMemoryFullFlag(false);
		WRITE_STEP_SLEEP_SUM(&HeartRate_Store_Logs,HEARTRATE_RECORDS_TOTAL_LOC,HEARTRATE_RECORDS_TOTAL_LEN);
		return E_RSP_OK; 
	}
	return E_RSP_FAIL;
}
/******************************************************************************************
上传心率数据
******************************************************************************************/
void SendHeartRateDataCmd(HEARTRATE_DATA_T uData)
{
	uint8 Sdata[7]={0x00};
	Sdata[0]=(uData.HeartRateIdx)&0xff;
	Sdata[1]=((uData.HeartRateIdx)>>8)&0xff;
	Sdata[2]=(uData.HeartRateTimeStamp)&0xff;
	Sdata[3]=((uData.HeartRateTimeStamp)>>8)&0xff;
	Sdata[4]=((uData.HeartRateTimeStamp)>>16)&0xff;
	Sdata[5]=((uData.HeartRateTimeStamp)>>24)&0xff;
	Sdata[6]=uData.HeartRateVal;
	BasicSendData(Sdata,7);
}

uint8 GetHeartRateDataCB(uint8 *pData)
{
	if(CurrentDataParseAction ==DATA_GET_CMD)
	{
		if(*pData==0)
		{
			gHeartRateDataUploadState=UPLOADING;
			HeartRate_UpLoad_Logs = 0;
		}		
		if(HeartRateUploadData(*pData))
			return E_RSP_NULL;
	}
	return E_RSP_FAIL; 
}




uint8 AutoHeartRateCB(uint8 *pData)
{
	if(CurrentDataParseAction ==DATA_GET_CMD)
	{
		uint8 Temp[2]={0x00};
		Temp[0] = AutoHeartRateTime;
		BasicSendData(Temp,1);
		return E_RSP_NULL;
	}
	else if(CurrentDataParseAction==DATA_SET_CMD)
	{
		uint8 time = pData[0];
		if((time==0)||(time==5)||(time==10)||(time==15)||(time==30)||(time==45)||(time==60))
		{
			AutoHeartRateTime = time;
			WRITE_USER_PARAMETER(&AutoHeartRateTime,AUTO_HEARTRATE_TIME_LOC,AUTO_HEARTRATE_TIME_LEN);
			if(time==0)
			{
				osal_stop_timerEx(GetHrTaskId(), AUTO_HEARTRATE_CHECK_EVENT);
			}
			else
			{
				osal_set_event(GetHrTaskId(), AUTO_HEARTRATE_CHECK_EVENT);
			}
			return E_RSP_OK;
		}
		else
		{
			return E_RSP_FAIL;
		}
	}
	return E_RSP_FAIL;
}

uint8 HeartRateAlarmCB(uint8 *pData)
{
	if(CurrentDataParseAction ==DATA_GET_CMD)
	{
		uint8 Temp[3]={0x00};
		Temp[0] = hr_alarm.HeartRateAlaUpper;
		Temp[1] = hr_alarm.HeartRateAlaLower;
		Temp[2] = hr_alarm.Alarm_en;
		BasicSendData(Temp, 3);
		return E_RSP_NULL;
	}
	else if(CurrentDataParseAction==DATA_SET_CMD)
	{
		if(pData[0] > pData[1])
		{
			hr_alarm.Alarm_en = pData[2];
			
			hr_alarm.HeartRateAlaUpper = hr_alarm.Alarm_en ? pData[0]:220;
			hr_alarm.HeartRateAlaLower = hr_alarm.Alarm_en ? pData[1]:0;
			WRITE_STEP_SLEEP_SUM((uint32*)&hr_alarm,HEARTRATE_ALARM_LOC,HEARTRATE_ALARM_LEN);
			return E_RSP_OK;
		}
		else
		{
			return E_RSP_FAIL;
		}
	}
	return E_RSP_FAIL;
}

uint8 SedentarinessRemindCB(uint8 *pData)
{
	if(CurrentDataParseAction ==DATA_GET_CMD)
	{
		uint8 Temp[8]={0x00};
		Temp[0] = (gSitRemind.SrSwitch?0x80:0x00) | gSitRemind.SrDayFrequency&0x7f;
		Temp[1] = gSitRemind.SrFrequency;
		Temp[2] = gSitRemind.SrBeginHour;
		Temp[3] = gSitRemind.SrBeginMinute;
		Temp[4] = gSitRemind.SrEndHour;
		Temp[5] = gSitRemind.SrEndMinute;
		Temp[6] = gSitRemind.SrStepThresholdL;
		Temp[7] = gSitRemind.SrStepThresholdH;
		
		BasicSendData(Temp, 8);
		return E_RSP_NULL;
	}
	else if(CurrentDataParseAction==DATA_SET_CMD)
	{
		gSitRemind.SrSwitch = ((pData[0]&0x80)==0x80);
		gSitRemind.SrDayFrequency = pData[0]&0x7f;
		gSitRemind.SrFrequency = pData[1];
		gSitRemind.SrBeginHour = pData[2];
		gSitRemind.SrBeginMinute = pData[3];
		gSitRemind.SrEndHour = pData[4];
		gSitRemind.SrEndMinute = pData[5];
		gSitRemind.SrStepThresholdL = pData[6];
		gSitRemind.SrStepThresholdH = pData[7];
		
		uint8 buf[SIT_REMIAND_LEN];
		buf[0] = gSitRemind.SrSwitch;
		buf[1] = gSitRemind.SrDayFrequency;
		buf[2] = gSitRemind.SrFrequency;
		buf[3] = gSitRemind.SrBeginHour;
		buf[4] = gSitRemind.SrBeginMinute;
		buf[5] = gSitRemind.SrEndHour;
		buf[6] = gSitRemind.SrEndMinute;
		buf[7] = gSitRemind.SrStepThresholdL;
		buf[8] = gSitRemind.SrStepThresholdH;
		WriteSitRemiandSetting(buf);
		
		return E_RSP_OK;
	}
	return E_RSP_FAIL;
}

uint8 BaseKcalTypeCB(uint8 *pData)
{
	if(CurrentDataParseAction == DATA_GET_CMD)
	{
		uint8 Temp[2]={0x00};
		Temp[0] = Base_Kcal_type;
		BasicSendData(Temp,1);
		return E_RSP_NULL;
	}
	else if(CurrentDataParseAction==DATA_SET_CMD)
	{
		if((pData[0]==0)||(pData[0]==1))
		{
			Base_Kcal_type = pData[0];
			WRITE_USER_PARAMETER(&Base_Kcal_type,BASE_KCAL_TYPE_LOC,BASE_KCAL_TYPE_LEN);
			StoreSportDataToFlash(0);
			return E_RSP_OK;
		}
		else
		{
			return E_RSP_FAIL;
		}
	}
	return E_RSP_FAIL;
}

/******************************************************************************************
设备计时运动记录总数
******************************************************************************************/
uint8 GetRecordWorkTotalCntCB(uint8 *pData)
{
	if(CurrentDataParseAction ==DATA_GET_CMD)
	{
		uint8 Temp[2]={0x00};

		if(RecordWork_Store_Logs>0)
		{
			gUploadRecordWorkState = UPLOAD_PREPARE;
		}	
		Temp[0] = RecordWork_Store_Logs&0xFF;
		Temp[1] = (RecordWork_Store_Logs>>8)&0xFF;
		BasicSendData(Temp,2);
		return E_RSP_NULL;
	}
	return E_RSP_FAIL; 
}
/******************************************************************************************
上传设备计时运动记录
******************************************************************************************/
void SendRecordWorkDataCmd(REC_WORK_DATA_T uData)
{
	uint8 Sdata[23]={0x00};
	Sdata[0]=(uData.Idx)&0xff;
	Sdata[1]=((uData.Idx)>>8)&0xff;
	
	Sdata[2]=(uData.TimeStamp)&0xff;
	Sdata[3]=((uData.TimeStamp)>>8)&0xff;
	Sdata[4]=((uData.TimeStamp)>>16)&0xff;
	Sdata[5]=((uData.TimeStamp)>>24)&0xff;
	
	Sdata[6]=(uData.Steps)&0xff;
	Sdata[7]=((uData.Steps)>>8)&0xff;
	Sdata[8]=((uData.Steps)>>16)&0xff;
	Sdata[9]=((uData.Steps)>>24)&0xff;	
	
	Sdata[10]=(uData.Kcal)&0xff;
	Sdata[11]=((uData.Kcal)>>8)&0xff;
	Sdata[12]=((uData.Kcal)>>16)&0xff;
	Sdata[13]=((uData.Kcal)>>24)&0xff;	
	
	Sdata[14]=(uData.Distance)&0xff;
	Sdata[15]=((uData.Distance)>>8)&0xff;
	Sdata[16]=((uData.Distance)>>16)&0xff;
	Sdata[17]=((uData.Distance)>>24)&0xff;

	Sdata[18]=(uData.Duration)&0xff;
	Sdata[19]=((uData.Duration)>>8)&0xff;
	Sdata[20]=((uData.Duration)>>16)&0xff;
	Sdata[21]=((uData.Duration)>>24)&0xff;	

	Sdata[22]=(uData.Hr)&0xff;
	BasicSendData(Sdata,23);
}

uint8 UploadRecordWorkCmdCB(uint8 *pData)
{
	if(CurrentDataParseAction ==DATA_GET_CMD)
	{
		if(*pData==0)
		{
			gUploadRecordWorkState=UPLOADING;
			RecordWork_UpLoad_Logs = 0;
		}		
		if(RecordWorkUploadData(*pData))
			return E_RSP_NULL;
	}
	return E_RSP_FAIL; 
}

/******************************************************************************************
删除设备计时运动记录
******************************************************************************************/
uint8 ClearRecordWorkDataCB(uint8 *pData)
{
	if(CurrentDataParseAction ==DATA_SET_CMD)
	{
		RecordWork_Store_Logs=0;
		WRITE_STEP_SLEEP_SUM(&RecordWork_Store_Logs,SPORT_TOTAL_LOC,SPORT_TOTAL_LEN);
		SetStepMemoryFullFlag(false);
		return E_RSP_OK; 
	}
	return E_RSP_FAIL;
}


/******************************************************************************************
设备端显示的运动/睡眠值
******************************************************************************************/
uint8 GetDisplaySportDataCmdCB(uint8 *pData)
{
	if(CurrentDataParseAction ==DATA_SET_CMD)
	{
		StopStep();
		Force_NordicFlash();
		StoreSportDataToFlash(0);
		gSportDisplay.SportSteps = pData[0]|pData[1]<<8|pData[2]<<16|pData[3]<<24;
		gSportDisplay.SportKcal = pData[4]|pData[5]<<8|pData[6]<<16|pData[7]<<24;
		gSportDisplay.SportDistance = pData[8]|pData[9]<<8|pData[10]<<16|pData[11]<<24;
		gSportDisplay.SportDuration = pData[16]|pData[17]<<8|pData[18]<<16|pData[19]<<24;	
		StepInit();
		Free_NordicFlash();
		return E_RSP_OK; 
	}
	if(CurrentDataParseAction ==DATA_GET_CMD)
	{
		uint8 Sdata[20]={0x00};
		Sdata[0]=(gSportDisplay.SportSteps)&0xff;
		Sdata[1]=((gSportDisplay.SportSteps)>>8)&0xff;
		Sdata[2]=((gSportDisplay.SportSteps)>>16)&0xff;
		Sdata[3]=((gSportDisplay.SportSteps)>>24)&0xff;

		Sdata[4]=(gSportDisplay.SportKcal)&0xff;
		Sdata[5]=((gSportDisplay.SportKcal)>>8)&0xff;
		Sdata[6]=((gSportDisplay.SportKcal)>>16)&0xff;
		Sdata[7]=((gSportDisplay.SportKcal)>>24)&0xff;	

		Sdata[8]=(gSportDisplay.SportDistance)&0xff;
		Sdata[9]=((gSportDisplay.SportDistance)>>8)&0xff;
		Sdata[10]=((gSportDisplay.SportDistance)>>16)&0xff;
		Sdata[11]=((gSportDisplay.SportDistance)>>24)&0xff;	

		Sdata[12]=(MinuteCount)&0xff;
		Sdata[13]=(MinuteCount>>8)&0xff;
		Sdata[14]=0;
		Sdata[15]=0;

		Sdata[16]=((gSportDisplay.SportDuration))&0xff;
		Sdata[17]=((gSportDisplay.SportDuration)>>8)&0xff;
		Sdata[18]=((gSportDisplay.SportDuration)>>16)&0xff;
		Sdata[19]=((gSportDisplay.SportDuration)>>24)&0xff;			
		BasicSendData(Sdata,20);
		return E_RSP_NULL;  
	}
	return E_RSP_FAIL;
}

/******************************************************************************************
自动睡眠监测
******************************************************************************************/
uint8 SetSleepModeCB(uint8 *pData)
{
	if(CurrentDataParseAction ==DATA_SET_CMD)
	{
		AutoSleepTime.BeginHour = *pData;
		AutoSleepTime.BeginMinute = *(pData+1);
		AutoSleepTime.EndHour =  *(pData+2);
		AutoSleepTime.EndMinute = *(pData+3);
		AutoSleepTime.SleepCycle = *(pData+4);		
		WRITE_USER_PARAMETER(pData,AUTO_SLEEP_LOC,AUTO_SLEEP_LEN);
		return E_RSP_OK; 
	}
	else if(CurrentDataParseAction ==DATA_GET_CMD)
	{
		uint8 Temp[5]={0x00};
		Temp[0]=AutoSleepTime.BeginHour;
		Temp[1]=AutoSleepTime.BeginMinute ;
		Temp[2]=AutoSleepTime.EndHour;
		Temp[3]=AutoSleepTime.EndMinute;
		Temp[4]=AutoSleepTime.SleepCycle;
		BasicSendData(Temp,5);
		return E_RSP_NULL;
	}
	return E_RSP_FAIL; 
}


/******************************************************************************************
电话姓名推送
******************************************************************************************/
uint8 AndroidCallCB(uint8 *pData)
{
	if (CurrentDataParseLen == 1)
		return E_RSP_FAIL;
	
	if(CurrentDataParseAction ==DATA_SET_CMD)
	{
		uint8 CallAttr = *pData;
		//INCALL_DATA_T uData;
		switch(CallAttr)
		{
			case 0:
				if(SWITCH_ANCS_INCOMING_CALL&&((CurrentDataParseLen-1) <= 90))
				{
					SaveAncsToBuf_ForAndroid_SmsAndCall(Ancs_IncomingCall,ID_TITLE,(pData+1),(CurrentDataParseLen-1),true);
#if 0
					uData.name_type = CallAttr;	
					uData.name_len=CurrentDataParseLen-1;
					IncomingCallNoticePhoneHandler(&uData,(pData+1));
#endif
					return E_RSP_OK; 
				}
			case 1:
				{
#if 0
					uData.name_type = CallAttr;	
					uData.name_len=CurrentDataParseLen-1;
					IncomingCallNoticePhoneHandler(&uData,(pData+1));
#endif
					return E_RSP_OK; 
				}
			case 2:
				if(SWITCH_ANCS_MISSED_CALL &&((CurrentDataParseLen-1) <= 90))
				{
					if(AndroidMissCallFlag==0)
					{
						SaveAncsToBuf_ForAndroid_SmsAndCall(Ancs_MissedCall,ID_TITLE,(pData+1),(CurrentDataParseLen-1),false);
						AndroidMissCallFlag=1;
					}
					else
					{
						AndroidMissCallFlag=0;
						break;
					}
					return E_RSP_OK; 

				}
			case 3:
				if(SWITCH_ANCS_MISSED_CALL &&((CurrentDataParseLen-1) == 15))
				{
					if(SWITCH_ANCS_MISSED_CALL &&((CurrentDataParseLen-1) == 15))
					{
						if(AndroidMissCallFlag==1)
						{
							SaveAncsToBuf_ForAndroid_SmsAndCall(Ancs_MissedCall,ID_DATE,(pData+1),(CurrentDataParseLen-1),true);
						}
						AndroidMissCallFlag=0;
						return E_RSP_OK;  
					}
				}					
			default:
				break;
		} 
	}
	return E_RSP_FAIL; 
}

/******************************************************************************************
短信息推送
******************************************************************************************/
uint8 AndroidSmsCallCB(uint8 *pData)
{	
	if (CurrentDataParseLen == 1)
		return E_RSP_FAIL;
		
	if(CurrentDataParseAction ==DATA_SET_CMD)
	{
		uint8 SmsAttr = *pData;
		switch(SmsAttr)
		{
			case 0:
		        	if((CurrentDataParseLen-1) <= 90)
		        	{
						if (AndroidSmsFlag == 0)
						{
							AndroidSmsFlag = 1;	
							SaveAncsToBuf_ForAndroid_SmsAndCall(Ancs_Sms,ID_TITLE,(pData+1),(CurrentDataParseLen-1),false);
			        	}
						else 
						{
							AndroidSmsFlag = 0;	
							break;
						}
						return E_RSP_OK;  
		        	}
	        				

			case 1:
		        	if((CurrentDataParseLen-1)  <= 120)
		        	{
						if (AndroidSmsFlag == 1)
						{
							SaveAncsToBuf_ForAndroid_SmsAndCall(Ancs_Sms,ID_MESSAGE,(pData+1),(CurrentDataParseLen-1),false);
							AndroidSmsFlag = 2; 	
			        	}
						else
						{
							AndroidSmsFlag = 0;
							break;
						}
						return E_RSP_OK; 
		        	}
						
					
			case 2:
		        	if((CurrentDataParseLen-1)  == 15)
		        	{
						if (AndroidSmsFlag == 2)
						{
							SaveAncsToBuf_ForAndroid_SmsAndCall(Ancs_Sms,ID_DATE,(pData+1),(CurrentDataParseLen-1),true);
							SMS_FROM_ANDROID = true;
						}
						AndroidSmsFlag = 0;
			        	return E_RSP_OK;  	
		        	}	
			default:
				break;
		} 
	}
	return E_RSP_FAIL;    
}


/******************************************************************************************
消息条数推送
******************************************************************************************/
ANCS_SAVE_TYPE GetAndroidSocialType(uint8 st)
{
	switch(st)
	{
		case 0x07:
			return Ancs_Social_Wechat;
		case 0x08:
			return Ancs_Social_Viber;
		case 0x09:
			return Ancs_Social_Snapchat;
		case 0x0A:
			return Ancs_Social_WhatsApp;
		case 0x0B:
			return Ancs_Social_QQ;	
		case 0x0C:
			return Ancs_Social_Facebook;
		case 0x0D:
			return Ancs_Social_Hangouts;
		case 0x0E:
			return Ancs_Social_Gmail;
		case 0x0F:
			return Ancs_Social_Messenger;
		case 0x10:
			return Ancs_Social_Instagram;	
		case 0x11:
			return Ancs_Social_Twitter;
		case 0x12:
			return Ancs_Social_Linkedin;
		case 0x13:
			return Ancs_Social_Uber;
		case 0x14:
			return Ancs_Social_Line;
		case 0x15:
			return Ancs_Social_Skype;				
		default:
			break;
	}
	return Ancs_Social;
}

uint8 NoticeNumberCB(uint8 *pData)
{
	if(CurrentDataParseAction ==DATA_SET_CMD)
	{
		uint8 NoticeEvent = *pData;
		//uint8 NoticeNumber = *(pData+1);
		switch(NoticeEvent)
		{
			case NOTICE_MISSEDCALL:	
				if(SWITCH_ANCS_MISSED_CALL) 
				{
					//gMissedCallCount =NoticeNumber;
					if(gMissedCallCountT==0)
					{
						gMissedCallCount=0;
						gMissedCallCountT=1;
					}
					if(gMissedCallCount < 255)
						gMissedCallCount += 1;					
					ChargeDisplayStateChange(false);
					osal_start_timerEx(GetAppTaskId(), MSG_MISSED_CALL_COUNT, 2500);
					return E_RSP_OK;  
				}
				break;
				
			case NOTICE_SMS:
				if(SWITCH_ANCS_SMS) 
				{
					//gShortMsgCount =NoticeNumber;
					if(gShortMsgCountT==0)
					{
						gShortMsgCount=0;
						gShortMsgCountT=1;
					}
					if(gShortMsgCount < 255)
						gShortMsgCount += 1;					
					ChargeDisplayStateChange(false);
					SendMsg(MSG_SMS_NOTIFY);
					return E_RSP_OK;  
				}
				break;
				
			case NOTICE_SOCIAL:
				if(SWITCH_ANCS_SOCIAL) 
				{
					//gSocialCount =NoticeNumber;
					if(gSocialCountT==0)
					{
						gSocialCount = 0;
						gSocialCountT =1;
					}
					if(gSocialCount < 255)
						gSocialCount += 1;				
					ChargeDisplayStateChange(false);
					SendMsg(MSG_SOCIAL_NOTIFY);
					return E_RSP_OK;  
				}
				break;
			case NOTICE_EMAIL:
				if(SWITCH_ANCS_EMIL) 
				{
					//gEMailCount =NoticeNumber;
					if(gEMailCountT==0)
					{
						gEMailCount=0;
						gEMailCountT=1;
					}
					if(gEMailCount < 255)
						gEMailCount += 1;					
					ChargeDisplayStateChange(false);
					SendMsg(MSG_E_MAIL_NOTIFY);           
					return E_RSP_OK;  
				}
				break;				
			case NOTICE_SCHEDULEL:
				if(SWITCH_ANCS_SCHEDULE) 
				{
					//gScheduleCount =NoticeNumber;
					if(gScheduleCountT==0)
					{
						gScheduleCount=0;
						gScheduleCountT=1;
					}
					if(gScheduleCount < 255)
						gScheduleCount += 1;					
					ChargeDisplayStateChange(false);
					SendMsg(MSG_DAILY_SCHEDULE_NOTIFY);        
					return E_RSP_OK;  
				}
				break;
				
			case NOTICE_INCOMING_CALL:
				if(SWITCH_ANCS_INCOMING_CALL) 
				{
					gIncomingCallCount =*(pData+1);
					ChargeDisplayStateChange(false);
					SendMsg(MSG_INCOMING_CALL_NOTIFY);
					return E_RSP_OK; 
				}
				break;
				
			case NOTICE_CALL_END:
				gIncomingCallCount =0;
				SendMsg(MSG_INCOMING_CALL_UNLOAD);
				return E_RSP_OK;  

			default:
				if((NoticeEvent>=0x07) && (NoticeEvent<=0x15))
				{
					if(SWITCH_ANCS_SOCIAL) 
					{
						ANCS_SAVE_TYPE ancsType =Ancs_Social;
						ancsType =GetAndroidSocialType(NoticeEvent);
						if(AndroidSocialFlag==3)
						{
							SaveAncsToBuf_ForAndroid(ancsType,ID_SAVE,NULL,0,true);
							//gSocialCount =NoticeNumber;
							if(gSocialCountT==0)
							{
								gSocialCount = 0;
								gSocialCountT =1;
							}
							if(gSocialCount < 255)
								gSocialCount += 1;								
							ChargeDisplayStateChange(false);						
							SendMsg(MSG_SOCIAL_NOTIFY);
						}
						AndroidSocialFlag=0;
						return E_RSP_OK;  
					}
				}				
				break;
		} 
	}
	return E_RSP_FAIL;    
}

/******************************************************************************************
社交推送
******************************************************************************************/
uint8 AndroidSocialCB(uint8 *pData)
{
	if (CurrentDataParseLen == 1)
		return E_RSP_FAIL;
	
	if(CurrentDataParseAction ==DATA_SET_CMD)
	{
		uint8 SmsAttr = *pData;
		switch(SmsAttr)
		{
			case 0:
		        	if((CurrentDataParseLen-1) <= 90)
		        	{
						if(AndroidSocialFlag==0)
						{		
							SaveAncsToBuf_ForAndroid(Ancs_Social,ID_TITLE,(pData+1),(CurrentDataParseLen-1),false); 
							AndroidSocialFlag=1;
						}
						else
						{
							AndroidSocialFlag=0;
							break;
						}
		        		return E_RSP_OK;  	
		        	}
			case 1:
		        	if((CurrentDataParseLen-1)  <= 120)
		        	{
		        		if(AndroidSocialFlag==1)
		        		{
							SaveAncsToBuf_ForAndroid(Ancs_Social,ID_MESSAGE,(pData+1),(CurrentDataParseLen-1),false);
							AndroidSocialFlag=2;
		        		}
						else
						{
							AndroidSocialFlag=0;
							break;
						}	
		        	}
		        	return E_RSP_OK;  	
			case 2:
		        	if((CurrentDataParseLen-1)  == 15)
		        	{
						if(AndroidSocialFlag==2)
						{
							SaveAncsToBuf_ForAndroid(Ancs_Social,ID_DATE,(pData+1),(CurrentDataParseLen-1),false);
							AndroidSocialFlag =3;
						}
						else
						{
							AndroidSocialFlag=0;
							break;
						}	
		        		return E_RSP_OK;  	
		        	}	
			default:
				break;
		} 
	}
	return E_RSP_FAIL;    
}

/******************************************************************************************
邮件推送
******************************************************************************************/
uint8 AndroideEmailCB(uint8 *pData)
{

	if (CurrentDataParseLen == 1)
		return E_RSP_FAIL;
	
	if(CurrentDataParseAction ==DATA_SET_CMD)
	{
		uint8 EmialAttr = *pData;
		switch(EmialAttr)
		{
			case 0:
		        	if((CurrentDataParseLen-1) <= 90)
		        	{
						if(AndroidEmailFlag==0)
						{
							SaveAncsToBuf_ForAndroid(Ancs_Email,ID_TITLE,(pData+1),(CurrentDataParseLen-1),false);	
							AndroidEmailFlag =1;
						}
						else
						{
							AndroidEmailFlag =0;
							break;
						}
		        		return E_RSP_OK;  	
		        	}
			case 1:
		        	if((CurrentDataParseLen-1)  <= 120)
		        	{
						if(AndroidEmailFlag==1)
						{
							SaveAncsToBuf_ForAndroid(Ancs_Email,ID_MESSAGE,(pData+1),(CurrentDataParseLen-1),false);	
							AndroidEmailFlag =2;
						}
						else
						{
							AndroidEmailFlag =0;
							break;
						}	

		        		return E_RSP_OK;  	
		        	}
			case 2:
		        	if((CurrentDataParseLen-1)  == 15)
		        	{
						if(AndroidEmailFlag==2)
							SaveAncsToBuf_ForAndroid(Ancs_Email,ID_DATE,(pData+1),(CurrentDataParseLen-1),true);
						AndroidEmailFlag=0; 

		        		return E_RSP_OK;  	
		        	}	
			default:
				break;
		} 
	}
	return E_RSP_FAIL;    
}
/******************************************************************************************
日程推送
******************************************************************************************/
uint8 AndroideScheduleCB(uint8 *pData)
{

	if (CurrentDataParseLen == 1)
		return E_RSP_FAIL;
	
	if(CurrentDataParseAction ==DATA_SET_CMD)
	{
		uint8 ScheduleAttr = *pData;
		switch(ScheduleAttr)
		{
			case 0:
					return E_RSP_OK;
			case 1:
					if((CurrentDataParseLen-1) <= 90)
					{
						if(AndroidScheduleFlag==0)
						{
							SaveAncsToBuf_ForAndroid(Ancs_Schedule,ID_TITLE,(pData+1),(CurrentDataParseLen-1),false);
							SaveAncsToBuf_ForAndroid(Ancs_Schedule,ID_MESSAGE,(pData+1),(CurrentDataParseLen-1),false);
							AndroidScheduleFlag =1;
						}
						else
						{
							AndroidScheduleFlag =0;
							break;
						}
						return E_RSP_OK;
					}
					break;
			case 2:
					if((CurrentDataParseLen-1)  == 15)
					{
						if(AndroidScheduleFlag==1)
							SaveAncsToBuf_ForAndroid(Ancs_Schedule,ID_DATE,(pData+1),(CurrentDataParseLen-1),true);
						AndroidScheduleFlag=0;
						return E_RSP_OK;
					}
					break;
			default:
				break;
		} 
	}
	return E_RSP_FAIL;
}

/******************************************************************************************
开关设置
******************************************************************************************/
void SleepContorl(void)
{
	if(SLEEP_MODE_FLAG)
	{
		if(!SWITCH_ENTER_SLEEP)
		{
			CleanAutoSleepFlag();	
			ChargeDisplayStateChange(false);
			SLEEP_MODE_DISPLAY_FLAG=false;
			osal_start_timerEx(GetAppTaskId(), MSG_SLEEP_MODE_EXIT, 1000);
		}
	}		
	else
	{	
		if(SWITCH_ENTER_SLEEP)
		{
			ChargeDisplayStateChange(false);
			AutoEnterSleep = false;
			SLEEP_MODE_DISPLAY_FLAG=true;	
			SleepKey=0;
			SendMsg(MSG_SLEEP_MODE_START);	
		}

	}
}

/*
void AutoSleepContorl(void)
{
	if(!SWITCH_AUTO_SLEEP)
	{
		if(SLEEP_MODE_FLAG)
		{
			ChargeDisplayStateChange(false);
			SLEEP_MODE_DISPLAY_FLAG=false; 			
			SendMsg(MSG_SLEEP_MODE_EXIT);
		}			
	}
}*/

uint8 UpdateDeviceSwitchSettingCB(uint8 *pData)
{
	if(CurrentDataParseAction ==DATA_SET_CMD)
	{
		uint8 SetBuf[4]={0x00};
		uint8 Set = *pData;
		if(Set == 0)
		{
			gSwitchSetting.u32PMD = (*(pData+1)) |((uint32)(*(pData+2))<<8);
			SleepContorl();	
		}
		else
		{
			uint32 Temp = gSwitchSetting.u32PMD;
			uint8 Offset =*(pData+1);
			uint8 Set =*(pData+2);	
			Temp &=  ~(0x00000001<<Offset);
			Temp |= (uint32)Set<<Offset;
			gSwitchSetting.u32PMD = Temp;
			if(Offset==0x02)
				SleepContorl();	
			if(Offset==0x0A)
			{
				gSitRemind.SrSwitch=SWITCH_SIT;
				uint8 buf[SIT_REMIAND_LEN];
				buf[0] = gSitRemind.SrSwitch;
				buf[1] = gSitRemind.SrDayFrequency;
				buf[2] = gSitRemind.SrFrequency;
				buf[3] = gSitRemind.SrBeginHour;
				buf[4] = gSitRemind.SrBeginMinute;
				buf[5] = gSitRemind.SrEndHour;
				buf[6] = gSitRemind.SrEndMinute;
				buf[7] = gSitRemind.SrStepThresholdL;
				buf[8] = gSitRemind.SrStepThresholdH;
				WriteSitRemiandSetting(buf);				
			}			
		}	
		SetBuf[0]=gSwitchSetting.u32PMD & 0xFF;
		SetBuf[1]=(gSwitchSetting.u32PMD>>8) & 0xFF;
		SetBuf[2]=(gSwitchSetting.u32PMD>>16) & 0xFF;
		SetBuf[3]=(gSwitchSetting.u32PMD>>24) & 0xFF;
		WRITE_USER_PARAMETER(SetBuf,SWITCH_LOC,SWITCH_LEN);		
		return E_RSP_OK; 
	}
	else if(CurrentDataParseAction ==DATA_GET_CMD)
	{
		uint8 Temp[2]={0x00};
		Temp[0]=gSwitchSetting.u32PMD & 0xFF;
		Temp[1]=((gSwitchSetting.u32PMD)>>8) & 0xFF;
		BasicSendData(Temp,2);
		return E_RSP_NULL;
	}
	return E_RSP_FAIL; 
}



/******************************************************************************************
提醒条数
******************************************************************************************/
uint8 ReminderNumberCmdCB(uint8 *pData)
{
	if(CurrentDataParseAction ==DATA_GET_CMD)
	{
		BasicSendData(&gRemindsCnt,1);
		return E_RSP_NULL;
	}
	return E_RSP_FAIL; 
}

/******************************************************************************************
(增加、修改、删除、查询)提醒(自定义支持多语言)
******************************************************************************************/
void ReminderSendData(uint8 idx,uint8*Rd)
{	
	uint8 Temp[PRE_REMINDER_BYTES+6] = {0x00};
	uint8 len=0;
	if(Rd[0]==0x07)
		len = 6+strlen((char*)(Rd+5));
	else
		len = 6;
	Temp[0]=DATA_START_CMD;
	Temp[1]=0x92;
	Temp[2]=DATA_GET_RSP;
	Temp[3]=len&0xFF;
	Temp[4]=(len>>8)&0xFF;	
	Temp[5]=idx;
	memcpy(&Temp[6],Rd,(len-1));
	Temp[5+len]=DATA_STOP_CMD;
	HwSendDataToBt(Temp,6+len);
}

uint8 ReminderCmdCB(uint8 *pData)
{
	if(CurrentDataParseAction ==DATA_SET_CMD)
	{
		uint8 action= *pData;
		switch(action)
		{
			case 0:
				if(CurrentDataParseLen>=6 && RemindsAdd(pData+1,(CurrentDataParseLen-6)))
					break;
				else
					return E_RSP_FAIL;
			case 1:
				if(CurrentDataParseLen>=11 && RemindsEdit(pData+1,(CurrentDataParseLen-11)))
					break;
				else
					return E_RSP_FAIL;				
			case 2:
				if(RemindsDel(pData+1))
					break;
				else
					return E_RSP_FAIL;
			case 3:
				ClearAllReminders();
				break;
			default:
				break;
		}
		SendMsg(MSG_REFRESH_REMIND);
		return E_RSP_OK;	
	}
	else if(CurrentDataParseAction ==DATA_GET_CMD)
	{
		if(*pData==0)
		{
			gUploadRemindsCnt =0;
			gUploadRemindsDataState = UPLOAD_REMIND_ING;
		}		
		if(RemindsUploadAll(*pData))
			return E_RSP_NULL;
	}
	return E_RSP_FAIL; 
}


/******************************************************************************************
绑定开始
******************************************************************************************/
uint8 BindingBeginCmdCB(uint8 *pData)
{
	if(CurrentDataParseAction ==DATA_SET_CMD)
	{
		uint8 action= *pData;
		if(BLE_BOND_REQUIRED_FLAG)
		{
			if(action==0)
			{
				UserIDTemp = ((uint32)((uint32)((*(pData+1)) & 0x00FF) + ((uint32)((*(pData+2)) & 0x00FF) << 8) + ((uint32)((*(pData+3)) & 0x00FF) << 16) + ((uint32)((*(pData+4)) & 0x00FF) << 24)));
			}
			else
				UserIDTemp =0;		
			BleBondRequiredOk = 1;
			BondRequiredCounter = 30;		
			ChargeDisplayStateChange(false);
			Android_ios =false;
			SMS_FROM_ANDROID=false;
			osal_start_timerEx(GetAppTaskId(), MSG_SLEEP_MODE_FORCE_EXIT, 100);	
			SendMsg(MSG_BLE_BOND_CHOOSE);
			return E_RSP_NULL;
		}	
	}
	else if(CurrentDataParseAction ==DATA_GET_CMD)
	{
		BasicSendData((uint8*)&UserID,4);
		return E_RSP_NULL;
	}	
	return E_RSP_FAIL; 
}

/******************************************************************************************
绑定结束
******************************************************************************************/
uint8 BindingEndCmdCB(uint8 *pData)
{
	if(CurrentDataParseAction ==DATA_SET_CMD)
	{
		//ClearInitMarkEvent();
		//SendMsg(MSG_BIND_OK);
		osal_start_timerEx(GetAppTaskId(), MSG_BIND_END, 100 ); 
		ClearInitMarkEvent();
		if(UserID != UserIDTemp)
			IsSameUser = false;
		else
			IsSameUser = true;
		if(UserIDTemp!=0)
		{
			UserID = UserIDTemp;
			UserIDTemp = 0;
		}		
		return E_RSP_OK;	
	}
	else if(CurrentDataParseAction ==DATA_GET_CMD)
	{
		BasicSendData((uint8*)&gInitMark,1);
		return E_RSP_NULL;
	}
	return E_RSP_FAIL; 
}



/******************************************************************************************
支付：读取芯片应用卡号
******************************************************************************************/
void SendPayAID(uint8* uData)
{
	uint8 len = strlen((char*)uData);
	BasicSendData(uData,len);
}
uint8 ReadPayAID(uint8 *pData)
{
	if(CurrentDataParseAction ==DATA_SET_CMD)
	{
		return E_RSP_OK;	
	}
	else if(CurrentDataParseAction ==DATA_GET_CMD)
	{
		if((NFC_FIELD_FLAG == 0) && (DATA_TRAN_FLAG == 0)) {
			Pay_Aid= *pData;
			DATA_TRAN_FLAG=1;
			osal_set_event(GetHwTaskId(), PAY_CARDID_UPLOAD_EVENT); 
			return E_RSP_NULL;
		}	
	}
	return E_RSP_FAIL; 
}
/******************************************************************************************
支付：读取余额
******************************************************************************************/
void SendPayBalance(uint8* uData)
{
	BasicSendData(uData,4);
}
uint8 ReadPayBalance(uint8 *pData)
{
	if(CurrentDataParseAction ==DATA_SET_CMD)
	{
		return E_RSP_OK;	
	}
	else if(CurrentDataParseAction ==DATA_GET_CMD)
	{
		if((NFC_FIELD_FLAG == 0) && (DATA_TRAN_FLAG == 0)) {
			Pay_Aid= *pData;
			DATA_TRAN_FLAG=1;
			osal_set_event(GetHwTaskId(), PAY_BALANCES_UPLOAD_EVENT); 
			return E_RSP_NULL;
		}	
	}
	return E_RSP_FAIL; 	
}
/******************************************************************************************
支付：读取交易记录
******************************************************************************************/
uint8 ReadPayRecords(uint8 *pData)
{
	if(CurrentDataParseAction ==DATA_SET_CMD)
	{
		return E_RSP_OK;	
	}
	else if(CurrentDataParseAction ==DATA_GET_CMD)
	{
		uint8 uData =0;
		BasicSendData(&uData,1);
		return E_RSP_NULL;	
	}
	return E_RSP_FAIL; 	
}


/******************************************************************************************
支付：透传 (预留)
******************************************************************************************/
void PayPassthroughToApp(uint8 Aid, uint8 *pData, uint16 len)
{
	uint8 TempSendData[284]={0x00};
	len = len+1;
	TempSendData[0]=DATA_START_CMD;
	TempSendData[1]=0xB3;
	TempSendData[2]=DATA_GET_RSP;
	TempSendData[3]=len&0xFF;
	TempSendData[4]=(len>>8)&0xFF;	
	TempSendData[5]=Aid;
	memcpy(&TempSendData[6],pData,len-1);
	TempSendData[5+len]=DATA_STOP_CMD;
	HwSendDataToBt(TempSendData,6+len);
}
uint8 PayPassthroughFromApp(uint8 *pData)
{
	if(CurrentDataParseAction ==DATA_GET_CMD)
	{
		if((pData!=NULL) && (NFC_FIELD_FLAG == 0) && (DATA_TRAN_FLAG == 0)) {
			Pay_Aid= pData[0];
			memcpy(pay_data, &pData[1], (pData[1]+1));
			DATA_TRAN_FLAG=1;
			osal_set_event(GetHwTaskId(), PAY_RECORD_UPLOAD_EVENT); 
			return E_RSP_NULL; 
		}
	}
	return E_RSP_FAIL;
}

/******************************************************************************************
音乐查询/设置
******************************************************************************************/
uint8 MusicSettingsCmdCB(uint8 *pData)
{
	MusicSet States = MS_NO;
	States = MusicStates;
	MusicStates=(MusicSet)(*pData);
	switch(MusicStates)
	{
		case MS_PLAY:
		case MS_STOP:
			{
				MusicBuf[0]=MusicStates;
				MusicBuf[1]=0;
				if(CurrentDataParseLen>2)
				{	
				 	if((MusicBuf[2]==0)||(strncmp((char*)(MusicBuf+3), (char*)(pData+1),(CurrentDataParseLen-1)) != 0))
				 	{
						memcpy((char*)(MusicBuf+3),(char*)(pData+1),(CurrentDataParseLen-1));
						MusicBuf[1]=1;
						MusicBuf[2]=(CurrentDataParseLen-1);
				 	}
				}
			}
			break;
		case 0x02:	//音量设置
			MusicStates = States;
			MusicVol = pData[1];
			return E_RSP_OK;
		default:
			return E_RSP_OK;
	}
	osal_stop_timerEx(GetAppTaskId(), MSG_MUSIC_NOTIC_CONNECTING);
	SendMsg(MSG_MUSIC_NOTIC);
	return E_RSP_OK;
}
/******************************************************************************************
相机控制
******************************************************************************************/
uint8 CameraControlCmdCB(uint8 *pData)
{
	//CameraStates= *pData;
	//SendMsg(MSG_CAMERA_NOTIC);
	return E_RSP_OK;
}
/******************************************************************************************
寻找手机
******************************************************************************************/
uint8 SearchForMobilePhonesCmdCB(uint8 *pData)
{
	//SearchStates = (SearchSet)(*pData);
	SendMsg(MSG_SEARCH_NOTIC);
	return E_RSP_OK;
}

/******************************************************************************************
设备间通信用户ID
******************************************************************************************/
uint8 DeviceCommIDCmdCB(uint8 *pData)
{
	uint8 len =CurrentDataParseLen;
	if(CurrentDataParseAction ==DATA_SET_CMD)
	{
		if(len>DEVICE_COMMID_LEN)
			len = DEVICE_COMMID_LEN;
		memset(mName,0,13);
		memcpy(mName,pData,len);
		WriteDeviceCommID(mName);
		return E_RSP_OK;	
	}
	else if(CurrentDataParseAction ==DATA_GET_CMD)
	{
		len = strlen((char*)mName);
		BasicSendData(mName,len);
		return E_RSP_NULL;
	}
	return E_RSP_FAIL; 	
}


/******************************************************************************************
擦除外部Flash的Flag保存区域
******************************************************************************************/
void SetFactoryStandbyTime(void)
{
	if(StandbyTimeModifyEnable==false)
	{
		StandbyTimeModifyEnable = true;
		SetSystemSuspendTime(DISPLAY_TIME_10S_FLAG);
		StandbyTimeModifyEnable = false;	
	}
	else
		SetSystemSuspendTime(DISPLAY_TIME_10S_FLAG);
}

uint8 SpiFlashEraseSaveFlag(uint8 *pData)
{
	if(CurrentDataParseAction ==DATA_SET_CMD)
	{
		//SpiFlash_4KSector_Erase(WATCH_ID_BASE_ADDR);
		SpiFlash_4KSector_Erase(WATCH_ID_BASE_ADDR);
		InitBandInfo();
		ResetBoard();
		return E_RSP_OK; 
	}
	return E_RSP_FAIL;
}

/******************************************************************************************
升级字库
******************************************************************************************/
uint8 SpiFlashUpdateFontLib(uint8 *pData)
{	
	if(CurrentDataParseAction ==DATA_SET_CMD)
	{
		CleanAppTaskMessage();
		UPDATE_FONTLIB =true;
		SendMsg(MSG_UPDATE_FONT_LIB);
		return E_RSP_OK; 
	}
	return E_RSP_FAIL;	
}

/******************************************************************************************
测试LED
******************************************************************************************/
uint8 LEDTest(uint8 *pData)
{	
	if(CurrentDataParseAction ==DATA_SET_CMD)
	{
		if(LedTest==0xff)
		{
			LedTest =0x00;
			LED_TEST =true;
			osal_start_timerEx(GetAppTaskId(), MSG_LED_TEST, 500);
			return E_RSP_OK; 
		}
	}
	return E_RSP_FAIL;	
}


/******************************************************************************************
测试NFC
******************************************************************************************/
uint8 NFCTest(uint8 *pData)
{	
	if(CurrentDataParseAction ==DATA_SET_CMD)
	{
		if(NfcTest==0xff)
		{
			SetSystemSuspendTime(15);
			CleanAppTaskMessage();
			NfcTest =0x00;
			NFC_TEST = *pData;
			osal_start_timerEx(GetAppTaskId(), MSG_NFC_TEST, 500);
			return E_RSP_NULL; 
		}
	}
	return E_RSP_FAIL;	
}


/******************************************************************************************
获取CSN
******************************************************************************************/
void SendCSNData(uint8* uData)
{
	BasicSendData(uData,10);
}

uint8 Get_CSN(uint8 *pData)
{	
	if(CurrentDataParseAction ==DATA_GET_CMD)
	{
		if(GetCSN==0xff)
		{
			SetSystemSuspendTime(15);
			CleanAppTaskMessage();
			GetCSN =0x00;
			osal_start_timerEx(GetAppTaskId(), MSG_GET_CSN, 500);
			return E_RSP_NULL; 
		}
	}
	return E_RSP_FAIL;	
}


/******************************************************************************************
恢复出厂设置
******************************************************************************************/
uint8 TestFactoryMode(uint8 *pData)
{
	if(CurrentDataParseAction ==DATA_SET_CMD)
	{
		SetFactorySettingEvent();
		SendMsg(MSG_SYSTEM_FACTORY_SET_STATE);
		return E_RSP_OK; 
	}
	return E_RSP_FAIL;	
}

/******************************************************************************************
获取蓝牙4.0地址
******************************************************************************************/
uint8  GetBT4AddrTest(uint8 *pData)
{
	if(CurrentDataParseAction ==DATA_GET_CMD)
	{
		ble_gap_addr_t   bt4Address;
		sd_ble_gap_address_get(&bt4Address);
		BasicSendData((uint8 *)(bt4Address.addr),6);
		return E_RSP_NULL;
	}
	return E_RSP_FAIL;
}

/******************************************************************************************
测试按键
******************************************************************************************/
uint8  KeypadTest(uint8 *pData)
{
	return E_RSP_OK;
}

/******************************************************************************************
获取当前设备时间
******************************************************************************************/
uint8  GetDateTimeTest(uint8 *pData)
{
	if(CurrentDataParseAction ==DATA_GET_CMD)
	{
		BAND_TIME_MSG_DATA_T uData;
		UTCTimeStruct uTime;

		gOS_GetUTCTime(&uTime);

		uData.Year = uTime.year;
		uData.Month= uTime.month;
		uData.Day= uTime.day;
		uData.Hour= uTime.hour;
		uData.Minute= uTime.minutes;
		uData.Second= uTime.seconds;
		BasicSendData((uint8*)&uData,7);
		return E_RSP_NULL;
	}
	return E_RSP_FAIL;
}

/******************************************************************************************
重启整个系统
******************************************************************************************/
uint8  PowOFFSystemTest(uint8 *pData)
{
	if(CurrentDataParseAction ==DATA_SET_CMD)
	{
		PowerOffSystem();
		return E_RSP_OK; 
	}
	return E_RSP_FAIL;
}
/******************************************************************************************
测试振子
******************************************************************************************/
uint8  MotorTest(uint8 *pData)
{
	if(Powerstate.standbystate==false)
	{
		SetFactoryStandbyTime();
		if(CurrentDataParseAction ==DATA_SET_CMD)
		{
			if(*pData == 0x01)
			{
				//MOTOR_PIN_OUT;
				//MOTOR_PIN_H;
				Set_Motor_Level();
			}
			else if (*pData == 0x00)
			{
				//MOTOR_PIN_L;
				Set_Motor_LevelLow();
			}
			return E_RSP_OK; 
		}
	}
	return E_RSP_FAIL;	
}

/******************************************************************************************
测试TP
******************************************************************************************/
uint8 TpTest(uint8 *pData)
{	
	if(Powerstate.standbystate==false)
	{
		SetFactoryStandbyTime();
		if(CurrentDataParseAction ==DATA_SET_CMD)
		{
			CleanAppTaskMessage();
			TP_TEST =true;
			TpTest_XY = *pData;
			SendMsg(MSG_TEST_TP);
			return E_RSP_OK; 
		}
	}
	return E_RSP_FAIL;	
}
/******************************************************************************************
测试G-Sensor
******************************************************************************************/
uint8  SensorTest(uint8 *pData)
{
	if(Powerstate.standbystate==false)
	{
		SetFactoryStandbyTime();
		if(CurrentDataParseAction ==DATA_GET_CMD)
		{
			if(GsensorType == SENSOR_LIS3DH)
			{
				static int16 acc[3] = {0};
				SENSOR_DATA_T data;
				StopStep();
				Lis3dh_Sleep_SetConfig();
				nrf_delay_ms(20);
				Gsensor_read_accel(acc);
				nrf_delay_ms(20);
				Gsensor_read_accel(acc);
				data.x = acc[0];
				data.y = acc[1];
				data.z = acc[2];
				BasicSendData((uint8*)&data,6);
				StartStep();
				return E_RSP_NULL;
			}
			else if(GsensorType == SENSOR_BMA250E)
			{
				short x=0,y=0,z=0;
				SENSOR_DATA_T data;
				StopStep();
				Bma250E_Sleep_SetConfig();
				extern void DelayMs(unsigned long ms);
				DelayMs(20);
				bma2x2_read_accel_x(&x);
				bma2x2_read_accel_y(&y);
				bma2x2_read_accel_z(&z);
				DelayMs(20);
				bma2x2_read_accel_x(&x);
				bma2x2_read_accel_y(&y);
				bma2x2_read_accel_z(&z);

				data.x = x;
				data.y = y;
				data.z = z;
				BasicSendData((uint8*)&data,6);
				StartStep();
				return E_RSP_NULL;			
			}
		}
	}
	return E_RSP_FAIL;
}

/******************************************************************************************
测试LCD
******************************************************************************************/
uint8  LCDTest(uint8 *pData)
{
	if(Powerstate.standbystate==false)
	{
		SetFactoryStandbyTime();
		osal_stop_timerEx(GetAppTaskId(), MSG_DISPLAY);
		osal_start_timerEx(GetAppTaskId(), MSG_DISPLAY, 10000);
		if(CurrentDataParseAction ==DATA_SET_CMD)
		{
			ShowLcdTest(*pData);
			return E_RSP_OK; 
		}
	}
	return E_RSP_FAIL;	
}

/******************************************************************************************
写WatchID
******************************************************************************************/
bool VerifyWatchId(uint8 * inWatchId, uint8 len)
{
	bool result = false;
	if(inWatchId != NULL && len >= WATCH_ID_LEN)
	{
		if(strncmp((char*)inWatchId, WATCH_ID_PREFIX, strlen(WATCH_ID_PREFIX)) == 0)
		{
			result = true;
		}		
	}
	return result;
}

uint8 WriteWatchIdCB(uint8 *uData)
{
	if(CurrentDataParseAction ==DATA_SET_CMD)
	{
		if((*uData=='$')&&(*(uData+1)=='$')&&(*(uData+WATCH_ID_LEN+2)=='&')&&(*(uData+WATCH_ID_LEN+3)=='&'))
		{	
	              uint8 ReadWatchIdTemp[WATCH_ID_LEN]={0};
			memset(ReadWatchIdTemp, 0, WATCH_ID_LEN);
			memcpy(ReadWatchIdTemp, uData+2, WATCH_ID_LEN);
			
			if(VerifyWatchId(ReadWatchIdTemp, WATCH_ID_LEN))
			{
				WriteWatchId(ReadWatchIdTemp);
				return E_RSP_OK;
			}
		}
	}
	return E_RSP_FAIL;		
}
uint8 WatchIDTest(uint8 *pData)
{
	if(CurrentDataParseAction ==DATA_SET_CMD)
	{
		uint8 InitMark=0;
		if(WriteWatchIdCB(pData) != E_RSP_OK)
			return E_RSP_FAIL;
		WRITE_USER_FLAG(&InitMark,INIT_MARK_LOC,INIT_MARK_LEN);
		SendRspCmd(IN_WATCH_ID_TEST_CMD, E_RSP_OK);
		InitBandInfo();
		ResetBoard();
		return E_RSP_NULL; 
	}
	return E_RSP_FAIL;
}


/******************************************************************************************
*函数名:		InitBandInfo
*参数:			
*返回值:		无
*描述:			手镯初始化信息
*编写:			statdream
*版本信息:		V1.0
******************************************************************************************/
void InitBandInfo(void)
{
	Force_NordicFlash();
/*Sector 1 存储 (重要标志)*/	
	uint8 Bc5Reconnected = 0;
	WRITE_USER_FLAG(&Bc5Reconnected,BC5_RECONNECTED_LOC,BC5_RECONNECTED_LEN);
	
/*Sector 2 存储 (用户设置)*/
	SpiFlash_4KSector_Erase(USER_PARAMETER_BASE_ADDR);

/*Sector 3 存储 (运动/休眠总条数)*/	
	SpiFlash_4KSector_Erase(STEP_SLEEP_SUM_BASE_ADDR);

/*Sector 4存储 (提醒) */
	SpiFlash_4KSector_Erase(REMINDER_BASE_ADDR);

/*Sector 5存储 (存储睡眠详细)*/
	SpiFlash_4KSector_Erase(SLEEP_DETAIL_BASE_ADDR);

/*Sector 6存储 (OTA存储区域)*/
	SpiFlash_4KSector_Erase(OTA_INFO_BASE_ADDR);

/*Sector 7存储 (时间保存)*/
	SpiFlash_4KSector_Erase(TIMER_SAVE_BASE_ADDR);

/*Sector 9存储 (运动显示保存)*/
	SpiFlash_4KSector_Erase(SPORT_DIPLAY_SAVE_BASE_ADDR);

/*ANCS信息保存*/
	ClearAncsFlash();
/*通信记录保存*/
	ClearPhoneRecordFlash();
/*通讯录信息保存*/
	ClearPhoneBookFlash();
	
	pS_OsWatchDogFeed(); /*喂狗*/
	Force_NordicFlash();
	//Free_NordicFlash();	
}


