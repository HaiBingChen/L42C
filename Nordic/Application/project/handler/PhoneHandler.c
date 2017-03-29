
#include "headfile.h"

PhoneHadlerTAG PhoneHadlerId;
struct PhoneStateList* pCurrentPhoneState=NULL;  /*用于保存来电姓名、号码、时间等信息*/
bool SkipKeyMsg = FALSE;                                       /*某些界面需要显示数秒钟如拒接、这时不响应按键消息*/
bool SkipIdleKeyMsg = FALSE;     
bool FreeState =FALSE;                                           /*RingHandler里决定是否要在Unload的时候释放内存*/

bool PhoneCallHandlerExist = FALSE;
bool PhoneRingHandlerExist = FALSE;
bool PhoneCallOutHandlerExist = FALSE;
bool PhoneDisplayHandlerExist = FALSE;
bool	PhonePrepareHandlerExit = FALSE;

uint8 Phone_DispalyType;
MsgType UsbType = MSG_HANDLER_BEGIN;
uint8 StopToneCounts = 0;

#define CALLER_ID_IN    	0x00
#define CALLER_ID_OUT    0x01
#define CALLER_ID_NULL 	0xFF
uint8 CallerIDFromApk = CALLER_ID_NULL;
uint8* CallerIDApk=NULL;
uint8 CallOutID = 0;

uint8 CallerID_Data[256];	
uint8 CallerID_NameData[128];
uint8 CallerID_NameData_len=0;
uint8 AncsDelaySaveNotice=0;


void AncsDelaySend(void)
{
	if (AncsDelaySaveNotice & ANCS_DELAY_SKIP_SOCIAL)
	{
		AncsDelaySaveNotice &= ~ANCS_DELAY_SKIP_SOCIAL;
		if(SWITCH_ANCS_SOCIAL) 
        			osal_start_timerEx(GetAppTaskId(), MSG_SOCIAL_NOTIFY, 100);
	}
	if (AncsDelaySaveNotice & ANCS_DELAY_SKIP_EMAIL)
	{
		AncsDelaySaveNotice &= ~ANCS_DELAY_SKIP_EMAIL;
		if(SWITCH_ANCS_EMIL)
        			osal_start_timerEx(GetAppTaskId(), MSG_E_MAIL_NOTIFY, 200);
	}
	if (AncsDelaySaveNotice & ANCS_DELAY_SKIP_MISSED_CALL)
	{
		AncsDelaySaveNotice &= ~ANCS_DELAY_SKIP_MISSED_CALL;
		if(SWITCH_ANCS_MISSED_CALL) 
        			osal_start_timerEx(GetAppTaskId(), MSG_MISSED_CALL_COUNT, 300);
	}
	if (AncsDelaySaveNotice & ANCS_DELAY_SKIP_SMS)
	{
		AncsDelaySaveNotice &= ~ANCS_DELAY_SKIP_SMS;
		if(SWITCH_ANCS_SMS) 
        			osal_start_timerEx(GetAppTaskId(), MSG_SMS_NOTIFY, 400);
	}
	if (AncsDelaySaveNotice & ANCS_DELAY_SKIP_SCHEDULE)
	{
		AncsDelaySaveNotice &= ~ANCS_DELAY_SKIP_SCHEDULE;
		if(SWITCH_ANCS_SCHEDULE) 
        			osal_start_timerEx(GetAppTaskId(), MSG_DAILY_SCHEDULE_NOTIFY, 500);
	}	
	AncsDelaySaveNotice =0;
}

void AncsDelaySet(MsgType msg)
{
	switch(msg)
	{
		case MSG_SMS_NOTIFY:
			AncsDelaySaveNotice |= ANCS_DELAY_SKIP_SMS;	
			break;
		case MSG_MISSED_CALL_COUNT:
			AncsDelaySaveNotice |= ANCS_DELAY_SKIP_MISSED_CALL;
			break;
		case MSG_INCOMING_CALL_NOTIFY:
			break;
		case MSG_E_MAIL_NOTIFY:
			AncsDelaySaveNotice |= ANCS_DELAY_SKIP_EMAIL;
			break;
		case MSG_DAILY_SCHEDULE_NOTIFY:
			AncsDelaySaveNotice |= ANCS_DELAY_SKIP_SCHEDULE;
			break;
		case MSG_SOCIAL_NOTIFY:	
			AncsDelaySaveNotice |= ANCS_DELAY_SKIP_SOCIAL;
			break;
		default:
			break;
	}
}



void SaveCallerIDToBuf(PhoneType Type,CALLER_ID id, uint8* Data, uint8 Len, bool save)
{
	switch(id)
	{
		case CID_NUM_DATE:							/*号码 日期*/
			{
				if((osal_stop_timerEx(GetAppTaskId(), MSG_SAVE_CALLER_ID_TO_FLASH)==SUCCESS)&&(CallerID_Data[1]!=0x00))
				{
					uint8 len = CallerID_Data[1]+CallerID_Data[2]+CallerID_Data[3]+4;
					SaveAncsMsg(Call_Record,CallerID_Data,len);								/*号码被中断，保存CallerID到Flash*/
				}
				memset(CallerID_Data,0,256);
				if(Data[0]==0x00)
					return;
				CallerID_Data[0] = (uint8)Type+0x50;
				CallerID_Data[1] = Len;
				memcpy((CallerID_Data+4),Data,Len);	
				
				UTCTimeStruct uTime;
				gOS_GetUTCTime(&uTime);
				uint8 offset =4+CallerID_Data[1];
				CallerID_Data[2] = 7;
				CallerID_Data[offset] = uTime.year>>8;
				CallerID_Data[offset+1] = uTime.year&0xff;
				CallerID_Data[offset+2] = uTime.month;
				CallerID_Data[offset+3] = uTime.day;
				CallerID_Data[offset+4] = uTime.hour;
				CallerID_Data[offset+5] = uTime.minutes;
				CallerID_Data[offset+6] = uTime.seconds;								
			}
			break;
			
		case CID_NAME:						/*姓名*/
			if(Len>0)
			{
				if(Len>128)
					Len=128;
				CallerID_NameData_len =Len;
				memset(CallerID_NameData,0,128);
				memcpy(CallerID_NameData,Data,Len);
			}
			break;
			
		default:
			break;
	}	
	if((save)&&(CallerID_Data[1]!=0x00))
	{	
		uint8 len=0;
		CallerID_Data[3] =CallerID_NameData_len;
		len = CallerID_Data[1]+CallerID_Data[2]+CallerID_Data[3]+4;
		if(CallerID_Data[3]>0)
		{
			memcpy((CallerID_Data+4+CallerID_Data[1]+CallerID_Data[2]),CallerID_NameData,CallerID_Data[3]);
			CallerID_NameData_len=0;
		}
		SaveAncsMsg(Call_Record,CallerID_Data,len);										/*保存CallerID到Flash*/
		memset(CallerID_Data,0,256);
	}
}

void DisplayPhoneCallTime(uint16 second)
{
	uint8 sendbuf[2]={0x00};
	uint16_encode(second,sendbuf);
	LCD_MainDataInterface(MI_PHONE_CALL_TIME,sendbuf,2);		     
}

void PhoneDisplay(PhoneDisplayStatus status)
{
	uint8* Name  =NULL;
	uint8* Number =NULL;
	PhoneType Type =PHONE_1ST_IN;
	uint8 Name_Len=0,Num_len=0;	
	uint8 Phonebuf[128]={0x00};
	if(pCurrentPhoneState!=NULL)
	{
		Name  = pCurrentPhoneState->Name;
		Number= pCurrentPhoneState->Num;
		Type = pCurrentPhoneState->type;
		Name_Len = strlen((char*)Name);
		Num_len =   strlen((char*)Number);
		Phonebuf[0]=status;
		Phonebuf[1]=(uint8)Type;
		if(Num_len){
			memcpy(Phonebuf+4,Number,Num_len);
			Phonebuf[4+Num_len]=0;
			Num_len++;
		}	
		if(Name_Len)
		{
			memcpy(Phonebuf+4+Num_len,Name,Name_Len);
			Phonebuf[4+Num_len+Name_Len]=0;
			Name_Len++;
		}
		Phonebuf[2]=Num_len;
		Phonebuf[3]=Name_Len;
		if(status==PHONE_IN_OUT_CALL)
		{
			uint8 buf1[2]={0x00};
			uint8 buf2[2]={0x00};
			uint32 TimeTemp=0;
			uint16 secondTemp =0;
			TimeTemp=GetTimeStamp();
			secondTemp =(uint16)(TimeTemp-pCurrentPhoneState->CallTime);
			buf1[0] =PhoneMic.PhoneMicStatus;
			buf1[1] =0x00;
			uint16_encode(secondTemp,buf2);
			LCD_MainDataInterface_SetBL_Step(MI_PHONE_Mic_STATUS,buf1, 2, 0);	
			LCD_MainDataInterface_SetBL_Step(MI_PHONE_CALL,Phonebuf,Num_len+Name_Len+4,0);
			LCD_MainDataInterface_SetBL_Step(MI_PHONE_CALL_TIME,buf2,2,1);
		}
		else if(status==PHONE_IN_RING)
		{
			uint8 buf1[2]={0x00};
			buf1[0] =PhoneSound.PhoneSoundStatus;
			buf1[1] =0x00;
			LCD_MainDataInterface_SetBL_Step(MI_PHONE_SOUND_STATUS,buf1, 2, 0);	
			LCD_MainDataInterface_SetBL_Step(MI_PHONE_CALL,Phonebuf,Num_len+Name_Len+4,1);	
		}
		else
			LCD_MainDataInterface(MI_PHONE_CALL,Phonebuf,Num_len+Name_Len+4);		
	}
	if(status==PHONE_IN_OUT_END||status==PHONE_IN_REJECT||status==PHONE_OUT_CANCEL)
		osal_start_timerEx(GetAppTaskId(), MSG_SAVE_CALLER_ID_TO_FLASH, 100);
} 

bool MemAlloc_CallerInfo(PhoneType Type,uint8 *cInfo)
{
	uint8 num_len= 0; 
	uint8 name_len= 0;
	struct PhoneStateList* CallInfo = NULL;
	if((Type==PHONE_1ST_IN ||Type==PHONE_OUT) && (pCurrentPhoneState!= NULL))
		return FALSE;
	if(Type==PHONE_2ND_IN &&(pCurrentPhoneState->pNext!=pCurrentPhoneState))
		return FALSE;
	if(Type!=PHONE_OUT)
	{	          
		num_len= *(cInfo+1); 
		name_len= *(cInfo+2);
		cInfo =cInfo+3;
	}
	CallInfo = osal_mem_alloc(sizeof(struct PhoneStateList));
    	if(CallInfo == NULL) 
       	return FALSE;
		
/*号码*/		
	CallInfo->Num= osal_mem_alloc(num_len+1);
	if(CallInfo->Num ==NULL )
	{
		osal_mem_free(CallInfo);
		return FALSE;
	}
	memset(CallInfo->Num,0,num_len+1);
	if(num_len>0)
    		memcpy(CallInfo->Num,cInfo,num_len);
	
/*名字*/	
	if(name_len==0 && ((Type==PHONE_OUT && CallerIDFromApk==CALLER_ID_OUT)||(Type!=PHONE_OUT && CallerIDFromApk==CALLER_ID_IN)))
	{
		CallInfo->Name = CallerIDApk;
		CallerIDFromApk = CALLER_ID_NULL;
	}
	else{
		CallInfo->Name= osal_mem_alloc(name_len+1);
		if(CallInfo->Name == NULL) 
		{
			osal_mem_free(CallInfo->Num);
			osal_mem_free(CallInfo);
			return FALSE;
		}	
		cInfo+=num_len;
		memset(CallInfo->Name,0,name_len+1);	
		if(name_len>0)
			memcpy(CallInfo->Name,cInfo,name_len);
			
	}
	
	CallInfo->pNext =CallInfo;
	if(Type==PHONE_2ND_IN)
	{
		CallInfo->pNext = pCurrentPhoneState;
		pCurrentPhoneState->pNext = CallInfo;			
	}
	pCurrentPhoneState = CallInfo;
	pCurrentPhoneState->type = Type; 
	pCurrentPhoneState->KeyCode = 0;
	pCurrentPhoneState->CallTime = GetTimeStamp();
	
	SaveCallerIDToBuf(pCurrentPhoneState->type,CID_NUM_DATE,pCurrentPhoneState->Num,strlen((char*)(pCurrentPhoneState->Num)),false);
	SaveCallerIDToBuf(pCurrentPhoneState->type,CID_NAME,pCurrentPhoneState->Name,strlen((char*)(pCurrentPhoneState->Name)),false);
	return TRUE;
}

void MemFree_CallerInfo(struct PhoneStateList* Info){
	struct PhoneStateList*temp=NULL;
	if(Info!=NULL){
		if(Info->pNext != Info)
			temp = Info->pNext;
		osal_mem_free(Info->Name);
		osal_mem_free(Info->Num);
		osal_mem_free(Info);
		if(temp!= NULL){
			pCurrentPhoneState = temp;
			pCurrentPhoneState->pNext =pCurrentPhoneState;
		}else{
			pCurrentPhoneState = NULL;
		}
	}
}

void ClearCallerIDFormApk(void)
{
	if(CallerIDFromApk!=CALLER_ID_NULL)
	{
		osal_mem_free(CallerIDApk);
		CallerIDApk = NULL;
		CallerIDFromApk=CALLER_ID_NULL;
	}
}
	
void  Switch_Call_Time(void)
{   
	pCurrentPhoneState = pCurrentPhoneState->pNext;
}

/*用于互斥 ，不允许Load多handler*/
void LoadPhoneHandler(PhoneHandlerType Ptype, uint8 *Cid, uint8 CallIndex)
{
	switch(Ptype)
	{
		case PHONE_CALL:
			if(PhoneCallHandlerExist==FALSE){
				if(Cid!=NULL){    //用于蓝牙断开重连
					if(CallIndex == 1){
						if(MemAlloc_CallerInfo(PHONE_1ST_IN,Cid)==TRUE)	
						LoadHandler(PhoneCall_Handler,NULL);
					}
					else if(CallIndex == 2)
						MemAlloc_CallerInfo(PHONE_2ND_IN,Cid);
				}
				else
					LoadHandler(PhoneCall_Handler,NULL);
			}
			break;
		case PHONE_RING:
			if(PhoneRingHandlerExist==FALSE){
				if(CallIndex==1){        //第一来电
					if(MemAlloc_CallerInfo(PHONE_1ST_IN,Cid)==TRUE)
						LoadHandler(PhoneRing_Handler,NULL); 
				}
				else if(CallIndex==2){  //第二来电
					if(MemAlloc_CallerInfo(PHONE_2ND_IN,Cid)==TRUE)
						LoadHandler(PhoneRing_Handler,NULL);
				}
			}
			break;
		case PHONE_CALLOUT:
			if(PhoneCallOutHandlerExist==FALSE){
				if(MemAlloc_CallerInfo(PHONE_OUT,0)==TRUE)	
					LoadHandler(PhoneCallOut_Handler,&CallIndex); 
			}
			break;  
	}
}


uint16 PhoneCall_Handler(MsgType msg, int iParam, void *pContext)
{	
	uint8 TouchID;
	switch(msg)
	{
		case MSG_HANDLER_BEGIN:
			PhoneHadlerId.PhoneCallId = iParam;
			PhoneCallHandlerExist = TRUE;
			pCurrentPhoneState->CallTime = GetTimeStamp();
			pCurrentPhoneState->KeyCode =0;
			SkipKeyMsg = FALSE;
			SkipIdleKeyMsg = FALSE;
			BC5_SetVolumeCmd(get_volume_level());
			PhoneMic.PhoneMicStatus =1;
			if(CallerID_Data[0]==0x50||CallerID_Data[0]==0x51)
				CallerID_Data[0] = 0x53;
			else if(CallerID_Data[0]==0x52)
				CallerID_Data[0] = 0x54;
			break;		
		case MSG_HANDLER_END:
			osal_stop_timerEx(GetAppTaskId(), MSG_CALL_TIME_DISPLAY);
			PhoneCallHandlerExist = FALSE;    
			MemFree_CallerInfo(pCurrentPhoneState->pNext);   	/*删除当前或前一个*/	
			if(UsbType != MSG_HANDLER_BEGIN && SkipKeyMsg == TRUE)
			{
				SendMsg(UsbType);
				UsbType= MSG_HANDLER_BEGIN; 
			}
			AncsDelaySend();
			break;
			
		case MSG_SWITCH_CALL:
			if(SkipKeyMsg== FALSE){
				if(pCurrentPhoneState!=pCurrentPhoneState->pNext){
					osal_stop_timerEx(GetAppTaskId(), MSG_CALL_TIME_DISPLAY);	
					Switch_Call_Time();
					SendMsg(MSG_DISPLAY);    /*可能需要刷新名字*/
				}
			}		
			break;      
			
		case MSG_DISPLAY:
			if(SkipKeyMsg== FALSE){   /*收到MSG_CALL_STATE_IDLE消息后就不再显示了*/ 
				osal_stop_timerEx(GetAppTaskId(), MSG_CALL_SKIP_KEY_FALSE);	
				SkipIdleKeyMsg=TRUE;  
				PhoneDisplay(PHONE_IN_OUT_CALL);			
				osal_stop_timerEx(GetAppTaskId(), MSG_CALL_TIME_DISPLAY);	
				osal_start_timerEx(GetAppTaskId(), MSG_CALL_TIME_DISPLAY, 1000);
				osal_start_timerEx(GetAppTaskId(), MSG_CALL_SKIP_KEY_FALSE, 800);		
			}
			break;
			
		case MSG_CALL_TIME_DISPLAY:              /*显示时间*/
			{   
				uint32 TimeTemp=GetTimeStamp();
				if(CheckpCurrentHandler(PhoneHadlerId.PhoneCallId)){
					DisplayPhoneCallTime((uint16)(TimeTemp-pCurrentPhoneState->CallTime));
				}
				osal_start_timerEx(GetAppTaskId(), MSG_CALL_TIME_DISPLAY, 1000);
			}		 
			break;

		case MSG_CALL_STATE_FORCE_IDLE:
			BC5_TO_CC2540_CallState(BC5_CALL_STATE_HANGUP);     	
			break;

		case MSG_CALL_SKIP_KEY_FALSE:
			SkipIdleKeyMsg= FALSE;
			break;	   
			
		//状态判断
		case MSG_CALL_STATE_IDLE:  
			if(SkipKeyMsg== FALSE){
				osal_stop_timerEx(GetAppTaskId(), MSG_CALL_TIME_DISPLAY);
				osal_stop_timerEx(GetAppTaskId(), MSG_CALL_STATE_FORCE_IDLE);
				SkipKeyMsg = TRUE;   //不再处理按键消息
				PutTheHandlerOnTop(PhoneHadlerId.PhoneCallId);	
				PhoneDisplay(PHONE_IN_OUT_END);
				if(pCurrentPhoneState!=pCurrentPhoneState->pNext) /*以防止第二通话信息未被删除*/
					MemFree_CallerInfo(pCurrentPhoneState->pNext);
				SetSystemSuspendTime(DISPLAY_TIME_10S_FLAG);
				osal_start_timerEx(GetAppTaskId(), MSG_PHONECALL_HANDLER_TIMEOUT, 2000);
			}
			break;

		case MSG_CALL_STATE_TWO_CALL_TALKING_ACTIVE_END:    
			if(pCurrentPhoneState!=pCurrentPhoneState->pNext){
				osal_stop_timerEx(GetAppTaskId(), MSG_CALL_TIME_DISPLAY);	
				MemFree_CallerInfo(pCurrentPhoneState);		  
				SendMsg(MSG_DISPLAY);    /*可能需要刷新名字*/
			}
			break;
			
		case MSG_CALL_STATE_TWO_CALL_TALKING_HOLD_END:    
			if(pCurrentPhoneState!=pCurrentPhoneState->pNext){
				if(pCurrentPhoneState->KeyCode == MSG_LEFTDOWN_SHORTKEY){
					osal_stop_timerEx(GetAppTaskId(), MSG_CALL_TIME_DISPLAY);	
					MemFree_CallerInfo(pCurrentPhoneState);  
					pCurrentPhoneState->KeyCode =0;
					SendMsg(MSG_DISPLAY);    /*可能需要刷新名字*/
				}else{
					MemFree_CallerInfo(pCurrentPhoneState->pNext);  
				}
			}
			break;	 
	
		case MSG_PHONECALL_HANDLER_TIMEOUT:
			UnloadHandler(PhoneHadlerId.PhoneCallId);	
			break;

		case MSG_SYSTEM_SUSPEND:  
			LCD_Sleep(LCD_ENTER_SLEEP);
			break;

		case MSG_CHARGE_START:
		case MSG_CHARGE_STOP:
			UsbType = msg;
			break;
			
		case MSG_CALL_STATE_LOAD_SECOND_CALL:
			SendMsg(MSG_DISPLAY);    /*可能需要刷新名字*/
			break;

		case MSG_CALL_SEND_CMD_AGAIN:
			BC5_SetCallOpCmd(BC5_CALL_AGAIN);
			break;

		case MSG_SMS_NOTIFY:
		case MSG_MISSED_CALL_COUNT:
		case MSG_INCOMING_CALL_NOTIFY:
		case MSG_E_MAIL_NOTIFY:
		case MSG_DAILY_SCHEDULE_NOTIFY:
		case MSG_SOCIAL_NOTIFY:
			AncsDelaySet(msg);
			break;

//用户操作:	

		case MSG_RIGHTDOWN_SHORTKEY: 				/*右键 单击*/
			if(DISPLAY_GOTO_SLEEP_FLAG){
				LCD_Sleep(LCD_EXIT_SLEEP);
					SendMsg(MSG_DISPLAY);
				}	 	
				else if(SkipKeyMsg== FALSE){
					if(pCurrentPhoneState!=pCurrentPhoneState->pNext){
						SwitchCall = true;
					BC5_SetCallOpCmd(BC5_CALL_SWAP_ACTIVE);				/*-->切换通话*/
				}
			}
			break;
			
		case MSG_LEFTDOWN_SHORTKEY:				/*左键 单击*/
			if(SkipKeyMsg== FALSE && SkipIdleKeyMsg == FALSE&&(!DISPLAY_GOTO_SLEEP_FLAG)){
				BC5_SetCallOpCmd(BC5_CALL_HANG_ACTIVE);					/*-->挂断通话*/
				if(pCurrentPhoneState!=pCurrentPhoneState->pNext)
					pCurrentPhoneState->KeyCode = MSG_LEFTDOWN_SHORTKEY;
				else
					osal_start_timerEx(GetAppTaskId(), MSG_CALL_STATE_FORCE_IDLE, 3000);
			}
			break;	
			
		case MSG_TOUCH_UP_DOUBLE_SHORTKEY:		/*触摸 双击*/
			if(DISPLAY_GOTO_SLEEP_FLAG){
				LCD_Sleep(LCD_EXIT_SLEEP);
				SendMsg(MSG_DISPLAY);
			}
			else if(SkipKeyMsg== FALSE){
				LoadVolumeHandler(false,true);								/*-->音量界面*/
			}
			break;
			

		case MSG_TOUCH_UP_SHORTKEY:				/*触摸 单击*/
			
			TouchID = Get_Screen_Locaton(3,3);

			if( TouchID==6 )			// 挂断
			{
				if(SkipKeyMsg== FALSE && SkipIdleKeyMsg == FALSE&&(!DISPLAY_GOTO_SLEEP_FLAG)){
				BC5_SetCallOpCmd(BC5_CALL_HANG_ACTIVE);					/*-->挂断通话*/
				if(pCurrentPhoneState!=pCurrentPhoneState->pNext)
					pCurrentPhoneState->KeyCode = MSG_LEFTDOWN_SHORTKEY;
				else
					osal_start_timerEx(GetAppTaskId(), MSG_CALL_STATE_FORCE_IDLE, 3000);
			}
									
			}
			else if(TouchID==7)			// 拨号键盘
			{
				LoadHandler(PhoneDialInCall_Handler,NULL);
			}
			else if(TouchID==8)		       // 静mic		
			{
				Change_ShowPhoneMicStatus();
			}
			break;
			

			
		case MSG_LEFTDOWN_DOUBLE_SHORTKEY:		/*左键 双击*/
		case MSG_LEFTDOWN_LONGKEY:           			/*左键 长按*/
		case MSG_RIGHTDOWN_DOUBLE_SHORTKEY:   	/*右键 双击*/	
		case MSG_RIGHTDOWN_LONGKEY:				/*右键 长按*/	
		case MSG_RIGHTDOWN_LONGKEY_UP:			/*右键 长按松开*/
		case MSG_RIGHTDOWN_HOLDKEY:				/*右键 HOLD*/	
		case MSG_RIGHTDOWN_HOLDKEY_UP:			/*右键 HOLD松开*/
		case MSG_TOUCH_UP_FLICK:					/*上*/
		case MSG_TOUCH_DOWN_FLICK:				/*下*/
		case MSG_TOUCH_DOWN_UP_FLICK:				/*左*/ 
		case MSG_TOUCH_UP_DOWN_FLICK: 			/*右*/ 
		case MSG_TOUCH_LONGKEY:					/*触摸 长按*/
			break;
			
		default:
		return RET_MSG_UNHANDLED;
	}
	return RET_MSG_HANDLED;
}
bool IsReject = false;
uint16 PhoneRing_Handler(MsgType msg, int iParam, void *pContext)
{
	uint8 TouchID;
	switch(msg)
	{
		case MSG_HANDLER_BEGIN:
			IsReject = false;
			PhoneHadlerId.PhoneRingId = iParam;
			SkipKeyMsg = FALSE; 
			FreeState =FALSE;
			PhoneRingHandlerExist = TRUE;
			pCurrentPhoneState->KeyCode =0;
			MotoRun(MOTO_INCOMING_CALL);
			PhoneSound.PhoneSoundStatus =1;
			break;

		case MSG_HANDLER_END:	  
			if(FreeState==TRUE){
				MemFree_CallerInfo(pCurrentPhoneState);
			}
			PhoneRingHandlerExist = FALSE;
			StopMotorRunTime();
			if(UsbType != MSG_HANDLER_BEGIN && SkipKeyMsg == TRUE)
			{
				SendMsg(UsbType);
				UsbType= MSG_HANDLER_BEGIN; 
			}
			AncsDelaySend();
			break;

		case MSG_CALL_STATE_FORCE_IDLE:
			BC5_TO_CC2540_CallState(BC5_CALL_STATE_HANGUP);     	
			break;
		//状态判断
		case MSG_CALL_STATE_IDLE:
			if(SkipKeyMsg==FALSE)
			{
				SkipKeyMsg = TRUE;   //不再处理按键消息
				osal_stop_timerEx(GetAppTaskId(), MSG_CALL_STATE_FORCE_IDLE);
				if(pCurrentPhoneState->KeyCode == MSG_LEFTDOWN_SHORTKEY)   /*如果是拒接只显示拒接*/
				{
					pCurrentPhoneState->KeyCode =0;	         
				}else{
					PutTheHandlerOnTop(PhoneHadlerId.PhoneRingId);
					PhoneDisplay(PHONE_IN_OUT_END);
				}
				if(pCurrentPhoneState!=pCurrentPhoneState->pNext) /*以防止第二通话信息未被删除*/
				{
					HandlerMsg(MSG_PHONECALL_HANDLER_TIMEOUT,0);
					if(pCurrentPhoneState!=pCurrentPhoneState->pNext) /*非存在phonecallhandler*/
					{
						MemFree_CallerInfo(pCurrentPhoneState->pNext);
					}
				}
				FreeState = TRUE;	 
				osal_start_timerEx(GetAppTaskId(), MSG_PHONERING_HANDLER_TIMEOUT, 2000);
				SetSystemSuspendTime(DISPLAY_TIME_10S_FLAG);
				StopMotorRunTime();
			}
			break;
			
		case MSG_CALL_STATE_ONE_CALL_TALKING:
			if(IsReject == false)
			{
				LoadPhoneHandler(PHONE_CALL,NULL,0);
				FreeState = FALSE;	 
				SendMsg(MSG_PHONERING_HANDLER_TIMEOUT);
			}
			break;
			
		case MSG_CALL_STATE_SECOND_CALL_INCOMING_FIRST_CALL_END:	     		
			HandlerMsg(MSG_PHONECALL_HANDLER_TIMEOUT,0);
			break;
			
		case MSG_CALL_STATE_SECOND_CALL_INCOMING_END:
			if(pCurrentPhoneState->KeyCode == MSG_RIGHTDOWN_LONGKEY){  /*有的机器按此按键是挂第二，这里修正*/
				pCurrentPhoneState->KeyCode= 0; 
				HandlerMsg(MSG_PHONECALL_HANDLER_TIMEOUT,0);
				LoadPhoneHandler(PHONE_CALL,NULL,0);
				FreeState = FALSE;
				SendMsg(MSG_PHONERING_HANDLER_TIMEOUT);
			}else{
				FreeState = TRUE;	
				SendMsg(MSG_PHONERING_HANDLER_TIMEOUT);
			}
			break;
			
		case MSG_CALL_STATE_ANSWER_SECOND_CALL_FIRST_CALL_END:
			HandlerMsg(MSG_PHONECALL_HANDLER_TIMEOUT,0);
			LoadPhoneHandler(PHONE_CALL,NULL,0);
			FreeState = FALSE;
			SendMsg(MSG_PHONERING_HANDLER_TIMEOUT);
			break;
			
		case MSG_CALL_STATE_TWO_CALL_TALKING:
			osal_stop_timerEx(GetAppTaskId(), MSG_CALL_STATE_SECOND_CALL_INCOMING_END);
			osal_stop_timerEx(GetAppTaskId(), MSG_CALL_STATE_ANSWER_SECOND_CALL_FIRST_CALL_END);
			pCurrentPhoneState->CallTime = GetTimeStamp(); 
			FreeState = FALSE;
			SendMsg(MSG_PHONERING_HANDLER_TIMEOUT);
			break;		

		case MSG_DISPLAY:	
			if(SkipKeyMsg== FALSE){   /*收到MSG_CALL_STATE_IDLE消息后就不再显示了*/          
				PhoneDisplay(PHONE_IN_RING);
			}
			break;
			
		case MSG_PHONERING_HANDLER_TIMEOUT:
			UnloadHandler(PhoneHadlerId.PhoneRingId);	
			break;
				
		case MSG_STOP_INCOMING_TONE:
			if(pCurrentPhoneState==pCurrentPhoneState->pNext)
			{
				if(PhoneSound.PhoneSoundStatus ==0)
					BC5_SetStopRingToneCmd(BC5_STOP_TONE);
				else	
					BC5_SetStopRingToneCmd(BC5_START_TONE);
				if(--StopToneCounts !=0)
					osal_start_timerEx(GetAppTaskId(), MSG_STOP_INCOMING_TONE, 1000);	  
			}
			break;
			
		case MSG_CHARGE_START:
		case MSG_CHARGE_STOP:
			UsbType = msg;
			break;

		case MSG_CALL_SEND_CMD_AGAIN:
			BC5_SetCallOpCmd(BC5_CALL_AGAIN);
			break;
			
		case MSG_SYSTEM_SUSPEND:
			break;
			
		case MSG_SMS_NOTIFY:
		case MSG_MISSED_CALL_COUNT:
		case MSG_INCOMING_CALL_NOTIFY:
		case MSG_E_MAIL_NOTIFY:
		case MSG_DAILY_SCHEDULE_NOTIFY:
		case MSG_SOCIAL_NOTIFY:
			AncsDelaySet(msg);
			break;
//用户操作:	

		case MSG_LEFTDOWN_SHORTKEY:				/*左键 单击*/	
			if(SkipKeyMsg== FALSE){
				StopMotorRunTime();
				if(pCurrentPhoneState==pCurrentPhoneState->pNext){		 	
					BC5_SetCallOpCmd(BC5_CALL_REJECT);								/*-->拒接一二来电*/
					pCurrentPhoneState->KeyCode = MSG_LEFTDOWN_SHORTKEY;
					PhoneDisplay(PHONE_IN_REJECT);
					IsReject = true;
					osal_start_timerEx(GetAppTaskId(), MSG_CALL_STATE_FORCE_IDLE, 3000);
				}
				else{
					BC5_SetCallOpCmd(BC5_CALL_REJECT);			
					BC5_SetCallOpCmd(BC5_CALL_REJECT_SECOND);  
				}		  
			}
			break;
			
		case MSG_RIGHTDOWN_SHORTKEY: 				/*右键 单击*/	
			if(SkipKeyMsg== FALSE){
				StopMotorRunTime();
				if(pCurrentPhoneState==pCurrentPhoneState->pNext){				
					BC5_SetCallOpCmd(BC5_CALL_ANSWER);							/*-->接通一二来电*/		 
				}
				else{
					BC5_SetCallOpCmd(BC5_CALL_ANSWER);
					BC5_SetCallOpCmd(BC5_CALL_ANSWER_SECOND_HOLD);      
				}
			}
			break;

		case MSG_RIGHTDOWN_LONGKEY:				/*右键 长按*/	
			if(SkipKeyMsg== FALSE){
				if(pCurrentPhoneState!=pCurrentPhoneState->pNext){					/*-->接通第二来电, 挂断第一来电*/
					BC5_SetCallOpCmd(BC5_CALL_ANSWER_SECOND_HANG);     			
					pCurrentPhoneState->KeyCode = MSG_RIGHTDOWN_LONGKEY;
				}
			}
			break;		
			
		case MSG_TOUCH_UP_DOUBLE_SHORTKEY:		/*触摸 双击*/
			if(DISPLAY_GOTO_SLEEP_FLAG){
				LCD_Sleep(LCD_EXIT_SLEEP);
				SendMsg(MSG_DISPLAY);
			}
		
			break;	
 			
		case MSG_TOUCH_UP_SHORTKEY:				/*触摸 单击*/	
			TouchID = Get_Screen_Locaton(3,3);
			if( TouchID==6 )			// 挂断
			{
				if(SkipKeyMsg== FALSE){
					StopMotorRunTime();
					if(pCurrentPhoneState==pCurrentPhoneState->pNext){		 	
						BC5_SetCallOpCmd(BC5_CALL_REJECT);								/*-->拒接一二来电*/
						pCurrentPhoneState->KeyCode = MSG_LEFTDOWN_SHORTKEY;
						PhoneDisplay(PHONE_IN_REJECT);
						IsReject = true;
						osal_start_timerEx(GetAppTaskId(), MSG_CALL_STATE_FORCE_IDLE, 3000);
					}
					else{
						BC5_SetCallOpCmd(BC5_CALL_REJECT);			
						BC5_SetCallOpCmd(BC5_CALL_REJECT_SECOND);  
					}		  
				}							
			}
			else if(TouchID==7)			// 静音
			{
				Change_ShowPhoneSoundStatus();
			}
			else if(TouchID==8)		    // 接通	
			{
				if(SkipKeyMsg== FALSE){
					StopMotorRunTime();
					if(pCurrentPhoneState==pCurrentPhoneState->pNext){				
						BC5_SetCallOpCmd(BC5_CALL_ANSWER);							/*-->接通一二来电*/		 
					}
					else{
						BC5_SetCallOpCmd(BC5_CALL_ANSWER);
						BC5_SetCallOpCmd(BC5_CALL_ANSWER_SECOND_HOLD);      
					}
				}
			}		
			break;

						
		case MSG_LEFTDOWN_DOUBLE_SHORTKEY:		/*左键 双击*/
		case MSG_LEFTDOWN_LONGKEY:           			/*左键 长按*/
		case MSG_RIGHTDOWN_DOUBLE_SHORTKEY:   	/*右键 双击*/		
		case MSG_RIGHTDOWN_LONGKEY_UP:			/*右键 长按松开*/
		case MSG_RIGHTDOWN_HOLDKEY:				/*右键 HOLD*/	
		case MSG_RIGHTDOWN_HOLDKEY_UP:			/*右键 HOLD松开*/
		
		case MSG_TOUCH_UP_FLICK:					/*上*/
		case MSG_TOUCH_DOWN_FLICK:				/*下*/
		case MSG_TOUCH_DOWN_UP_FLICK:				/*左*/ 
		case MSG_TOUCH_UP_DOWN_FLICK: 			/*右*/	
		case MSG_TOUCH_LONGKEY:					/*触摸 长按*/	
			break;
		default:
			return RET_MSG_UNHANDLED;
	}
	return RET_MSG_HANDLED;
}

bool IsCancel = false;   
uint16 PhoneCallOut_Handler(MsgType msg, int iParam, void *pContext)
{
	switch(msg)
	{
		case MSG_HANDLER_BEGIN:
			{
				 IsCancel = false;
				uint8* CallOutindex=(uint8*)pContext;
				CallOutID = *CallOutindex;
				PhoneHadlerId.PhoneCallOutId = iParam;
				SkipKeyMsg = FALSE; 
				PhoneCallOutHandlerExist = TRUE;
				if(BC5_State==BC5_BT_CONNECT_OK)
				{
					if(CallOutID == 0){
						BC5_SetCallOpCmd(BC5_CALL_REDIAL);		      
						osal_start_timerEx(GetAppTaskId(), MSG_PHONECALLOUT_HANDLER_NORSP, 5000);  	
					}
					osal_start_timerEx(GetAppTaskId(), MSG_UNLOADHANDLER_CMD, 1000);
				}else{	                     
					osal_start_timerEx(GetAppTaskId(), MSG_PHONECALLOUT_HANDLER_CANCEL, 3000);  
				}
			}
			break;

		case MSG_HANDLER_END:
			PhoneCallOutHandlerExist = FALSE;
			CallOutID= 0;	 
			osal_stop_timerEx(GetAppTaskId(), MSG_PHONECALLOUT_HANDLER_NORSP);
			if(UsbType != MSG_HANDLER_BEGIN && SkipKeyMsg == TRUE)
			{
				SendMsg(UsbType);
				UsbType= MSG_HANDLER_BEGIN; 
			}
			AncsDelaySend();
			break;
			
		case MSG_CALL_STATE_FORCE_IDLE:
			BC5_TO_CC2540_CallState(BC5_CALL_STATE_HANGUP);   
			break;
			
//状态判断
		case MSG_CALL_STATE_IDLE:
			if(SkipKeyMsg== FALSE)
			{
				SkipKeyMsg = TRUE;
				osal_stop_timerEx(GetAppTaskId(), MSG_CALL_STATE_FORCE_IDLE);
				PutTheHandlerOnTop(PhoneHadlerId.PhoneCallOutId);
				PhoneDisplay(PHONE_OUT_CANCEL);
				if(pCurrentPhoneState!=pCurrentPhoneState->pNext) /*以防止第二通话信息未被删除*/
				{
					HandlerMsg(MSG_PHONECALL_HANDLER_TIMEOUT,0);
					if(pCurrentPhoneState!=pCurrentPhoneState->pNext) /*非存在phonecallhandler*/
					{
						MemFree_CallerInfo(pCurrentPhoneState->pNext);
					}
				}
				MemFree_CallerInfo(pCurrentPhoneState);
				osal_start_timerEx(GetAppTaskId(), MSG_PHONECALLOUT_HANDLER_TIMEOUT, 2000);  
				SetSystemSuspendTime(DISPLAY_TIME_10S_FLAG);
			}
			break;
			
		case MSG_CALL_STATE_ONE_CALL_TALKING:
			if( IsCancel == false)
			{
				SendMsg(MSG_PHONECALLOUT_HANDLER_TIMEOUT);
				LoadPhoneHandler(PHONE_CALL,NULL,0);				
			}
			break;

		case MSG_DISPLAY:
			if(SkipKeyMsg== FALSE){   /*收到MSG_CALL_STATE_IDLE消息后就不再显示了*/
				PhoneDisplay(PHONE_OUT_RING);
			}
			break;
			
		case MSG_PHONECALLOUT_HANDLER_TIMEOUT:
			UnloadHandler(PhoneHadlerId.PhoneCallOutId );
			break;
			
		case MSG_PHONECALLOUT_HANDLER_CANCEL:
			if(BC5_State!=BC5_BT_CONNECT_OK || CallOutID ==0)
			{
				SkipKeyMsg = TRUE;
				MemFree_CallerInfo(pCurrentPhoneState); 
				PhoneDisplay(PHONE_OUT_CANCEL);	  
				osal_start_timerEx(GetAppTaskId(), MSG_PHONECALLOUT_HANDLER_TIMEOUT, 1000);
			}
			break;

		case MSG_CHARGE_START:
		case MSG_CHARGE_STOP:
			UsbType = msg;
			break;

		case MSG_CALL_STATE_OUT_GOING:
			osal_stop_timerEx(GetAppTaskId(), MSG_PHONECALLOUT_HANDLER_NORSP);
			break;
			
		case MSG_PHONECALLOUT_HANDLER_NORSP:  /*手机没有拨出*/
			MemFree_CallerInfo(pCurrentPhoneState); 	
			SkipKeyMsg = TRUE;
			PhoneDisplay(PHONE_OUT_CANCEL);
			osal_start_timerEx(GetAppTaskId(), MSG_PHONECALLOUT_HANDLER_TIMEOUT, 1000);  
			break;
		
		case MSG_CALL_SEND_CMD_AGAIN:
			BC5_SetCallOpCmd(BC5_CALL_AGAIN);
			break;

		case MSG_SMS_NOTIFY:
		case MSG_MISSED_CALL_COUNT:
		case MSG_INCOMING_CALL_NOTIFY:
		case MSG_E_MAIL_NOTIFY:
		case MSG_DAILY_SCHEDULE_NOTIFY:
		case MSG_SOCIAL_NOTIFY:
			AncsDelaySet(msg);
			break;
		case MSG_SYSTEM_SUSPEND:
			break;
//用户操作:	
		case MSG_TOUCH_UP_SHORTKEY:				/*触摸 单击*/  
			{
				uint8 TouchID = Get_Screen_Locaton(3,3);	
				if(TouchID!=7)
					break;
			}
		case MSG_LEFTDOWN_SHORTKEY:				/*左键 单击*/	
			if(SkipKeyMsg == FALSE){
				if(BC5_State==BC5_BT_CONNECT_OK){
					IsCancel = true;
					BC5_SetCallOpCmd(BC5_CALL_HANG_ACTIVE);									/*-->取消重拨*/
					osal_start_timerEx(GetAppTaskId(), MSG_CALL_STATE_FORCE_IDLE, 3000);
				}else{
					SkipKeyMsg = TRUE;
					PhoneDisplay(PHONE_OUT_CANCEL);
					MemFree_CallerInfo(pCurrentPhoneState);
					osal_start_timerEx(GetAppTaskId(), MSG_PHONECALLOUT_HANDLER_TIMEOUT, 2000);  
				}
			}
			break;

		case MSG_TOUCH_UP_DOUBLE_SHORTKEY:		/*触摸 双击*/
			if(SkipKeyMsg== FALSE){
				LoadVolumeHandler(false,true); 													/*-->音量界面*/
			}
			break;
			
		case MSG_LEFTDOWN_DOUBLE_SHORTKEY:		/*左键 双击*/
		case MSG_LEFTDOWN_LONGKEY:           			/*左键 长按*/
		case MSG_RIGHTDOWN_SHORTKEY: 				/*右键 单击*/	
		case MSG_RIGHTDOWN_DOUBLE_SHORTKEY:   	/*右键 双击*/	
		case MSG_RIGHTDOWN_LONGKEY:				/*右键 长按*/	
		case MSG_RIGHTDOWN_LONGKEY_UP:			/*右键 长按松开*/
		case MSG_RIGHTDOWN_HOLDKEY:				/*右键 HOLD*/	
		case MSG_RIGHTDOWN_HOLDKEY_UP:			/*右键 HOLD松开*/
		
		case MSG_TOUCH_UP_FLICK:					/*上*/
		case MSG_TOUCH_DOWN_FLICK:				/*下*/
		case MSG_TOUCH_DOWN_UP_FLICK:				/*左*/ 
		case MSG_TOUCH_UP_DOWN_FLICK: 			/*右*/		
		case MSG_TOUCH_LONGKEY:					/*触摸 长按*/
			break;
			
		default:
			return RET_MSG_UNHANDLED;
	}
	return RET_MSG_HANDLED;
}

/*拨号键盘*/
uint8 DialNumber[20]={0x00};
uint8 PrepareDialNumber[20]={0x00};
uint8 DialPos=0;
uint8 DialNum_Recover=0;
bool IsPush = false;
uint8 DialIncallNum_Recover=0;
bool IsPush_Incall = false;
uint16 PhoneDialInCall_Handler(MsgType msg, int iParam, void *pContext)
{
	switch(msg)
	{
		case MSG_HANDLER_BEGIN:
			PhoneHadlerId.PhoneDialInCallId = iParam;
			memset(DialNumber,0,20);
			DialPos=0;
			 IsPush_Incall = false;
			DialIncallNum_Recover=0;	
			break;
		case MSG_HANDLER_END:
			memset(DialNumber,0,20);
			PhoneHadlerId.PhoneDialInCallId = 0;
			 IsPush_Incall = false;
			break;
/*用户操作*/
		case MSG_LEFTDOWN_SHORTKEY:				/*左键 单击*/	
		case MSG_LEFTDOWN_DOUBLE_SHORTKEY:		/*左键 双击*/
		case MSG_LEFTDOWN_LONGKEY:           			/*左键 长按*/
		case MSG_RIGHTDOWN_DOUBLE_SHORTKEY:   	/*右键 双击*/	
		case MSG_RIGHTDOWN_LONGKEY:				/*右键 长按*/	
		case MSG_RIGHTDOWN_LONGKEY_UP:			/*右键 长按松开*/		
		case MSG_TOUCH_UP_FLICK:					/*上*/
		case MSG_TOUCH_DOWN_FLICK:				/*下*/
		case MSG_TOUCH_DOWN_UP_FLICK:				/*左*/ 
		case MSG_TOUCH_UP_DOWN_FLICK: 			/*右*/		 
			break;
		case MSG_TOUCH_LONGKEY:					/*触摸 长按*/			
			break;
		
		case MSG_RIGHTDOWN_SHORTKEY: 				/*右键 单击*/		
			UnloadHandler(PhoneHadlerId.PhoneDialInCallId);
			break;
		case MSG_TOUCH_UP_SHORTKEY_PRESS:					/*触摸 单击 按下*/
			if(CheckpCurrentHandler(PhoneHadlerId.PhoneDialInCallId)&&DISPLAY_GOTO_SLEEP_FLAG==false)
			{
				uint8 TouchID = Get_Screen_Locaton(5,3);
				IsPush_Incall =false;
				switch(TouchID)
				{
					case 3:		// 1
						{
							DialIncallNum_Recover= 0x31;
							IsPush_Incall = true;
							break;
						}
					case 4:		// 2
						{
							DialIncallNum_Recover= 0x32;
							IsPush_Incall = true;
							break;
						}
					case 5:		// 3
						{
							DialIncallNum_Recover= 0x33;
							IsPush_Incall = true;
							break;	
						}
					case 6:		// 4
						{
							DialIncallNum_Recover= 0x34;
							IsPush_Incall = true;
							break;
						}
					case 7:		// 5
						{
							DialIncallNum_Recover= 0x35;
							IsPush_Incall = true;
							break;
						}
					case 8:		// 6
						{
							DialIncallNum_Recover= 0x36;
							IsPush_Incall = true;
							break;	
						}
					case 9:		// 7
						{
							DialIncallNum_Recover= 0x37;
							IsPush_Incall = true;
							break;
						}
					case 10:		// 8
						{
							DialIncallNum_Recover= 0x38;
							IsPush_Incall = true;
							break;
						}
					case 11:		// 9
						{
							DialIncallNum_Recover= 0x39;
							IsPush_Incall = true;
							break;	
						}
					case 12:		// *
						{
							DialIncallNum_Recover= 0x2a;
							IsPush_Incall = true;
							break;	
						}
					case 13:		// 0
						{
							DialIncallNum_Recover= 0x30;
							IsPush_Incall = true;
							break;
						}
					case 14:		// #
						{
							DialIncallNum_Recover= 0x23;
							IsPush_Incall = true;
							break;
						}
					default:
						return RET_MSG_HANDLED;
				}
				if(IsPush_Incall)
				{
					uint8 NumTemp[20]={0x00};
					uint8 NumDisplay[20]={0x00};
					if(DialPos<12)
					{
						DialNumber[DialPos++] = DialIncallNum_Recover;
						memcpy(NumDisplay,DialNumber,DialPos);
					}
					else
					{
						memcpy(NumTemp,DialNumber,12);
						memset(DialNumber,0,20);
						memcpy(DialNumber,NumTemp+1,11);
						DialNumber[11]=DialIncallNum_Recover;
						memset(NumDisplay,0x2e,3);
						memcpy(NumDisplay+3,DialNumber,12);
					}
					if(DialIncallNum_Recover>0)
					{	
						BC5_SendDtmfNumber(DialIncallNum_Recover);	
						ShowPhoneDialNumber_Incall(NumDisplay);				
						ShowPhoneDialIncallNumber_Fade(DialIncallNum_Recover);
						DialIncallNum_Recover=0;	
					}
				}
			}
			break;
		case MSG_TOUCH_UP_DOUBLE_SHORTKEY:		/*触摸 双击*/
			if(DISPLAY_GOTO_SLEEP_FLAG){
				LCD_Sleep(LCD_EXIT_SLEEP);
				SendMsg(MSG_DISPLAY);
				break;			
			}
		case MSG_TOUCH_UP_SHORTKEY:					/*触摸 单击 按下*/
			if(CheckpCurrentHandler(PhoneHadlerId.PhoneDialInCallId))
			{
				ShowPhoneDialIncallNumber_Recover(0);
				IsPush_Incall = false;
			}			
			break;
			
		case MSG_DISPLAY:
			ShowPhoneDial_Incall();
			ShowPhoneDialNumber_Incall(DialNumber);
			break;
			
		case MSG_PHONECALLOUT_HANDLER_NORSP:
		case MSG_CALL_STATE_IDLE:
			UnloadHandler(PhoneHadlerId.PhoneDialInCallId);
			SendMsg(MSG_CALL_STATE_IDLE);
			break;
		
		default:
			return RET_MSG_UNHANDLED;	
	}	
	return RET_MSG_HANDLED;
}



uint16 PhoneDial_Handler(MsgType msg, int iParam, void *pContext)
{
	switch(msg)
	{
		case MSG_HANDLER_BEGIN:
			PhoneHadlerId.PhoneDialId = iParam;
			memset(DialNumber,0,20);
			DialPos=0;
			DialNum_Recover=0;
			IsPush = false;
			break;
		case MSG_HANDLER_END:
			memset(DialNumber,0,20);
			PhoneHadlerId.PhoneDialId = 0;
			IsPush = false;
			break;
/*用户操作*/
		case MSG_LEFTDOWN_SHORTKEY:				/*左键 单击*/	
		case MSG_LEFTDOWN_DOUBLE_SHORTKEY:		/*左键 双击*/
		case MSG_LEFTDOWN_LONGKEY:           			/*左键 长按*/
		case MSG_RIGHTDOWN_DOUBLE_SHORTKEY:   	/*右键 双击*/	
		case MSG_RIGHTDOWN_LONGKEY:				/*右键 长按*/	
		case MSG_RIGHTDOWN_LONGKEY_UP:			/*右键 长按松开*/		
		case MSG_TOUCH_UP_FLICK:					/*上*/
		case MSG_TOUCH_DOWN_FLICK:				/*下*/
		case MSG_TOUCH_DOWN_UP_FLICK:				/*左*/ 
		case MSG_TOUCH_UP_DOWN_FLICK: 			/*右*/		 
			break;
		case MSG_TOUCH_LONGKEY:					/*触摸 长按*/
			{
				uint8 TouchID = Get_Screen_Locaton(5,3);
				IsPush = false;
				switch(TouchID)
				{
					case 3:		// 1
					case 4:		// 2
					case 5:		// 3	
					case 6:		// 4
					case 7:		// 5
					case 8:		// 6					
						break;	
					case 9:		// 7
						if(DialPos<15)
						{
							if(DialPos>0)
							{
								DialNumber[DialPos-1] = 0x2A;
							}
							else
							{
								DialNumber[DialPos++] = 0x2A;
							}
							DialNum_Recover = 0x2A;
							IsPush = true;
						}
						break;
					case 10:			//8		
						break;
					case 11:		// 9
						if(DialPos<15)
						{
							if(DialPos>0)
							{
								DialNumber[DialPos-1] = 0x23;
							}
							else
							{
								DialNumber[DialPos++] = 0x23;
							}
							DialNum_Recover = 0x23;
							IsPush = true;
						}				
						break;		
					case 12:	
						break;
					case 13:		// 0
						if(DialPos<15)
						{
							if(DialPos>0)
							{
								DialNumber[DialPos-1] = 0x2B;
							}
							else
							{
								DialNumber[DialPos++] = 0x2B;
							}
							DialNum_Recover = 0x2B;
							IsPush = true;
						}
						break;
					default:	
						break;							
				}				
				if(IsPush)
				{	
					ShowPhoneDialNumber(DialNumber);
					if(DialNum_Recover>0)
					{
						ShowPhoneDialNumber_Fade(DialNum_Recover);
						DialNum_Recover=0;
					}
				}
			}
			break;
		case MSG_RIGHTDOWN_SHORTKEY: 				/*右键 单击*/		
			UnloadHandler(PhoneHadlerId.PhoneDialId);
			break;
			
		case MSG_TOUCH_UP_SHORTKEY_PRESS:				/*触摸 单击*/ 
			{
				uint8 TouchID = Get_Screen_Locaton(5,3);
				IsPush = false;
				switch(TouchID)
				{
					case 3:		// 1
						if(DialPos<15)
						{
							DialNumber[DialPos++] = 0x31;
							DialNum_Recover = 0x31;	
							IsPush = true;
						}
						break;
					case 4:		// 2
						if(DialPos<15)
						{
							DialNumber[DialPos++] = 0x32;
							DialNum_Recover = 0x32;
							IsPush = true;
						}	
						break;
					case 5:		// 3
						if(DialPos<15)
						{
							DialNumber[DialPos++] = 0x33;
							DialNum_Recover = 0x33;
							IsPush = true;
						}
						break;		
					case 6:		// 4
						if(DialPos<15)
						{
							DialNumber[DialPos++] = 0x34;
							DialNum_Recover = 0x34;
							IsPush = true;
						}
						break;
					case 7:		// 5
						if(DialPos<15)
						{
							DialNumber[DialPos++] = 0x35;
							DialNum_Recover = 0x35;
							IsPush = true;
						}
						break;
					case 8:		// 6
						if(DialPos<15)
						{
							DialNumber[DialPos++] = 0x36;
							DialNum_Recover = 0x36;
							IsPush = true;
						}				
						break;	
					case 9:		// 7
						if(DialPos<15)
						{
							DialNumber[DialPos++] = 0x37;
							DialNum_Recover = 0x37;
							IsPush = true;
						}
						break;
					case 10:		// 8
						if(DialPos<15)
						{
							DialNumber[DialPos++] = 0x38;
							DialNum_Recover = 0x38;
							IsPush = true;
						}		
						break;
					case 11:		// 9
						if(DialPos<15)
						{
							DialNumber[DialPos++] = 0x39;
							DialNum_Recover = 0x39;
							IsPush = true;
						}			
						break;		
					case 12:		// 拨出
						BC5_SetCallRedialNumber(BC5_CALL_REDIAL_NUMBER);
						IsPush = true;
						break;
					case 13:		// 0
						if(DialPos<15)
						{
							DialNumber[DialPos++] = 0x30;
							DialNum_Recover = 0x30;
							IsPush = true;
						}	
						break;
					case 14:		// 回退
						if(DialPos>0)
						{
							DialNumber[--DialPos]=0x00; 
							IsPush = true;
						}
						break;							
				}
				if(IsPush)
				{
					ShowPhoneDialNumber(DialNumber);
					if(DialNum_Recover>0)
					{
						ShowPhoneDialNumber_Fade(DialNum_Recover);
						DialNum_Recover=0;
					}
				}
			}
			break;
		case MSG_CALL_SEND_CMD_AGAIN:
			BC5_SetCallRedialNumber(BC5_CALL_AGAIN);
			break;
		case MSG_DISPLAY:
			ShowPhoneDial();
			ShowPhoneDialNumber(DialNumber);
			break;

		case MSG_UNLOADHANDLER_CMD:
			UnloadHandler(PhoneHadlerId.PhoneDialId);
			break;
		case MSG_TOUCH_UP_SHORTKEY:				/*触摸 单击*/	
		case MSG_TOUCH_LONGKEY_UP:				/*触摸 长按松开*/
		case MSG_TOUCH_UP_DOUBLE_SHORTKEY:		/*触摸 双击*/		
			if(CheckpCurrentHandler(PhoneHadlerId.PhoneDialId))
			{
				ShowPhoneDialNumber_Recover(0);
				IsPush = false;			
			}
			break;
		default:
			return RET_MSG_UNHANDLED;	
	}	
	return RET_MSG_HANDLED;
}

uint32 PpAddr =0;
uint8 PpNamelen =0,PpNumlen=0;
uint8 PpDiplayType =0,LastPpDiplayType=0;
bool IsAndroidDisplay =false;
void LoadPhonePrepareHandler(uint32 addr,uint8 NameLen,uint8 NumLen,uint8*Number,bool IsAndroid)
{
	if(PhonePrepareHandlerExit == FALSE)
	{
		if((Number!=NULL)&&(NumLen>0)&&(NumLen<=20))
		{
			memset(PrepareDialNumber,0,20);
			memcpy(PrepareDialNumber,Number,NumLen);
		}
		else if(NameLen==0&&NumLen==0)				/*电话本*/
		{
			memset(PrepareDialNumber,0,20);
			SpiFlash_Read(addr,PrepareDialNumber,16);
		}	
		else if(NameLen==1&&NumLen==0)				/*通话记录*/
		{
			uint8 len =0;
			memset(PrepareDialNumber,0,20);
			Force_NordicFlash();
			SpiFlash_Read(addr+1,&len,1);
			if(len<=16)
				SpiFlash_Read(addr+4,PrepareDialNumber,len);	
			Free_NordicFlash();
		}
		PpAddr = addr;
		PpNamelen = NameLen;
		PpNumlen = NumLen;
		IsAndroidDisplay = IsAndroid;
		LoadHandler(PhonePrepareHandler,0);
	}
	else if(addr==0)										/*只更新号码*/
	{
		if((Number!=NULL)&&(NumLen>0)&&(NumLen<=20))
		{
			memset(PrepareDialNumber,0,20);
			memcpy(PrepareDialNumber,Number,NumLen);
			PpNumlen = NumLen;
			SendMsg(MSG_PHONE_PREPARE_NUMBER);
		}		
	}	
}

uint16 PhonePrepareHandler(MsgType msg, int iParam, void *pContext)
{
	switch(msg)
	{
		case MSG_HANDLER_BEGIN:
			PhonePrepareHandlerExit = TRUE;
			PpDiplayType=0;
			LastPpDiplayType =0;
			PhoneHadlerId.PhonePrepareId = iParam;
			break;
		case MSG_HANDLER_END:
			PhonePrepareHandlerExit = FALSE;
			PhoneHadlerId.PhonePrepareId = 0;
			memset(DialNumber,0,20);
			break;
/*用户操作*/
		case MSG_LEFTDOWN_SHORTKEY:				/*左键 单击*/	
		case MSG_LEFTDOWN_DOUBLE_SHORTKEY:		/*左键 双击*/
		case MSG_LEFTDOWN_LONGKEY:           			/*左键 长按*/
		case MSG_RIGHTDOWN_DOUBLE_SHORTKEY:   	/*右键 双击*/	
		case MSG_RIGHTDOWN_LONGKEY:				/*右键 长按*/	
		case MSG_RIGHTDOWN_LONGKEY_UP:			/*右键 长按松开*/		
		case MSG_TOUCH_UP_DOUBLE_SHORTKEY:		/*触摸 双击*/	
		case MSG_TOUCH_LONGKEY:					/*触摸 长按*/
			break;

		case MSG_RIGHTDOWN_SHORTKEY: 				/*右键 单击*/	
			if(PpDiplayType==0)
				UnloadHandler(PhoneHadlerId.PhonePrepareId);
			else
			{
				if(PpDiplayType<=3)
					PpDiplayType=0;
				else if(PpDiplayType<=5)
					PpDiplayType=LastPpDiplayType;
				SendMsg(MSG_DISPLAY);
			}	
			break;
			
		case MSG_TOUCH_UP_SHORTKEY:				/*触摸 单击*/  
			{
				if(PpDiplayType==0)
				{
					if(Android_ios||SMS_FROM_ANDROID)
					{
						uint8 TouchID = Get_Screen_Locaton(3,2);
						if(TouchID==4)						/*拨号*/		
						{
							if(PrepareDialNumber[0]!=0x00)
							{
								memset(DialNumber,0,20);
								//memcpy(DialNumber,PrepareDialNumber,20);
								uint8 i,j;
								for(i=0, j =0;i<20;i++)
								{
									if((PrepareDialNumber[i]>=0x30 && PrepareDialNumber[i]<=0x39)||PrepareDialNumber[i] == 0x23 ||PrepareDialNumber[i] == 0x2A ||PrepareDialNumber[i] == 0x2B)
									{
										DialNumber[j] = PrepareDialNumber[i];
										j++;
									}
								}
								BC5_SetCallRedialNumber(BC5_CALL_REDIAL_NUMBER);
							}
						}
						else if(TouchID==5)					/*SMS*/	
						{
							if(Android_ios||SMS_FROM_ANDROID)
							{
								PpDiplayType=1;
								SendMsg(MSG_DISPLAY);
							}
						}
					}
					else
					{
						uint8 TouchID = Get_Screen_Locaton(3,1);
						if(TouchID==2)						/*拨号*/		
						{
							if(PrepareDialNumber[0]!=0x00)
							{
								memset(DialNumber,0,20);
								memcpy(DialNumber,PrepareDialNumber,20);
								BC5_SetCallRedialNumber(BC5_CALL_REDIAL_NUMBER);
							}
						}					
					}
				}
				else if(PpDiplayType<=3)					/*快捷回复*/
				{
					uint8 TouchID = Get_Screen_Locaton(4,3);
					uint8 ViewIndex =0x10+(PpDiplayType -1)*3; 
					if(TouchID<=2)
					{
						if(TouchID==1)
							TouchID = 0;
						else
							TouchID = 0xff;

					}	
					else if(TouchID<=5)
						TouchID = 1;
					else if(TouchID<=8)
						TouchID = 2;
					else if(TouchID<=11)
						TouchID = 3;
					if(TouchID != 0xff)
					{
						if(TouchID==0)
						{
							LastPpDiplayType = PpDiplayType;
							PpDiplayType = 4;
							SendMsg(MSG_DISPLAY);								
						}
						else
						{
							ViewIndex+=TouchID;
							SendSms_Number(ViewIndex,PrepareDialNumber);
							LoadHandler(AncsSendHandler, 0);				
						}	
					}
				}
				else if(PpDiplayType<=5)					/*表情*/
				{
					uint8 TouchID = Get_Screen_Locaton(4,4);
					uint8 EmoIndex =0; 
					if( TouchID==0 )								// 11点位置
					{
						EmoIndex=PpDiplayType==4?0x05:0x08;
					}
					else if((TouchID==1) || (TouchID==2))			// 0点位置
					{
						EmoIndex=PpDiplayType==4?0x02:0x0E;
					}	
					else if(TouchID==3)							// 1点位置		
					{
						EmoIndex=PpDiplayType==4?0x04:0x0C;
					}
					else if((TouchID==4) || (TouchID==8))			// 9点位置					
					{
						EmoIndex=PpDiplayType==4?0x07:0x09;
					}
					else if((TouchID==7) || (TouchID==11))			// 3点位置				
					{
						EmoIndex=PpDiplayType==4?0x01:0x0B;	
					}				
					else if (TouchID==12)							// 7点位置	
					{
   						EmoIndex=PpDiplayType==4?0x06:0x0A;
					}
					else if ((TouchID==13) || (TouchID==14))		// 6点位置
					{
						EmoIndex=PpDiplayType==4?0x0F:0x10;
					}
					else if (TouchID==15)							// 5点位置
					{
						EmoIndex=PpDiplayType==4?0x03:0x0D;
					}
					if(EmoIndex>0)
					{
						SendSms_Number(EmoIndex,PrepareDialNumber);
						LoadHandler(AncsSendHandler, 0);
					}
				}
			}
			break;
			
		case MSG_TOUCH_UP_FLICK:					/*上*/
			if(PpDiplayType==1)
			{
				PpDiplayType=2;
				SendMsg(MSG_DISPLAY);
			}	
			else if(PpDiplayType==2)
			{
				PpDiplayType=3;
				SendMsg(MSG_DISPLAY);
			}
			break;
			
		case MSG_TOUCH_DOWN_FLICK:				/*下*/	
			if(PpDiplayType==3)
			{
				PpDiplayType=2;
				SendMsg(MSG_DISPLAY);
			}	
			else if(PpDiplayType==2)
			{
				PpDiplayType=1;
				SendMsg(MSG_DISPLAY);
			}
			break;

		case MSG_TOUCH_DOWN_UP_FLICK:				/*左*/ 
			if(PpDiplayType==4)
			{
				PpDiplayType=5;
				SendMsg(MSG_DISPLAY);
			}
			break;
			
		case MSG_TOUCH_UP_DOWN_FLICK: 			/*右*/
			if(PpDiplayType==5)
			{
				PpDiplayType=4;
				SendMsg(MSG_DISPLAY);
			}		
			break;
			
		case MSG_PHONE_PREPARE_NUMBER:
			if(CheckpCurrentHandler(PhoneHadlerId.PhonePrepareId) == true)
				Flash_Display_PhonePrepare(0,0,PpNumlen,PrepareDialNumber,IsAndroidDisplay);
			break;
			
		case MSG_DISPLAY:
			if(PpDiplayType==0)			
				Flash_Display_PhonePrepare(PpAddr,PpNamelen,PpNumlen,PrepareDialNumber,IsAndroidDisplay);
			else if(PpDiplayType<=5)
				ShowSmsReply(PpDiplayType-1);
			break;
			
		case MSG_CALL_SEND_CMD_AGAIN:
			BC5_SetCallRedialNumber(BC5_CALL_AGAIN);
			break;	

		case MSG_UNLOADHANDLER_CMD:
			UnloadHandler(PhoneHadlerId.PhonePrepareId);
			break;
			
		default:
			return RET_MSG_UNHANDLED;				
	}
	return RET_MSG_HANDLED;
}





bool IsUnknownNumberCallerID(void)
{
	uint8* CallerID = pCurrentPhoneState->Name;
	uint8 Clen=strlen((char*)CallerID);
	uint8 pData[14] ={0x55,0x4E,0x4B,0x4E,0x4F,0x57,0x4E,0x20,0x4E,0x55,0x4D,0x42,0x45,0x52};
	if(Clen!=14)
		return FALSE;
	for(uint8 i=0;i<14;i++)
	{
		if(CallerID[i]!=pData[i])
		return  FALSE;
	}
	return TRUE;
}
	
bool CheckAndReplaceUnknownNumber(uint8 *Cid)					/*BC5的CallID消息取代"UNKNOWN NUMBER"字符串*/
{
	bool ret = FALSE;
	if(pCurrentPhoneState!=NULL && (PhoneRingHandlerExist==TRUE || PhoneCallHandlerExist==TRUE) )
	{
		if(pCurrentPhoneState->Name!=NULL && pCurrentPhoneState->Num!=NULL)
		{
			if(IsUnknownNumberCallerID()==TRUE){
				uint8 NumLen = *(Cid+1);
				uint8 NameLen = *(Cid+2);  
				uint8* Numid = Cid+3;
				uint8* Nameid = Cid+3+NumLen;
				uint8* NumTemp =NULL;
				uint8* NameTemp = NULL;

				NumTemp= osal_mem_alloc(NumLen+1);
				if(NumTemp== NULL) 
					return FALSE;	
				memset(NumTemp,0,NumLen+1);
				memcpy(NumTemp,Numid,NumLen);
				osal_mem_free(pCurrentPhoneState->Num);
				pCurrentPhoneState->Num = NumTemp;	
			
				NameTemp = osal_mem_alloc(NameLen+1);
				if(NameTemp == NULL) 
					return FALSE;
				memset(NameTemp,0,NameLen+1);
				memcpy(NameTemp,Nameid,NameLen);
				osal_mem_free(pCurrentPhoneState->Name);
				pCurrentPhoneState->Name = NameTemp; 
				
				SaveCallerIDToBuf(pCurrentPhoneState->type,CID_NUM_DATE,pCurrentPhoneState->Num,strlen((char*)(pCurrentPhoneState->Num)),false);
				SaveCallerIDToBuf(pCurrentPhoneState->type,CID_NAME,pCurrentPhoneState->Name,strlen((char*)(pCurrentPhoneState->Name)),false);
				SendMsg(MSG_DISPLAY);
				ret = TRUE;
			}
		}
	}
	return ret;
}


void IncomingCallNoticePhoneHandlerOutHandler(uint8* CData)		/*BC5的CallID消息取代外拨电话*/
{
	if(pCurrentPhoneState!=NULL && PhoneCallOutHandlerExist==TRUE){ 
		uint8*Num = NULL;
		uint8*Name =NULL;
		uint8 NumLen =*(CData+1);
		uint8 NameLen =*(CData+2);
		CData =CData+3;
		if(NumLen>0)
		{
			Num= osal_mem_alloc(NumLen+1);
			if(Num == NULL) 
				return;
			memset(Num,0,NumLen+1);
			memcpy(Num,CData,NumLen);
			osal_mem_free(pCurrentPhoneState->Num);
			pCurrentPhoneState->Num = Num;
			SaveCallerIDToBuf(pCurrentPhoneState->type,CID_NUM_DATE,pCurrentPhoneState->Num,strlen((char*)(pCurrentPhoneState->Num)),false);
		}
		if(NameLen>0)
		{
			CData+= NumLen;
			Name= osal_mem_alloc(NameLen+1);
			if(Name == NULL) 
				return;
			memset(Name,0,NameLen+1);
			memcpy(Name,CData,NameLen);			
			osal_mem_free(pCurrentPhoneState->Name);
			pCurrentPhoneState->Name = Name;	
			SaveCallerIDToBuf(pCurrentPhoneState->type,CID_NAME,pCurrentPhoneState->Name,strlen((char*)(pCurrentPhoneState->Name)),false);
		}
		SendMsg(MSG_DISPLAY);
	}
}

void IncomingCallNoticePhoneHandler(INCALL_DATA_T* CData, uint8*CidName){		/*Android的CallID消息取代原来信息*/
	uint8 name_len = 0;
	uint8 num_len =0;
	uint8* NameTemp = NULL;
	if(pCurrentPhoneState!=NULL && ((PhoneRingHandlerExist==TRUE && CData->name_type==CALLER_ID_IN && FreeState==FALSE) ||(PhoneCallOutHandlerExist==TRUE && CData->name_type==CALLER_ID_OUT))){
		if(pCurrentPhoneState->Name!=NULL)
			name_len = strlen((char*)(pCurrentPhoneState->Name));
		if(pCurrentPhoneState->Num!=NULL)
			num_len = strlen((char*)(pCurrentPhoneState->Num));
		if(CData->name_len>32)
			CData->name_len = 32;
		
		if(name_len>0 || CData->name_len==0)
			return;
		if(num_len>0 || CData->name_type==CALLER_ID_OUT){
			NameTemp= osal_mem_alloc(CData->name_len +1);
			if(NameTemp == NULL) 
				return;
			memset(NameTemp,0,CData->name_len +1);
			memcpy(NameTemp,CidName,CData->name_len);
			if(pCurrentPhoneState->Name!=NULL)
				osal_mem_free(pCurrentPhoneState->Name);		
			pCurrentPhoneState->Name = NameTemp;
			SaveCallerIDToBuf(pCurrentPhoneState->type,CID_NAME,pCurrentPhoneState->Name,strlen((char*)(pCurrentPhoneState->Name)),false);
			SendMsg(MSG_DISPLAY);
		}
	}	
	else if(CallerIDFromApk == CALLER_ID_NULL && PhoneRingHandlerExist==FALSE && PhoneCallHandlerExist==FALSE && PhoneCallOutHandlerExist==FALSE)
	{
		if(CData->name_type==CALLER_ID_IN||CData->name_type==CALLER_ID_OUT)
		{
			if(CData->name_len>32)
				CData->name_len = 32;
			if(CData->name_len==0)
				return;

			CallerIDApk = osal_mem_alloc((CData->name_len +1));
			if(CallerIDApk == NULL) 
				return;
			memset(CallerIDApk,0,CData->name_len +1);
			memcpy(CallerIDApk,CidName,CData->name_len);
			CallerIDFromApk = CData->name_type;
			osal_start_timerEx(GetAppTaskId(), MSG_CALL_ID_CLEAR, 5000);  
		}
	}
}

void ShowPhoneSound(bool display)
{
	uint8 buf[10]={0x00};
	buf[0] =PhoneSound.PhoneSoundStatus;
	if(display)
		buf[1]=0x01;		
	LCD_MainDataInterface(MI_PHONE_SOUND_STATUS,buf, 2);	
}

void Change_ShowPhoneSoundStatus(void)
{
	if(PhoneSound.PhoneSoundStatus ==1)
	{
		PhoneSound.PhoneSoundStatus =0;
		if(pCurrentPhoneState==pCurrentPhoneState->pNext)
		{		
			StopToneCounts =3;
			BC5_SetStopRingToneCmd(BC5_STOP_TONE);							/*-->来电铃声静音*/	
			osal_stop_timerEx(GetAppTaskId(), MSG_STOP_INCOMING_TONE);
			osal_start_timerEx(GetAppTaskId(), MSG_STOP_INCOMING_TONE ,1000);
		}
		StopMotorRunTime();		
	}
	else
	{
		PhoneSound.PhoneSoundStatus =1;
		if(pCurrentPhoneState==pCurrentPhoneState->pNext)
		{		
			StopToneCounts =3;
			BC5_SetStopRingToneCmd(BC5_START_TONE);							/*-->来电铃声静音*/	
			osal_stop_timerEx(GetAppTaskId(), MSG_STOP_INCOMING_TONE);
			osal_start_timerEx(GetAppTaskId(), MSG_STOP_INCOMING_TONE ,1000);
		}		
		MotoRun(MOTO_INCOMING_CALL);
	}
	ShowPhoneSound(true);
}


void ShowPhoneMic(bool display)
{
	uint8 buf[10]={0x00};
	buf[0] =PhoneMic.PhoneMicStatus;
	if(display)
		buf[1]=0x01;	
	LCD_MainDataInterface(MI_PHONE_Mic_STATUS,buf, 2);	
}
void Change_ShowPhoneMicStatus(void)
{
	if(PhoneMic.PhoneMicStatus==1)
	{
		PhoneMic.PhoneMicStatus =0;		
	}
	else
	{
		PhoneMic.PhoneMicStatus =1;
	}
	BC5_SetMicCmd(PhoneMic.PhoneMicStatus);
	ShowPhoneMic(true);
}



