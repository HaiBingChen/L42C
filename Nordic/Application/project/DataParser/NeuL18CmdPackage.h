/******************************************************************************************
*文件名：		NeuL18CmdPackage.h
*说明：			L18通讯协议数据解析头文件
*编写:			statdream
*版本信息:		V1.0
******************************************************************************************/

#include "Typedef.h"


/*从缓冲中得到数据*/

#define DATA_START_CMD  0x6F
#define DATA_STOP_CMD   0x8F
#define DATA_SET_CMD 	0x71
#define DATA_SET_RSP 	0x81
#define DATA_GET_CMD 	0x70
#define DATA_GET_RSP	0x80


#define DATA_PUT_TYPE  0x0B  //0x0A




//提醒类型
typedef enum
{
	REMIND_ADD=0x00,
	REMIND_UPDATE=0x01,
	REMIND_DELETE=0x02,
	REMIND_DELETEALL=0x03,
	REMIND_GET=0x04,	
}REMIND_D;

//接收命令列表
typedef enum 
{
	IN_SPI_FLASH_ERASE_CMD=0xF0,
       IN_MOTOR_TEST_CMD=0xFA,
	IN_LCD_DISPLAY_TEST_CMD=0xFE,
	IN_WATCH_ID_TEST_CMD =0xFF
}IN_DATA_E;

//发送命令列表
typedef enum 
{
	BAT_VALUE=0x00,
	RSP_CMD=0x01,
	GET_PUT_TYPE_CMD=0x02,
	WATCH_ID_CMD=0x03,
	UPLOAD_SPORT_CMD=0x04,
	GET_STORE_MSG_CMD=0x05,
	SOFTWARE_VERSON_CMD=0x06,
	GET_INIT_MARK_CMD=0x07,
	BAND_PERSON_MSG_CMD=0x08,
	DAYDATE_SPORT_DATA_CMD=0x0A,
	UPLOAD_REMINDER_CMD=0x0C,
	UPLOAD_SPORT_TOTAL_CMD=0x0D,
	UPLOAD_SLEEP_AWAKE_DATA_CMD=0x0E,
	UPLOAD_SLEEP_AWAKE_TOTAL_CNT_CMD=0x10, 

	UPLOAD_DAILY_GOAL_SETTING_CMD=0x15,
	UPLOAD_DEVICE_SWITCH_SETTING_CMD=0x17,
	UPLOAD_DATE_TIME_FORMAT_SETTING_CMD=0x18,
	UPLOAD_SLEEP_MODE_SET_CMD=0x19,
	UPLOAD_VOLUME_VALUE_CMD=0x1A,
	UPLOAD_SPORT_MODE_CMD=0x1B,
	UPLOAD_SIT_REMIAND_SETTING_CMD =0x1D,
	UPLOAD_PAY_AID =0x1E,
	UPLOAD_PAY_BALANCE =0x1F,
	UPLOAD_PAY_RECORDS =0x20,
	UPLOAD_PAY_PASSTHROUGH =0x21,
	UPLOAD_LANGUAGE_FORMAT =0x22,
	UPLOAD_TIME_UI_INTERFACE =0x23,
       GET_FREESCALE_VERSION =0x24,
	
	UPLOAD_BT2_ADDR_CMD=0xF8,
	UPLOAD_BT2_NAME_CMD=0xF9,
	UPLOAD_BT4_NAME_CMD=0xFA,
	UPLOAD_BT4_ADDR_CMD=0xFB,
	UPLOAD_KEYBOARD_TEST_RESULT_CMD=0xFC,
	UPLOAD_DATE_TIME_CMD=0xFE,
	UPLOAD_SENSOR_DATA_CMD=0xFF,
	
}DATA_E;


uint8 DataParseHandler(const void *InData);
void SendRspCmd(uint8 Cmd,RSP_VAULE_E uData);
uint8 GetCmdRespond(uint8 *pData);
uint8 GetWatchIdCmdCB(uint8 *pData);
uint8 GetVersionMsgCmdCB(uint8 *pData);
uint8 DateTimeSettingCmdCB(uint8 *pData);
uint8 TimeOrderCmdCB(uint8 *pData);
uint8 MenuOrderCmdCB(uint8 *pData);
uint8 BacklightSettingCmdCB(uint8 *pData);
uint8 GetBatValueCmdCB(uint8 *pData);
uint8 VolumeValueCB(uint8 *pData);
uint8 MotoOrderValueCB(uint8 *pData);
uint8 SetLanguageFormat(uint8 *pData);
uint8 SetUnitFormat(uint8 *pData);
uint8 FactorySettingCmdCB(uint8 *pData);
void OtaProgMsg(void);
uint8 UpdateProgMsgCmdCB(uint8 *pData);
uint8 PersonInitCmdCB(uint8 *pData);
uint8 UserHabitCmdCB(uint8 *pData);
uint8 DailyGoalSettingCmdCB(uint8 *pData);
uint8 GetSportModeCB(uint8 *pData);
uint8 GetSportTotalCntCB(uint8 *pData);
uint8 ClearSportDataCB(uint8 *pData);
void SendSportDataCmd(SPORT_DATA_T uData);
uint8 UploadSportCmdCB(uint8 *pData);
uint8 ClearSleepDataCB(uint8 *pData);
void SendSleepDataCmd(SLEEP_DATA_T uData);
uint8 GetSleepAwakeDataCB(uint8 *pData);

uint8 GetHeartRateTotalCntCB(uint8 *pData);
uint8 ClearHeartRateDataCB(uint8 *pData);
void SendHeartRateDataCmd(HEARTRATE_DATA_T uData);
uint8 GetHeartRateDataCB(uint8 *pData);
uint8 AutoHeartRateCB(uint8 *pData);
uint8 HeartRateAlarmCB(uint8 *pData);
uint8 SedentarinessRemindCB(uint8 *pData);
uint8 BaseKcalTypeCB(uint8 *pData);
uint8 GetRecordWorkTotalCntCB(uint8 *pData);
void SendRecordWorkDataCmd(REC_WORK_DATA_T uData);
uint8 UploadRecordWorkCmdCB(uint8 *pData);
uint8 ClearRecordWorkDataCB(uint8 *pData);

uint8 GetDisplaySportDataCmdCB(uint8 *pData);
uint8 SetSleepModeCB(uint8 *pData);
uint8 AndroidCallCB(uint8 *pData);
uint8 AndroidSmsCallCB(uint8 *pData);
uint8 NoticeNumberCB(uint8 *pData);
uint8 AndroidSocialCB(uint8 *pData);
uint8 AndroideEmailCB(uint8 *pData);
uint8 AndroideScheduleCB(uint8 *pData);
uint8 UpdateDeviceSwitchSettingCB(uint8 *pData);
uint8 ReminderNumberCmdCB(uint8 *pData);
void ReminderSendData(uint8 idx,uint8*Rd);
uint8 ReminderCmdCB(uint8 *pData);
uint8 BindingBeginCmdCB(uint8 *pData);
uint8 BindingEndCmdCB(uint8 *pData);
void SendPayAID(uint8* uData);
uint8 ReadPayAID(uint8 *pData);
void SendPayBalance(uint8* uData);
uint8 ReadPayBalance(uint8 *pData);
uint8 ReadPayRecords(uint8 *pData);
void PayPassthroughToApp(uint8 Aid, uint8 *pData, uint16 len);
uint8 PayPassthroughFromApp(uint8 *pData);
uint8 MusicSettingsCmdCB(uint8 *pData);
uint8 CameraControlCmdCB(uint8 *pData);
uint8 SearchForMobilePhonesCmdCB(uint8 *pData);
uint8 DeviceCommIDCmdCB(uint8 *pData);
uint8 SpiFlashEraseSaveFlag(uint8 *pData);
uint8 SpiFlashUpdateFontLib(uint8 *pData);
uint8 LEDTest(uint8 *pData);
uint8 TestFactoryMode(uint8 *pData);
uint8  GetBT4AddrTest(uint8 *pData);
uint8  KeypadTest(uint8 *pData);
uint8  GetDateTimeTest(uint8 *pData);
uint8  PowOFFSystemTest(uint8 *pData);
uint8  MotorTest(uint8 *pData);
uint8 TpTest(uint8 *pData);
uint8  SensorTest(uint8 *pData);
uint8  LCDTest(uint8 *pData);
uint8 WatchIDTest(uint8 *pData);
uint8 NFCTest(uint8 *pData);
uint8 Get_CSN(uint8 *pData);
void SendCSNData(uint8 *uData);
bool VerifyWatchId(uint8 * inWatchId, uint8 len);
void SetFactoryStandbyTime(void);
uint8 PhoneBookCB(uint8 *pData);
uint8 MedalCB(uint8 *pData);
void InitBandInfo(void);
uint8 MotoVibrationIntensityCB(uint8 *pData);

extern uint8 CurrentDataParseCmd;
extern uint8 Pay_Aid;
extern uint8 pay_data[12];
extern const uint8 SoftWareVersion[20];


