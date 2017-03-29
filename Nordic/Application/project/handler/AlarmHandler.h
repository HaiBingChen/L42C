#include "Typedef.h"
#include "AppHandler.h"


#define INCOMING_CALL_SKIP_SMS				0x01
#define INCOMING_CALL_SKIP_MISSED_CALL		0x02
#define INCOMING_CALL_SKIP_EMAIL				0x04
#define INCOMING_CALL_SKIP_SCHEDULE			0x08
#define INCOMING_CALL_SKIP_SOCIAL				0x10

typedef enum{
	ANCS_INIT = 0x00,
	ANCS_FIRST_PAGE,
	ANCS_NEXT_PAGE,	
	ANCS_RESUME,
	ANCS_END
}AncsStatus;

typedef enum{
	READ_BEGIN = 0x00,
	READING,	
	READ_FINISHED
}AncsReadStatus;


typedef enum{
	TITLE_DISPLAY = 0x00,
	TITLE_END,	
	MESSAGE_DISPLAY,
	MESSAGE_END,
	DATE_DISPLAY,
	DATE_END
}SMSTypeStatus;

typedef enum{
	PAGE_1ST = 0x00,
       PAGE_1ST_TITLE_NEED_ROLL,
	PAGE_NEXT
}SMSPageStatus;

typedef struct 
{
	int		AncsSendHandlerId; 
	int		SMSHomePageHandlerId; 	
	int		SMSHandlerId; 
	int		MissedCallHomePageHandlerId; 	
	int		MissedCallAlertHandlerId; 
	int		PhoneRecordHandlerId; 	
	int		IncomingCallAlertHandlerId;	
	int		E_MailHomePageHandlerId; 		
	int		E_MailHandlerId; 
	int		DailyScheduleHomePageHandlerId; 	
	int		DailyScheduleHandlerId; 
	int		SocialClassHandlerId; 		
	int		SocialClassHomeHandlerId; 	
	int		SocialHomePageHandlerId; 
	int		SocialMessageHandlerId; 
	int		BtBleDisconnectHandlerId; 
	int		RemindHandler; 
	int		SetupAlertHandlerId;   
	int		MemoryFullHandlerId;   
	int		SitRemindHandlerId;
	int		BleBondChooseHandlerId;
	int		ToolAppsHandlerId;
	int		SettingHanderId;
	int		AppBoxHanderId;
	int		LanguageHandlerId;
	int		ToolHandlerId;
	int		MusicHandlerId;	
	int		BluetoothHandlerId;
	int		CurrencyHandlerId;
	int		TimerMenuHandlerId;
	int		SoundHandlerId;
	int		CameraHandlerId;
	int		SearchHandlerId;
	int		TimerHandlerId;
	int		BackHandlerId;
	int		UpdateFontLibHandlerId;
	int		TpTestHandlerId;	
	int		SerialNumHandlerId;	
	int		ShortCutHandlerId;
	int		BrightnessHandlerId;
	int		ResetHandlerId;
	int		WristWakeUpHandlerId;
	int		WatchInfoHandlerId;
	int		VirateHandlerId;
	int		heartRate_HandlerId;
	int		HrRemindHandlerId;
	int		SearchingHandlerId;
	int		OtaHandlerId;
	int		PhoneHandlerId;
	int		ClearAllHandlerId;
	int		SportMmenuHandlerId;	
	int		HRMmenuHandlerId;
	int		RecordWorkoutsMmenuHandlerId;
	int		RecordWorkHandlerId;
	int		RecordWorkDetailHandlerId;		
	int		SleepMmenuHandlerId;
	int		CameraMmenuHandlerId;
	int		MusicMmenuHandlerId;
	int		NotificationMmenuHandlerId;
	int		AppsMmenuHandlerId;	
	int		AppsBoxHanderId;
	int		NotificationBoxHandlerId;
	int		RestartHandlerId;
	int		RemindViewHandlerId;
	int		BatteryHandlerId;
	int		MedalHandlerId;
	int		NoNewNotificationHandlerId;
	int		DeleteAncsHandlerId;
	int		NoReminderHandlerId;
	int		ExitsleepmodeHandlerId;
	int		ExitRecordWorkOutsHandlerId;
	
} RemindIdTypeTAG;
typedef enum 
{
    ST_PAGE = 0,
    ST_RUN,
    ST_STOP,
}SecondTimerUI;

typedef enum{
	LAN_ENGLISH= 0x00,
	LAN_CHINESE_SIMPLIFID,
	LAN_CHINESE_TRADITIONAL,
	LAN_KOREAN,
	LAN_THAI,
	LAN_JAPANESE,
	LAN_ESPANOL,
	LAN_FRANCAIS,
	LAN_DEUTSCH,
	LAN_ITALIANO,
	LAN_POLSKI,
	LAN_PORTUGUESE,
	LAN_RUSSIAN,
	LAN_DUTCH,
}LanguageType;


extern RemindIdTypeTAG DifferentRemindId; 
extern SecondTimerUI STui;
extern uint8 IsLeftRight;
typedef struct 
{
        int  powerstate;				/*开机: 1     关机: 0*/
        int  standbystate;			/* Normal : 1     待机: 0 */
        int  displaystate;				/* 打开: 1     关闭: 0*/
        int  InterruptType;			/*按键中断: 1    其它类型: 0*/
        int  chargeinterface;			/*开机界面: 1     关机界面: 0*/
        int  chargepowerstate;		/*开机充电: 1/2/3     关机充电: 0*/
        int  bc5state;				/* BC5的状态*/
} PowerStateContext;


extern   PowerStateContext Powerstate;

uint16 AncsSendHandler(MsgType msg, int iParam, void *pContext);
uint16 SMSHomePageHandler(MsgType msg, int iParam, void *pContext);
uint16 SMSHandler(MsgType msg, int iParam, void *pContext);
uint16 MissedCallHomePageHandler(MsgType msg, int iParam, void *pContext);
uint16 MissedCallAlertHandler(MsgType msg, int iParam, void *pContext);
uint16 PhoneRecordHandler(MsgType msg, int iParam, void *pContext);
uint16 IncomingCallHandler(MsgType msg, int iParam, void *pContext);
uint16 E_MailHomePageHandler(MsgType msg, int iParam, void *pContext);
uint16 E_MailHandler(MsgType msg, int iParam, void *pContext);
uint16 DailyScheduleHomePageHandler(MsgType msg, int iParam, void *pContext);
uint16 DailyScheduleHandler(MsgType msg, int iParam, void *pContext);
uint16 SocialHomePageHandler(MsgType msg, int iParam, void *pContext);
uint16 SocialMessageHandler(MsgType msg, int iParam, void *pContext);
uint16 BtBleDisconnectHandler(MsgType msg, int iParam, void *pContext);
uint16 RemindHandler(MsgType msg, int iParam, void *pContext);
uint16 SetupAlertHandler(MsgType msg, int iParam, void *pContext);
uint16 MemmoryFullHandler(MsgType msg, int iParam, void *pContext);
uint16 SitRemindHandler(MsgType msg, int iParam, void *pContext);
uint16 HrRemindHandler(MsgType msg, int iParam, void *pContext);
uint16 BleBondChooseHandler(MsgType msg, int iParam, void *pContext);
uint16 UpdateFontLibHandler(MsgType msg, int iParam, void *pContext);
uint16 TpTestHandler(MsgType msg, int iParam, void *pContext);
uint16 ToolAppsHandler(MsgType msg, int iParam, void *pContext);
uint16 ToolHandler(MsgType msg, int iParam, void *pContext);
uint16 BluetoothHandler(MsgType msg, int iParam, void *pContext);
uint16 CurrencyHandler(MsgType msg, int iParam, void *pContext);
uint16 TimerMenuHandler(MsgType msg, int iParam, void *pContext);
uint16 SoundHandler(MsgType msg, int iParam, void *pContext);
uint16 MusicHandler(MsgType msg, int iParam, void *pContext);
uint16 CameraHandler(MsgType msg, int iParam, void *pContext);
uint16 SearchHandler(MsgType msg, int iParam, void *pContext);
uint16 SearchingHandler(MsgType msg, int iParam, void *pContext);
uint16 TimerHandler(MsgType msg, int iParam, void *pContext);
uint16 SerialNumHandler(MsgType msg, int iParam, void *pContext);
uint16 ShortCutHandler(MsgType msg, int iParam, void *pContext);
uint16 BrightnessHandler(MsgType msg, int iParam, void *pContext);
uint16 WristWakeUpHandler(MsgType msg, int iParam, void *pContext);
uint16 WatchInfoHandler(MsgType msg, int iParam, void *pContext);
uint16 VirateHandler(MsgType msg, int iParam, void *pContext);
uint16 OtaHandler(MsgType msg, int iParam, void *pContext);


uint16 SettingHandler(MsgType msg, int iParam, void *pContext);
uint16 AppBoxHandler(MsgType msg, int iParam, void *pContext);
//static uint16 LanguageHandler(MsgType msg, int iParam, void *pContext);

uint16 BackHandler(MsgType msg, int iParam, void *pContext);
bool SmsQueryCharDotCB(uint8 *pData);
uint16 PhoneBookHandler(MsgType msg, int iParam, void *pContext);
uint16 ClearAllHandler(MsgType msg, int iParam, void *pContext);

uint16 SportMmenuHandler(MsgType msg, int iParam, void *pContext);
uint16 HRMmenuHandler(MsgType msg, int iParam, void *pContext);
uint16 RecordWorkoutsMmenuHandler(MsgType msg, int iParam, void *pContext);
uint16 SleepMmenuHandler(MsgType msg, int iParam, void *pContext);
uint16 CameraMmenuHandler(MsgType msg, int iParam, void *pContext);
uint16 MusicMmenuHandler(MsgType msg, int iParam, void *pContext);
uint16 NotificationMmenuHandler(MsgType msg, int iParam, void *pContext);
uint16 AppsMmenuHandler(MsgType msg, int iParam, void *pContext);
uint16 AppsBoxHandler(MsgType msg, int iParam, void *pContext);
uint16 NotificationBoxHandler(MsgType msg, int iParam, void *pContext);
uint16 RestartHandler(MsgType msg, int iParam, void *pContext);
uint16 RemindViewHandler(MsgType msg, int iParam, void *pContext);
uint16 BatteryHandler(MsgType msg, int iParam, void *pContext);
uint16 RecordWorkHandler(MsgType msg, int iParam, void *pContext);
void SaveRecordWork(void);
uint16 RecordWorkDetailHandler(MsgType msg, int iParam, void *pContext);
uint16 MedalHandler(MsgType msg, int iParam, void *pContext);
uint16 NoNewNotificationHandler(MsgType msg, int iParam, void *pContext);
uint16 NoReminderHandler(MsgType msg, int iParam, void *pContext);
uint16 ExitsleepmodeHandler(MsgType msg, int iParam, void *pContext);
uint16 DeleteAncsHandler(MsgType msg, int iParam, void *pContext);
uint16 ExitRecordWorkOutsHandler(MsgType msg, int iParam, void *pContext);
typedef enum{
	OVER_VIEW= 0x00,
	PRE_VIEW,
	DETAIL_VIEW,
	REPLAY_VIEW1,
	REPLAY_VIEW2,
	REPLAY_VIEW3,
	EMOTION_VIEW1,
	EMOTION_VIEW2,
}AncsView;

typedef enum{
	SENDING_VIEW=0x00,
	SEND_OK_VIEW,
	SEND_FAIL_VIEW,
}AncsSendView;

typedef struct 
{
	AncsView View;
	bool UpdateOverview;
	uint8 PreViewIndex;
	uint8 DetailViewIndex;
} AncsHandler;

typedef enum{
	Clear_Non = 0x00,
	Clear_IncomingCall,
	Clear_MissedCall,
	Clear_Sms,
	Clear_Socail,
	Clear_Email,
	Clear_PhoneRecord,
}AncsClear;


extern uint8 ToolIdex;
extern uint8 TimerMenuFlag;
