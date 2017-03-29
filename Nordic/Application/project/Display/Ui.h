#ifndef _UI_H
#define _UI_H

#include "Res.h"
#include "Typedef.h"

//**********************************************
#ifdef   __cplusplus 
extern   "C "   { 
#endif 
//**********************************************

extern uint8 G_HOUR_FORMAT;
extern uint8 display_index;
extern uint8 STflag;
extern uint8 BleDisable;
	
typedef enum 
{
    AM_FORMAT = 0,
    PM_FORMAT
}APMformat;

typedef enum{
	HR_OPTION = 0x00,
	STEP_OPTION,
	CALOR_OPTION,
	DURATION_OPTION,
	DISTANCE_OPTION,
}TimeMenuDisOption;


typedef enum 
{
    M_PAGE = 0,
    M_NO_MUSIC,
    M_PLAY,
    M_STOP,
    M_NEXT_PREVIOUS,
    M_BACK
}MusicUI;

typedef enum 
{
    C_PAGE = 0,
    C_CONTROL,
    C_CONTROL_SUCCESS
}CameraUI;

typedef enum 
{
    S_PAGE = 0,
    S_U1,
    S_U2,
    S_U3,
    S_U4,
    S_U5,
    S_U6,
    S_U7
}SearchUI;


typedef struct  shortcut_s{
	uint8 find_me;					//1: on;  0: off
	uint8 airplane_mode;			//1: on; 0: off
	uint8 vibration_level;			//0~100
	uint8 battery_level;
	uint8 battery_percent;			
	uint8 brightness_level;			//level0~level10
	uint8 BC5_Connect;
} shortcut_t;

typedef struct {
	uint8 PhoneSoundStatus;	 //1: on;  0: off
} PhoneSound_t;

typedef struct {
	uint8 PhoneMicStatus;	 //1: on;  0: off
} PhoneMic_t;

typedef struct  setting_info_s{
	uint8 sounds;
	uint8 wrist;
} setting_info_t;


typedef struct watch_info_s{
	uint8 WatchId[20];
	uint8 version[3];
}watch_info_t;

extern shortcut_t shortcut_info;
extern sleep_info_t sleep_info;
extern setting_info_t setting_info;
extern sleep_overview_t sleep_overview_1;
extern sleep_overview_t sleep_overview_2;
extern sleep_overview_t sleep_overview_3;
extern sleep_overview_t sleep_overview_4;
extern sleep_overview_t sleep_overview_5;
extern sleep_overview_t sleep_overview_6;
extern sleep_overview_t sleep_overview_7;





extern watch_info_t watch_info;
extern uint32 balance;
extern PhoneSound_t PhoneSound;
extern PhoneMic_t PhoneMic;


#define BATTERY_CHARGE_LEVEL_0			0x00
#define BATTERY_CHARGE_LEVEL_1			0x01
#define BATTERY_CHARGE_LEVEL_2			0x02
#define BATTERY_CHARGE_LEVEL_3			0x03
#define BATTERY_CHARGE_LEVEL_4			0x04
#define BATTERY_CHARGE_LEVEL_5			0x05

#define MOTOR_LEVEL_STRONG		        100L
#define MOTOR_LEVEL_STANDARD		    75L
#define MOTOR_LEVEL_WEAK		        50L
#define MOTOR_LEVEL_OFF		            0L


int CalclateWeeks(int year, int month, int day);
void ShowStandby(UTCTimeStruct* uUtcTime, uint8 type);		/*To KL17*/
void RefreshTimeUIFun(uint8 TimerMenu_Index);								/*To KL17*/
void ShowMusicControl(uint8 Mu);							/*To KL17*/	
void ShowMusic(uint8 Mu, bool ForceRefresh);					/*To KL17*/
void ShowCamera(uint8 Ca);									/*To KL17*/	
void ShowSearch(uint8 Su);									/*To KL17*/	
void ShowSearching(uint8 Su);								/*To KL17*/	
void ShowToolApps(uint8 index);							/*To KL17*/
void ShowSettingApps(uint8 index);							/*To KL17*/
void ShowAppBox(void);
void ShowLanguageApps(uint8 index);

void ShowBluetoothOnApps(void);						/*To KL17*/
void ShowBluetoothOffApps(void);						/*To KL17*/	
void ShowCurrencyApps(void);							/*To KL17*/
void ShowTimerMenu(uint8 index);							/*To KL17*/
void ChooseLanguage(uint8 index);						/*To KL17*/
void ShowTimerMenuUpdated(void);						/*To KL17*/
void ShowSoundApps(void);		 						/*To KL17*/
void ShowTool(uint8 index);									/*To KL17*/
void ChangeGoals(void);									/*To KL17*/
void ShowSportTotal(void);
void ShowActivityDuration(uint32 uActDiration);				/*To KL17*/
void ShowSteps(uint32 uSteps);								/*To KL17*/
void ShowCalor(uint32 uCalor);								/*To KL17*/
void ShowDistance(uint32 uDistance);						/*To KL17*/
void ShowOtaLogo(void); 									/*To KL17*/
void ShowOtaResult(uint8 flag, uint8 result); 					/*To KL17*/
void ShowAncsSend(uint8 type);	 						/*To KL17*/
void ShowSmsReply(uint8 type); 							/*To KL17*/
void ShowLcdTest(uint8 Lcd);								/*To KL17*/
void ShowTpTest(uint8 t_xy, uint8 t_id);						/*To KL17*/
void ShowSportOverview(uint8 ui_index);							/*To KL17*/	
void ShowBtConnect(void);								/*To KL17*/
void ShowBtPairSuccess(void);							/*To KL17*/
void ShowBtPairFail(void);									/*To KL17*/
void ShowVolume(void);									/*To KL17*/
void ShowVolumeLevel(int level);							/*To KL17*/
void ShowSiri(uint8 State);									/*To KL17*/
void ShowPhoneDial(void);									/*To KL17*/
void ShowPhoneDialNumber(uint8* number);				/*To KL17*/
void ShowPhoneDial_Incall(void);									/*To KL17*/
void ShowPhoneDialNumber_Incall(uint8* number);				/*To KL17*/

void ShowSleepMonitor(uint16 quality, uint16 Minute , uint8 seconds, uint8 waiting, uint8 SleepOrNot);
void ShowBalances(uint32 Balances);
void ShowCardID(uint8 * CardID);

void ShowIncomingcallIcon(void);
void ShowNewMsg(uint16 uCount);
void ShowMissedCall(uint16 count);
void ShowE_MailMsg(uint16 count);
void ShowDailyScheduleMsg(uint16 count);
void ShowSocialMsg(uint16 count);


void ShowSecondsTimer(SecondTimerUI STu);
void ShowBack(void);

void ShowSetUp(void);
void ShowLogo(void);
void ShowBleBondQuery(void);
void ShowBleBondOk(void);
void ShowSleepStart(void);
void ShowSleepEnd(uint32 min) ;
void ShowWakeup(void);
void ShowSleep(uint32 min);
void ShowMedicine(void);
void ShowEat(void);
void ShowMoving(void);
void ShowMeeting(void);
void ShowUserRemind(void);
void showUpgrade(void);
void showUpgradeSuccess(void);
void showUpgradeFail(void);
void ShowSyncingIcon(void);
void ShowBtDisconnected(void);
void ShowMemFull(void);
void ShowGoalOver(void);
void DispalyFactorySetLogo(void);
void ShowLEDTime(void);
void ShowInactAlert(void);
void ShowHello(void);
void ShowDevicesCom(void);
void ShowDevicesSea(void);
void ShowDevicesNone(void);
void ShowDevicesAdded(void);
void ShowDevicesNonAdded(void);
void ShowSendRequest(void);
void ShowAddDevicesRecSuccess(void);
void ShowAddDevicesRecFail(void);
void ShowAddDevicesSuccess(void);
void ShowEmotionStart(void);
void ShowDevicesMotoRepeat(void);
void ShowEmotionOK(void);
void ShowSendEmotionRepeat(void);
void ShowEmotionHomeMenu(void);
void ShowEmotionIdex(uint8 idx);
void ShowEmotionOne(void);
void ShowEmotionTwo(void);
void ShowEmotionThree(void);
void ShowEmotionFour(void);
void ShowEmotionFive(void);
void ShowDevDel(void);
void ShowDevicesDel(void);
void ShowDevicesDeleting(void);
void ShowDevicesDelSuccess(void);
void ShowDevicesDelFail(void);

void ShowStatus(uint8 scene_mode, uint8 sleep_mode, uint8 memory_full, uint8 bc5_connect,uint8 ble_connect);
void ShowBtConnecting(void);
void DisplayEnString(const uint8 *pCn,uint8 x,uint8 y);
void DisplaySecondTimerTime(uint8 Minutes,uint8 Seconds,uint8 mSeconds,uint8 x,uint8 y,uint8 Sy);
void ShowOnBoldIcon(void);
void ShowOffBlodIcon(void);
void ShowOnIcon(void);
void ShowOffIcon(void);
void ShowReceivedReq(void);
void ShowAddDevicesFail(void);
void ShowSerialNum(void);
void ShowBleBondFail(void);
void ShowCharging(uint8 level, uint8 percent);
void ShowBatteryLow(void);
void ShowShortCut(uint8 flags, bool flash_write);
void ShowBrightness(uint8 flags,  bool flash_write);
void SetBrightness(uint8 Brightness);
void ShowUpdateDone(void);

void ShowSleepOverviewOne(void);
void ShowSleepOverviewTwo(void);
void ShowSleepOverviewThree(void);
void ShowSleepDetail(uint8 Which_date);
void ShowSleepInfoOne(void);
void ShowSleepInfoTwo(void);


void ShowReset(void);
void ShowResetProgress(uint8 level);
void ShowTimerStatus(uint8 status, uint8 minutes, uint8 seconds, uint8 millSeconds);
void ShowSwitchState(uint8 state, bool flash_write);
void ShowWatchInfo(void);
void ShowSports(void);
void ShowSleeping(void);
void ShowPaymentLock(void);
void ShowPaymentUnlock(void);
void ShowPaymentAnimation1(void);
void ShowPaymentAnimation2(void);
void ShowPaymentConsume(void);
void ShowPaymentBalance(void);
void ShowPaymentLog(void);
void ShowVirateOnApps(void);	
void ShowVirateOffApps(void);		
void ShowWristOnApps(void);
void ShowWristOffApps(void);
void ShowWristStateChange(uint8 state);

void ShowHeartRate(UiHRStruct gDispalyHrData);
void ShowHRAlarm(UiHRStruct gDispalyHrData,uint8 HR_Alarm_Value);
void ShowHRAlarm_WithoutClean(UiHRStruct gDispalyHrData,uint8 HR_Alarm_Value);

uint8 CalcAccumulateTime(uint8* A_TotalMin, UTCTimeStruct uUtcTime_C,UTCTimeStruct uUtcTime_L);

void CalculatePercent(uint32_t Values, uint8 Goal_Index, uint8 *Ptr_Percent);
void UpdateBleStatus(void);
void ShowPaySetUp(void);
void ShowPayProgress(void);
void ShowClearAll(void);
void ShowBatteryFull(void);
void ShowPhoneDialNumber_Recover(uint8 number);
void ShowPhoneDialNumber_Fade(uint8 number);
void ShowPhoneDialIncallNumber_Recover(uint8 number);
void ShowPhoneDialIncallNumber_Fade(uint8 number);

void RefreshBatteryInfo(void);
void ShowMainMenu(uint8 index);
void ShowAppsMenu(uint8 index);
void ShowNotificationMenu(uint8 index);
void ShowRemindList(REMIND_DATA_T* RDTableTemp);
void ShowRestart(void);
void ShowVirateLevel(bool flash_write);
void ShowCameraCountDown(uint8 Ca);
void ShowBatteryInfo(void);

void ShowRecordgo(void);
void ShowRecordWorkoutsRUN(uint8 status,uint16 TimerMinutes, uint8 TimerSeconds, uint8 TimerMSec);
void ShowRecordResult(uint8 ui_index, uint8 DistanceFormat,uint8 Hr_value,
							uint32 uSteps,uint32 uDistance,
							uint32 uCalor,uint32 TimerMSec);
void ShowEarnedMeadalRemind(uint8 level);
void ShowSocialClass(uint8 page);
void ShowNoNewNotifications(void);
void ShowNoNewReminders(void);
void ShowExitSleepMode(void);
void ShowDeleteAncs(void);
void ShowExitRecordWorkouts(void);
void ShowExitSleep(void);

//**********************************************
#ifdef __cplusplus 
} 
#endif 
//*
#endif
