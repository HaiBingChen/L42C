#include "Typedef.h"
#include "AppHandler.h"


typedef struct 
{
	int		PhoneCallId;  
	int		PhoneRingId;            
	int		PhoneCallOutId;
	int		PhoneCallDisplayId;
	int		PhoneDialId;
	int		PhoneDialInCallId;
	int		PhonePrepareId;
} PhoneHadlerTAG;

typedef enum{
    PHONE_1ST_IN,
    PHONE_2ND_IN,
    PHONE_OUT,
    PHONE_DEFAULT,
}PhoneType;

typedef enum 
{
	CID_NUM_DATE=0,
	CID_NAME,
	CID_SAVE
}CALLER_ID;

typedef enum{
    PHONE_CALL,
    PHONE_RING,
    PHONE_CALLOUT,
}PhoneHandlerType;

typedef enum{
    PHONE_IN_RING,			/*来电界面*/
    PHONE_OUT_RING,			/*去电界面*/	
    PHONE_IN_OUT_CALL,		/*通话界面*/	
    PHONE_IN_OUT_END,		/*通话结束界面*/	
    PHONE_IN_REJECT,		/*拒绝来电*/
    PHONE_OUT_CANCEL,		/*去电取消*/
}PhoneDisplayStatus;

typedef enum
{
    PHONE_BATTERY_LOW_WARNING
}PhoneDisplayType;

struct  PhoneStateList
{
    struct PhoneStateList *pNext;
    PhoneType type;
    uint32 CallTime; 
    uint8* Name;
    uint8* Num;
    uint8 KeyCode;
};

#define ANCS_DELAY_SKIP_SMS				0x01
#define ANCS_DELAY_SKIP_MISSED_CALL		0x02
#define ANCS_DELAY_SKIP_EMAIL				0x04
#define ANCS_DELAY_SKIP_SCHEDULE			0x08
#define ANCS_DELAY_SKIP_SOCIAL			0x10

extern bool PhoneRingHandlerExist;
extern uint8 DialNumber[20];
void LoadPhoneHandler(PhoneHandlerType Ptype, uint8 *Cid, uint8 CallIndex);
uint16 PhoneRing_Handler(MsgType msg, int iParam, void *pContext);
uint16 PhoneCall_Handler(MsgType msg, int iParam, void *pContext);
uint16 PhoneCallOut_Handler(MsgType msg, int iParam, void *pContext);
uint16 PhoneDial_Handler(MsgType msg, int iParam, void *pContext);
uint16 PhoneDialInCall_Handler(MsgType msg, int iParam, void *pContext);
void IncomingCallNoticePhoneHandlerOutHandler(uint8* CData);
void IncomingCallNoticePhoneHandler(INCALL_DATA_T* CData, uint8*CidName);
void ClearCallerIDFormApk(void);
bool CheckAndReplaceUnknownNumber(uint8 *Cid);
void ShowPhoneSound(bool display);
void Change_ShowPhoneSoundStatus(void);
void ShowPhoneMic(bool display);
void Change_ShowPhoneMicStatus(void);
void LoadPhonePrepareHandler(uint32 addr,uint8 NameLen,uint8 NumLen,uint8*Number,bool IsAndroid);
uint16 PhonePrepareHandler(MsgType msg, int iParam, void *pContext);
void SaveCallerIDToBuf(PhoneType Type,CALLER_ID id, uint8* Data, uint8 Len, bool save);

