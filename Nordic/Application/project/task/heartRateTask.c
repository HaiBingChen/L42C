#include "headfile.h"

uint8  HrTaskId;
bool  SyncingStartFlag = false;
uint32 HeartRate_Store_Logs=0;		/*存储的运动记录数*/
uint32 HeartRate_UpLoad_Logs=0;     /*用于记录上传的logs数据*/
UploadSportStateT gHeartRateDataUploadState = UPLOAD_NO;
uint8 AutoHeartRateTime=0;
HR_ALARM hr_alarm;

static bool bGotFirstValidVal = false;
bool isHrReady = false;
bool hrRunning = false;

static UTCTimeStruct uUtcTime_last_hr;

bool isHrAlertChecked = false;

static uint32 AutoHeartRate_start = 0;

void HrInitTask(uint8 task_id)
{
	HrTaskId = task_id;
	//osal_set_event(task_id, AUTO_HEARTRATE_CHECK_EVENT);
}


uint8 GetHrTaskId(void)
{
	return HrTaskId;
}

bool HrIsRunning(void)
{
	return hrRunning;
}


void HrOpenHW(void)
{
	if(HR_PRO_20)
		AS7000_PowerON_ExtALG();
	else
		AS7000_PowerON();
	
}
void HrCloseHW(void)
{
	bGotFirstValidVal = false;

	AS7000_PowerOFF();
}
void HrGetValueHW(void)
{
	if(hrRunning==false)
	{
		isHrReady = false;
		bGotFirstValidVal = false;
		return;
	}
	isHrReady = AS7000_GetHeartRateData(&gHeartrateDisplay.HeartRateVal);
	if((isHrReady)&&(bGotFirstValidVal==false))
		bGotFirstValidVal = true;
}

uint8 HrGetValue(void)
{
	return gHeartrateDisplay.HeartRateVal;
}
bool HrValueValid(void)
{
	return isHrReady;
}
bool HrStarting(void)
{
	return ((hrRunning==true)&&(bGotFirstValidVal==false));
}

bool HrDisplayIsChg(uint8 op)
{
	static bool display_status_chg = false;
	bool rel;
	if(op==0)
	{
		display_status_chg = false;
		rel = display_status_chg;
	}
	else if(op==1)
	{
		display_status_chg = true;
		rel = display_status_chg;
	}
	else if(op==2)
	{
		rel = display_status_chg;
		display_status_chg = false;
	}
	else
	{
		rel = (hrRunning==true)?true:display_status_chg;
		display_status_chg = false;
	}
	
	return rel;
}

void HrGetLastValueTime(UTCTimeStruct *t)
{
	memcpy(t, &uUtcTime_last_hr, sizeof(UTCTimeStruct));
}

uint16 HrEventHandler(uint8 task_id, uint16 events)
{
	static bool collected = false;
	static uint32 hr_open_time = 0;
	static uint32 hr_valid_tick = 0;

	HEARTRATE_DATA_T hr_dat;

	if(events & HR_OP_TOGGLE_EVENT)
	{
		if(hrRunning == false)
			osal_set_event(HrTaskId, HR_OPEN_EVENT);
		else
			osal_set_event(HrTaskId, HR_CLOSE_EVENT);
		return events ^ HR_OP_TOGGLE_EVENT;
	}

	if(events & HR_OPEN_EVENT)
	{
		if(hrRunning==false)
		{
			collected = false;
			hr_open_time = pS_OsGetSysTick();
			hr_valid_tick = pS_OsGetSysTick();
			//open it
			HrOpenHW();
			osal_start_timerEx(HrTaskId, HR_GET_HR_VALUE, 1000);
		}
		HrDisplayIsChg(1);
		hrRunning = true;
		
		return events ^ HR_OPEN_EVENT;
	}
	
	if(events & HR_CLOSE_EVENT)
	{
		osal_stop_timerEx(HrTaskId, HR_CLOSE_EVENT);
		osal_stop_timerEx(HrTaskId, HR_GET_HR_VALUE);

		
		if(hrRunning==true)
		{
			//close it
			HrCloseHW();
		}
		HrDisplayIsChg(1);
		hrRunning = false;
		//AutoHeartRate_start = pS_OsGetSysTick();
		return events ^ HR_CLOSE_EVENT;
	}
	

	if(events & HR_GET_HR_VALUE)
	{
		HrGetValueHW();
		if(HrValueValid())
		{
			hr_valid_tick = pS_OsGetSysTick();
			
			if(pS_OsGetSysTick() - hr_open_time > REPEAT_COLLECT_TIME)
			{
				collected = false;
				hr_open_time = pS_OsGetSysTick();
			}
			
			if(collected==false)
			{
				//coolect 1 time
				collected = true;
				hr_dat.HeartRateTimeStamp = GetTimeStamp();
				hr_dat.HeartRateVal = HrGetValue();
				StoreHeartRateDataToFlash(hr_dat);
			}
			
			usr_ble_hrs_refresh_hr_val(HrGetValue());
			gOS_GetUTCTime(&uUtcTime_last_hr);

			if((HrGetValue() < hr_alarm.HeartRateAlaUpper)&&(HrGetValue() > hr_alarm.HeartRateAlaLower))
				isHrAlertChecked = false;
			
			//if((hr_alarm.Alarm_en)&&(USB_GO_IN_FLAG==false)&&(!isHrAlertChecked))
			if((hr_alarm.Alarm_en)&&(USB_GO_IN_FLAG==false)&&(HrGetValue()>0))
			{
				if(HrGetValue() > hr_alarm.HeartRateAlaUpper)
				{	
					if(!isHrAlertChecked)
					{
						//memcpy(&DispalyHrAlarmData, &DispalyHrData, sizeof(UiHRStruct));
						DispalyHrAlarmData.HR_Value = HrGetValue();
						bLowHR = HR_ALARM_HIGH;	  			
						SendMsg(MSG_HR_UPPER_REMIND);
					}
				}
				else if(HrGetValue() < hr_alarm.HeartRateAlaLower)
				{
					if(!isHrAlertChecked)
					{
						//memcpy(&DispalyHrAlarmData, &DispalyHrData, sizeof(UiHRStruct));
						DispalyHrAlarmData.HR_Value = HrGetValue();
						bLowHR = HR_ALARM_LOW;
						SendMsg(MSG_HR_LOWER_REMIND);
					}
				}
			}

			if(StandbyState.TimeHandlerId != 0)
			{
				SendMsg(MSG_REFRESH_HR);
			}
		}
		else
		{
			if(pS_OsGetSysTick() - hr_valid_tick > COLLECT_TIMEOUT)
			{
				collected = false;
				osal_set_event(HrTaskId, HR_CLOSE_EVENT);
				return events ^ HR_GET_HR_VALUE;
			}
		}

		if((Powerstate.powerstate==false)||(USB_GO_IN_FLAG==true))
		{	
			osal_set_event(HrTaskId, HR_CLOSE_EVENT);
		}			
		else if(hrRunning==true)
		{
			osal_start_timerEx(HrTaskId, HR_GET_HR_VALUE, 1000);
		}
		return events ^ HR_GET_HR_VALUE;
	}


	if(events & AUTO_HEARTRATE_CHECK_EVENT)//自动心率开启监测
	{	
		if((AutoHeartRateTime > 0)
			&&(pS_OsGetSysTick() - AutoHeartRate_start >= 60ul*AutoHeartRateTime*1000ul)
			&&(Powerstate.powerstate==true)
			&&(UPDATE_FONTLIB==false))
		{
			if(hrRunning == false)
			{
				osal_set_event(HrTaskId, HR_OPEN_EVENT);
				osal_start_timerEx(HrTaskId, HR_CLOSE_EVENT, 60ul*1000ul);//close it after 1min
				AutoHeartRate_start = pS_OsGetSysTick();
			}
		}
		hearRate_state_check();	
		osal_start_timerEx(HrTaskId, AUTO_HEARTRATE_CHECK_EVENT, 1000);

		return ( events ^ AUTO_HEARTRATE_CHECK_EVENT );
	}


	if (events & HEARTRATE_CNT_UPLOAD_EVENT)//上传心率数据
	{
		HeartRateUploadData(0);
		return ( events ^ HEARTRATE_CNT_UPLOAD_EVENT );
	}

	

	return 0;
}



void SetHeartRateMemoryFullFlag(bool flag){
    if((MEM_FULL_FLAG != flag)&&(Powerstate.powerstate==true))
    {
    	 MEM_FULL_FLAG = flag;
    	 SendMsg(MSG_MEMORY_CHANGE);
    }
}

void StoreHeartRateDataToFlash(HEARTRATE_DATA_T uData)
{
	if(StoreHeartRateData(&uData,&HeartRate_Store_Logs))
	{
		HeartRate_Store_Logs++;
		WRITE_STEP_SLEEP_SUM(&HeartRate_Store_Logs,HEARTRATE_RECORDS_TOTAL_LOC,HEARTRATE_RECORDS_TOTAL_LEN);
	}
}


bool HeartRateUploadData(uint8 idx)
{
	HEARTRATE_DATA_T  uData;
	if(idx>0){
		HeartRate_UpLoad_Logs = idx-1;
		gHeartRateDataUploadState = UPLOAD_OK;
	}
	if(HeartRate_UpLoad_Logs<HeartRate_Store_Logs)
	{
		SyncingStartFlag = true;
		if(Syncing_Handler_flag==FALSE)
		{
			SendMsg(MSG_SYNCING_START);
			osal_start_timerEx(GetAppTaskId(), MSG_SYNCING_CHECK_CMD, 5000);
		}

		
		GetHeartRateData(&uData,&HeartRate_UpLoad_Logs);
		uData.HeartRateIdx= HeartRate_UpLoad_Logs+1;
		SendHeartRateDataCmd(uData);
#if 1		
		if (BtState != BT_CONNECTED)	 //USB、和蓝牙都断开
		{
			gHeartRateDataUploadState=UPLOAD_NO;
			SendMsg(MSG_SYNCING_STOP);					
			return false;
		}			
#endif

		HeartRate_UpLoad_Logs++;
	}
	else{
		SendMsg(MSG_SYNCING_STOP); 
		gHeartRateDataUploadState = UPLOAD_OK;
		return false;
	}
	return true;
}		



