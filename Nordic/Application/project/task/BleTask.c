
#include "headfile.h"





typedef struct
{
    uint8   EventId;              //0
    uint8   EventFlags;           //1
    uint8   CategoryID;           //2
    uint8   CategoryCount;        //3
    uint8   NotificationUID[4];      //4
} ANCS_NOTIF_INFO_T;

typedef enum
{
    NotifEventIdIndex = 0,
    NotifEventFlagsIndex = 1,
    NotifCategoryIDIndex = 2,
    NotifCategoryCountIndex = 3,
    NotifNotificationUIDIndex = 4,
} ANCS_NOTIF_INDEX_E;

//CategoryID Values
typedef enum
{
    CategoryIDOther = 0,
    CategoryIDIncomingCall = 1,
    CategoryIDMissedCall = 2,
    CategoryIDVoicemail = 3,
    CategoryIDSocial = 4,
    CategoryIDSchedule = 5,
    CategoryIDEmail = 6,
    CategoryIDNews = 7,
    CategoryIDHealthAndFitness = 8,
    CategoryIDBusinessAndFinance = 9,
    CategoryIDLocation = 10,
    CategoryIDEntertainment = 11,
    CategoryIDReserved, //12~255
} ANCS_CATEGORY_ID_E;

//EventID Values
typedef enum
{
    EventIDNotificationAdded = 0,
    EventIDNotificationModified = 1,
    EventIDNotificationRemoved = 2,
} ANCS_EVENT_ID_VALUES_E;

//EventFlags
typedef enum
{
    EventFlagSilent = (1 << 0), //静默提醒
    EventFlagImportant = (1 << 1), //重要提醒
} ANCS_EVENT_FLAGS_E;

//CommandID Values
typedef enum
{
    CommandIDGetNotificationAttributes = 0,
    CommandIDGetAppAttributes = 1,
} ANCS_COMMAND_ID_VALUES_E;

//NotificationAttributeID Values
typedef enum
{
    NotificationAttributeIDAppIdentifier = 0,
    NotificationAttributeIDTitle = 1,   //(Needs to be followed by a 2-bytes max length parameter)
    NotificationAttributeIDSubtitle = 2, //(Needs to be followed by a 2-bytes max length parameter)
    NotificationAttributeIDMessage = 3, //(Needs to be followed by a 2-bytes max length parameter)
    NotificationAttributeIDMessageSize = 4,
    NotificationAttributeIDDate = 5,
    NotificationAttributeIDReserved,//6-255
} ANCS_NOTIF_ATT_ID_VALUES_E;


typedef struct 
{
        bool               SocialNumberAdd; 
        bool               SocialNumberRemove; 
	 bool	        MissedCallNumberRemove; 
	 bool	        EmailCallNumberRemove; 
} AncsNumberTAG;




#define SMS_UID_ARRAY_LEN				180
#define INCOMINGCALL_UID_ARRAY_LEN	10
#define MISSCALL_UID_ARRAY_LEN		100
#define EMAIL_UID_ARRAY_LEN			60
#define SCHEDULE_UID_ARRAY_LEN		20
#define UNKNOWN_NAME					"E69CAAE698BEE7A4BAE4B8BBE58FABE58FB7E7A081"

#define SOCIAL_SMS						"com.apple.MobileSMS" 
#define SOCIAL_Facebook					"com.facebook.Facebook"
#define SOCIAL_Gmail_L					"com.google.gmail"
#define SOCIAL_Gmail_B					"com.google.Gmail"
#define SOCIAL_Hangouts					"com.google.hangouts"
#define SOCIAL_Instagram				"com.burbn.instagram"			
#define SOCIAL_Line						"jp.naver.line"
#define SOCIAL_Linkedin					"com.jiuyan.infashion"
#define SOCIAL_Messenger 				"com.facebook.Messenger"
#define SOCIAL_QQ						"com.tencent.mqq"
#define SOCIAL_Skype					"com.skype.tomskype"
#define SOCIAL_Snapchat					"com.toyopagroup.picaboo"
#define SOCIAL_Twitter					"com.atebits.Tweetie2"
#define SOCIAL_Uber 						"com.ubercab.UberClient"
#define SOCIAL_Viber						"com.viber"
#define SOCIAL_Wechat					"com.tencent.xin"
#define SOCIAL_WhatsApp				"net.whatsapp.WhatsApp"


ancs_data_t AncsData;
ancs_data_t AncsData_SmsAndCall;
ANCS_SAVE_TYPE SocialType = Ancs_Null;

bool    UidStoreFlag=false;
uint8   ConnectedGetAllInfoFlag=0;
uint8   QueryValue[1];
uint8   NotifCategoryIDType = 0xFF;
uint8   GlobalNotificationUID[4]={0};
uint8   GlobalNotificationUIDRemove1[4]={0};
uint8   *SmsUidTempArray=NULL;
uint8   *IncomingcallUidTempArray=NULL;
uint8   *MisscallUidTempArray=NULL;
uint8   *EmailUidTempArray=NULL;
uint8   *ScheduleUidTempArray=NULL;

static uint8 SocialNumberTemp;
static uint8 SmsUidTempIndex;
static uint8 IncomingUidTempIndex;
static uint8 MisscallUidTempIndex;
static uint8 EmailUidTempIndex;
static uint8 ScheduleUidTempIndex;

uint8 NewMessageState=0xFF;

AncsNumberTAG AncsTypeNumber;
static uint8 BleTaskId=0;
uint8 BleBondRequiredOk = 0;
uint8 BondRequiredCounter=0;
uint8 FaceBookMessageIDTitle=0;
uint8 SkeypeIDTitle=0;
uint8 WhatsappIDTitle=0;
uint8 QQIDTitle=0;
uint8 SnapchatTitle=0;

extern dm_handle_t                m_peer_handle; 
extern ble_ancs_c_evt_notif_t m_notification_latest;  
extern void Ancs_Discovery_Restart(void);




/******************************************************************************************
*函数名:		BleInitTask
*参数:			
*返回值:		
*描述:			BLE  任务初始化
*编写:			 
*版本信息:	    
******************************************************************************************/ 
void BleInitTask(uint8 task_id)
{
	BleTaskId=task_id;
}
/******************************************************************************************
*函数名:		GetBleTaskId
*参数:			
*返回值:		
*描述:			得到BLE  事件Id
*编写:			 
*版本信息:	   
******************************************************************************************/ 
uint8 GetBleTaskId(void)
{
	return BleTaskId;
}

void BlePktSendComplete(void)
{
	osal_set_event(BleTaskId, BLE_PKT_SENT_COMPLETE_EVENT);
}

void FLP_TurnOnBle(void)
{
	if (shortcut_info.airplane_mode == 0)
	{
        	BtState=BT_OPEN;
        	ble_advertising_start(BLE_ADV_MODE_FAST);
	}
}


void FLP_TurnOffBle(void)
{
        sd_ble_gap_disconnect(m_conn_handle,BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
        BtState=BT_CLOSE;
        BledisconnectFlag = BT_CLOSE;
        sd_ble_gap_adv_stop();
}


uint16 BleEventHandler(uint8 task_id,uint16 events)
{    
        if (events & BLE_SET_OK_RSP_EVENT)
        {
                SendRspCmd(CurrentDataParseCmd,E_RSP_OK);
                return events ^ BLE_SET_OK_RSP_EVENT;
        }
		
        if (events & BLE_SET_FAIL_RSP_EVENT)
        {
                SendRspCmd(CurrentDataParseCmd,E_RSP_FAIL);
                return events ^ BLE_SET_FAIL_RSP_EVENT;
        }
		
        if (events & BLE_CONNECTED_EVENT)
        {
        	  
                gSocialCountT = 0;
                gShortMsgCountT = 0;
                gMissedCallCountT = 0;
                gIncomingCallCountT = 0;
		  gEMailCountT=0;
		  gScheduleCountT =0;
		  
                BondSuccessfulState = true;
                BtState=BT_CONNECTED;
                BledisconnectFlag = BT_CONNECTED;
                FreeReceiveBuff();
		  nus_data_init();
                //FreeAndroidMsgSendBuff();
                SetSocialNotifEventFlag();
                SendMsg(MSG_BLE_CONNECTED);      
                ChargeDisplayStateChange(false);   
                osal_start_timerEx(GetAppTaskId(), MSG_BLE_DISCONNECT, 1000);
                
                return events ^ BLE_CONNECTED_EVENT;
        }

        if (events & BLE_DISCONNECTED_EVENT)
        {
                BleBondRequiredOk=0;
                BondRequiredCounter=0;
                FreeReceiveBuff();
                //FreeAndroidMsgSendBuff();
                CountForCloseBLE = 0;
                BondSuccessfulState = true;
                if(Powerstate.powerstate && (!POWER_OFF_BT_OFF))
                {
                        BtState=BT_DISCONNECT;
                        BledisconnectFlag = BT_DISCONNECT;
                        ChargeDisplayStateChange(false);   
			   if (shortcut_info.airplane_mode == 0)			
                        	ble_advertising_start(BLE_ADV_MODE_DIRECTED);
                        osal_start_timerEx(GetAppTaskId(), MSG_BC5_DISCONNECT, 100);
                }
                
                return events ^ BLE_DISCONNECTED_EVENT;
        }

        if (events & BLE_PAIRED_EVENT)
        {
                if(BleBondRequiredOk==0)
                {
                        if(BondRequiredCounter > 0)
                        {
                                BondRequiredCounter = 0;
                                osal_start_timerEx( GetBleTaskId(), BLE_PAIRED_EVENT, 1000 );
                        }
                        else
                        {
                                dm_security_setup_req(&m_peer_handle);
                        }
                }
                else if(BleBondRequiredOk==1)
                {
                        if(BondRequiredCounter > 0)
                        {
                                BondRequiredCounter --;
                                osal_start_timerEx( GetBleTaskId(), BLE_PAIRED_EVENT, 1000 );
                        }
                        else
                        {
                                FLP_TurnOffBle(); 
                                osal_start_timerEx(GetAppTaskId(), MSG_BLE_DISCONNECT, 1000);
                                FLP_TurnOnBle();
                        }
                }
                else if(BleBondRequiredOk==2)
                {
                        FLP_TurnOffBle(); 
                        osal_start_timerEx(GetAppTaskId(), MSG_BLE_DISCONNECT, 1000);
                        FLP_TurnOnBle();
                }
                                
                return events ^ BLE_PAIRED_EVENT;
        }

        if (events & BLE_CONNECTEING_EVENT)
        {
                sec_req_timeout_handler(NULL);
                
                return events ^ BLE_CONNECTEING_EVENT;
        }

        if (events & BLE_KEEP_LINK_EVENT)
        {
                apple_notification_setup();
		  if(BtState==BT_CONNECTED)		
                	osal_start_timerEx( GetBleTaskId(), BLE_KEEP_LINK_EVENT, 20000);
                return events ^ BLE_KEEP_LINK_EVENT;
        }
		
        if (events & BLE_PKT_SENT_COMPLETE_EVENT)
        {
		if(gUploadSportDataState==UPLOADING)
			osal_start_timerEx( GetStepTaskId(), STEP_RECORD_UPLOAD_EVENT, 200 );
		else if(gSleepDataUploadState==UPLOADING)
			osal_start_timerEx( GetSleepTaskId(), SLEEP_CNT_UPLOAD_EVENT, 200 );   
		else if (gUploadPayRecordDataState == UPLOAD_RECORD_ING)
			osal_start_timerEx(GetHwTaskId(), PAY_RECORD_UPLOAD_EVENT, 200 ); 
		else if (gUploadRemindsDataState == UPLOAD_REMIND_ING)
			osal_start_timerEx(GetAppTaskId(), MSG_UPLOAD_REMINDS, 200 ); 
		else if (gHeartRateDataUploadState == UPLOADING)
			osal_start_timerEx(GetHrTaskId(), HEARTRATE_CNT_UPLOAD_EVENT, 200 );
		else if (gUploadRecordWorkState ==UPLOADING )
			osal_start_timerEx( GetStepTaskId(), RECORDWORK_UPLOAD_EVENT, 200 );
		return events ^ BLE_PKT_SENT_COMPLETE_EVENT;
        }
        if (events & BLE_ANCS_SERVICE_FIND_EVENT)
        {		
			Ancs_Discovery_Restart();
			return events ^ BLE_ANCS_SERVICE_FIND_EVENT;
        }			
        return ( 0 );     
}


void SendGetNotifAttributesTitleCmd(uint8 *pNotificationUID, uint8 Type)
{
        ble_ancs_c_request_attrs(&m_notification_latest,pNotificationUID,Type);
}

void mem_retreat(uint8* buf, uint32 len, uint8 off)
{
	uint32 i;
	for(i=0; i<len-off; i++)
	{
		buf[i] = buf[i+off];
	}
}

void BleAncsAppMsg(ble_ancs_c_evt_notif_t * p_notif)
{
        switch(p_notif->category_id)
        {
            case CategoryIDIncomingCall:
			#if 1
	         if(!SWITCH_ANCS_INCOMING_CALL)
                	break;


                if(p_notif->evt_id == EventIDNotificationAdded)
                {
                        if((IncomingUidTempIndex==0)&&(IncomingcallUidTempArray==NULL))
                        {
                                IncomingcallUidTempArray = osal_mem_alloc(MISSCALL_UID_ARRAY_LEN);
                        }
                        if(IncomingcallUidTempArray) 
                        {
							if(IncomingUidTempIndex >= INCOMINGCALL_UID_ARRAY_LEN)
							{
								mem_retreat(IncomingcallUidTempArray, INCOMINGCALL_UID_ARRAY_LEN, 2);
								IncomingUidTempIndex -= 2;
							}
							osal_memcpy(IncomingcallUidTempArray+IncomingUidTempIndex,(uint8 *)&p_notif->notif_uid,2);
							 INCOMMING_CALL_ADD = true;
							IncomingUidTempIndex += 2;
							ConnectedGetAllInfoFlag = 3;
							AncsTypeNumber.MissedCallNumberRemove =true;					
							osal_start_timerEx(GetAppTaskId(), MSG_ANCS_CONNECTED_GET_ALL, 2000);

                                
                        }
                }
                else if(p_notif->evt_id == EventIDNotificationRemoved)
                {
                	   INCOMMING_CALL_ADD = false;
                        gIncomingCallCount = p_notif->category_count;
                        ChargeDisplayStateChange(false);
                        SendMsg(MSG_INCOMING_CALL_NOTIFY);
                }
                break;
				#endif
                
            case CategoryIDMissedCall:
                if(!SWITCH_ANCS_MISSED_CALL)
                        break;
                if(p_notif->evt_id == EventIDNotificationAdded)
                {
                        if((MisscallUidTempIndex==0)&&(MisscallUidTempArray==NULL))
                        {
                                MisscallUidTempArray = osal_mem_alloc(MISSCALL_UID_ARRAY_LEN);
                        }
                        if(MisscallUidTempArray) 
                        {
							if(MisscallUidTempIndex >= MISSCALL_UID_ARRAY_LEN)
							{
								mem_retreat(MisscallUidTempArray, MISSCALL_UID_ARRAY_LEN, 2);
								MisscallUidTempIndex -= 2;
							}
							osal_memcpy(MisscallUidTempArray+MisscallUidTempIndex,(uint8 *)&p_notif->notif_uid,2);
							MisscallUidTempIndex += 2;
							ConnectedGetAllInfoFlag = 3;
							AncsTypeNumber.MissedCallNumberRemove =false;
							osal_start_timerEx(GetAppTaskId(), MSG_ANCS_CONNECTED_GET_ALL, 2000);    
                        }
                }
#if 0				
                else if(p_notif->evt_id == EventIDNotificationRemoved)
                {
                	    if(AncsTypeNumber.MissedCallNumberRemove==false)
                	   {	                    
				   //osal_memcpy(GlobalNotificationUID,(uint8 *)&p_notif->notif_uid,2);
				   if(p_notif->category_count==0)
				   {	
				   	   gMissedCallCount = p_notif->category_count;
		                        ChargeDisplayStateChange(false);
		                        osal_start_timerEx(GetAppTaskId(), MSG_MISSED_CALL_COUNT, 500);
				   }
                	   }
                }
#endif				
                break;
                
            case CategoryIDSocial:
                if((p_notif->evt_id == EventIDNotificationAdded)&&(!AncsTypeNumber.SocialNumberRemove))
                {
                        if((SmsUidTempIndex==0)&&(SmsUidTempArray==NULL))
                        {
                                SmsUidTempArray = osal_mem_alloc(SMS_UID_ARRAY_LEN);
                        }
                        if(SmsUidTempArray) 
                        {
							if(SmsUidTempIndex>= SMS_UID_ARRAY_LEN)
							{
								mem_retreat(SmsUidTempArray, SMS_UID_ARRAY_LEN, 2);
								SmsUidTempIndex -= 2;
							}
							osal_memcpy(SmsUidTempArray+SmsUidTempIndex,(uint8 *)&p_notif->notif_uid,2);
							SmsUidTempIndex += 2;
							AncsTypeNumber.SocialNumberAdd=true;
							ConnectedGetAllInfoFlag = 3;
							osal_start_timerEx(GetAppTaskId(), MSG_ANCS_CONNECTED_GET_ALL, 2000);	

                        }
                }
#if 0				
                else if((p_notif->evt_id == EventIDNotificationRemoved)&&(!AncsTypeNumber.SocialNumberAdd))
                {
                        AncsTypeNumber.SocialNumberRemove = true;
                        SocialNumberTemp = p_notif->category_count;
                        if(!UidStoreFlag)
                        {
                                UidStoreFlag = true;
                                osal_memcpy(GlobalNotificationUIDRemove1,(uint8 *)&p_notif->notif_uid,2);
                        }
                        osal_memcpy(GlobalNotificationUID,(uint8 *)&p_notif->notif_uid,2);
                        osal_start_timerEx(GetAppTaskId(), MSG_SEND_SOCIAL_MESSAGE, 1000);                   
                }
#endif				
                break;

            case CategoryIDOther:
		#if 1
			if((p_notif->evt_id == EventIDNotificationAdded)&&(!AncsTypeNumber.SocialNumberRemove))
                {
                        if((SmsUidTempIndex==0)&&(SmsUidTempArray==NULL))
                        {
                                SmsUidTempArray = osal_mem_alloc(SMS_UID_ARRAY_LEN);
                        }
                        if(SmsUidTempArray) 
                        {
							if(SmsUidTempIndex>= SMS_UID_ARRAY_LEN)
							{
								mem_retreat(SmsUidTempArray, SMS_UID_ARRAY_LEN, 2);
								SmsUidTempIndex -= 2;
							}
							osal_memcpy(SmsUidTempArray+SmsUidTempIndex,(uint8 *)&p_notif->notif_uid,2);
							SmsUidTempIndex += 2;
							AncsTypeNumber.SocialNumberAdd=true;
							ConnectedGetAllInfoFlag = 3;
							osal_start_timerEx(GetAppTaskId(), MSG_ANCS_CONNECTED_GET_ALL, 2000);	

                        }
                }
#if 0			
                else if((p_notif->evt_id == EventIDNotificationRemoved)&&(!AncsTypeNumber.SocialNumberAdd))
                {
                        AncsTypeNumber.SocialNumberRemove = true;
                        SocialNumberTemp = p_notif->category_count;
                        if(!UidStoreFlag)
                        {
                                UidStoreFlag = true;
                                osal_memcpy(GlobalNotificationUIDRemove1,(uint8 *)&p_notif->notif_uid,2);
                        }
                        osal_memcpy(GlobalNotificationUID,(uint8 *)&p_notif->notif_uid,2);
                        osal_start_timerEx(GetAppTaskId(), MSG_SEND_SOCIAL_MESSAGE, 1000);                   
                }
#endif				
		break;
		#else
                if(!SWITCH_ANCS_SOCIAL)
                        break;
                if(p_notif->evt_id == EventIDNotificationAdded)
                {
                        if(gSocialCount < 255)
                                gSocialCount += 1;
                        //ChargeDisplayStateChange(false);
                        //SendMsg(MSG_SOCIAL_NOTIFY);
                }
                else if(p_notif->evt_id == EventIDNotificationRemoved)
                {
                        SocialNumberTemp = p_notif->category_count;
                        if(SocialNumberTemp > gShortMsgCount)
                                gSocialCount = SocialNumberTemp - gShortMsgCount;
                        else
                                gSocialCount = 0;
                        //ChargeDisplayStateChange(false);
                        //SendMsg(MSG_SOCIAL_NOTIFY);
                }
                break;
                #endif
            case CategoryIDEmail:
#if 0				
                if(!SWITCH_ANCS_EMIL)
                        break;
                if(p_notif->evt_id == EventIDNotificationAdded)
                {
                        gEMailCount = p_notif->category_count;
                        ChargeDisplayStateChange(false);
                        SendMsg(MSG_E_MAIL_NOTIFY);
                }
                else if(p_notif->evt_id == EventIDNotificationRemoved)
                {
                        gEMailCount = p_notif->category_count;
                        ChargeDisplayStateChange(false);
                        SendMsg(MSG_E_MAIL_NOTIFY);
                }
                break;
#else
		  if(!SWITCH_ANCS_EMIL)
                        break;
                if(p_notif->evt_id == EventIDNotificationAdded)
                {  	   
                        if((EmailUidTempIndex==0)&&(EmailUidTempArray==NULL))
                        {
                                EmailUidTempArray = osal_mem_alloc(EMAIL_UID_ARRAY_LEN);
                        }
                        if(EmailUidTempArray) 
                        {
							if(EmailUidTempIndex>= EMAIL_UID_ARRAY_LEN)
							{
								mem_retreat(EmailUidTempArray, EMAIL_UID_ARRAY_LEN, 2);
								EmailUidTempIndex -= 2;
							}
							osal_memcpy(EmailUidTempArray+EmailUidTempIndex,(uint8 *)&p_notif->notif_uid,2);
							EmailUidTempIndex += 2;
							ConnectedGetAllInfoFlag = 3;
							//AncsTypeNumber.EmailCallNumberRemove =false;
							osal_start_timerEx(GetAppTaskId(), MSG_ANCS_CONNECTED_GET_ALL, 500);     
                        }
                }
#if 0				
                else if(p_notif->evt_id == EventIDNotificationRemoved)
                {
                	   //if(AncsTypeNumber.EmailCallNumberRemove==false)
                	  //{
	                        gEMailCount = p_notif->category_count;
				   osal_memcpy(GlobalNotificationUID,(uint8 *)&p_notif->notif_uid,2);
	                        ChargeDisplayStateChange(false);
				   //SendMsg(MSG_E_MAIL_NOTIFY);
                	   //}
                }
#endif					
                break;			
#endif

            case CategoryIDSchedule:
#if 0				
                if(!SWITCH_ANCS_SCHEDULE)
                        break;
                if(p_notif->evt_id == EventIDNotificationAdded)
                {
                        gScheduleCount = p_notif->category_count;
                        ChargeDisplayStateChange(false);
                        SendMsg(MSG_DAILY_SCHEDULE_NOTIFY);
                }
                else if(p_notif->evt_id == EventIDNotificationRemoved)
                {
                        gScheduleCount = p_notif->category_count;
                        ChargeDisplayStateChange(false);
                        SendMsg(MSG_DAILY_SCHEDULE_NOTIFY);
                }

                break;
#else
		if(!SWITCH_ANCS_SCHEDULE)
			break;
		if(p_notif->evt_id == EventIDNotificationAdded)
		{
			if((ScheduleUidTempIndex==0)&&(ScheduleUidTempArray==NULL))
			{
				ScheduleUidTempArray = osal_mem_alloc(SCHEDULE_UID_ARRAY_LEN);
			}
			if(ScheduleUidTempArray) 
			{
				if(ScheduleUidTempIndex>= SCHEDULE_UID_ARRAY_LEN)
				{
					mem_retreat(ScheduleUidTempArray, SCHEDULE_UID_ARRAY_LEN, 2);
					ScheduleUidTempIndex -= 2;
				}
				osal_memcpy(ScheduleUidTempArray+ScheduleUidTempIndex,(uint8 *)&p_notif->notif_uid,2);
				ScheduleUidTempIndex += 2;
				ConnectedGetAllInfoFlag = 3;
				osal_start_timerEx(GetAppTaskId(), MSG_ANCS_CONNECTED_GET_ALL, 500);   
			}
		}
#if 0		
		else if(p_notif->evt_id == EventIDNotificationRemoved)
		{
			gScheduleCount = p_notif->category_count;
			osal_memcpy(GlobalNotificationUID,(uint8 *)&p_notif->notif_uid,2);
			ChargeDisplayStateChange(false);
		}
#endif			
		break;		
#endif
		case CategoryIDNews:
	             	 if((p_notif->evt_id == EventIDNotificationAdded)&&(!AncsTypeNumber.SocialNumberRemove))
                {
                        if((SmsUidTempIndex==0)&&(SmsUidTempArray==NULL))
                        {
                                SmsUidTempArray = osal_mem_alloc(SMS_UID_ARRAY_LEN);
                        }
                        if(SmsUidTempArray) 
                        {
							if(SmsUidTempIndex >= SMS_UID_ARRAY_LEN)
							{
								mem_retreat(SmsUidTempArray, SMS_UID_ARRAY_LEN, 2);
								SmsUidTempIndex -= 2; 
							}
							osal_memcpy(SmsUidTempArray+SmsUidTempIndex,(uint8 *)&p_notif->notif_uid,2);
							SmsUidTempIndex += 2;
							AncsTypeNumber.SocialNumberAdd=true;
							ConnectedGetAllInfoFlag = 3;
							osal_start_timerEx(GetAppTaskId(), MSG_ANCS_CONNECTED_GET_ALL, 2000);  
                        }
                }
#if 0					 
                else if((p_notif->evt_id == EventIDNotificationRemoved)&&(!AncsTypeNumber.SocialNumberAdd))
                {
                        AncsTypeNumber.SocialNumberRemove = true;
                        SocialNumberTemp = p_notif->category_count;
                        if(!UidStoreFlag)
                        {
                                UidStoreFlag = true;
                                osal_memcpy(GlobalNotificationUIDRemove1,(uint8 *)&p_notif->notif_uid,2);
                        }
                        osal_memcpy(GlobalNotificationUID,(uint8 *)&p_notif->notif_uid,2);
                        osal_start_timerEx(GetAppTaskId(), MSG_SEND_SOCIAL_MESSAGE, 1000);                   
                }	
#endif				
	        break;
			
		case CategoryIDLocation:
			if((p_notif->evt_id == EventIDNotificationAdded)&&(!AncsTypeNumber.SocialNumberRemove))
                {
                        if((SmsUidTempIndex==0)&&(SmsUidTempArray==NULL))
                        {
                                SmsUidTempArray = osal_mem_alloc(SMS_UID_ARRAY_LEN);
                        }
                        if(SmsUidTempArray) 
                        {
							if(SmsUidTempIndex >= SMS_UID_ARRAY_LEN)
							{
								mem_retreat(SmsUidTempArray, SMS_UID_ARRAY_LEN, 2);
								SmsUidTempIndex -= 2; 
							}
							osal_memcpy(SmsUidTempArray+SmsUidTempIndex,(uint8 *)&p_notif->notif_uid,2);
							SmsUidTempIndex += 2;
							AncsTypeNumber.SocialNumberAdd=true;
							ConnectedGetAllInfoFlag = 3;
							osal_start_timerEx(GetAppTaskId(), MSG_ANCS_CONNECTED_GET_ALL, 2000);  
                        }
                }
#if 0			
                else if((p_notif->evt_id == EventIDNotificationRemoved)&&(!AncsTypeNumber.SocialNumberAdd))
                {
                        AncsTypeNumber.SocialNumberRemove = true;
                        SocialNumberTemp = p_notif->category_count;
                        if(!UidStoreFlag)
                        {
                                UidStoreFlag = true;
                                osal_memcpy(GlobalNotificationUIDRemove1,(uint8 *)&p_notif->notif_uid,2);
                        }
                        osal_memcpy(GlobalNotificationUID,(uint8 *)&p_notif->notif_uid,2);
                        osal_start_timerEx(GetAppTaskId(), MSG_SEND_SOCIAL_MESSAGE, 1000);                   
                }
#endif				
		break;    
		
        }
}

void InitAncsBuf(void)
{
	memset(AncsData.AncsData,0,256);
	AncsData.msg=0;
	AncsData.TimeOut=0;
}


void SaveAncsToBuf_ForAndroid(ANCS_SAVE_TYPE type,ANCS_SAVE_ID id, uint8* Data, uint8 Len, bool save)
{
	uint8 temp_len=0; 

	switch(id)
	{
		case ID_TITLE:
			InitAncsBuf();
			AncsData.AncsData[1] = Len;
			memcpy((AncsData.AncsData+4),Data,Len);
			break;
		case ID_MESSAGE:
			temp_len=Format_Gmail(Data, Len);
			Len=temp_len;
			AncsData.AncsData[2] = Len;
			memcpy((AncsData.AncsData+4+AncsData.AncsData[1]),Data,Len);			
			break;
		case ID_DATE:
			AncsData.AncsData[3] = Len;
			memcpy((AncsData.AncsData+4+AncsData.AncsData[1]+AncsData.AncsData[2]),Data,Len);				
			break;
		default:
			break;
	}
	AncsData.AncsData[0] = type;
	if(save)
	{
		uint8 len = AncsData.AncsData[1]+AncsData.AncsData[2]+AncsData.AncsData[3]+4;
		SaveAncsMsg((ANCS_SAVE_TYPE)(AncsData.AncsData[0]),AncsData.AncsData,len);		/*保存ANCS消息到Flash*/
	}
}

void InitAncsBuf_SmsAndCall(void)
{
	memset(AncsData_SmsAndCall.AncsData,0,256);
	AncsData_SmsAndCall.msg=0;
	AncsData_SmsAndCall.TimeOut=0;
}
void SaveAncsToBuf_ForAndroid_SmsAndCall(ANCS_SAVE_TYPE type,ANCS_SAVE_ID id, uint8* Data, uint8 Len, bool save)
{
	uint8 temp_len=0; 

	switch(id)
	{
		case ID_TITLE:
			InitAncsBuf_SmsAndCall();
			AncsData_SmsAndCall.AncsData[1] = Len;
			memcpy((AncsData_SmsAndCall.AncsData+4),Data,Len);
			break;
		case ID_MESSAGE:
			temp_len=Format_Gmail(Data, Len);
			Len=temp_len;
			AncsData_SmsAndCall.AncsData[2] = Len;
			memcpy((AncsData_SmsAndCall.AncsData+4+AncsData_SmsAndCall.AncsData[1]),Data,Len);			
			break;
		case ID_DATE:
			AncsData_SmsAndCall.AncsData[3] = Len;
			memcpy((AncsData_SmsAndCall.AncsData+4+AncsData_SmsAndCall.AncsData[1]+AncsData_SmsAndCall.AncsData[2]),Data,Len);				
			break;
		default:
			break;
	}
	AncsData_SmsAndCall.AncsData[0] = type;
	if(save)
	{
		uint8 len = AncsData_SmsAndCall.AncsData[1]+AncsData_SmsAndCall.AncsData[2]+AncsData_SmsAndCall.AncsData[3]+4;
		SaveAncsMsg((ANCS_SAVE_TYPE)(AncsData_SmsAndCall.AncsData[0]),AncsData_SmsAndCall.AncsData,len);		/*保存ANCS消息到Flash*/
	}
}

void SaveAncsToBuf(ANCS_SAVE_TYPE type,ANCS_SAVE_ID id, uint8* Data, uint8 Len)
{
	switch(id)
	{
		case ID_TITLE:
			InitAncsBuf();
			AncsData.AncsData[1] = Len;
			memcpy((AncsData.AncsData+4),Data,Len);
			break;
		case ID_MESSAGE:
			AncsData.AncsData[2] = Len;
			memcpy((AncsData.AncsData+4+AncsData.AncsData[1]),Data,Len);			
			break;
		case ID_DATE:
			AncsData.AncsData[3] = Len;
			memcpy((AncsData.AncsData+4+AncsData.AncsData[1]+AncsData.AncsData[2]),Data,Len);				
			break;
		default:
			break;
	}
	AncsData.AncsData[0] = type;
}

void SaveAncsToFlash_SendMsg(MsgType msg, uint16 Time)
{
	AncsData.msg = msg;
	AncsData.TimeOut = Time;
	SendMsg(MSG_ANCS_SAVE_TO_FLASH);
}

ANCS_SAVE_TYPE GetSocialType(char* st)
{
	ANCS_SAVE_TYPE TheType =Ancs_Null;
	if(strncmp(st, SOCIAL_SMS, strlen(SOCIAL_SMS)) == 0)
		TheType = Ancs_Sms;	 	
	else if(strncmp(st, SOCIAL_Facebook, strlen(SOCIAL_Facebook)) == 0)
		TheType = Ancs_Social_Facebook;	 	
	else if(strncmp(st, SOCIAL_Gmail_B, strlen(SOCIAL_Gmail_B)) == 0)
		TheType = Ancs_Social_Gmail;	 	
	else if(strncmp(st, SOCIAL_Gmail_L, strlen(SOCIAL_Gmail_L)) == 0)
		TheType = Ancs_Social_Gmail;	 	
	//else if(strncmp(st, SOCIAL_Hangouts, strlen(SOCIAL_Hangouts)) == 0)
		//TheType = Ancs_Social_Hangouts;	 
	else if(strncmp(st, SOCIAL_Instagram, strlen(SOCIAL_Instagram)) == 0)
		TheType = Ancs_Social_Instagram;	 
	else if(strncmp(st, SOCIAL_Line, strlen(SOCIAL_Line)) == 0)
		TheType = Ancs_Social_Line;	 
	//else if(strncmp(st, SOCIAL_Linkedin, strlen(SOCIAL_Linkedin)) == 0)
		//TheType = Ancs_Social_Linkedin;	 
	else if(strncmp(st, SOCIAL_Messenger, strlen(SOCIAL_Messenger)) == 0)
		TheType = Ancs_Social_Messenger;	 
	else if(strncmp(st, SOCIAL_QQ, strlen(SOCIAL_QQ)) == 0)
		TheType = Ancs_Social_QQ;	 
	//else if(strncmp(st, SOCIAL_Skype, strlen(SOCIAL_Skype)) == 0)
		//TheType = Ancs_Social_Skype;	 
	else if(strncmp(st, SOCIAL_Snapchat, strlen(SOCIAL_Snapchat)) == 0)
		TheType = Ancs_Social_Snapchat;	 
	else if(strncmp(st, SOCIAL_Twitter, strlen(SOCIAL_Twitter)) == 0)
		TheType = Ancs_Social_Twitter;	 
	//else if(strncmp(st, SOCIAL_Uber, strlen(SOCIAL_Uber)) == 0)
		//TheType = Ancs_Social_Uber;	 
	//else if(strncmp(st, SOCIAL_Viber, strlen(SOCIAL_Viber)) == 0)
		//TheType = Ancs_Social_Viber;	 
	else if(strncmp(st, SOCIAL_Wechat, strlen(SOCIAL_Wechat)) == 0)
		TheType = Ancs_Social_Wechat;	 
	else if(strncmp(st, SOCIAL_WhatsApp, strlen(SOCIAL_WhatsApp)) == 0)
		TheType = Ancs_Social_WhatsApp;	 	
	return TheType;
}

void Send_AncsConnectedGetAllInfo(void)
{
	NewMessageState = 0xFF;
	NotifCategoryIDType = 0xFF;
	ConnectedGetAllInfoFlag = 3;
	AncsCmdFlag = false;
	SendMsg(MSG_ANCS_CONNECTED_GET_ALL);  	
}


uint8  Format_Gmail(uint8 * pBuf, uint8 len)
{
	static uint8 temp_buf[255]={0};
	uint8 temp_len=0;
	uint8 i=0;
	uint8 j=0;
	uint16 timeout = 0x100;

	while((i<len) && (timeout != 0))
	{
		timeout--;
		if (pBuf[i] == 0xE2)
		{
			if (((pBuf[i+1] == 0x80) && (pBuf[i+2] >= 0x80) && (pBuf[i+2] <= 0xbf)) ||  ( (pBuf[i+1] == 0x81) && (pBuf[i+2] >= 0x80) && (pBuf[i+2] <= 0xbf)) ||  ( (pBuf[i+1] == 0x98) && (pBuf[i+2] >= 0x80) && (pBuf[i+2] <= 0xbf)))		
			{
				i += 3;
				
			}

		}
		else if ((pBuf[i] & 0xF0) == 0xF0) 
		{
			memcpy(&temp_buf[j], &pBuf[i], 4);
			i += 4;
			j += 4;
		}
		else if ((pBuf[i] & 0xF0) == 0xE0)
		{
			memcpy(&temp_buf[j], &pBuf[i], 3);
			i += 3;
			j += 3;
		}
		else if (((pBuf[i] & 0xF0) == 0xC0) || ((pBuf[i] & 0xF0) == 0xD0))	
		{
			memcpy(&temp_buf[j], &pBuf[i], 2);
			i += 2;
			j += 2;
		}
		else if (((pBuf[i] & 0x80) == 0x00) && (pBuf[i] != 0x00))	
		{

			memcpy(&temp_buf[j], &pBuf[i], 1);
			i += 1;
			j += 1;
		}

	}
	if(j!=0)
	{
		temp_len = j;
		memcpy(pBuf, temp_buf, temp_len);
	}
	else
		temp_len = len;

	return temp_len;

}

	
void GetNotifAttributesTitleInf(uint8 *pNotifData, uint8 len)
{
	if(pNotifData != NULL)
	{
		osal_stop_timerEx(GetAppTaskId(), MSG_ANCS_CONNECTED_GET_ALL);
		if(NewMessageState==NotificationAttributeIDAppIdentifier)
		{
			
			SocialType = GetSocialType((char*)(pNotifData));
			switch(SocialType)
			{
				case Ancs_Sms:					/*短信*/
					if(SWITCH_ANCS_SMS)
					{
						//WriteAncsUid(GlobalNotificationUID);		/* 保存短信  的UID  到flash */
						NewMessageState = NotificationAttributeIDTitle;
						SendGetNotifAttributesTitleCmd(GlobalNotificationUID, NotificationAttributeIDTitle);
						return;
					}
					break;

				case Ancs_Social_Facebook:		/*社交*/			
				case Ancs_Social_Gmail:
				case Ancs_Social_Hangouts:
				case Ancs_Social_Instagram:
				case Ancs_Social_Line:
				case Ancs_Social_Linkedin:
				case Ancs_Social_Messenger:
				case Ancs_Social_QQ:
				case Ancs_Social_Skype:
				case Ancs_Social_Snapchat:
				case Ancs_Social_Twitter:
				case Ancs_Social_Uber:
				case Ancs_Social_Viber:
				case Ancs_Social_Wechat:
				case Ancs_Social_WhatsApp:			
					if(SWITCH_ANCS_SOCIAL||((SocialType==Ancs_Social_Gmail)&&(SWITCH_ANCS_EMIL)))
					{
						if (SocialType == Ancs_Social_Messenger)
						{
							if (FaceBookMessageIDTitle == 0)
							{
								FaceBookMessageIDTitle = 1;
								osal_start_timerEx(GetAppTaskId(), MSG_FACEBOOK_MESSAGE_IDTITLE, 2000);  	
							}
						}
						if (SocialType == Ancs_Social_Skype)
						{

							if (SkeypeIDTitle == 0)
							{
								SkeypeIDTitle = 1;
								osal_start_timerEx(GetAppTaskId(), MSG_SKYPE_IDTITLE, 2000);  	
							}
						}
						if (SocialType == Ancs_Social_WhatsApp)
						{
							if (WhatsappIDTitle== 0)
							{
								WhatsappIDTitle = 1;
								osal_start_timerEx(GetAppTaskId(), MSG_WHATSAPP_IDTITLE, 2000);  	
							}
						}
						if (SocialType == Ancs_Social_QQ)
						{
							if (QQIDTitle== 0)
							{
								QQIDTitle = 1;
								osal_start_timerEx(GetAppTaskId(), MSG_QQ_IDTITLE, 2000);  	
							}
						}
						if (SocialType == Ancs_Social_Snapchat)
						{
							if (SnapchatTitle== 0)
							{
								SnapchatTitle = 1;
								osal_start_timerEx(GetAppTaskId(), MSG_SNAPCHAT_IDTITLE, 2000);  	
							}
						}						
						NewMessageState = NotificationAttributeIDTitle;
						SendGetNotifAttributesTitleCmd(GlobalNotificationUID, NotificationAttributeIDTitle);	
						return;
					}					
					break;
									
				default:
					break;
			}
			Send_AncsConnectedGetAllInfo();
		}
		
		else if(NewMessageState==NotificationAttributeIDTitle)
		{
			if(NotifCategoryIDType == CategoryIDIncomingCall)		/*来电*/
			{   
				if ((*(pNotifData) == 0xE2) && (*(pNotifData+1) == 0x80) && (*(pNotifData+2) == 0xAD))
				{
					osal_memcpy(pNotifData,pNotifData+3,13);
					len -= 6;
				}			
				if((*(pNotifData+3) == 0x2D) &&(*(pNotifData+8) == 0x2D))
				{
					if(len == 13)
					{
						osal_memcpy(pNotifData+3,pNotifData+4,9);
						osal_memcpy(pNotifData+7,pNotifData+8,4);
					}
					SaveAncsToBuf(Ancs_IncomingCall,ID_TITLE,pNotifData,(len -2));
				}
				else if(strncmp((char*)pNotifData, UNKNOWN_NAME, strlen(UNKNOWN_NAME))==0)
				{
					SaveAncsToBuf(Ancs_IncomingCall,ID_TITLE,"Unknown Name",12);
				}
				else
					SaveAncsToBuf(Ancs_IncomingCall,ID_TITLE,pNotifData,len);

				ChargeDisplayStateChange(false);        
				SaveAncsToFlash_SendMsg(MSG_INCOMING_CALL_NOTIFY,0);
				return;
			}
			
			else if(NotifCategoryIDType == CategoryIDMissedCall)	/*未接来电*/
			{        
				/* 保存未接来电姓名或号码到flash */
				SocialType =Ancs_MissedCall; 
				if ((*(pNotifData) == 0xE2) && (*(pNotifData+1) == 0x80) && (*(pNotifData+2) == 0xAD))
				{
					osal_memcpy(pNotifData,pNotifData+3,13);
					len -= 6;
				}				
				if((*(pNotifData+3) == 0x2D) &&(*(pNotifData+8) == 0x2D))
				{
					if(len == 13)
					{
						osal_memcpy(pNotifData+3,pNotifData+4,9);
						osal_memcpy(pNotifData+7,pNotifData+8,4);
					}
					SaveAncsToBuf(Ancs_MissedCall,ID_TITLE,pNotifData,(len -2));
				}
				else if(strncmp((char*)pNotifData, UNKNOWN_NAME, strlen(UNKNOWN_NAME))==0)
				{
					SaveAncsToBuf(Ancs_MissedCall,ID_TITLE,"Unknown Name",12);
				}
				else
					SaveAncsToBuf(Ancs_MissedCall,ID_TITLE,pNotifData,len);

				NewMessageState = NotificationAttributeIDDate;
				SendGetNotifAttributesTitleCmd(GlobalNotificationUID, NotificationAttributeIDDate);
				return;
			}
			else if(NotifCategoryIDType == CategoryIDEmail)	/*邮件*/
			{        
				/* 保存Email到flash */
				SocialType =Ancs_Email;
				SaveAncsToBuf(Ancs_Email,ID_TITLE,pNotifData,len);
				NewMessageState = NotificationAttributeIDMessage;
				SendGetNotifAttributesTitleCmd(GlobalNotificationUID, NotificationAttributeIDMessage);
				return;
			}	
			else if(NotifCategoryIDType == CategoryIDSchedule)		/*日程*/
			{
				/* 保存日程il到flash */
				SocialType =Ancs_Schedule;
				SaveAncsToBuf(Ancs_Schedule,ID_TITLE,pNotifData,len);
				SaveAncsToBuf(Ancs_Schedule,ID_MESSAGE,pNotifData,len);
				NewMessageState = NotificationAttributeIDDate;
				SendGetNotifAttributesTitleCmd(GlobalNotificationUID, NotificationAttributeIDDate);		
			}
			else
			{
				switch(SocialType)
				{
					case Ancs_Sms:					/*短信*/
						SaveAncsToBuf(Ancs_Sms,ID_TITLE,pNotifData,len);
						//WriteSenderInfo((char*)pNotifData, len);		/* 保存短信发送者后五个字节到flash */
						NewMessageState = NotificationAttributeIDMessage;
						SendGetNotifAttributesTitleCmd(GlobalNotificationUID, NotificationAttributeIDMessage);
						return;
						
					case Ancs_Social_Facebook:		/*社交*/
					case Ancs_Social_Gmail:
					case Ancs_Social_Hangouts:
					case Ancs_Social_Instagram:
					case Ancs_Social_Line:
					case Ancs_Social_Linkedin:
					case Ancs_Social_Messenger:
					case Ancs_Social_QQ:
					case Ancs_Social_Skype:
					case Ancs_Social_Snapchat:
					case Ancs_Social_Twitter:
					case Ancs_Social_Uber:
					case Ancs_Social_Viber:
					case Ancs_Social_Wechat:
					case Ancs_Social_WhatsApp:	
						if (SocialType == Ancs_Social_Messenger)
						{
							FaceBookMessageIDTitle = 0;
							osal_stop_timerEx(GetAppTaskId(),MSG_FACEBOOK_MESSAGE_IDTITLE);
						}
						if (SocialType == Ancs_Social_Skype)
						{
							SkeypeIDTitle = 0;
							osal_stop_timerEx(GetAppTaskId(),MSG_SKYPE_IDTITLE);
						}

						if(SocialType==Ancs_Social_WhatsApp)
						{
							WhatsappIDTitle= 0;
							osal_stop_timerEx(GetAppTaskId(),MSG_WHATSAPP_IDTITLE);
							if((pNotifData[0]==0xE2)&&(pNotifData[1]==0x80)&&(pNotifData[2]==0x8E))
							{
								pNotifData=pNotifData+3;
							}
						}
						if (SocialType == Ancs_Social_QQ)
						{
							QQIDTitle = 0;
							osal_stop_timerEx(GetAppTaskId(),MSG_QQ_IDTITLE);
						}
						if (SocialType == Ancs_Social_Snapchat)
						{
							SnapchatTitle = 0;
							osal_stop_timerEx(GetAppTaskId(),MSG_SNAPCHAT_IDTITLE);
						}						
						SaveAncsToBuf(SocialType,ID_TITLE,pNotifData,len);
						NewMessageState = NotificationAttributeIDMessage;
						SendGetNotifAttributesTitleCmd(GlobalNotificationUID, NotificationAttributeIDMessage);		
						return;
						
					default:
						break;
				}
				Send_AncsConnectedGetAllInfo();
			}
		}
		else if(NewMessageState==NotificationAttributeIDMessage)
		{
			uint8 temp_len=0;
			switch(SocialType)
			{
				case Ancs_Sms:					/*短信*/
					temp_len=Format_Gmail(pNotifData, len);
					len=temp_len;
					SaveAncsToBuf(Ancs_Sms,ID_MESSAGE,pNotifData,len);
					NewMessageState = NotificationAttributeIDDate;
					SendGetNotifAttributesTitleCmd(GlobalNotificationUID, NotificationAttributeIDDate);
					return;
					
				case Ancs_Email:					/*邮件*/	
					SaveAncsToBuf(Ancs_Email,ID_MESSAGE,pNotifData,len);
					NewMessageState = NotificationAttributeIDDate;
					SendGetNotifAttributesTitleCmd(GlobalNotificationUID, NotificationAttributeIDDate);				
					return;	
					
				case Ancs_Schedule:				/*日程*/		
					return;	
																		
				case Ancs_Social_Facebook:		/*社交*/
				case Ancs_Social_Gmail:
				case Ancs_Social_Hangouts:
				case Ancs_Social_Instagram:
				case Ancs_Social_Line:
				case Ancs_Social_Linkedin:
				case Ancs_Social_Messenger:
				case Ancs_Social_QQ:
				case Ancs_Social_Skype:
				case Ancs_Social_Snapchat:
				case Ancs_Social_Twitter:
				case Ancs_Social_Uber:
				case Ancs_Social_Viber:
				case Ancs_Social_Wechat:
				case Ancs_Social_WhatsApp:
					
					if((SocialType==Ancs_Social_Gmail) || (SocialType==Ancs_Social_WhatsApp))
					{
						uint8 temp_len=Format_Gmail(pNotifData, len);
						len=temp_len;
					}

					SaveAncsToBuf(SocialType,ID_MESSAGE,pNotifData,len);
					NewMessageState = NotificationAttributeIDDate;
					SendGetNotifAttributesTitleCmd(GlobalNotificationUID, NotificationAttributeIDDate);	
					return;
					
				default:
					break;
			}
			Send_AncsConnectedGetAllInfo();
		}
		else if(NewMessageState==NotificationAttributeIDDate)
		{	
			switch(SocialType)
			{
				case Ancs_MissedCall:				/*未接来电*/
					SaveAncsToBuf(Ancs_MissedCall,ID_DATE,pNotifData,len);
					ChargeDisplayStateChange(false);                        
					SaveAncsToFlash_SendMsg(MSG_MISSED_CALL_COUNT, 1000);
					return;
					
				case Ancs_Sms:					/*短信*/
					SaveAncsToBuf(Ancs_Sms,ID_DATE,pNotifData,len);
					ChargeDisplayStateChange(false);      
					SMS_FROM_ANDROID = false;
					SaveAncsToFlash_SendMsg(MSG_SMS_NOTIFY,100);
					return;
					
				case Ancs_Email:					/*邮件*/	
					SaveAncsToBuf(Ancs_Email,ID_DATE,pNotifData,len);
					ChargeDisplayStateChange(false);      
					SaveAncsToFlash_SendMsg(MSG_E_MAIL_NOTIFY,100);				
					return;
					
				case Ancs_Schedule:				/*日程*/	
					SaveAncsToBuf(Ancs_Schedule,ID_DATE,pNotifData,len);
					ChargeDisplayStateChange(false);      
					SaveAncsToFlash_SendMsg(MSG_DAILY_SCHEDULE_NOTIFY,100);				
					return;				
					
				case Ancs_Social_Facebook:		/*社交*/
				case Ancs_Social_Gmail:
				case Ancs_Social_Hangouts:
				case Ancs_Social_Instagram:
				case Ancs_Social_Line:
				case Ancs_Social_Linkedin:
				case Ancs_Social_Messenger:
				case Ancs_Social_QQ:
				case Ancs_Social_Skype:
				case Ancs_Social_Snapchat:
				case Ancs_Social_Twitter:
				case Ancs_Social_Uber:
				case Ancs_Social_Viber:
				case Ancs_Social_Wechat:
				case Ancs_Social_WhatsApp:
					SaveAncsToBuf(SocialType,ID_DATE,pNotifData,len);
					ChargeDisplayStateChange(false);      
					SaveAncsToFlash_SendMsg(MSG_SOCIAL_NOTIFY,100);
					return;
					
				default:
					break;
			}	
			Send_AncsConnectedGetAllInfo();
		}
	}
}


void SetSocialNotifEventFlag(void)
{
        UidStoreFlag = false;
        AncsTypeNumber.SocialNumberAdd = false;
        AncsTypeNumber.SocialNumberRemove = false;
}


void AncsRemoveFunction(void)
{
        uint8 ret = ReadAncsUid(GlobalNotificationUIDRemove1,GlobalNotificationUID,QueryValue);
        if(ret == 1)
        { 
                if(SWITCH_ANCS_SMS)
                {
                        osal_start_timerEx(GetAppTaskId(), MSG_ANCS_SMS_QUERY_UID, 200);
                        ChargeDisplayStateChange(false);
                }
        }
        else if(ret == 2)
        {
                return;
        }
        else
        {
                if(SWITCH_ANCS_SOCIAL)
                {
                		if (gSocialCount != 0)
                		{
						
	                        if(SocialNumberTemp > gShortMsgCount)
	                                gSocialCount = SocialNumberTemp - gShortMsgCount;
	                        else
	                                gSocialCount = 0;
	                        //ChargeDisplayStateChange(false);
	                        //SendMsg(MSG_SOCIAL_NOTIFY);
                		}
                }
        }
        SetSocialNotifEventFlag();
}


void RemoveSMS(void)
{
        uint8 position[1];

        if(GetSenderInfo(position, QueryValue[0]))
        {
                if(gShortMsgCount>0)
                        gShortMsgCount -= 1;
                else
                        gShortMsgCount = 0;
                ResetSMSuid(position[0]);                
                osal_start_timerEx(GetAppTaskId(), MSG_ANCS_SMS_QUERY_UID, 100);
        }
        else
        {
                if((QuerySMSOffset() == true) ||(SocialNumberTemp == 0))
                {
                        gShortMsgCount = 0;
                }
                osal_start_timerEx(GetAppTaskId(), MSG_ANCS_SMS_RESET_SENDER, 100);
        }
}


void ResetSendInfo(void)
{
        static uint8 Resetflag = true;
        if(Resetflag)
        {
                Resetflag = false;
                ResetSenderID(QueryValue[0]);
                osal_start_timerEx(GetAppTaskId(), MSG_ANCS_SMS_RESET_SENDER, 100);
        }
        else
        {
                Resetflag = true;
                ResetSenderInfo(QueryValue[0]);
                SendMsg(MSG_SMS_NOTIFY);
        }
}


bool AncsCmdFlag=false;

void AncsIncomingcalledGetInfo(void)
{
		if (!AncsCmdFlag)
		{
	        NewMessageState=NotificationAttributeIDTitle;
	        NotifCategoryIDType = CategoryIDIncomingCall;
	        SendGetNotifAttributesTitleCmd(GlobalNotificationUID, NotificationAttributeIDTitle);
			AncsCmdFlag = true;
			osal_start_timerEx(GetAppTaskId(), MSG_ClEAR_ANCS_CMD, 5000);  
		}
}


void AncsMisscalledGetInfo(void)
{
		if (!AncsCmdFlag)
		{
	        NewMessageState=NotificationAttributeIDTitle;
	        NotifCategoryIDType = CategoryIDMissedCall;
	        SendGetNotifAttributesTitleCmd(GlobalNotificationUID, NotificationAttributeIDTitle);
			AncsCmdFlag = true;
			osal_start_timerEx(GetAppTaskId(), MSG_ClEAR_ANCS_CMD, 5000);  
		}
}

void AncsEmailGetInfo(void)
{
		if (!AncsCmdFlag)
		{
	        NewMessageState=NotificationAttributeIDTitle;
	        NotifCategoryIDType = CategoryIDEmail;
	        SendGetNotifAttributesTitleCmd(GlobalNotificationUID, NotificationAttributeIDTitle);
			AncsCmdFlag = true;
			osal_start_timerEx(GetAppTaskId(), MSG_ClEAR_ANCS_CMD, 5000);  
		}
}

void AncsScheduleGetInfo(void)
{
	if (!AncsCmdFlag)
	{
		NewMessageState=NotificationAttributeIDTitle;
		NotifCategoryIDType = CategoryIDSchedule;
		SendGetNotifAttributesTitleCmd(GlobalNotificationUID, NotificationAttributeIDTitle);
		AncsCmdFlag = true;
		osal_start_timerEx(GetAppTaskId(), MSG_ClEAR_ANCS_CMD, 5000);  
	}

}

void AncsSmsGetInfo(void)
{
		if (!AncsCmdFlag)
		{
	        AncsTypeNumber.SocialNumberAdd=true;
	        NewMessageState=NotificationAttributeIDAppIdentifier;
	        NotifCategoryIDType = CategoryIDSocial;
	        SendGetNotifAttributesTitleCmd(GlobalNotificationUID, NotificationAttributeIDAppIdentifier);
			AncsCmdFlag = true;
			osal_start_timerEx(GetAppTaskId(), MSG_ClEAR_ANCS_CMD, 5000);  
		}
}


void IncomingcallPollGetInfo(void)
{
        if(IncomingUidTempIndex > 0)
        {
                GlobalNotificationUID[0] = *(IncomingcallUidTempArray+0);
                GlobalNotificationUID[1] = *(IncomingcallUidTempArray+1);
                GlobalNotificationUID[2] = 0;
                GlobalNotificationUID[3] = 0;
                
                IncomingUidTempIndex -= 2;
                if(IncomingcallUidTempArray)
                        osal_memcpy(IncomingcallUidTempArray,IncomingcallUidTempArray+2,IncomingUidTempIndex);
                osal_start_timerEx(GetAppTaskId(), MSG_ANCS_INCOMINGCALL_LATER_GET, 100);  
        }
        else
        {
                if(IncomingcallUidTempArray)
                        osal_mem_free(IncomingcallUidTempArray);
                IncomingcallUidTempArray=NULL;
        }
         
}


void MisscallPollGetInfo(void)
{
        if(MisscallUidTempIndex > 0)
        {
                GlobalNotificationUID[0] = *(MisscallUidTempArray+0);
                GlobalNotificationUID[1] = *(MisscallUidTempArray+1);
                GlobalNotificationUID[2] = 0;
                GlobalNotificationUID[3] = 0;
                
                MisscallUidTempIndex -= 2;
                if(MisscallUidTempIndex)
                        osal_memcpy(MisscallUidTempArray,MisscallUidTempArray+2,MisscallUidTempIndex);
                osal_start_timerEx(GetAppTaskId(), MSG_ANCS_MISSCALL_LATER_GET, 100);  
        }
        else
        {
                if(MisscallUidTempArray)
                        osal_mem_free(MisscallUidTempArray);
                MisscallUidTempArray=NULL;
        }
         
}

void EmailPollGetInfo(void)
{
        if(EmailUidTempIndex > 0)
        {
                GlobalNotificationUID[0] = *(EmailUidTempArray+0);
                GlobalNotificationUID[1] = *(EmailUidTempArray+1);
                GlobalNotificationUID[2] = 0;
                GlobalNotificationUID[3] = 0;
                
                EmailUidTempIndex -= 2;
                if(EmailUidTempIndex)
                        osal_memcpy(EmailUidTempArray,EmailUidTempArray+2,EmailUidTempIndex);
                osal_start_timerEx(GetAppTaskId(), MSG_ANCS_EMAIL_LATER_GET, 100);  
        }
        else
        {
                if(EmailUidTempArray)
                        osal_mem_free(EmailUidTempArray);
                EmailUidTempArray=NULL;
        }
         
}

void SchedulePollGetInfo(void)
{
        if(ScheduleUidTempIndex > 0)
        {
                GlobalNotificationUID[0] = *(ScheduleUidTempArray+0);
                GlobalNotificationUID[1] = *(ScheduleUidTempArray+1);
                GlobalNotificationUID[2] = 0;
                GlobalNotificationUID[3] = 0;
                
                ScheduleUidTempIndex -= 2;
                if(ScheduleUidTempIndex)
                        osal_memcpy(ScheduleUidTempArray,ScheduleUidTempArray+2,ScheduleUidTempIndex);
                osal_start_timerEx(GetAppTaskId(), MSG_ANCS_SCHEDULE_LATER_GET, 100);  
        }
        else
        {
                if(ScheduleUidTempArray)
                        osal_mem_free(ScheduleUidTempArray);
                ScheduleUidTempArray=NULL;	
        }
         
}

void SmsPollGetInfo(void)
{
        if(SmsUidTempIndex > 0)
        {
                GlobalNotificationUID[0] = *(SmsUidTempArray+0);
                GlobalNotificationUID[1] = *(SmsUidTempArray+1);
                GlobalNotificationUID[2] = 0;
                GlobalNotificationUID[3] = 0;
                
                SmsUidTempIndex -= 2;
                if(SmsUidTempIndex)
                        osal_memcpy(SmsUidTempArray,SmsUidTempArray+2,SmsUidTempIndex);
                osal_start_timerEx(GetAppTaskId(), MSG_ANCS_SMS_LATER_GET, 100);   
        }
        else
        {
                if(SmsUidTempArray)
                        osal_mem_free(SmsUidTempArray);
                SmsUidTempArray=NULL;
        }
        
}


void AncsConnectedGetAllInfo(void)
{
        if(osal_stop_timerEx(GetAppTaskId(), MSG_SEND_SOCIAL_MESSAGE)==SUCCESS)
                SetSocialNotifEventFlag();
        if(ConnectedGetAllInfoFlag==0)
        {
                NewMessageState = 0xFF;
                NotifCategoryIDType = 0xFF;
                ConnectedGetAllInfoFlag=3;
        }
        osal_start_timerEx(GetAppTaskId(), MSG_ANCS_CONNECTED_GET_ALL, 5000);   

        if(NotifCategoryIDType==CategoryIDIncomingCall)
                AncsIncomingcalledGetInfo();
        else if(NotifCategoryIDType==CategoryIDMissedCall)
                AncsMisscalledGetInfo();
        else if(NotifCategoryIDType== CategoryIDSocial)
                AncsSmsGetInfo();
	 else if(NotifCategoryIDType== CategoryIDEmail)
	 	  AncsEmailGetInfo();
	 else if(NotifCategoryIDType==CategoryIDSchedule)
		  AncsScheduleGetInfo();
        else if(IncomingcallUidTempArray)
                IncomingcallPollGetInfo();
        else if(MisscallUidTempArray)
                MisscallPollGetInfo();      
        else if(SmsUidTempArray)
                SmsPollGetInfo();
	 else if(EmailUidTempArray)
	 	  EmailPollGetInfo();  
	 else if(ScheduleUidTempArray)
	 	  SchedulePollGetInfo();  	 
        else
        {   
                ConnectedGetAllInfoFlag=0;
                osal_stop_timerEx(GetAppTaskId(), MSG_ANCS_CONNECTED_GET_ALL);
        }
        
        if(ConnectedGetAllInfoFlag > 1)
                ConnectedGetAllInfoFlag --;
        else
        {
                NewMessageState = 0xFF;
                NotifCategoryIDType = 0xFF;
                ConnectedGetAllInfoFlag=0;
                SetSocialNotifEventFlag();
                if(SmsUidTempArray)
                {
                        osal_mem_free(SmsUidTempArray);
                        SmsUidTempArray=NULL;
                        SmsUidTempIndex = 0;
                }
                if(MisscallUidTempArray)
                {
                        osal_mem_free(MisscallUidTempArray);
                        MisscallUidTempArray=NULL;
                        MisscallUidTempIndex = 0;
                }
				
		  if(EmailUidTempArray)
                {
                        osal_mem_free(EmailUidTempArray);
                        EmailUidTempArray=NULL;
                        EmailUidTempIndex = 0;
                }	
		  
		  if(ScheduleUidTempArray)
                {
                        osal_mem_free(ScheduleUidTempArray);
                        ScheduleUidTempArray=NULL;
                        ScheduleUidTempIndex = 0;
                }		
		  		  
                if(IncomingcallUidTempArray)
                {
                        osal_mem_free(IncomingcallUidTempArray);
                        IncomingcallUidTempArray=NULL;
                        IncomingUidTempIndex = 0;
                }
                
        }
}


void FreeReceiveBuff(void)
{
        NewMessageState = 0xFF;
        NotifCategoryIDType = 0xFF;
        ConnectedGetAllInfoFlag=0;
        SetSocialNotifEventFlag();
        if(SmsUidTempArray)
        {
                osal_mem_free(SmsUidTempArray);
                SmsUidTempArray=NULL;
                SmsUidTempIndex = 0;
        }
        if(MisscallUidTempArray)
        {
                osal_mem_free(MisscallUidTempArray);
                MisscallUidTempArray=NULL;
                MisscallUidTempIndex = 0;
        }
	if(EmailUidTempArray)
	{
		osal_mem_free(EmailUidTempArray);
		EmailUidTempArray=NULL;
		EmailUidTempIndex = 0;
	}
  	if(ScheduleUidTempArray)
        {
                osal_mem_free(ScheduleUidTempArray);
                ScheduleUidTempArray=NULL;
                ScheduleUidTempIndex = 0;
        }		
        if(IncomingcallUidTempArray)
        {
                osal_mem_free(IncomingcallUidTempArray);
                IncomingcallUidTempArray=NULL;
                IncomingUidTempIndex = 0;
        }
}

void FacebookMessageSet(void)
{
	SocialType =Ancs_Social_Messenger;
	NotifCategoryIDType = CategoryIDSocial;
	NewMessageState=NotificationAttributeIDTitle;
}

void SkypeSet(void)
{
	SocialType = Ancs_Social_Skype;
	NotifCategoryIDType = CategoryIDSocial;
	NewMessageState=NotificationAttributeIDTitle;
}

void WhatsappSet(void)
{
	SocialType = Ancs_Social_WhatsApp;
	NotifCategoryIDType = CategoryIDSocial;
	NewMessageState=NotificationAttributeIDTitle;
}

void QQSet(void)
{
	SocialType = Ancs_Social_QQ;
	NotifCategoryIDType = CategoryIDSocial;
	NewMessageState=NotificationAttributeIDTitle;
}

void SnapchatSet(void)
{
	SocialType = Ancs_Social_Snapchat;
	NotifCategoryIDType = CategoryIDSocial;
	NewMessageState=NotificationAttributeIDTitle;
}

