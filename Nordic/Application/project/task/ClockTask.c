#include "headfile.h"



uint8 ClockTaskId;
uint8 CountForCloseBLE=0; 

UTCTimeStruct gCurrentUtcTime;

/******************************************************************************************
*函数名:		ClockEvtInit
*参数:			
*返回值:		
*描述:			初始时钟事件
*编写:			
*版本信息:	    
******************************************************************************************/
void ClockEvtInit(void)
{
        UTCTimeStruct uTime;      //当前时间
        
	osal_set_event(GetClockTaskId(), CLOCK_EVENT);  
        osal_start_timerEx(GetClockTaskId(), CLOCK_SUSPEND_TIMEOUT_EVENT, 100);
	
	gOS_GetUTCTime(&uTime);   //
	GetNoSameDay(uTime);
        memcpy(&gCurrentUtcTime, &uTime, sizeof(UTCTimeStruct));
}



/******************************************************************************************
*函数名:		StartSecondEvent
*参数:			
*返回值:		
*描述:			启动1  秒的循环事件
*编写:			luohuigui 
*版本信息:	    v1.0
******************************************************************************************/
void StartSecondEvent(void)
{
        osal_set_event(GetClockTaskId(),CLOCK_1S_DISPLAY_EVENT);
}

/******************************************************************************************
*函数名:		StopSecondEvent
*参数:			
*返回值:		
*描述:			停止1  秒的循环事件
*编写:			luohuigui 
*版本信息:	    v1.0
******************************************************************************************/
void StopSecondEvent(void)
{
        osal_stop_timerEx(GetClockTaskId(),CLOCK_1S_DISPLAY_EVENT);
        osal_clear_event(GetClockTaskId(),CLOCK_1S_DISPLAY_EVENT);
}

/******************************************************************************************
*函数名:		ClockInitTask
*参数:			
*返回值:		
*描述:			时钟任务初始化
*编写:			
*版本信息:	    
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
*函数名:		ClockEventHandler
*参数:			
*返回值:		
*描述:			时钟事件处理，用于检测提醒等
*编写:			
*版本信息:	   
******************************************************************************************/
uint16 ClockEventHandler(uint8 task_id, uint16 events)
{
        UTCTimeStruct uTime;      
        gOS_GetUTCTime(&uTime);

	if (events & CLOCK_EVENT) 
        {
        	  SetFlashRtcClock(&uTime);
                if(GetNoSameDay(uTime)) 
                { //清除当天的数据
                        StoreSportDataToFlash(1);
                        ClearSportData();  
                        ClearSleepData(false);
                }

                if((gInitMark==1)&&(Powerstate.powerstate==true)) 
                { 
			   CheckMinuteRemindEvent();				
			   CheckSitRemindEvent();
                }	
                osal_start_timerEx(GetClockTaskId(), CLOCK_EVENT, CLOCK_EVENT_PERIOD-uTime.seconds*1000);//1分钟后检测一次
                
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

			
                //电量检测、振动不检测电量，防电量瞬间滑动
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
				    uint8   SWeek=GetMonthWeekDays(uTime.year,uTime.month,uTime.day);//0~6(日~六)//	
				    uint8   SWeekMaskTable[7]={0x40,0x01,0x02,0x04,0x08,0x10,0x20}; 
 				    if(AutoSleepTime.SleepCycle & SWeekMaskTable[SWeek])
 				    {
	                                uint16 now = uTime.hour*60+uTime.minutes;
	                                uint16 begin = AutoSleepTime.BeginHour*60 +AutoSleepTime.BeginMinute;
	                                uint16 end = AutoSleepTime.EndHour*60 +AutoSleepTime.EndMinute;
	                              
	                                
	                                if(begin!=end) /*启动结束时间不能相等*/
	                                {
	                                        bool AutoSleep =false;
	                                        if(begin>end)
	                                                AutoSleep=(now>=begin && now<=1439) ||(now<=end);	
	                                        else
	                                                AutoSleep=(now>=begin && now<=end);
	                                        if(AutoSleep)
	                                        {
	                                                if(AutoSleepCount ==0) /*第一次进入, 启动睡眠监测*/
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
                osal_start_timerEx(GetClockTaskId(), CLOCK_1S_DISPLAY_EVENT, 1000);// 1  秒后检测一次
      
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
*函数名:		GetTimeStamp
*参数:			
*返回值:		当前的时间戳
*描述:			得到当前的时间戳
*编写:			
*版本信息:	  
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
*函数名:		pS_OsGetUTCSecs
*参数:			无
*返回值:		无
*描述:			将pS_UTCTimeStruct 中的结构体转换为UTC秒钟数
*编写:			
*版本信息:		
******************************************************************************************/
UTCTime pS_OsGetUTCSecs( UTCTimeStruct *tm )
{
  return ( osal_ConvertUTCSecs( tm ) );
}

//*********************************以下为计算星期函数************************************//
/******************************************************************************************
*计算y年是否为闰年
*参数y：为给定的年
*返回1:是闰年；返回0：不是闰年
******************************************************************************************/
int IsLeap(int y)
{
    return y%4==0&&y%100!=0||y%400==0;//满足闰年的条件，成立则为1，不成立则为0；
}

/******************************************************************************************
*计算y年的天数
*参数y:给定的年
*返回y年的天数
******************************************************************************************/

int GetYearDays(int y)
{
    return IsLeap(y)?366:365 ;
}

/******************************************************************************************
*计算y年的第一天的星期。以2000年为参照，其第一天星期为周六
*参数y：给定的年
*返回y年第一天的星期
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
       return ( -sum%7+6)%7;//这个地方的看看 
    }
}

/******************************************************************************************
*计算y年m月的天数
*参数y:给定的年；m：给定月
返回y年m月的天数
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
*计算y年m月的第一天的星期
*参数y：给定的年，m：给定的月 ,z:给定的天
*返回y年m月的星期
******************************************************************************************/
int GetMonthWeekDays(int y,int m,int z)
{
    int sum=0;
    int i=0;

    for(i=1;i<m;i++)//每个月在一年中的多少天(该月的前一个月)
    {
        sum+=GetMonthDays(y,i);
    }
    return (GetYearWeekDays(y)+sum+(z-1))%7; //
}

