#include "headfile.h"

uint8 AppTaskId=0;


static HandlerId nextId = INVALID_HANDLER_ID + 1;
struct HandlerList *pCurrentHandler = NULL;







uint8 SendMsg(MsgType msgType)
{
    uint8 iRet = FALSE;
    pU8 *pMsg = pS_OsMsgMalloc(sizeof(MsgType));

    if (pMsg)
    {
        *(MsgType *)pMsg = msgType;
        iRet = (pS_OsMsgSend(AppTaskId, pMsg) == SUCCESS);
    }
    
    return iRet;
}


uint8 GetMsg(MsgType*  msgRet)
{
     pU8 *pMsg = pS_OsMsgReceive(AppTaskId);

    if (pMsg)
    {
        *msgRet = *((MsgType*)pMsg);
        pS_OsMsgDeallocate(pMsg);
        
        return TRUE;
    }

    return FALSE;        
 
 }


void CleanAppTaskMessage(void)
{
        uint8 *pMsgTemp;

        do
        {
                pMsgTemp = pS_OsMsgReceive(AppTaskId);
                if(pMsgTemp)
                        pS_OsMsgDeallocate(pMsgTemp);
                else
                        break;
                
        }while(1);
}


bool PreHandler(MsgType message)
{
	switch(message)
	{
		//case MSG_DATE_TIME_UPDATE:
		case MSG_DATE_TIME_FORMAT_UPDATE:
			if((!PhoneRingHandlerExist) &&(DISPLAY_GOTO_SLEEP_FLAG))
				ClearScreen();
		case MSG_MEMORY_CHANGE:
		case MSG_DISPLAY_RING_MODE_STATE:	
		case MSG_SSD13XX_EXIT_SLEEP:
			if((!PhoneRingHandlerExist) &&(DISPLAY_GOTO_SLEEP_FLAG))
				SendMsg(MSG_DISPLAY);
			break;

		case MSG_SMS_NOTIFY:
		case MSG_MISSED_CALL_COUNT:
		case MSG_E_MAIL_NOTIFY:
		case MSG_DAILY_SCHEDULE_NOTIFY:
		case MSG_SOCIAL_NOTIFY:
		case MSG_ACTIVE_NFC:
			if(SLEEP_MODE_FLAG==true){
				return true;
			}
		case MSG_VOL_SET:
		case MSG_SLEEP_MODE_START:
		case MSG_SLEEP_MODE_EXIT:
			if( DISPLAY_GOTO_SLEEP_FLAG == true)
			{
				ClearScreen();
				SendMsg(MSG_DISPLAY);	          
			}
			break;
		case MSG_INCOMING_CALL_NOTIFY:
			if(SLEEP_MODE_FLAG==true){
				return true;
			}
		//case MSG_MUSIC_NOTIC:		
		case MSG_UPDATE_FONT_LIB:	
		case MSG_TEST_TP:	
		case MSG_CHARGE_STOP:
		case MSG_POWER_OFF_BATTERY:
		case MSG_LOW_BATTERY:
		case MSG_CHARGE_START:
		case MSG_BC5_BT_CHANGE: 
		case MSG_SIT_REMIND:
		case MSG_BLE_BOND_CHOOSE:
			
		case MSG_MEDAL:
			
		case MSG_HR_UPPER_REMIND:
		case MSG_HR_LOWER_REMIND:

		case MSG_USER_REMIND:

		case MSG_SLEEP_CLOSE_BT:
		case MSG_SLEEP_OPEN_BT:
		case MSG_BC5_DISCONNECT:
		case MSG_SYSTEM_FACTORY_SET_STATE:

		case MSG_CALL_STATE_LOAD_FIRST_CALL:
		case MSG_CALL_STATE_LOAD_SECOND_CALL:
		case MSG_CALL_STATE_LOAD_UNKNOW_CALL:
		case MSG_CALL_STATE_IDLE:
		//case MSG_SYNCING_START:
		case MSG_PAY_SYNCING_START:	
		case MSG_GOAL_OVER_NOTIFY:
		case MSG_CALL_STATE_OUT_GOING:
		case MSG_OTA:

		case MSG_GET_SCAN_ADD_FRIENDS:
		case MSG_GET_SCAN_ADD_FRIENDS_REC:	  	
		case MSG_GET_SCAN_ADD_FRIENDS_OK:	
		case MSG_GET_SCAN_ADD_FRIENDS_FAIL:

		case MSG_GET_SCAN_DEL_FRIENDS:	
		case MSG_GET_SCAN_DEL_FRIENDS_OK:	
		case MSG_GET_SCAN_MOTO:
		case MSG_GET_SCAN_MOTO_OK:
		case MSG_GET_SCAN_MOTO_FAIL:
		case MSG_GET_SCAN_EXP:
		case MSG_GET_SCAN_EXP_OK:	
		case MSG_GET_SCAN_EXP_FAIL:	
			break;

		case MSG_WAKEUP_ACTION:
			if(DISPLAY_GOTO_SLEEP_FLAG) {
				osal_start_timerEx(GetAppTaskId(), MSG_SYSTEM_SUSPEND, 200);	
			}
			break;

		case MSG_SEND_SOCIAL_MESSAGE:
			AncsRemoveFunction();
			return false;

		case MSG_ANCS_SMS_QUERY_UID:
			RemoveSMS();
			return false;

		case MSG_ANCS_SMS_RESET_SENDER:
			ResetSendInfo();
			return false;

		case MSG_ANCS_INCOMINGCALL_LATER_GET:
			AncsIncomingcalledGetInfo();
			return false;

		case MSG_ANCS_MISSCALL_LATER_GET:
			AncsMisscalledGetInfo();
			return false;

		case MSG_ANCS_EMAIL_LATER_GET:
			AncsEmailGetInfo();
			return false;
			
		case MSG_ANCS_SCHEDULE_LATER_GET:
			AncsScheduleGetInfo();
			return false;	
			
		case MSG_ANCS_SMS_LATER_GET:
			AncsSmsGetInfo();
			return false;

		case MSG_ANCS_CONNECTED_GET_ALL:
			AncsConnectedGetAllInfo();
			return false;

		case MSG_TOUCH_UP_SHORTKEY:
			if(!LCD_DISPLAY_FLAG)
				return false;

		default:
			return true;
	}    

	CancelSendSuspendEvent();
	if((Powerstate.powerstate== true)&&(Powerstate.standbystate== true))
	{
		SystemResume();
	}
	LCD_Sleep(LCD_EXIT_SLEEP);
	SetBrightness(1);
	return true;
}


uint16 AppEventHandler(uint8 task_id,uint16 events)
{
        MsgType message;

        if(PROGRAM_DATA_VALID_FLAG || UPDATE_FONTLIB || LED_TEST||TP_TEST || (!NfcTest) || (!GetCSN))
        {
                if ( events & SYS_EVENT_MSG )
                {
                        if(GetMsg(&message)==TRUE)
                        {
                                if(PreHandler(message))
                                {
                                        HandlerMsg(message,0);
                                }
                                
                                return events; 
                        }
                        else
                                return ( events ^ SYS_EVENT_MSG );
                }	
       }
    	return ( 0 );     
}

void AppInitTask(uint8 task_id)
{
	AppTaskId=task_id;
	SCAN_KEY_VALID_FLAG=true;
	LoadHandler(Standby_Handler,0);
	
}

uint8 GetAppTaskId(void)
{
	return AppTaskId;
}



/******************************************************************************************
*函数名:		HandlerMsg
*参数:
*返回值:
*描述:	消息流，当当前Handler对消息处理的返回值为FALSE时，将消息传给下一级
		handler处理，返回TRUE,则直接返回。
*编写:
*版本信息:
******************************************************************************************/
void HandlerMsg(MsgType msg, int iParam)
{
    int iRet;
    struct HandlerList *pTemp = pCurrentHandler;
    
    do
    {
        iRet = pTemp->pHandler(msg, iParam, pTemp->pParam);
        if(! iRet)
        {
            return;
        }
        
        pTemp = pTemp->pPrev;
            
    }
    while(pTemp != pCurrentHandler);
}

/******************************************************************************************
*函数名:		LoadHandler
*参数:
*返回值:
*描述:		将函数句柄压入堆栈
*编写:
*版本信息:
******************************************************************************************/
HandlerId LoadHandler (pHandlerFun pFun,  void *pParam)
{
    struct HandlerList *pNew;
    struct HandlerList *pTemp;
    
    if(pFun == NULL)
        return INVALID_HANDLER_ID;
    pNew = osal_mem_alloc(sizeof(struct HandlerList));
    if(pNew == NULL)
        return INVALID_HANDLER_ID;

	if(CurrentMenuIParam>0)
	{
		 UnloadHandler_WithoutDisplay(CurrentMenuIParam);
		 if((*(MENU_DIRECTION *)pParam == MENU_SLEEP_FORWARD) || (*(MENU_DIRECTION *)pParam == MENU_SLEEP_BACKWARD))/*主动卸载*/
		 {
		 	if(!CheckpCurrentHandler(StandbyState.MyId))  /*不是主界面*/
		 	{
		 		SendMsg(MSG_DISPLAY);
				return 0;
		 	}
		 }
	}
    DisplayUnicode_Stop();	
    osal_memset(pNew, 0, sizeof(struct HandlerList));
    pNew->pNext = NULL;
    pNew->pHandler = pFun;
    if(pCurrentHandler == NULL)
    {
        pNew->pPrev = pNew;
        pNew->pNext = pNew;
    }
    else
    {
        pTemp = pCurrentHandler->pNext;
        pCurrentHandler->pNext = pNew;
        pNew->pPrev = pCurrentHandler;
        pTemp->pPrev = pNew;
        pNew->pNext = pTemp;
    }
    
    pNew->Id = nextId;
    if (++nextId == INVALID_HANDLER_ID)
        nextId = INVALID_HANDLER_ID + 2;
    pCurrentHandler = pNew;
	SetSystemSuspendTime(DISPLAY_TIME_10S_FLAG);
    pNew->pParam =(void *)pFun(MSG_HANDLER_BEGIN, pNew->Id, pParam);
    osal_set_event(GetDisplayTaskId(), CLEAR_SCREEN_EVENT);
    SendMsg(MSG_DISPLAY);
    
    return pNew->Id;
}

/******************************************************************************************
*函数名:		UnloadHandler
*参数:
*返回值:
*描述:		将函数句柄出栈
*编写:
*版本信息:
******************************************************************************************/
void UnloadHandler (HandlerId handlerId)
{
    struct HandlerList *p1;
    struct HandlerList *pTemp = pCurrentHandler;
    bool IsCurrentHandler = TRUE;
    do
    {
        if(pTemp->Id == handlerId)
        {
            if(pTemp == pCurrentHandler)
            {
                pCurrentHandler = pCurrentHandler->pPrev;
            }
            p1 = pTemp->pPrev;
            p1->pNext = pTemp->pNext;
            p1->pNext->pPrev = p1;
            pTemp->pHandler(MSG_HANDLER_END, NULL, pTemp->pParam);
            osal_mem_free(pTemp);
	     if(IsCurrentHandler==TRUE){
                osal_set_event(GetDisplayTaskId(), CLEAR_SCREEN_EVENT);
                SendMsg(MSG_DISPLAY);
	     }            
            return;
        }
        else
        {
            pTemp = pTemp->pPrev;
	     IsCurrentHandler= FALSE;
        }
    }
    while(pTemp != pCurrentHandler);
}



/******************************************************************************************
*函数名:		UnloadHandler_WithoutDisplay
*参数:
*返回值:
*描述:		将函数句柄出栈
*编写:
*版本信息:
******************************************************************************************/
void UnloadHandler_WithoutDisplay (HandlerId handlerId)
{
    struct HandlerList *p1;
    struct HandlerList *pTemp = pCurrentHandler;
    do
    {
        if(pTemp->Id == handlerId)
        {
            if(pTemp == pCurrentHandler)
            {
                pCurrentHandler = pCurrentHandler->pPrev;
            }
            p1 = pTemp->pPrev;
            p1->pNext = pTemp->pNext;
            p1->pNext->pPrev = p1;
            pTemp->pHandler(MSG_HANDLER_END, NULL, pTemp->pParam);
            osal_mem_free(pTemp); 
            return;
        }
        else
        {
            pTemp = pTemp->pPrev;
        }
    }
    while(pTemp != pCurrentHandler);
}


/******************************************************************************************
*函数名:		UnloadMultiHandler
*参数:
*返回值:
*描述:		将多个函数句柄出栈，只保留最底层的handler
*编写:
*版本信息:
******************************************************************************************/
void UnloadMultiHandler (void)
{
    struct HandlerList *pTemp = pCurrentHandler;
    struct HandlerList *p1;

    
    while(pTemp->pPrev != pCurrentHandler)
    {
            pCurrentHandler = pTemp->pPrev;
            p1 = pTemp->pPrev;
            p1->pNext = pTemp->pNext;
            p1->pNext->pPrev = p1;
            pTemp->pHandler(MSG_HANDLER_END, NULL, pTemp->pParam);
            osal_mem_free(pTemp);
            pTemp = pCurrentHandler;
    }
    
}


bool CheckpCurrentHandler(HandlerId Id)
{
        if(pCurrentHandler->Id == Id)
                return true;
        else
                return false;
}

bool PutTheHandlerOnTop(HandlerId handlerId)
{
    struct HandlerList *p1;
    struct HandlerList *p2;
    struct HandlerList *pTemp = pCurrentHandler;
    do
    {
        if(pTemp->Id == handlerId)
        {
            if(pTemp != pCurrentHandler)
            {
                p1 = pTemp->pPrev;
                p1->pNext = pTemp->pNext;
                p1->pNext->pPrev = p1;

		  p2= pCurrentHandler->pNext;
                pCurrentHandler->pNext = pTemp;
                pTemp->pPrev = pCurrentHandler;
                p2->pPrev = pTemp;
                pTemp->pNext = p2;
		  pCurrentHandler = pTemp;
		  return true;
            }
	     else
	     {
	         return false;
	     }
        }
        else
        {
            pTemp = pTemp->pPrev;
        }
    }
    while(pTemp != pCurrentHandler);
    return false;
}

