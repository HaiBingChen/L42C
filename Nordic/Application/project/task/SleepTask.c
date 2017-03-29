#include "headfile.h"

static uint8 SleepTaskId=0;
uint32 Sleep_UpLoad_Logs=0;		/*用于记录上传的logs数据*/
uint32 Sleep_Store_Logs=0;		/*存储的运动记录数*/

uint32 gSleepTimeStart=0;
uint32 gSleepTimeStop=0;

uint16 MinuteCount=0;        		/*统计睡眠时候的分钟数*/
uint8   SecondCount=0;        		/*统计睡眠时候的秒数*/
static uint16 SleepCnt30min =0;
UploadSportStateT gSleepDataUploadState=UPLOAD_NO;

uint32 Prepare_Time = 0;
uint32 Awake_Time = 0;
uint32 Light_Time = 0;
uint32 Asleep_Time = 0;

uint8 Last_SleepType = 0;

uint8 GetLast_SleepType(void)
{
	return Last_SleepType;
}

void SetLast_SleepType(uint8 SleepType)
{
	Last_SleepType = SleepType;
}

void SleepInitTask(uint8 task_id)
{
	SleepTaskId=task_id;
}

uint8 GetSleepTaskId(void)
{
	return SleepTaskId;
}

void SetSleepMemoryFullFlag(bool flag){
    if((MEM_FULL_FLAG!=flag)&&(Powerstate.powerstate==true))
    {
    	 MEM_FULL_FLAG = flag;
    	 SendMsg(MSG_MEMORY_CHANGE);
    }
}
/*存储睡眠详细*/
void InitSleepDetailForDisplay(void)
{
	uint8 SleepDaySave[128]={0x00};
	SpiFlash_Read(SLEEP_DETAIL_BASE_ADDR,SleepDaySave,128);
	SleepDayCount=0;
	memset(SleepDay,0,sizeof(SleepMapData)*7);
	for(uint8 i=0; i<8;i++)
	{
		if(SleepDaySave[14*i]==0xFF||SleepDaySave[14*i]==0x00)
		{
			SleepDayCount=i;
			break;
		}
	}
	if(SleepDayCount>0)
		memcpy(SleepDay,SleepDaySave,sizeof(SleepMapData)*SleepDayCount);
}
void StoreSleepDetailForDisplay(uint32 TimeStamp, uint16 AwakeM, uint16 LightM,uint16 DeepM, uint16 TotalM)
{
	uint8 findIndex=0;
	uint8 SleepDaySave[128]={0x00};
	bool HasExist=false;
	UTCTimeStruct uTime;
	uint32 SecToTime = TimeStamp-BEGYEAR_STAMP;
	osal_ConvertUTCTime(&uTime,SecToTime);
	uTime.day+=1;
  	uTime.month+=1;
	for(uint8 i=0; i<SleepDayCount;i++)
	{
		if(uTime.year==SleepDay[i].year && uTime.month==SleepDay[i].month && uTime.day==SleepDay[i].day)
		{
			findIndex=i;
			HasExist=true;
			break;
		}
	}

	if(HasExist)		/*原有*/
	{
		
	}
	else				/*新增*/
	{
		if(SleepDayCount<7)
		{
			findIndex = SleepDayCount;
			SleepDayCount++;
		}
		else
		{
			for(uint8 j=0; j<7; j++)
			{
				SleepDay[j].day=SleepDay[j+1].day;
				SleepDay[j].month=SleepDay[j+1].month;
				SleepDay[j].year=SleepDay[j+1].year;
				SleepDay[j].total=SleepDay[j+1].total;
				SleepDay[j].Awake=SleepDay[j+1].Awake;
				SleepDay[j].Light=SleepDay[j+1].Light;
				SleepDay[j].Deep=SleepDay[j+1].Deep;
			}
			findIndex = 6;
		}
	}

	SleepDay[findIndex].year=uTime.year;
	SleepDay[findIndex].month = uTime.month;
	SleepDay[findIndex].day= uTime.day;
	SleepDay[findIndex].total+=TotalM;
	SleepDay[findIndex].Awake+=AwakeM;
	SleepDay[findIndex].Light+=LightM;
	SleepDay[findIndex].Deep+=DeepM;
	//PRINTF("SleepDay:(%d)(%d) %d-%d-%d %d-%d-%d-%d\r\n",findIndex,SleepDayCount,SleepDay[findIndex].year,SleepDay[findIndex].month,SleepDay[findIndex].day,
		//SleepDay[findIndex].total,SleepDay[findIndex].Awake,SleepDay[findIndex].Light,SleepDay[findIndex].Deep);
	memcpy(SleepDaySave,SleepDay,sizeof(SleepMapData)*7);
	SpiFlash_4KSector_Erase(SLEEP_DETAIL_BASE_ADDR);
	SpiFlash_Write(SLEEP_DETAIL_BASE_ADDR,SleepDaySave,128);		
}




void StoreSleepDataToFlash(SLEEP_DATA_T uData)
{
	static uint32 TimeStamp=0;
	static uint8 SleepType = 0;
	static uint8 sleep_tag = 0;
	
	if(StoreSleepData(&uData,&Sleep_Store_Logs))
	{
		Sleep_Store_Logs++;
		WRITE_STEP_SLEEP_SUM(&Sleep_Store_Logs,SLEEP_TOTAL_LOC,SLEEP_TOTAL_LEN);
	}
	
	if(uData.SleepType == AWAKE || uData.SleepType == STOP_SLEEP|| uData.SleepType == PREPARED)
	{
		uint8 LastSleepType = GetLast_SleepType();
		if(LastSleepType == LIGHT ||LastSleepType == ASLEEP)
			sleep_info.awake++;
	}
	
	SetLast_SleepType(uData.SleepType);
	
	if (sleep_tag)
	{
		if (SleepType == PREPARED)
		{
			if (uData.SleepTimeStamp >= TimeStamp)
			{
				sleep_info.fell_sleep += (uData.SleepTimeStamp - TimeStamp)/60;
				Prepare_Time += (uData.SleepTimeStamp - TimeStamp);
			}
		}
		else if (SleepType == AWAKE)
		{

			if (uData.SleepTimeStamp >= TimeStamp)
			{
				sleep_info.awake_sleep += (uData.SleepTimeStamp - TimeStamp)/60;
				Awake_Time += (uData.SleepTimeStamp - TimeStamp);
			}
			//sleep_info.awake++;
		}
		else if (SleepType == LIGHT)
		{
			if (uData.SleepTimeStamp >= TimeStamp)
			{
				sleep_info.light_sleep += (uData.SleepTimeStamp - TimeStamp)/60;
				Light_Time += (uData.SleepTimeStamp - TimeStamp);
			}

		}
		else if (SleepType == ASLEEP)
		{
			if (uData.SleepTimeStamp >= TimeStamp)
			{
				sleep_info.deep_sleep += (uData.SleepTimeStamp - TimeStamp)/60;
				Asleep_Time += (uData.SleepTimeStamp - TimeStamp);
			}
		}
		else
		{
		}
	}

	if (uData.SleepType == START_SLEEP)
	{
		sleep_info.total_sleep = 0x00;
		sleep_info.fell_sleep = 0x00;
		sleep_info.light_sleep = 0x00;
		sleep_info.deep_sleep = 0x00;
		sleep_info.awake_sleep = 0x00;
		sleep_info.awake = 0x00;
		sleep_tag = 1;
		TimeStamp = 0;
		SleepType = 0;
		Prepare_Time = 0;
		Awake_Time = 0;
		Light_Time = 0;
		Asleep_Time = 0;
	}

	if ((uData.SleepType == STOP_SLEEP) || (uData.SleepType == AUTO_SLEEP))
	{
		sleep_tag = 0;
		sleep_info.fell_sleep = Prepare_Time/60;
		sleep_info.awake_sleep = Awake_Time/60+sleep_info.fell_sleep;
		sleep_info.light_sleep = Light_Time/60;
		sleep_info.deep_sleep= Asleep_Time/60;
		sleep_info.total_sleep = sleep_info.awake_sleep + sleep_info.light_sleep + sleep_info.deep_sleep;
		StoreSleepDetailForDisplay(uData.SleepTimeStamp,sleep_info.awake_sleep,sleep_info.light_sleep,sleep_info.deep_sleep,sleep_info.total_sleep);
		//WRITE_USER_PARAMETER((uint8 *)&sleep_info, SLEEP_LOC, SLEEP_LEN);
	}

	TimeStamp = uData.SleepTimeStamp;
	SleepType = uData.SleepType;
	
}


void GotoSleepMode(void)
{
	if(!SLEEP_MODE_FLAG)  
	{
		StopStep();
		StoreSportDataToFlash(0);
		if (STui == ST_RUN)
		{
			STui = ST_STOP;
			gStopWatchLastMsSave = gStopWatchLastMs;
		}
	}	
	StopTPInt();
	if(GsensorType == SENSOR_LIS3DH)
	{
		Lis3dh_Step_SetConfig();
		Lis3dh_Sleep_SetConfig();
	}
	else if(GsensorType == SENSOR_BMA250E)
	{
		Bma250E_Step_SetConfig();
		Bma250E_Sleep_SetConfig();
	}
	StartTPInt();
	gSleepTimeStart=GetTimeStamp();//初始化睡眠计数	
	SleepMonitorInit(); //准备休眠函数
	MinuteCount = 0 ;
	SecondCount = 0;
	SleepCnt30min=0;
	IsGoingToPowerDown = false;
	SLEEP_MODE_FLAG=true;
	SWITCH_ENTER_SLEEP = true;
	SLEEP_DATA_SAVE=false;
	CleanAutoSleepFlag();	
	//进入睡眠模式
	osal_set_event(SleepTaskId, SLEEP_ALG_RUNNING_EVENT);
}


void GotoNormalMode(bool force)
{
	int16 acc[3]={0};
	SLEEP_DATA_T uData;	   
	if(force==true)
	{
		SLEEP_MODE_FLAG=false;  
		SWITCH_ENTER_SLEEP = false;
	}
	CleanAutoSleepFlag();
	osal_stop_timerEx(SleepTaskId, SLEEP_ALG_RUNNING_EVENT);
	osal_clear_event(SleepTaskId, SLEEP_ALG_RUNNING_EVENT);
	gSleepTimeStop = GetTimeStamp();
	if(GsensorType == SENSOR_LIS3DH)
		Gsensor_read_accel(acc);	
	else if(GsensorType == SENSOR_BMA250E)
	{
		bma2x2_read_accel_x(&acc[0]);
		bma2x2_read_accel_y(&acc[1]);
		bma2x2_read_accel_z(&acc[2]);		
	}
	QuitSleepMode(acc,gSleepTimeStop-gSleepTimeStart);   //结束算法睡眠
	SLEEP_DATA_SAVE =true;
	uData.SleepTimeStamp= gSleepTimeStart;
	uData.SleepType=START_SLEEP;
	StoreSleepDataToFlash(uData);
	SLEEP_DATA_SET=false;
	osal_set_event(SleepTaskId, SLEEP_STORE_DATA_EVENT);
	if(!IsGoingToPowerDown)
	{
		osal_start_timerEx(GetAppTaskId(), MSG_SLEEP_OPEN_BT, 300);
		if(CheckSleepGoalOverRemind()>0)
			osal_start_timerEx(GetAppTaskId(), MSG_GOAL_OVER_NOTIFY, 3000);	
	}
}


bool SleepUploadData(uint8 idx)
{
	SLEEP_DATA_T  uData;
	if(idx>0){
		Sleep_UpLoad_Logs = idx-1;
		gSleepDataUploadState = UPLOAD_OK;
	}
	if(Sleep_UpLoad_Logs<Sleep_Store_Logs)
	{
		SyncingStartFlag = true;
		if(Syncing_Handler_flag==FALSE)
		{
			SendMsg(MSG_SYNCING_START);
			osal_start_timerEx(GetAppTaskId(), MSG_SYNCING_CHECK_CMD, 5000);
		}

		
		GetSleepData(&uData,&Sleep_UpLoad_Logs);
		uData.SleepIdx= Sleep_UpLoad_Logs+1;
		SendSleepDataCmd(uData);
#if 1		
		if (BtState != BT_CONNECTED) 	 //USB、和蓝牙都断开
		{
			gSleepDataUploadState=UPLOAD_NO;
			SendMsg(MSG_SYNCING_STOP);                  
			return false;
		}		
#endif		
		Sleep_UpLoad_Logs++;
	}
	else{
		SendMsg(MSG_SYNCING_STOP); 
		gSleepDataUploadState = UPLOAD_OK;
		return false;
	}
	return true;
}		



uint16 SleepEventHandler(uint8 task_id, uint16 events)
{
	if (events & SLEEP_ALG_RUNNING_EVENT)//休眠算法计算
	{
		if(SLEEP_MODE_FLAG)
		{
			int16 acc[3]={0};
			
			StopTPInt();
			UnlockI2c();
			if(GsensorType == SENSOR_LIS3DH)
				Gsensor_read_accel(acc);	
			else if(GsensorType == SENSOR_BMA250E)
			{
				bma2x2_read_accel_x(&acc[0]);
				bma2x2_read_accel_y(&acc[1]);
				bma2x2_read_accel_z(&acc[2]);		
			}
			StartTPInt();	
			
			SleepMonitor(acc, GetTimeStamp()-gSleepTimeStart);   //开启算法	
			SecondCount ++;
			if(SecondCount>59){
				SecondCount = 0;
				MinuteCount++;
				SleepCnt30min++;
				if((Base_Kcal_type==1) && (!USB_GO_IN_FLAG))
				{
					gSportDisplay.SportKcal += CalBaseKcal(1);
					//每隔30分钟，保存运动数据到flash
					if(SleepCnt30min>=30)
					{
						SleepCnt30min = 0;
						StoreSportDataToFlash(0);			
					}	
				}					
				if(GetSleepState()==QUIT) //自动退出休眠
				{
					CleanAutoSleepFlag();			
                    			ChargeDisplayStateChange(false);
					SLEEP_MODE_DISPLAY_FLAG=false;
                    			osal_start_timerEx(GetAppTaskId(), MSG_SLEEP_MODE_EXIT, 200);
					return ( events ^ SLEEP_ALG_RUNNING_EVENT );
				}
			}
			osal_start_timerEx(task_id, SLEEP_ALG_RUNNING_EVENT,1000);  
		}
		return ( events ^ SLEEP_ALG_RUNNING_EVENT );
	}

  
	if (events & SLEEP_CNT_UPLOAD_EVENT)//上传睡眠数据
	{
		SleepUploadData(0);             
		return ( events ^ SLEEP_CNT_UPLOAD_EVENT );
	}

  
	if( events & SLEEP_STORE_DATA_EVENT )
	{
		SLEEP_DATA_T uData;
		uint32 NodeBuff[2]={0x00};

		if(GetSleepTimeNodeSingle(NodeBuff,gSleepTimeStart,gSleepTimeStop) == 0) { // 转换完成
			if(AutoEnterSleep)
				uData.SleepType=AUTO_SLEEP;
			else
				uData.SleepType=STOP_SLEEP;
			if(SLEEP_DATA_SET)
				uData.SleepTimeStamp= gSleepTimeStop;
			else		
				uData.SleepTimeStamp= GetTimeStamp();
			StoreSleepDataToFlash(uData);
			SLEEP_DATA_SAVE =false;
			if(!IsGoingToPowerDown)    
			{
				if(!SLEEP_MODE_FLAG)
					StartStep();   //开启计步 
				else
					GotoSleepMode();	//继续睡眠
			}
			else
				IsGoingToPowerDown = false;
		}
		else {
			uData.SleepTimeStamp = NodeBuff[0];
			uData.SleepType=NodeBuff[1] & 0xff;
			StoreSleepDataToFlash(uData);    
			osal_start_timerEx(SleepTaskId, SLEEP_STORE_DATA_EVENT, 450);
		}
		
		return ( events ^ SLEEP_STORE_DATA_EVENT );
	}


	if( events & SLEEP_BUTTON_PRESS_EVENT )
	{    
		SetButtonPress(1);
		return ( events ^ SLEEP_BUTTON_PRESS_EVENT );
	}  
	return (0);   
}


void ClearSleepData(bool force)
{
	if (!SLEEP_MODE_FLAG) {
		MinuteCount = 0;
		SecondCount = 0;
		SleepMonitorInit();
	}
	else if (force)				/*强制保存睡眠数据，绑定使用*/
	{
		MinuteCount = 0;
		SecondCount = 0;		
		GotoNormalModeWhenBond();
	}
}

void GotoNormalModeWhenDateTimeSetting(uint32 times)
{
	int16 acc[3]={0};
	SLEEP_DATA_T uData;	   
	CleanAutoSleepFlag();
	osal_stop_timerEx(SleepTaskId, SLEEP_ALG_RUNNING_EVENT);
	osal_clear_event(SleepTaskId, SLEEP_ALG_RUNNING_EVENT);
	gSleepTimeStop = times;
	if(GsensorType == SENSOR_LIS3DH)
		Gsensor_read_accel(acc);	
	else if(GsensorType == SENSOR_BMA250E)
	{
		bma2x2_read_accel_x(&acc[0]);
		bma2x2_read_accel_y(&acc[1]);
		bma2x2_read_accel_z(&acc[2]);		
	}
	QuitSleepMode(acc,gSleepTimeStop-gSleepTimeStart);   //结束算法睡眠
	SLEEP_DATA_SAVE =true;
	uData.SleepTimeStamp= gSleepTimeStart;
	uData.SleepType=START_SLEEP;
	StoreSleepDataToFlash(uData);
	SLEEP_DATA_SET=true;
	osal_set_event(SleepTaskId, SLEEP_STORE_DATA_EVENT);
	if(!IsGoingToPowerDown)
		osal_start_timerEx(GetAppTaskId(), MSG_SLEEP_OPEN_BT, 300);
}

void ClearSleepDataWhenDateTimeSetting(uint32 times)
{
	if (!SLEEP_MODE_FLAG) {
		MinuteCount = 0;
		SecondCount = 0;
		SleepMonitorInit();
	}
	else 						
		GotoNormalModeWhenDateTimeSetting(times);
}

void GotoNormalModeWhenBond(void)		/*退出睡眠不保存数据*/
{
	int16 acc[3]={0};
	SLEEP_MODE_FLAG=false;  
	SWITCH_ENTER_SLEEP = false;
	CleanAutoSleepFlag();
	osal_stop_timerEx(SleepTaskId, SLEEP_ALG_RUNNING_EVENT);
	osal_clear_event(SleepTaskId, SLEEP_ALG_RUNNING_EVENT);
	osal_stop_timerEx(SleepTaskId, SLEEP_STORE_DATA_EVENT);
	osal_clear_event(SleepTaskId, SLEEP_STORE_DATA_EVENT);	
	gSleepTimeStop = GetTimeStamp();
	if(GsensorType == SENSOR_LIS3DH)
		Gsensor_read_accel(acc);	
	else if(GsensorType == SENSOR_BMA250E)
	{
		bma2x2_read_accel_x(&acc[0]);
		bma2x2_read_accel_y(&acc[1]);
		bma2x2_read_accel_z(&acc[2]);		
	}
	QuitSleepMode(acc,gSleepTimeStop-gSleepTimeStart);   //结束算法睡眠
	SLEEP_DATA_SET=false;	
	StartStep();   //开启计步 
}


