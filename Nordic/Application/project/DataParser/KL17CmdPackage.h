/******************************************************************************************
*文件名：			KL17CmdPackage.c
*说明：			KL17通讯协议数据解析实现文件
*编写:			CHENHAO
*版本信息:		V1.0
******************************************************************************************/
#ifndef KL17CMDPACKAGE_H__
#define KL17CMDPACKAGE_H__

#include "Typedef.h"

/*通信成功-返回值*/
#define SPI_RSP_SUCCED			0x00
#define SPI_RSP_FAILED			0x01
#define SPI_RSP_DATA			0x02
#define SPI_RSP_STATUS			0x03

/*通信失败码，大于SPI_SUCCED则SPI通信出错*/
#define SPI_SUCCED				0xa0
#define SPI_FAILED				0xa1
#define SPI_RET_TIMEOUT		0xa2
#define SPI_RET_INV_CMD		0xa3
#define SPI_RET_INV_LEN			0xa4

/*命令码*/
#define GET_KL17_VERSION			0x01
#define LED_LIGHT					0x02
#define NORDIC_PM					0x03
#define OTA_LANGUAGE_INFO			0x04
#define POWER_OFF_SYSTEM			0x05
#define RESUME_BACKLIGHT			0x06

#define FLASH_WRITE					0x20
#define FLASH_READ					0x21
#define FLASH_ERASE					0x22
#define FLASH_ACCESS				0x23
#define FLASH_ANCS_DISPLAY			0x24
#define FLASH_PHONEBOOK_DISPLAY	0x25
#define FLASH_PHONEPREPARE		0x26

#define LCD_UPDATE_STATUS			0x50
#define LCD_UPDATE_TIME			0x51
#define LCD_UPDATE_TIME_FORMAT	0x52
#define LCD_MAIN_DATA_INTERFACE	0x53
#define LCD_MUSIC_INTERFACE		0x54
#define LCD_PICTURE_INTERFACE		0x55
#define LCD_PHONE_INTERFACE		0x56
#define LCD_MSG_INTERFACE			0x57
#define LCD_MSG_SEC_INTERFACE		0x58
#define LCD_DEV_COMM_INTERFACE	0x59
#define LCD_STOP_WATCH_INTERFACE	0x5A
#define LCD_VOLUME_INTERFACE		0x5B
#define LCD_HEART_RATE_INTERFACE	0x5C
#define LCD_CAS_INTERFACE			0x5D

#define NFC_ACTION					0xA0
#define APDU_POWER_ON				0xA1
#define APDU_POWER_OFF			0xA2
#define APDU_GET_STATUS			0xA3
#define APDU_TX_DATA				0xA4
#define APDU_BLOCK_UNBLOCK		0xA5
#define APDU_READ_LOG				0xA6
#define BOOSTER_SET					0xA7
#define BOOSTER_GET					0xA8

typedef enum 
{
    F_SECTOR = 0,
    F_BLOCK,
    F_CHIP
}FlashErase;

typedef enum 
{
    F_FREESCAL = 0,
    F_NORDIC,
}FlashAccess;

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

typedef enum 
{
    P_REDIAL = 0,
    P_REDIAL_END,
    P_RINGING,
    P_REJECT,
    P_INCALL,
    P_CALLEND
}PhoneUI;

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
    M_MISSEDCALL_ID = 0,
    M_SMS_ID,
    M_SMS_MSG,
    M_SMS_DATE,
    M_SMS_NEXT_PAGE
}MsgSecUI;

typedef enum 
{
    D_DEV_COMM = 0,
    D_SEARCH,
    D_FRIEND,
    D_STRANGER,
    D_ADD_FRIENDS,
    D_EMO_1,
    D_EMO_2,
    D_EMO_3,
    D_EMO_4,
    D_EMO_5
}DevCommUI;

typedef enum 
{
    S_SW = 0,
    S_SW_TOUCH
}StopWatchUI;

typedef enum 
{
    C_CAMERA= 0,
    C_CAMERA_OPEN,
    C_SEARCH,
    C_SEARCH_OPEN
}CameraAndSearchUI;

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
	MI_SLEEP_END,
	MI_SLEEP_OVERVIEW_ONE,
	MI_SLEEP_OVERVIEW_TWO,
	MI_SLEEP_OVERVIEW_THREE,
	MI_SLEEP_DETAIL,
	MI_SLEEP_INFO_ONE,
	MI_SLEEP_INFO_TWO,
	MI_RESET_PROGRESS,
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

typedef enum 
{
	TIMER_START = 0,
	TIMER_RUN,
	TIMER_STOP,
}TimerSet;


typedef enum{
    N_SUSPEND =0,
    N_RESUME
}NordicPmType;


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


typedef union 
{
  uint16 u16DMS;
  struct {
	uint16 DMS0:1;
	uint16 DMS1:1;
	uint16 DMS2:1;
	uint16 DMS3:1;
	uint16 DMS4:1;
	uint16 DMS5:1;
	uint16 DMS6:1;
	uint16 DMS7:1;
	uint16 DMS8:1;
	uint16 DMS9:1;
	uint16 DMS10:1;
	uint16 DMS11:1;
	uint16 DMS12:1;
	uint16 DMS13:1;
	uint16 DMS14:1;
	uint16 DMS15:1;
    }DMS;
}DISPLAY_STATUS_T;  
extern DISPLAY_STATUS_T gDisplayStatus;
extern bool IsOTAFontRight;

#define LCD_RING_MODE					gDisplayStatus.DMS.DMS0
#define LCD_SLEEP_MODE					gDisplayStatus.DMS.DMS1
#define LCD_MEM_FULL					gDisplayStatus.DMS.DMS2
#define LCD_BT4_CONNECT				gDisplayStatus.DMS.DMS3
#define LCD_BT2_CONNECT				gDisplayStatus.DMS.DMS4




uint8 Spi_Send_MSG(uint8 cmd,uint8*sdata,uint16 slen, uint8* rdata, uint8* rlen);

bool LED_Control_Light(uint16_t Light,uint16_t Twinkle,uint16_t R,uint16_t G,uint16_t B);
bool NFC_Action(NfcAction action);
bool Get_KL17_Version(void);

bool NordicPM(NordicPmType pm);
bool Set_OTALanguageInfo(void);
bool PowerOffSystem(void);
bool SPI_LCD_Enable(void);


bool Flash_Write(uint32 addr,uint8*pBuf, uint16 len);
bool Flash_Read(uint32 addr,uint8*pBuf, uint16 len);
bool Flash_Erase(uint32 addr,FlashErase Fe);
bool Flash_Set_Access(FlashAccess Fa);
bool Flash_Display_Ancs_LayOut(uint32 addr, uint8*LayOut, uint8 LayOutIndex,uint8 Detail,DetailPageDir dir);

bool LCD_UpdateStatus(void);
bool LCD_UpdateTime(UTCTimeStruct* NTime);
bool LCD_UpdateTimeFormat(void);
bool LCD_MainDataInterface(MainDataI Mi,uint8* Data, uint32 len);
bool LCD_MainDataInterface_SetBL_Step(MainDataI Mi,uint8* Data, uint32 len,uint8 Step);
bool LCD_MainDataInterface_NoDisplay(MainDataI Mi,uint8* Data, uint32 len);
bool LCD_PictureInterface(PictureUI Pi);
bool LCD_PhoneInterface(PhoneUI Pi);
bool LCD_MsgInterface(MsgUI Mi,uint8 count);
bool LCD_MsgSecInterface(MsgSecUI Msi,uint8* data);
bool LCD_DevCommInterface(DevCommUI Di,uint8* data);
bool LCD_StopWatchInterface(StopWatchUI Si);
bool LCD_VoIumeInterface(uint8  Vol);
bool LCD_HeartRateInterface(uint8  Hr);
bool LCD_CameraAndSearchInterface(CameraAndSearchUI Ci);
bool Flash_Display_PhBook_Inf(uint32 Inf_addr,uint8 Inf_Cnt,uint32 Grp_addr,uint8 Grp_Cnt,uint8 Percent_Showed);
bool Flash_Display_PhonePrepare(uint32 Paddr,uint8 NameLen,uint8 NumLen, uint8*Num, bool IsAndroid);
#endif

