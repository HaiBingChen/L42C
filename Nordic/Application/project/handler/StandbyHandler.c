#include "headfile.h"



StandbyStateTAG StandbyState;
bool BondSuccessfulState=true;
bool FilterDisconnectDisplay=false;
bool IsGoingToPowerDown = false;
bool StandbyTimeModifyEnable = true;
bool Update_Pay_Info = false;
uint8 StandbyTimeCounter = DISPLAY_TIME_10S_FLAG;
uint8 CurrentMenuIdx=0;
HandlerId CurrentMenuIParam=0;
uint8 BleBondSaveNotice=0;
uint8 gTimeDisMode = 0;

void SetSystemSuspendTime(uint8 time)
{
	if((StandbyTimeCounter <= 5))
	{
		SetBrightness(1);//no display, no flash write
	}
        if(StandbyTimeModifyEnable)
    	{
    		StandbyTimeCounter = time;
    	}
}
uint8 GetSystemSuspendTime(void)
{
	if(StandbyTimeModifyEnable)
		return StandbyTimeCounter;
	else
		return 0;
}

/*һ������:ʱ��Handler*/
uint16 Time_Handler(MsgType msg, int iParam, void *pContext)
{
	UTCTimeStruct uTime;
	switch(msg)
	{
		case MSG_HANDLER_BEGIN:
			StandbyState.TimeHandlerId= iParam;
			gTimeDisMode = 0;
			//TimerMenuFlag =6;
			break;

		case MSG_HANDLER_END:
			StandbyState.TimeHandlerId= 0;
			break;
/*�û�����*/	
		case MSG_TOUCH_UP_DOUBLE_SHORTKEY:			/*���� ˫��*/			
		case MSG_TOUCH_UP_SHORTKEY:					/*���� ����*/
		case MSG_TOUCH_LONGKEY:						/*���� ����*/	
		case MSG_LEFTDOWN_SHORTKEY:					/*��� ����*/			
		case MSG_RIGHTDOWN_DOUBLE_SHORTKEY:			/*�Ҽ� ˫��*/	
		case MSG_RIGHTDOWN_LONGKEY:					/*�Ҽ� ����*/		
		case MSG_RIGHTDOWN_LONGKEY_UP:				/*�Ҽ� �����ɿ�*/		
			break;
			
		case MSG_TOUCH_DOWN_FLICK:					/*��*/
			//LoadHandler(ShortCutHandler,0);		
			break;
		
		case MSG_TOUCH_UP_FLICK:						/*��*/	
			//if(GetAncsMsgLayOut(Ancs_Social,NULL))
				//LoadHandler(SocialMessageHandler, 0);	
			break;
		
		case MSG_TOUCH_DOWN_UP_FLICK:					/*��*/
			LoadMenuHandler(MENU_FORWARD);
			break;
		
		case MSG_TOUCH_UP_DOWN_FLICK: 				/*��*/
			LoadMenuHandler(MENU_BACKWARD);
			break;
			
		case MSG_LEFTDOWN_LONGKEY:							
			{
#if 0				
				uint8 TempFlag=0x00;
				osal_stop_timerEx(GetAppTaskId(), MSG_BUTTON_POWERUP_BC5_ENABLE);
				osal_stop_timerEx(GetAppTaskId(), MSG_BC5_ON_RESET);
				osal_stop_timerEx(GetAppTaskId(), MSG_BC5_OFF_RESET);
				BC5_POWERON = false;
				KEEP_BC5_POWERON =false;
				BUTTON_RECONNECT_BC5 =false;
				WRITE_USER_FLAG(&TempFlag,BC5_RECONNECTED_LOC,BC5_RECONNECTED_LEN);
				LoadHandler(BTConnecting_Handler,0);
#endif
			}
			break;			
			
		case MSG_RIGHTDOWN_SHORTKEY:					/*�Ҽ� ����*/
			//StandbyTimeCounter = DISPLAY_STATE_NO_CHANGE;
			//SendMsg(MSG_SYSTEM_SUSPEND);	
			break;	
			
	
			
		case MSG_DATE_TIME_UPDATE:						/*--->APPͬ��ʱ��*/
		case MSG_DATE_UPDATE:							/*--->ϵͳÿһ���Ӹ���ʱ��*/
		case MSG_DATE_TIME_FORMAT_UPDATE:	 			/*--->ʱ�������ı�*/
			if(CheckpCurrentHandler(StandbyState.TimeHandlerId) == false || DISPLAY_GOTO_SLEEP_FLAG)
				break;
					
		case MSG_DISPLAY:
			gOS_GetUTCTime(&uTime);
			ShowStandby(&uTime,0);
			if((TimerMenuFlag==1)||(TimerMenuFlag==4)||(TimerMenuFlag==5))
			{
				RefreshTimeUIFun(TimerMenuFlag);
				osal_start_timerEx(GetAppTaskId(), MSG_TIME_CLOCK,1000);
			}
			break;
		case MSG_TIME_CLOCK:
			if(CheckpCurrentHandler(StandbyState.TimeHandlerId)&&(!DISPLAY_GOTO_SLEEP_FLAG)&&(!USB_GO_IN_FLAG))
			{
				if((TimerMenuFlag==1)||(TimerMenuFlag==4)||(TimerMenuFlag==5))
				{
					gTimeDisMode++;
					RefreshTimeUIFun(TimerMenuFlag);
					osal_start_timerEx(GetAppTaskId(), MSG_TIME_CLOCK,1000);
				}
				
			}
			break;
		default:
			return RET_MSG_UNHANDLED;
	}
	return RET_MSG_HANDLED;
}


void LoadMenuHandler(MENU_DIRECTION Md)
{
	uint8 idx = CurrentMenuIdx;
	uint8 idxMax = 7;
	switch(Md)
	{
		case MENU_STAY:
			if(MenuOrder[idx]!=0x00)
				break;
		case MENU_FORWARD:
			if(idx == (idxMax-1) ||MenuOrder[idx+1]==0)
				idx=0;
			else
				idx++;
			break;
		case MENU_BACKWARD:
			if(idx ==0)
			{
				idx=idxMax-1;
				while(MenuOrder[idx]==0)
					idx--;				
			}
			else
				idx--;
			break;
		case MENU_JUMP_TIME_HANDLER://һ���˵��ڲſ���
			idx=0;
			break;
		default:
			break;
	}
	switch(MenuOrder[idx])
	{
		case 1:
			CurrentMenuIParam=LoadHandler(Time_Handler,&Md);
			break;
		case 2:
			CurrentMenuIParam=LoadHandler(SportMmenuHandler, &Md);
			break;
		case 3:
			CurrentMenuIParam=LoadHandler(HRMmenuHandler, &Md);	
			break;
		case 4:
			CurrentMenuIParam=LoadHandler(RecordWorkoutsMmenuHandler, &Md);	
			break;	
		case 5:
			CurrentMenuIParam=LoadHandler(SleepMmenuHandler, &Md);	
			break;	
		case 6:
			CurrentMenuIParam=LoadHandler(NotificationMmenuHandler, &Md);
			break;		
		case 7:
			CurrentMenuIParam=LoadHandler(AppsMmenuHandler, &Md);
			break;	
		default:
			break;
	}
	CurrentMenuIdx= idx;	
}


/*�ײ�Handler*/
uint16 Standby_Handler(MsgType msg, int iParam, void *pContext)
{
	uint8 MessageTemp;
	switch(msg)
	{
		case MSG_HANDLER_BEGIN:
			StandbyState.MyId = iParam;
			break;

		case MSG_HANDLER_END:
			break;

/*(1) �û�����*/
		case MSG_LEFTDOWN_SHORTKEY:				/*��� ����*/	
		case MSG_LEFTDOWN_DOUBLE_SHORTKEY:		/*��� ˫��*/	
		case MSG_RIGHTDOWN_LONGKEY:				/*�Ҽ� ����*/	
		case MSG_TOUCH_UP_FLICK:					/*��*/
		case MSG_TOUCH_DOWN_FLICK:				/*��*/
		case MSG_TOUCH_DOWN_UP_FLICK:				/*��*/ 
		case MSG_TOUCH_UP_DOWN_FLICK: 			/*��*/
		case MSG_TOUCH_UP_SHORTKEY:				/*���� ����*/ 
		case MSG_TOUCH_UP_DOUBLE_SHORTKEY:		/*���� ˫��*/	 	 			
		case MSG_TOUCH_LONGKEY:					/*���� ����*/
			break;
			
		case MSG_TOUCH_UP_DOUBLE_SHORTKEY_FOR_RESUME: 	/*���� ˫�� ���߻���--->����ϵͳ*/	
			if((Powerstate.powerstate) && (Powerstate.standbystate))  
			{
				if(SLEEP_MODE_FLAG)                              
					osal_start_timerEx(GetSleepTaskId(), SLEEP_BUTTON_PRESS_EVENT,200);
   				SCAN_KEY_VALID_FLAG=true;
				Powerstate.InterruptType = true;
				SendMsg(MSG_SYSTEM_RESUME);
			}  
			else if(!LCD_DISPLAY_FLAG)		
				SendMsg(MSG_DISPLAY);
			break;
			
		case MSG_LEFTDOWN_LONGKEY:				/*��� ����--->�ز��绰*/			
			//LoadPhoneHandler(PHONE_CALLOUT,NULL,0);
			break;
		case MSG_BACKONMAINMENUE_BOND:	
		case MSG_BACKONMAINMENUE:
		case MSG_FORCE_BACK_HOME:
			if((Powerstate.powerstate== true)&&(!DISPLAY_GOTO_SLEEP_FLAG)&&(!USB_GO_IN_FLAG))
			{
				UnloadMultiHandler();
				CurrentMenuIdx=0;
				LoadMenuHandler(MENU_JUMP_TIME_HANDLER);
			}
			break;
		

			
		case MSG_RIGHTDOWN_SHORTKEY: 				/*�Ҽ� ����--->�ػ������ֹͣ����ɨ��*/
		case MSG_RIGHTDOWN_LONGKEY_UP:			/*�Ҽ� �����ɿ�--->�ػ������ֹͣ����ɨ��*/
			if(Powerstate.powerstate== false)
				StopKeyScan();
			break;	

		case MSG_RIGHTDOWN_DOUBLE_SHORTKEY: 		/*�Ҽ� ˫��--->��������siri,�ػ������ֹͣ����ɨ��*/
			if(Powerstate.powerstate== true)
				;//LoadHandler(Siri_Handler,NULL);
			else
				StopKeyScan(); 
			break;
			
		case MSG_RIGHTDOWN_HOLDKEY: 				/*�Ҽ� HOLD--->�ػ����*/	
			if((USB_GO_IN_FLAG == false)||(Powerstate.chargeinterface == true))
			{
				if(Powerstate.powerstate== true)
				{
					if(Powerstate.displaystate == true)
					{
						SystemPowerOff();
					}
				}
				else 
				{     
					if(Powerstate.displaystate == false)
					{
						SystemPowerOn();
						SendMsg(MSG_DISPLAY);
					}
				}
			}
			else
			{     
				if(Powerstate.chargeinterface == true)
				{
					ChargeDisplayStateChange(true);
				}
				else
				{
					ChargeDisplayStateChange(false);
				}
			}
			break;
			
		case MSG_RIGHTDOWN_HOLDKEY_UP:			/*�Ҽ� HOLD�ɿ�--->�ػ����*/
			if(Powerstate.powerstate== true)
			{
				if(Powerstate.displaystate == false)
				{
					// �ػ��˳�˯��ģʽ
					if (SLEEP_MODE_FLAG) 
					{
						IsGoingToPowerDown = true;
						SLEEP_MODE_DISPLAY_FLAG=false;
						GotoNormalMode(TRUE);
					}
					Powerstate.powerstate = false;
					StopKeyScan();
					SetSystemSuspendTime(DISPLAY_STATE_NO_CHANGE);
				}
			}
			else
			{
				if(Powerstate.displaystate == true)
				{
					Powerstate.powerstate = true;
				}
			}   
			break;


/*(2) ϵͳ��Ӧ*/
		case MSG_SYSTEM_SUSPEND:															
			SendSuspendEvent();
			break;

		case MSG_SYSTEM_RESUME:
			SendResumeEvent();
			break;
			
		case MSG_DISPLAY:
			if(gInitMark == 1)
			{
				LoadMenuHandler(MENU_STAY);
				if(BleReconnectFlag)
				{
					FLP_TurnOnBle();
					ANCS_ENABLE =true;				
					BleReconnectFlag = false;
				}

				if(BC5_POWERON)
				{
					BC5_POWERON = false;	
					KEEP_BC5_POWERON=true;
					BUTTON_RECONNECT_BC5 =true;
					Powerstate.bc5state = BC5_BT_RECONNECTED;
					SendMsg(MSG_BC5_OFF_RESET);				  
				}                  
			}
			else
			{
				LoadHandler(SetupAlertHandler, 0);
			}
			break;

		case MSG_SYSTEM_FACTORY_SET_STATE:		/*--->�ָ���������*/	
			SystemFactoryModeSet();
			break;
			
		case MSG_SLEEP_MODE_FORCE_EXIT:			/*--->ǿ�ƻ���˯��ģʽ:���USB*/		
			if(SLEEP_MODE_FLAG && USB_GO_IN_FLAG)
			{
				ChargeDisplayStateChange(false);
				SLEEP_MODE_DISPLAY_FLAG=false;
				osal_start_timerEx(GetAppTaskId(), MSG_SLEEP_MODE_EXIT, 100);
			}
			break;

		case MSG_SLEEP_MODE_EXIT:					/*--->�˳�˯��ģʽ*/	
			if(SLEEP_MODE_FLAG)
			{
				SecondsTimerStop();
				GotoNormalMode(true);
				LoadHandler(SleepMonitor_Handler,0);
			}
			break;
		case MSG_SLEEP_MODE_START:					/*--->����˯��ģʽ*/
			if(!SLEEP_MODE_FLAG)
			{	
				SecondsTimerStop();
				GotoSleepMode();
				LoadHandler(SleepMonitor_Handler,0);
			}
			break;	
			
		case MSG_VOL_SET:							/*--->APP��������*/
			LoadVolumeHandler(true,true);
			break;

		case MSG_UPDATE_FONT_LIB:					/*--->�ֿ���¼*/
			LoadHandler(UpdateFontLibHandler,0);	
			break;

		case MSG_TEST_TP:							/*--->TP����*/				
			LoadHandler(TpTestHandler,0);	
			break;		
		case MSG_OTA:								/*--->OTA����*/											
			LoadHandler(OtaHandler,0);
			break;	
		case MSG_SECONDS_TIMER_EVENT:
			//SecondsTimerStart();
			break;		
		case MSG_OPEN_STEP:
			if(!STEP_ON_FLAG)	//�Ʋ�ֹͣ
	            	{
	            		StartStep(); 		//���Ʋ�
	             	}
			break;
			
		case MSG_SET_BACKLIGHT_DAKR:
			if(StandbyTimeCounter <= 5)
				SetBrightness(0);
			break;
			
		case MSG_MOTO_DEFAULT:
			MotoRun(MOTO_DEFAULT);					/*����ʱ����档������һ��*/
			break;
			
/*(3) ��Ϣ���ѽ���*/
		case MSG_USER_REMIND:						/*--->���ѽ���*/			
			LoadHandler(RemindHandler, 0);
			break;
			
		case MSG_UPLOAD_REMINDS:					/*--->�ϴ���������*/
			RemindsUploadAll(0);
			break;
			
		case MSG_MEMORY_CHANGE:					/*--->�ڴ�ռ�������*/				
			//if((MEM_FULL_FLAG) && (DifferentRemindId.MemoryFullHandlerId==0))
				//LoadHandler(MemmoryFullHandler, 0);
			break;
			
		case MSG_SYNCING_START:					/*--->ͬ��ͼ�����*/				
			LoadHandler(Syncing_Handler,NULL);
			break;
			
		case MSG_GOAL_OVER_NOTIFY:					/*--->Ŀ���ɽ���*/
			LoadHandler(GoalOver_Handler,NULL);
			break;
			
		case MSG_SIT_REMIND:						/*--->�������ѽ���*/
			if((!SLEEP_MODE_FLAG) && (!USB_GO_IN_FLAG))
				LoadHandler(SitRemindHandler,0);
			break;
		
		 case MSG_HR_UPPER_REMIND:
		 	LoadHandler(HrRemindHandler, (void*)0);
		 	//LoadHandler(HrRemindHandler, (void*)HR_ALARM_LOW);		 	
			//TRACE("LoadHandler MSG_HR_UPPER_REMIND\r\n");
		 	break;
	 	case MSG_HR_LOWER_REMIND:
			LoadHandler(HrRemindHandler, (void*)1);
		 	//LoadHandler(HrRemindHandler, (void*)HR_ALARM_HIGH);
		 	//TRACE("LoadHandler MSG_HR_LOWER_REMIND\r\n");
		 	break;
		
		case MSG_SMS_NOTIFY:
			if((gShortMsgCount > 0)&&(BondSuccessfulState)&&(!SLEEP_MODE_FLAG)&&(SWITCH_ANCS_SMS))
			{
				if(GetAncsMsgLayOut(Ancs_Sms,NULL))
					LoadHandler(SMSHomePageHandler, 0);
				else
					break;
			}	
			if (!BondSuccessfulState)
				BleBondSaveNotice |= BLE_BOND_SKIP_SMS;			
			break;

		case MSG_MISSED_CALL_COUNT:
			if((gMissedCallCount > 0)&&(BondSuccessfulState)&&(!SLEEP_MODE_FLAG)&&(SWITCH_ANCS_MISSED_CALL))
			{	if(GetAncsMsgLayOut(Ancs_MissedCall,NULL))
					LoadHandler(MissedCallHomePageHandler, 0);
				else
					break;
			}
			if (!BondSuccessfulState)
				BleBondSaveNotice |= BLE_BOND_SKIP_MISSED_CALL;			
			break;

		case MSG_INCOMING_CALL_NOTIFY:
			if((gIncomingCallCount > 0)&&(BondSuccessfulState)&&(!SLEEP_MODE_FLAG)&&(SWITCH_ANCS_INCOMING_CALL))
				LoadHandler(IncomingCallHandler, 0);
			break;	

		case MSG_E_MAIL_NOTIFY:
			if((gEMailCount > 0)&&(BondSuccessfulState)&&(!SLEEP_MODE_FLAG)&&(SWITCH_ANCS_EMIL))
			{
				if(GetAncsMsgLayOut(Ancs_Email,NULL))
					LoadHandler(E_MailHomePageHandler, 0);
				else
					break;
			}
			if (!BondSuccessfulState)
				BleBondSaveNotice |= BLE_BOND_SKIP_EMAIL;		
			break;

		case MSG_DAILY_SCHEDULE_NOTIFY:
			if((gScheduleCount > 0)&&(BondSuccessfulState)&&(!SLEEP_MODE_FLAG)&&(SWITCH_ANCS_SCHEDULE))
					LoadHandler(DailyScheduleHomePageHandler, 0);
			if (!BondSuccessfulState)
				BleBondSaveNotice |= BLE_BOND_SKIP_SCHEDULE;			
			break;

		case MSG_SOCIAL_NOTIFY:
			if((gSocialCount > 0)&&(BondSuccessfulState)&&(!SLEEP_MODE_FLAG)&&(SWITCH_ANCS_SOCIAL))
			{
				if(GetAncsMsgLayOut(Ancs_Social,NULL))
					LoadHandler(SocialHomePageHandler, 0);	
				else
					break;
			}
			if (!BondSuccessfulState)
				BleBondSaveNotice |= BLE_BOND_SKIP_SOCIAL;			
			break;    
			
/*(4) ANCS���ʹ���*/
		case MSG_ANCS_CANCEL_DISPLAY:												
			gSocialCount = 0;
			gEMailCount = 0;
			gShortMsgCount = 0;
			gMissedCallCount = 0;
			gScheduleCount = 0;
		case MSG_ANCS_REQUIRE_DISPLAY:
			if (BleBondSaveNotice & BLE_BOND_SKIP_SOCIAL)
			{
				BleBondSaveNotice &= ~BLE_BOND_SKIP_SOCIAL;
				if(SWITCH_ANCS_SOCIAL) 
	            			osal_start_timerEx(GetAppTaskId(), MSG_SOCIAL_NOTIFY, 100);
			}

			if (BleBondSaveNotice & BLE_BOND_SKIP_EMAIL)
			{
				BleBondSaveNotice &= ~BLE_BOND_SKIP_EMAIL;
				if(SWITCH_ANCS_EMIL)
	            			osal_start_timerEx(GetAppTaskId(), MSG_E_MAIL_NOTIFY, 200);
			}
			if (BleBondSaveNotice & BLE_BOND_SKIP_MISSED_CALL)
			{
				BleBondSaveNotice &= ~BLE_BOND_SKIP_MISSED_CALL;
				if(SWITCH_ANCS_MISSED_CALL) 
	            			osal_start_timerEx(GetAppTaskId(), MSG_MISSED_CALL_COUNT, 300);
			}
			if (BleBondSaveNotice & BLE_BOND_SKIP_SMS)
			{
				BleBondSaveNotice &= ~BLE_BOND_SKIP_SMS;
				if(SWITCH_ANCS_SMS) 
	            			osal_start_timerEx(GetAppTaskId(), MSG_SMS_NOTIFY, 400);
			}
			if (BleBondSaveNotice & BLE_BOND_SKIP_SCHEDULE)
			{
				BleBondSaveNotice &= ~BLE_BOND_SKIP_SCHEDULE;
				if(SWITCH_ANCS_SCHEDULE) 
	            			osal_start_timerEx(GetAppTaskId(), MSG_DAILY_SCHEDULE_NOTIFY, 500);
			}
			osal_start_timerEx(GetAppTaskId(), MSG_INCOMING_CALL_UNLOAD, 600);
			break;
		case MSG_ANCS_SAVE_TO_FLASH:	
			{
				/*����ANCS��Ϣ��Flash*/
				AncsSaveToFlash();

				/*׼����ȡ��һ��ANCS��Ϣ*/
				Send_AncsConnectedGetAllInfo();
			}
			break;
			
/*(5) ������*/
		case MSG_POWER_OFF_BATTERY:									/*--->�͵�ػ�*/	
			SimulatePowerOffKey(100,500);
			break;

		case MSG_LOW_BATTERY:											/*--->�͵�*/	
			MessageTemp = BATTERY_LOW_WARNING;
			LoadHandler(BatteryStatusHandler, &MessageTemp);
			break;

		case MSG_CHARGE_START:											/*--->��������*/	
			ChargeTime = 0;
			MessageTemp =  BATTERY_CHARGE_NOTICE;
			osal_start_timerEx(GetAppTaskId(), MSG_SLEEP_MODE_FORCE_EXIT, 1000);		
			LoadHandler(BatteryStatusHandler, &MessageTemp);
			break;

		case MSG_CHARGE_STOP:											/*--->�γ������*/
			ChargeTime = 0;
			StopSecondEvent();
			SendMsg(MSG_DISPLAY);
			Powerstate.chargeinterface = false;
			Powerstate.chargepowerstate = false;
			break;
			
		case MSG_DISPLAY_BATTERY_STATE:								/*--->���³��Сͼ��*/								     
			if(ChargeTime <= BATTERY_FULL_SENCOND)
				ChargeTime++;
			if(CheckpCurrentHandler(StandbyState.TimeHandlerId) == true)
			{
				UpdateBatteryStatus(0,0);
			}
			break;		
			
/*(6) �������*/
		case MSG_BLE_BOND_CHOOSE:										/*--->BLEѡ����Խ���*/
			LoadHandler(BleBondChooseHandler, 0);
			break;
			
		case MSG_BLE_DISCONNECT:										/*--->BLE���*/			
			break;
			
		case MSG_BLE_ADVERTISING_CLOSE_STATE:							/*--->BLE���*/
			if(Powerstate.standbystate)
				FLP_TurnOffBle();
			break;
			
		case MSG_BC5_DISCONNECT:										/*--->��������*/		
			if(!POWER_OFF_BT_OFF)
			{
				SendMsg(MSG_INCOMING_CALL_UNLOAD);	
				if((SWITCH_ANTI_LOST)&& (!SLEEP_MODE_FLAG)) 
				{
					LoadHandler(BtBleDisconnectHandler, 0);
				}	
			}
			break;

		case MSG_BC5_BT_CHANGE: 										/*--->BC5����״̬�ı�*/	
			if((BC5_State == BC5_BT_DISCONNECT)||(BC5_State == BC5_BT_RECONNECTED))
			{
				if(FilterDisconnectDisplay)
				{
					KEEP_BC5_POWERON = false;
					FilterDisconnectDisplay = false;
					osal_start_timerEx(GetAppTaskId(), MSG_BUTTON_POWERUP_BC5_ENABLE, 10000);		      
					if(SWITCH_ANTI_LOST) 
					{
						ChargeDisplayStateChange(false);
						osal_start_timerEx(GetAppTaskId(), MSG_BC5_DISCONNECT, 2500);
					}
				}
				else 
				{
					BC5_State = 0xFE;
					KEEP_BC5_POWERON = false;
					SetSystemSuspendTime(DISPLAY_TIME_1S_FLAG);
				}
			}
			else if(BC5_State == BC5_BT_CONNECT_OK)
			{
				KEEP_BC5_POWERON = false;
				FilterDisconnectDisplay = true;
				BUTTON_RECONNECT_BC5 =false;
				SetSystemSuspendTime(DISPLAY_TIME_10S_FLAG);
				osal_stop_timerEx(GetAppTaskId(), MSG_BC5_SUSPEND);
				osal_stop_timerEx(GetAppTaskId(), MSG_BUTTON_POWERUP_BC5_ENABLE);
			}
			SendMsg(MSG_BC5_STATE_CHANGE);
			break;

		case MSG_SLEEP_CLOSE_BT:										/*--->˯�߿�ʼ�ر�BC5*/	
			SetBc5Power(false);
			BC5_POWERON = false;
			KEEP_BC5_POWERON =false;
			FilterDisconnectDisplay = false;
			BUTTON_RECONNECT_BC5 =false;
			BC5_State = BC5_BT_POWEROFF;
			Powerstate.bc5state = BC5_BT_POWEROFF;
			osal_stop_timerEx(GetAppTaskId(), MSG_MISSED_CALL_COUNT);
			osal_stop_timerEx(GetAppTaskId(), MSG_INCOMING_CALL_NOTIFY);
			osal_start_timerEx(GetAppTaskId(), MSG_CALL_STATE_FORCE_IDLE, 50);	
			break;	
			
		case MSG_SLEEP_OPEN_BT:											/*--->˯�߽�����BC5�����*/	
			if(BC5_State != BC5_BT_CONNECT_OK)
			{
				StartBc5ReconnectFlag();
				SendMsg(MSG_SLEEP_EXIT_BC5_OPEN);
			}
			break;
		case MSG_SLEEP_EXIT_BC5_OPEN:									/*--->˯�߽�����BC5*/	
			if(BC5_POWERON)
			{
				BC5_POWERON = false;	
				KEEP_BC5_POWERON=true;
				BUTTON_RECONNECT_BC5 =true;
				Powerstate.bc5state = BC5_BT_RECONNECTED;
				SendMsg(MSG_BC5_OFF_RESET);				  
			}
			break;

		case MSG_BC5_OFF_RESET:										/*--->��λBC5�����*/	
			SetBc5Power(false); 	
			osal_start_timerEx(GetAppTaskId(), MSG_BC5_ON_RESET, 300);		
			break;

		case MSG_BC5_ON_RESET:	  										/*--->��λBC5*/		
			SetBc5Power(true);	
			break;

		case MSG_BUTTON_POWERUP_BC5_ENABLE:							/*--->�������ѣ�BC5��Ҫ����*/	
			BUTTON_RECONNECT_BC5 =true;
			break;

		case MSG_BC5_SUSPEND:											/*--->���߹���������BC5*/								
			if(USB_GO_IN_FLAG)
			{
				BC5_State = 0xFE;
				KEEP_BC5_POWERON = false;
			}
			else
			{
				BC5_State = BC5_BT_DISCONNECT;
				SendMsg(MSG_BC5_BT_CHANGE);
			}
			break;

		case MSG_UART0_READY:											/*--->BC5��ʼ�����*/			
			if((Powerstate.bc5state == BC5_BT_RECONNECTED) ||(BC5_State == BC5_BT_DISCONNECT))
			{
				BC5_SetBtPairCmd(BC5_RE_CONNECT);
				SetSystemSuspendTime(DISPLAY_TIME_10S_FLAG);
			}	
			break;

		case MSG_CALL_STATE_FORCE_IDLE:								/*--->BC5:ǿ�ƹҶ�ͨ��*/
			BC5_TO_CC2540_CallState(BC5_CALL_STATE_HANGUP);
			break;

		case MSG_CALL_STATE_LOAD_UNKNOW_CALL:							/*--->BC5:Unknow Call*/
			{
				uint8 pData[] ={0x00,0x00,0x0E,0x55,0x4E,0x4B,0x4E,0x4F,0x57,0x4E,0x20,0x4E,0x55,0x4D,0x42,0x45,0x52,0x0A,0x0D};
				LoadPhoneHandler(PHONE_RING,pData,1);
			}
			break;
			
		case MSG_CALL_STATE_ONE_CALL_TALKING: 							/*--->BC5:ON Talking*/
			{
				uint8 pData[]={0x00,0x00,0x0A,0x54,0x61,0x6c,0x6b,0x69,0x6e,0x67,0x20,0x4f,0x6e,0x0A,0x0D};
				LoadPhoneHandler(PHONE_CALL,pData,1);
			}
			break;
			
		case MSG_CALL_STATE_TWO_CALL_TALKING:							/*--->BC5:ON Talking*/
			{
				uint8 pData[]={0x01,0x00,0x0D,0x54,0x61,0x6c,0x6b,0x69,0x6e,0x67,0x20,0x4f,0x6e,0x28,0x32,0x29,0x0A,0x0D};
				LoadPhoneHandler(PHONE_CALL,pData,2);
			}
			break;
			
		case MSG_CALL_STATE_OUT_GOING:									/*--->BC5:�ֻ��Ⲧ�绰*/
			LoadPhoneHandler(PHONE_CALLOUT,NULL,1);
			break;
			
		case MSG_CALL_ID_CLEAR:											/*--->BC5:���CallerID����*/
			ClearCallerIDFormApk();
			break;
			
		case MSG_VOL_SET_CMD_AGAIN:									/*--->BC5��������*/
			BC5_SetVolumeCmd(BC5_CALL_AGAIN);
			break;

		case MSG_RING_MODE_CMD_AGAIN:									/*--->BC5����ģʽ����*/
			BC5_SetRingModeCmd(BC5_CALL_AGAIN);
			break;
			
		case MSG_SAVE_CALLER_ID_TO_FLASH:								/*--->BC5������������*/
			SaveCallerIDToBuf(PHONE_DEFAULT,CID_SAVE,NULL,0,true);
			break;

/*(7) ֧�����*/
#if 0
		case MSG_PAY_SYNCING_START:									/*--->֧��ͬ��ͼ�����*/				
			LoadHandler(PaySyncing_Handler,NULL);
			break;	
			
		case MSG_NFC_TEST:											
			nfc_test();
			break;
		case MSG_GET_CSN:
			nfc_test_csn();
			break;
		case MSG_POWER_ON_CMD:
			process_power_on_cmd();
			break;
		case MSG_POWER_OFF_CMD:
			process_power_off_cmd();
			break;
		case MSG_APDU_CMD:
			process_apdu_cmd();
			break;
		case MSG_VIBRATE_CMD:
			process_vibrate_cmd();
			break;
		case MSG_APPLICATION_BLOCK_UNBLOCK:
			process_app_block_unblock();
			break;
		case MSG_READ_TRANSACTION_LOG:
			process_read_transaction_log();
			break;
		case MSG_WAIT_BLE_PACKETS_TIMEOUT:
			ble_wait_packet_timeout(); 
			break;
		case MSG_NOTIFYCATION_ACC_BALANCE:
			process_notify_acc_balance();
			break;
		case MSG_GET_BOOSTER_PARAMETERS:
			process_get_booster_parameters_cmd();
			break;
		case MSG_SET_BOOSTER_PARAMETERS:
			process_set_booster_parameters_cmd();
			break;

		case MSG_ACTIVE_NFC:
			process_active_nfc_cmd();
			LoadHandler(NFC_Handler,0);
			break;
#endif			
/*(8) �豸��ͨ�����*/		
#if 0
		case MSG_ADVERTISER_START:
			if (m_beacon.is_running == false) {
				app_beacon_start();
				DevicesCom.mode = DeviceAdvMode;
				DevicesCom.stage = DeviceAdvStartStatus;
			}
			break;

		case MSG_ADVERTISER_STOP:
			if (m_beacon.is_running == true) {
				app_beacon_stop();
				DevicesCom.stage = DeviceAdvStopStatus;
			}
			break;

		case MSG_SCANNER_START:
			if (m_beacon_scanner.is_running == false) {
				app_beacon_scanner_start();
				DevicesCom.mode = DeviceScanMode;
				DevicesCom.stage = DeviceScanStartStatus;
			}
			break;

		case MSG_SCANNER_STOP:
			if (m_beacon_scanner.is_running == true) {
				app_beacon_scanner_stop();
				DevicesCom.stage = DeviceScanStopStatus;
			}
			if(mScannerMsg !=MSG_HANDLER_BEGIN)
			{
				SendMsg(mScannerMsg);
				mScannerMsg = MSG_HANDLER_BEGIN;
			}
			break;

		case MSG_DEVICESCOM_REPEAT:
			{
				switch(DevicesCom.stage)
				{
					case DeviceIdleStatus:
						if (DevicesCom.mode == DeviceIdleMode)
							SendMsg(MSG_ADVERTISER_START);
						break;

					case DeviceAdvStartStatus:
						if (DevicesCom.mode == DeviceAdvMode)
							SendMsg(MSG_ADVERTISER_STOP);
						break;

					case DeviceAdvStopStatus:
						if (DevicesCom.mode == DeviceIdleMode)
							SendMsg(MSG_SCANNER_START);
						break;

					case DeviceScanStartStatus:
						if (DevicesCom.mode == DeviceScanMode)
							SendMsg(MSG_SCANNER_STOP);
						break;

					case DeviceScanStopStatus:
						if (DevicesCom.mode == DeviceIdleMode)
							DevicesCom.stage = DeviceIdleStatus;
						break;

					default:
						break;
				}
				osal_start_timerEx(GetAppTaskId(), MSG_DEVICESCOM_REPEAT, 200);
			}
			break;

		case MSG_GET_SCAN_MOTO:
			LoadHandler(ReceiveMoToHandler,0);
			break;

		case MSG_GET_SCAN_EXP:
			LoadHandler(ReceiveEXPHandler,0);
			break;

		case MSG_GET_SCAN_ADD_FRIENDS:						
			LoadHandler(DevicesReceiveAddFriend_Handler,0);					
			break;

		case MSG_GET_SCAN_ADD_FRIENDS_OK:
			if(DevAddFriendRspFlag==0)
			{
				DevAddFriendRspFlag =1;
				LoadHandler(DevicesReceiveAddFriendRsp_Handler,0);
			}
			break;	
			
		case MSG_GET_SCAN_ADD_FRIENDS_FAIL:
			if(DevAddFriendRspFlag==0)
			{
				DevAddFriendRspFlag =2;
				LoadHandler(DevicesReceiveAddFriendRsp_Handler,0);
			}
			break;

		case MSG_GET_SCAN_DEL_FRIENDS:
			LoadHandler(ReceiveDelHandler,0);	
			break;

		case MSG_DELAY_ADVERTISE_INIT:
			if(NextDevAdv ==Dev_Init_Delay_6000ms)
				ChangeDeviceAdvertise(Dev_Init);
			break;
#endif			
		case MSG_REMOVE_ALL_ANCS:
			ClearAncsFlash();
			break;
		case MSG_CLEAR_ALL:
			LoadHandler(ClearAllHandler, 0);	
			break;
		case MSG_ClEAR_ANCS_CMD:
			AncsCmdFlag = false;
			break;
		case MSG_FACEBOOK_MESSAGE_IDTITLE:
			if (FaceBookMessageIDTitle == 1)
			{
				FaceBookMessageIDTitle = 0;
				FacebookMessageSet();
				GetNotifAttributesTitleInf("Messenger", 9);
			}
			break;
		case MSG_SKYPE_IDTITLE:
			if (SkeypeIDTitle == 1)
			{
				SkeypeIDTitle = 0;
				SkypeSet();
				GetNotifAttributesTitleInf("Skype", 5);
			}
			break;
		case MSG_WHATSAPP_IDTITLE:
			if (WhatsappIDTitle== 1)
			{
				WhatsappIDTitle = 0;
				WhatsappSet();
				GetNotifAttributesTitleInf("WhatsApp", 8);
			}
			break;
		case MSG_QQ_IDTITLE:
			if (QQIDTitle== 1)
			{
				QQIDTitle = 0;
				QQSet();
				GetNotifAttributesTitleInf("QQ", 2);
			}
			break;
			
		case MSG_SNAPCHAT_IDTITLE:
			if (SnapchatTitle== 1)
			{
				SnapchatTitle = 0;
				SnapchatSet();
				GetNotifAttributesTitleInf("Snapchat", 8);
			}
			break;
			
		case MSG_BIND_END:
			BindEndDealWith();
			break;
		case MSG_MEDAL:
			LoadHandler(MedalHandler,0);
			break;
		default:
			break;
	}
	return RET_MSG_HANDLED;
}

bool isSystemSupend(void)
{
	return ((Powerstate.powerstate) && (Powerstate.standbystate) && (!Powerstate.displaystate));
}

