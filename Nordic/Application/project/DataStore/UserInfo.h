


#include "Typedef.h"

#define SWITCH_ANTI_LOST				gSwitchSetting.PMD.PMD0
#define SWITCH_AUTO_SYS				gSwitchSetting.PMD.PMD1
#define SWITCH_ENTER_SLEEP				gSwitchSetting.PMD.PMD2
#define SWITCH_AUTO_SLEEP				gSwitchSetting.PMD.PMD3
#define SWITCH_ANCS_INCOMING_CALL	gSwitchSetting.PMD.PMD4
#define SWITCH_ANCS_MISSED_CALL		gSwitchSetting.PMD.PMD5
#define SWITCH_ANCS_SMS				gSwitchSetting.PMD.PMD6
#define SWITCH_ANCS_SOCIAL			gSwitchSetting.PMD.PMD7
#define SWITCH_ANCS_EMIL				gSwitchSetting.PMD.PMD8
#define SWITCH_ANCS_SCHEDULE			gSwitchSetting.PMD.PMD9
#define SWITCH_SIT						gSwitchSetting.PMD.PMD10
#define SWITCH_LOW_POWER				gSwitchSetting.PMD.PMD11
#define SWITCH_SECOND_REMIND			gSwitchSetting.PMD.PMD12


#define USB_GO_IN_FLAG 					gDataConfig.PMD.PMD0
#define DISPLAY_GOTO_SLEEP_FLAG		gDataConfig.PMD.PMD1
#define MEM_FULL_FLAG     				gDataConfig.PMD.PMD2
#define STEP_ON_FLAG  					gDataConfig.PMD.PMD3
#define SCAN_KEY_VALID_FLAG  			gDataConfig.PMD.PMD4
#define PROGRAM_DATA_VALID_FLAG  		gDataConfig.PMD.PMD5
#define SLEEP_MODE_FLAG  				gDataConfig.PMD.PMD6
#define SLEEP_MODE_DISPLAY_FLAG 		gDataConfig.PMD.PMD7
#define BC5_POWERON 					gDataConfig.PMD.PMD8
#define KEEP_BC5_POWERON  				gDataConfig.PMD.PMD9
#define BUTTON_RECONNECT_BC5  			gDataConfig.PMD.PMD10
#define FORCE_SUSPEND_FLAG  			gDataConfig.PMD.PMD11
#define UPDATE_FONTLIB					gDataConfig.PMD.PMD12
#define UPDATE_FONTLIB_SEND_C			gDataConfig.PMD.PMD13
#define ANCS_ENABLE						gDataConfig.PMD.PMD14
#define BLE_BOND_REQUIRED_FLAG		gDataConfig.PMD.PMD15
#define LED_LIGHT_TIME 					gDataConfig.PMD.PMD16
#define TOUCHKEY_PRESS_FLAG 			gDataConfig.PMD.PMD17
#define UART0_OFF_FLAG					gDataConfig.PMD.PMD18
#define LED_TEST						gDataConfig.PMD.PMD19
#define LCD_DISPLAY_FLAG				gDataConfig.PMD.PMD20
#define CYPRESS_INT_FLAG				gDataConfig.PMD.PMD21
#define FORCE_NORDIC_FLASH_FLAG		gDataConfig.PMD.PMD22
#define TP_RESUME_FLAG					gDataConfig.PMD.PMD23
#define LCD_ENABLE						gDataConfig.PMD.PMD24
#define BC5_UART_FLAG					gDataConfig.PMD.PMD25
#define TP_TEST							gDataConfig.PMD.PMD26
#define SMS_FROM_ANDROID				gDataConfig.PMD.PMD27
#define SUSPEND_NORDIC_USE_FLASH		gDataConfig.PMD.PMD28
#define SCAN_KEY_ENABLE				gDataConfig.PMD.PMD29
#define POWER_KEY_ENABLE				gDataConfig.PMD.PMD30
#define POWER_OFF_BT_OFF				gDataConfig.PMD.PMD31


#define OPEN_BACKLIGHT_FLAG 			gDataConfig2.PMD.PMD0
#define INCOMMING_CALL_ADD			gDataConfig2.PMD.PMD1
#define SLEEP_DATA_SAVE				gDataConfig2.PMD.PMD2
#define SLEEP_DATA_SET					gDataConfig2.PMD.PMD3
#define RECORDWORK_SAVE_FLAG			gDataConfig2.PMD.PMD4

//恢复出厂设置的默认值
#define INIT_BAND_USERINFO_SEX    0
#define INIT_BAND_USERINFO_AGE    20
#define INIT_BAND_USERINFO_HEIGHT    160
#define INIT_BAND_USERINFO_WEIGHT    6000
#define INIT_BAND_DAILYGOAL_STEPS    7000
#define INIT_BAND_DAILYGOAL_DISTANCE    5000
#define INIT_BAND_DAILYGOAL_ACTIVETIME    60
#define INIT_BAND_DAILYGOAL_KCAL    350
#define INIT_BAND_DAILYGOAL_SLEEPTIME    480
#define INIT_BAND_SWITCH_BACKLINGT    5
#define INIT_BAND_SWITCH_ANTILOST    1
#define INIT_BAND_SWITCH_NOTICEMODE    1
#define INIT_BAND_SWITCH_SMS    1
#define INIT_BAND_SWITCH_MISSEDCALL    1
#define INIT_BAND_SWITCH_INCOMINGCALL    1
#define INIT_BAND_SWITCH_EMAIL    1
#define INIT_BAND_SWITCH_SCHEDULE    1
#define INIT_BAND_SWITCH_SOCIAL    1


extern uint8 gInitMark,gDateTimeValid;
extern DATA_CONFIG_T gDataConfig;
extern DATA_CONFIG_T gDataConfig2;
extern SPORT_DATA_T gSportDisplay;
extern SPORT_DATA_T gSportDisplayStore;
extern USER_INFO_DATA_T UserInfoData;
extern SIT_REMIAND_DATA_T   gSitRemind;
extern uint8 gShortMsgCount, gIncomingCallCount, gMissedCallCount,gEMailCount,gScheduleCount,gSocialCount;
extern uint8 gShortMsgCountT, gIncomingCallCountT, gMissedCallCountT,gEMailCountT,gScheduleCountT,gSocialCountT;
extern uint32 SpiFlash_Addr,CrcTotalValue; 

extern uint8 MenuOrder[9];
extern uint8 MotoOrder[10];
extern TIME_ORDER TimeOrder;
extern SET_VALUE SetValue;
extern DAILY_GOAL DailyGoal[5];
extern AUTO_SLEEP_DATA_T AutoSleepTime;
extern DATA_CONFIG_T gSwitchSetting;
extern bool UpdateKl17Success;
extern bool UpdateAS7000Success;
extern HEARTRATE_DATA_T gHeartrateDisplay;
extern uint8 OTALanguagetype;
extern uint8 OTALanguageVersion[2];
extern uint8 LastLanguage;
extern uint8 PhoneBoookSave;

extern bool UpdateTPSuccess;
extern uint8 MemFullCount;
extern uint8 LedTest;
extern uint8 LedStop;
extern uint8 OtaFlag;
extern uint8 NfcTest;
extern uint8 GetCSN;
extern uint8 NFC_TEST;
extern uint8 TpTest_XY;
extern uint16 RtcSaveIndex;
extern uint8 ReadEnd;
extern uint16 SportDisplySaveIndex;
extern bool  AncsMotoSkip; 

extern uint32 UserID;
extern uint32 UserIDTemp;
extern bool IsSameUser;
extern Gsensor GsensorType;
extern uint8 Bma250EGotoLowPower;
extern SleepMapData SleepDay[8];
extern uint8 SleepDayCount;
extern uint32 TotalHR;
extern uint32 CountHR;
extern uint32 RwSec;
extern uint32 RwMSec,RwMMinute,gRecordSysTick_start;
extern uint32 uStepsSave,uDistanceSave,uCalorSave,uTimeSave;
extern uint32 uStepsResult,uDistanceResult,uCalorResult;
extern uint8 SocialClassCount[9];
extern uint8 Medal_Type;
extern uint8 Camera_state;
extern bool ExitSleepKey;
void UserInfoInit(void);



