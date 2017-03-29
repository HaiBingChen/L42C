#include "headfile.h"


HR_STATE hr_state = HR_WAITING_INIT;
UiHRStruct DispalyHrData;
UiHRStruct DispalyHrAlarmData;

uint8 bLowHR = 0xFF;

static bool refresh_first = true;
static bool hr_run = false;

void heartRate_dat_clr(void)
{
	hr_state = HR_WAITING_INIT;
}

void hearRate_state_check(void)
{
	
	UTCTimeStruct uUtcTime_cur;
	UTCTimeStruct uUtcTime_last_hr;
	uint8 DHM_Flag;
	uint8 DHM_Time;

	uint8 Goal_High=hr_alarm.HeartRateAlaUpper;
	uint8 Goal_Low=hr_alarm.HeartRateAlaLower;
	uint8 Goal_Diff=Goal_High-Goal_Low;
	uint16 Level;
	if(HrGetValue()<Goal_Low)
	{	Level=0;	}
	else if(HrGetValue()>Goal_High)
	{	Level=10;	}
	else
	{	Level=((HrGetValue()-Goal_Low)*10)/Goal_Diff;	}

	
	if(hr_run != HrIsRunning())
	{
		refresh_first = true;
	}
	hr_run = HrIsRunning();

	if(hr_run==true)
	{
		hr_state = HR_GOING;
	}
	else
	{
		if(hr_state != HR_WAITING_INIT)
			hr_state = HR_WAITING;
	}

	gOS_GetUTCTime(&uUtcTime_cur);
	HrGetLastValueTime(&uUtcTime_last_hr);

	if(hr_state==HR_GOING)
	{
		static bool hrValValid = false;
		if(hrValValid != HrValueValid())
		{
			refresh_first = true;
			hrValValid = !hrValValid;
		}
		if(HrStarting())
		{
			DHM_Flag=0xFF;
			DHM_Time=0;
			DispalyHrData.HR_refresh_first=refresh_first;
			DispalyHrData.HR_mode=1;
			DispalyHrData.HR_Run_State=hr_state;
			DispalyHrData.HR_Value_Val=0;  
			DispalyHrData.HR_Value=0;         
			DispalyHrData.HR_Running=HrStarting();

			DispalyHrData.DHM_Flag=DHM_Flag;
			DispalyHrData.DHM_Time=DHM_Time;
		}
		else
		{
			if(refresh_first)
			{
				refresh_first = false;
				DHM_Flag=0xFF;
				DHM_Time=0;
				DispalyHrData.HR_refresh_first=refresh_first;
				DispalyHrData.HR_mode=1;
				DispalyHrData.HR_Run_State=hr_state;
				DispalyHrData.HR_Value_Val=HrValueValid();  
				DispalyHrData.HR_Value=HrGetValue();         
				DispalyHrData.HR_Running=0;

				DispalyHrData.DHM_Flag=DHM_Flag;
				DispalyHrData.DHM_Time=DHM_Time;
				DispalyHrData.HR_Level = Level;
			}
			else
			{
				DHM_Flag=0;
				DHM_Time=0;
				DispalyHrData.HR_refresh_first=refresh_first;
				DispalyHrData.HR_mode=3;
				DispalyHrData.HR_Run_State=hr_state;
				DispalyHrData.HR_Value_Val=HrValueValid();  
				DispalyHrData.HR_Value=HrGetValue();          
				DispalyHrData.HR_Running=0;

				DispalyHrData.DHM_Flag=DHM_Flag;
				DispalyHrData.DHM_Time=DHM_Time;
				DispalyHrData.HR_Level = Level;
			}
		}

	}
	else if(hr_state==HR_WAITING)
	{
		refresh_first = true;
		DHM_Flag=CalcAccumulateTime(&DHM_Time,uUtcTime_cur,uUtcTime_last_hr);
		DispalyHrData.HR_refresh_first=refresh_first;
		DispalyHrData.HR_mode=2;
		DispalyHrData.HR_Run_State=hr_state;//HR_STOP;
		//DispalyHrData.HR_Value_Val=HrValueValid();  
		DispalyHrData.HR_Value=HrGetValue();          
		DispalyHrData.HR_Running=0;
		DispalyHrData.DHM_Flag=DHM_Flag;
		DispalyHrData.DHM_Time=DHM_Time;

	}
	else if(hr_state==HR_WAITING_INIT)
	{
		refresh_first = true;
		DHM_Flag=0xFF;
		DHM_Time=0;
		DispalyHrData.HR_refresh_first=refresh_first;
		DispalyHrData.HR_mode=0;
		DispalyHrData.HR_Run_State=hr_state;
		//DispalyHrData.HR_Value_Val=HrValueValid();  
		//DispalyHrData.HR_Value=HrGetValue();          
		//DispalyHrData.HR_Running=0;

		//DispalyHrData.DHM_Flag=DHM_Flag;
		//DispalyHrData.DHM_Time=DHM_Time;			
	}
}

uint16 heartRate_Handler(MsgType msg, int iParam, void *pContext)
{


	switch(msg)
	{
		case MSG_HANDLER_BEGIN:
			DifferentRemindId.heartRate_HandlerId = iParam;
			SetSystemSuspendTime(DISPLAY_TIME_10S_FLAG);
			osal_start_timerEx(GetAppTaskId(), MSG_HR_STATE_CHECK, 1000);
			refresh_first = true;
			hearRate_state_check();
			break;

		case MSG_HANDLER_END:
			DifferentRemindId.heartRate_HandlerId = 0;
			StopMotorRunTime();
			osal_stop_timerEx(GetAppTaskId(), MSG_HR_STATE_CHECK);
			osal_clear_event(GetAppTaskId(), MSG_HR_STATE_CHECK);
			break;

/*ÓÃ»§²Ù×÷*/	
		case MSG_LEFTDOWN_DOUBLE_SHORTKEY:		/*×ó¼ü Ë«»÷*/
		case MSG_LEFTDOWN_LONGKEY:           			/*×ó¼ü ³¤°´*/	
		case MSG_RIGHTDOWN_DOUBLE_SHORTKEY:   	/*ÓÒ¼ü Ë«»÷*/	
		case MSG_RIGHTDOWN_LONGKEY:				/*ÓÒ¼ü ³¤°´*/	
		case MSG_RIGHTDOWN_LONGKEY_UP:			/*ÓÒ¼ü ³¤°´ËÉ¿ª*/		
		case MSG_TOUCH_UP_FLICK:					/*ÉÏ*/
		case MSG_TOUCH_DOWN_FLICK:				/*ÏÂ*/
		case MSG_TOUCH_DOWN_UP_FLICK:				/*×ó*/ 
		case MSG_TOUCH_UP_DOWN_FLICK: 			/*ÓÒ*/		
		case MSG_TOUCH_LONGKEY:					/*´¥Ãþ ³¤°´*/
		case MSG_TOUCH_UP_DOUBLE_SHORTKEY:		/*´¥Ãþ Ë«»÷*/
		case MSG_LEFTDOWN_SHORTKEY:				/*×ó¼ü µ¥»÷*/
			break;
			

						
		case MSG_TOUCH_UP_SHORTKEY:				/*´¥Ãþ µ¥»÷*/  

			{
				if(!HrIsRunning())
				{
					//MotoRun(MOTO_DEFAULT);
					osal_set_event(GetHrTaskId(), HR_OPEN_EVENT);
					SetSystemSuspendTime(15);
				}
				else if(HrIsRunning())
				{
					//MotoRun(MOTO_DEFAULT);
					osal_set_event(GetHrTaskId(), HR_CLOSE_EVENT); 
				}
			}
			break;
			
		case MSG_RIGHTDOWN_SHORTKEY: 				/*ÓÒ¼ü µ¥»÷*/			
			UnloadHandler(DifferentRemindId.heartRate_HandlerId);	
			break;
			
		case MSG_HR_STATE_CHECK:
			hearRate_state_check();
			osal_start_timerEx(GetAppTaskId(), MSG_HR_STATE_CHECK, 1000);
			if(!HrDisplayIsChg(3))
				//SendMsg(MSG_DISPLAY);
			break;
		case MSG_SCREEN_DIR_CHG:
			if(!CheckpCurrentHandler(DifferentRemindId.heartRate_HandlerId))
				break;
		case MSG_DATE_TIME_FORMAT_UPDATE:		
		case MSG_DISPLAY:
			ShowHeartRate(DispalyHrData);	
			break;

		default:
			return RET_MSG_UNHANDLED;
	}
	return RET_MSG_HANDLED;
}

