#include "headfile.h"




//多少毫秒更新一次系统时钟
#define UPDATE_TIME_N_MS        (20)
#define MAX_TICKS_MASK          (0xFFFFFFFF)
#define ADJUST_TICKS_1MIN		(59)



pU32 gPs_SysTicks=0;
pU32 PreviousTimerTicks=0;//用于保存当前时钟Ticks
pU32 gPs_OsTimeSeconds=0;



pU32 pS_OsGetSysTick(void)
{
	return gPs_SysTicks;
}



/******************************************************************************************
*函数名:		pS_OsTimeClockUpdate
*参数:			无
*返回值:		无
*描述:			用于更新系统时钟和任务时钟
*编写:			
*版本信息:		
******************************************************************************************/

void pS_OsTimeClockUpdate( void )
{
  //pU32 TicksnMs=0;
  pU32 ElapsedMSec=0;

  //gPs_SysTicks每20ms更新一次，所以在大循环中每次执行pS_OsTimeClockUpdate这个函数的时候gPs_SysTicks不一定更新，如果没更就什么都不做
  if (PreviousTimerTicks!=gPs_SysTicks )
  {
    if(PreviousTimerTicks>gPs_SysTicks)//溢出时间
    {
      ElapsedMSec=MAX_TICKS_MASK-PreviousTimerTicks+gPs_SysTicks;
    }
    else
    {
	  //PreviousTimerTicks为上次的时间计数，gPs_SysTicks为当前的时间计数
	  ElapsedMSec=gPs_SysTicks-PreviousTimerTicks;
    }

    PreviousTimerTicks=gPs_SysTicks;
   
    //ElapsedMSec=(pU32)( TicksnMs )%0xffffffff;//n ms更新一次
    
    if ( ElapsedMSec )
    {
      pS_OsClockUpdate( ElapsedMSec );//用于用户时钟用，可做闹钟等
      pS_OsTimerUpdate( ElapsedMSec );//用于更新系统的时钟，更新时钟定时器
    }
  }
  
}
/******************************************************************************************
*函数名:		pS_OsClockUpdate
*参数:			无
*返回值:		无
*描述:			更新系统时钟
*编写:			
*版本信息:		
******************************************************************************************/

void pS_OsClockUpdate( pU32 nMSec )//用于时钟
{
  static pU32 TimeMSec = 0;
  // Add elapsed milliseconds to the saved millisecond portion of time
  TimeMSec += nMSec;

  // Roll up milliseconds to the number of Seconds
  if ( TimeMSec >= 1000 )
  {
    gPs_OsTimeSeconds += TimeMSec / 1000;
    TimeMSec = TimeMSec % 1000;
  }
}

/******************************************************************************************
*函数名:		pS_OsHwTimerUpdate
*参数:			无
*返回值:		无
*描述:			更新系统时钟，在硬件定时器中调用1ms 、10ms 、100ms  、1s
*编写:			
*版本信息:		
******************************************************************************************/
 void pS_OsHwTimerUpdate(void)
{
	//关闭中断
	IRQ_DISABLE_SAVE();
	
	gPs_SysTicks+=UPDATE_TIME_N_MS;
	
	//开启中断
	IRQ_ENABLE_RESTORE();
}

void pS_OsHwTimerAdjust_1min(void)
{
	//关闭中断
	//IRQ_DISABLE_SAVE();
	
	gPs_SysTicks+=ADJUST_TICKS_1MIN;
	
	//开启中断
	//IRQ_ENABLE_RESTORE();
}



