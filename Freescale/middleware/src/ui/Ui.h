#ifndef _UI_H
#define _UI_H

#include "NordicCmdPackage.h"
#include "typedef.h"

#define L40A

#define WATCH_ID_LEN                        			(20)
typedef struct  shortcut_s{
	uint8 find_me;					//1: on;  0: off
	uint8 airplane_mode;			//1: on; 0: off
	uint8 vibration;				//1: on; 0: off
	uint8 battery_level;
	uint8 battery_percent;			
	uint8 brightness_level;			//level0~level10
	uint8 BC5_Connect;
} shortcut_t;

typedef struct 
{
	uint16 total_sleep;
	uint16 fell_sleep;
	uint16 light_sleep;
	uint16 deep_sleep;
	uint16 awake_sleep;
	uint16 awake;
}sleep_info_t;

typedef struct 
{
	uint8 hour;
	uint8 minutes;
	uint8 TimeFormat;
	uint8 bat_level;
	uint8 bat_percent;
}time_battery_info_t;


typedef struct 
{
	uint8 state;   //0: 正常显示；1:高亮显示
	uint8 hour;
	uint8 minute;	
	uint8 date;
	uint8 month;
	uint16 year;
}sleep_overview_t;

typedef struct watch_info_s{
	uint8 WatchId[WATCH_ID_LEN];
	uint8 version[3];
}watch_info_t;


typedef enum
{
  REMIND_EAT=0x00,
  REMIND_MEDICINE,
  REMIND_DRINKING,
  REMIND_SLEEP,
  REMIND_WAKEUP,
  REMIND_MOVING,
  REMIND_MEETING,
  REMIND_USER,
}REMIND_TYPE_T;

typedef enum{
    PHONE_IN_RING,			/*来电界面*/
    PHONE_OUT_RING,			/*去电界面*/	
    PHONE_IN_OUT_CALL,		/*通话界面*/	
    PHONE_IN_OUT_END,		/*通话结束界面*/	
    PHONE_IN_REJECT,		/*拒绝来电*/
    PHONE_OUT_CANCEL		/*去电取消*/
}PhoneDisplayStatus;

typedef enum{
    bar_activity_barH_a168x10=0,			/*横向进度条*/
    bar_activity_barH_a210x12,			/*横向进度条*/	
    bar_activity_barV_a12x118,		/*纵向进度条*/	
    bar_dail_barV_a7x176,		/*纵向进度条*/	
    bar_hr_barH_a210x12,		/*横向进度条*/
    bar_update_barH_a194x11,		/*横向进度条*/
    bar_volume_bar_108x9,
    bar_brightness_bar_108x9,
    bar_sleep_barH_a210x12,
    barH_steps_a240x10,
    barH_distance_a240x10,
    barH_calories_a240x10,
    barH_sporttimer_a240x10,
    barH_music_vol_a114x20,
    barH_music_schedule_a138x16,
}BarDisplayType;

typedef enum{
    blue_timer=0,	
    green_diatance,	
    red_calaries,	
    yellow_step,
    yellow_timer,
    blue_diatance,
    green_step,
    other,
}BarDisplaymode;

typedef enum
{
  Bold_26x40=0x00,
  Ceriph_26x40,
  Dot_13x40,
}Font_TYPE_T;

typedef enum
{
	HR_GOING, 
	HR_WAITING, 
	HR_WAITING_INIT,
	HR_STOP,
}HR_STATE;

typedef enum 
{
	BC5_SIRI_CLOSE=0x00,
	BC5_SIRI_OPEN=0x01
}Siri_STATE;

typedef struct
{
	uint8_t HR_refresh_first;
	uint8_t HR_mode; 
	uint8_t HR_Run_State;
    uint8_t HR_Value_Val; 	
    uint8_t HR_Value;         
    uint8_t HR_Running;
	uint8_t DHM_Flag;
	uint8_t DHM_Time;
	uint8_t HR_Level;
	uint8_t HR_ALARM; //Low--0x01,High--0x02
} UiHRStruct;

#define HR_ALARM_LOW 	0x00
#define HR_ALARM_HIGH 	0x01



//运动数据结构体
typedef struct 
{
  uint16 SportIdx;
  uint32 SportTimeStamp;
  uint32 SportSteps;
  uint32 SportKcal;
  uint32 SportDistance;
  uint32 SportDuration;
}SPORT_DATA_T;

//目标设置
typedef struct 
{
  uint32	Goal;
  uint8 	MainGoalFlag;
}DAILY_GOAL;

typedef struct {
	uint8 PhoneSoundStatus;	 //1: on;  0: off
} PhoneSound_t;
typedef struct {
	uint8 PhoneMicStatus;	 //1: on;  0: off
} PhoneMic_t;

typedef enum
{
	MiSSED_CALLS, 
	DIAL_OUT, 
	DIAL_IN,
}DIAL_TYPE;

typedef enum{
	HR_OPTION = 0x00,
	STEP_OPTION,
	CALOR_OPTION,
	DURATION_OPTION,
	DISTANCE_OPTION,
}TimeMenuDisOption;

//时间界面设置结构体
typedef struct 
{
  uint8	DateFormat; 
  uint8 	TimeFormat; 
  uint8 	BatFormat; 
  uint8 	LunarFormat; 
  uint8 	RotateFormat; 
  uint8	BackgroundStyle; 
  uint8 	HeartRateFormat; 
  uint8 	UserNameFormat; 
}TIME_ORDER;



extern sleep_info_t sleep_info;
extern sleep_overview_t sleep_overview_1;
extern sleep_overview_t sleep_overview_2;
extern sleep_overview_t sleep_overview_3;
extern time_battery_info_t time_battery_info;
extern TIME_ORDER TimeOrder;

extern shortcut_t shortcut_info;
extern uint8 MusicBuf[128];
extern watch_info_t watch_info;
extern SPORT_DATA_T gSportDisplay;
extern DAILY_GOAL DailyGoal[5];
extern UTCTimeStruct SPORT_uTime;
extern UiHRStruct DispalyHrData;
extern PhoneSound_t PhoneSound;
extern PhoneMic_t PhoneMic;
extern uint8_t OtaRest;
extern uint8_t   UnknownCheck;

void ShowTime(UTCTimeStruct uUtcTime);
void ShowSportOverview(uint8 ui_index, uint8 DistanceFormat);
//void ShowSportOverview(uint8 Steps_ROP,uint8 Distance_ROP,uint8 Calor_ROP,uint8 Duration_ROP,uint8 week, uint8 day, uint8 month);
void ShowSportTotal(uint8 Steps_ROP,uint8 Distance_ROP,uint8 Calor_ROP,uint8 Duration_ROP,uint8 week, uint8 day, uint8 month);
void ShowSteps(uint32_t uSteps, uint8_t Goal_Percent);
void ShowCalor(uint32_t uCalor, uint8_t Goal_Percent);
void ShowDistance(uint32_t uDistance, uint8_t Goal_Percent, uint8 Distance_Format);
void ShowActivityTime(uint32_t min, uint8_t Goal_Percent);
void ShowChangeGoals(void);
void ShowShortCutKey(void);
void ShowBrightness(void);
void ShowUpdatedDone(void);
void ShowAppMenuOne(void);
void ShowAppMenuTwo(void);
void ShowAppMenuThree(void);
void ShowSettingMenuOne(uint8_t raise_wake_switch);
void ShowSettingMenuTwo(uint8_t Vibration, uint8 Brightness,uint8 raise_wake_switch);
void ShowSettingMenuThree(void);
void ShowAppBox(void);
 

void ShowLanguageMenuOne(void);
void ShowLanguageMenuTwo(void);
void ShowLanguageMenuThree(void);
void ShowLanguageMenuFour(void);





void ShowTimeMenuOne(uint8 Distance_Format);
void ShowTimeMenuTwo(void);
void ShowTimeMenuThree(void);
void ShowTimeMenuFour(uint8 Distance_Format);
void ShowTimeMenuFive(uint8 Distance_Format);
void ShowTimeMenuSix(void);
void RefreshTimeUIFun(uint8 DisOption,uint8 DisState,uint32 Value,uint8 TimerMenu_Index );


void ShowBlueToothOnState(void);
void ShowBlueToothOffState(void);

void ShowTimeMenuUpdated(void);
void ShowBlueToothState(void);
void ShowCurrencyType(void);
void ShowSoundState(void);

void ShowFindMe(void);
void ShowSearching(uint8 status);
void ShowMusicPlay(uint8 Vol);
void ShowMusicPause(uint8 Vol);
void ShowMusic(MusicSet Ms, uint8 Vol_Init);
void ShowCameraReady(void);
void ShowCameraAction(void);
void ShowCamera(uint8_t Ca);
void ShowSearch(uint8_t Su);
void ShowPairRequest(void);
void ShowPairSucced(void);
void ShowPaidFaid(void);
void ShowSetup(void);
void ShowBleDisconnected(void);
void ShowCharging(uint8 level, uint8 percent);
void ShowBatteryLow(void);
void ShowGoalOver(uint8 mode);
void ShowMoving(void);
void ShowCharging34x16(uint8 level, uint8 percent);
void ShowShortCut(void);

void ShowSleep(uint8 hour, uint8 minute);
void ShowSleepTimes(uint8 hour, uint8 minute, uint8 TimeHour, uint8 TimeMin, uint8 Sleep_Percent);
void ShowSleepStart(uint8 hour, uint8 minute, uint8 TimeHour, uint8 TimeMin, uint8 Sleep_Percent);
void ShowSleepEnd(uint8 hour, uint8 minute, uint8 TimeHour, uint8 TimeMin, uint8 Sleep_Percent);
void ShowSleepOverviewOne(uint8_t *data);

void ShowSleepDetail(uint8_t *data);
void ShowSleepInfoOne(void);
void ShowSleepInfoTwo(void);


void ShowLogo(void);

void ShowMenu1(uint8 hour,uint8 minute,uint8 month,uint8 day,uint8 week);
void ShowMenu2(uint8 hour,uint8 minute,uint8 month,uint8 day,uint8 week);
void ShowMenu3(void);
void ShowMenu4(void);

void ShowMenu5(uint8 hour,uint8 minute,uint8 month,uint8 day,uint8 week);
void ShowMenu6(uint8 hour,uint8 minute,uint8 month,uint8 day,uint8 week);
void ShowMenu7(void);
void ShowMenu8(void);


void ShowReset(void);
void ShowResetProgress(uint8 level);
void ShowResetFailed(void);






void ShowOtaLogo(void);
void ShowOtaLogoProgress(uint8_t prog);
void ShowOtaResult(uint8_t result);
void ShowTimerStart(uint8 minutes, uint8 seconds, uint8 millSeconds);
void ShowTimerEnd(uint8 minutes, uint8 seconds, uint8 millSeconds);
void ShowTimerDisplay(uint8 minutes, uint8 seconds, uint8 millSeconds, uint8 End_hour, uint8 End_Minutes);
void ShowAncsSend(uint8_t type);
void ShowAncsReply(uint8_t type);
void ShowSwitchState(uint8 state);
void ShowWatchInfo(void);
void ShowUserRemind(uint8* buf);
void ShowRemind(uint8 ui_index);

void ShowPaymentCodeInterface(void);
void ShowPaymentCodeNumber(uint8* Num);

void ShowPaySetUp(void);
void ShowPayProgress(void);
void ShowPaymentLock(void);
void ShowPaymentUnlock(void);
void ShowPaymentProgress_55x69(uint8 Posx,uint8 Posy,uint8 level);
void ShowPaymentAnimation1(void);
void ShowPaymentAnimation2(void);
void ShowPaymentConsume(void);
void ShowPaymentBalance(void);
void ShowPaymentLog(void);


void ShowLcdTest(uint8_t ui_index);
void ShowTpTest(uint8_t t_xy, uint8_t t_id);
void ShowPhoneCall(uint8_t* PStr);
void ShowPhoneCallTime(uint16_t PTime);
void ShowMicstatus(uint8 MicStatus);
void ShowPhoneSoundStatus(uint8 SoundStatus);

void ShowVibrateOnState(void);
void ShowVibrateOffState(void);
void ShowWristState(uint8 state);
void ShowWristStateChange(uint8 state);


void ShowHeartRate(UiHRStruct gDispalyHrData);
void ShowHRAlarm(uint8 HR_Value,uint8 HR_Alarm_Value);
void ShowHRAlarm_Without_Cls(uint8 HR_Value,uint8 HR_Alarm_Value);

void ShowHRStart(uint8 Mode);

void ShowChargingback(void);
void DisplayStringNumber9x16(char * str, uint8 x, uint8 y);
void DisplayStringNumber50x72(char * str, uint8 x, uint8 y);
void DisplayStringNumberWhite(uint8 Type, char * str, uint8 x, uint8 y);
void ShowBarWithoutClear(uint8 type, uint8 mode, uint8 Posx,uint8 Posy,uint8 level);
void ShowBar(uint8 type, uint8 mode, uint8 Posx,uint8 Posy,uint8 level);
void ShowPageSchedule(uint8 TotlePageNum, uint8 PageNum,uint8 Posx_Center, uint8 Posy);


void ShowVolume(void);	
void ShowVolumeLevel(uint8 level);						

void ShowBTPairing(void);
void ShowBTPairingSucced(void);
void ShowBTPairingFaid(void);

void ShowDailInterface(void);
void ShowDailNumber(uint8_t* Num);
void ShowDailInterface_Incall(void);
void ShowDailNumber_Incall(uint8_t* Num);

void ShowDailCalling_1(void);
void ShowDailCalling_2(uint8 MicStatus);

void ShowDailCallEnded(void);
void ShowDailIncomeCall_1(uint8 SoundStatus);
void ShowDailIncomeCall_2(uint8 MicStatus);

void ShowCallHistory(char* CalledList);
void ShowCallHistoryDetial(char* CalledList);
void ShowAddressBook(char *str, uint8 Inf_Cnt, uint8_t Percent_Showed);



void ShowSiri(uint8 su);
void ShowSiriSwitch(void);
void ShowSiriOnGoing(uint8 status);
void ShowSyncingIcon(void);
void DisplayPhoneBook(uint32_t PhBInf_Read_Addr,uint8_t   Inf_Index ,uint32_t PhBGrp_Read_Addr,uint8_t   Grp_Index, uint8_t Percent_Showed);
void DisplayPhonePreparek(uint32_t Paddr,uint8_t NameLen,uint8_t NumLen, uint8_t*Num,uint8_t IsAndroid);


void ShowYellowdate(uint8 month,uint8 day);
void ShowClearAll(void);
void ShowBatteryFull(void);

void ShowDailInterface_FadingIncall(uint8_t Num, bool Is_Fade);
void ShowDailInterface_Fadingcall(uint8_t Num, bool Is_Fade);
void ShowDailInterfaceManage(uint8_t Num, bool Is_Fade);
void ShowDailInterfaceManage_Incall(uint8_t Num, bool Is_Fade);

void ShowTest(void);

void ShowMultipleLanTypeOne(
							uint8 LanType1,const uint16 *val_1,uint8 Len_1,uint8 x1,uint8 y1,
							uint8 LanType2,const uint16 *val_2,uint8 Len_2,uint8 x2,uint8 y2,
							uint8 LanType3,const uint16 *val_3,uint8 Len_3,uint8 x3,uint8 y3,
							uint8 LanType4,const uint16 *val_4,uint8 Len_4,uint8 x4,uint8 y4);

void ShowMultipleLanTypeTwo(uint8 LanType,
							uint16 *val_1,uint8 Len_1,uint16 *val_12,uint8 Len_12,
							uint16 *val_2,uint8 Len_2,uint16 *val_22,uint8 Len_22,
							uint16 *val_3,uint8 Len_3,uint16 *val_32,uint8 Len_32,
							uint16 *val_4,uint8 Len_4,uint16 *val_42,uint8 Len_42);

void ShowMultipleLanTypeThree(	uint8 LanType,
							const uint16 *val_1,uint8 Len_1,
							const uint16 *val_12,uint8 Len_12,
							const uint16 *val_2,uint8 Len_2,
							const uint16 *val_22,uint8 Len_22,
							const uint16 *val_3,uint8 Len_3,
							const uint16 *val_32,uint8 Len_32,
							const uint16 *val_4,uint8 Len_4,
							const uint16 *val_42,uint8 Len_42);


void ShowMultipleLanTypeFour(
							uint8 LanType1,const uint16 *val_1,uint16 color1,uint8 Len_1,uint8 x1,uint8 y1,
							uint8 LanType2,const uint16 *val_2,uint16 color2,uint8 Len_2,uint8 x2,uint8 y2);


void ShowPowerOffLogo(void);


void ShowMainSport(void);
void ShowMainHeartrate(void);
void ShowMainSleep(void);
void ShowMainCamera(void);
void ShowMainMusic(void);
void ShowMainNotification(void);
void ShowMainApps(void);
void ShowMainMenuTimeOne(void);

void ShowMainMenuTopTime(void);

void ShowAppsMenuOne(void);
void ShowAppsMenuTwo(void);
void ShowAppsMenuThree(void);

void ShowNotificationMenuOne(uint8 MisscallCnt, uint8 SmsCnt, uint8 EmailCnt);
void ShowNotificationMenuTwo(uint8 SocialCnt, uint8 ScheduleCnt);
void ShowMemoryFull(void);
void ShowRestart(void);
void ShowRemindList(uint8 *buf);
void ShowTimer(uint8 status, uint8 TimerMinutes, uint8 TimerSeconds, uint8 TimerMillSeconds);
void ShowVibrateLevel(uint8 level);
void ShowCameraCountDown(uint8 ca);
void ShowBatteryInfo(uint8 level,uint8 pecent);
void ShowMainRecordWorkouts(void);

void ShowRecordgo(void);
void ShowRecordgo(void);
void ShowRecordWorkoutsRUN(uint8 status,uint16 TimerMinutes, 
							uint8 TimerSeconds, uint8 TimerMSec);
void ShowRecordResult(uint8 ui_index, uint8 DistanceFormat,uint8 Hr_value,
							uint32 uSteps,uint32 uDistance,
							uint32 uCalor,uint32 TimerMSec);

void ShowEarnedMeadalRemind(uint8 level);
void ShowSocialClass(uint8 Page,uint8 c0,uint8 c1, uint8 c2);
void ShowNoNewNotifications(void);
void ShowNoNewReminders(void);
void ShowExitSleepMode(void);
void ShowDeleteAncs(void);
void ShowExitRecordWorkouts(void);
void ShowExitSleep(void);
#endif


