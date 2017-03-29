#include "headfile.h"


RemindIdTypeTAG DifferentRemindId; 
SecondTimerUI STui=ST_PAGE;
AncsClear ClearAllType = Clear_Non;
RemindIdTypeTAG DifferentRemindId; 


uint8 TimerMenuFlag=1;
void StartDisconnectQuitEvent(uint32 time)
{
        osal_start_timerEx(GetAppTaskId(), MSG_DISCONNECT_QUIT_STATE, time);
}

void StopDisconnectQuitEvent(void)
{
        osal_stop_timerEx(GetAppTaskId(), MSG_DISCONNECT_QUIT_STATE);
        osal_clear_event(GetAppTaskId(), MSG_DISCONNECT_QUIT_STATE);
}

void IncomingCall_Recovery(uint8 ReFlag)
{
	if(ReFlag&INCOMING_CALL_SKIP_SMS)
		osal_start_timerEx(GetAppTaskId(), MSG_SMS_NOTIFY, 500);
	if(ReFlag&INCOMING_CALL_SKIP_MISSED_CALL)	
		osal_start_timerEx(GetAppTaskId(), MSG_MISSED_CALL_COUNT, 400);
	if(ReFlag&INCOMING_CALL_SKIP_EMAIL)
		osal_start_timerEx(GetAppTaskId(), MSG_E_MAIL_NOTIFY, 300);
	if(ReFlag&INCOMING_CALL_SKIP_SCHEDULE)	
		osal_start_timerEx(GetAppTaskId(), MSG_DAILY_SCHEDULE_NOTIFY, 200);
	if(ReFlag&INCOMING_CALL_SKIP_SOCIAL)
		osal_start_timerEx(GetAppTaskId(), MSG_SOCIAL_NOTIFY, 100);
}

AncsSendView AncsSend;
AncsHandler Sms;
/*短信回复Handler*/
uint16 AncsSendHandler(MsgType msg, int iParam, void *pContext)
{
	switch(msg)
	{
		case MSG_HANDLER_BEGIN:
			DifferentRemindId.AncsSendHandlerId = iParam;    
			AncsSend =SENDING_VIEW;
			SetSystemSuspendTime(DISPLAY_TIME_10S_FLAG);
			osal_start_timerEx(GetAppTaskId(), MSG_ANCS_REPLAY_FAIL, 5000);
			break;

		case MSG_HANDLER_END:
			DifferentRemindId.AncsSendHandlerId = 0;	  
			break;
			
/*用户操作*/
		case MSG_LEFTDOWN_SHORTKEY:				/*左键 单击*/	
		case MSG_LEFTDOWN_DOUBLE_SHORTKEY:		/*左键 双击*/
		case MSG_LEFTDOWN_LONGKEY:           			/*左键 长按*/
		case MSG_RIGHTDOWN_SHORTKEY: 				/*右键 单击*/	
		case MSG_RIGHTDOWN_DOUBLE_SHORTKEY:   	/*右键 双击*/	
		case MSG_RIGHTDOWN_LONGKEY:				/*右键 长按*/	
		case MSG_RIGHTDOWN_LONGKEY_UP:			/*右键 长按松开*/		
		case MSG_TOUCH_UP_FLICK:					/*上*/
		case MSG_TOUCH_DOWN_FLICK:				/*下*/
		case MSG_TOUCH_DOWN_UP_FLICK:				/*左*/ 
		case MSG_TOUCH_UP_DOWN_FLICK: 			/*右*/
		case MSG_TOUCH_UP_SHORTKEY:				/*触摸 单击*/  
		case MSG_TOUCH_UP_DOUBLE_SHORTKEY:		/*触摸 双击*/	
		case MSG_TOUCH_LONGKEY:					/*触摸 长按*/
			break;
			
		case MSG_ANCS_REPLAY_HANDLER_TIMEOUT:
			UnloadHandler(DifferentRemindId.AncsSendHandlerId);
			Sms.View = PRE_VIEW;
			break;

		case MSG_ANCS_REPLAY_SUCCESS:
			AncsSend =SEND_OK_VIEW;
			osal_stop_timerEx(GetAppTaskId(), MSG_ANCS_REPLAY_FAIL);
			osal_start_timerEx(GetAppTaskId(), MSG_ANCS_REPLAY_HANDLER_TIMEOUT, 2000);
			if(CheckpCurrentHandler(DifferentRemindId.AncsSendHandlerId))	
				SendMsg(MSG_DISPLAY);
			break;
			
		case MSG_ANCS_REPLAY_FAIL:
			AncsSend =SEND_FAIL_VIEW;
			osal_start_timerEx(GetAppTaskId(), MSG_ANCS_REPLAY_HANDLER_TIMEOUT, 2000);
			if(CheckpCurrentHandler(DifferentRemindId.AncsSendHandlerId))	
				SendMsg(MSG_DISPLAY);
			break;

		
		case MSG_DISPLAY:
			ShowAncsSend((uint8)AncsSend);
			break;

		default:
			return RET_MSG_UNHANDLED;
	}
	return RET_MSG_HANDLED;
}


/*短信息主页面Handler*/
uint16 SMSHomePageHandler(MsgType msg, int iParam, void *pContext)
{
	switch(msg)
	{
		case MSG_HANDLER_BEGIN:
			DifferentRemindId.SMSHomePageHandlerId = iParam;
			if(gShortMsgCount>0&&AncsMotoSkip==false)
				MotoRun(MOTO_SMS);										/*震动2秒*/
			if(AncsMotoSkip)
				AncsMotoSkip=false;
			SetSystemSuspendTime(DISPLAY_TIME_10S_FLAG);
			break;

		case MSG_HANDLER_END:
			StopMotorRunTime();
			DifferentRemindId.SMSHomePageHandlerId = 0;
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
		case MSG_TOUCH_UP_DOUBLE_SHORTKEY:		/*触摸 双击*/	
		case MSG_TOUCH_LONGKEY:					/*触摸 长按*/
			break;
			
		case MSG_RIGHTDOWN_SHORTKEY: 					/*右键 单击*/		
			UnloadHandler(DifferentRemindId.SMSHomePageHandlerId);
			break;
			
		case MSG_TOUCH_UP_SHORTKEY:					/*触摸 单击*/ 
			if(GetAncsMsgLayOut(Ancs_Sms,NULL))
			{
				UnloadHandler_WithoutDisplay(DifferentRemindId.SMSHomePageHandlerId);
				LoadHandler(SMSHandler, 0);	
			}
			break;
			
		case MSG_DISPLAY:
			LCD_MsgInterface(M_SMS,gShortMsgCount);				/*更新主界面*/			
			break;
			
		case MSG_SMS_NOTIFY:
			SetSocialNotifEventFlag();
			if(!SWITCH_ANCS_SMS)
				break;
			if(gShortMsgCount==0)
			{
				UnloadHandler(DifferentRemindId.SMSHomePageHandlerId);
				break;
			}
			if(CheckpCurrentHandler(DifferentRemindId.SMSHomePageHandlerId))
			{
				MotoRun(MOTO_SMS);										/*震动2秒*/
				LCD_MsgInterface(M_COUNT_UPDATE,gShortMsgCount);		/*更新主界面的数字*/
			}
			else if((gShortMsgCount > 0)&&(BondSuccessfulState)&&(!SLEEP_MODE_FLAG))
			{
				UnloadHandler(DifferentRemindId.SMSHomePageHandlerId);
				SendMsg(MSG_SMS_NOTIFY);
			}
			break;
			
		case MSG_REMOVE_ALL_ANCS:
			UnloadHandler(DifferentRemindId.SMSHomePageHandlerId);
			SendMsg(MSG_REMOVE_ALL_ANCS);
			break;
			
		default:
			return RET_MSG_UNHANDLED;
	}
	return RET_MSG_HANDLED;	
}

/*短信息详细内容页面Handler*/
uint16 SMSHandler(MsgType msg, int iParam, void *pContext)
{
	uint8 NumberofPage =3;
	switch(msg)
	{
		case MSG_HANDLER_BEGIN:
			DifferentRemindId.SMSHandlerId = iParam;
			Sms.View = PRE_VIEW;
			Sms.PreViewIndex=0;
			Sms.DetailViewIndex=0;   
			SetSystemSuspendTime(DISPLAY_TIME_10S_FLAG);
			break;

		case MSG_HANDLER_END:
			{
				StopMotorRunTime();
				uint8 rm=ForAncsHandlerEnd(Ancs_Sms);
				if(rm!=0xff)
					gShortMsgCount=rm;
				ResetSMSOffset(); 
				DifferentRemindId.SMSHandlerId = 0;	  
			}	  
			break;
/*用户操作*/
		case MSG_LEFTDOWN_SHORTKEY:				/*左键 单击*/	
		case MSG_LEFTDOWN_DOUBLE_SHORTKEY:		/*左键 双击*/
		case MSG_LEFTDOWN_LONGKEY:           			/*左键 长按*/
		case MSG_RIGHTDOWN_DOUBLE_SHORTKEY:   	/*右键 双击*/	
		case MSG_RIGHTDOWN_LONGKEY:				/*右键 长按*/	
		case MSG_RIGHTDOWN_LONGKEY_UP:			/*右键 长按松开*/		
		case MSG_TOUCH_UP_DOUBLE_SHORTKEY:		/*触摸 双击*/	
			break;

		case MSG_TOUCH_LONGKEY:					/*触摸 长按*/
#if 0
			if(Sms.View == PRE_VIEW)
			{
				ClearAllType = Clear_Sms;
				SendMsg(MSG_CLEAR_ALL);			
			}
#endif
			break;	
				
		case MSG_RIGHTDOWN_SHORTKEY: 				/*右键 单击*/
			switch(Sms.View)
			{
				case PRE_VIEW:
					UnloadHandler(DifferentRemindId.SMSHandlerId);	
					break;
				case DETAIL_VIEW:
#if 0	
					if(ReadEnd==1)			/*需求点:看完消息后删除本消息*/
					{
						uint8 index=Sms.PreViewIndex*4+Sms.DetailViewIndex;
						if(index<5)
						{
							uint8 rm=DeleteAncsMsg(Ancs_Sms,index);
							if(rm<=5)
								Sms.PreViewIndex=0;
							if(gShortMsgCount>0)
								gShortMsgCount--;
						}						
					}	
#endif
					Sms.View = PRE_VIEW;
					SendMsg(MSG_DISPLAY);	
					break;
				case REPLAY_VIEW1:
				case REPLAY_VIEW2:
				case REPLAY_VIEW3:	
				case EMOTION_VIEW1:
				case EMOTION_VIEW2:
					Sms.View = DETAIL_VIEW;
					SendMsg(MSG_DISPLAY);				
					break;
			}
			break;	
			
		case MSG_TOUCH_UP_SHORTKEY:				/*触摸 单击*/ 
			switch(Sms.View)
			{
				case PRE_VIEW:
					{
						uint8 Getlayout[ANCS_LAYOUT_MAX*2] ={0};
						memset(Getlayout,0x40,ANCS_LAYOUT_MAX*2);
						if(!GetAncsMsgLayOut(Ancs_Sms,Getlayout))
						{
							UnloadHandler(DifferentRemindId.SMSHandlerId);
							break;
						}
						uint8 TouchID = Get_Screen_Locaton(NumberofPage,1);
						if(Getlayout[Sms.PreViewIndex*NumberofPage+TouchID]!=0x40)
						{
							if(Sms.PreViewIndex*NumberofPage+TouchID<=5)
							{
								if(Getlayout[Sms.PreViewIndex*NumberofPage+TouchID]>=0x20)
								{
									if(gShortMsgCount>0)
										gShortMsgCount--;	
								}
								Sms.DetailViewIndex = TouchID;
								Sms.View = DETAIL_VIEW;
								ReadEnd=0;
								SendMsg(MSG_DISPLAY);	
							}
						}
					}
					break;
				case DETAIL_VIEW:
					{
#if 0
						uint8 TouchID = Get_Screen_Locaton(4,3);
						if(TouchID==10&& SMS_FROM_ANDROID)
						{
							uint8 Getlayout[ANCS_LAYOUT_MAX*2] ={0};
							memset(Getlayout,0x40,ANCS_LAYOUT_MAX*2);
							if(GetAncsMsgLayOut(Ancs_Sms,Getlayout))
							{
								uint8 LayoutOffset =Sms.PreViewIndex*4;
								uint8 DetailTemp =Sms.DetailViewIndex;
								if(DetailTemp==0x30)
									DetailTemp =0x00;
								LayoutOffset+=DetailTemp;
								if(LayoutOffset<5)
								{
									if(Getlayout[LayoutOffset]!=0x40)
									{
										uint32 SmsAddr = ANCS_BASE_ADDR+(0x00002000UL*2)+Getlayout[LayoutOffset]*256;
										SetSmsSenderAddr(SmsAddr);
										Sms.View = REPLAY_VIEW1;
										SendMsg(MSG_DISPLAY);	
									}
								}
							}
						}
#endif
					}
					break;
				case REPLAY_VIEW1:
					{
#if 0
						uint8 TouchID = Get_Screen_Locaton(4,3);
						uint8 ViewIndex =0x10; 
						if(TouchID==1)
						{
							Sms.View = EMOTION_VIEW1;
							SendMsg(MSG_DISPLAY);								
						}
						else if((TouchID!=0)&&(TouchID!=2))
						{
							/*给APP发送TouchID*/
							if((TouchID==3)||(TouchID==4)||(TouchID==5)){
								TouchID=1;
							}else if((TouchID==6)||(TouchID==7)||(TouchID==8)){
								TouchID=2;
							}else if((TouchID==9)||(TouchID==10)||(TouchID==11)){
								TouchID=3;
							}
							ViewIndex+=TouchID;
							SendSms(ViewIndex);
							LoadHandler(AncsSendHandler, 0);				
						}
#endif
					}
					break;
				case REPLAY_VIEW2:
					{
#if 0
						uint8 TouchID = Get_Screen_Locaton(4,3);
						uint8 ViewIndex =0x13; 
						if(TouchID==1)
						{
							Sms.View = EMOTION_VIEW1;
							SendMsg(MSG_DISPLAY);								
						}
						else if((TouchID!=0)&&(TouchID!=2))
						{
							/*给APP发送TouchID*/
							if((TouchID==3)||(TouchID==4)||(TouchID==5)){
								TouchID=1;
							}else if((TouchID==6)||(TouchID==7)||(TouchID==8)){
								TouchID=2;
							}else if((TouchID==9)||(TouchID==10)||(TouchID==11)){
								TouchID=3;
							}
							ViewIndex+=TouchID;
							SendSms(ViewIndex);
							LoadHandler(AncsSendHandler, 0);				
						}
#endif
					}
					break;		
				case REPLAY_VIEW3:
					{
#if 0
						uint8 TouchID = Get_Screen_Locaton(4,3);
						uint8 ViewIndex =0x16; 
						if(TouchID==1)
						{
							Sms.View = EMOTION_VIEW1;
							SendMsg(MSG_DISPLAY);								
						}
						else if((TouchID!=0)&&(TouchID!=2))
						{
							/*给APP发送TouchID*/
							if((TouchID==3)||(TouchID==4)||(TouchID==5)){
								TouchID=1;
							}else if((TouchID==6)||(TouchID==7)||(TouchID==8)){
								TouchID=2;
							}else if((TouchID==9)||(TouchID==10)||(TouchID==11)){
								TouchID=3;
								break;
							}
							ViewIndex+=TouchID;
							SendSms(ViewIndex);
							LoadHandler(AncsSendHandler, 0);				
						}
#endif
					}
					break;						
				case EMOTION_VIEW1:
					{
#if 0
						uint8 TouchID = Get_Screen_Locaton(4,4);
						uint8 EmoIndex =0; 
						if( TouchID==0 )								// 11点位置
						{
							EmoIndex=0x05;
						}
						else if((TouchID==1) || (TouchID==2))			// 0点位置
						{
							EmoIndex=0x02;
						}	
						else if(TouchID==3)							// 1点位置		
						{
							EmoIndex=0x04;
						}
						else if((TouchID==4) || (TouchID==8))			// 9点位置					
						{
							EmoIndex=0x07;
						}
						else if((TouchID==7) || (TouchID==11))			// 3点位置				
						{
							EmoIndex=0x01;	
						}				
						else if (TouchID==12)							// 7点位置	
						{
       						EmoIndex=0x06;
						}
						else if ((TouchID==13) || (TouchID==14))		// 6点位置
						{
							EmoIndex=0x0F;
						}
						else if (TouchID==15)							// 5点位置
						{
							EmoIndex=0x03;
						}
						if(EmoIndex>0)
						{
							/*给APP发送EmoIndex*/
							SendSms(EmoIndex);
							LoadHandler(AncsSendHandler, 0);
						}
#endif
					}
					break;
				case EMOTION_VIEW2:
					{
#if 0
						uint8 TouchID = Get_Screen_Locaton(4,4);
						uint8 EmoIndex =0; 
						if( TouchID==0 )								// 11点位置
						{
							EmoIndex=0x08;
						}
						else if((TouchID==1) || (TouchID==2))			// 0点位置
						{
							EmoIndex=0x0E;
						}	
						else if(TouchID==3)							// 1点位置		
						{
							EmoIndex=0x0C;
						}
						else if((TouchID==4) || (TouchID==8))			// 9点位置					
						{
							EmoIndex=0x09;
						}
						else if((TouchID==7) || (TouchID==11))			// 3点位置				
						{
							EmoIndex=0x0B;	
						}				
						else if (TouchID==12)							// 7点位置	
						{
       						EmoIndex=0x0A;
						}
						else if ((TouchID==13) || (TouchID==14))		// 6点位置
						{
							EmoIndex=0x10;
						}
						else if (TouchID==15)							// 5点位置
						{
							EmoIndex=0x0D;
						}
						if(EmoIndex>0)
						{
							/*给APP发送EmoIndex*/
							SendSms(EmoIndex);
							LoadHandler(AncsSendHandler, 0);
						}
#endif
					}
					break;                    
			}
			break;	
			
		case MSG_TOUCH_UP_FLICK:			/*上*/
			switch(Sms.View)
			{
				case PRE_VIEW:
					if(Sms.PreViewIndex==0)
					{
						uint8 Getlayout[ANCS_LAYOUT_MAX*2] ={0};
						memset(Getlayout,0x40,ANCS_LAYOUT_MAX*2);
						GetAncsMsgLayOut(Ancs_Sms,Getlayout);
						if(Getlayout[NumberofPage]!=0x40)
						{
							Sms.PreViewIndex=1;
							ReadAncsMsg_ToKl17(Ancs_Sms,Sms.PreViewIndex,0x40,NumberofPage,De_INIT);
						}
					}					
					break;
				case DETAIL_VIEW:
					if(!ReadAncsMsg_ToKl17(Ancs_Sms,Sms.PreViewIndex,Sms.DetailViewIndex,NumberofPage,De_NEXT))
						UnloadHandler(DifferentRemindId.SMSHandlerId);		
					break;
				case REPLAY_VIEW1:
					//Sms.View = REPLAY_VIEW2;
					//SendMsg(MSG_DISPLAY);					
					break;
				case REPLAY_VIEW2:
					//Sms.View = REPLAY_VIEW3;
					//SendMsg(MSG_DISPLAY);	
					break;
				case REPLAY_VIEW3:
					break;				
			}
			break;
			
		case MSG_TOUCH_DOWN_FLICK:		/*下*/	
			switch(Sms.View)
			{
				case PRE_VIEW:
					if(Sms.PreViewIndex==1)
					{
						uint8 Getlayout[ANCS_LAYOUT_MAX*2] ={0};
						memset(Getlayout,0x40,ANCS_LAYOUT_MAX*2);
						GetAncsMsgLayOut(Ancs_Sms,Getlayout);
						if(Getlayout[0]!=0x40)
						{
							Sms.PreViewIndex=0;
							ReadAncsMsg_ToKl17(Ancs_Sms,Sms.PreViewIndex,0x40,NumberofPage,De_INIT);
						}
					}					
					break;
				case DETAIL_VIEW:
					if(!ReadAncsMsg_ToKl17(Ancs_Sms,Sms.PreViewIndex,Sms.DetailViewIndex,NumberofPage,De_PRE))
						UnloadHandler(DifferentRemindId.SMSHandlerId);		
					break;
				case REPLAY_VIEW1:	
					break;
				case REPLAY_VIEW2:
					//Sms.View = REPLAY_VIEW1;
					//SendMsg(MSG_DISPLAY);	
					break;
				case REPLAY_VIEW3:
					//Sms.View = REPLAY_VIEW2;
					//SendMsg(MSG_DISPLAY);					
					break;						
			}
			break;
			
		case MSG_TOUCH_DOWN_UP_FLICK:				/*左*/ 
			switch(Sms.View)
			{
				case EMOTION_VIEW1:
					//Sms.View = EMOTION_VIEW2;
					//SendMsg(MSG_DISPLAY);					
					break;
				case EMOTION_VIEW2:				
					break;	
			}
			break;
			
		case MSG_TOUCH_UP_DOWN_FLICK: 			/*右*/
			switch(Sms.View)
			{
				case DETAIL_VIEW:
					{
						uint8 index=Sms.PreViewIndex*NumberofPage+Sms.DetailViewIndex;
						if(index<5)
						{
							uint8 rm=DeleteAncsMsg(Ancs_Sms,index);
							if(rm<=5)
								Sms.PreViewIndex=0;
							Sms.View = PRE_VIEW;
							SendMsg(MSG_DISPLAY);
						}							
					}
					break;			
				case EMOTION_VIEW1:							
					break;	
				case EMOTION_VIEW2:
					//Sms.View = EMOTION_VIEW1;
					//SendMsg(MSG_DISPLAY);						
					break;		
			}			
			break;
			
		case MSG_SMS_NOTIFY:
			SetSocialNotifEventFlag();			
			if(CheckpCurrentHandler(DifferentRemindId.SMSHandlerId)) {
				if(gShortMsgCount>0)
				{
					MotoRun(MOTO_SMS);	
					SetSystemSuspendTime(DISPLAY_TIME_10S_FLAG);
					if(Sms.View==DETAIL_VIEW)
					{
						Sms.PreViewIndex=0;
						Sms.DetailViewIndex=0x30;
						if(gShortMsgCount>0)
							gShortMsgCount--;						
						ReadEnd=0;
					}
					SendMsg(MSG_DISPLAY);	
				}
			}
			else if((gShortMsgCount > 0)&&(BondSuccessfulState)&&(!SLEEP_MODE_FLAG))
			{
				UnloadHandler(DifferentRemindId.SMSHandlerId);
				SendMsg(MSG_SMS_NOTIFY);
			}
			break;
			
		case MSG_DISPLAY:	
			switch(Sms.View)
			{
				case PRE_VIEW:
					if(!ReadAncsMsg_ToKl17(Ancs_Sms,Sms.PreViewIndex,0x40,NumberofPage,De_INIT))
						UnloadHandler(DifferentRemindId.SMSHandlerId);	
					break;
					
				case DETAIL_VIEW:
					{
						uint8 SmsAndroid=0;
						//if(SMS_FROM_ANDROID)
							//SmsAndroid=1;
						LCD_MainDataInterface(MI_SMS_FROM_ANDROID,&SmsAndroid,1);
						if(!ReadAncsMsg_ToKl17(Ancs_Sms,Sms.PreViewIndex,Sms.DetailViewIndex,NumberofPage,De_INIT))
							UnloadHandler(DifferentRemindId.SMSHandlerId);	
						else if(Sms.DetailViewIndex==0x30)
						{
							Sms.DetailViewIndex =0;
						}		
					}
					break;
				case REPLAY_VIEW1:
				case REPLAY_VIEW2:
				case REPLAY_VIEW3:	
				case EMOTION_VIEW1:
				case EMOTION_VIEW2:
					//ShowSmsReply((uint8)(Sms.View-REPLAY_VIEW1));
					break;					
			}	
			SetSocialNotifEventFlag();
			break;

		case MSG_REMOVE_ALL_ANCS:
			UnloadHandler(DifferentRemindId.SMSHandlerId);	
			SendMsg(MSG_REMOVE_ALL_ANCS);
			break;


		default:
			return RET_MSG_UNHANDLED;
	}
	return RET_MSG_HANDLED;
}

AncsHandler MissedCall;
/*未接来电主页面Handler*/
uint16 MissedCallHomePageHandler(MsgType msg, int iParam, void *pContext)
{
	switch(msg)
	{
		case MSG_HANDLER_BEGIN:
			DifferentRemindId.MissedCallHomePageHandlerId = iParam;
			if(gMissedCallCount>0&&AncsMotoSkip==false)
			{
				MotoRun(MOTO_MISSED_CALL);									/*震动2秒*/
			}
			if(AncsMotoSkip)
				AncsMotoSkip=false;
			SetSystemSuspendTime(DISPLAY_TIME_10S_FLAG);
			break;

		case MSG_HANDLER_END:
			StopMotorRunTime();
			DifferentRemindId.MissedCallHomePageHandlerId = 0;
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
		case MSG_TOUCH_UP_DOUBLE_SHORTKEY:		/*触摸 双击*/	
		case MSG_TOUCH_LONGKEY:					/*触摸 长按*/
			break;
			
		case MSG_RIGHTDOWN_SHORTKEY: 				/*右键 单击*/		
			UnloadHandler(DifferentRemindId.MissedCallHomePageHandlerId);
			break;
			
		case MSG_TOUCH_UP_SHORTKEY:				/*触摸 单击*/ 
			if(GetAncsMsgLayOut(Ancs_MissedCall,NULL))
			{
				UnloadHandler_WithoutDisplay(DifferentRemindId.MissedCallHomePageHandlerId);
				LoadHandler(MissedCallAlertHandler, 0);	
			}
			break;
			
		case MSG_DISPLAY:
			LCD_MsgInterface(M_MISSEDCALL,gMissedCallCount);		/*更新主界面*/
			break;
			
		case MSG_MISSED_CALL_COUNT:
			if(!SWITCH_ANCS_MISSED_CALL)
				break;	
			if(gMissedCallCount==0)
			{
				UnloadHandler(DifferentRemindId.MissedCallHomePageHandlerId);
				break;
			}
			if(CheckpCurrentHandler(DifferentRemindId.MissedCallHomePageHandlerId))
			{
				MotoRun(MOTO_MISSED_CALL);								/*震动2秒*/
				LCD_MsgInterface(M_COUNT_UPDATE,gMissedCallCount);		/*更新主界面的数字*/
			}
			else if((gMissedCallCount > 0)&&(BondSuccessfulState)&&(!SLEEP_MODE_FLAG))
			{
				UnloadHandler(DifferentRemindId.MissedCallHomePageHandlerId);
				SendMsg(MSG_MISSED_CALL_COUNT);
			}
			break;

		case MSG_REMOVE_ALL_ANCS:
			UnloadHandler(DifferentRemindId.MissedCallHomePageHandlerId);	
			SendMsg(MSG_REMOVE_ALL_ANCS);
			break;


					
		default:
			return RET_MSG_UNHANDLED;
	}
	return RET_MSG_HANDLED;	
}

/*未接来电详细内容页面Handler*/
uint16 MissedCallAlertHandler(MsgType msg, int iParam, void *pContext)
{
	uint8 NumberofPage =3;
	switch(msg)
	{
		case MSG_HANDLER_BEGIN:
			DifferentRemindId.MissedCallAlertHandlerId = iParam;
			MissedCall.View =PRE_VIEW;
			MissedCall.PreViewIndex =0;
			MissedCall.DetailViewIndex=0x40;	
			SetSystemSuspendTime(DISPLAY_TIME_10S_FLAG);
			break;

		case MSG_HANDLER_END:
			StopMotorRunTime();
			DifferentRemindId.MissedCallAlertHandlerId = 0;
			gMissedCallCount=0;
			break;
/*用户操作*/
		case MSG_LEFTDOWN_SHORTKEY:				/*左键 单击*/	
		case MSG_LEFTDOWN_DOUBLE_SHORTKEY:		/*左键 双击*/
		case MSG_LEFTDOWN_LONGKEY:           			/*左键 长按*/
		case MSG_RIGHTDOWN_DOUBLE_SHORTKEY:   	/*右键 双击*/	
		case MSG_RIGHTDOWN_LONGKEY:				/*右键 长按*/	
		case MSG_RIGHTDOWN_LONGKEY_UP:			/*右键 长按松开*/		
		case MSG_TOUCH_DOWN_UP_FLICK:				/*左*/ 
		case MSG_TOUCH_UP_DOWN_FLICK: 			/*右*/
		case MSG_TOUCH_UP_DOUBLE_SHORTKEY:		/*触摸 双击*/	
			break;

		case MSG_TOUCH_LONGKEY:					/*触摸 长按*/
#if 0
			if(MissedCall.View == PRE_VIEW)
			{
				ClearAllType = Clear_MissedCall;
				SendMsg(MSG_CLEAR_ALL);		
			}
#endif
			break;	
			
		case MSG_RIGHTDOWN_SHORTKEY: 				/*右键 单击*/	
			switch(MissedCall.View)
			{
				case PRE_VIEW:
					UnloadHandler(DifferentRemindId.MissedCallAlertHandlerId);
					break;
				case DETAIL_VIEW:
					break;
			}
			break;
			
		case MSG_TOUCH_UP_SHORTKEY:				/*触摸 单击*/ 
			switch(MissedCall.View)
			{
				case PRE_VIEW:	
					{
						uint8 Getlayout[ANCS_LAYOUT_MAX*2] ={0};
						memset(Getlayout,0x40,ANCS_LAYOUT_MAX*2);
						if(!GetAncsMsgLayOut(Ancs_MissedCall,Getlayout))
						{
							UnloadHandler(DifferentRemindId.MissedCallAlertHandlerId);
							break;
						}
						uint8 TouchID = Get_Screen_Locaton(NumberofPage,1);
						if(Getlayout[MissedCall.PreViewIndex*NumberofPage+TouchID]!=0x40)
						{
							if((MissedCall.PreViewIndex*NumberofPage+TouchID)<=5)
							{	
								#if 0
								uint32 MissAddr =ReadAncsMsg(Ancs_MissedCall,(MissedCall.PreViewIndex*4+TouchID));
								if(MissAddr>0)
									LoadPhonePrepareHandler(MissAddr,1,0,NULL,SMS_FROM_ANDROID);
								#else
								MissedCall.DetailViewIndex = TouchID;			
								SendMsg(MSG_DISPLAY);	
								#endif
							}
						}
					}					
					break;
				case DETAIL_VIEW:
					break;
			}			
			break;
					
		case MSG_TOUCH_UP_FLICK:				/*上*/
			switch(MissedCall.View)
			{
				case PRE_VIEW:
					if(MissedCall.PreViewIndex==0)
					{
						uint8 Getlayout[ANCS_LAYOUT_MAX*2] ={0};
						memset(Getlayout,0x40,ANCS_LAYOUT_MAX*2);
						GetAncsMsgLayOut(Ancs_MissedCall,Getlayout);
						if(Getlayout[NumberofPage]!=0x40)
						{
							MissedCall.PreViewIndex=1;
							ReadAncsMsg_ToKl17(Ancs_MissedCall,MissedCall.PreViewIndex,0x40,NumberofPage,De_INIT);
						}
					}					
					break;
			}
			break;	
			
		case MSG_TOUCH_DOWN_FLICK:			/*下*/	
			switch(MissedCall.View)
			{
				case PRE_VIEW:
					if(MissedCall.PreViewIndex==1)
					{
						uint8 Getlayout[ANCS_LAYOUT_MAX*2] ={0};
						memset(Getlayout,0x40,ANCS_LAYOUT_MAX*2);
						GetAncsMsgLayOut(Ancs_MissedCall,Getlayout);
						if(Getlayout[0]!=0x40)
						{
							MissedCall.PreViewIndex=0;
							ReadAncsMsg_ToKl17(Ancs_MissedCall,MissedCall.PreViewIndex,0x40,NumberofPage,De_INIT);
						}
					}					
					break;
			}			
			break;		
			
		case MSG_MISSED_CALL_COUNT:
			if(CheckpCurrentHandler(DifferentRemindId.MissedCallAlertHandlerId))
			{
				if(gMissedCallCount>0)
				{
					MotoRun(MOTO_MISSED_CALL);	
					SetSystemSuspendTime(DISPLAY_TIME_10S_FLAG);
					SendMsg(MSG_DISPLAY);	
				}
			}
			else if((gMissedCallCount > 0)&&(BondSuccessfulState)&&(!SLEEP_MODE_FLAG))
			{
				UnloadHandler(DifferentRemindId.MissedCallAlertHandlerId);   
				SendMsg(MSG_MISSED_CALL_COUNT);
			}
			break;
			
		case MSG_DISPLAY:
			switch(MissedCall.View)
			{	
				case PRE_VIEW:
					if(!ReadAncsMsg_ToKl17(Ancs_MissedCall,MissedCall.PreViewIndex,MissedCall.DetailViewIndex,NumberofPage,De_INIT))
						UnloadHandler(DifferentRemindId.MissedCallAlertHandlerId);  
					MissedCall.DetailViewIndex=0x40; 
					break;
					
				case DETAIL_VIEW:
					break;
			}			         
			break;

		case MSG_REMOVE_ALL_ANCS:
			UnloadHandler(DifferentRemindId.MissedCallAlertHandlerId);  
			SendMsg(MSG_REMOVE_ALL_ANCS);
			break;

		default:
			return RET_MSG_UNHANDLED;
	}
	return RET_MSG_HANDLED;
}


/*通话记录Handler*/
AncsHandler PhoneRecord;
uint16 PhoneRecordHandler(MsgType msg, int iParam, void *pContext)
{
	switch(msg)
	{
		case MSG_HANDLER_BEGIN:
			DifferentRemindId.PhoneRecordHandlerId = iParam;
			PhoneRecord.View =PRE_VIEW;
			PhoneRecord.PreViewIndex =0;
			PhoneRecord.DetailViewIndex=0x40;	
			SetSystemSuspendTime(DISPLAY_TIME_10S_FLAG);
			break;

		case MSG_HANDLER_END:
			StopMotorRunTime();
			DifferentRemindId.PhoneRecordHandlerId = 0;
			//gMissedCallCount=0;
			break;
/*用户操作*/
		case MSG_LEFTDOWN_SHORTKEY:				/*左键 单击*/	
		case MSG_LEFTDOWN_DOUBLE_SHORTKEY:		/*左键 双击*/
		case MSG_LEFTDOWN_LONGKEY:           			/*左键 长按*/
		case MSG_RIGHTDOWN_DOUBLE_SHORTKEY:   	/*右键 双击*/	
		case MSG_RIGHTDOWN_LONGKEY:				/*右键 长按*/	
		case MSG_RIGHTDOWN_LONGKEY_UP:			/*右键 长按松开*/		
		case MSG_TOUCH_DOWN_UP_FLICK:				/*左*/ 
		case MSG_TOUCH_UP_DOWN_FLICK: 			/*右*/
		case MSG_TOUCH_UP_DOUBLE_SHORTKEY:		/*触摸 双击*/	
			break;
			
		case MSG_TOUCH_LONGKEY:					/*触摸 长按*/
			if(PhoneRecord.View == PRE_VIEW)
			{
				ClearAllType = Clear_PhoneRecord;
				SendMsg(MSG_CLEAR_ALL);			
			}
			break;	
		case MSG_RIGHTDOWN_SHORTKEY: 				/*右键 单击*/	
			switch(PhoneRecord.View)
			{
				case PRE_VIEW:
					UnloadHandler(DifferentRemindId.PhoneRecordHandlerId);
					break;
				case DETAIL_VIEW:
					break;
			}
			break;
			
		case MSG_TOUCH_UP_SHORTKEY:				/*触摸 单击*/ 
			switch(PhoneRecord.View)
			{
				case PRE_VIEW:	
					{
						uint8 Getlayout[ANCS_LAYOUT_MAX*2] ={0};
						memset(Getlayout,0x40,ANCS_LAYOUT_MAX*2);
						if(!GetAncsMsgLayOut(Call_Record,Getlayout))
						{
							UnloadHandler(DifferentRemindId.PhoneRecordHandlerId);
							break;
						}
						uint8 TouchID = Get_Screen_Locaton(4,1);
						if(Getlayout[PhoneRecord.PreViewIndex*4+TouchID]>=0x20)
						{
							if((PhoneRecord.PreViewIndex*4+TouchID)<=5)
							{	
								uint32 MissAddr =ReadAncsMsg(Call_Record,(PhoneRecord.PreViewIndex*4+TouchID));
								if(MissAddr>0)
									LoadPhonePrepareHandler(MissAddr,1,0,NULL,Android_ios);

							}
						}
					}					
					break;
				case DETAIL_VIEW:
					break;
			}			
			break;
					
		case MSG_TOUCH_UP_FLICK:				/*上*/
			switch(PhoneRecord.View)
			{
				case PRE_VIEW:
					if(PhoneRecord.PreViewIndex==0)
					{
						uint8 Getlayout[ANCS_LAYOUT_MAX*2] ={0};
						memset(Getlayout,0x40,ANCS_LAYOUT_MAX*2);
						GetAncsMsgLayOut(Call_Record,Getlayout);
						if(Getlayout[4]!=0x40)
						{
							PhoneRecord.PreViewIndex=1;
							ReadAncsMsg_ToKl17(Call_Record,PhoneRecord.PreViewIndex,0x40,4,De_INIT);
						}
					}					
					break;
			}
			break;	
			
		case MSG_TOUCH_DOWN_FLICK:			/*下*/	
			switch(PhoneRecord.View)
			{
				case PRE_VIEW:
					if(PhoneRecord.PreViewIndex==1)
					{
						uint8 Getlayout[ANCS_LAYOUT_MAX*2] ={0};
						memset(Getlayout,0x40,ANCS_LAYOUT_MAX*2);
						GetAncsMsgLayOut(Call_Record,Getlayout);
						if(Getlayout[0]!=0x40)
						{
							PhoneRecord.PreViewIndex=0;
							ReadAncsMsg_ToKl17(Call_Record,PhoneRecord.PreViewIndex,0x40,4,De_INIT);
						}
					}					
					break;
			}			
			break;		
				
		case MSG_DISPLAY:
			switch(PhoneRecord.View)
			{	
				case PRE_VIEW:
					if(!ReadAncsMsg_ToKl17(Call_Record,PhoneRecord.PreViewIndex,PhoneRecord.DetailViewIndex,4,De_INIT))
						UnloadHandler(DifferentRemindId.PhoneRecordHandlerId);  
					PhoneRecord.DetailViewIndex=0x40; 
					break;
					
				case DETAIL_VIEW:
					break;
			}			         
			break;

		default:
			return RET_MSG_UNHANDLED;
	}
	return RET_MSG_HANDLED;
}

uint8 IncomingCallSkipSave; 
/*来电主页面Handler*/
uint16 IncomingCallHandler(MsgType msg, int iParam, void *pContext)
{
	switch(msg)
	{
		case MSG_HANDLER_BEGIN:
			DifferentRemindId.IncomingCallAlertHandlerId = iParam;
			IncomingCallSkipSave=0;
			SetSystemSuspendTime(DISPLAY_TIME_10S_FLAG);
			break;

		case MSG_HANDLER_END:
			DifferentRemindId.IncomingCallAlertHandlerId = 0;
			gIncomingCallCount = 0;
			StopMotorRunTime();
			IncomingCall_Recovery(IncomingCallSkipSave);
			IncomingCallSkipSave=0;	 
			SetSystemSuspendTime(DISPLAY_TIME_10S_FLAG);	
			break;
			
		case MSG_SYSTEM_SUSPEND:				
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
		case MSG_TOUCH_UP_SHORTKEY:				/*触摸 单击*/  
		case MSG_TOUCH_UP_DOUBLE_SHORTKEY:		/*触摸 双击*/	
		case MSG_TOUCH_LONGKEY:					/*触摸 长按*/
			break;
			
		case MSG_RIGHTDOWN_SHORTKEY: 				/*右键 单击*/	 	
		case MSG_INCOMING_CALL_UNLOAD:
			UnloadHandler(DifferentRemindId.IncomingCallAlertHandlerId);
			break;


		case MSG_INCOMING_CALL_NOTIFY:
			if(gIncomingCallCount < 1)
			{
				SendMsg(MSG_INCOMING_CALL_UNLOAD);
				break;
			}
			if(CheckpCurrentHandler(DifferentRemindId.IncomingCallAlertHandlerId))
			{
				SetSystemSuspendTime(DISPLAY_TIME_10S_FLAG);
				SendMsg(MSG_DISPLAY);
			}
			else
			{
				UnloadHandler(DifferentRemindId.IncomingCallAlertHandlerId);
				SendMsg(msg);
			}
			break;
			
		case MSG_DISPLAY:
			if(gIncomingCallCount < 1)
			{
				SendMsg(MSG_INCOMING_CALL_UNLOAD);
				break;
			}
			if(!ReadAncsMsg_ToKl17(Ancs_IncomingCall,0,0x40,1,De_INIT))
				SendMsg(MSG_INCOMING_CALL_UNLOAD);
			MotoRun(MOTO_INCOMING_CALL);
			break;

		case MSG_CHARGE_START:
			osal_start_timerEx(GetAppTaskId(), MSG_INCOMING_CALL_UNLOAD, 100);
			return RET_MSG_UNHANDLED;
			
		case MSG_SMS_NOTIFY:
			IncomingCallSkipSave |= INCOMING_CALL_SKIP_SMS;	
			break;

		case MSG_MISSED_CALL_COUNT:
			IncomingCallSkipSave |= INCOMING_CALL_SKIP_MISSED_CALL;	
			break;	

		case MSG_E_MAIL_NOTIFY:
			IncomingCallSkipSave |= INCOMING_CALL_SKIP_EMAIL;	
			break;	

		case MSG_DAILY_SCHEDULE_NOTIFY:
			IncomingCallSkipSave |= INCOMING_CALL_SKIP_SCHEDULE;	
			break;	

		case MSG_SOCIAL_NOTIFY:
			IncomingCallSkipSave |= INCOMING_CALL_SKIP_SOCIAL;	
			break;

		default:
			return RET_MSG_UNHANDLED;
	}
	return RET_MSG_HANDLED;		
	
}

AncsHandler Email;
/*邮件主页面Handler*/
uint16 E_MailHomePageHandler(MsgType msg, int iParam, void *pContext)
{
	switch(msg)
	{
		case MSG_HANDLER_BEGIN:
			DifferentRemindId.E_MailHomePageHandlerId= iParam;
			if(gEMailCount>0)
				MotoRun(MOTO_EMIL);
			SetSystemSuspendTime(DISPLAY_TIME_10S_FLAG);
			break;
			
		case MSG_HANDLER_END:
			StopMotorRunTime();
			DifferentRemindId.E_MailHomePageHandlerId = 0;
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
		case MSG_TOUCH_UP_DOUBLE_SHORTKEY:		/*触摸 双击*/	
		case MSG_TOUCH_LONGKEY:					/*触摸 长按*/
			break;

		case MSG_RIGHTDOWN_SHORTKEY: 					/*右键 单击*/		
			UnloadHandler(DifferentRemindId.E_MailHomePageHandlerId);
			break;
			
		case MSG_TOUCH_UP_SHORTKEY:					/*触摸 单击*/ 
			if(GetAncsMsgLayOut(Ancs_Email,NULL))
			{
				UnloadHandler_WithoutDisplay(DifferentRemindId.E_MailHomePageHandlerId);
				LoadHandler(E_MailHandler, 0);	
			}
			break;
			
		case MSG_DISPLAY:
			LCD_MsgInterface(M_E_MAIL,gEMailCount);						/*更新主界面*/
			break;
			
		case MSG_E_MAIL_NOTIFY:
			if(!SWITCH_ANCS_EMIL)
				break;
			if(gEMailCount==0)
			{
				UnloadHandler(DifferentRemindId.E_MailHomePageHandlerId);
				break;
			}
			if(CheckpCurrentHandler(DifferentRemindId.E_MailHomePageHandlerId))
			{
				MotoRun(MOTO_EMIL);										/*震动2秒*/
				LCD_MsgInterface(M_COUNT_UPDATE,gEMailCount);			/*更新主界面的数字*/
			}
			else if((gEMailCount > 0)&&(BondSuccessfulState)&&(!SLEEP_MODE_FLAG))
			{
				UnloadHandler(DifferentRemindId.E_MailHomePageHandlerId);
				SendMsg(MSG_E_MAIL_NOTIFY);
			}
			break;

		case MSG_REMOVE_ALL_ANCS:
			UnloadHandler(DifferentRemindId.E_MailHomePageHandlerId);
			SendMsg(MSG_REMOVE_ALL_ANCS);
			break;

		default:
			return RET_MSG_UNHANDLED;			
	}
	return RET_MSG_HANDLED;	
}

uint16 E_MailHandler(MsgType msg, int iParam, void *pContext)
{
	switch(msg)
	{
		case MSG_HANDLER_BEGIN:
			DifferentRemindId.E_MailHandlerId = iParam;
			Email.View=PRE_VIEW;
			Email.PreViewIndex=0;
			Email.DetailViewIndex=0;
			SetSystemSuspendTime(DISPLAY_TIME_10S_FLAG);
			break;

		case MSG_HANDLER_END:
			{
				StopMotorRunTime();	
				uint8 rm=ForAncsHandlerEnd(Ancs_Email);
				if(rm!=0xff)
					gEMailCount=rm;			
				DifferentRemindId.E_MailHandlerId = 0;
			}
			break;

/*用户操作*/
		case MSG_LEFTDOWN_SHORTKEY:				/*左键 单击*/	
		case MSG_LEFTDOWN_DOUBLE_SHORTKEY:		/*左键 双击*/
		case MSG_LEFTDOWN_LONGKEY:           			/*左键 长按*/
		case MSG_RIGHTDOWN_DOUBLE_SHORTKEY:   	/*右键 双击*/	
		case MSG_RIGHTDOWN_LONGKEY:				/*右键 长按*/	
		case MSG_RIGHTDOWN_LONGKEY_UP:			/*右键 长按松开*/		
		case MSG_TOUCH_DOWN_UP_FLICK:				/*左*/ 
		case MSG_TOUCH_UP_DOUBLE_SHORTKEY:		/*触摸 双击*/	
			break;

		case MSG_TOUCH_LONGKEY:					/*触摸 长按*/
#if 0
			if(Email.View == PRE_VIEW)
			{
				ClearAllType = Clear_Email;
				SendMsg(MSG_CLEAR_ALL);			
			}
#endif
			break;	
	

			
		case MSG_RIGHTDOWN_SHORTKEY: 				/*右键 单击*/		
			switch(Email.View)
			{
				case PRE_VIEW:
					UnloadHandler(DifferentRemindId.E_MailHandlerId);
					break;
				case DETAIL_VIEW:
#if 0					
					if(ReadEnd==1)			/*需求点:看完消息后删除本消息*/
					{
						uint8 index=Email.PreViewIndex*3+Email.DetailViewIndex;
						if(index<5)
						{
							uint8 rm=DeleteAncsMsg(Ancs_Email,index);
							if(rm<=4)
								Email.PreViewIndex=0;
							if(gEMailCount>0)
								gEMailCount--;
						}						
					}
#endif					
					Email.View = PRE_VIEW;
					SendMsg(MSG_DISPLAY);				
					break;
			}
			break;

		case MSG_TOUCH_UP_SHORTKEY:				/*触摸 单击*/	
			switch(Email.View)
			{
				case PRE_VIEW:
					{
						uint8 Getlayout[ANCS_LAYOUT_MAX*2] ={0};
						memset(Getlayout,0x40,ANCS_LAYOUT_MAX*2);
						if(!GetAncsMsgLayOut(Ancs_Email,Getlayout))
						{
							UnloadHandler(DifferentRemindId.E_MailHandlerId);
							break;
						}
						uint8 TouchID = Get_Screen_Locaton(3,1);
						if(Getlayout[Email.PreViewIndex*3+TouchID]!=0x40)
						{
							if(Getlayout[Email.PreViewIndex*3+TouchID]>=0x20)
							{
								if(gEMailCount>0)
									gEMailCount--;	
							}						
							Email.DetailViewIndex = TouchID;
							Email.View = DETAIL_VIEW;
							ReadEnd=0;
							SendMsg(MSG_DISPLAY);		
						}
					}
					break;
				case DETAIL_VIEW:
					break;
			}
			break;
			
		case MSG_TOUCH_UP_FLICK:					/*上*/	
			switch(Email.View)
			{
				case PRE_VIEW:
					if(Email.PreViewIndex==0)
					{
						uint8 Getlayout[ANCS_LAYOUT_MAX*2] ={0};
						memset(Getlayout,0x40,ANCS_LAYOUT_MAX*2);
						GetAncsMsgLayOut(Ancs_Email,Getlayout);
						if(Getlayout[3]!=0x40)
						{
							Email.PreViewIndex=1;
							ReadAncsMsg_ToKl17(Ancs_Email,Email.PreViewIndex,0x40,3,De_INIT);
						}
					}
					break;
				case DETAIL_VIEW:
					if(!ReadAncsMsg_ToKl17(Ancs_Email,Email.PreViewIndex,Email.DetailViewIndex,3,De_NEXT))
						UnloadHandler(DifferentRemindId.E_MailHandlerId);
					break;
			}
			break;
			
		case MSG_TOUCH_DOWN_FLICK:		/*下*/	
			switch(Email.View)
			{
				case PRE_VIEW:
					if(Email.PreViewIndex==1)
					{
						uint8 Getlayout[ANCS_LAYOUT_MAX*2] ={0};
						memset(Getlayout,0x40,ANCS_LAYOUT_MAX*2);
						GetAncsMsgLayOut(Ancs_Email,Getlayout);
						if(Getlayout[0]!=0x40)
						{
							Email.PreViewIndex=0;
							ReadAncsMsg_ToKl17(Ancs_Email,Email.PreViewIndex,0x40,3,De_INIT);
						}
					}					
					break;
				case DETAIL_VIEW:
					if(!ReadAncsMsg_ToKl17(Ancs_Email,Email.PreViewIndex,Email.DetailViewIndex,3,De_PRE))
						UnloadHandler(DifferentRemindId.E_MailHandlerId);
					break;
			}
			break;
			
		case MSG_TOUCH_UP_DOWN_FLICK: 			/*右*/		
			if(Email.View==DETAIL_VIEW)
			{
				uint8 index=Email.PreViewIndex*3+Email.DetailViewIndex;
				if(index<5)
				{
					uint8 rm=DeleteAncsMsg(Ancs_Email,index);
					if(rm<=4)
						Email.PreViewIndex=0;
					Email.View = PRE_VIEW;
					SendMsg(MSG_DISPLAY);
				}
			}
			break;			


		case MSG_E_MAIL_NOTIFY:
			if(CheckpCurrentHandler(DifferentRemindId.E_MailHandlerId))
			{
				if(gEMailCount > 0)
				{
					MotoRun(MOTO_EMIL);									/*震动2秒*/		
					SetSystemSuspendTime(DISPLAY_TIME_10S_FLAG);
					if(Email.View==DETAIL_VIEW)
					{
						Email.PreViewIndex=0;
						Email.DetailViewIndex=0x30;
						if(gEMailCount>0)
							gEMailCount--;	
						ReadEnd=0;
					}
					SendMsg(MSG_DISPLAY);	
				}
			}			
			else if((gEMailCount > 0)&&(BondSuccessfulState)&&(!SLEEP_MODE_FLAG))
			{
				UnloadHandler(DifferentRemindId.E_MailHandlerId);
				SendMsg(MSG_E_MAIL_NOTIFY);
			}
			break;
			
		case MSG_DISPLAY:
			switch(Email.View)
			{
				case PRE_VIEW:
					if(!ReadAncsMsg_ToKl17(Ancs_Email,Email.PreViewIndex,0x40,3,De_INIT))
						UnloadHandler(DifferentRemindId.E_MailHandlerId);
					break;
					
				case DETAIL_VIEW:
					if(!ReadAncsMsg_ToKl17(Ancs_Email,Email.PreViewIndex,Email.DetailViewIndex,3,De_INIT))
						UnloadHandler(DifferentRemindId.E_MailHandlerId);
					else if(Email.DetailViewIndex==0x30)
					{
						Email.DetailViewIndex =0;
					}
					break;
			}
			break;

		case MSG_REMOVE_ALL_ANCS:
			UnloadHandler(DifferentRemindId.E_MailHandlerId);
			SendMsg(MSG_REMOVE_ALL_ANCS);
			break;


		default:
			return RET_MSG_UNHANDLED;
	}
	return RET_MSG_HANDLED;
}

AncsHandler DailySchedule;
/*行程日历主页面Handler*/
uint16 DailyScheduleHomePageHandler(MsgType msg, int iParam, void *pContext)
{
	switch(msg)
	{
		case MSG_HANDLER_BEGIN:
			DifferentRemindId.DailyScheduleHomePageHandlerId= iParam;
			if(gScheduleCount>0)
				MotoRun(MOTO_SCHEDULE);
			SetSystemSuspendTime(DISPLAY_TIME_10S_FLAG);
			break;
			
		case MSG_HANDLER_END:
			StopMotorRunTime();
			DifferentRemindId.DailyScheduleHomePageHandlerId = 0;
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
		case MSG_TOUCH_UP_DOUBLE_SHORTKEY:		/*触摸 双击*/	
		case MSG_TOUCH_LONGKEY:					/*触摸 长按*/
			break;

		case MSG_RIGHTDOWN_SHORTKEY: 					/*右键 单击*/		
			UnloadHandler(DifferentRemindId.DailyScheduleHomePageHandlerId);
			break;
			
		case MSG_TOUCH_UP_SHORTKEY:					/*触摸 单击*/ 
			if(GetAncsMsgLayOut(Ancs_Schedule,NULL))
			{
				UnloadHandler_WithoutDisplay(DifferentRemindId.DailyScheduleHomePageHandlerId);
				LoadHandler(DailyScheduleHandler, 0);	
			}
			break;
			
		case MSG_DISPLAY:
			LCD_MsgInterface(M_DAILY_SCHEDULE,gScheduleCount);		/*更新主界面*/
			break;
			
		case MSG_DAILY_SCHEDULE_NOTIFY:
			if(!SWITCH_ANCS_SCHEDULE)
				break;
			if(gScheduleCount==0)
			{
				UnloadHandler(DifferentRemindId.DailyScheduleHomePageHandlerId);
				break;
			}
			if(CheckpCurrentHandler(DifferentRemindId.DailyScheduleHomePageHandlerId))
			{
				MotoRun(MOTO_SCHEDULE);										/*震动2秒*/
				LCD_MsgInterface(M_COUNT_UPDATE,gScheduleCount);			/*更新主界面的数字*/
			}
			else if((gScheduleCount > 0)&&(BondSuccessfulState)&&(!SLEEP_MODE_FLAG))
			{
				UnloadHandler(DifferentRemindId.DailyScheduleHomePageHandlerId);
				SendMsg(MSG_DAILY_SCHEDULE_NOTIFY);
			}
			break;

		case MSG_REMOVE_ALL_ANCS:
			UnloadHandler(DifferentRemindId.DailyScheduleHomePageHandlerId);
			SendMsg(MSG_REMOVE_ALL_ANCS);
			break;

		default:
			return RET_MSG_UNHANDLED;			
	}
	return RET_MSG_HANDLED;	
}

uint16 DailyScheduleHandler(MsgType msg, int iParam, void *pContext)
{
	switch(msg)
	{
		case MSG_HANDLER_BEGIN:
			DifferentRemindId.DailyScheduleHandlerId = iParam;
			DailySchedule.View=PRE_VIEW;
			DailySchedule.PreViewIndex=0;
			DailySchedule.DetailViewIndex=0;
			SetSystemSuspendTime(DISPLAY_TIME_10S_FLAG);
			break;

		case MSG_HANDLER_END:
			{
				StopMotorRunTime();	
				uint8 rm=ForAncsHandlerEnd(Ancs_Schedule);
				if(rm!=0xff)
					gScheduleCount=rm;			
				DifferentRemindId.DailyScheduleHandlerId = 0;
			}
			break;

/*用户操作*/
		case MSG_LEFTDOWN_SHORTKEY:				/*左键 单击*/	
		case MSG_LEFTDOWN_DOUBLE_SHORTKEY:		/*左键 双击*/
		case MSG_LEFTDOWN_LONGKEY:           			/*左键 长按*/
		case MSG_RIGHTDOWN_DOUBLE_SHORTKEY:   	/*右键 双击*/	
		case MSG_RIGHTDOWN_LONGKEY:				/*右键 长按*/	
		case MSG_RIGHTDOWN_LONGKEY_UP:			/*右键 长按松开*/		
		case MSG_TOUCH_DOWN_UP_FLICK:				/*左*/ 
		case MSG_TOUCH_UP_DOUBLE_SHORTKEY:		/*触摸 双击*/	
			break;

		case MSG_TOUCH_LONGKEY:					/*触摸 长按*/
#if 0
			if(Email.View == PRE_VIEW)
			{
				ClearAllType = Clear_Email;
				SendMsg(MSG_CLEAR_ALL);			
			}
#endif
			break;	
	

			
		case MSG_RIGHTDOWN_SHORTKEY: 				/*右键 单击*/		
			switch(DailySchedule.View)
			{
				case PRE_VIEW:
					UnloadHandler(DifferentRemindId.DailyScheduleHandlerId);
					break;
				case DETAIL_VIEW:
#if 0					
					if(ReadEnd==1)			/*需求点:看完消息后删除本消息*/
					{
						uint8 index=Email.PreViewIndex*3+Email.DetailViewIndex;
						if(index<5)
						{
							uint8 rm=DeleteAncsMsg(Ancs_Email,index);
							if(rm<=4)
								Email.PreViewIndex=0;
							if(gScheduleCount>0)
								gScheduleCount--;
						}						
					}
#endif					
					DailySchedule.View = PRE_VIEW;
					SendMsg(MSG_DISPLAY);				
					break;
			}
			break;

		case MSG_TOUCH_UP_SHORTKEY:				/*触摸 单击*/	
			switch(DailySchedule.View)
			{
				case PRE_VIEW:
					{
						uint8 Getlayout[ANCS_LAYOUT_MAX*2] ={0};
						memset(Getlayout,0x40,ANCS_LAYOUT_MAX*2);
						if(!GetAncsMsgLayOut(Ancs_Schedule,Getlayout))
						{
							UnloadHandler(DifferentRemindId.DailyScheduleHandlerId);
							break;
						}
						uint8 TouchID = Get_Screen_Locaton(3,1);
						if(Getlayout[DailySchedule.PreViewIndex*3+TouchID]!=0x40)
						{
							if(Getlayout[DailySchedule.PreViewIndex*3+TouchID]>=0x20)
							{
								if(gScheduleCount>0)
									gScheduleCount--;	
							}						
							DailySchedule.DetailViewIndex = TouchID;
							DailySchedule.View = DETAIL_VIEW;
							ReadEnd=0;
							SendMsg(MSG_DISPLAY);		
						}
					}
					break;
				case DETAIL_VIEW:
					break;
			}
			break;
			
		case MSG_TOUCH_UP_FLICK:					/*上*/	
			switch(DailySchedule.View)
			{
				case PRE_VIEW:
					if(DailySchedule.PreViewIndex==0)
					{
						uint8 Getlayout[ANCS_LAYOUT_MAX*2] ={0};
						memset(Getlayout,0x40,ANCS_LAYOUT_MAX*2);
						GetAncsMsgLayOut(Ancs_Schedule,Getlayout);
						if(Getlayout[3]!=0x40)
						{
							DailySchedule.PreViewIndex=1;
							ReadAncsMsg_ToKl17(Ancs_Schedule,DailySchedule.PreViewIndex,0x40,3,De_INIT);
						}
					}
					break;
				case DETAIL_VIEW:
					if(!ReadAncsMsg_ToKl17(Ancs_Schedule,DailySchedule.PreViewIndex,DailySchedule.DetailViewIndex,3,De_NEXT))
						UnloadHandler(DifferentRemindId.DailyScheduleHandlerId);
					break;
			}
			break;
			
		case MSG_TOUCH_DOWN_FLICK:		/*下*/	
			switch(DailySchedule.View)
			{
				case PRE_VIEW:
					if(DailySchedule.PreViewIndex==1)
					{
						uint8 Getlayout[ANCS_LAYOUT_MAX*2] ={0};
						memset(Getlayout,0x40,ANCS_LAYOUT_MAX*2);
						GetAncsMsgLayOut(Ancs_Schedule,Getlayout);
						if(Getlayout[0]!=0x40)
						{
							DailySchedule.PreViewIndex=0;
							ReadAncsMsg_ToKl17(Ancs_Schedule,DailySchedule.PreViewIndex,0x40,3,De_INIT);
						}
					}					
					break;
				case DETAIL_VIEW:
					if(!ReadAncsMsg_ToKl17(Ancs_Schedule,DailySchedule.PreViewIndex,DailySchedule.DetailViewIndex,3,De_PRE))
						UnloadHandler(DifferentRemindId.DailyScheduleHandlerId);
					break;
			}
			break;
			
		case MSG_TOUCH_UP_DOWN_FLICK: 			/*右*/		
			if(DailySchedule.View==DETAIL_VIEW)
			{
				uint8 index=DailySchedule.PreViewIndex*3+DailySchedule.DetailViewIndex;
				if(index<5)
				{
					uint8 rm=DeleteAncsMsg(Ancs_Schedule,index);
					if(rm<=4)
						DailySchedule.PreViewIndex=0;
					DailySchedule.View = PRE_VIEW;
					SendMsg(MSG_DISPLAY);
				}
			}
			break;			


		case MSG_DAILY_SCHEDULE_NOTIFY:
			if(CheckpCurrentHandler(DifferentRemindId.DailyScheduleHandlerId))
			{
				if(gScheduleCount > 0)
				{
					MotoRun(MOTO_SCHEDULE);									/*震动2秒*/		
					SetSystemSuspendTime(DISPLAY_TIME_10S_FLAG);
					if(DailySchedule.View==DETAIL_VIEW)
					{
						DailySchedule.PreViewIndex=0;
						DailySchedule.DetailViewIndex=0x30;
						if(gScheduleCount>0)
							gScheduleCount--;	
						ReadEnd=0;
					}
					SendMsg(MSG_DISPLAY);	
				}
			}			
			else if((gScheduleCount > 0)&&(BondSuccessfulState)&&(!SLEEP_MODE_FLAG))
			{
				UnloadHandler(DifferentRemindId.DailyScheduleHandlerId);
				SendMsg(MSG_DAILY_SCHEDULE_NOTIFY);
			}
			break;
			
		case MSG_DISPLAY:
			switch(DailySchedule.View)
			{
				case PRE_VIEW:
					if(!ReadAncsMsg_ToKl17(Ancs_Schedule,DailySchedule.PreViewIndex,0x40,3,De_INIT))
						UnloadHandler(DifferentRemindId.DailyScheduleHandlerId);
					break;
					
				case DETAIL_VIEW:
					if(!ReadAncsMsg_ToKl17(Ancs_Schedule,DailySchedule.PreViewIndex,DailySchedule.DetailViewIndex,3,De_INIT))
						UnloadHandler(DifferentRemindId.DailyScheduleHandlerId);
					else if(DailySchedule.DetailViewIndex==0x30)
					{
						DailySchedule.DetailViewIndex =0;
					}
					break;
			}
			break;

		case MSG_REMOVE_ALL_ANCS:
			UnloadHandler(DifferentRemindId.DailyScheduleHandlerId);
			SendMsg(MSG_REMOVE_ALL_ANCS);
			break;


		default:
			return RET_MSG_UNHANDLED;
	}
	return RET_MSG_HANDLED;
}

uint8 SClassPage=0;
uint8 SClassID=0;
AncsHandler SocialClass;
ANCS_SAVE_TYPE SC=Ancs_Social_Facebook;
/*社交分类进入页面Handler*/
uint16 SocialClassHandler(MsgType msg, int iParam, void *pContext)
{
	switch(msg)
	{
		case MSG_HANDLER_BEGIN:
			DifferentRemindId.SocialClassHandlerId = iParam;
			SocialClass.View =PRE_VIEW;
			SocialClass.PreViewIndex =0;
			SocialClass.DetailViewIndex=0;
			SetSystemSuspendTime(DISPLAY_TIME_10S_FLAG);
			break;

		case MSG_HANDLER_END:
			{
				StopMotorRunTime();	
				uint8 rm=ForAncsHandlerEnd_SOCIAL(SC);
				if(rm!=0xff)
					SocialClassCount[(uint8)(SC-Ancs_Social_Facebook)] = rm;
				DifferentRemindId.SocialClassHandlerId = 0;
			}	
			break;
/*用户操作*/
		case MSG_LEFTDOWN_SHORTKEY:				/*左键 单击*/	
		case MSG_LEFTDOWN_DOUBLE_SHORTKEY:		/*左键 双击*/
		case MSG_LEFTDOWN_LONGKEY:           			/*左键 长按*/
		case MSG_RIGHTDOWN_DOUBLE_SHORTKEY:   	/*右键 双击*/	
		case MSG_RIGHTDOWN_LONGKEY:				/*右键 长按*/	
		case MSG_RIGHTDOWN_LONGKEY_UP:			/*右键 长按松开*/		
		case MSG_TOUCH_DOWN_UP_FLICK:				/*左*/ 
		case MSG_TOUCH_UP_DOUBLE_SHORTKEY:		/*触摸 双击*/	
		case MSG_TOUCH_LONGKEY:					/*触摸 长按*/
			break;
			
		case MSG_RIGHTDOWN_SHORTKEY: 				/*右键 单击*/		
			switch(SocialClass.View)
			{
				case PRE_VIEW:
					UnloadHandler(DifferentRemindId.SocialClassHandlerId);
					break;
				case DETAIL_VIEW:		
					SocialClass.View = PRE_VIEW;
					SendMsg(MSG_DISPLAY);				
					break;
			}
			break;
			
		case MSG_TOUCH_UP_SHORTKEY:				/*单击*/	
			switch(SocialClass.View)
			{
				case PRE_VIEW:
					{
						uint8 Getlayout[ANCS_LAYOUT_MAX*2] ={0};
						memset(Getlayout,0x40,ANCS_LAYOUT_MAX*2);
						if(!GetAncsMsgLayOut_SOCIAL(SC,Getlayout))
						{
							UnloadHandler(DifferentRemindId.SocialClassHandlerId);
							break;
						}
						uint8 TouchID = Get_Screen_Locaton(3,1);
						if(Getlayout[SocialClass.PreViewIndex*3+TouchID]!=0x40)
						{
							if(Getlayout[SocialClass.PreViewIndex*3+TouchID]>=0x20)
							{
								if(SocialClassCount[(uint8)(SC-Ancs_Social_Facebook)]>0)
									SocialClassCount[(uint8)(SC-Ancs_Social_Facebook)]--;	
							}
							SocialClass.DetailViewIndex = TouchID;
							SocialClass.View = DETAIL_VIEW;
							ReadEnd=0;
							SendMsg(MSG_DISPLAY);		
						}
					}
					break;
				case DETAIL_VIEW:
					break;
			}
			break;
			
		case MSG_TOUCH_UP_FLICK:					/*上*/	
			switch(SocialClass.View)
			{
				case PRE_VIEW:
					if(SocialClass.PreViewIndex==0)
					{
						uint8 Getlayout[ANCS_LAYOUT_MAX*2] ={0};
						memset(Getlayout,0x40,ANCS_LAYOUT_MAX*2);
						GetAncsMsgLayOut_SOCIAL(SC,Getlayout);
						if(Getlayout[3]!=0x40)
						{
							SocialClass.PreViewIndex=1;
							ReadAncsMsg_ToKl17_SOCIAL(SC,SocialClass.PreViewIndex,0x40,3,De_INIT);
						}
					}
					break;
				case DETAIL_VIEW:
					if(!ReadAncsMsg_ToKl17_SOCIAL(SC,SocialClass.PreViewIndex,SocialClass.DetailViewIndex,3,De_NEXT))
						UnloadHandler(DifferentRemindId.SocialClassHandlerId);
					break;
			}
			break;
		case MSG_TOUCH_DOWN_FLICK:		/*下*/	
			switch(SocialClass.View)
			{
				case PRE_VIEW:
					if(SocialClass.PreViewIndex==1)
					{
						uint8 Getlayout[ANCS_LAYOUT_MAX*2] ={0};
						memset(Getlayout,0x40,ANCS_LAYOUT_MAX*2);
						GetAncsMsgLayOut_SOCIAL(SC,Getlayout);
						if(Getlayout[0]!=0x40)
						{
							SocialClass.PreViewIndex=0;
							ReadAncsMsg_ToKl17_SOCIAL(SC,SocialClass.PreViewIndex,0x40,3,De_INIT);
						}
					}					
					break;
				case DETAIL_VIEW:
					if(!ReadAncsMsg_ToKl17_SOCIAL(SC,SocialClass.PreViewIndex,SocialClass.DetailViewIndex,3,De_PRE))
						UnloadHandler(DifferentRemindId.SocialClassHandlerId);
					break;
			}
			break;
			
		case MSG_TOUCH_UP_DOWN_FLICK: 			/*右*/		
			if(SocialClass.View==DETAIL_VIEW)
			{
				uint8 index=SocialClass.PreViewIndex*3+SocialClass.DetailViewIndex;
				if(index<5)
				{
					uint8 rm=DeleteAncsMsg_SOCIAL(SC,index);
					if(rm<=4)
						SocialClass.PreViewIndex=0;
					SocialClass.View = PRE_VIEW;					
					SendMsg(MSG_DISPLAY);
				}
			}
			break;	
			
		case MSG_SOCIAL_NOTIFY:
#if 0			
			SetSocialNotifEventFlag();
			if(CheckpCurrentHandler(DifferentRemindId.SocialClassHandlerId))
			{
				if(SocialClassCount[(uint8)(SC-Ancs_Social_Facebook)]  > 0)
				{
					MotoRun(MOTO_SOCIAL);									/*震动2秒*/		
					SetSystemSuspendTime(DISPLAY_TIME_10S_FLAG);
					if(SocialClass.View==DETAIL_VIEW)
					{
						SocialClass.PreViewIndex=0;
						SocialClass.DetailViewIndex=0x30;
						//if(SocialClassCount[(uint8)(SC-Ancs_Social_Facebook)] >0)
							//SocialClassCount[(uint8)(SC-Ancs_Social_Facebook)] --;		
						ReadEnd=0;
					}
					SendMsg(MSG_DISPLAY);	
				}
			}			
			else if((SocialClassCount[(uint8)(SC-Ancs_Social_Facebook)]  > 0)&&(BondSuccessfulState)&&(!SLEEP_MODE_FLAG))
			{
				UnloadHandler(DifferentRemindId.SocialClassHandlerId);
				SendMsg(MSG_SOCIAL_NOTIFY);
			}
#else
			UnloadHandler(DifferentRemindId.SocialClassHandlerId);
			SendMsg(MSG_SOCIAL_NOTIFY);
#endif
			break;
			
		case MSG_DISPLAY:
			switch(SocialClass.View)
			{
				case PRE_VIEW:
					if(!ReadAncsMsg_ToKl17_SOCIAL(SC,SocialClass.PreViewIndex,0x40,3,De_INIT))
						UnloadHandler(DifferentRemindId.SocialClassHandlerId);
					break;
					
				case DETAIL_VIEW:
					if(!ReadAncsMsg_ToKl17_SOCIAL(SC,SocialClass.PreViewIndex,SocialClass.DetailViewIndex,3,De_INIT))
						UnloadHandler(DifferentRemindId.SocialClassHandlerId);
					else if(SocialClass.DetailViewIndex==0x30)
					{
						SocialClass.DetailViewIndex =0;
					}
					break;
			}
			SetSocialNotifEventFlag();
			break;

		case MSG_REMOVE_ALL_ANCS:
			UnloadHandler(DifferentRemindId.SocialClassHandlerId);
			SendMsg(MSG_REMOVE_ALL_ANCS);
			break;

			
		default:
			return RET_MSG_UNHANDLED;
	}
	return RET_MSG_HANDLED;	
}


uint16 SocialClassHomeHandler(MsgType msg, int iParam, void *pContext)
{
	switch(msg)
	{
		case MSG_HANDLER_BEGIN:
			DifferentRemindId.SocialClassHomeHandlerId = iParam;
			SClassPage=0;
			SClassID=0;
			break;
			
		case MSG_HANDLER_END:
			DifferentRemindId.SocialClassHomeHandlerId = 0;
			break;
/*用户操作*/
		case MSG_LEFTDOWN_SHORTKEY:				/*左键 单击*/	
		case MSG_LEFTDOWN_DOUBLE_SHORTKEY:		/*左键 双击*/
		case MSG_LEFTDOWN_LONGKEY:           			/*左键 长按*/		
		case MSG_RIGHTDOWN_DOUBLE_SHORTKEY:   	/*右键 双击*/	
		case MSG_RIGHTDOWN_LONGKEY:				/*右键 长按*/	
		case MSG_RIGHTDOWN_LONGKEY_UP:			/*右键 长按松开*/		
		case MSG_TOUCH_DOWN_UP_FLICK:				/*左*/ 
		case MSG_TOUCH_UP_DOWN_FLICK: 			/*右*/
		case MSG_TOUCH_UP_DOUBLE_SHORTKEY:		/*触摸 双击*/	
		case MSG_TOUCH_LONGKEY:					/*触摸 长按*/
			break;
			
		case MSG_RIGHTDOWN_SHORTKEY: 				/*右键 单击*/
			UnloadHandler(DifferentRemindId.SocialClassHomeHandlerId);
			break;
			
		case MSG_TOUCH_UP_FLICK:					/*上*/
			if(SClassPage==0)
			{
				SClassPage=1;
				SendMsg(MSG_DISPLAY);
			}
			else if(SClassPage==1)
			{
				SClassPage=2;
				SendMsg(MSG_DISPLAY);
			}
			break;
		case MSG_TOUCH_DOWN_FLICK:				/*下*/	
			if(SClassPage==2)
			{
				SClassPage=1;
				SendMsg(MSG_DISPLAY);
			}
			else if(SClassPage==1)
			{
				SClassPage=0;
				SendMsg(MSG_DISPLAY);
			}
			break;
			
		case MSG_TOUCH_UP_SHORTKEY:				/*触摸 单击*/ 
			{
				uint8 Getlayout[ANCS_LAYOUT_MAX*2] ={0};	
				uint8 TouchID = Get_Screen_Locaton(3,1);
				SC = (ANCS_SAVE_TYPE)(Ancs_Social_Facebook+3*SClassPage+TouchID);
				if(GetAncsMsgLayOut_SOCIAL(SC,Getlayout))
					LoadHandler(SocialClassHandler, 0);	
			}
			break;
			
		case MSG_DISPLAY:
			ShowSocialClass(SClassPage);
			break;	
			
		default:
			return RET_MSG_UNHANDLED;			
	}
	return RET_MSG_HANDLED;
}


AncsHandler Social;
/*社交消息主页面Handler*/
uint16 SocialHomePageHandler(MsgType msg, int iParam, void *pContext)
{
	switch(msg)
	{
		case MSG_HANDLER_BEGIN:
			DifferentRemindId.SocialHomePageHandlerId = iParam;
			if(gSocialCount>0&&AncsMotoSkip==false)
				MotoRun(MOTO_SOCIAL);									/*震动2秒*/
			if(AncsMotoSkip)
				AncsMotoSkip=false;
			SetSystemSuspendTime(DISPLAY_TIME_10S_FLAG);
			break;

		case MSG_HANDLER_END:
			StopMotorRunTime();
			DifferentRemindId.SocialHomePageHandlerId = 0;
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
		case MSG_TOUCH_UP_DOUBLE_SHORTKEY:		/*触摸 双击*/	
		case MSG_TOUCH_LONGKEY:					/*触摸 长按*/
			break;
			
		case MSG_RIGHTDOWN_SHORTKEY: 					/*右键 单击*/		
			UnloadHandler(DifferentRemindId.SocialHomePageHandlerId);
			break;
			
		case MSG_TOUCH_UP_SHORTKEY:					/*触摸 单击*/ 
			if(GetAncsMsgLayOut(Ancs_Social,NULL))
			{
				UnloadHandler_WithoutDisplay(DifferentRemindId.SocialHomePageHandlerId);
				LoadHandler(SocialMessageHandler, 0);	
			}
			break;
			
		case MSG_DISPLAY:
			LCD_MsgInterface(M_SOCIAL,gSocialCount);					/*更新主界面*/
			break;
			
		case MSG_SOCIAL_NOTIFY:
			if(!SWITCH_ANCS_SOCIAL)
				break;			
			SetSocialNotifEventFlag();
			if(gSocialCount==0)
			{
				UnloadHandler(DifferentRemindId.SocialHomePageHandlerId);
				break;
			}
			if(CheckpCurrentHandler(DifferentRemindId.SocialHomePageHandlerId))
			{
				MotoRun(MOTO_SOCIAL);									/*震动2秒*/
				LCD_MsgInterface(M_COUNT_UPDATE,gSocialCount);			/*更新主界面的数字*/
			}
			else if((gSocialCount > 0)&&(BondSuccessfulState)&&(!SLEEP_MODE_FLAG))
			{
				UnloadHandler(DifferentRemindId.SocialHomePageHandlerId);
				SendMsg(MSG_SOCIAL_NOTIFY);
			}
			break;


		case MSG_REMOVE_ALL_ANCS:
			UnloadHandler(DifferentRemindId.SocialHomePageHandlerId);
			SendMsg(MSG_REMOVE_ALL_ANCS);
			break;
			
		default:
			return RET_MSG_UNHANDLED;
	}
	return RET_MSG_HANDLED;	
}

/*社交详细内容页面Handler*/
uint16 SocialMessageHandler(MsgType msg, int iParam, void *pContext)
{
	switch(msg)
	{
		case MSG_HANDLER_BEGIN:
			DifferentRemindId.SocialMessageHandlerId = iParam;
			Social.View =PRE_VIEW;
			Social.PreViewIndex =0;
			Social.DetailViewIndex=0;
			SetSystemSuspendTime(DISPLAY_TIME_10S_FLAG);
			break;

		case MSG_HANDLER_END:
			{
				StopMotorRunTime();	
				uint8 rm=ForAncsHandlerEnd(Ancs_Social);
				if(rm!=0xff)
					gSocialCount=rm;
				DifferentRemindId.SocialMessageHandlerId = 0;
			}	
			break;
/*用户操作*/
		case MSG_LEFTDOWN_SHORTKEY:				/*左键 单击*/	
		case MSG_LEFTDOWN_DOUBLE_SHORTKEY:		/*左键 双击*/
		case MSG_LEFTDOWN_LONGKEY:           			/*左键 长按*/
		case MSG_RIGHTDOWN_DOUBLE_SHORTKEY:   	/*右键 双击*/	
		case MSG_RIGHTDOWN_LONGKEY:				/*右键 长按*/	
		case MSG_RIGHTDOWN_LONGKEY_UP:			/*右键 长按松开*/		

		case MSG_TOUCH_DOWN_UP_FLICK:				/*左*/ 
		case MSG_TOUCH_UP_DOUBLE_SHORTKEY:		/*触摸 双击*/	
			break;

		case MSG_TOUCH_LONGKEY:					/*触摸 长按*/
#if 0
			if(Social.View == PRE_VIEW)
			{
				ClearAllType = Clear_Socail;
				SendMsg(MSG_CLEAR_ALL);		
			}
#endif
			break;
			
		case MSG_RIGHTDOWN_SHORTKEY: 				/*右键 单击*/		
			switch(Social.View)
			{
				case PRE_VIEW:
					UnloadHandler(DifferentRemindId.SocialMessageHandlerId);
					break;
				case DETAIL_VIEW:
#if 0					
					if(ReadEnd==1)			/*需求点:看完消息后删除本消息*/
					{
						uint8 index=Social.PreViewIndex*3+Social.DetailViewIndex;
						if(index<5)
						{
							uint8 rm=DeleteAncsMsg(Ancs_Social,index);
							if(rm<=4)
								Social.PreViewIndex=0;
							if(gSocialCount>0)
								gSocialCount--;
						}						
					}
#endif					
					Social.View = PRE_VIEW;
					SendMsg(MSG_DISPLAY);				
					break;
			}
			break;
			
		case MSG_TOUCH_UP_SHORTKEY:				/*单击*/	
			switch(Social.View)
			{
				case PRE_VIEW:
					{
						uint8 Getlayout[ANCS_LAYOUT_MAX*2] ={0};
						memset(Getlayout,0x40,ANCS_LAYOUT_MAX*2);
						if(!GetAncsMsgLayOut(Ancs_Social,Getlayout))
						{
							UnloadHandler(DifferentRemindId.SocialMessageHandlerId);
							break;
						}
						uint8 TouchID = Get_Screen_Locaton(3,1);
						if(Getlayout[Social.PreViewIndex*3+TouchID]!=0x40)
						{
							if(Getlayout[Social.PreViewIndex*3+TouchID]>=0x20)
							{
								if(gSocialCount>0)
									gSocialCount--;	
							}
							Social.DetailViewIndex = TouchID;
							Social.View = DETAIL_VIEW;
							ReadEnd=0;
							SendMsg(MSG_DISPLAY);		
						}
					}
					break;
				case DETAIL_VIEW:
					break;
			}
			break;
			
		case MSG_TOUCH_UP_FLICK:					/*上*/	
			switch(Social.View)
			{
				case PRE_VIEW:
					if(Social.PreViewIndex==0)
					{
						uint8 Getlayout[ANCS_LAYOUT_MAX*2] ={0};
						memset(Getlayout,0x40,ANCS_LAYOUT_MAX*2);
						GetAncsMsgLayOut(Ancs_Social,Getlayout);
						if(Getlayout[3]!=0x40)
						{
							Social.PreViewIndex=1;
							ReadAncsMsg_ToKl17(Ancs_Social,Social.PreViewIndex,0x40,3,De_INIT);
						}
					}
					break;
				case DETAIL_VIEW:
					if(!ReadAncsMsg_ToKl17(Ancs_Social,Social.PreViewIndex,Social.DetailViewIndex,3,De_NEXT))
						UnloadHandler(DifferentRemindId.SocialMessageHandlerId);
					break;
			}
			break;
		case MSG_TOUCH_DOWN_FLICK:		/*下*/	
			switch(Social.View)
			{
				case PRE_VIEW:
					if(Social.PreViewIndex==1)
					{
						uint8 Getlayout[ANCS_LAYOUT_MAX*2] ={0};
						memset(Getlayout,0x40,ANCS_LAYOUT_MAX*2);
						GetAncsMsgLayOut(Ancs_Social,Getlayout);
						if(Getlayout[0]!=0x40)
						{
							Social.PreViewIndex=0;
							ReadAncsMsg_ToKl17(Ancs_Social,Social.PreViewIndex,0x40,3,De_INIT);
						}
					}					
					break;
				case DETAIL_VIEW:
					if(!ReadAncsMsg_ToKl17(Ancs_Social,Social.PreViewIndex,Social.DetailViewIndex,3,De_PRE))
						UnloadHandler(DifferentRemindId.SocialMessageHandlerId);
					break;
			}
			break;
			
		case MSG_TOUCH_UP_DOWN_FLICK: 			/*右*/		
			if(Social.View==DETAIL_VIEW)
			{
				uint8 index=Social.PreViewIndex*3+Social.DetailViewIndex;
				if(index<5)
				{
					uint8 rm=DeleteAncsMsg(Ancs_Social,index);
					if(rm<=4)
						Social.PreViewIndex=0;
					Social.View = PRE_VIEW;
					SendMsg(MSG_DISPLAY);
				}
			}
			break;	
			
		case MSG_SOCIAL_NOTIFY:
			SetSocialNotifEventFlag();
			if(CheckpCurrentHandler(DifferentRemindId.SocialMessageHandlerId))
			{
				if(gSocialCount > 0)
				{
					MotoRun(MOTO_SOCIAL);									/*震动2秒*/		
					SetSystemSuspendTime(DISPLAY_TIME_10S_FLAG);
					if(Social.View==DETAIL_VIEW)
					{
						Social.PreViewIndex=0;
						Social.DetailViewIndex=0x30;
						if(gSocialCount>0)
							gSocialCount--;		
						ReadEnd=0;
					}
					SendMsg(MSG_DISPLAY);	
				}
			}			
			else if((gSocialCount > 0)&&(BondSuccessfulState)&&(!SLEEP_MODE_FLAG))
			{
				UnloadHandler(DifferentRemindId.SocialMessageHandlerId);
				SendMsg(MSG_SOCIAL_NOTIFY);
			}
			break;
			
		case MSG_DISPLAY:
			switch(Social.View)
			{
				case PRE_VIEW:
					if(!ReadAncsMsg_ToKl17(Ancs_Social,Social.PreViewIndex,0x40,3,De_INIT))
						UnloadHandler(DifferentRemindId.SocialMessageHandlerId);
					break;
					
				case DETAIL_VIEW:
					if(!ReadAncsMsg_ToKl17(Ancs_Social,Social.PreViewIndex,Social.DetailViewIndex,3,De_INIT))
						UnloadHandler(DifferentRemindId.SocialMessageHandlerId);
					else if(Social.DetailViewIndex==0x30)
					{
						Social.DetailViewIndex =0;
					}
					break;
			}
			SetSocialNotifEventFlag();
			break;


		case MSG_REMOVE_ALL_ANCS:
			UnloadHandler(DifferentRemindId.SocialMessageHandlerId);
			SendMsg(MSG_REMOVE_ALL_ANCS);
			break;

			
		default:
			return RET_MSG_UNHANDLED;
	}
	return RET_MSG_HANDLED;	
}




uint16 MemmoryFullHandler(MsgType msg, int iParam, void *pContext)
{
	switch(msg)
	{
		case MSG_HANDLER_BEGIN:
			DifferentRemindId.MemoryFullHandlerId = iParam;
			MotoRun(MOTO_SIT);
			SetSystemSuspendTime(DISPLAY_TIME_10S_FLAG);
			MemFullCount=0;		
			break;

		case MSG_HANDLER_END:
			DifferentRemindId.MemoryFullHandlerId = 0;
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
		case MSG_TOUCH_UP_SHORTKEY:				/*触摸 单击*/  
		case MSG_TOUCH_UP_DOUBLE_SHORTKEY:		/*触摸 双击*/	
		case MSG_TOUCH_LONGKEY:					/*触摸 长按*/
			break;


		case MSG_RIGHTDOWN_SHORTKEY: 				/*右键 单击*/				
			StopMotorRunTime();
			UnloadHandler(DifferentRemindId.MemoryFullHandlerId);
			break;	 

		case MSG_MEMORY_CHANGE:
			if(CheckpCurrentHandler(DifferentRemindId.MemoryFullHandlerId))
			{
				if(!MEM_FULL_FLAG)
				{
					StopMotorRunTime();
					UnloadHandler(DifferentRemindId.MemoryFullHandlerId);
					break;
				}
				else
				{
					MotoRun(MOTO_SIT);
					SetSystemSuspendTime(DISPLAY_TIME_10S_FLAG);
				}
			}
			else
			{
				UnloadHandler(DifferentRemindId.MemoryFullHandlerId);
				SendMsg(msg);
				break;
			}
		case MSG_DISPLAY:
			ClearScreen();
			ShowMemFull();
			break;

		default:
			return RET_MSG_UNHANDLED;
	}
	return RET_MSG_HANDLED;
}


uint16 BtBleDisconnectHandler(MsgType msg, int iParam, void *pContext)
{
	switch(msg)
	{
		case MSG_HANDLER_BEGIN:
			DifferentRemindId.BtBleDisconnectHandlerId = iParam;
			BC5_SetTonesCmd(BC5_DISCONNECTED_TONE);     
			MotoRun(MOTO_ANTI_LOST);
			SetSystemSuspendTime(DISPLAY_TIME_10S_FLAG);
			StartDisconnectQuitEvent(12000);
			return RET_MSG_HANDLED;

		case MSG_HANDLER_END:
			DifferentRemindId.BtBleDisconnectHandlerId = 0;
			return RET_MSG_HANDLED;

		case MSG_BC5_BT_CHANGE: 
			if(BC5_State != BC5_BT_CONNECT_OK)
			{
				break;
			}
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
		case MSG_TOUCH_UP_SHORTKEY:				/*触摸 单击*/  
		case MSG_TOUCH_UP_DOUBLE_SHORTKEY:		/*触摸 双击*/	
		case MSG_TOUCH_LONGKEY:					/*触摸 长按*/
			return RET_MSG_HANDLED;        

		case MSG_RIGHTDOWN_SHORTKEY: 				/*右键 单击*/				
		case MSG_BLE_DISCONNECT:
			StopDisconnectQuitEvent();
		case MSG_DISCONNECT_QUIT_STATE:
			StopMotorRunTime();
			UnloadHandler(DifferentRemindId.BtBleDisconnectHandlerId);
			return RET_MSG_HANDLED;

		case MSG_BC5_DISCONNECT:
			if(CheckpCurrentHandler(DifferentRemindId.BtBleDisconnectHandlerId))
			{
				StopDisconnectQuitEvent();
				MotoRun(MOTO_ANTI_LOST);
				SetSystemSuspendTime(DISPLAY_TIME_10S_FLAG);
				StartDisconnectQuitEvent(8000);
			}
			else
			{
				UnloadHandler(DifferentRemindId.BtBleDisconnectHandlerId);
				SendMsg(msg); 	
				return RET_MSG_HANDLED;
			}
		case MSG_DISPLAY:
			DisplayUnicode_Stop();
			ClearScreen();
			ShowBtDisconnected();
			return RET_MSG_HANDLED;		

		default:
			break;
	}
	return RET_MSG_UNHANDLED;
}


static void UpdateRemindUI(uint8 ui_index)
{
	switch(ui_index)
	{
		case REMIND_MEDICINE:
		    ShowMedicine();
		    break;
		
		case REMIND_EAT:
		    ShowEat();
		    break;
		
		case REMIND_MOVING:
		    ShowSports();
		    break;

		case REMIND_WAKEUP:
		    ShowWakeup();
		    break;

		case REMIND_SLEEP:
		    ShowSleeping();
		    break;
			
		case REMIND_MEETING:
		    ShowMeeting();
		    break;
			
		case REMIND_USER:
		    ShowUserRemind();
		    break;
			
		default:
		    break;
	}
    
}

uint16 RemindHandler(MsgType msg, int iParam, void *pContext)
{
	switch(msg)
	{
		case MSG_HANDLER_BEGIN:
			DifferentRemindId.RemindHandler = iParam;
			MotoRun(MOTO_REMIND);
			osal_start_timerEx(GetAppTaskId(), MSG_REMIND_HANDLER_TIMEOUT, 10000);	
			return RET_MSG_HANDLED;

		case MSG_HANDLER_END:
			DifferentRemindId.RemindHandler = 0;
			DisplayUnicode_Stop();			
			return RET_MSG_HANDLED;
				
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
		case MSG_TOUCH_UP_SHORTKEY:				/*触摸 单击*/  
		case MSG_TOUCH_UP_DOUBLE_SHORTKEY:		/*触摸 双击*/	
		case MSG_TOUCH_LONGKEY:					/*触摸 长按*/
			return RET_MSG_HANDLED;

		case MSG_FORCE_BACK_HOME:
			StopSecondRemind();
			SendMsg(MSG_FORCE_BACK_HOME);
			break;
		case MSG_RIGHTDOWN_SHORTKEY: 				/*右键 单击*/
			StopMotorRunTime();
			StopSecondRemind();
		case MSG_REMIND_HANDLER_TIMEOUT:	 	
			UnloadHandler(DifferentRemindId.RemindHandler);
			return RET_MSG_HANDLED;

		case MSG_MEMORY_CHANGE:    
			if(MEM_FULL_FLAG)
			{
				DisplayUnicode_Stop();
			}
			return RET_MSG_UNHANDLED;

		case MSG_SMS_NOTIFY:
			if(SWITCH_ANCS_SMS) 
			{
				DisplayUnicode_Stop();
			}
			return RET_MSG_UNHANDLED;

                case MSG_INCOMING_CALL_NOTIFY:
                        if(SWITCH_ANCS_INCOMING_CALL) 
                        {
                                DisplayUnicode_Stop();
                        }
                        return RET_MSG_UNHANDLED;

		case MSG_MISSED_CALL_COUNT:
			if(SWITCH_ANCS_MISSED_CALL) 
			{
				DisplayUnicode_Stop();
			}
			return RET_MSG_UNHANDLED;

		case MSG_E_MAIL_NOTIFY:
			if(SWITCH_ANCS_EMIL) 
			{
				DisplayUnicode_Stop();
			}
			return RET_MSG_UNHANDLED;

		case MSG_DAILY_SCHEDULE_NOTIFY:
			if(SWITCH_ANCS_SCHEDULE) 
			{
				DisplayUnicode_Stop();
			}
			return RET_MSG_UNHANDLED;

		case MSG_SOCIAL_NOTIFY:
			if(SWITCH_ANCS_SOCIAL) 
			{
				DisplayUnicode_Stop();
			}
			return RET_MSG_UNHANDLED;
            
                case MSG_SLEEP_MODE_START:
                case MSG_SLEEP_MODE_EXIT:

		case MSG_SYSTEM_SUSPEND:
			DisplayUnicode_Stop();
			return RET_MSG_UNHANDLED;

		case MSG_USER_REMIND:
			if(CheckpCurrentHandler(DifferentRemindId.RemindHandler))
			{
				MotoRun(MOTO_REMIND);
				SetSystemSuspendTime(DISPLAY_TIME_10S_FLAG);
			}
			else
			{
				UnloadHandler(DifferentRemindId.RemindHandler);
				SendMsg(msg);
				return RET_MSG_HANDLED;
			}
			
		case MSG_DISPLAY:
			SetSystemSuspendTime(DISPLAY_TIME_10S_FLAG);
			if(!DISPLAY_GOTO_SLEEP_FLAG){
				REMIND_DATA_T data;
				DisplayUnicode_Stop();
				ClearScreen();
				GetRemindsEvent(&data);
				UpdateRemindUI(data.Id);
			}
			return RET_MSG_HANDLED;

		case MSG_FONT_DISPLAY_NEXT_LETTER:
			if(CheckpCurrentHandler(DifferentRemindId.RemindHandler))
				DisplayLineUnicode();	
			return RET_MSG_HANDLED;  
		default:
			break;
	}
	return RET_MSG_UNHANDLED;
}

uint16 HrRemindHandler(MsgType msg, int iParam, void *pContext)
{
	
	switch(msg)
	{
		case MSG_HANDLER_BEGIN:
			DifferentRemindId.HrRemindHandlerId = iParam;
			MotoRun(MOTO_REMIND);
			SetSystemSuspendTime(DISPLAY_TIME_10S_FLAG);
			break;

		case MSG_HANDLER_END:
			DifferentRemindId.HrRemindHandlerId = 0;
			break;

/*用户操作*/
		case MSG_LEFTDOWN_SHORTKEY:				/*左键 单击*/	
		case MSG_LEFTDOWN_DOUBLE_SHORTKEY:		/*左键 双击*/
		case MSG_LEFTDOWN_LONGKEY:           	/*左键 长按*/
		case MSG_RIGHTDOWN_DOUBLE_SHORTKEY:   	/*右键 双击*/	
		case MSG_RIGHTDOWN_LONGKEY:				/*右键 长按*/	
		case MSG_RIGHTDOWN_LONGKEY_UP:			/*右键 长按松开*/		
		case MSG_TOUCH_UP_FLICK:					/*上*/
		case MSG_TOUCH_DOWN_FLICK:				/*下*/
		case MSG_TOUCH_DOWN_UP_FLICK:				/*左*/ 
		case MSG_TOUCH_UP_DOWN_FLICK: 			/*右*/
		case MSG_TOUCH_UP_SHORTKEY:				/*触摸 单击*/  
		case MSG_TOUCH_UP_DOUBLE_SHORTKEY:		/*触摸 双击*/	
		case MSG_TOUCH_LONGKEY:					/*触摸 长按*/
			break;
	
		case MSG_RIGHTDOWN_SHORTKEY:		/*右键 单击*/ 
			isHrAlertChecked = true;
			StopMotorRunTime();
			UnloadHandler(DifferentRemindId.HrRemindHandlerId);
			break;
			//return RET_MSG_HANDLED;
		 case MSG_HR_UPPER_REMIND:
		case MSG_HR_LOWER_REMIND:
            if(CheckpCurrentHandler(DifferentRemindId.HrRemindHandlerId))
            {
				MotoRun(MOTO_REMIND);
            	if(!DISPLAY_GOTO_SLEEP_FLAG)
            	{
					//memcpy(&DispalyHrAlarmData, &DispalyHrData, sizeof(UiHRStruct));
					//ShowHRAlarm_WithoutClean(DispalyHrAlarmData,bLowHR);
					SendMsg(MSG_DISPLAY);
				}
				
            }
            else
            {
                UnloadHandler(DifferentRemindId.HrRemindHandlerId);
                SendMsg(msg);
            }
            return RET_MSG_HANDLED;
		case MSG_DATE_TIME_FORMAT_UPDATE:	
		case MSG_DISPLAY:
			ShowHRAlarm(DispalyHrAlarmData,bLowHR);
			break;

		default:
			return RET_MSG_UNHANDLED;
	}
	return RET_MSG_HANDLED;
	
}

uint16 SitRemindHandler(MsgType msg, int iParam, void *pContext)
{
	switch(msg)
	{
		case MSG_HANDLER_BEGIN:
			DifferentRemindId.SitRemindHandlerId = iParam;
			MotoRun(MOTO_SIT);
			SetSystemSuspendTime(DISPLAY_TIME_10S_FLAG);
			break;	

		case MSG_HANDLER_END:
			DifferentRemindId.SitRemindHandlerId = 0;
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
		case MSG_TOUCH_UP_SHORTKEY:				/*触摸 单击*/  
		case MSG_TOUCH_UP_DOUBLE_SHORTKEY:		/*触摸 双击*/	
		case MSG_TOUCH_LONGKEY:					/*触摸 长按*/
			break;		

		case MSG_RIGHTDOWN_SHORTKEY: 				/*右键 单击*/			
			StopMotorRunTime();
			UnloadHandler(DifferentRemindId.SitRemindHandlerId);
			break;		 

		case MSG_SIT_REMIND:
			if(CheckpCurrentHandler(DifferentRemindId.SitRemindHandlerId))
			{
				MotoRun(MOTO_SIT);
				SetSystemSuspendTime(DISPLAY_TIME_10S_FLAG);
			}
			else
			{
				UnloadHandler(DifferentRemindId.SitRemindHandlerId);
				SendMsg(msg);
				break;	
			}
		case MSG_DISPLAY:
			ShowMoving();
			break;	

		default:
			return RET_MSG_UNHANDLED;
	}
	return RET_MSG_HANDLED;
}

/*蓝牙4.0配对Handler*/
bool BleBondChooseSkipKey=false;
uint16 BleBondChooseHandler(MsgType msg, int iParam, void *pContext)
{
	static bool BleBondOkFlag;
	switch(msg)
	{
		case MSG_HANDLER_BEGIN:
			BleBondOkFlag=false;
			BleBondChooseSkipKey = false;
			BondSuccessfulState = false;
			DifferentRemindId.BleBondChooseHandlerId = iParam;
			SetSystemSuspendTime(20);
			osal_start_timerEx(GetAppTaskId(), MSG_BLE_BOND_CHOOSE_TIMEOUT, 10000);
			break;

		case MSG_HANDLER_END:
			DifferentRemindId.BleBondChooseHandlerId = 0;
			break;	
				
		case MSG_BC5_DISCONNECT:
		case MSG_SMS_NOTIFY:
		case MSG_SOCIAL_NOTIFY:
		case MSG_E_MAIL_NOTIFY:
		case MSG_INCOMING_CALL_NOTIFY:
		case MSG_MISSED_CALL_COUNT:
		case MSG_DAILY_SCHEDULE_NOTIFY:
			break;			
/*用户操作*/		
		case MSG_LEFTDOWN_DOUBLE_SHORTKEY:		/*左键 双击*/
		case MSG_LEFTDOWN_LONGKEY:           			/*左键 长按*/
		case MSG_RIGHTDOWN_DOUBLE_SHORTKEY:   	/*右键 双击*/	
		case MSG_RIGHTDOWN_LONGKEY:				/*右键 长按*/	
		case MSG_RIGHTDOWN_LONGKEY_UP:			/*右键 长按松开*/		
		case MSG_TOUCH_UP_FLICK:					/*上*/
		case MSG_TOUCH_DOWN_FLICK:				/*下*/
		case MSG_TOUCH_DOWN_UP_FLICK:				/*左*/ 
		case MSG_TOUCH_UP_DOWN_FLICK: 			/*右*/
		case MSG_TOUCH_LONGKEY:					/*触摸 长按*/
		case MSG_TOUCH_UP_DOUBLE_SHORTKEY:		/*触摸 双击*/				
			break;
			
		case MSG_TOUCH_UP_SHORTKEY:				/*触摸 单击*/ 
			{
				uint8 TouchID = Get_Screen_Locaton(3,3);
				if(TouchID==6)
				{
					if ((!BleBondOkFlag)&&(BleBondChooseSkipKey==false))
					{	
						BleBondChooseSkipKey=true;
						BleBondOkFlag = true;
						BleBondRequiredOk = 0;
						BondSuccessfulState = true;
						SendMsg(MSG_DISPLAY);
						SendSettingOkRspCmd();      //设置消息返回
						osal_stop_timerEx(GetAppTaskId(), MSG_BLE_BOND_CHOOSE_TIMEOUT);
						osal_start_timerEx(GetAppTaskId(), MSG_BLE_BOND_CHOOSE_QUIT, 3000);
						osal_start_timerEx(GetAppTaskId(), MSG_ANCS_REQUIRE_DISPLAY, 5000);
					}
				}
				else if(TouchID==8)
				{			
					if(BleBondChooseSkipKey==false)
					{
						BleBondChooseSkipKey=true;
						BleBondRequiredOk = 2;
						osal_stop_timerEx(GetAppTaskId(), MSG_BLE_BOND_CHOOSE_TIMEOUT);
						SendSettingFailRspCmd();     //设置消息返回
						BleBondChooseSkipKey=true;
						if(CheckpCurrentHandler(DifferentRemindId.BleBondChooseHandlerId))
							ShowBleBondFail();
						SetSystemSuspendTime(15);
						osal_start_timerEx(GetAppTaskId(), MSG_BLE_BOND_CHOOSE_QUIT, 10000);						
					}		
				}
			}	
			break;
			
		case MSG_LEFTDOWN_SHORTKEY:				/*左键 单击*/
			if ((!BleBondOkFlag)&&(BleBondChooseSkipKey==false))
			{	
				BleBondChooseSkipKey=true;
				BleBondOkFlag = true;
				BleBondRequiredOk = 0;
				BondSuccessfulState = true;
				SendMsg(MSG_DISPLAY);
				SendSettingOkRspCmd();      //设置消息返回
				osal_stop_timerEx(GetAppTaskId(), MSG_BLE_BOND_CHOOSE_TIMEOUT);
				osal_start_timerEx(GetAppTaskId(), MSG_BLE_BOND_CHOOSE_QUIT, 3000);
				osal_start_timerEx(GetAppTaskId(), MSG_ANCS_REQUIRE_DISPLAY, 5000);
			}
			break;
			
		case MSG_RIGHTDOWN_SHORTKEY: 				/*右键 单击*/
			if(BleBondChooseSkipKey==false)
			{
				BleBondChooseSkipKey=true;
				BleBondRequiredOk = 2;
				osal_stop_timerEx(GetAppTaskId(), MSG_BLE_BOND_CHOOSE_TIMEOUT);
			}
			else
				break;
		case MSG_BLE_BOND_CHOOSE_TIMEOUT:
			SendSettingFailRspCmd();     //设置消息返回
			BleBondChooseSkipKey=true;
			if(CheckpCurrentHandler(DifferentRemindId.BleBondChooseHandlerId))
				ShowBleBondFail();
			SetSystemSuspendTime(15);
			osal_start_timerEx(GetAppTaskId(), MSG_BLE_BOND_CHOOSE_QUIT, 10000);
			break;
			
		case MSG_BLE_BOND_CHOOSE_QUIT:
			if(!BleBondOkFlag) 
			{
				SendMsg(MSG_ANCS_CANCEL_DISPLAY);
			}
			UnloadHandler(DifferentRemindId.BleBondChooseHandlerId);
			if(BleBondOkFlag)				
				SendMsg(MSG_BACKONMAINMENUE_BOND);
			break;

		case MSG_BLE_BOND_CHOOSE:
			osal_stop_timerEx(GetAppTaskId(), MSG_BLE_BOND_CHOOSE_TIMEOUT);
			osal_stop_timerEx(GetAppTaskId(), MSG_BLE_BOND_CHOOSE_QUIT);
			UnloadHandler(DifferentRemindId.BleBondChooseHandlerId);
			SendMsg(MSG_BLE_BOND_CHOOSE);
			break;

		case MSG_DATE_TIME_FORMAT_UPDATE:
		case MSG_DISPLAY:
			ClearScreen();
			if(BleBondOkFlag)
			{
				ShowBleBondOk();
				MotoRun(MOTO_DEFAULT);
			}
			else
			{
				ShowBleBondQuery();
			}
			break;
			
		case MSG_FORCE_BACK_HOME:
			break;

		default:
			return RET_MSG_UNHANDLED;
	}
	return RET_MSG_HANDLED;
}

uint16 UpdateFontLibHandler(MsgType msg, int iParam, void *pContext)
{
	switch(msg)
	{
		case MSG_HANDLER_BEGIN:	
		{
			uint8 Wid[WATCH_ID_LEN]={0x00};	
			uint8 UserFlag[USER_FLAG_MAX_LEN]={0x00};	
			DifferentRemindId.UpdateFontLibHandlerId = iParam;
			FLP_TurnOffBle();	
			ReadWatchId(Wid);
			READ_USER_FLAG(UserFlag,0,USER_FLAG_MAX_LEN);
			SpiFlash_ChipErase();
			WriteWatchId(Wid);
			SpiFlash_Write(USER_FLAG_BASE_ADDR,UserFlag,USER_FLAG_MAX_LEN);
			UPDATE_FONTLIB_SEND_C = true;
			SpiFlash_Addr = 0x00000000UL;	
			UartRevValueOffset =0;
			CrcTotalValue =0;
			osal_start_timerEx(GetAppTaskId(), MSG_UPDATE_FONT_LIB_SEND_CMD, 100);
			osal_start_timerEx(GetAppTaskId(), MSG_UPDATE_FONT_LIB_FAIL, 30000);
			osal_set_event(GetHrTaskId(), HR_CLOSE_EVENT);
		}	
		break;

		case MSG_HANDLER_END:
			Free_NordicFlash();
			FLP_TurnOnBle();		
			UPDATE_FONTLIB =false;	
			UPDATE_FONTLIB_SEND_C = false;
			UartRevValueOffset =0;
			DifferentRemindId.UpdateFontLibHandlerId = 0;
			break;

		case MSG_UPDATE_FONT_LIB_SUCCESS:	
			//showUpgradeSuccess();
			break;

		case MSG_UPDATE_FONT_LIB_FAIL:	
			Free_NordicFlash();
			showUpgradeFail();
			osal_start_timerEx(GetAppTaskId(), MSG_UPDATE_FONT_LIB_TIMEOUT, 2000);
			break;

		case MSG_UPDATE_FONT_LIB_TIMEOUT:
			UnloadHandler(DifferentRemindId.UpdateFontLibHandlerId);		
			break;

		case MSG_UPDATE_FONT_LIB_SEND_CMD:
		{
			if(UPDATE_FONTLIB_SEND_C)
			{
				UART_Send("C",1);	
				osal_start_timerEx(GetAppTaskId(), MSG_UPDATE_FONT_LIB_SEND_CMD, 2000);	
			}
		}
			break;

		case MSG_DISPLAY:
			showUpgrade();
			Force_NordicFlash();
			break;
			
		 case MSG_CHARGE_STOP:
		      Powerstate.chargeinterface = false;
	             Powerstate.chargepowerstate = false;
		      break;
			  
		default:
			break;
	}
	return RET_MSG_HANDLED;
	
}


uint32 TpIndex=0;
uint16 TpTestHandler(MsgType msg, int iParam, void *pContext)
{
	switch(msg)
	{
		case MSG_HANDLER_BEGIN:	
			DifferentRemindId.TpTestHandlerId = iParam;
			if(TpTest_XY==3)
				TpIndex = 0x000001FFUL;
			else if(TpTest_XY==4)
				TpIndex = 0x0000FFFFUL;
			else if(TpTest_XY==5)
				TpIndex = 0x01FFFFFFUL;
			break;

		case MSG_HANDLER_END:		
			TP_TEST =false;	
			DifferentRemindId.TpTestHandlerId = 0;
			break;
			
		case MSG_TEST_TP:
			if(TpTest_XY==0)
			{
				UnloadHandler(DifferentRemindId.TpTestHandlerId);	
				break;
			}
			else if(TpTest_XY==0xFF)
			{
				TpTest_XY =0;
				ShowTpTest(0xFF,0xFF);
				osal_start_timerEx(GetAppTaskId(), MSG_TEST_TP, 3000);
				break;
			}	
			else if(TpTest_XY==3)
				TpIndex = 0x000001FFUL;
			else if(TpTest_XY==4)
				TpIndex = 0x0000FFFFUL;
			else if(TpTest_XY==5)
				TpIndex = 0x01FFFFFFUL;
			osal_stop_timerEx(GetAppTaskId(), MSG_TEST_TP);
		case MSG_DISPLAY:
			ShowTpTest(TpTest_XY,0xFF);
			break;
			
		case MSG_TOUCH_UP_SHORTKEY:			/*单击*/	
			{
				uint8 TouchID = Get_Screen_Locaton(TpTest_XY,TpTest_XY);
				ShowTpTest(TpTest_XY,TouchID);
				TpIndex&=~(0x00000001UL<<TouchID);
				if(TpIndex==0)
				{
					TpTest_XY =0xFF;
					osal_start_timerEx(GetAppTaskId(), MSG_TEST_TP, 2000);	
				}		
			}
			break;
			
/*用户操作*/
		case MSG_LEFTDOWN_SHORTKEY:				/*左键 单击*/	
		case MSG_LEFTDOWN_DOUBLE_SHORTKEY:		/*左键 双击*/
		case MSG_LEFTDOWN_LONGKEY:           			/*左键 长按*/
		case MSG_RIGHTDOWN_SHORTKEY: 				/*右键 单击*/	
		case MSG_RIGHTDOWN_DOUBLE_SHORTKEY:   	/*右键 双击*/	
		case MSG_RIGHTDOWN_LONGKEY:				/*右键 长按*/	
		case MSG_RIGHTDOWN_LONGKEY_UP:			/*右键 长按松开*/		
		case MSG_TOUCH_UP_FLICK:					/*上*/
		case MSG_TOUCH_DOWN_FLICK:				/*下*/
		case MSG_TOUCH_DOWN_UP_FLICK:				/*左*/ 
		case MSG_TOUCH_UP_DOWN_FLICK: 			/*右*/
		case MSG_TOUCH_UP_DOUBLE_SHORTKEY:		/*触摸 双击*/	
		case MSG_TOUCH_LONGKEY:					/*触摸 长按*/
			break;

			
		 case MSG_CHARGE_STOP:
		      Powerstate.chargeinterface = false;
	             Powerstate.chargepowerstate = false;
		      break;
			  
		default:
			break;
	}
	return RET_MSG_HANDLED;
}

/*蓝牙开关Handler*/
uint16 BluetoothHandler(MsgType msg, int iParam, void *pContext)
{
	uint8 TouchID;
	switch(msg)
	{
		case MSG_HANDLER_BEGIN:
			DifferentRemindId.BluetoothHandlerId = iParam;	
			SetSystemSuspendTime(DISPLAY_TIME_10S_FLAG);
			break;

		case MSG_HANDLER_END:
			DifferentRemindId.BluetoothHandlerId = 0;
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
		case MSG_TOUCH_UP_DOUBLE_SHORTKEY:		/*触摸 双击*/	
		case MSG_TOUCH_LONGKEY:					/*触摸 长按*/
			break;

		case MSG_TOUCH_UP_SHORTKEY:				/*触摸 单击*/  
			TouchID = Get_Screen_Locaton(4,4);
			if ((TouchID == 0) || (TouchID == 1)|| (TouchID == 2)|| (TouchID == 3))
			{					
				if (shortcut_info.airplane_mode == 1)
				{
					shortcut_info.airplane_mode = 0;
					ShowBluetoothOnApps();	
				}
				else
				{
					shortcut_info.airplane_mode = 1;
					ShowBluetoothOffApps();	
				}
				BleDisable = 1;
				ShowShortCut(0,1);
			}
			break;

					
		case MSG_RIGHTDOWN_SHORTKEY: 				/*右键 单击*/					
			UnloadHandler(DifferentRemindId.BluetoothHandlerId);	
			break;
			
			
		case MSG_DISPLAY:
			if (shortcut_info.airplane_mode)
				ShowBluetoothOffApps();			
			else
				ShowBluetoothOnApps();	
			break;
					
		default:
			return RET_MSG_UNHANDLED;
	}
	return RET_MSG_HANDLED;
}

uint16 VirateHandler(MsgType msg, int iParam, void *pContext)
{
	uint8 TouchID;
	switch(msg)
	{
		case MSG_HANDLER_BEGIN:
			DifferentRemindId.VirateHandlerId = iParam;	
			SetSystemSuspendTime(DISPLAY_TIME_10S_FLAG);
			break;

		case MSG_HANDLER_END:
			DifferentRemindId.VirateHandlerId = 0;
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
		case MSG_TOUCH_UP_DOUBLE_SHORTKEY:		/*触摸 双击*/	
		case MSG_TOUCH_LONGKEY:					/*触摸 长按*/
			break;

		case MSG_TOUCH_UP_SHORTKEY:				/*触摸 单击*/  
			TouchID = Get_Screen_Locaton(4,1);
			if (TouchID == 0)
			{			
				shortcut_info.vibration_level = MOTOR_LEVEL_OFF;
			}
			else if (TouchID == 1)
			{			
				shortcut_info.vibration_level = MOTOR_LEVEL_WEAK;
			}
			else if (TouchID == 2)
			{			
				shortcut_info.vibration_level = MOTOR_LEVEL_STANDARD;
			}
			else if (TouchID == 3)
			{			
				shortcut_info.vibration_level = MOTOR_LEVEL_STRONG;
			}
			if(shortcut_info.vibration_level != MOTOR_LEVEL_OFF)
				MotoRun(MOTO_DEFAULT);
			SendMsg(MSG_DISPLAY);
			break;

		case MSG_RIGHTDOWN_SHORTKEY: 				/*右键 单击*/		
			UnloadHandler(DifferentRemindId.VirateHandlerId);	
			break;

		case MSG_DISPLAY:
			ShowVirateLevel(1);	
			break;

		default:
			return RET_MSG_UNHANDLED;
	}
	return RET_MSG_HANDLED;
}

uint16 CurrencyHandler(MsgType msg, int iParam, void *pContext)
{
	switch(msg)
	{
		case MSG_HANDLER_BEGIN:
			DifferentRemindId.CurrencyHandlerId = iParam;	
			SetSystemSuspendTime(DISPLAY_TIME_10S_FLAG);
			break;

		case MSG_HANDLER_END:
			DifferentRemindId.CurrencyHandlerId = 0;
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
		case MSG_TOUCH_UP_SHORTKEY:				/*触摸 单击*/  
		case MSG_TOUCH_UP_DOUBLE_SHORTKEY:		/*触摸 双击*/	
		case MSG_TOUCH_LONGKEY:					/*触摸 长按*/
			break;
	
		case MSG_RIGHTDOWN_SHORTKEY: 				/*右键 单击*/		
			UnloadHandler(DifferentRemindId.CurrencyHandlerId);	
			break;

		case MSG_DISPLAY:	
			ShowCurrencyApps();				
			break;

		default:
			return RET_MSG_UNHANDLED;
	}
	return RET_MSG_HANDLED;	
}

uint8 TimerMenuIndex=1;
/*时间界面选择Handler*/
uint16 TimerMenuHandler(MsgType msg, int iParam, void *pContext)
{
	switch(msg)
	{
		case MSG_HANDLER_BEGIN:
			DifferentRemindId.TimerMenuHandlerId = iParam;	
			SetSystemSuspendTime(DISPLAY_TIME_10S_FLAG);
			break;

		case MSG_HANDLER_END:
			DifferentRemindId.TimerMenuHandlerId = 0;
			break;
				
/*用户操作*/		
		case MSG_LEFTDOWN_DOUBLE_SHORTKEY:		/*左键 双击*/
		case MSG_LEFTDOWN_LONGKEY:           			/*左键 长按*/	
		case MSG_RIGHTDOWN_DOUBLE_SHORTKEY:   	/*右键 双击*/	
		case MSG_RIGHTDOWN_LONGKEY:				/*右键 长按*/	
		case MSG_RIGHTDOWN_LONGKEY_UP:			/*右键 长按松开*/		
		case MSG_TOUCH_UP_FLICK:					/*上*/
		case MSG_TOUCH_DOWN_FLICK:				/*下*/		
		case MSG_TOUCH_UP_DOUBLE_SHORTKEY:		/*触摸 双击*/	
		case MSG_TOUCH_LONGKEY:					/*触摸 长按*/
			break;
			
		case MSG_TOUCH_DOWN_UP_FLICK:				/*左*/
			if(TimerMenuIndex<5)
				TimerMenuIndex++;
			if(TimerMenuIndex>=5) {
				TimerMenuIndex=1;
			}
			ShowTimerMenu(TimerMenuIndex);
			break;

		case MSG_TOUCH_UP_DOWN_FLICK: 			/*右*/
			if(TimerMenuIndex<=1)
				TimerMenuIndex=4;
			else
				TimerMenuIndex--;
			ShowTimerMenu(TimerMenuIndex);
			break;
			
		case MSG_RIGHTDOWN_SHORTKEY: 				/*右键 单击*/	
			if(TimerMenuIndex!=1){
				TimerMenuIndex=1;
			} 
			UnloadHandler(DifferentRemindId.TimerMenuHandlerId); 
			break;

		case MSG_LEFTDOWN_SHORTKEY:				/*左键 单击*/	
		case MSG_TOUCH_UP_SHORTKEY:				/*触摸 单击*/  	
			if(TimerMenuIndex==1){
				TimerMenuFlag=1;
			}else if(TimerMenuIndex==2){
				TimerMenuFlag=2;
			}else if(TimerMenuIndex==3){
				TimerMenuFlag=3;
			}else if(TimerMenuIndex==4){
				TimerMenuFlag=4;
			}else if(TimerMenuIndex==5){
				TimerMenuFlag=5;
			}
			WRITE_USER_PARAMETER((uint8 *)&TimerMenuFlag, TIMER_MENU_LOC, TIMER_MENU_LEN);
			ShowTimerMenuUpdated();
			osal_start_timerEx(GetAppTaskId(), MSG_DISPLAY, 2000);
			break;
			
		case MSG_DISPLAY:	
			ShowTimerMenu(TimerMenuIndex);		
			break;
					
		default:
			return RET_MSG_UNHANDLED;
	}
	return RET_MSG_HANDLED;
}

uint16 SoundHandler(MsgType msg, int iParam, void *pContext)
{
	switch(msg)
	{
		case MSG_HANDLER_BEGIN:
			DifferentRemindId.SoundHandlerId = iParam;	
			SetSystemSuspendTime(DISPLAY_TIME_10S_FLAG);
			break;

		case MSG_HANDLER_END:
			DifferentRemindId.SoundHandlerId = 0;
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
		case MSG_TOUCH_UP_DOUBLE_SHORTKEY:		/*触摸 双击*/	
		case MSG_TOUCH_LONGKEY:					/*触摸 长按*/
			break;

		case MSG_RIGHTDOWN_SHORTKEY: 				/*右键 单击*/	
			UnloadHandler(DifferentRemindId.SoundHandlerId); 
			break;

		case MSG_TOUCH_UP_SHORTKEY:				/*触摸 单击*/
			{
				uint8 TouchID = Get_Screen_Locaton(4,1);
				switch(TouchID)
				{
					case 0:
					case 1:
						setting_info.sounds = 1;	//ON
						ShowSwitchState(1,true);
						break;

					case 2:
					case 3:
						setting_info.sounds = 0;	//OFF
						ShowSwitchState(0, true);
						break;
				}
			}
			break;

		case MSG_DISPLAY:	
			ShowSwitchState(setting_info.sounds, false);	
			break;

		default:
			return RET_MSG_UNHANDLED;
	}
	return RET_MSG_HANDLED;
	
}

/*音乐Handler*/
uint16 MusicHandler(MsgType msg, int iParam, void *pContext)
{
	switch(msg)
	{
		case MSG_HANDLER_BEGIN:
			DifferentRemindId.MusicHandlerId = iParam;	
			SetSystemSuspendTime(DISPLAY_TIME_10S_FLAG);
			if ((BledisconnectFlag == BT_CLOSE) || (BledisconnectFlag == BT_DISCONNECT))
			{
				MusicStates=MS_CONNECT;		
				osal_start_timerEx(GetAppTaskId(), MSG_MUSIC_NOTIC_CONNECTING, 5000);
			}
			else
			{
				MusicStates=MS_NO;	
				memset((char*)(MusicBuf),0,128);
				MusicSetting(MS_CHECK);
			}
			break;

		case MSG_HANDLER_END:
			DifferentRemindId.MusicHandlerId = 0;
			DisplayUnicode_Stop();
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
		case MSG_TOUCH_UP_DOUBLE_SHORTKEY:		/*触摸 双击*/	
		case MSG_TOUCH_LONGKEY:					/*触摸 长按*/
			break;
					
		case MSG_RIGHTDOWN_SHORTKEY: 				/*右键 单击*/
		case MSG_MUSIC_HANDLER_TIMEOUT:	
			UnloadHandler(DifferentRemindId.MusicHandlerId);	
			break;
			
		case MSG_TOUCH_UP_SHORTKEY:				/*触摸 单击*/
			{
				uint8 TouchID = Get_Screen_Locaton(3,3);
				switch(MusicStates)		
				{
					case MS_CONNECT:
					case MS_CONNECT_FAIL:	
						break;

					case MS_NO:	
						break;
					case MS_STOP:
					case MS_PLAY:
						if(TouchID==8)					/*+*/
						{
							MusicSetting(MS_VOL_ADD);
							ShowMusicControl(MS_VOL_ADD);
						}
						else if(TouchID==6) /*-*/
						{
							MusicSetting(MS_VOL_REDUCE);
							ShowMusicControl(MS_VOL_REDUCE);
						}
						if(TouchID==3) 	/*pre*/	
						{
							MusicSetting(MS_PREVIOUS);
							ShowMusicControl(MS_PREVIOUS);
						}
						else if(TouchID==5) 	/*next*/	
						{
							MusicSetting(MS_NEXT);		
							ShowMusicControl(MS_NEXT);
						}
						else if(TouchID==4) 	/*play stop*/	
						{
							if(MusicStates==MS_STOP)
								MusicSetting(MS_PLAY);
							else if(MusicStates==MS_PLAY)
								MusicSetting(MS_STOP);
						}
						break;	
				}
			}
			break;

		case MSG_MUSIC_CONTROL_TIMEOUT:
			if(CheckpCurrentHandler(DifferentRemindId.MusicHandlerId))	
				ShowMusicControl(MS_RESUME);
			break;
		 
		case MSG_MUSIC_NOTIC_CONNECTING:			/*Connecting超时*/
			MusicStates=MS_CONNECT_FAIL;
			osal_start_timerEx(GetAppTaskId(), MSG_MUSIC_HANDLER_TIMEOUT, 2000);
		case MSG_MUSIC_NOTIC:	
			if(CheckpCurrentHandler(DifferentRemindId.MusicHandlerId))
			{
				if(LCD_DISPLAY_FLAG)
					ShowMusic(MusicStates,false);
				else
					SendMsg(MSG_DISPLAY);
			}
			break;
		case MSG_DATE_TIME_FORMAT_UPDATE:		
		case MSG_DISPLAY:
			if(CheckpCurrentHandler(DifferentRemindId.MusicHandlerId))
			{			
				ShowMusic(MusicStates,true);	
			}
			break;
					
		default:
			return RET_MSG_UNHANDLED;
	}
	return RET_MSG_HANDLED;
}

/*相机Handler*/
uint8 camera_countdown_cnt = 0;
uint8 Unload_Camera = 0x00;
uint16 CameraHandler(MsgType msg, int iParam, void *pContext)
{
	switch(msg)
	{
		case MSG_HANDLER_BEGIN:
			DifferentRemindId.CameraHandlerId = iParam;	
			SetSystemSuspendTime(DISPLAY_TIME_10S_FLAG);
			if ((BledisconnectFlag == BT_CLOSE) || (BledisconnectFlag == BT_DISCONNECT))
			{
				CameraStates=0xFF;		
				osal_start_timerEx(GetAppTaskId(), MSG_CAMERA_NOTIC_CONNECTING, 5000);
			}
			else
				CameraStates =0x00;
			camera_countdown_cnt = 0;
			Unload_Camera = 0;
			break;

		case MSG_HANDLER_END:
			DifferentRemindId.CameraHandlerId = 0;
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
		case MSG_TOUCH_UP_DOUBLE_SHORTKEY:		/*触摸 双击*/	
		case MSG_TOUCH_LONGKEY:					/*触摸 长按*/
			if(Unload_Camera)
				UnloadHandler(DifferentRemindId.CameraHandlerId);	
			break;
					
		case MSG_TOUCH_UP_SHORTKEY:				/*触摸 单击*/  
			{
				if(CameraStates==0x00||CameraStates ==0x03)
				{
					CameraStates =0x03;
					CameraControl(CameraStates);
				}
			}
			if(Unload_Camera)
			{
				UnloadHandler(DifferentRemindId.CameraHandlerId);
			}
			break;
			
		case MSG_RIGHTDOWN_SHORTKEY: 				/*右键 单击*/	
			if (CameraStates == 0x01)
				break;
			if(Unload_Camera)
			{
				UnloadHandler(DifferentRemindId.CameraHandlerId);	
				break;
			}
		case MSG_CAMERA_HANDLER_TIMEOUT:	
			Unload_Camera = 1;
			break;
			
		case MSG_CAMERA_NOTIC:							/*相机连接,准备拍照*/
			osal_stop_timerEx(GetAppTaskId(), MSG_CAMERA_NOTIC_CONNECTING);	
		case MSG_CAMERA_CONTROL_TIMEOUT:	
			CameraStates =0x00;
			if(CheckpCurrentHandler(DifferentRemindId.CameraHandlerId))
				ShowCamera(CameraStates);	
			break;
				
		case MSG_CAMERA_NOTIC_CONNECTING:			/*Connecting超时Failed*/
			CameraStates=0xFE;
			osal_start_timerEx(GetAppTaskId(), MSG_CAMERA_HANDLER_TIMEOUT, 100);	
			if(CheckpCurrentHandler(DifferentRemindId.CameraHandlerId))
				ShowCamera(CameraStates);
			break;	
		case MSG_DATE_TIME_FORMAT_UPDATE:		
		case MSG_DISPLAY:	
			ShowCamera(CameraStates);
			if(CameraStates==0xFF)						/*Connecting*/
				CameraControl(CameraStates);
			break;

		case MSG_CAMERA_COUNTDOWN:			/*倒计时*/
			if(CheckpCurrentHandler(DifferentRemindId.CameraHandlerId))
			{
				camera_countdown_cnt ++;
				if(camera_countdown_cnt>3)
				{
					camera_countdown_cnt = 0;
					CameraControl(0x00);
					SendMsg(MSG_CAMERA_CONTROL_TIMEOUT);
				}
				else
				{
					ShowCameraCountDown(camera_countdown_cnt);
					osal_start_timerEx(GetAppTaskId(), MSG_CAMERA_COUNTDOWN, 1000);
				}

			}
			break;
		case MSG_OPEN_CAMERA_NOTIC:
			if(Camera_state)
			{
				CameraStates =0x01;
				SendMsg(MSG_CAMERA_COUNTDOWN);
				Camera_state = 0;
			}
			else
			{
				osal_start_timerEx(GetAppTaskId(), MSG_CAMERA_NOTIC_CONNECTING, 1000);
			}
			break;
		default:
			 return RET_MSG_UNHANDLED;
	}
	return RET_MSG_HANDLED;
}

/*寻找手机Handler*/
uint16 SearchHandler(MsgType msg, int iParam, void *pContext)
{
	switch(msg)
	{
		case MSG_HANDLER_BEGIN:
			DifferentRemindId.SearchHandlerId = iParam;	
			SearchStates =0x00;
			SetSystemSuspendTime(DISPLAY_TIME_10S_FLAG);
			break;

		case MSG_HANDLER_END:
			DifferentRemindId.SearchHandlerId = 0;
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
		case MSG_TOUCH_UP_DOUBLE_SHORTKEY:		/*触摸 双击*/	
		case MSG_TOUCH_LONGKEY:					/*触摸 长按*/
			break;
	
		case MSG_TOUCH_UP_SHORTKEY:				/*触摸 单击*/ 
			{
				uint8 TouchID = Get_Screen_Locaton(4,4);
				if(TouchID==5||TouchID==6||TouchID==9||TouchID==10)		
				{
					if(shortcut_info.find_me)
					{
						SearchStates =0x01;
						LoadHandler(SearchingHandler,0);
					}

				}
			}
			return RET_MSG_HANDLED;
		case MSG_RIGHTDOWN_SHORTKEY: 				/*右键 单击*/		
		case MSG_CAMERA_HANDLER_TIMEOUT:	
			UnloadHandler(DifferentRemindId.SearchHandlerId);	
			break;
			
		case MSG_SEARCH_NOTIC_CONNECTING:
			/*
			if(SearchStates==0x01)
			{
				SearchForMobilePhones(SS_START); 
				osal_start_timerEx(GetAppTaskId(), MSG_SEARCH_NOTIC_CONNECTING, 1000);	
			}
			break;
			*/
			return RET_MSG_HANDLED;
		case MSG_DATE_TIME_FORMAT_UPDATE:	
		case MSG_DISPLAY:	
			ShowSearch(SearchStates);
			break;
					
		default:
			return RET_MSG_UNHANDLED;
	}
	return RET_MSG_HANDLED;
}

/*寻找手机Handler*/
uint16 SearchingHandler(MsgType msg, int iParam, void *pContext)
{
	switch(msg)
	{
		case MSG_HANDLER_BEGIN:
			DifferentRemindId.SearchingHandlerId = iParam;	
			SearchStates =0x01;
			SetSystemSuspendTime(DISPLAY_TIME_10S_FLAG);
			SearchForMobilePhones(SS_START); 
			osal_start_timerEx(GetAppTaskId(), MSG_SEARCH_NOTIC_CONNECTING, 1000);	
			break;

		case MSG_HANDLER_END:
			osal_stop_timerEx(GetAppTaskId(), MSG_SEARCH_NOTIC_CONNECTING);
			osal_clear_event(GetAppTaskId(), MSG_SEARCH_NOTIC_CONNECTING);
			SearchStates =0x00;
			SearchForMobilePhones(SS_STOP);
			DifferentRemindId.SearchingHandlerId = 0;
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
		case MSG_TOUCH_UP_DOUBLE_SHORTKEY:		/*触摸 双击*/	
		case MSG_TOUCH_LONGKEY:					/*触摸 长按*/
			break;
			
		case MSG_TOUCH_UP_SHORTKEY:				/*触摸 单击*/  	
		case MSG_RIGHTDOWN_SHORTKEY: 				/*右键 单击*/		
		case MSG_CAMERA_HANDLER_TIMEOUT:	
			UnloadHandler(DifferentRemindId.SearchingHandlerId);	
			break;
			
		case MSG_SEARCH_NOTIC_CONNECTING:
			if(SearchStates==0x01)
			{
				SearchForMobilePhones(SS_START); 
				SearchStates = 0x00;
			}
			else
			{	
				SearchStates = 0x01;
			}
			osal_start_timerEx(GetAppTaskId(), MSG_SEARCH_NOTIC_CONNECTING, 1000);	
			if((CheckpCurrentHandler(DifferentRemindId.SearchingHandlerId)==false)||(Powerstate.standbystate==true))
				break;
		case MSG_DATE_TIME_FORMAT_UPDATE:	
		case MSG_DISPLAY:	
			ShowSearching(SearchStates);			
			break;
					
		default:
			return RET_MSG_UNHANDLED;
	}
	return RET_MSG_HANDLED;
}


uint8 ToolAppsIdex=1;
uint16 ToolAppsHandler(MsgType msg, int iParam, void *pContext)
{
	switch(msg)
	{
		case MSG_HANDLER_BEGIN:
			ToolAppsIdex =1;
			DifferentRemindId.ToolAppsHandlerId = iParam;
			SetSystemSuspendTime(DISPLAY_TIME_10S_FLAG);
			break;

		case MSG_HANDLER_END:
			DifferentRemindId.ToolAppsHandlerId = 0;
			break;

		/*用户操作*/
		case MSG_LEFTDOWN_SHORTKEY:				/*左键 单击*/	
		case MSG_LEFTDOWN_DOUBLE_SHORTKEY:		/*左键 双击*/
		case MSG_LEFTDOWN_LONGKEY:           			/*左键 长按*/
		case MSG_RIGHTDOWN_DOUBLE_SHORTKEY:   	/*右键 双击*/	
		case MSG_RIGHTDOWN_LONGKEY:				/*右键 长按*/	
		case MSG_RIGHTDOWN_LONGKEY_UP:			/*右键 长按松开*/		
		case MSG_TOUCH_DOWN_UP_FLICK:				/*左*/ 
		case MSG_TOUCH_UP_DOWN_FLICK: 			/*右*/
		case MSG_TOUCH_UP_DOUBLE_SHORTKEY:		/*触摸 双击*/	
		case MSG_TOUCH_LONGKEY:					/*触摸 长按*/
			break;

		case MSG_TOUCH_UP_FLICK:					/*上*/
			if(ToolAppsIdex<2)
			{
				ToolAppsIdex++;
			}
			ShowToolApps(ToolAppsIdex);
			break;

		case MSG_TOUCH_DOWN_FLICK:				/*下*/
			if(ToolAppsIdex>1)
			{
				ToolAppsIdex--;
			}
			ShowToolApps(ToolAppsIdex);			
			break;

		case MSG_RIGHTDOWN_SHORTKEY: 				/*右键 单击*/	
			UnloadHandler(DifferentRemindId.ToolAppsHandlerId);
			break;

		case MSG_TOUCH_UP_SHORTKEY:				/*触摸 单击*/  		
		{
			uint8 TouchID = Get_Screen_Locaton(3,1);
			switch(ToolAppsIdex)
			{
				case 1:
					if(TouchID==0)
					{
						LoadHandler(CameraHandler, 0);
					}
					else if(TouchID==1)
					{
						LoadHandler(MusicHandler, 0);
					}
					else if(TouchID==2)
					{
						LoadHandler(TimerHandler, 0);
					}						
					break;
				case 2:
					if(TouchID==0)
					{
						LoadHandler(SearchHandler, 0);				
					}
					else if(TouchID==1)
					{
					}
					else if(TouchID==2)
					{
					}					
					break;
				case 3:
					if(TouchID==0)
					{
					}
					else if(TouchID==1)
					{
					}
					else if(TouchID==2)
					{
					}						
					break;
			}
		}		
		break;

		case MSG_DISPLAY:
			ShowToolApps(ToolAppsIdex);
			break;

		default:
			return RET_MSG_UNHANDLED;	
	}
	return RET_MSG_HANDLED;
}

/*重置Handler*/
bool IsRestProgress = false;
uint16 ResetHandler(MsgType msg, int iParam, void *pContext)
{
	static uint8 update_level = 0;
	uint8 InitMark=0;
	switch(msg)
	{
		case MSG_HANDLER_BEGIN:
			IsRestProgress = false;
			DifferentRemindId.ResetHandlerId = iParam;
			SetSystemSuspendTime(DISPLAY_TIME_10S_FLAG);
			break;

		case MSG_HANDLER_END:
			IsRestProgress = false;
			DifferentRemindId.ResetHandlerId = 0;
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
		case MSG_TOUCH_UP_DOUBLE_SHORTKEY:		/*触摸 双击*/	
		case MSG_TOUCH_LONGKEY:					/*触摸 长按*/
			break;
			
		case MSG_RIGHTDOWN_SHORTKEY: 				/*右键 单击*/
			UnloadHandler(DifferentRemindId.ResetHandlerId);
			break;
			
		case MSG_TOUCH_UP_SHORTKEY:				/*触摸 单击*/  
			{
				uint8 TouchID = Get_Screen_Locaton(3,3);
				
				if (update_level == 0)	//works only  in showReset display
				{
					if(TouchID==6)
					{
						osal_start_timerEx(GetAppTaskId(), MSG_RESET_PROGRESS_UPDATE,10);
						IsRestProgress = true;
					}
					else if(TouchID==8)
					{
						UnloadHandler(DifferentRemindId.ResetHandlerId);
					}	
				}
			}	
			break;

		case MSG_RESET_PROGRESS_UPDATE:
				update_level = 0;
		              FLP_TurnOffBle();
		              WRITE_USER_FLAG(&InitMark,INIT_MARK_LOC,INIT_MARK_LEN);
		              InitBandInfo();				
				ResetBoard();			
#if 0
			ShowResetProgress(update_level++);
			if (update_level >10)
			{
				update_level = 0;
		              FLP_TurnOffBle();
		              WRITE_USER_FLAG(&InitMark,INIT_MARK_LOC,INIT_MARK_LEN);
		              InitBandInfo();				
				ResetBoard();
			}
			else 
			{
				osal_start_timerEx(GetAppTaskId(), MSG_RESET_PROGRESS_UPDATE, 200);
			}
#endif
			break;	
		case MSG_DATE_TIME_FORMAT_UPDATE:	
		case MSG_DISPLAY:
			ShowReset();
			break;

		default:
			if(IsRestProgress == false)
				return RET_MSG_UNHANDLED;
			else
				return RET_MSG_HANDLED;
	}
	return RET_MSG_HANDLED;
}


/*设置Handler*/
uint8 SettingIndex=1;
uint16 SettingHandler(MsgType msg, int iParam, void *pContext)
{
	switch(msg)
	{
		case MSG_HANDLER_BEGIN:
			SettingIndex =1;
			DifferentRemindId.SettingHanderId = iParam;
			SetSystemSuspendTime(DISPLAY_TIME_10S_FLAG);
			break;

		case MSG_HANDLER_END:
			DifferentRemindId.SettingHanderId = 0;
			break;
/*用户操作*/
		case MSG_LEFTDOWN_SHORTKEY:				/*左键 单击*/	
		case MSG_LEFTDOWN_DOUBLE_SHORTKEY:		/*左键 双击*/
		case MSG_LEFTDOWN_LONGKEY:           			/*左键 长按*/
		case MSG_RIGHTDOWN_DOUBLE_SHORTKEY:   	/*右键 双击*/	
		case MSG_RIGHTDOWN_LONGKEY:				/*右键 长按*/	
		case MSG_RIGHTDOWN_LONGKEY_UP:			/*右键 长按松开*/		
		case MSG_TOUCH_DOWN_UP_FLICK:				/*左*/ 
		case MSG_TOUCH_UP_DOWN_FLICK: 			/*右*/		
		case MSG_TOUCH_UP_DOUBLE_SHORTKEY:		/*触摸 双击*/	
		case MSG_TOUCH_LONGKEY:					/*触摸 长按*/
			break;
		
			
		case MSG_RIGHTDOWN_SHORTKEY: 				/*右键 单击*/	
			UnloadHandler(DifferentRemindId.SettingHanderId);
			break;

		case MSG_TOUCH_UP_FLICK:					/*上*/
			if(SettingIndex<2)
			{
				SettingIndex++;
				ShowSettingApps(SettingIndex);
			}
			break;

		case MSG_TOUCH_DOWN_FLICK:				/*下*/
			if(SettingIndex>1)
			{
				SettingIndex--;
				ShowSettingApps(SettingIndex); 	
			}
			break;

		case MSG_TOUCH_UP_SHORTKEY:				/*触摸 单击*/  
			{
				uint8 TouchID = Get_Screen_Locaton(3,1);
				switch(SettingIndex)
				{
					case 1:
						if(TouchID==0)
						{
							LoadHandler(BatteryHandler, 0);
						}
						else if(TouchID==1)
						{	
							LoadHandler(ResetHandler, 0);	
						}
						else if(TouchID==2)
						{
							LoadHandler(RestartHandler, 0);							
						}						
						break;					
					case 2:
						if(TouchID==0)
						{
							LoadHandler(WristWakeUpHandler, 0);
						}
						else if(TouchID==1)
						{

						}
						else if(TouchID==2)
						{
						}											
						break;
				}
			}	
			break;

		case MSG_DISPLAY:
			ShowSettingApps(SettingIndex);
			break;

		default:
			return RET_MSG_UNHANDLED;
	}
	return RET_MSG_HANDLED;
}


uint16 AppBoxHandler(MsgType msg, int iParam, void *pContext)
{
	switch(msg)
	{
		case MSG_HANDLER_BEGIN:
			DifferentRemindId.AppBoxHanderId = iParam;
			SetSystemSuspendTime(DISPLAY_TIME_10S_FLAG);
			break;

		case MSG_HANDLER_END:
			DifferentRemindId.AppBoxHanderId = 0;
			break;
/*用户操作*/
		case MSG_LEFTDOWN_SHORTKEY:				/*左键 单击*/	
		case MSG_LEFTDOWN_DOUBLE_SHORTKEY:		/*左键 双击*/
		case MSG_LEFTDOWN_LONGKEY:           			/*左键 长按*/
		case MSG_RIGHTDOWN_DOUBLE_SHORTKEY:   	/*右键 双击*/	
		case MSG_RIGHTDOWN_LONGKEY:				/*右键 长按*/	
		case MSG_RIGHTDOWN_LONGKEY_UP:			/*右键 长按松开*/		
		case MSG_TOUCH_DOWN_UP_FLICK:				/*左*/ 
		case MSG_TOUCH_UP_DOWN_FLICK: 			/*右*/		
		case MSG_TOUCH_UP_DOUBLE_SHORTKEY:		/*触摸 双击*/	
		case MSG_TOUCH_LONGKEY:					/*触摸 长按*/
			break;

		case MSG_RIGHTDOWN_SHORTKEY: 				/*右键 单击*/	
			UnloadHandler(DifferentRemindId.AppBoxHanderId);
			break;

		case MSG_TOUCH_UP_FLICK:					/*上*/
			break;
		case MSG_TOUCH_DOWN_FLICK:				/*下*/		
			break;

		case MSG_TOUCH_UP_SHORTKEY:				/*触摸 单击*/  
			{
				uint8 TouchID = Get_Screen_Locaton(4,1);
			
					if(TouchID==0)
					{
						LoadHandler(CameraHandler, 0);
					}
					else if(TouchID==1)
					{			
						LoadHandler(MusicHandler, 0);		
					}
					else if(TouchID==2)
					{	
						LoadHandler(TimerHandler, 0);			
					}						
					else if(TouchID==3)
					{		
						LoadHandler(SearchHandler, 0);
					}						
					
				
			}	
			break;

		case MSG_DISPLAY:
			ShowAppBox();
			break;

		default:
			return RET_MSG_UNHANDLED;
	}
	return RET_MSG_HANDLED;
}


uint8 LanguageIndex=1;
#if 0
static uint16 LanguageHandler(MsgType msg, int iParam, void *pContext)
{
	switch(msg)
	{
		case MSG_HANDLER_BEGIN:
			DifferentRemindId.LanguageHandlerId = iParam;	
			SetSystemSuspendTime(DISPLAY_TIME_10S_FLAG);
			return RET_MSG_HANDLED;

		case MSG_HANDLER_END:
			DifferentRemindId.LanguageHandlerId = 0;
			return RET_MSG_HANDLED;
				
		/*用户操作*/
		case MSG_LEFTDOWN_SHORTKEY:				/*左键 单击*/	
		case MSG_LEFTDOWN_DOUBLE_SHORTKEY:		/*左键 双击*/
		case MSG_LEFTDOWN_LONGKEY:           			/*左键 长按*/
		case MSG_RIGHTDOWN_DOUBLE_SHORTKEY:   	/*右键 双击*/	
		case MSG_RIGHTDOWN_LONGKEY:				/*右键 长按*/	
		case MSG_RIGHTDOWN_LONGKEY_UP:			/*右键 长按松开*/		
		case MSG_TOUCH_DOWN_UP_FLICK:				/*左*/ 
		case MSG_TOUCH_UP_DOWN_FLICK: 			/*右*/		
		case MSG_TOUCH_UP_DOUBLE_SHORTKEY:		/*触摸 双击*/	
		case MSG_TOUCH_LONGKEY:					/*触摸 长按*/
			return RET_MSG_HANDLED;


		case MSG_RIGHTDOWN_SHORTKEY: 				/*右键 单击*/	
			LanguageIndex=1;
			UnloadHandler(DifferentRemindId.LanguageHandlerId); 
			return RET_MSG_HANDLED;
			
		case MSG_TOUCH_UP_FLICK:						/*上*/
			if(LanguageIndex<3)
			{
				LanguageIndex++;
				ShowLanguageApps(LanguageIndex);
			}
			return RET_MSG_HANDLED;
			
		case MSG_TOUCH_DOWN_FLICK:					/*下*/
			if(LanguageIndex>1)
			{
				LanguageIndex--;
				ShowLanguageApps(LanguageIndex); 		
			}
			return RET_MSG_HANDLED;
							
		case MSG_TOUCH_UP_SHORTKEY: 				/*单击*/
			{
				uint8 TouchID = Get_Screen_Locaton(4,1);
				switch(LanguageIndex)
				{
					case 1:
						if(TouchID==0){
							SetValue.Language=LAN_ENGLISH;
							ShowTimerMenuUpdated();
							osal_start_timerEx(GetAppTaskId(), MSG_DISPLAY, 2000);
						}else if(TouchID==1){
							SetValue.Language=LAN_ESPANOL;
							ShowTimerMenuUpdated();
							osal_start_timerEx(GetAppTaskId(), MSG_DISPLAY, 2000);
						}else if(TouchID==2){
							SetValue.Language=LAN_FRANCAIS;
							ShowTimerMenuUpdated();
							osal_start_timerEx(GetAppTaskId(), MSG_DISPLAY, 2000);
						}
						else if(TouchID==3){
							SetValue.Language=LAN_DEUTSCH;
							ShowTimerMenuUpdated();
							osal_start_timerEx(GetAppTaskId(), MSG_DISPLAY, 2000);
						}
						break;
					case 2:
						if(TouchID==0){
							SetValue.Language=LAN_DUTCH;
							ShowTimerMenuUpdated();
							osal_start_timerEx(GetAppTaskId(), MSG_DISPLAY, 2000);
						}else if(TouchID==1){
							SetValue.Language=LAN_ITALIANO;
							ShowTimerMenuUpdated();
							osal_start_timerEx(GetAppTaskId(), MSG_DISPLAY, 2000);
						
						}else if(TouchID==2){
							SetValue.Language=LAN_POLSKI;
							ShowTimerMenuUpdated();
							osal_start_timerEx(GetAppTaskId(), MSG_DISPLAY, 2000);
						}
						else if(TouchID==3){
							SetValue.Language=LAN_PORTUGUESE;
							ShowTimerMenuUpdated();
							osal_start_timerEx(GetAppTaskId(), MSG_DISPLAY, 2000);
						}
						break;
					case 3:
						if(TouchID==0){
							SetValue.Language=LAN_RUSSIAN;
							ShowTimerMenuUpdated();
							osal_start_timerEx(GetAppTaskId(), MSG_DISPLAY, 2000);
						}else if(TouchID==1){
							if(IsOTAFontRight == false)
								break;
							SetValue.Language=LAN_CHINESE_SIMPLIFID;
							ShowTimerMenuUpdated();
							osal_start_timerEx(GetAppTaskId(), MSG_DISPLAY, 2000);
						}else if(TouchID==2){
							//SetValue.Language=LAN_CHINESE_TRADITIONAL;
							//ShowTimerMenuUpdated();
							//osal_start_timerEx(GetAppTaskId(), MSG_DISPLAY, 2000);
						}
						else if(TouchID==3){
							
						}
						break;
					case 4:
						if(TouchID==0){
							                                                                  
						}else if(TouchID==1){
						
						}
						break;
				}
			}
			ChooseLanguage((uint8)SetValue.Language);
			WRITE_USER_PARAMETER(&SetValue.Language,LAST_LANGUAGE_LOC,1);
			return RET_MSG_HANDLED;
			
		case MSG_DISPLAY:	
			ShowLanguageApps(LanguageIndex);
			return RET_MSG_HANDLED;
					
		default:
			break;
	}

	return RET_MSG_UNHANDLED;
}
#endif



/*一级界面:应用选择圆Handler*/
uint8 ToolIdex=1;
uint16 ToolHandler(MsgType msg, int iParam, void *pContext)
{
	switch(msg)
	{
		case MSG_HANDLER_BEGIN:
			DifferentRemindId.ToolHandlerId = iParam;
			SetSystemSuspendTime(DISPLAY_TIME_10S_FLAG);
			break;

		case MSG_HANDLER_END:
			DifferentRemindId.ToolHandlerId = 0;
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
		case MSG_TOUCH_UP_DOUBLE_SHORTKEY:		/*触摸 双击*/	
		case MSG_TOUCH_LONGKEY:					/*触摸 长按*/
			break;

		case MSG_TOUCH_DOWN_UP_FLICK:				/*左*/ 
			if(ToolIdex==1)
			{
				LoadMenuHandler(MENU_FORWARD);
			}	
			break;
			
		case MSG_RIGHTDOWN_SHORTKEY: 				/*右键 单击*/	
			if(ToolIdex==2)
			{
				ToolIdex=1;
			}
		case MSG_TOUCH_UP_DOWN_FLICK: 			/*右*/
			LoadMenuHandler(MENU_BACKWARD); 
			break;

		case MSG_TOUCH_UP_SHORTKEY:				/*触摸 单击*/  	
			{
				uint8 TouchID = Get_Screen_Locaton(4,4);
									
				switch(ToolIdex)
				{	
					case 1:
						if( TouchID==0 )								// 11点位置
						{
							if(GetAncsMsgLayOut(Ancs_Social,NULL))
								LoadHandler(SocialMessageHandler, 0);		
							else
							{
								AncsMotoSkip= true;
								LoadHandler(SocialHomePageHandler, 0);						
							}
						}
						else if((TouchID==1) || (TouchID==2))			// 0点位置
						{
							LoadHandler(NFC_Handler, 0);	
						}
						else if(TouchID==3)							// 1点位置		
						{
							LoadHandler(PhoneDial_Handler, 0);		
						}
						else if((TouchID==4) || (TouchID==8))			// 9点位置					
						{
							if(GetAncsMsgLayOut(Ancs_Sms,NULL))
								LoadHandler(SMSHandler, 0);		
							else
							{
								AncsMotoSkip =true;
								LoadHandler(SMSHomePageHandler, 0);				
							}
						}
						else if((TouchID==7) || (TouchID==11))			// 3点位置				
						{
							LoadHandler(SportOverviewHandler, 0);
							
						}
						
						else if (TouchID==12)							// 7点位置	
						{
                            			LoadHandler(Siri_Handler, 0);	//TRACE("ToolHandler to Sound\r\n");	
						}
						else if ((TouchID==13) || (TouchID==14))		// 6点位置
						{
							LoadHandler(heartRate_Handler, 0);
						}
						else if (TouchID==15)							// 5点位置
						{
                           				LoadHandler(TimerMenuHandler, 0);
						}
						break;   
					case 2:
						if( TouchID==0 )								// 11点位置
						{
							if(GetPhBok_Flag())
								LoadHandler(PhoneBookHandler, 0);
						}
						else if((TouchID==1) || (TouchID==2))			// 0点位置
						{			
							LoadHandler(SleepMonitor_Handler, 0);
						}
						else if(TouchID==3)							// 1点位置				
						{			
							if(GetAncsMsgLayOut(Call_Record,NULL))
							{
								LoadHandler(PhoneRecordHandler, 0);	
							}	
						}
						else if((TouchID==4) || (TouchID==8))			// 9点位置				
						{
							LoadHandler(CameraHandler, 0);
						}
						else if((TouchID==7) || (TouchID==11))			// 3点位置				
						{
							LoadHandler(MusicHandler, 0);
						}					
						else if (TouchID==12)							// 7点位置	
						{               
							LoadHandler(TimerHandler, 0);							
						}
						else if ((TouchID==13) || (TouchID==14))		// 6点位置
						{
							LoadHandler(SearchHandler, 0);
						}
						else if (TouchID==15)							// 5点位置
						{	
							LoadHandler(SettingHandler, 0);
						}
						break;
				}

			}		
			break;

		case MSG_DISPLAY:
			ShowTool(ToolIdex);
			break;

		default:
			return RET_MSG_UNHANDLED;
	}
	return RET_MSG_HANDLED;
}


/*秒表Handler*/
uint16 TimerHandler(MsgType msg, int iParam, void *pContext)
{
	extern uint8 STflag;
	switch(msg)
	{
		case MSG_HANDLER_BEGIN:
			DifferentRemindId.TimerHandlerId = iParam;
			SetSystemSuspendTime(DISPLAY_TIME_10S_FLAG);
			break;
			
		case MSG_HANDLER_END:			
			DifferentRemindId.TimerHandlerId = 0;
			break;
			 	
/*用户操作*/
		case MSG_LEFTDOWN_SHORTKEY:				/*左键 单击*/	
		case MSG_LEFTDOWN_DOUBLE_SHORTKEY:		/*左键 双击*/
		case MSG_LEFTDOWN_LONGKEY:           			/*左键 长按*/
		case MSG_RIGHTDOWN_DOUBLE_SHORTKEY:   	/*右键 双击*/	
		
				
		case MSG_TOUCH_UP_FLICK:					/*上*/
		case MSG_TOUCH_DOWN_FLICK:				/*下*/
		case MSG_TOUCH_DOWN_UP_FLICK:				/*左*/ 
		case MSG_TOUCH_UP_DOWN_FLICK: 			/*右*/
		
		case MSG_TOUCH_UP_DOUBLE_SHORTKEY:		/*触摸 双击*/	
		case MSG_TOUCH_LONGKEY:					/*触摸 长按*/          
			break;

		case MSG_RIGHTDOWN_LONGKEY:				/*右键 长按*/	
			if (STui == ST_RUN)
			{	
				osal_stop_timerEx(GetClockTaskId(), STOP_WATCH_CNT_EVENT);
				STui = ST_STOP;
			}
		case MSG_RIGHTDOWN_LONGKEY_UP:			/*右键 长按松开*/
			SendMsg(msg);
                     break;
					 
		case MSG_RIGHTDOWN_SHORTKEY: 				/*右键 单击*/
			if (!DISPLAY_GOTO_SLEEP_FLAG)
			{
				UnloadHandler(DifferentRemindId.TimerHandlerId);
			}
			break;
			
		case MSG_TOUCH_UP_SHORTKEY:				/*触摸 单击*/  	
			{
				uint8 TouchID = Get_Screen_Locaton(3,3);
				switch(TouchID)
				{
					case 6:
						if(STui == ST_STOP)
						{
							STui = ST_PAGE;
                            gStopWatchLastMs = 0;
                            gStopWatchLastMsSave = 0;
							if(CheckpCurrentHandler(DifferentRemindId.TimerHandlerId ) == true)
							{
								ShowSecondsTimer(STui);
							}
						}
						break;
					case 8:
						if (STui == ST_PAGE)
						{
							STui = ST_RUN;
                            gStopWatchLastMs = 0;
                            gStopWatchLastMsSave = 0;
                            gStopWatchSysTick_start = pS_OsGetSysTick();
                            osal_set_event(GetClockTaskId(), STOP_WATCH_CNT_EVENT);
						}
						else if (STui == ST_RUN)
						{
							STui = ST_STOP;
                            gStopWatchLastMsSave = gStopWatchLastMs;
                            break;
						}
						else
						{
							if(STui == ST_STOP)
							{
								STui = ST_RUN;
                                gStopWatchSysTick_start = pS_OsGetSysTick();
                                osal_set_event(GetClockTaskId(), STOP_WATCH_CNT_EVENT);
							}
						}
						SendMsg(MSG_DISPLAY);
						break;

					default:
						return RET_MSG_HANDLED;
				}
				break;
			}

		case MSG_TIMER_TIMEOUT:
			break;

		case MSG_SECONDS_TIMER_EVENT:
			if((CheckpCurrentHandler(DifferentRemindId.TimerHandlerId ) == true)&&(!DISPLAY_GOTO_SLEEP_FLAG))
			{
				//SecondsTimerStart();
    			ShowSecondsTimer(STui);
                if(STui == ST_RUN)
                    osal_start_timerEx(GetAppTaskId(), MSG_SECONDS_TIMER_EVENT, 100);
            }
			break;
		case MSG_DATE_TIME_FORMAT_UPDATE:	
		case MSG_DISPLAY:
			ShowSecondsTimer(STui);
            if(STui == ST_RUN)
                osal_start_timerEx(GetAppTaskId(), MSG_SECONDS_TIMER_EVENT, 100);
			break;
		
		default:
			return RET_MSG_UNHANDLED;
	}
	return RET_MSG_HANDLED;	
}



uint16 BackHandler(MsgType msg, int iParam, void *pContext)
{
	switch(msg)
	{
		case MSG_HANDLER_BEGIN:
			DifferentRemindId.BackHandlerId = iParam;
			SetSystemSuspendTime(DISPLAY_TIME_10S_FLAG);
			break;

		case MSG_HANDLER_END:
			DifferentRemindId.BackHandlerId = 0;
			break;
/*用户操作*/
		case MSG_LEFTDOWN_SHORTKEY:				/*左键 单击*/	
		case MSG_LEFTDOWN_DOUBLE_SHORTKEY:		/*左键 双击*/
		case MSG_LEFTDOWN_LONGKEY:           			/*左键 长按*/
		case MSG_RIGHTDOWN_SHORTKEY: 				/*右键 单击*/	
		case MSG_RIGHTDOWN_DOUBLE_SHORTKEY:   	/*右键 双击*/	
		case MSG_RIGHTDOWN_LONGKEY:				/*右键 长按*/	
		case MSG_RIGHTDOWN_LONGKEY_UP:			/*右键 长按松开*/		
		case MSG_TOUCH_UP_FLICK:					/*上*/
		case MSG_TOUCH_DOWN_FLICK:				/*下*/		 
		case MSG_TOUCH_UP_DOUBLE_SHORTKEY:		/*触摸 双击*/	
			break;
			
		case MSG_TOUCH_LONGKEY:					/*触摸 长按*/	
			UnloadHandler(DifferentRemindId.BackHandlerId);	
			CurrentMenuIParam=LoadHandler(ToolHandler,0);	
			break;
			
		case MSG_TOUCH_DOWN_UP_FLICK:				/*左*/ 
			UnloadHandler(DifferentRemindId.BackHandlerId);	
			LoadHandler(MusicHandler,0);	 
			break;	 
			
		 case MSG_TOUCH_UP_DOWN_FLICK: 			/*右*/
			UnloadHandler(DifferentRemindId.BackHandlerId);	
			LoadHandler(DevicesCom_Handler,0);	 
			break; 		
			
		case MSG_TOUCH_UP_SHORTKEY:				/*触摸 单击*/ 
		     	UnloadHandler(DifferentRemindId.BackHandlerId);
		     	CurrentMenuIParam=LoadHandler(ToolHandler,0);
		     	break;
			 
		case MSG_DISPLAY:
			ShowBack();
			break;

		default:
			return RET_MSG_UNHANDLED;
	}
	return RET_MSG_HANDLED;

}

/*待激活界面Handler*/
uint16 SetupAlertHandler(MsgType msg, int iParam, void *pContext)
{
	switch(msg)
	{
		case MSG_HANDLER_BEGIN:
			DifferentRemindId.SetupAlertHandlerId = iParam;
			break;

		case MSG_HANDLER_END:
			DifferentRemindId.SetupAlertHandlerId = 0;
			StandbyTimeModifyEnable = true;
			StepInit();
			SendMsg(MSG_MOTO_DEFAULT);			/*进入时间界面。。。震动一秒*/
			break;
			
/*用户操作*/
		case MSG_LEFTDOWN_SHORTKEY:				/*左键 单击*/	
		//case MSG_RIGHTDOWN_SHORTKEY: 			/*右键 单击*/	
		case MSG_RIGHTDOWN_DOUBLE_SHORTKEY:   	/*右键 双击*/	
		case MSG_RIGHTDOWN_LONGKEY:				/*右键 长按*/	
		case MSG_RIGHTDOWN_LONGKEY_UP:			/*右键 长按松开*/

		case MSG_TOUCH_UP_FLICK:					/*上*/
		case MSG_TOUCH_DOWN_FLICK:				/*下*/
		case MSG_TOUCH_DOWN_UP_FLICK:				/*左*/ 
		case MSG_TOUCH_UP_DOWN_FLICK: 			/*右*/
		case MSG_TOUCH_UP_SHORTKEY:				/*触摸 单击*/  
		case MSG_TOUCH_UP_DOUBLE_SHORTKEY:		/*触摸 双击*/	
		case MSG_TOUCH_LONGKEY:					/*触摸 长按*/
			break;
			
		case MSG_RIGHTDOWN_SHORTKEY: 				/*右键 单击*/
			 SendMsg(MSG_SYSTEM_SUSPEND);
			break;
			
		case MSG_OTA:
		case MSG_TOUCH_UP_DOUBLE_SHORTKEY_FOR_RESUME:	/*触摸 双击 休眠唤醒*/
		case MSG_RIGHTDOWN_HOLDKEY:						/*右键 HOLD*/	
		case MSG_RIGHTDOWN_HOLDKEY_UP:					/*右键 HOLD松开*/
		case MSG_SYSTEM_SUSPEND:
		case MSG_SYSTEM_RESUME:
		case MSG_POWER_OFF_BATTERY:
		case MSG_BLE_BOND_CHOOSE:
		case MSG_UPDATE_FONT_LIB:
			StandbyTimeModifyEnable = true;
		case MSG_BC5_OFF_RESET	:
		case MSG_BC5_ON_RESET	:
		case MSG_CHARGE_STOP:	
		case MSG_LED_TEST:
		case MSG_NFC_TEST:
		case MSG_GET_CSN:
		case MSG_ANCS_SAVE_TO_FLASH:	
			return RET_MSG_UNHANDLED;

		case MSG_LEFTDOWN_LONGKEY_5S:				/*左键 长按7秒*/	
			PowerOffSystem();
			break;
			
		case MSG_LEFTDOWN_DOUBLE_SHORTKEY:		/*左键 双击*/
			if(DISPLAY_GOTO_SLEEP_FLAG == true)
				break;
		case MSG_SETUP_QUIT:	
			gDateTimeValid = 1;
			ClearInitMarkEvent();
			break;

		case MSG_SETUP_END:
			WRITE_USER_FLAG(&gInitMark,INIT_MARK_LOC,INIT_MARK_LEN);
			osal_stop_timerEx(GetAppTaskId(),MSG_SETUP_QUIT);
			UnloadHandler(DifferentRemindId.SetupAlertHandlerId);
			break;

		case MSG_DISPLAY:
			if(BleReconnectFlag)
			{
				FLP_TurnOnBle();
			}
			ShowSetUp();
			StandbyTimeModifyEnable = true;
			SetSystemSuspendTime(DISPLAY_TIME_30S_FLAG);
			StandbyTimeModifyEnable = false;
			break;


		case MSG_CHARGE_START:		
#if 0			
			if(Powerstate.chargepowerstate==3)
				return RET_MSG_UNHANDLED;
			else
			{
				StartSecondEvent();
				Powerstate.chargeinterface = true;	
				SendMsg(MSG_DISPLAY);	          
				break;
			}	
#else
			return RET_MSG_UNHANDLED;
#endif
		case MSG_DISPLAY_BATTERY_STATE:	     
			if(ChargeTime <= BATTERY_FULL_SENCOND)
				ChargeTime++;	  
			break;

		default:
			break;
	}
	return RET_MSG_HANDLED;
}

uint16 SerialNumHandler(MsgType msg, int iParam, void *pContext)
{
    switch(msg)
    {
		case MSG_HANDLER_BEGIN:
			DifferentRemindId.SerialNumHandlerId = iParam;
			osal_start_timerEx(GetAppTaskId(), MSG_SERIALNUM_TIMEOUT, 10000);
			break;

		case MSG_HANDLER_END:
			DifferentRemindId.SerialNumHandlerId = 0;
			break;
			
		case MSG_SERIALNUM_TIMEOUT:
			UnloadHandler(DifferentRemindId.SerialNumHandlerId);
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
		case MSG_TOUCH_UP_SHORTKEY:				/*触摸 单击*/  
		case MSG_TOUCH_UP_DOUBLE_SHORTKEY:		/*触摸 双击*/	
		case MSG_TOUCH_LONGKEY:					/*触摸 长按*/
			break;

		case MSG_RIGHTDOWN_SHORTKEY: 				/*右键 单击*/	
			UnloadHandler(DifferentRemindId.SerialNumHandlerId);
			break;
			
		case MSG_DISPLAY:
			ShowSerialNum();
			break;
				
		default:
			return RET_MSG_UNHANDLED;
    }
    return RET_MSG_HANDLED;
}


/*亮度调节Handler*/
uint16 BrightnessHandler(MsgType msg, int iParam, void *pContext)
{
    switch(msg)
    {
		case MSG_HANDLER_BEGIN:
			DifferentRemindId.BrightnessHandlerId = iParam;
			break;

		case MSG_HANDLER_END:
			DifferentRemindId.BrightnessHandlerId = 0;
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
		case MSG_TOUCH_UP_DOUBLE_SHORTKEY:		/*触摸 双击*/	
		case MSG_TOUCH_LONGKEY:					/*触摸 长按*/
			break;
			
		case MSG_TOUCH_DOWN_UP_FLICK:				/*左*/ 
			if (shortcut_info.brightness_level >= 4)
			shortcut_info.brightness_level -=2;		
			SendMsg(MSG_DISPLAY);
			break;
		case MSG_TOUCH_UP_DOWN_FLICK: 			/*右*/
			if (shortcut_info.brightness_level <= 8)
				shortcut_info.brightness_level +=2;	
			SendMsg(MSG_DISPLAY);		
			break;

		case MSG_RIGHTDOWN_SHORTKEY: 				/*右键 单击*/	
			UnloadHandler(DifferentRemindId.BrightnessHandlerId);
			break;
		case MSG_TOUCH_UP_SHORTKEY:				/*触摸 单击*/ 
			{
				uint8 TouchID = Get_Screen_Locaton(3,3);
				if(TouchID==8)	//增加亮度
				{
					if (shortcut_info.brightness_level <= 8)
					shortcut_info.brightness_level +=2;	
					SendMsg(MSG_DISPLAY);				
				}
				else if(TouchID==6)	//减少亮度
				{
					if (shortcut_info.brightness_level >= 4)
					shortcut_info.brightness_level -=2;		
					SendMsg(MSG_DISPLAY);				
				}
			}
			break;

		case MSG_DATE_TIME_FORMAT_UPDATE:	 	
		case MSG_DISPLAY:
			ShowBrightness(1, 1);
			break;
				
		default:
			return RET_MSG_UNHANDLED;
    }
    return RET_MSG_HANDLED;
}


/*时间界面上拉Handler*/
uint16 ShortCutHandler(MsgType msg, int iParam, void *pContext)
{
	switch(msg)
	{
		case MSG_HANDLER_BEGIN:
			DifferentRemindId.ShortCutHandlerId = iParam;
			break;

		case MSG_HANDLER_END:
			DifferentRemindId.ShortCutHandlerId = 0;
			break;
/*用户操作*/
		case MSG_LEFTDOWN_SHORTKEY:				/*左键 单击*/	
		case MSG_LEFTDOWN_DOUBLE_SHORTKEY:		/*左键 双击*/
		case MSG_LEFTDOWN_LONGKEY:           			/*左键 长按*/		
		case MSG_RIGHTDOWN_DOUBLE_SHORTKEY:   	/*右键 双击*/	
		case MSG_RIGHTDOWN_LONGKEY:				/*右键 长按*/	
		case MSG_RIGHTDOWN_LONGKEY_UP:			/*右键 长按松开*/		
		case MSG_TOUCH_DOWN_FLICK:				/*下*/
		case MSG_TOUCH_DOWN_UP_FLICK:				/*左*/ 
		case MSG_TOUCH_UP_DOWN_FLICK: 			/*右*/	 
		case MSG_TOUCH_UP_DOUBLE_SHORTKEY:		/*触摸 双击*/	
		case MSG_TOUCH_LONGKEY:					/*触摸 长按*/
		case MSG_BC5_STATE_CHANGE:
			break;
			
		case MSG_RIGHTDOWN_SHORTKEY: 				/*右键 单击*/
		case MSG_TOUCH_UP_FLICK:					/*上*/
			UnloadHandler(DifferentRemindId.ShortCutHandlerId );
			break;

		case MSG_TOUCH_UP_SHORTKEY:				/*触摸 单击*/ 	
		{
			uint8 TouchID = Get_Screen_Locaton(3,3);
			switch(TouchID)
			{
				case 0:
				case 1:
				case 2:
					break;

				case 3:
					if (shortcut_info.airplane_mode == 0)
					{
						shortcut_info.airplane_mode = 1;
					}
					else {
						shortcut_info.airplane_mode = 0;
					}
					BleDisable = 1;		
					break;

				case 4:
					if (shortcut_info.vibration_level== 0)
					{
						shortcut_info.vibration_level= 1;
						MotoRun(MOTO_DEFAULT);
					}
					else
					{
						shortcut_info.vibration_level= 0;
					}
					break;

				case 5:
					LoadHandler(BrightnessHandler,0);
					break;

				case 6:
					break;

				case 7:
					//shortcut_info.find_me = !shortcut_info.find_me;
					LoadHandler(SearchHandler,0);
					break;

				case 8:
					break;

				default:
					return RET_MSG_HANDLED;
			}
			SendMsg(MSG_DISPLAY);
			break;
		}		
		case MSG_DISPLAY:
			ShowShortCut(1, true);
			break;

		default:
			return RET_MSG_UNHANDLED;
	}
	return RET_MSG_HANDLED;
}

/*抬手手势开关Handler*/
uint16 WristWakeUpHandler(MsgType msg, int iParam, void *pContext)
{
	switch(msg)
	{
		case MSG_HANDLER_BEGIN:
			DifferentRemindId.WristWakeUpHandlerId = iParam;	
			SetSystemSuspendTime(DISPLAY_TIME_10S_FLAG);
			break;

		case MSG_HANDLER_END:
			DifferentRemindId.WristWakeUpHandlerId = 0;
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
		case MSG_TOUCH_UP_DOUBLE_SHORTKEY:		/*触摸 双击*/	
		case MSG_TOUCH_LONGKEY:					/*触摸 长按*/
			break;
			
		case MSG_RIGHTDOWN_SHORTKEY: 				/*右键 单击*/	
			UnloadHandler(DifferentRemindId.WristWakeUpHandlerId); 
			break;

		case MSG_TOUCH_UP_SHORTKEY:				/*触摸 单击*/  	
			{
				uint8 TouchID = Get_Screen_Locaton(3,1);
				if(TouchID==0)
				{
					if(!setting_info.wrist)
					{
						setting_info.wrist =1;
						InitGesture();
						WRITE_USER_PARAMETER((uint8 *)&setting_info, SETTING_LOC, SETTING_LEN);
						//ShowWristStateChange(setting_info.wrist);
						ShowWristOnApps();
					}
				}		
				else if(TouchID==1)	
				{
					if(setting_info.wrist)
					{
						setting_info.wrist =0;
						WRITE_USER_PARAMETER((uint8 *)&setting_info, SETTING_LOC, SETTING_LEN);
						//ShowWristStateChange(setting_info.wrist);
						ShowWristOffApps();
					}					
				}
			}		
			break;
			
		case MSG_DISPLAY:	
			if (setting_info.wrist)
				ShowWristOnApps();
			else 
				ShowWristOffApps();
			break;
					
		default:
			return RET_MSG_UNHANDLED;
	}
	return RET_MSG_HANDLED;
}

/*WatchId 查看Handler*/
uint16 WatchInfoHandler(MsgType msg, int iParam, void *pContext)
{
	switch(msg)
	{
		case MSG_HANDLER_BEGIN:
			DifferentRemindId.WatchInfoHandlerId = iParam;	
			SetSystemSuspendTime(DISPLAY_TIME_10S_FLAG);
			break;

		case MSG_HANDLER_END:
			DifferentRemindId.WatchInfoHandlerId = 0;
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
		case MSG_TOUCH_UP_SHORTKEY:				/*触摸 单击*/ 
		case MSG_TOUCH_UP_DOUBLE_SHORTKEY:
		case MSG_TOUCH_LONGKEY:					/*触摸 长按*/
			break;

		case MSG_RIGHTDOWN_SHORTKEY: 				/*右键 单击*/	
			UnloadHandler(DifferentRemindId.WatchInfoHandlerId); 
			break;

		case MSG_DISPLAY:	
			ShowWatchInfo();
			break;
					
		default:
			return RET_MSG_UNHANDLED;
	}
	return RET_MSG_HANDLED;
}


uint16 OtaHandler(MsgType msg, int iParam, void *pContext)
{
	switch(msg)
	{
		case MSG_HANDLER_BEGIN:
			DifferentRemindId.OtaHandlerId = iParam;
			break;

		case MSG_HANDLER_END:
			DifferentRemindId.OtaHandlerId = 0;
			break;

		case MSG_DISPLAY:
			OtaProgMsg();
			break;

		default:
			return RET_MSG_HANDLED;
	}
	return RET_MSG_HANDLED;
}

uint16 Show_Index =0;
uint16 PhoneBookHandler(MsgType msg, int iParam, void *pContext)
{
	uint8 TouchID = 0;
	uint8 ChooseIndex =0; 
	uint32 Choose_addr = 0;
	uint16 ChsGrpIndex =0xffff; 
	uint8 Percent_Showed = 0;
	switch(msg)
	{
		case MSG_HANDLER_BEGIN:
			Show_Index =0;
			DifferentRemindId.PhoneHandlerId = iParam;
			SetSystemSuspendTime(DISPLAY_TIME_10S_FLAG);
			break;

		case MSG_HANDLER_END:
			Show_Index =0;
			DifferentRemindId.PhoneHandlerId = 0;
			break;
/*用户操作*/
		case MSG_LEFTDOWN_SHORTKEY:				/*左键 单击*/	
		case MSG_LEFTDOWN_DOUBLE_SHORTKEY:		/*左键 双击*/
		case MSG_LEFTDOWN_LONGKEY:           			/*左键 长按*/
		case MSG_RIGHTDOWN_DOUBLE_SHORTKEY:   	/*右键 双击*/	
		case MSG_RIGHTDOWN_LONGKEY:				/*右键 长按*/	
		case MSG_RIGHTDOWN_LONGKEY_UP:			/*右键 长按松开*/		
		case MSG_TOUCH_DOWN_UP_FLICK:				/*左*/ 
		case MSG_TOUCH_UP_DOWN_FLICK: 			/*右*/
		case MSG_TOUCH_UP_DOUBLE_SHORTKEY:		/*触摸 双击*/	
		case MSG_TOUCH_LONGKEY:					/*触摸 长按*/
			break;
			
		case MSG_RIGHTDOWN_SHORTKEY: 				/*右键 单击*/	
			UnloadHandler(DifferentRemindId.PhoneHandlerId);
			break;
			
		case MSG_TOUCH_UP_SHORTKEY:				/*触摸 单击*/ 			
			TouchID = Get_Screen_Locaton(9,6);
			
			if((TouchID>=0x06)&&(TouchID<=0x17))
			{
				if(TouchID%0x06 != 0 && TouchID%0x06 != 5)
				{
					ChooseIndex = 0;
					if(GetChooseInfNum(&Choose_addr,ChooseIndex))
						LoadPhonePrepareHandler(Choose_addr, 0, 0, NULL,Android_ios);
					break;
				}
				
			}
			else if((TouchID>=0x18)&&(TouchID<=0x29))
			{
				if(TouchID%0x06 != 0 && TouchID%0x06 != 5)
				{
					
					ChooseIndex =  1;
					if(GetChooseInfNum(&Choose_addr,ChooseIndex))
						LoadPhonePrepareHandler(Choose_addr, 0, 0, NULL,Android_ios);
					break;
				}
				
			}
			else if((TouchID>=0x2A)&&(TouchID<=0x3B))
			{
				if(TouchID%0x06 != 0 && TouchID%0x06 != 5)
				{
					ChooseIndex = 2;
					if(GetChooseInfNum(&Choose_addr,ChooseIndex))
						LoadPhonePrepareHandler(Choose_addr, 0, 0, NULL,Android_ios);
					break;
				}
			}
						
			if(TouchID == 0x0B ||TouchID == 0x11)
				ChsGrpIndex = 0;
			else if(TouchID == 0x17 ||TouchID == 0x1d)
				ChsGrpIndex = 1;
			else if(TouchID == 0x23)
				ChsGrpIndex = 2;
			else if(TouchID == 0x29)
				ChsGrpIndex = 3;
			else if(TouchID == 0x2f)
				ChsGrpIndex = 4;
			else if(TouchID == 0x35)
				ChsGrpIndex = 5;
			else if(TouchID == 0x3b)
				ChsGrpIndex = 6;
			if(ChsGrpIndex != 0xffff)
			{
				if(CheckpCurrentHandler(DifferentRemindId.PhoneHandlerId))
				{
					if(!ReadChosePhoneBook_ToKl17(ChsGrpIndex,&Show_Index))
						break;
				}
			}					
			break;
			
		case MSG_TOUCH_UP_FLICK:				/*上*/
				Show_Index += 3;
			if(Show_Index > GetInfMaxCnt())
				Show_Index = Show_Index - 3;
			Percent_Showed = ShowedCntPercent(Show_Index);
			if(CheckpCurrentHandler(DifferentRemindId.PhoneHandlerId))
			{
				if(!ReadPhoneBook_ToKl17(Percent_Showed,Show_Index))
					break;
			}
			break;	
			
		case MSG_TOUCH_DOWN_FLICK:			/*下*/			
			if(Show_Index < 3)
				Show_Index = 0;	
			else
				Show_Index -= 3;
			Percent_Showed = ShowedCntPercent(Show_Index + 3);
			if(CheckpCurrentHandler(DifferentRemindId.PhoneHandlerId))
			{
				if(!ReadPhoneBook_ToKl17(Percent_Showed,Show_Index))
				break;
			}			
			break;
		case MSG_DISPLAY:
			Percent_Showed = ShowedCntPercent(Show_Index + 3);
			if(!ReadPhoneBook_ToKl17(Percent_Showed,Show_Index))
				UnloadHandler(DifferentRemindId.PhoneHandlerId);
			break;
		case MSG_BIND_OK:
			UnloadHandler(DifferentRemindId.PhoneHandlerId);
			break;
		
		default:
			return RET_MSG_UNHANDLED;
	}
	return RET_MSG_HANDLED;
}


uint16 ClearAllHandler(MsgType msg, int iParam, void *pContext)
{	
	switch(msg)
	{
		case MSG_HANDLER_BEGIN:
			DifferentRemindId.ClearAllHandlerId = iParam;
			SetSystemSuspendTime(DISPLAY_TIME_10S_FLAG);
			return RET_MSG_HANDLED;

		case MSG_HANDLER_END:
			DifferentRemindId.ClearAllHandlerId = 0;
			return RET_MSG_HANDLED;
			
		case MSG_TOUCH_DOWN_UP_FLICK:		/*?*/ 		
		case MSG_TOUCH_UP_DOWN_FLICK:		/*?*/
		case MSG_TOUCH_UP_FLICK:			/*?*/
		case MSG_TOUCH_DOWN_FLICK:			/*?*/			
		case MSG_TOUCH_LONGKEY: 			/*??*/	
			return RET_MSG_HANDLED;	
			
		case MSG_TOUCH_UP_SHORTKEY: 	/*??*/	
			{
				uint8 TouchID = Get_Screen_Locaton(3,2);
				if(TouchID==4)
				{
					if(ClearAllType == Clear_MissedCall)
					{
						ClearAncs_MissedCall();
					}
					else if(ClearAllType == Clear_Sms)
					{
						ClearAncs_Sms();
					}
					else if(ClearAllType == Clear_Socail)
					{
						ClearAncs_Social();
					}
					else if(ClearAllType == Clear_Email)
					{
						ClearAncs_Email();
					}
					else if(ClearAllType == Clear_PhoneRecord)
					{
						ClearAncs_PhoneRecord();
					}
					UnloadHandler(DifferentRemindId.ClearAllHandlerId);
				}
				else if(TouchID==5)
				{
					UnloadHandler(DifferentRemindId.ClearAllHandlerId);
				}
			}	
			return RET_MSG_HANDLED;
			
		case MSG_LEFTDOWN_SHORTKEY:				/*左键 单击*/	
		case MSG_LEFTDOWN_DOUBLE_SHORTKEY:		/*左键 双击*/
		case MSG_LEFTDOWN_LONGKEY:           			/*左键 长按*/
		case MSG_RIGHTDOWN_DOUBLE_SHORTKEY:   	/*右键 双击*/	
		case MSG_RIGHTDOWN_LONGKEY:				/*右键 长按*/	
		case MSG_RIGHTDOWN_LONGKEY_UP:			/*右键 长按松开*/	
			return RET_MSG_HANDLED;	
			
		case MSG_RIGHTDOWN_SHORTKEY: 				/*右键 单击*/	
			UnloadHandler(DifferentRemindId.ClearAllHandlerId);
			return RET_MSG_HANDLED;	
			
		case MSG_TOUCH_UP_DOUBLE_SHORTKEY:
		case MSG_TOUCH_DOWN_DOUBLE_SHORTKEY:	
			if (DISPLAY_GOTO_SLEEP_FLAG)
			{
				break;
			}
			else
			{
				return RET_MSG_HANDLED;	
			}
		case MSG_DISPLAY:
			ShowClearAll();
			return RET_MSG_HANDLED;
		case MSG_CLEAR_ALL:
			UnloadHandler(DifferentRemindId.ClearAllHandlerId);
			SendMsg(MSG_CLEAR_ALL);
			return RET_MSG_HANDLED;	
		default:
			break;
	}
	return RET_MSG_UNHANDLED;	
}

uint16 Mmenu_Index =1;
//一级界面
uint16 SportMmenuHandler(MsgType msg, int iParam, void *pContext)
{
	switch(msg)
	{
		case MSG_HANDLER_BEGIN:
			DifferentRemindId.SportMmenuHandlerId = iParam;
			SetSystemSuspendTime(DISPLAY_TIME_10S_FLAG);
			break;

		case MSG_HANDLER_END:
			DifferentRemindId.SportMmenuHandlerId = 0;
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
		case MSG_TOUCH_UP_DOUBLE_SHORTKEY:		/*触摸 双击*/	
		case MSG_TOUCH_LONGKEY:					/*触摸 长按*/
			break;

		case MSG_TOUCH_DOWN_UP_FLICK:				/*左*/ 
			LoadMenuHandler(MENU_FORWARD);
			break;
			
		case MSG_RIGHTDOWN_SHORTKEY: 				/*右键 单击*/
			LoadMenuHandler(MENU_JUMP_TIME_HANDLER);
			break;
			
		case MSG_TOUCH_UP_DOWN_FLICK: 			/*右*/
			LoadMenuHandler(MENU_BACKWARD); 
			break;

		case MSG_TOUCH_UP_SHORTKEY:				/*触摸 单击*/  
			{
				LoadHandler(SportOverviewHandler, 0);
			}				
			break;
		case MSG_DATE_TIME_FORMAT_UPDATE:
		case MSG_DISPLAY:
			Mmenu_Index=1;
			ShowMainMenu(Mmenu_Index);
			break;

		default:
			return RET_MSG_UNHANDLED;	
	}
	return RET_MSG_HANDLED;
}

//一级界面
uint16 HRMmenuHandler(MsgType msg, int iParam, void *pContext)
{
	switch(msg)
	{
		case MSG_HANDLER_BEGIN:
			DifferentRemindId.HRMmenuHandlerId = iParam;
			SetSystemSuspendTime(DISPLAY_TIME_10S_FLAG);
			break;

		case MSG_HANDLER_END:
			DifferentRemindId.HRMmenuHandlerId = 0;
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
		case MSG_TOUCH_UP_DOUBLE_SHORTKEY:		/*触摸 双击*/	
		case MSG_TOUCH_LONGKEY:					/*触摸 长按*/
			break;

		case MSG_TOUCH_DOWN_UP_FLICK:				/*左*/ 
			LoadMenuHandler(MENU_FORWARD);
			break;
			
		case MSG_RIGHTDOWN_SHORTKEY: 				/*右键 单击*/
			LoadMenuHandler(MENU_JUMP_TIME_HANDLER);
			break;
			
		case MSG_TOUCH_UP_DOWN_FLICK: 			/*右*/
			LoadMenuHandler(MENU_BACKWARD); 
			break;

		case MSG_TOUCH_UP_SHORTKEY:				/*触摸 单击*/  
			{
				LoadHandler(heartRate_Handler, 0);
			}				
			break;
		case MSG_DATE_TIME_FORMAT_UPDATE:
		case MSG_DISPLAY:
			Mmenu_Index=2;
			ShowMainMenu(Mmenu_Index);
			break;

		default:
			return RET_MSG_UNHANDLED;	
	}
	return RET_MSG_HANDLED;
}


RecordWork Rw =Rw_Ready; 
void StartRecordWork(void)
{
	TotalHR=0;
	CountHR=0;
	RwSec =0;
	RwMSec=0;
	RwMMinute =0;
	gRecordSysTick_start=pS_OsGetSysTick();
	uStepsSave=gSportDisplay.SportSteps;
	uDistanceSave=gSportDisplay.SportDistance;
	uCalorSave=gSportDisplay.SportKcal;
	uTimeSave = GetTimeStamp();
	RECORDWORK_SAVE_FLAG =false;
	osal_set_event(GetHrTaskId(), HR_OPEN_EVENT);
	osal_start_timerEx(GetAppTaskId(), MSG_RECORD_WORK_1S, 1000);
	osal_start_timerEx(GetAppTaskId(), MSG_RECORD_WORK_100MS, 1000);
	//osal_set_event(GetClockTaskId(), STOP_WATCH_CNT_EVENT);
	//osal_start_timerEx(GetAppTaskId(), STOP_WATCH_CNT_EVENT, 1000);
}

void SaveRecordWork(void)
{
	uint8 HR_val;
	if(CountHR !=0)
	{
		HR_val=(uint8)(TotalHR/CountHR);
	}
	else
	{
		HR_val=0;
	}	
	REC_WORK_DATA_T RwD;
	RwD.TimeStamp =uTimeSave;  
	RwD.Steps =uStepsResult;
	RwD.Kcal = uCalorResult;
	RwD.Distance =uDistanceResult;
	RwD.Duration = RwMMinute;
	RwD.Hr =(uint32)HR_val;
	if(RwD.Steps>0||RwD.Hr>0)
	{
		if(gUploadRecordWorkState!=UPLOAD_PREPARE && gUploadRecordWorkState!=UPLOADING)
		{
			RECORDWORK_SAVE_FLAG=false;
			Force_NordicFlash();	
			StoreRecordWorkData(&RecordWork_Store_Logs,&RwD);
			RecordWork_Store_Logs++;
			WRITE_STEP_SLEEP_SUM(&RecordWork_Store_Logs,RECORD_WORK_LOC,RECORD_WORK_LEN);	
			Free_NordicFlash();
		}
		else
		{
			RECORDWORK_SAVE_FLAG=true;
		}
			
	}
}
void StopRecordWork(void)
{
	osal_stop_timerEx(GetAppTaskId(), MSG_RECORD_WORK_1S);
	osal_stop_timerEx(GetAppTaskId(), MSG_RECORD_WORK_100MS);
	osal_set_event(GetHrTaskId(), HR_CLOSE_EVENT); 
	uStepsResult= gSportDisplay.SportSteps-uStepsSave;
	uDistanceResult=gSportDisplay.SportDistance-uDistanceSave;
	uCalorResult=gSportDisplay.SportKcal-uCalorSave;
	RwMMinute = (RwMSec/1000)/60;
	//osal_set_event(GetAppTaskId(), STOP_WATCH_CNT_EVENT);
	SaveRecordWork();
}

uint8 RwIndex=0;
uint16 RecordWorkDetailHandler(MsgType msg, int iParam, void *pContext)
{	
	uint8 HR_val;
	switch(msg)
	{
		case MSG_HANDLER_BEGIN:
			DifferentRemindId.RecordWorkDetailHandlerId= iParam;
			SetSystemSuspendTime(DISPLAY_TIME_10S_FLAG);
			RwIndex=0;
			break;
		case MSG_HANDLER_END:
			DifferentRemindId.RecordWorkDetailHandlerId = 0;
			break;		
/*用户操作*/
		case MSG_LEFTDOWN_SHORTKEY:				/*左键 单击*/	
		case MSG_LEFTDOWN_DOUBLE_SHORTKEY:		/*左键 双击*/
		case MSG_LEFTDOWN_LONGKEY:           			/*左键 长按*/		
		case MSG_RIGHTDOWN_DOUBLE_SHORTKEY:   	/*右键 双击*/	
		case MSG_RIGHTDOWN_LONGKEY:				/*右键 长按*/	
		case MSG_RIGHTDOWN_LONGKEY_UP:			/*右键 长按松开*/	
		case MSG_TOUCH_DOWN_UP_FLICK:				/*左*/ 	
		case MSG_TOUCH_UP_DOWN_FLICK: 			/*右*/
		case MSG_TOUCH_UP_DOUBLE_SHORTKEY:		/*触摸 双击*/	
		case MSG_TOUCH_LONGKEY:					/*触摸 长按*/
			break;
			
		case MSG_RIGHTDOWN_SHORTKEY: 				/*右键 单击*/
			UnloadHandler(DifferentRemindId.RecordWorkDetailHandlerId);
			break;
			
		case MSG_TOUCH_UP_FLICK:					/*上*/
			if(RwIndex==0)
			{
				RwIndex=1;
				SendMsg(MSG_DISPLAY);
			}
			break;
			
		case MSG_TOUCH_DOWN_FLICK:				/*下*/ 	
			if(RwIndex==1)
			{
				RwIndex=0;
				SendMsg(MSG_DISPLAY);
			}
			break;
			
		case MSG_TOUCH_UP_SHORTKEY:				/*触摸 单击*/  
			{
				uint8 TouchID = Get_Screen_Locaton(3,1);
				switch(TouchID)
				{
					case 0:
					case 1:
						break;
					case 2:
						if(RwIndex==1)
						{
							UnloadHandler(DifferentRemindId.RecordWorkDetailHandlerId);
						}
						break;
					default:
						break;	
				}
			}	
			break;		
		case MSG_DATE_TIME_FORMAT_UPDATE:	
		case MSG_DISPLAY:
			if(CountHR !=0)
			{
				HR_val=(uint8)(TotalHR/CountHR);
			}
			else
			{
				HR_val=0;
			}			
			ShowRecordResult(RwIndex, SetValue.DistanceFormat,HR_val,uStepsResult,uDistanceResult,uCalorResult,RwMMinute);
			break;	
			
		default:
			return RET_MSG_UNHANDLED;										
	}
	return RET_MSG_HANDLED;
}

uint16 RecordWorkHandler(MsgType msg, int iParam, void *pContext)
{
	switch(msg)
	{
		case MSG_HANDLER_BEGIN:
			DifferentRemindId.RecordWorkHandlerId= iParam;
			SetSystemSuspendTime(DISPLAY_TIME_10S_FLAG);
			Rw = Rw_Ready;
			IsLeftRight = 0;
			break;
		case MSG_HANDLER_END:
			StopRecordWork();
			DifferentRemindId.RecordWorkHandlerId = 0;
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
		case MSG_TOUCH_UP_DOUBLE_SHORTKEY:		/*触摸 双击*/	
		case MSG_TOUCH_LONGKEY:					/*触摸 长按*/
			break;
		case MSG_UNLOADRECORDWORKOUTS:	
			UnloadHandler(DifferentRemindId.RecordWorkHandlerId);
			break;
			
		case MSG_RIGHTDOWN_SHORTKEY: 				/*右键 单击*/
			if(Rw == Rw_TimerRun)
				LoadHandler(ExitRecordWorkOutsHandler,0);
			else
				UnloadHandler(DifferentRemindId.RecordWorkHandlerId);
			break;
		
		case MSG_FORCE_BACK_HOME: 
			if(Rw == Rw_TimerRun)
			{
				LoadHandler(ExitRecordWorkOutsHandler,0);
				IsLeftRight = 1;
				break;
			}
			else
				return RET_MSG_UNHANDLED;
			
		case MSG_TOUCH_UP_SHORTKEY_PRESS:					/*触摸 单击 按下*/
			if(CheckpCurrentHandler(DifferentRemindId.RecordWorkHandlerId)&&DISPLAY_GOTO_SLEEP_FLAG==false)
			{
				uint8 TouchID = Get_Screen_Locaton(3,3);
				switch(TouchID)
				{
					case 0:
					case 1:
					case 2:
					case 3:		
					case 4:
					case 5:
						break;
					case 6:	
						if(Rw==Rw_TimerStop)
						{
							Rw=Rw_go;
							StartRecordWork();
							SendMsg(MSG_DISPLAY);
						}
						break;						
					case 7:
						if(Rw==Rw_Ready)
						{
							Rw=Rw_go;
							StartRecordWork();
							SendMsg(MSG_DISPLAY);
						}												
						else if(Rw==Rw_TimerRun)
						{
							Rw=Rw_TimerStop;
							StopRecordWork();
							SendMsg(MSG_DISPLAY);
						}							
						break;
					case 8:		
						break;
					default:
						break;                        
				}			
			}				
			break;	
			
		case MSG_TOUCH_UP_SHORTKEY:				/*触摸 单击*/  
			{
				uint8 TouchID = Get_Screen_Locaton(3,3);
				switch(TouchID)
				{
					case 0:
					case 1:
					case 2:
					case 3:		
					case 4:
					case 5:
					case 6:	
					case 7:	
						break;
					case 8:		
						if(Rw==Rw_TimerStop)
						{
							LoadHandler(RecordWorkDetailHandler, 0);
						}
						break;
					default:
						break;                        
				}			
			}				
			break;	
			
		case MSG_RECORD_WORK_1S:
			Rw=Rw_TimerRun;
			RwSec++;
			if(HrValueValid())
			{
				CountHR++;
				TotalHR+=HrGetValue();				
			}			
			osal_start_timerEx(GetAppTaskId(), MSG_RECORD_WORK_1S, 1000);
			break;
			
		case MSG_RECORD_WORK_100MS:
			//RwMSec =gStopWatchLastMs ;
			RwMSec = (pS_OsGetSysTick()-gRecordSysTick_start);
			//SendMsg(MSG_DISPLAY);		
			if(Rw==Rw_TimerRun)
				osal_start_timerEx(GetAppTaskId(), MSG_RECORD_WORK_100MS, 100);
			else
				break;
			if(!CheckpCurrentHandler(DifferentRemindId.RecordWorkHandlerId)||(DISPLAY_GOTO_SLEEP_FLAG))
				break;
		case MSG_DATE_TIME_FORMAT_UPDATE:
		case MSG_DISPLAY:
			switch(Rw)
			{
				case Rw_Ready:
					ShowRecordWorkoutsRUN(0,0,0,0);
					break;
				case Rw_go:
					ShowRecordgo();
					break;
				case Rw_TimerRun:
					ShowRecordWorkoutsRUN(1,RwMSec/1000/60,RwMSec/1000%60,RwMSec%1000/100);
					break;
				case Rw_TimerStop:
					ShowRecordWorkoutsRUN(2,RwMSec/1000/60,RwMSec/1000%60,RwMSec%1000/100);
					break;
			}
			break;

		default:
			return RET_MSG_UNHANDLED;										
	}
	return RET_MSG_HANDLED;
}

//一级界面
uint16 RecordWorkoutsMmenuHandler(MsgType msg, int iParam, void *pContext)
{
	switch(msg)
	{
		case MSG_HANDLER_BEGIN:
			DifferentRemindId.RecordWorkoutsMmenuHandlerId = iParam;
			SetSystemSuspendTime(DISPLAY_TIME_10S_FLAG);
			break;

		case MSG_HANDLER_END:
			DifferentRemindId.RecordWorkoutsMmenuHandlerId = 0;
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
		case MSG_TOUCH_UP_DOUBLE_SHORTKEY:		/*触摸 双击*/	
		case MSG_TOUCH_LONGKEY:					/*触摸 长按*/
			break;

		case MSG_TOUCH_DOWN_UP_FLICK:				/*左*/ 
			LoadMenuHandler(MENU_FORWARD);
			break;
			
		case MSG_RIGHTDOWN_SHORTKEY: 				/*右键 单击*/
			LoadMenuHandler(MENU_JUMP_TIME_HANDLER);
			break;
			
		case MSG_TOUCH_UP_DOWN_FLICK: 			/*右*/
			LoadMenuHandler(MENU_BACKWARD); 
			break;

		case MSG_TOUCH_UP_SHORTKEY:				/*触摸 单击*/  
			{
				LoadHandler(RecordWorkHandler, 0);
			}				
			break;
		case MSG_DATE_TIME_FORMAT_UPDATE:
		case MSG_DISPLAY:
			Mmenu_Index=3;
			ShowMainMenu(Mmenu_Index);
			break;

		default:
			return RET_MSG_UNHANDLED;	
	}
	return RET_MSG_HANDLED;
}

//一级界面
uint16 SleepMmenuHandler(MsgType msg, int iParam, void *pContext)
{
	switch(msg)
	{
		case MSG_HANDLER_BEGIN:
			DifferentRemindId.SleepMmenuHandlerId = iParam;
			SetSystemSuspendTime(DISPLAY_TIME_10S_FLAG);
			break;

		case MSG_HANDLER_END:
			DifferentRemindId.SleepMmenuHandlerId = 0;
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
		case MSG_TOUCH_UP_DOUBLE_SHORTKEY:		/*触摸 双击*/	
		case MSG_TOUCH_LONGKEY:					/*触摸 长按*/
			break;

		case MSG_TOUCH_DOWN_UP_FLICK:				/*左*/ 
			LoadMenuHandler(MENU_FORWARD);
			break;
			
		case MSG_RIGHTDOWN_SHORTKEY: 				/*右键 单击*/
			LoadMenuHandler(MENU_JUMP_TIME_HANDLER);
			break;
			
		case MSG_TOUCH_UP_DOWN_FLICK: 			/*右*/
			LoadMenuHandler(MENU_BACKWARD); 
			break;

		case MSG_TOUCH_UP_SHORTKEY:				/*触摸 单击*/  
			{
				LoadHandler(SleepMonitor_Handler, 0);
			}				
			break;
		case MSG_DATE_TIME_FORMAT_UPDATE:
		case MSG_DISPLAY:
			Mmenu_Index=4;
			ShowMainMenu(Mmenu_Index);
			break;

		default:
			return RET_MSG_UNHANDLED;	
	}
	return RET_MSG_HANDLED;
}

//一级界面
uint16 CameraMmenuHandler(MsgType msg, int iParam, void *pContext)
{
	switch(msg)
	{
		case MSG_HANDLER_BEGIN:
			DifferentRemindId.CameraMmenuHandlerId = iParam;
			SetSystemSuspendTime(DISPLAY_TIME_10S_FLAG);
			break;

		case MSG_HANDLER_END:
			DifferentRemindId.CameraMmenuHandlerId = 0;
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
		case MSG_TOUCH_UP_DOUBLE_SHORTKEY:		/*触摸 双击*/	
		case MSG_TOUCH_LONGKEY:					/*触摸 长按*/
			break;

		case MSG_TOUCH_DOWN_UP_FLICK:				/*左*/ 
			LoadMenuHandler(MENU_FORWARD);
			break;
			
		case MSG_RIGHTDOWN_SHORTKEY: 				/*右键 单击*/	
		case MSG_TOUCH_UP_DOWN_FLICK: 			/*右*/
			LoadMenuHandler(MENU_BACKWARD); 
			break;

		case MSG_TOUCH_UP_SHORTKEY:				/*触摸 单击*/  
			{
				//LoadHandler(CameraHandler, 0);
			}				
			break;

		case MSG_DISPLAY:
			Mmenu_Index=5;
			ShowMainMenu(Mmenu_Index);
			break;

		default:
			return RET_MSG_UNHANDLED;	
	}
	return RET_MSG_HANDLED;
}

//一级界面
uint16 MusicMmenuHandler(MsgType msg, int iParam, void *pContext)
{
	switch(msg)
	{
		case MSG_HANDLER_BEGIN:
			DifferentRemindId.MusicMmenuHandlerId = iParam;
			SetSystemSuspendTime(DISPLAY_TIME_10S_FLAG);
			break;

		case MSG_HANDLER_END:
			DifferentRemindId.MusicMmenuHandlerId = 0;
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
		case MSG_TOUCH_UP_DOUBLE_SHORTKEY:		/*触摸 双击*/	
		case MSG_TOUCH_LONGKEY:					/*触摸 长按*/
			break;

		case MSG_TOUCH_DOWN_UP_FLICK:				/*左*/ 
			LoadMenuHandler(MENU_FORWARD);
			break;
			
		case MSG_RIGHTDOWN_SHORTKEY: 				/*右键 单击*/	
		case MSG_TOUCH_UP_DOWN_FLICK: 			/*右*/
			LoadMenuHandler(MENU_BACKWARD); 
			break;

		case MSG_TOUCH_UP_SHORTKEY:				/*触摸 单击*/  
			{
				//LoadHandler(MusicHandler, 0);
			}				
			break;

		case MSG_DISPLAY:
			Mmenu_Index=6;
			ShowMainMenu(Mmenu_Index);
			break;

		default:
			return RET_MSG_UNHANDLED;	
	}
	return RET_MSG_HANDLED;
}


//一级界面
uint16 NotificationMmenuHandler(MsgType msg, int iParam, void *pContext)
{
	switch(msg)
	{
		case MSG_HANDLER_BEGIN:
			DifferentRemindId.NotificationMmenuHandlerId = iParam;
			SetSystemSuspendTime(DISPLAY_TIME_10S_FLAG);
			break;

		case MSG_HANDLER_END:
			DifferentRemindId.NotificationMmenuHandlerId = 0;
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
		case MSG_TOUCH_UP_DOUBLE_SHORTKEY:		/*触摸 双击*/	
		case MSG_TOUCH_LONGKEY:					/*触摸 长按*/
			break;

		case MSG_TOUCH_DOWN_UP_FLICK:				/*左*/ 
			LoadMenuHandler(MENU_FORWARD);
			break;
			
		case MSG_RIGHTDOWN_SHORTKEY: 				/*右键 单击*/
			LoadMenuHandler(MENU_JUMP_TIME_HANDLER);
			break;
			
		case MSG_TOUCH_UP_DOWN_FLICK: 			/*右*/
			LoadMenuHandler(MENU_BACKWARD); 
			break;

		case MSG_TOUCH_UP_SHORTKEY:				/*触摸 单击*/  
			{
				LoadHandler(NotificationBoxHandler, 0);
			}				
			break;
		case MSG_DATE_TIME_FORMAT_UPDATE:	
		case MSG_DISPLAY:
			Mmenu_Index=5;
			ShowMainMenu(Mmenu_Index);
			break;

		default:
			return RET_MSG_UNHANDLED;	
	}
	return RET_MSG_HANDLED;
}


//一级界面
uint16 AppsMmenuHandler(MsgType msg, int iParam, void *pContext)
{
	switch(msg)
	{
		case MSG_HANDLER_BEGIN:
			DifferentRemindId.AppsMmenuHandlerId = iParam;
			SetSystemSuspendTime(DISPLAY_TIME_10S_FLAG);
			break;

		case MSG_HANDLER_END:
			DifferentRemindId.AppsMmenuHandlerId = 0;
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
		case MSG_TOUCH_UP_DOUBLE_SHORTKEY:		/*触摸 双击*/	
		case MSG_TOUCH_LONGKEY:					/*触摸 长按*/
			break;

		case MSG_TOUCH_DOWN_UP_FLICK:				/*左*/ 
			LoadMenuHandler(MENU_FORWARD);
			break;
			
		case MSG_RIGHTDOWN_SHORTKEY: 				/*右键 单击*/
			LoadMenuHandler(MENU_JUMP_TIME_HANDLER);
			break;
			
		case MSG_TOUCH_UP_DOWN_FLICK: 			/*右*/
			LoadMenuHandler(MENU_BACKWARD); 
			break;

		case MSG_TOUCH_UP_SHORTKEY:				/*触摸 单击*/  
			{
				LoadHandler(AppsBoxHandler, 0);
			}				
			break;
		case MSG_DATE_TIME_FORMAT_UPDATE:	
		case MSG_DISPLAY:
			Mmenu_Index=6;
			ShowMainMenu(Mmenu_Index);
			break;

		default:
			return RET_MSG_UNHANDLED;	
	}
	return RET_MSG_HANDLED;
}

uint8 AppIndex=1;
uint16 AppsBoxHandler(MsgType msg, int iParam, void *pContext)
{
	switch(msg)
	{
		case MSG_HANDLER_BEGIN:
			AppIndex =1;
			DifferentRemindId.AppsBoxHanderId = iParam;
			SetSystemSuspendTime(DISPLAY_TIME_10S_FLAG);
			break;

		case MSG_HANDLER_END:
			DifferentRemindId.AppsBoxHanderId = 0;
			break;
/*用户操作*/
		case MSG_LEFTDOWN_SHORTKEY:				/*左键 单击*/	
		case MSG_LEFTDOWN_DOUBLE_SHORTKEY:		/*左键 双击*/
		case MSG_LEFTDOWN_LONGKEY:           			/*左键 长按*/
		case MSG_RIGHTDOWN_DOUBLE_SHORTKEY:   	/*右键 双击*/	
		case MSG_RIGHTDOWN_LONGKEY:				/*右键 长按*/	
		case MSG_RIGHTDOWN_LONGKEY_UP:			/*右键 长按松开*/		
		case MSG_TOUCH_DOWN_UP_FLICK:				/*左*/ 
		case MSG_TOUCH_UP_DOWN_FLICK: 			/*右*/		
		case MSG_TOUCH_UP_DOUBLE_SHORTKEY:		/*触摸 双击*/	
		case MSG_TOUCH_LONGKEY:					/*触摸 长按*/
			break;
		
			
		case MSG_RIGHTDOWN_SHORTKEY: 				/*右键 单击*/	
			UnloadHandler(DifferentRemindId.AppsBoxHanderId);
			break;

		case MSG_TOUCH_UP_FLICK:					/*上*/
			if(AppIndex<3)
			{
				AppIndex++;
				ShowAppsMenu(AppIndex);
			}
			break;

		case MSG_TOUCH_DOWN_FLICK:				/*下*/
			if(AppIndex>1)
			{
				AppIndex--;
				ShowAppsMenu(AppIndex); 	
			}
			break;

		case MSG_TOUCH_UP_SHORTKEY:				/*触摸 单击*/  
			{
				uint8 TouchID = Get_Screen_Locaton(3,1);
				switch(AppIndex)
				{
					case 1:
						if(TouchID==0)
						{
							LoadHandler(MusicHandler, 0);
						}
						else if(TouchID==1)
						{			
							LoadHandler(CameraHandler, 0);		
						}
						else if(TouchID==2)
						{	
							LoadHandler(TimerHandler, 0);			
						}						
						break;					
					case 2:
						if(TouchID==0)
						{
							//SearchHandler
								LoadHandler(SearchHandler,0);
						}
                        else if(TouchID==1)
						{
							if(CheckRemindNum())
								LoadHandler(RemindViewHandler,0);
							else
								LoadHandler(NoReminderHandler, 0);
						}
						else if(TouchID==2)
						{
							//setting mode
							LoadHandler(SettingHandler, 0);
						}
						break;
					case 3:
						if(TouchID==0)
						{
							//LoadHandler(SettingHandler, 0);
							//Brightness
							LoadHandler(BrightnessHandler,0);
						}
						else if(TouchID==1)
						{
							//motor mode
							LoadHandler(VirateHandler,0);
						}
						break;
				}
			}	
			break;
		
		case MSG_DISPLAY:
			ShowAppsMenu(AppIndex);
			break;

		default:
			return RET_MSG_UNHANDLED;
	}
	return RET_MSG_HANDLED;
}


uint8 NotificationIndex=1;
uint16 NotificationBoxHandler(MsgType msg, int iParam, void *pContext)
{
	switch(msg)
	{
		case MSG_HANDLER_BEGIN:
			NotificationIndex =1;
			DifferentRemindId.NotificationBoxHandlerId = iParam;
			SetSystemSuspendTime(DISPLAY_TIME_10S_FLAG);
			break;

		case MSG_HANDLER_END:
			DifferentRemindId.NotificationBoxHandlerId = 0;
			break;
/*用户操作*/
		case MSG_LEFTDOWN_SHORTKEY:				/*左键 单击*/	
		case MSG_LEFTDOWN_DOUBLE_SHORTKEY:		/*左键 双击*/
		case MSG_LEFTDOWN_LONGKEY:           			/*左键 长按*/
		case MSG_RIGHTDOWN_DOUBLE_SHORTKEY:   	/*右键 双击*/	
		case MSG_RIGHTDOWN_LONGKEY:				/*右键 长按*/	
		case MSG_RIGHTDOWN_LONGKEY_UP:			/*右键 长按松开*/		
		case MSG_TOUCH_DOWN_UP_FLICK:				/*左*/ 
		case MSG_TOUCH_UP_DOWN_FLICK: 			/*右*/		
		case MSG_TOUCH_UP_DOUBLE_SHORTKEY:		/*触摸 双击*/	
		case MSG_TOUCH_LONGKEY:					/*触摸 长按*/
			break;
		
			
		case MSG_RIGHTDOWN_SHORTKEY: 				/*右键 单击*/	
			UnloadHandler(DifferentRemindId.NotificationBoxHandlerId);
			break;

		case MSG_TOUCH_UP_FLICK:					/*上*/
			if(NotificationIndex<2)
			{
				NotificationIndex++;
				ShowNotificationMenu(NotificationIndex);
			}
			break;

		case MSG_TOUCH_DOWN_FLICK:				/*下*/
			if(NotificationIndex>1)
			{
				NotificationIndex--;
				ShowNotificationMenu(NotificationIndex); 	
			}
			break;

		case MSG_TOUCH_UP_SHORTKEY:				/*触摸 单击*/  
			{
				uint8 TouchID = Get_Screen_Locaton(3,1);
				switch(NotificationIndex)
				{
					case 1:
						if(TouchID==0)
						{
							if(GetAncsMsgLayOut(Ancs_MissedCall,NULL))
								LoadHandler(MissedCallAlertHandler, 0);
							else
								LoadHandler(NoNewNotificationHandler, 0);
						}
						else if(TouchID==1)
						{			
							if(GetAncsMsgLayOut(Ancs_Sms,NULL))
								LoadHandler(SMSHandler, 0);
							else
								LoadHandler(NoNewNotificationHandler, 0);
						}
						else if(TouchID==2)
						{	
							if(GetAncsMsgLayOut(Ancs_Email,NULL))
								LoadHandler(E_MailHandler, 0);
							else
								LoadHandler(NoNewNotificationHandler, 0);
						}						
						break;					
					case 2:
						if(TouchID==0)
						{
							LoadHandler(SocialClassHomeHandler, 0);
							//if(GetAncsMsgLayOut(Ancs_Social,NULL))
								//LoadHandler(SocialMessageHandler, 0);
							//else
								//LoadHandler(NoNewNotificationHandler, 0);
						}
						else if(TouchID==1)
						{			
							//DailySchedule
							if(GetAncsMsgLayOut(Ancs_Schedule,NULL))
								LoadHandler(DailyScheduleHandler, 0);
							else
								LoadHandler(NoNewNotificationHandler, 0);
						}	
						else if(TouchID==2)
						{			
							LoadHandler(DeleteAncsHandler, 0);
						}							
						break;
				}
			}	
			break;

		case MSG_DISPLAY:
			ShowNotificationMenu(NotificationIndex);
			break;

		default:
			return RET_MSG_UNHANDLED;
	}
	return RET_MSG_HANDLED;
}

/*重启Handler*/
bool IsRstartProgress = false;
uint16 RestartHandler(MsgType msg, int iParam, void *pContext)
{
	static uint8 update_level = 0;
	switch(msg)
	{
		case MSG_HANDLER_BEGIN:
			IsRstartProgress = false;
			DifferentRemindId.RestartHandlerId = iParam;
			SetSystemSuspendTime(DISPLAY_TIME_10S_FLAG);
			break;

		case MSG_HANDLER_END:
			IsRstartProgress = false;
			DifferentRemindId.RestartHandlerId = 0;
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
		case MSG_TOUCH_UP_DOUBLE_SHORTKEY:		/*触摸 双击*/	
		case MSG_TOUCH_LONGKEY:					/*触摸 长按*/
			break;
			
		case MSG_RIGHTDOWN_SHORTKEY: 				/*右键 单击*/
			UnloadHandler(DifferentRemindId.RestartHandlerId);
			break;
			
		case MSG_TOUCH_UP_SHORTKEY:				/*触摸 单击*/  
			{
				uint8 TouchID = Get_Screen_Locaton(3,3);
				
				if (update_level == 0)	//works only  in showReset display
				{
					if(TouchID==6)
					{
						osal_start_timerEx(GetAppTaskId(), MSG_RESTART_PROGRESS_UPDATE,10);
						IsRstartProgress = true;
					}
					else if(TouchID==8)
					{
						UnloadHandler(DifferentRemindId.RestartHandlerId);
					}	
				}
			}	
			break;

		case MSG_RESTART_PROGRESS_UPDATE:
			{
				update_level = 0;
				FLP_TurnOffBle();
				Force_NordicFlash();
				ResetBoard();
			}
			break;	
		case MSG_DATE_TIME_FORMAT_UPDATE:	
		case MSG_DISPLAY:
			ShowRestart();
			break;

		default:
			if(IsRstartProgress == false)
				return RET_MSG_UNHANDLED;
			else
				return RET_MSG_HANDLED;
	}
	return RET_MSG_HANDLED;
}

uint8 RemindViewIndex=1;
uint16 RemindViewHandler(MsgType msg, int iParam, void *pContext)
{
	static uint8 Pages = 0;
	static REMIND_DATA_T RDTableTemp[(REMINDER_MAX_CNT/3+1)*3]={0x00};
	
	switch(msg)
	{
		case MSG_HANDLER_BEGIN:
			RemindViewIndex=1;
			DifferentRemindId.RemindViewHandlerId = iParam;
			SetSystemSuspendTime(DISPLAY_TIME_10S_FLAG);
			memset(RDTableTemp,0,sizeof(RDTableTemp));
			Pages = GetRemindDisplay(RDTableTemp);			
			break;

		case MSG_HANDLER_END:
			DifferentRemindId.RemindViewHandlerId = 0;
			break;
/*用户操作*/
		case MSG_LEFTDOWN_SHORTKEY:				/*左键 单击*/	
		case MSG_LEFTDOWN_DOUBLE_SHORTKEY:		/*左键 双击*/
		case MSG_LEFTDOWN_LONGKEY:           			/*左键 长按*/
		case MSG_RIGHTDOWN_DOUBLE_SHORTKEY:   	/*右键 双击*/	
		case MSG_RIGHTDOWN_LONGKEY:				/*右键 长按*/	
		case MSG_RIGHTDOWN_LONGKEY_UP:			/*右键 长按松开*/		
		case MSG_TOUCH_DOWN_UP_FLICK:				/*左*/ 
		case MSG_TOUCH_UP_DOWN_FLICK: 			/*右*/		
		case MSG_TOUCH_UP_DOUBLE_SHORTKEY:		/*触摸 双击*/	
		case MSG_TOUCH_LONGKEY:					/*触摸 长按*/
		case MSG_TOUCH_UP_SHORTKEY:				/*触摸 单击*/  			
			break;
		
			
		case MSG_RIGHTDOWN_SHORTKEY: 				/*右键 单击*/	
			UnloadHandler(DifferentRemindId.RemindViewHandlerId);
			break;

		case MSG_TOUCH_UP_FLICK:					/*上*/
			if(RemindViewIndex<Pages)
			{
				RemindViewIndex++;
				ShowRemindList(RDTableTemp+(RemindViewIndex-1)*3);
			}
			break;

		case MSG_TOUCH_DOWN_FLICK:				/*下*/
			if(RemindViewIndex>1)
			{
				RemindViewIndex--;
				ShowRemindList(RDTableTemp+(RemindViewIndex-1)*3); 	
			}
			break;

		case MSG_DISPLAY:
			if(!CheckRemindNum())
			{
				UnloadHandler(DifferentRemindId.RemindViewHandlerId);
				break;
			}	
			if(RemindViewIndex>Pages)
				RemindViewIndex = Pages;			
			ShowRemindList(RDTableTemp+(RemindViewIndex-1)*3);
			break;

		case MSG_REFRESH_REMIND:
			if(!CheckRemindNum())
			{
				UnloadHandler(DifferentRemindId.RemindViewHandlerId);
				break;
			}
			memset(RDTableTemp,0,sizeof(RDTableTemp));               
			Pages = GetRemindDisplay(RDTableTemp);
			if(RemindViewIndex>Pages)
				RemindViewIndex = Pages;
			if(CheckpCurrentHandler(DifferentRemindId.RemindViewHandlerId))
			{
				ShowRemindList(RDTableTemp+(RemindViewIndex-1)*3);
			}
			break;
			
		default:
			return RET_MSG_UNHANDLED;
	}
	return RET_MSG_HANDLED;
}

/*显示电量Handler*/
uint16 BatteryHandler(MsgType msg, int iParam, void *pContext)
{
	switch(msg)
	{
		case MSG_HANDLER_BEGIN:
			DifferentRemindId.BatteryHandlerId = iParam;	
			SetSystemSuspendTime(DISPLAY_TIME_10S_FLAG);
			break;

		case MSG_HANDLER_END:
			DifferentRemindId.BatteryHandlerId = 0;
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
		case MSG_TOUCH_UP_SHORTKEY:				/*触摸 单击*/ 
		case MSG_TOUCH_UP_DOUBLE_SHORTKEY:
		case MSG_TOUCH_LONGKEY:					/*触摸 长按*/
			break;

		case MSG_RIGHTDOWN_SHORTKEY: 				/*右键 单击*/	
			UnloadHandler(DifferentRemindId.BatteryHandlerId); 
			break;
		case MSG_DISPLAY:	
			ShowBatteryInfo();
			break;
					
		default:
			return RET_MSG_UNHANDLED;
	}
	return RET_MSG_HANDLED;
}

/*显示勋章Handler*/
uint16 MedalHandler(MsgType msg, int iParam, void *pContext)
{
	switch(msg)
	{
		case MSG_HANDLER_BEGIN:
			DifferentRemindId.MedalHandlerId = iParam;	
			SetSystemSuspendTime(DISPLAY_TIME_10S_FLAG);
			break;

		case MSG_HANDLER_END:
			DifferentRemindId.MedalHandlerId = 0;
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
		case MSG_TOUCH_UP_SHORTKEY:				/*触摸 单击*/ 
		case MSG_TOUCH_UP_DOUBLE_SHORTKEY:
		case MSG_TOUCH_LONGKEY:					/*触摸 长按*/
		case MSG_RIGHTDOWN_SHORTKEY: 				/*右键 单击*/
			UnloadHandler(DifferentRemindId.MedalHandlerId); 
			break;
		case MSG_MEDAL:
			UnloadHandler(DifferentRemindId.MedalHandlerId); 
			SendMsg(MSG_MEDAL);
			break;
		case MSG_DISPLAY:	
			ShowEarnedMeadalRemind(Medal_Type);
			break;
					
		default:
			return RET_MSG_UNHANDLED;
	}
	return RET_MSG_HANDLED;
}

/*显示 Have no new notifications */
uint16 NoNewNotificationHandler(MsgType msg, int iParam, void *pContext)
{
	switch(msg)
	{
		case MSG_HANDLER_BEGIN:
			DifferentRemindId.NoNewNotificationHandlerId = iParam;	
			SetSystemSuspendTime(DISPLAY_TIME_10S_FLAG);
			break;

		case MSG_HANDLER_END:
			DifferentRemindId.NoNewNotificationHandlerId = 0;
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
		
		case MSG_TOUCH_UP_SHORTKEY:				/*触摸 单击*/ 
		case MSG_TOUCH_UP_DOUBLE_SHORTKEY:
		case MSG_TOUCH_LONGKEY:					/*触摸 长按*/
			break;
		case MSG_RIGHTDOWN_SHORTKEY: 				/*右键 单击*/
		case MSG_TOUCH_DOWN_UP_FLICK:			/*左*/ 
		case MSG_TOUCH_UP_DOWN_FLICK: 			/*右*/ 
			UnloadHandler(DifferentRemindId.NoNewNotificationHandlerId); 
			break;
		case MSG_DISPLAY:	
			ShowNoNewNotifications();
			break;
					
		default:
			return RET_MSG_UNHANDLED;
	}
	return RET_MSG_HANDLED;
}
/*显示 Delete Ancs*/
uint16 DeleteAncsHandler(MsgType msg, int iParam, void *pContext)
{
	switch(msg)
	{
		case MSG_HANDLER_BEGIN:
			DifferentRemindId.DeleteAncsHandlerId = iParam;	
			SetSystemSuspendTime(DISPLAY_TIME_10S_FLAG);
			break;

		case MSG_HANDLER_END:
			DifferentRemindId.DeleteAncsHandlerId = 0;
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
		case MSG_TOUCH_UP_DOUBLE_SHORTKEY:
		case MSG_TOUCH_LONGKEY:					/*触摸 长按*/
			break;

		case MSG_RIGHTDOWN_SHORTKEY: 				/*右键 单击*/
			UnloadHandler(DifferentRemindId.DeleteAncsHandlerId); 
			break;
		case MSG_TOUCH_UP_SHORTKEY:				/*触摸 单击*/ 
			{
				uint8 TouchID = Get_Screen_Locaton(3,3);
				if(TouchID==6)
				{					
					ClearAncsFlash();
					UnloadHandler(DifferentRemindId.DeleteAncsHandlerId); 
				}
				else if(TouchID==8)
				{
					UnloadHandler(DifferentRemindId.DeleteAncsHandlerId); 
				}
			}	
			break;
		case MSG_DISPLAY:	
			ShowDeleteAncs();
			break;
					
		default:
			return RET_MSG_UNHANDLED;
	}
	return RET_MSG_HANDLED;
}

/*显示 Have no new reminders */
uint16 NoReminderHandler(MsgType msg, int iParam, void *pContext)
{
	switch(msg)
	{
		case MSG_HANDLER_BEGIN:
			DifferentRemindId.NoReminderHandlerId = iParam;	
			SetSystemSuspendTime(DISPLAY_TIME_10S_FLAG);
			break;

		case MSG_HANDLER_END:
			DifferentRemindId.NoReminderHandlerId = 0;
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
		case MSG_TOUCH_UP_SHORTKEY:				/*触摸 单击*/ 
		case MSG_TOUCH_UP_DOUBLE_SHORTKEY:
		case MSG_TOUCH_LONGKEY:					/*触摸 长按*/
			break;
		case MSG_RIGHTDOWN_SHORTKEY: 				/*右键 单击*/
			UnloadHandler(DifferentRemindId.NoReminderHandlerId); 
			break;
		case MSG_DISPLAY:	
			ShowNoNewReminders();
			break;
					
		default:
			return RET_MSG_UNHANDLED;
	}
	return RET_MSG_HANDLED;
}

/*显示 是否退出睡眠模式 */
uint8 IsLeftRight = 0;
uint16 ExitsleepmodeHandler(MsgType msg, int iParam, void *pContext)
{
	switch(msg)
	{
		case MSG_HANDLER_BEGIN:
			DifferentRemindId.ExitsleepmodeHandlerId = iParam;	
			SetSystemSuspendTime(DISPLAY_TIME_10S_FLAG);
			break;

		case MSG_HANDLER_END:
			DifferentRemindId.ExitsleepmodeHandlerId = 0;
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
		case MSG_TOUCH_UP_DOUBLE_SHORTKEY:
		case MSG_TOUCH_LONGKEY:					/*触摸 长按*/
		case MSG_RIGHTDOWN_SHORTKEY: 				/*右键 单击*/
			break;
		case MSG_TOUCH_UP_SHORTKEY:				/*触摸 单击*/ 
			{
				uint8 TouchID = Get_Screen_Locaton(3,3);
				if(TouchID==6)
				{
					SendMsg(MSG_REMOVE_SLEEP_MONITOR);
					osal_start_timerEx(GetAppTaskId(), MSG_UNLOADSELF_HANDLER_TIMEOUT, 100);			
				}
				else if(TouchID==8)
				{
					UnloadHandler(DifferentRemindId.ExitsleepmodeHandlerId); 
				}
			}	
			break;
		case MSG_UNLOADSELF_HANDLER_TIMEOUT:
			if(SLEEP_MODE_FLAG)
			{
				GotoNormalMode(true);
			}
			UnloadHandler(DifferentRemindId.ExitsleepmodeHandlerId);
			if(IsLeftRight)
			{
				SendMsg(MSG_BACKONMAINMENUE);
				IsLeftRight = 0;
			}
			osal_clear_event(GetAppTaskId(), MSG_UNLOADSELF_HANDLER_TIMEOUT);
			break; 
		case MSG_DISPLAY:	
			ShowExitSleepMode();
			break;
					
		default:
			return RET_MSG_UNHANDLED;
	}
	return RET_MSG_HANDLED;
}

/*显示 是否退出record模式 */
uint16 ExitRecordWorkOutsHandler(MsgType msg, int iParam, void *pContext)
{
	switch(msg)
	{
		case MSG_HANDLER_BEGIN:
			DifferentRemindId.ExitRecordWorkOutsHandlerId = iParam;	
			SetSystemSuspendTime(DISPLAY_TIME_10S_FLAG);
			break;

		case MSG_HANDLER_END:
			DifferentRemindId.ExitRecordWorkOutsHandlerId = 0;
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
		case MSG_TOUCH_UP_DOUBLE_SHORTKEY:
		case MSG_TOUCH_LONGKEY:					/*触摸 长按*/
		case MSG_RIGHTDOWN_SHORTKEY: 				/*右键 单击*/
			break;
		case MSG_TOUCH_UP_SHORTKEY:				/*触摸 单击*/ 
			{
				uint8 TouchID = Get_Screen_Locaton(3,3);
				if(TouchID==6)
				{
					SendMsg(MSG_UNLOADRECORDWORKOUTS);
					osal_start_timerEx(GetAppTaskId(), MSG_UNLOADSELF_HANDLER_TIMEOUT, 100);
				}
				else if(TouchID==8)
				{
					UnloadHandler(DifferentRemindId.ExitRecordWorkOutsHandlerId); 
				}
			}	
			break;
		case MSG_UNLOADSELF_HANDLER_TIMEOUT:
			UnloadHandler(DifferentRemindId.ExitRecordWorkOutsHandlerId);
			if(IsLeftRight)
			{
				SendMsg(MSG_BACKONMAINMENUE);
				IsLeftRight = 0;
			}
			osal_clear_event(GetAppTaskId(), MSG_UNLOADSELF_HANDLER_TIMEOUT);
			break;
		case MSG_DISPLAY:	
			ShowExitRecordWorkouts();
			break;
					
		default:
			return RET_MSG_UNHANDLED;
	}
	return RET_MSG_HANDLED;
}


