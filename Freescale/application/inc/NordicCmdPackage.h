#ifndef _NORDICCMDPACKAGE_H
#define _NORDICCMDPACKAGE_H

#include "spi.h"

#define SPI_RSP_SUCCED					0x00
#define SPI_RSP_FAILED					0x01
#define SPI_RSP_DATA					0x02
#define SPI_RSP_STATUS					0x03

#define CCID_STATUS						0x80
#define CCID_ERR_BLK_NO_BLOCK			0x81
#define CCID_ERR_BLK_ERR_SEQUENCE		0x83
#define CCID_ERR_BLK_RTT_LENGTH		0x85
#define CCID_ERR_BLK_ERR_CCID_LEN 		0x86
#define CCID_ERR_CMD_SLOT_BUSY 		0xE0
#define CCID_ERR_ICC_MUTE 				0xFE

#define CCID_SLOT_STATUS_FAILED 		(1 << 6)

#define POWER_SUSPEND					0x01
#define POWER_RESUME					0x02

typedef union 
{
  uint16_t u16DMS;
  struct {
	uint16_t DMS0:1;
	uint16_t DMS1:1;
	uint16_t DMS2:1;
	uint16_t DMS3:1;
	uint16_t DMS4:1;
	uint16_t DMS5:1;
	uint16_t DMS6:1;
	uint16_t DMS7:1;
	uint16_t DMS8:1;
	uint16_t DMS9:1;
	uint16_t DMS10:1;
	uint16_t DMS11:1;
	uint16_t DMS12:1;
	uint16_t DMS13:1;
	uint16_t DMS14:1;
	uint16_t DMS15:1;
    }DMS;
}DISPLAY_STATUS_T;  

typedef enum 
{
	P_SETUP = 0,
	P_LOGO,
	P_BOND_QUERY,
	P_BOND_OK,
	P_BOND_FAIL,
	P_BLE_DISCONNECTED,
	P_GOAL_OVER,
	P_MOVE_NOW,
	P_TOOL_APP1,
	P_TOOL_APP2,
	P_TOOL_APP3,
	P_UPDATE_DONE,
	P_SETTING_APP1,
	P_SETTING_APP2,
	P_SETTING_APP3,
	P_APP_BOX,
	P_BLUETOOTH_ON_STATE,
	P_BLUETOOTH_OFF_STATE,
	P_CURRENCY_TYPE,
	P_SOUND_STATE,
	P_RESET,
	P_OTA_LOGO,
	P_TIMER_MENU_ONE,
	P_TIMER_MENU_TWO,
	P_TIMER_MENU_THREE,
	P_TIMER_MENU_FOUR,
	P_TIMER_MENU_FIVE,
	P_VIRATE_ON_STATE,
	P_VIRATE_OFF_STATE,
	P_WRIST_ON_STATE,
	P_WRIST_OFF_STATE,
	P_TIMER_MENU_UPDATED,
	P_BT_PAIRING,
	P_BT_PAIRING_SUCCESS,
	P_BT_PAIRING_FAIL,
	P_VOL,
	P_DIAL,
	P_DIAL_INCALL,
	P_SYNCING,
	P_PAY_SET_UP,
	P_PAY_PROGRESS,
	P_LANGUAGE_APP1,
	P_LANGUAGE_APP2,
	P_LANGUAGE_APP3,
	P_LANGUAGE_APP4,
	P_LANGUAGE_CHOOSE_ONE,
	P_LANGUAGE_CHOOSE_END = P_LANGUAGE_CHOOSE_ONE+14,
	P_CLEARALL,
	P_BATTERY_FULL,
	P_APPS_MENU_ONE,
	P_APPS_MENU_TWO,
	P_APPS_MENU_THREE,
	P_NO_NEW_NTIFICATIONS,
	P_NO_REMINDERS,
	P_EXITSLEEPMODE,
	P_DELETE_ANCS,
	P_EXITWORKOUTS,
	P_EXITSLEEP,
}PictureUI;

typedef struct 
{
  uint8_t 	Style;
  uint8_t 	TimeFormat;   
  uint8_t	DateFormat;
}Format;

typedef enum{
    F_SECTOR,
    F_BLOCK,
    F_CHIP
}FlashErase;

typedef enum{
	MI_TIME,	
	MI_TOOL,	
	MI_CHANGE_GOALS,
	MI_ACT_DIRATION,
	MI_SPORT,
	MI_CAL,
	MI_DISTANCE,
	MI_CHARGING,
	MI_LOW_BATTERY,
	MI_SHORTCUT,
	MI_BRIGHTNESS,
	MI_BRIGHTNESS_SET,
	MI_MUSIC,
	MI_MUSIC_CONTROL,
	MI_CAMERA,
	MI_SEARCH,
	MI_SLEEP_CHANGE,
	MI_SLEEP_TIME,
	MI_SLEEP_START,
	MI_SLLEP_END,
	MI_SLEEP_OVERVIEW_ONE,
	MI_SLEEP_OVERVIEW_TWO,
	MI_SLEEP_OVERVIEW_THREE,
	MI_SLEEP_DETAIL,
	MI_SLEEP_INFO_ONE,
	MI_SLEEP_INFO_TWO,
	MI_RESET_PROGESS,
	MI_OTA_RESULT,
	MI_TIMER_CONTROL,
	MI_SWITCH_ON_OFF,
	MI_ANCS_SEND,
	MI_ANCS_REPLY,
	MI_REMIND_INDEX,
	MI_PAYMENT,
	MI_LCD_TEST,
	MI_TP_TEST,
	MI_PHONE_CALL,
	MI_PHONE_CALL_TIME,		
	MI_WATCH_INFO,
	MI_HR,
	MI_HR_ALARM,
	MI_HR_ALARM_WITHOUT_CLS,
	MI_SPORT_OVER_VIEW,
	MI_SPORTTOTAL,
	MI_VOL_LEV,
	MI_SIRI_SWITCH,
	MI_DIAL_NUM,
	MI_DIAL_NUM_INCALL,
	MI_PHONE_SOUND_STATUS,
	MI_PHONE_Mic_STATUS,
	MI_SEARCHING,
	MI_SMS_FROM_ANDROID,
	MI_DIAL_NUM_FADE,
	MI_DIAL_NUM_RECOVER,
	MI_DIAL_INCALL_NUM_FADE,
	MI_DIAL_INCALL_NUM_RECOVER,
	MI_WRIST_STATE_CHANGE,
	MI_MAIN_MENU_ONE,
	MI_MAIN_MENU_TWO,
	MI_MAIN_MENU_THREE,
	MI_MAIN_MENU_FOUR,
	MI_MAIN_MENU_FIVE,	
	MI_MAIN_MENU_SIX,
	MI_MAIN_MENU_SEVEN,
	MI_MAIN_MENU_EIGHT,
	MI_BATTERY_FULL,
	MI_SETUP,
	MI_BOND_QUERY,	
	MI_BOND_OK,
	MI_BOND_FAIL,
	MI_BLE_DISCONNECTED,
	MI_MOVE_NOW,
	MI_GOAL_OVER,
	MI_SYNCING,
	MI_NOTIFICATION_MENU_ONE,
	MI_NOTIFICATION_MENU_TWO,
	MI_MEMORY_FULL,
	MI_SETTING_APP1,
	MI_SETTING_APP2,
	MI_RESET,
	MI_RESTART,
	MI_REMIND_LIST,
	MI_REFRESH_TIME_UI_FUN,
    MI_VIRATE_LEVEL,
    MI_CAMERA_COUNTDOWN,
    MI_BATTERY_INFO,
    MI_RECORD_GO,
    MI_RECORD_RESULT,
    MI_RECORD_RUN,
    MI_EARNED_MEDAL_REMIND,
    MI_SOCIAL_CLASS,
    MI_USER_REMAND,
}MainDataI;

//ancs消息推送主界面
typedef enum 
{
    M_INCOMINGCALL =0,
    M_MISSEDCALL,
    M_SMS,
    M_SOCIAL,
    M_E_MAIL,
    M_DAILY_SCHEDULE,
    M_COUNT_UPDATE
}MsgUI;

typedef enum 
{
	Ancs_Null = 0,
	Ancs_IncomingCall,	
	Ancs_MissedCall,
	Ancs_Sms,
	Ancs_Social,
	Ancs_Email,
	Ancs_Schedule,
	Call_Record,
	Ancs_Social_Facebook,
	Ancs_Social_Gmail,
	Ancs_Social_Hangouts,
	Ancs_Social_Instagram,
	Ancs_Social_Line,
	Ancs_Social_Linkedin,
	Ancs_Social_Messenger,
	Ancs_Social_QQ,
	Ancs_Social_Skype,
	Ancs_Social_Snapchat,
	Ancs_Social_Twitter,
	Ancs_Social_Uber,
	Ancs_Social_Viber,
	Ancs_Social_Wechat,
	Ancs_Social_WhatsApp
}ANCS_SAVE_TYPE;

typedef enum 
{
    MS_PLAY = 0,
    MS_STOP,
    MS_PREVIOUS,
    MS_NEXT,
    MS_VOL_ADD,
    MS_VOL_REDUCE,
    MS_CHECK=0xFB,
    MS_RESUME = 0xFC,
    MS_CONNECT=0xFD,
    MS_CONNECT_FAIL=0xFE,
    MS_NO = 0xFF
}MusicSet;

typedef enum 
{
	TIMER_START = 0,
	TIMER_RUN,
	TIMER_STOP,
}TimerSet;


typedef enum 
{
	PAY_LOCK = 0,
	PAY_UNLOCK,
	PAY_ANIMATION_1,
	PAY_ANIMATION_2,
	PAY_CONSUME,
	PAY_BALANCE,
	PAY_LOG,
}Payment;

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


typedef struct log_data_s {
	uint32_t 				Balance;
	uint32_t				Amount;
	uint8_t					Date[6];
	uint8_t 				Time[6];
} log_data_t;


typedef struct
{
    uint16_t year;   
    uint8_t month;    
    uint8_t day;      
    uint8_t hour;     
    uint8_t minutes;
    uint8_t week;	 	//0~6(日~六)
} UTCTimeStruct;

extern DISPLAY_STATUS_T gDisplayStatus;
extern UTCTimeStruct gDisplayTime;
extern UTCTimeStruct DisplayTime;
extern Format gFormat;
extern uint8_t power_state;
extern log_data_t log_data;
extern LanguageType LanNow;


/*主界面状态栏*/
#define LCD_RING_MODE					gDisplayStatus.DMS.DMS0
#define LCD_SLEEP_MODE					gDisplayStatus.DMS.DMS1
#define LCD_MEM_FULL					gDisplayStatus.DMS.DMS2
#define LCD_BT4_CONNECT				gDisplayStatus.DMS.DMS3
#define LCD_BT2_CONNECT				gDisplayStatus.DMS.DMS4


//响应枚举
typedef enum 
{
	F_RSP_OK=0x00,
	F_RSP_FAIL=0x01,
	F_RSP_PARSE_ERROR=0x02,
	F_RSP_NULL=0x04
}F_RSP_VAULE;

//协议函数结构
typedef uint8_t (*pHandlerFn)(uint8_t, uint8_t *,uint16_t);
typedef struct
{
	uint8_t DataCmd;
	pHandlerFn pFn;
} NORDIC_DATA_CMD_T;

uint8_t NordicDataParseHandler(uint8_t cmd, uint8_t* pdata, uint16_t plen);


uint8_t APDU_PowerOn(uint8_t cmd, uint8_t *pdata,uint16_t plen);
uint8_t APDU_PowerOff(uint8_t cmd, uint8_t *pdata,uint16_t plen);
uint8_t APDU_GetStatus(uint8_t cmd, uint8_t *pdata,uint16_t plen);
uint8_t APDU_TxData(uint8_t cmd, uint8_t *pdata,uint16_t plen);
uint8_t APDU_BlockUnblock(uint8_t cmd, uint8_t *pdata,uint16_t plen);
uint8_t APDU_ReadLog(uint8_t cmd, uint8_t *pdata,uint16_t plen);
uint8_t NFC_Action(uint8_t cmd, uint8_t *pdata,uint16_t plen);
uint8_t BoosterSet(uint8_t cmd, uint8_t *pdata,uint16_t plen);
uint8_t BoosterGet(uint8_t cmd, uint8_t *pdata,uint16_t plen);
uint8_t LEDLight(uint8_t cmd, uint8_t *pdata,uint16_t plen);
uint8_t NordicPM(uint8_t cmd, uint8_t *pdata,uint16_t plen);
uint8_t GetKl17Version(uint8_t cmd, uint8_t *pdata,uint16_t plen);
uint8_t Set_OTALanguageInfo(uint8_t cmd, uint8_t *pdata,uint16_t plen);
uint8_t PowerOffSystem(uint8_t cmd, uint8_t *pdata,uint16_t plen);
uint8_t ResumeBacklight(uint8_t cmd, uint8_t *pdata,uint16_t plen);
uint8_t Flash_Write(uint8_t cmd, uint8_t *pdata,uint16_t plen);
uint8_t Flash_Read(uint8_t cmd, uint8_t *pdata,uint16_t plen);
uint8_t Flash_Erase(uint8_t cmd, uint8_t *pdata,uint16_t plen);
uint8_t Flash_Set_Access(uint8_t cmd, uint8_t *pdata,uint16_t plen);
uint8_t Flash_Display_Ancs_LayOut(uint8_t cmd, uint8_t *pdata,uint16_t plen);
uint8_t LCD_UpdateStatus(uint8_t cmd, uint8_t *pdata,uint16_t plen);
uint8_t LCD_UpdateTime(uint8_t cmd, uint8_t *pdata,uint16_t plen);
uint8_t LCD_UpdateTimeFormat(uint8_t cmd, uint8_t *pdata,uint16_t plen);
uint8_t LCD_MainDataInterface(uint8_t cmd, uint8_t *pdata,uint16_t plen);
uint8_t LCD_MusicInterface(uint8_t cmd, uint8_t *pdata,uint16_t plen);
uint8_t LCD_Picturenterface(uint8_t cmd, uint8_t *pdata,uint16_t plen);
uint8_t LCD_PhoneInterface(uint8_t cmd, uint8_t *pdata,uint16_t plen);
uint8_t LCD_MsgInterface(uint8_t cmd, uint8_t *pdata,uint16_t plen);
uint8_t LCD_MsgSecInterface(uint8_t cmd, uint8_t *pdata,uint16_t plen);
uint8_t LCD_DevCommInterface(uint8_t cmd, uint8_t *pdata,uint16_t plen);
uint8_t LCD_StopWatchInterface(uint8_t cmd, uint8_t *pdata,uint16_t plen);
uint8_t LCD_VoIumeInterface(uint8_t cmd, uint8_t *pdata,uint16_t plen);
uint8_t LCD_HeartRateInterface(uint8_t cmd, uint8_t *pdata,uint16_t plen);
uint8_t LCD_CameraAndSearchInterface(uint8_t cmd, uint8_t *pdata,uint16_t plen);
uint8_t Flash_Display_PhBook_Inf(uint8_t cmd, uint8_t *pdata,uint16_t plen);
uint8_t Flash_Display_PhonePrepare(uint8_t cmd, uint8_t *pdata,uint16_t plen);
uint16_t uint16_decode(uint8_t * p_encoded_data);
#endif


