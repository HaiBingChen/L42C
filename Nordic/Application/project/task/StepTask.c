
#include "headfile.h"

static uint8 StepTaskId;
static uint32 Last10sSteps = 0; 		//记录上个10秒的步数
static uint32 Last1MinSteps = 0; 	//记录上个1分钟的步数
static uint32 Last20sSteps = 0; 		//记录上个20秒的步数
static uint8 DurationSecondCount =0; 


static uint8   SportCnt1min = 0;
static uint16 SportCnt10s =0;

SPORT_DATA_T  gSportStoreDataBackUp;
UploadSportStateT gUploadSportDataState =UPLOAD_NO;
uint32 Step_UpLoad_Logs=0;		/*用于记录上传的logs数据*/
uint32 Step_Store_Logs=0;		/*存储的运动记录数*/
uint16 AutoSleepCount =0;
bool AutoEnterSleep =false;
uint8 Base_Kcal_type;


UploadSportStateT gUploadRecordWorkState =UPLOAD_NO;
uint32 RecordWork_UpLoad_Logs=0;		
uint32 RecordWork_Store_Logs=0;	

int16 ALG_SG[32][3]={0x00};
int16 LastX=0,LastY=0,LastZ=0;
uint8 ALG_Pos=0;

void StepInitTask(uint8 task_id)
{
	StepTaskId = task_id;
}

uint8 GetStepTaskId(void)
{
	return StepTaskId;
}

void SetStepMemoryFullFlag(bool flag)
{
    if((MEM_FULL_FLAG!=flag)&&(Powerstate.powerstate==true))
    {
	    MEM_FULL_FLAG = flag;
	    SendMsg(MSG_MEMORY_CHANGE);
    }
}
void CleanAutoSleepFlag(void)
{
	AutoSleepCount = 0;	
	vSetAutoSleepTime(0,0);
}
void SetAutoSleepTime(uint16 BeginTime,uint16 EndTime)
{		 	
	 vSetAutoSleepTime(BeginTime,EndTime);
}

void StartStep(void)
{
	if(!SLEEP_MODE_FLAG && gInitMark == 1)
	{
		osal_start_timerEx(GetStepTaskId(), STEP_EVENT, 1000);
		osal_start_timerEx(GetStepTaskId(), STEP_CNT_EVENT, 1000);
		SportCnt1min=0;	
		SportCnt10s=0;
		STEP_ON_FLAG=true;
		StopTPInt();
		UnlockI2c();
		if(GsensorType == SENSOR_LIS3DH)
			Lis3dh_Step_SetConfig();
		else if(GsensorType == SENSOR_BMA250E)
			Bma250E_Step_SetConfig();
		StartTPInt();
		}            
	}

void StopStep(void)
{
	STEP_ON_FLAG=false;
	osal_stop_timerEx(GetStepTaskId(), STEP_EVENT);
	osal_stop_timerEx(GetStepTaskId(), STEP_CNT_EVENT);
}

void CloseStep(void)
{
	STEP_ON_FLAG=false;
	osal_stop_timerEx(GetStepTaskId(), STEP_EVENT);
	osal_stop_timerEx(GetStepTaskId(), STEP_CNT_EVENT);
	StopTPInt();
	UnlockI2c();
	if(GsensorType == SENSOR_LIS3DH)
		Lis3dh_Standby();
	else if(GsensorType == SENSOR_BMA250E)
		Bma250E_Standby();
	StartTPInt();
}

void StepInit(void)
{
	Last10sSteps=gSportDisplay.SportSteps;
	Last1MinSteps=gSportDisplay.SportSteps;
	Last20sSteps = gSportDisplay.SportSteps;
	gSportStoreDataBackUp.SportSteps=gSportDisplay.SportSteps;
	gSportStoreDataBackUp.SportKcal=gSportDisplay.SportKcal;
	gSportStoreDataBackUp.SportDistance=gSportDisplay.SportDistance;
	gSportStoreDataBackUp.SportDuration = gSportDisplay.SportDuration;

	gSportDisplayStore.SportSteps=    gSportDisplay.SportSteps;
	gSportDisplayStore.SportKcal=      gSportDisplay.SportKcal;
	gSportDisplayStore.SportDistance=gSportDisplay.SportDistance;
	gSportDisplayStore.SportDuration=gSportDisplay.SportDuration;
	SetDisplySportData();
		
	vStepCounterInit();
	SetSteps(gSportDisplay.SportSteps);

	CleanAutoSleepFlag();
	StartStep();
	vGesture_Recog_Init_ALG();
}

void ClearSportData(void)
{
	Last10sSteps = 0; 
	Last1MinSteps = 0;
	Last20sSteps = 0;
	DurationSecondCount =0;
	SportCnt1min = 0;
	SportCnt10s =0;
	
	gSportDisplay.SportSteps=0;
	gSportDisplay.SportKcal=0;
	gSportDisplay.SportDistance=0;
	gSportDisplay.SportDuration=0;
	
	gSportStoreDataBackUp.SportSteps=0;
	gSportStoreDataBackUp.SportKcal=0;
	gSportStoreDataBackUp.SportDistance=0;
	gSportStoreDataBackUp.SportDuration=0;
	
	for(uint8 i =0; i<4;i++)
	{
		if(DailyGoal[i].MainGoalFlag==1)
			DailyGoalOverRemindEnable|=(1<<i);
		else
			DailyGoalOverRemindEnable&=~(1<<i);
	}
	gSportDisplayStore.SportSteps=    gSportDisplay.SportSteps;
	gSportDisplayStore.SportKcal=      gSportDisplay.SportKcal;
	gSportDisplayStore.SportDistance=gSportDisplay.SportDistance;
	gSportDisplayStore.SportDuration=gSportDisplay.SportDuration;
	SetDisplySportData();	
	vClearAllStat();
	ClearSitRemindEvent();
}


uint32 GetDistance(uint32 Steps)
{
	uint16 FootAge=0;
	if(UserInfoData.UserSex==0x00||UserInfoData.UserSex==0xff){
		FootAge = UserInfoData.UserHeight*0.415;
	}
	else if(UserInfoData.UserSex==0x01){
		FootAge = UserInfoData.UserHeight*0.413;
	}
	return (uint32)((Steps*FootAge)/100);
}
uint32 GetSportKcal(uint32 Steps)
{
	uint32 cal = 0;
	if(Steps < 40)
		cal = (UserInfoData.UserWeight * 176ul);
	else if(Steps < 80)
		cal = (UserInfoData.UserWeight * 176ul * 15ul);
	else if(Steps < 100)
		cal = (UserInfoData.UserWeight * 176ul * 20ul);
	else if(Steps < 120)
		cal = (UserInfoData.UserWeight * 176ul * 35ul);
	else if(Steps < 140)
		cal = (UserInfoData.UserWeight * 176ul * 55ul);
	else if(Steps < 160)
		cal = (UserInfoData.UserWeight * 176ul * 80ul);
	else if(Steps < 180)
		cal = (UserInfoData.UserWeight * 176ul * 110ul);
	else
		cal = (UserInfoData.UserWeight * 176ul * 140ul);
	return (uint32)(cal / 10000ul);
}

uint32 CalBaseKcal(uint32 min)
{
	uint32 BaseCalory;
	
	if(UserInfoData.UserSex>1)
		UserInfoData.UserSex = 1;
	
	BaseCalory = 5ul*((889ul*UserInfoData.UserHeight + 14ul*UserInfoData.UserWeight - 699ul*UserInfoData.UserAge + 1414ul)/1024ul - 23ul*UserInfoData.UserSex);
	if(BaseCalory>3700)
		BaseCalory = 3700;
	return (BaseCalory * min);
}

void StartStepCounter(void)
{
	StopTPInt();
	UnlockI2c();
	int16 ALGDataBuf[32][3]={0x00};
	if(GsensorType == SENSOR_LIS3DH)
	{
		uint16 dly = 24;
		uint8 frame_count = 0;
		do
		{
			Gsensor_get_fifo_framecount(&frame_count);
			if(frame_count >= 0x20) break;
			nrf_delay_ms(10);
			//TRACE("dly = %d\r\n",dly);
		}while(--dly);
		
		Gsensor_receive_data(&ALGDataBuf[0][0]);
		
		//MemoryCopy(ALG_data,ALGDataBuf,192);
		
		for (register uint8 i = 0; i < STEP_BUFFER_FIFO_CNT; i++) {
			ALGDataBuf[i][0] = ALGDataBuf[i][0] ;
			ALGDataBuf[i][1] = ALGDataBuf[i][1] ;
			ALGDataBuf[i][2] = ALGDataBuf[i][2] ;
			if((frame_count < 0x20)&&(i>(STEP_BUFFER_FIFO_CNT>>1))&&(ALGDataBuf[i][0]==0&&ALGDataBuf[i][1]==0 && ALGDataBuf[i][2]==0))	
			{
				ALGDataBuf[i][0]=ALGDataBuf[i-1][0];
				ALGDataBuf[i][1]=ALGDataBuf[i-1][1];
				ALGDataBuf[i][2]=ALGDataBuf[i-1][2];
			}
			//TRACE("%d,%d,%d\r\n",ALGDataBuf[i][0],ALGDataBuf[i][1],ALGDataBuf[i][2]);
		}
		vStepCounter(ALGDataBuf,32);
		if(DISPLAY_GOTO_SLEEP_FLAG && setting_info.wrist)
		{

			for(register uint8 i = 0; i < STEP_BUFFER_FIFO_CNT; i++)
		    {
				vGesture_Recog_ALG(ALGDataBuf[i]);
				uint8 gesture_stat = ucGetGestureType_ALG(); 
				if(gesture_stat == GT_HAND_UP || gesture_stat == GT_HAND_ROTATION)
				{
					SendMsg(MSG_TOUCH_UP_DOUBLE_SHORTKEY_FOR_RESUME);
					gesture_stat = GT_DEFAULT;
					break;
				}
		    }
		}
	}
	else if(GsensorType == SENSOR_BMA250E)
	{
		uint8 DataBuf[6]={0x00};
		for (register uint8 i = 0; i < STEP_BUFFER_FIFO_CNT; i++) {
			Bma2x2GetFifoData(DataBuf);
			ALGDataBuf[i][0] = GetAccData(&DataBuf[2]);
			ALGDataBuf[i][1] = GetAccData(&DataBuf[0]);
			ALGDataBuf[i][2] = GetAccData(&DataBuf[4]);
		}
		vStepCounter(ALGDataBuf,32);
	}
	StartTPInt();
}

void StartStepCounterWithGesture(void)
{
	StopTPInt();
	UnlockI2c();
	uint8 DataBuf[6]={0x00};
	uint8 vGestureBegin = 0,vGestureEnd =0;
	vGestureBegin = ALG_Pos;
	for (register uint8 i = 0; i < STEP_BUFFER_FIFO_CNT; i++) {
		Bma2x2GetFifoData(DataBuf);
		if(i<16 && ALG_Pos<32)
		{
			ALG_SG[ALG_Pos][0] = GetAccData(&DataBuf[2]);
			ALG_SG[ALG_Pos][1] = GetAccData(&DataBuf[0]);
			ALG_SG[ALG_Pos][2] = GetAccData(&DataBuf[4]);
			if(ALG_SG[ALG_Pos][0]==0&&ALG_SG[ALG_Pos][1]==0 && ALG_SG[ALG_Pos][2]==0)	
			{
				ALG_SG[ALG_Pos][0]=LastX;
				ALG_SG[ALG_Pos][1]=LastY;
				ALG_SG[ALG_Pos][2]=LastZ;
			}
			LastX = ALG_SG[ALG_Pos][0];
			LastY = ALG_SG[ALG_Pos][1];
			LastZ = ALG_SG[ALG_Pos][2];
			ALG_Pos++;
		}
	}
	vGestureEnd = ALG_Pos;
	if(ALG_Pos==32)
	{
		vStepCounter(ALG_SG,32);
		ALG_Pos=0;
	}
	if(DISPLAY_GOTO_SLEEP_FLAG)
	{
		for(register uint8 i = vGestureBegin; i < vGestureEnd; i++)
	   	{
			vGesture_Recog_ALG(ALG_SG[i]);
			uint8 gesture_stat = ucGetGestureType_ALG(); 
			if(gesture_stat == GT_HAND_UP || gesture_stat == GT_HAND_ROTATION)
			{
				SendMsg(MSG_TOUCH_UP_DOUBLE_SHORTKEY_FOR_RESUME);
				gesture_stat = GT_DEFAULT;
				break;
			}
	    	}
	}
	StartTPInt();
}

void InitGesture(void)
{
	if(GsensorType == SENSOR_BMA250E)
	{
		ALG_Pos =0;
		LastX=0;
    	LastY=0;
    	LastZ=0;
	}
}

void StoreSportDataToFlash(uint8 flag)
{
	SPORT_DATA_T tSportStoreData;
	tSportStoreData.SportSteps=gSportDisplay.SportSteps-gSportStoreDataBackUp.SportSteps;				 	//步数
	tSportStoreData.SportKcal=gSportDisplay.SportKcal-gSportStoreDataBackUp.SportKcal;						//卡路里
	tSportStoreData.SportDuration=gSportDisplay.SportDuration-gSportStoreDataBackUp.SportDuration;	 			//运动时长
	if (tSportStoreData.SportSteps>0||tSportStoreData.SportKcal>0||tSportStoreData.SportDuration>0)
	{ 
		if(Step_Store_Logs < STORE_STEP_DATA_MAX)
		{
			if(flag==0)
				tSportStoreData.SportTimeStamp=GetTimeStamp();  											//时间戳    
			else
				tSportStoreData.SportTimeStamp=GetTimeStamp()-90; 										//0点时减去一分半钟。
				
			gSportDisplay.SportDistance = GetDistance(gSportDisplay.SportSteps);			
			tSportStoreData.SportDistance=gSportDisplay.SportDistance-gSportStoreDataBackUp.SportDistance;	 	//距离
					
			if(tSportStoreData.SportTimeStamp == gSportStoreDataBackUp.SportTimeStamp)
				tSportStoreData.SportTimeStamp+=1;
			
			gSportStoreDataBackUp.SportTimeStamp = tSportStoreData.SportTimeStamp;
			gSportStoreDataBackUp.SportSteps=gSportDisplay.SportSteps;
			gSportStoreDataBackUp.SportKcal=gSportDisplay.SportKcal;
			gSportStoreDataBackUp.SportDistance=gSportDisplay.SportDistance;
			gSportStoreDataBackUp.SportDuration=gSportDisplay.SportDuration;
			
			StoreStepData(&Step_Store_Logs,&tSportStoreData);												//存储采样数据
			Step_Store_Logs++;
			WRITE_STEP_SLEEP_SUM(&Step_Store_Logs,SPORT_TOTAL_LOC,SPORT_TOTAL_LEN);
			if(flag==0)
				SetDisplySportData();	
		}
		else
		{
			SetStepMemoryFullFlag(true);			
		}
	}
}

bool StepUploadData(uint8 idx)
{
	SPORT_DATA_T  uData;
	if(idx>0)
	{
		Step_UpLoad_Logs = idx-1;
		gUploadSportDataState = UPLOAD_OK;
	}
	if(Step_UpLoad_Logs<Step_Store_Logs)
	{
		SyncingStartFlag = true;
		if(Syncing_Handler_flag==FALSE)
		{
			SendMsg(MSG_SYNCING_START);
			osal_start_timerEx(GetAppTaskId(), MSG_SYNCING_CHECK_CMD, 2000);
		}
		
		GetStepData(&Step_UpLoad_Logs,&uData);
		uData.SportIdx = Step_UpLoad_Logs+1;
		SendSportDataCmd(uData);
#if 1		
		if (BtState != BT_CONNECTED) 	 //USB、和蓝牙都断开
		{
			gUploadSportDataState=UPLOAD_NO;
			SendMsg(MSG_SYNCING_STOP);                  
			return false;
		}			
#endif		
		Step_UpLoad_Logs++;
	}
	else{
		SendMsg(MSG_SYNCING_STOP); 
		gUploadSportDataState = UPLOAD_OK;
		return false;
	}
	return true;
}		
	

uint16 StepEventHandler(uint8 task_id, uint16 events)
{
	uint8 DailyGoalOverTemp = 0;

	if (events & STEP_CNT_EVENT) { 
		if (!SLEEP_MODE_FLAG) { 		
			if(GsensorType == SENSOR_LIS3DH)
			{
				StartStepCounter();
				gSportDisplay.SportSteps=GetSteps();	
				osal_start_timerEx(GetStepTaskId(), STEP_CNT_EVENT, 660);              //32/50=0.64
			}
			else if(GsensorType == SENSOR_BMA250E)
			{
				if(setting_info.wrist)
				{				
					StartStepCounterWithGesture();
					gSportDisplay.SportSteps=GetSteps();			
					osal_start_timerEx(GetStepTaskId(), STEP_CNT_EVENT, 520); 							
				}
				else
				{
					StartStepCounter();
					gSportDisplay.SportSteps=GetSteps();			
					osal_start_timerEx(GetStepTaskId(), STEP_CNT_EVENT, 1100); 			// 1s定时器
				}
			}
		}
		return ( events ^ STEP_CNT_EVENT );
	}

	if (events & STEP_EVENT) { 
		if ((!SLEEP_MODE_FLAG)&&(gInitMark == 1) ) { 
			osal_start_timerEx(task_id, STEP_EVENT, STEP_EVENT_PERIOD); 			// 10s定时器
			
			if(AutoSleepMode() == 1)
			{
				ChargeDisplayStateChange(false);
				AutoEnterSleep =true;
				SLEEP_MODE_DISPLAY_FLAG=true;
				SleepKey=0;
				osal_start_timerEx(GetAppTaskId(), MSG_SLEEP_MODE_START, 500);
			}
			
			uint32 NowSteps=GetSteps();
			uint32 StepsChange =NowSteps-Last10sSteps; 
			bool CheckGoal=false;
			Last10sSteps = NowSteps;
			SportCnt1min++;	
			SportCnt10s++;

			//每隔10秒，有步数产生,获取步数和计算距离
			if(StepsChange>0)			
			{
				gSportDisplay.SportSteps=NowSteps;
				gSportDisplay.SportDistance = GetDistance(gSportDisplay.SportSteps);
				CheckGoal =true;
			}
			
			//每隔20秒，有步数产生,获取运动时长
			if((SportCnt10s%2)==0)
			{
				if(NowSteps>Last20sSteps)
				{
					DurationSecondCount+=20;
					if(DurationSecondCount==60)
					{
						DurationSecondCount=0;
						gSportDisplay.SportDuration +=1;
						CheckGoal =true;
					}
				}
				Last20sSteps = NowSteps;
			}	
			
			//每隔1分钟，计算动态卡路里和静态卡路里
			if(SportCnt1min>=6)
			{
				if(NowSteps>Last1MinSteps)		// 1分钟内有步数 计算动态卡路里
				{
					gSportDisplay.SportKcal += GetSportKcal(NowSteps-Last1MinSteps);
					CheckGoal =true;
				}

				if((Base_Kcal_type==1) && (!USB_GO_IN_FLAG))
				{
					gSportDisplay.SportKcal += CalBaseKcal(1);
					CheckGoal =true;
				}
				
				Last1MinSteps=NowSteps;
				SportCnt1min = 0;
			}

			//每隔30分钟，保存运动数据到flash
			if(SportCnt10s>=180)
			{
				SportCnt10s = 0;
				StoreSportDataToFlash(0);			
			}

			//目标监测值有变化，检查是否目标达成
			if(CheckGoal)
			{
				DailyGoalOverTemp = CheckSportGoalOverRemind();
				if (DailyGoalOverTemp>0)                 
				{
					DailyGoalOverType = DailyGoalOverTemp;
					SendMsg(MSG_GOAL_OVER_NOTIFY);
				}
				if(!isSystemSupend())
				{
					SendMsg(MSG_REFRESH_STEP);
				}
			}
		}
		return ( events ^ STEP_EVENT );
	}

	if (events & STEP_RECORD_UPLOAD_EVENT) { //上传运动数据
		StepUploadData(0);
		return ( events ^ STEP_RECORD_UPLOAD_EVENT );
	}
	
	if (events & RECORDWORK_UPLOAD_EVENT) { //上传运动数据
		RecordWorkUploadData(0);
		return ( events ^ RECORDWORK_UPLOAD_EVENT );
		
	}
	return (0);
}

bool RecordWorkUploadData(uint8 idx)
{
	REC_WORK_DATA_T  uData;
	if(idx>0)
	{
		RecordWork_UpLoad_Logs = idx-1;
		gUploadRecordWorkState = UPLOAD_OK;
	}
	if(RecordWork_UpLoad_Logs<RecordWork_Store_Logs)
	{
		SyncingStartFlag = true;
		if(Syncing_Handler_flag==FALSE)
		{
			SendMsg(MSG_SYNCING_START);
			osal_start_timerEx(GetAppTaskId(), MSG_SYNCING_CHECK_CMD, 2000);
		}
		
		GetRecordWorkData(&RecordWork_UpLoad_Logs,&uData);
		uData.Idx = RecordWork_UpLoad_Logs+1;
		SendRecordWorkDataCmd(uData);
#if 1	
		if (BtState != BT_CONNECTED) 	 //USB、和蓝牙都断开
		{
			gUploadRecordWorkState=UPLOAD_NO;
			SendMsg(MSG_SYNCING_STOP);                  
			return false;
		}			
#endif	
		RecordWork_UpLoad_Logs++;
	}
	else{
		SendMsg(MSG_SYNCING_STOP); 
		gUploadRecordWorkState = UPLOAD_OK;
		if(RECORDWORK_SAVE_FLAG)
		{
			RecordWork_Store_Logs=0;
			Force_NordicFlash();
			WRITE_STEP_SLEEP_SUM(&RecordWork_Store_Logs,RECORD_WORK_LOC,RECORD_WORK_LEN);
			SaveRecordWork();
			Free_NordicFlash();
			RECORDWORK_SAVE_FLAG =false;
		}
		else
		{
			RecordWork_Store_Logs=0;
			WRITE_STEP_SLEEP_SUM(&RecordWork_Store_Logs,RECORD_WORK_LOC,RECORD_WORK_LEN);
		}
		return false;
	}
	return true;
}	

