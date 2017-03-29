#include "headfile.h"







uint8 DisplayTaskId;
uint8 BledisconnectFlag=BT_CLOSE;
bool  BleReconnectFlag=false;
uint8 Bc5ConnectToneFlag=0;
PowerStateContext Powerstate;
extern uint8 simpleBLEPeripheral_TaskID;
uint8 WakeUpCmdCount = 0; 
uint8 SuspendCmdCount = 0;	
void DelayMSecond(uint8 second)
{
        uint16 i,j;

        for(i=0; i< second; i++)
        {
                for(j=0; j< 50000; j++)
                {
                        ;
                }
        }
}
/******************************************************************************************
*������:		DisplayLogo
*����:			true:����    false:�ر���
*����ֵ:		
*����:			��ʾ��ʼ��
*��д:			statdream 
*�汾��Ϣ:	    v1.0
******************************************************************************************/

void DisplayLogo(bool VibrateFlag)
{
#if 0
	InitMotorVibrate();
	MOTOR_PIN_H;

	ShowLogo();

	if(VibrateFlag)
		InitMotorVibrate();
#endif
	Set_Motor_LevelLow();
	if(VibrateFlag)
	{
		Set_Motor_Level();
	}

	ShowLogo(); 

	nrf_delay_ms(300);

	if(VibrateFlag)
	{
		Set_Motor_LevelLow();
	}

}

/******************************************************************************************
*������:		DisplayInitTask
*����:			
*����ֵ:		
*����:			��ʾ�����ʼ��
*��д:			statdream 
*�汾��Ϣ:	    v1.0
******************************************************************************************/
void DisplayInitTask(uint8 task_id)
{
  DisplayTaskId = task_id;
}
/******************************************************************************************
*������:		GetDisplayTaskId
*����:			
*����ֵ:		
*����:			�õ���ʾ�����ID
*��д:			statdream 
*�汾��Ϣ:	    v1.0
******************************************************************************************/
uint8 GetDisplayTaskId(void)
{
  return DisplayTaskId;
}

/******************************************************************************************
*������:		DisplayEventHandler
*����:			
*����ֵ:		
*����:			��ʾ�����¼�����
*��д:			statdream 
*�汾��Ϣ:	    v1.0
******************************************************************************************/
uint16 DisplayEventHandler(uint8 task_id, uint16 events)
{
	if(events & CLEAR_SCREEN_EVENT)
	{
		ClearScreen();
		return ( events ^ CLEAR_SCREEN_EVENT );
	}

	if(events & CLEAR_INIT_MARK_DISPLAY_EVENT)//��ʾ���³�ʼ����Ϣ
	{
		if(gDateTimeValid == 1)
		{
			gInitMark=1;
			ClearSportData();		/*����˶���ʾ*/
			ClearSleepData(true);		/*���˯����ʾ*/
			
			Force_NordicFlash();
			ClearAllReminders();		/*����豸����������*/
			
			Step_Store_Logs=0;
			WRITE_STEP_SLEEP_SUM(&Step_Store_Logs,SPORT_TOTAL_LOC,SPORT_TOTAL_LEN);
			SetStepMemoryFullFlag(false);			
			Sleep_Store_Logs=0;
			WRITE_STEP_SLEEP_SUM(&Sleep_Store_Logs,SLEEP_TOTAL_LOC,SLEEP_TOTAL_LEN);
			SetSleepMemoryFullFlag(false);		
			
			SetDefaultGoal();
			Free_NordicFlash();
			osal_start_timerEx(GetAppTaskId(), MSG_REMOVE_ALL_ANCS, 100);
			SendMsg(MSG_SETUP_END);       
		}
		return ( events ^ CLEAR_INIT_MARK_DISPLAY_EVENT );
	}

	if(events & DISPLAY_SUSPEND_EVENT)//ϵͳ����
	{
		//if((nrf_gpio_pin_read(BC5_INT_PIN)==0)||(BC5_State == BC5_BT_POWEROFF) ||FORCE_SUSPEND_FLAG)
		if((BC5_State == BC5_BT_POWEROFF) ||FORCE_SUSPEND_FLAG)
		{
			FORCE_SUSPEND_FLAG =false;
			SystemSuspend();
		}
		else
		{
			FORCE_SUSPEND_FLAG =true;
			SetSystemSuspendTime(DISPLAY_TIME_10S_FLAG);
		}
		return ( events ^ DISPLAY_SUSPEND_EVENT );
	}

	if(events & DISPLAY_RESUME_EVENT)//ϵͳ����
	{
		SystemResume();
		if(Powerstate.InterruptType)
		{
			SendMsg(MSG_DISPLAY);
			LCD_Sleep(LCD_EXIT_SLEEP);
			if(BUTTON_RECONNECT_BC5)
			{
				if(BC5_State != BC5_BT_CONNECT_OK)
				{
				        BC5_State = 0xFE;                           //��ֹ���Ѽ�ANCSȥ����
				        Bc5ConnectToneFlag = 0;
				        KEEP_BC5_POWERON = true;
				        Powerstate.bc5state = BC5_BT_RECONNECTED;
				        osal_start_timerEx(GetAppTaskId(), MSG_BC5_OFF_RESET, 100);	
				}
			}
		}
		else
		{
			Powerstate.InterruptType = true;
			osal_start_timerEx(GetDisplayTaskId(), DISPLAY_SUSPEND_EVENT, 3000);
		}
		return ( events ^ DISPLAY_RESUME_EVENT );
	}

	if(events & SCAN_KEY_VALID_EVENT) //�򿪰�����Чʹ��
	{
		SCAN_KEY_VALID_FLAG=true;
		return ( events ^ SCAN_KEY_VALID_EVENT );
	}

	if(events & DISPLAY_BC5_CONNECT_STATE) //����ϵͳ��������֪ͨ��BC5
	{
		BC5_SetStandbyModeCmd(BC5_NORMAL_STATE);
		WakeUpCmdCount++;
		if(WakeUpCmdCount<10)
			osal_start_timerEx(GetDisplayTaskId(), DISPLAY_BC5_CONNECT_STATE, 500);
		return ( events ^ DISPLAY_BC5_CONNECT_STATE );
	}
	
	if(events & DISPLAY_BC5_SUSPEND_STATE) //����ϵͳ���߹���֪ͨ��BC5
	{
		BC5_SetStandbyModeCmd(BC5_STANDBY_STATE);
		SuspendCmdCount++;
		if(SuspendCmdCount<10)
			osal_start_timerEx(GetDisplayTaskId(), DISPLAY_BC5_SUSPEND_STATE, 500);
		else if(Powerstate.standbystate== true)
			UartPowerOff();
		return ( events ^ DISPLAY_BC5_SUSPEND_STATE );
	}	
	
	return ( 0 );
}

//�����ʼ������
void ClearInitMarkEvent(void)
{
	osal_set_event(GetDisplayTaskId(),CLEAR_INIT_MARK_DISPLAY_EVENT);
}


/******************************************************************************************
*������:		StartBc5ReconnectFlag
*����:			
*����ֵ:		
*����:			��ȡBC5  �Ƿ���Ҫ���������ı�ʶ
*��д:			luohuigui
*�汾��Ϣ:	    v1.0
******************************************************************************************/

void StartBc5ReconnectFlag(void)
{
        uint8 TempFlag;
	READ_USER_FLAG(&TempFlag,BC5_RECONNECTED_LOC,BC5_RECONNECTED_LEN);
        if(TempFlag == 0x01)
                BC5_POWERON = true; 
        else
                BC5_POWERON = false;
}

/******************************************************************************************
*������:		IsPowerReduce
*����:			
*����ֵ:		
*����:			�ж��Ƿ���Ҫ���͹���
*��д:			luohuigui
*�汾��Ϣ:	    v1.0
******************************************************************************************/
bool IsPowerReduce(void)
{
        return (!Powerstate.displaystate);
}

/******************************************************************************************
*������:		SendNormalStateToBC5
*����:			
*����ֵ:		
*����:			����ϵͳ��������
*��д:			luohuigui
*�汾��Ϣ:	    v1.0
******************************************************************************************/
void SendNormalStateToBC5(void)
{
        osal_set_event(GetDisplayTaskId(), DISPLAY_BC5_CONNECT_STATE);
}
void SendsSuspendStateToBC5(void)
{
        osal_set_event(GetDisplayTaskId(), DISPLAY_BC5_SUSPEND_STATE);
}
/******************************************************************************************
*������:		SendSuspendEvent
*����:			
*����ֵ:		
*����:			����ϵͳ������Ϣ
*��д:			luohuigui
*�汾��Ϣ:	    v1.0
******************************************************************************************/
void SendSuspendEvent(void)
{
        osal_set_event(GetDisplayTaskId(), DISPLAY_SUSPEND_EVENT);
}

/******************************************************************************************
*������:		CancelSendSuspendEvent
*����:			
*����ֵ:		
*����:			ȡ������ϵͳ������Ϣ
*��д:			luohuigui
*�汾��Ϣ:	    v1.0
******************************************************************************************/
void CancelSendSuspendEvent(void)
{
        osal_stop_timerEx(GetDisplayTaskId(), DISPLAY_SUSPEND_EVENT);
}

/******************************************************************************************
*������:		SendResumeEvent
*����:			
*����ֵ:		
*����:			����ϵͳ������Ϣ
*��д:			luohuigui
*�汾��Ϣ:	    v1.0
******************************************************************************************/
void SendResumeEvent(void)
{
        osal_set_event(GetDisplayTaskId(), DISPLAY_RESUME_EVENT);
}

/******************************************************************************************
*������:		StartScanKeyValid
*����:			
*����ֵ:		
*����:			ʹ�ܰ���ɨ����Ч
*��д:			luohuigui
*�汾��Ϣ:	    v1.0
******************************************************************************************/
void SetScanKeyValidFlag(void)
{
        //osal_start_timerEx(GetDisplayTaskId(), SCAN_KEY_VALID_EVENT, 2000);
        osal_start_timerEx(GetDisplayTaskId(), SCAN_KEY_VALID_EVENT, 300);
}

/******************************************************************************************
*������:		SystemPowerOn
*����:			
*����ֵ:		
*����:			ϵͳ����
*��д:			luohuigui
*�汾��Ϣ:	    v1.0
******************************************************************************************/
void PowerStateInit(void)
{
        Powerstate.powerstate = true;
        Powerstate.standbystate = false;
        Powerstate.displaystate = true;
        Powerstate.InterruptType = true;
        Powerstate.chargeinterface = false;
        Powerstate.chargepowerstate = false;
        Powerstate.bc5state = BC5_BT_POWEROFF;
}

/******************************************************************************************
*������:		SystemPowerOn
*����:			
*����ֵ:		
*����:			ϵͳ����
*��д:			luohuigui
*�汾��Ϣ:	    v1.0
******************************************************************************************/
void SystemPowerOn(void)
{
	DisableUsbPinInt();
	ClearScreen();
	LCD_Sleep(LCD_EXIT_SLEEP);
	
	DisplayLogo(true);	
	CypressI2CInit();
	//nrf_delay_ms(1000);	
	
	Powerstate.displaystate = true;
	Powerstate.chargeinterface = false;
	UartPowerOn();
	StartStep();
	POWER_OFF_BT_OFF =false;
	if(Powerstate.chargepowerstate == false)
	{
		ANCS_ENABLE =true;
		if (shortcut_info.airplane_mode  == 0)
			FLP_TurnOnBle();
	}
	else
	{
		BleReconnectFlag = true;
	}

	if (shortcut_info.airplane_mode  == 0)
		StartBc5ReconnectFlag();

	SetSystemSuspendTime(DISPLAY_TIME_10S_FLAG);
	ClearScreen();

	CheckPowerOnUsbStateEvent();     
}

/******************************************************************************************
*������:		SystemPowerOff
*����:			
*����ֵ:		
*����:			ϵͳ�ػ�
*��д:			luohuigui
*�汾��Ϣ:	    v1.0
******************************************************************************************/
void SystemPowerOff(void)
{
	ANCS_ENABLE =false;
	DisableUsbPinInt();
	BleReconnectFlag = false;
	Powerstate.displaystate = false;
	SetSystemSuspendTime(DISPLAY_STATE_NO_CHANGE);

	BC5_POWERON = false;
	KEEP_BC5_POWERON =false;
	FilterDisconnectDisplay = false;
	BUTTON_RECONNECT_BC5 =false;
	FreeReceiveBuff();
	osal_set_event(GetHrTaskId(), HR_CLOSE_EVENT);
	osal_stop_timerEx(GetAppTaskId(), MSG_BUTTON_POWERUP_BC5_ENABLE);
	osal_stop_timerEx(GetAppTaskId(), MSG_BC5_ON_RESET);
	osal_stop_timerEx(GetAppTaskId(), MSG_BC5_OFF_RESET);
	osal_stop_timerEx(GetAppTaskId(), MSG_MISSED_CALL_COUNT);
	osal_stop_timerEx(GetAppTaskId(), MSG_INCOMING_CALL_NOTIFY);
	osal_stop_timerEx(GetAppTaskId(), MSG_SMS_NOTIFY);
	osal_stop_timerEx(GetAppTaskId(), MSG_SOCIAL_NOTIFY);
	osal_stop_timerEx(GetAppTaskId(), MSG_CALL_STATE_LOAD_UNKNOW_CALL);
	StopMotorRunTime(); 
	POWER_OFF_BT_OFF =true;
	if (shortcut_info.airplane_mode  == 0) 
	{
		SetBc5Power(false);
		FLP_TurnOffBle();
	}
	UnloadMultiHandler();
	CurrentMenuIdx=0;	
	CleanAppTaskMessage();   
	SecondsTimerOff();
	ClearScreen();
	UartPowerOff();
	CloseStep();
	zinitix_suspend();
	DisplayLogo(true);	
	nrf_delay_ms(1000);	
	
	LCD_Sleep(LCD_ENTER_SLEEP);

	CheckPowerOffUsbStateEvent();
}

/******************************************************************************************
*������:		SystemSuspend
*����:			
*����ֵ:		
*����:			ϵͳ����
*��д:			luohuigui
*�汾��Ϣ:	    v1.0
******************************************************************************************/
void SystemSuspend(void)
{
	StopKeyScan();		 
	Powerstate.standbystate= true;
	Powerstate.displaystate= false;
	SCAN_KEY_VALID_FLAG = false;

	StopMotorRunTime();
	if((BC5_State == BC5_BT_CONNECT_OK) ||(BC5_State == BC5_BT_RECONNECTED))
	{
		SuspendCmdCount=0;
		SendsSuspendStateToBC5();
	}
	else
	{	  
		if(KEEP_BC5_POWERON == false)
		{
			SetBc5Power(false);
		}
		else
			osal_start_timerEx(GetAppTaskId(), MSG_BC5_SUSPEND, 15000);

		UartPowerOff();
	}
#if 0
	if(Powerstate.chargepowerstate == false)
	{
		if(BtState != BT_CONNECTED)
		{
			if((BledisconnectFlag != BT_DISCONNECT) && (DevicesCom.on_off == DeviceOffStatus))
			{
				if (shortcut_info.airplane_mode == 0)
					FLP_TurnOffBle();
			}
		}
	}
#endif	
	osal_stop_timerEx(GetClockTaskId(), CLOCK_SUSPEND_TIMEOUT_EVENT);	
	LCD_Sleep(LCD_ENTER_SLEEP);			
	zinitix_suspend();	
	PWM_Disable();
}
	
/******************************************************************************************
*������:		SystemResume
*����:			
*����ֵ:		
*����:			ϵͳ����
*��д:			luohuigui
*�汾��Ϣ:		v1.0
******************************************************************************************/
void SystemResume(void)
{
	UartPowerOn();
	PWM_Enable();
	Powerstate.standbystate= false;
	Powerstate.displaystate = true; 	   
	if(Powerstate.chargepowerstate == false)
	{
		if(BtState != BT_CONNECTED)
		{
			if((BledisconnectFlag != BT_DISCONNECT)&& (DevicesCom.on_off == DeviceOffStatus))
			{
				if (shortcut_info.airplane_mode  == 0)
				FLP_TurnOnBle();
			}
		}
	}
	if((BC5_State == BC5_BT_CONNECT_OK) ||(BC5_State == BC5_BT_RECONNECTED))
	{
		WakeUpCmdCount=0;
		SendNormalStateToBC5(); 	  
	}
	osal_stop_timerEx(GetAppTaskId(), MSG_BC5_SUSPEND);
	osal_start_timerEx(GetClockTaskId(), CLOCK_SUSPEND_TIMEOUT_EVENT, 1000);			
}

/******************************************************************************************
*������:		SystemFactoryModeSet
*����:			
*����ֵ:		
*����:			ϵͳ�ָ���������
*��д:			luohuigui
*�汾��Ϣ:	    v1.0
******************************************************************************************/
void SystemFactoryModeSet(void)
{
        DisableUsbPinInt();
        SetSystemSuspendTime(DISPLAY_STATE_NO_CHANGE);
}




