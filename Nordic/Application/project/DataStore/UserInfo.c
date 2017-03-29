#include "headfile.h"


uint8 gInitMark=0,gDateTimeValid=0;
DATA_CONFIG_T gDataConfig;
DATA_CONFIG_T gDataConfig2;
SPORT_DATA_T gSportDisplay,gSportDisplayStore;
HEARTRATE_DATA_T gHeartrateDisplay;


USER_INFO_DATA_T UserInfoData;
SIT_REMIAND_DATA_T   gSitRemind;
uint8 gShortMsgCount=0, gIncomingCallCount=0, gMissedCallCount=0,gEMailCount=0,gScheduleCount=0,gSocialCount=0;
uint8 gShortMsgCountT=0, gIncomingCallCountT=0, gMissedCallCountT=0,gEMailCountT=0,gScheduleCountT=0,gSocialCountT=0;
uint32 SpiFlash_Addr=0,CrcTotalValue=0;

uint8 MenuOrder[9]={0x00};
uint8 MotoOrder[10]={0x00};
TIME_ORDER TimeOrder;
SET_VALUE SetValue;
DAILY_GOAL DailyGoal[5];
AUTO_SLEEP_DATA_T AutoSleepTime;
DATA_CONFIG_T gSwitchSetting;
bool UpdateKl17Success = false;
bool UpdateTPSuccess = false;
uint8 OtaFlag = 0;
bool UpdateAS7000Success = false;
uint8 OTALanguagetype;
uint8 OTALanguageVersion[2];
uint8 LastLanguage=0;
uint8 PhoneBoookSave = 0;

uint8 MemFullCount=0xff;
uint8 LedTest=0xff;
uint8 NfcTest=0xff;
uint8 GetCSN=0xff;
uint8 LedStop= 0;
uint8 NFC_TEST = 0;
uint8 TpTest_XY=0;
uint16 RtcSaveIndex =0;
uint8 ReadEnd=0;
uint16 SportDisplySaveIndex =0;
bool  AncsMotoSkip = false;

uint32 UserID = 0;
uint32 UserIDTemp = 0;
bool IsSameUser = false;
Gsensor GsensorType;
uint8 Bma250EGotoLowPower;
SleepMapData SleepDay[8];
uint8 SleepDayCount;
uint32 TotalHR;
uint32 CountHR;
uint32 RwSec;
uint32 RwMSec,RwMMinute,gRecordSysTick_start;
uint32 uStepsSave,uDistanceSave,uCalorSave,uTimeSave;
uint32 uStepsResult,uDistanceResult,uCalorResult;
uint8 SocialClassCount[9];
uint8 Medal_Type = 0;
uint8 Camera_state = 0;
bool ExitSleepKey=false;
/******************************************************************************************
*函数名:		UserInfoInit
*参数:			
*返回值:		
*描述:			初始化用户等信息
*编写:			statdream 
*版本信息:	    v1.0
******************************************************************************************/
void UserInfoInit(void)
{     
	bool ForceNordicFlash=FORCE_NORDIC_FLASH_FLAG;
	gDataConfig.u32PMD=0;//初始化DataConfig數據
	gDataConfig2.u32PMD=0;//初始化DataConfig數據
       gSwitchSetting.u32PMD =0;
	OPEN_BACKLIGHT_FLAG=true;
	ANCS_ENABLE=true;
	LED_LIGHT_TIME =true;
	memset(&gSportDisplay,0,sizeof(SPORT_DATA_T));//当天运动数据
	
        gShortMsgCount = 0;
        gMissedCallCount = 0;
        gIncomingCallCount = 0;
        gEMailCount=0;
        gScheduleCount=0;
        gSocialCount=0;
	 FORCE_NORDIC_FLASH_FLAG = 	ForceNordicFlash;
}

