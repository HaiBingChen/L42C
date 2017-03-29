

#include "headfile.h"




#define CLOCK_TIMER_PRESCALER 0
#define ONE_MSECOND_INTERVAL         APP_TIMER_TICKS(20, CLOCK_TIMER_PRESCALER)






app_timer_id_t                  ClockID;









extern pU32 gPs_OsTimeSeconds;




void Clock_init(void)
{
    uint32_t err_code;
    
    err_code = app_timer_create(&ClockID, APP_TIMER_MODE_REPEATED, Clock_second_handle);
    APP_ERROR_CHECK(err_code);
}


void Clock_start(void)
{
     app_timer_start(ClockID, ONE_MSECOND_INTERVAL, (void *)3);    
}


void osal_setClock( UTCTime newTime )
{
    gPs_OsTimeSeconds = newTime;
}


UTCTime osal_getClock( void )
{
        return ( gPs_OsTimeSeconds );
}

uint16 GetEmptyRtcClockSaveIndex(void)
{
	uint8 Ut[128] ={0x00};
	for(uint8 i =0; i<4; i++)
	{
		SpiFlash_Read(TIMER_SAVE_BASE_ADDR+i*128,Ut,128);
		for(uint16 j=(i*128);j<((i+1)*128);j++)
		{
			if(Ut[j-(i*128)]==0xFF)
				return j;
		}
	}
	return 512;
}

void SetFlashRtcClock(UTCTimeStruct* Ut)
{
	if(!UPDATE_FONTLIB)
	{
		uint8 SetBuf =0x01;
		uint8 TimerBuf[7];
		TimerBuf[0] = Ut->year;
		TimerBuf[1] = (Ut->year)>>8;
		TimerBuf[2] = Ut->month;
		TimerBuf[3] = Ut->day;
		TimerBuf[4] = Ut->hour;
		TimerBuf[5] = Ut->minutes;
		TimerBuf[6] = Ut->seconds;	
		if(RtcSaveIndex>=512)
		{
			RtcSaveIndex=0;
			SpiFlash_4KSector_Erase(TIMER_SAVE_BASE_ADDR);	
		}
		Force_NordicFlash();
		SpiFlash_Write(TIMER_SAVE_BASE_ADDR+RtcSaveIndex,&SetBuf,1);
		SpiFlash_Write((TIMER_SAVE_BASE_ADDR+512+RtcSaveIndex*7),TimerBuf,7);
		Free_NordicFlash();
		RtcSaveIndex++;
	}
}

void GetFlashRtcClock(UTCTimeStruct* Ut)
{
	uint8 TimerBuf[7];
	Force_NordicFlash();
	RtcSaveIndex = GetEmptyRtcClockSaveIndex();
	if(RtcSaveIndex==0)
	{
		Ut->year = 2014;
		Ut->month = 2;
		Ut->day = 8;
		Ut->hour = 10;
		Ut->minutes = 58;
		Ut->seconds = 0;		
	}
	else
	{
		SpiFlash_Read((TIMER_SAVE_BASE_ADDR+512+(RtcSaveIndex-1)*7),TimerBuf,7);
		Ut->year = (uint16)TimerBuf[1]<<8 | TimerBuf[0];;
		Ut->month = TimerBuf[2];
		Ut->day = TimerBuf[3];
		Ut->hour = TimerBuf[4];
		Ut->minutes = TimerBuf[5];
		Ut->seconds = TimerBuf[6];				
	}
	Free_NordicFlash();
}

void ResetRtcClock(void)
{
	UTCTimeStruct UtcTime;
	GetFlashRtcClock(&UtcTime);
	gOS_SetUTCTime(&UtcTime);
}

void gOS_SetUTCTime( UTCTimeStruct *p_newTime )
{
  p_newTime->day-=1;
  p_newTime->month-=1;
  
  osal_setClock( osal_ConvertUTCSecs( p_newTime ) );
}

void gOS_GetUTCTime(UTCTimeStruct * p_store)
{
  osal_ConvertUTCTime( p_store, osal_getClock() );
  
  p_store->day+=1;
  p_store->month+=1;
}


static uint8_t monthLength( uint8_t lpyr, uint8_t mon )
{
  uint8_t days = 31;

  if ( mon == 1 ) // feb
  {
    days = ( 28 + lpyr );
  }
  else
  {
    if ( mon > 6 ) // aug-dec
    {
      mon--;
    }

    if ( mon & 1 )
    {
      days = 30;
    }
  }

  return ( days );
}


UTCTime osal_ConvertUTCSecs( UTCTimeStruct *tm )
{
  pU32 seconds;

  /* Seconds for the partial day */
  seconds = (((tm->hour * 60UL) + tm->minutes) * 60UL) + tm->seconds;

  /* Account for previous complete days */
  {
    /* Start with complete days in current month */
    pU16 days = tm->day;

    /* Next, complete months in current year */
    {
      signed char month = tm->month;
      while ( --month >= 0 )
      {
        days += monthLength( IsLeapYear( tm->year ), month );
      }
    }

    /* Next, complete years before current year */
    {
      pU16 year = tm->year;
      while ( --year >= BEGYEAR )
      {
        days += YearLength( year );
      }
    }

    /* Add total seconds before partial day */
    seconds += (days * DAY);
  }

  return ( seconds );
}


void osal_ConvertUTCTime( UTCTimeStruct *tm, UTCTime secTime )
{
  // calculate the time less than a day - hours, minutes, seconds
  {
    pU32 day = secTime % DAY;
    tm->seconds = day % 60UL;
    tm->minutes = (day % 3600UL) / 60UL;
    tm->hour = day / 3600UL;
  }

  // Fill in the calendar - day, month, year
  {
    pU16 numDays = secTime / DAY;
    tm->year = BEGYEAR;
    while ( numDays >= YearLength( tm->year ) )
    {
      numDays -= YearLength( tm->year );
      tm->year++;
    }

    tm->month = 0;
    while ( numDays >= monthLength( IsLeapYear( tm->year ), tm->month ) )
    {
      numDays -= monthLength( IsLeapYear( tm->year ), tm->month );
      tm->month++;
    }

    tm->day = numDays;
  }
}









void Clock_second_handle(void *p_context)
{
     pS_OsHwTimerUpdate();// 更新osal时间
	 if(gPs_SysTicks%200==0){
		//if(STui==ST_RUN)
		 	//SendMsg(MSG_SECONDS_TIMER_EVENT);
	 }
}



