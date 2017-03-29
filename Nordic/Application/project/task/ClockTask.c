#include "headfile.h"



uint8 ClockTaskId;
uint8 CountForCloseBLE=0; 

UTCTimeStruct gCurrentUtcTime;

/******************************************************************************************
*������:		ClockEvtInit
*����:			
*����ֵ:		
*����:			��ʼʱ���¼�
*��д:			
*�汾��Ϣ:	    
******************************************************************************************/
void ClockEvtInit(void)
{
        UTCTimeStruct uTime;      //��ǰʱ��
        
	osal_set_event(GetClockTaskId(), CLOCK_EVENT);  
        osal_start_timerEx(GetClockTaskId(), CLOCK_SUSPEND_TIMEOUT_EVENT, 100);
	
	gOS_GetUTCTime(&uTime);   //
	GetNoSameDay(uTime);
        memcpy(&gCurrentUtcTime, &uTime, sizeof(UTCTimeStruct));
}



/******************************************************************************************
*������:		StartSecondEvent
*����:			
*����ֵ:		
*����:			����1  ���ѭ���¼�
*��д:			luohuigui 
*�汾��Ϣ:	    v1.0
******************************************************************************************/
void StartSecondEvent(void)
{
        osal_set_event(GetClockTaskId(),CLOCK_1S_DISPLAY_EVENT);
}

/******************************************************************************************
*������:		StopSecondEvent
*����:			
*����ֵ:		
*����:			ֹͣ1  ���ѭ���¼�
*��д:			luohuigui 
*�汾��Ϣ:	    v1.0
******************************************************************************************/
void StopSecondEvent(void)
{
        osal_stop_timerEx(GetClockTaskId(),CLOCK_1S_DISPLAY_EVENT);
        osal_clear_event(GetClockTaskId(),CLOCK_1S_DISPLAY_EVENT);
}

/******************************************************************************************
*������:		ClockInitTask
*����:			
*����ֵ:		
*����:			ʱ�������ʼ��
*��д:			
*�汾��Ϣ:	    
******************************************************************************************/
void ClockInitTask(uint8 task_id)
{
	ClockTaskId = task_id;
}

uint8 GetClockTaskId(void)
{
	return ClockTaskId;
}

/******************************************************************************************
*������:		ClockEventHandler
*����:			
*����ֵ:		
*����:			ʱ���¼��������ڼ�����ѵ�
*��д:			
*�汾��Ϣ:	   
******************************************************************************************/
uint16 ClockEventHandler(uint8 task_id, uint16 events)
{
        UTCTimeStruct uTime;      
        gOS_GetUTCTime(&uTime);

	if (events & CLOCK_EVENT) 
        {
        	  SetFlashRtcClock(&uTime);
                if(GetNoSameDay(uTime)) 
                { //������������
                        StoreSportDataToFlash(1);
                        ClearSportData();  
                        ClearSleepData(false);
                }

                if((gInitMark==1)&&(Powerstate.powerstate==true)) 
                { 
			   CheckMinuteRemindEvent();				
			   CheckSitRemindEvent();
                }	
                osal_start_timerEx(GetClockTaskId(), CLOCK_EVENT, CLOCK_EVENT_PERIOD-uTime.seconds*1000);//1���Ӻ���һ��
                
		  if(MemFullCount<=60)
		  {		
		  	if(MemFullCount==60)
		  	{
		  		if(MEM_FULL_FLAG)
		  		{
		  			SendMsg(MSG_MEMORY_CHANGE);
					MemFullCount=0;
		  		}
				else
					MemFullCount=0xff;
		  	}
			else if(MemFullCount<60)
				MemFullCount++;
		  }

			
                //������⡢�񶯲���������������˲�们��
                if(GetMotorVibrateTimes()==0)
                {
                        BatteryAdcMeasure();
                }

                if(uTime.year != gCurrentUtcTime.year 
                || uTime.month != gCurrentUtcTime.month 
                || uTime.day != gCurrentUtcTime.day
                || uTime.hour!= gCurrentUtcTime.hour
                || uTime.minutes!= gCurrentUtcTime.minutes) 
                {
                        if((uTime.year < gCurrentUtcTime.year)||
                        (uTime.year == gCurrentUtcTime.year && uTime.month < gCurrentUtcTime.month) ||
                        (uTime.year == gCurrentUtcTime.year && uTime.month == gCurrentUtcTime.month && uTime.day < gCurrentUtcTime.day))
                        {
                                uTime.year   = gCurrentUtcTime.year;
                                uTime.month= gCurrentUtcTime.month;
                                uTime.day    = gCurrentUtcTime.day;
                                gOS_SetUTCTime(&uTime);

                        }

                        memcpy(&gCurrentUtcTime, &uTime, sizeof(UTCTimeStruct));
#if 0
                        if(BledisconnectFlag == BT_DISCONNECT) 
                        {
                                if(CountForCloseBLE < 60)
                                        CountForCloseBLE = 1;
                                else
                                {
                                        CountForCloseBLE = 0;
                                        BledisconnectFlag = BT_CLOSE;
                                        SendMsg(MSG_BLE_ADVERTISING_CLOSE_STATE);
                                }
                        } 
#endif			 
                        if((Powerstate.powerstate==true)&&(USB_GO_IN_FLAG==false)&&(!SLEEP_MODE_FLAG)&&(SWITCH_AUTO_SLEEP))
                        { 
                                gOS_GetUTCTime(&uTime);
				    uint8   SWeek=GetMonthWeekDays(uTime.year,uTime.month,uTime.day);//0~6(��~��)//	
				    uint8   SWeekMaskTable[7]={0x40,0x01,0x02,0x04,0x08,0x10,0x20}; 
 				    if(AutoSleepTime.SleepCycle & SWeekMaskTable[SWeek])
 				    {
	                                uint16 now = uTime.hour*60+uTime.minutes;
	                                uint16 begin = AutoSleepTime.BeginHour*60 +AutoSleepTime.BeginMinute;
	                                uint16 end = AutoSleepTime.EndHour*60 +AutoSleepTime.EndMinute;
	                              
	                                
	                                if(begin!=end) /*��������ʱ�䲻�����*/
	                                {
	                                        bool AutoSleep =false;
	                                        if(begin>end)
	                                                AutoSleep=(now>=begin && now<=1439) ||(now<=end);	
	                                        else
	                                                AutoSleep=(now>=begin && now<=end);
	                                        if(AutoSleep)
	                                        {
	                                                if(AutoSleepCount ==0) /*��һ�ν���, ����˯�߼��*/
	                                                {
	                                                        SetAutoSleepTime(((AutoSleepTime.BeginHour<<8)|AutoSleepTime.BeginMinute),((AutoSleepTime.EndHour<<8)|AutoSleepTime.EndMinute));
	                                                }
	                                                AutoSleepCount++;
	                                        }
	                                        else
	                                                CleanAutoSleepFlag();
	                                }
 				    }
                        }

                        SendMsg(MSG_DATE_UPDATE);

                }

                return ( events ^ CLOCK_EVENT );
        }

        if (events & CLOCK_1S_DISPLAY_EVENT) 
        {
                if(uTime.seconds != gCurrentUtcTime.seconds) 
                {
                        gCurrentUtcTime.seconds = uTime.seconds;
                        SendMsg(MSG_DISPLAY_BATTERY_STATE);
                }
                osal_start_timerEx(GetClockTaskId(), CLOCK_1S_DISPLAY_EVENT, 1000);// 1  �����һ��
      
                return ( events ^ CLOCK_1S_DISPLAY_EVENT );      
        }

        if (events & CLOCK_SUSPEND_TIMEOUT_EVENT)
        {
                if(StandbyTimeCounter != DISPLAY_STATE_NO_CHANGE)
                {
                        if(StandbyTimeCounter == 0)
                        {
                                StandbyTimeCounter = DISPLAY_STATE_NO_CHANGE;
                                SendMsg(MSG_SYSTEM_SUSPEND);
                        }
                        else
                        {
                                StandbyTimeCounter --;
				if(StandbyTimeCounter == 5)
				{
					 SendMsg(MSG_SET_BACKLIGHT_DAKR);
				}
                        }
						
                }
                osal_start_timerEx(GetClockTaskId(), CLOCK_SUSPEND_TIMEOUT_EVENT, 1000);
                
                return ( events ^ CLOCK_SUSPEND_TIMEOUT_EVENT );
        }
		
        if(events & CLOCK_NUS_DATA_PARSE)
        {
		nus_data_Parse();
		return ( events ^ CLOCK_NUS_DATA_PARSE );	
        }
		
        if(events & CLOCK_MEDIA_DATA_PARSE)
        {
		MediaDataParse();
		return ( events ^ CLOCK_MEDIA_DATA_PARSE );	
        }

    	if(events & STOP_WATCH_CNT_EVENT)
    	{
            if(STui == ST_RUN)
            {
        		gStopWatchLastMs = gStopWatchLastMsSave + (pS_OsGetSysTick()-gStopWatchSysTick_start);
        		
        		if(gStopWatchLastMs >= MAX_SW_mSEC)
        		{
        			gStopWatchLastMs = MAX_SW_mSEC;
                    STui = ST_STOP;
        		}
                else
                {
        		    osal_start_timerEx(GetClockTaskId(), STOP_WATCH_CNT_EVENT, 100);
                }
            }
    		return (events ^ STOP_WATCH_CNT_EVENT);
    	}        
        return ( 0 );
}





bool GetNoSameDay(UTCTimeStruct uTime)
{
  static  uint8  day=0;				//!< 1-30
  static  uint8  month=0;			//!< 1-12
  static  uint16  year=0;			//!< 2000-65535
  
  bool bVuale=false;
  
  if(uTime.day!=day)
  {
    bVuale=true;
    day=uTime.day;
  }
  if(uTime.month!=month)
  {
    bVuale=true;
    month=uTime.month;
  }
  if(uTime.year!=year)
  {
    bVuale=true;
    year=uTime.year;
  }
  
  return bVuale;

}

bool GetNoSameTime(UTCTimeStruct uTime)
{
  static  uint8  hour=0;				//!< 0-23
  static  uint8  minutes=0;			//!< 0-59
  
  
  bool bVuale=false;
  
  if(uTime.hour!=hour)
  {
    bVuale=true;
    hour=uTime.hour;
  }
  if(uTime.minutes!=minutes)
  {
    bVuale=true;
    minutes=uTime.minutes;
  }
  
  return bVuale;
}

/******************************************************************************************
*������:		GetTimeStamp
*����:			
*����ֵ:		��ǰ��ʱ���
*����:			�õ���ǰ��ʱ���
*��д:			
*�汾��Ϣ:	  
******************************************************************************************/
uint32 GetTimeStamp(void)
{
  UTCTime timep;
  UTCTimeStruct UtcTime;		
  
  gOS_GetUTCTime(&UtcTime);
  UtcTime.month -= 1;
  UtcTime.day -= 1;

  timep=pS_OsGetUTCSecs(&UtcTime)+BEGYEAR_STAMP;
  return (uint32)timep;
}

/******************************************************************************************
*������:		pS_OsGetUTCSecs
*����:			��
*����ֵ:		��
*����:			��pS_UTCTimeStruct �еĽṹ��ת��ΪUTC������
*��д:			
*�汾��Ϣ:		
******************************************************************************************/
UTCTime pS_OsGetUTCSecs( UTCTimeStruct *tm )
{
  return ( osal_ConvertUTCSecs( tm ) );
}

//*********************************����Ϊ�������ں���************************************//
/******************************************************************************************
*����y���Ƿ�Ϊ����
*����y��Ϊ��������
*����1:�����ꣻ����0����������
******************************************************************************************/
int IsLeap(int y)
{
    return y%4==0&&y%100!=0||y%400==0;//���������������������Ϊ1����������Ϊ0��
}

/******************************************************************************************
*����y�������
*����y:��������
*����y�������
******************************************************************************************/

int GetYearDays(int y)
{
    return IsLeap(y)?366:365 ;
}

/******************************************************************************************
*����y��ĵ�һ������ڡ���2000��Ϊ���գ����һ������Ϊ����
*����y����������
*����y���һ�������
******************************************************************************************/
int GetYearWeekDays(int y)
{
    int sum=0;
    int i=0;

    if(y>=2000)
    {
        for(i=2000;i<y;i++)
     {
            sum+=GetYearDays(i);
     }
        return (sum+6)%7;
    }
    else
    {
        for(i=y;i<2000;i++)
     {
            sum+=GetYearDays(i);
     }
       return ( -sum%7+6)%7;//����ط��Ŀ��� 
    }
}

/******************************************************************************************
*����y��m�µ�����
*����y:�������ꣻm��������
����y��m�µ�����
******************************************************************************************/        
int GetMonthDays(int y,int m)
{
    switch(m)
    {
        case 1:
        case 3:
        case 5:
        case 7:
        case 8:
        case 10:
        case 12:
                return 31;
        case 4:
        case 6:
        case 9:
        case 11:
                return 30;
        case 2	:
                return IsLeap(y)?29:28;
        default :
                return 0;
    
    }

} 
  
/******************************************************************************************
*����y��m�µĵ�һ�������
*����y���������꣬m���������� ,z:��������
*����y��m�µ�����
******************************************************************************************/
int GetMonthWeekDays(int y,int m,int z)
{
    int sum=0;
    int i=0;

    for(i=1;i<m;i++)//ÿ������һ���еĶ�����(���µ�ǰһ����)
    {
        sum+=GetMonthDays(y,i);
    }
    return (GetYearWeekDays(y)+sum+(z-1))%7; //
}

