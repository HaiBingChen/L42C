

#include "Typedef.h"
#include "ble_ancs_c.h"





#define BLE_SET_OK_RSP_EVENT                               0x0001
#define BLE_SET_FAIL_RSP_EVENT                            0x0002
#define BLE_CONNECTED_EVENT                                 0x0004
#define BLE_DISCONNECTED_EVENT                           0x0008
#define BLE_CONNECTEING_EVENT                             0x0010
#define BLE_PAIRED_EVENT                                        0x0020
#define BLE_KEEP_LINK_EVENT                                   0x0040
#define BLE_PKT_SENT_COMPLETE_EVENT			0x0080
#define BLE_ANCS_SERVICE_FIND_EVENT			0x0100


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
	Ancs_Social_Gmail,
	Ancs_Social_Facebook,
	Ancs_Social_Twitter,
	Ancs_Social_Wechat,
	Ancs_Social_Snapchat,
	Ancs_Social_Instagram,
	Ancs_Social_Line,
	Ancs_Social_WhatsApp,
	Ancs_Social_Messenger,
	Ancs_Social_QQ,	
	Ancs_Social_Hangouts,
	Ancs_Social_Linkedin,	
	Ancs_Social_Skype,	
	Ancs_Social_Uber,
	Ancs_Social_Viber
}ANCS_SAVE_TYPE;

	
	


typedef enum 
{
	ID_TITLE=0,
	ID_MESSAGE,
	ID_DATE,
	ID_SAVE
}ANCS_SAVE_ID;

typedef struct
{
    uint8	AncsData[256];		
    uint16 msg;
    uint16 TimeOut;	
} ancs_data_t;


typedef enum 
{
    ID_ID=0,
    ID_CNT,
    ID_NUM_NAME
}PHONEBOOK_SAVE_ID;


extern uint8 BleBondRequiredOk;
extern uint8 BondRequiredCounter;
extern uint16 m_conn_handle;
extern ancs_data_t AncsData;
extern bool AncsCmdFlag;
extern uint8 FaceBookMessageIDTitle;
extern uint8 SkeypeIDTitle;
extern uint8 WhatsappIDTitle;
extern uint8 QQIDTitle;
extern uint8 SnapchatTitle;




void BleInitTask(uint8 task_id);
uint8 GetBleTaskId(void);
void BlePktSendComplete(void);
void FLP_TurnOnBle(void);
void FLP_TurnOffBle(void);
uint16 BleEventHandler(uint8 task_id,uint16 events);
void BleAncsAppMsg(ble_ancs_c_evt_notif_t * p_notif);
void FreeReceiveBuff(void);
void AncsIncomingcalledGetInfo(void);
void AncsMisscalledGetInfo(void);
void AncsEmailGetInfo(void);
void AncsScheduleGetInfo(void);
void AncsSmsGetInfo(void);
void AncsConnectedGetAllInfo(void);
void GetNotifAttributesTitleInf(uint8 *pData, uint8 len);
void SetSocialNotifEventFlag(void);
void AncsRemoveFunction(void);
void RemoveSMS(void);
void ResetSendInfo(void);
void Send_AncsConnectedGetAllInfo(void);
void SaveAncsToBuf_ForAndroid(ANCS_SAVE_TYPE type,ANCS_SAVE_ID id, uint8* Data, uint8 Len,bool save);
void SaveAncsToBuf_ForAndroid_SmsAndCall(ANCS_SAVE_TYPE type,ANCS_SAVE_ID id, uint8* Data, uint8 Len, bool save);
extern void sec_req_timeout_handler(void * p_context);
extern void apple_notification_setup(void);
void FacebookMessageSet(void);
void SkypeSet(void);
void WhatsappSet(void);
void QQSet(void);
void SnapchatSet(void);
uint8  Format_Gmail(uint8 * pBuf, uint8 len);









