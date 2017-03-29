#include "headfile.h"


/*********************************************************************
 * TYPEDEFS
 */

typedef union {
  pU32 Time32;
  pU16 Time16[2];
  pU8  Time8[4];
} pS_OsTime_T;

typedef struct
{
  void          *Next;
  pS_OsTime_T   Timeout;
  pU16          EventFlag;
  pU16          TaskId;
  pU32          ReloadTimeout;
} pS_OsTimerRec_T;

/*********************************************************************
 * GLOBAL VARIABLES
 */

pS_OsTimerRec_T *TimerHead=NULL;
static pU32 pS_OsSystemClock=0;

/******************************************************************************************
*函数名:		pS_OsTimerInit
*参数:			无
*返回值:		无
*描述:			初始化任务计时器
*编写:			
*版本信息:		
******************************************************************************************/
void pS_OsTimerInit(void)
{
  pS_OsSystemClock=0;
  TimerHead=NULL;
}
/******************************************************************************************
*函数名:		pS_OsGetSystemClock
*参数:			无
*返回值:		无
*描述:			得到系统Clock
*编写:			
*版本信息:		
******************************************************************************************/
pU32 pS_OsGetSystemClock( void )
{
  return ( pS_OsSystemClock );
}

/******************************************************************************************
*函数名:		pS_OsFindTimer
*参数:			无
*返回值:		无
*描述:			查找任务计时器
*编写:			
*版本信息:		
******************************************************************************************/

pS_OsTimerRec_T *pS_OsFindTimer( pU16 TaskId, pU16 EventFlag )
{
  pS_OsTimerRec_T *srchTimer;

  // Head of the timer list
  srchTimer = TimerHead;

  // Stop when found or at the end
  while ( srchTimer )
  {
    if ( srchTimer->EventFlag == EventFlag &&
         srchTimer->TaskId == TaskId )
    {
      break;
    }

    // Not this one, check another
    srchTimer = srchTimer->Next;
  }

  return ( srchTimer );
}
/******************************************************************************************
*函数名:		pS_OsAddTimer
*参数:			无
*返回值:		无
*描述:			添加任务
*编写:			
*版本信息:		
******************************************************************************************/

pS_OsTimerRec_T * pS_OsAddTimer( pU16 TaskId, pU16 EventFlag, pU32 Timeout )
{
  pS_OsTimerRec_T *newTimer=NULL;
  pS_OsTimerRec_T *srchTimer=NULL;

  // Look for an existing timer first
  newTimer = pS_OsFindTimer( TaskId, EventFlag );
  if ( newTimer )
  {
    // Timer is found - update it.
    newTimer->Timeout.Time32 = Timeout;

    return ( newTimer );
  }
  else
  {
    // New Timer
    newTimer = pS_OsMalloc( sizeof( pS_OsTimerRec_T ) );//动态申请内存、加入定时器

    if ( newTimer )
    {
      // Fill in new timer
      newTimer->TaskId = TaskId;
      newTimer->EventFlag = EventFlag;
      newTimer->Timeout.Time32 = Timeout;
      newTimer->Next = (void *)NULL;
      newTimer->ReloadTimeout = 0;

      // Does the timer list already exist
      if ( TimerHead == NULL )
      {
        // Start task list
        TimerHead = newTimer;
      }
      else
      {
        // Add it to the end of the timer list
        srchTimer = TimerHead;

        // Stop at the last record
        while ( srchTimer->Next )
          srchTimer = srchTimer->Next;

        // Add to the list
        srchTimer->Next = newTimer;
      }

      return ( newTimer );
    }
    else
    {
      return ( (pS_OsTimerRec_T *)NULL );
    }
  }
}
/******************************************************************************************
*函数名:		pS_OsDeleteTimer
*参数:			无
*返回值:		无
*描述:			删除任务
*编写:			
*版本信息:		
******************************************************************************************/

void pS_OsDeleteTimer( pS_OsTimerRec_T *rmTimer )
{
  // Does the timer list really exist
  if ( rmTimer )
  {
    // Clear the event flag and osalTimerUpdate() will delete
    // the timer from the list.
    rmTimer->EventFlag = 0;
  }
}

/******************************************************************************************
*函数名:		pS_OsStartTimer
*参数:			无
*返回值:		无
*描述:			开始任务计时,在初始化系统之后调用
*编写:			
*版本信息:		
******************************************************************************************/

pU8 pS_OsStartTimer( pU16 TaskId, pU16 EventId, pU32 TimeoutValue )
{
  pS_OsTimerRec_T *newTimer;

  // Add timer
  newTimer = pS_OsAddTimer( TaskId, EventId, TimeoutValue );

  return ( (newTimer != NULL) ? SUCCESS : NO_TIMER_AVAIL );
}

/******************************************************************************************
*函数名:		osal_start_timerEx
*参数:			
*返回值:		
*描述:			开始任务计时
*编写:			
*版本信息:		
******************************************************************************************/
pU8 osal_start_timerEx( pU16 TaskId, pU16 EventId, pU32 TimeoutValue )
{
  pS_OsStartTimer(TaskId,EventId,TimeoutValue);

  return ( SUCCESS );
}

/******************************************************************************************
*函数名:		pS_OsReloadTimer
*参数:			无
*返回值:		无
*描述:			重装任务时间
*编写:			
*版本信息:		
******************************************************************************************/

pU8 pS_OsReloadTimer( pU16 TaskId, pU16 EventId, pU32 TimeoutValue )
{
  pS_OsTimerRec_T *newTimer;

  // Add timer
  newTimer = pS_OsAddTimer( TaskId, EventId, TimeoutValue );
  if ( newTimer )
  {
    // Load the reload timeout value
    newTimer->ReloadTimeout = TimeoutValue;
  }

  return ( (newTimer != NULL) ? SUCCESS : NO_TIMER_AVAIL );
}
/******************************************************************************************
*函数名:		pS_OsStopTimer
*参数:			无
*返回值:		无
*描述:			停止任务计时器、清除任务
*编写:			
*版本信息:		
******************************************************************************************/

pU8 pS_OsStopTimer(  pU16 TaskId, pU16 EventId )
{
  pS_OsTimerRec_T *foundTimer;

  // Find the timer to stop
  foundTimer = pS_OsFindTimer( TaskId, EventId );
  if ( foundTimer )
  {
    pS_OsDeleteTimer( foundTimer );
  }


  return ( (foundTimer != NULL) ? SUCCESS : INVALID_EVENT_ID );
}

/******************************************************************************************
*函数名:		pS_OsStopTimer
*参数:			无
*返回值:		无
*描述:			停止任务计时器、清除任务
*编写:			
*版本信息:		
******************************************************************************************/
pU8 osal_stop_timerEx(  pU16 TaskId, pU16 EventId )
{
    return (pS_OsStopTimer(TaskId,EventId));
}

/******************************************************************************************
*函数名:		pS_OsGetTimeout
*参数:			无
*返回值:		无
*描述:			得到任务超时时间
*编写:			
*版本信息:		
******************************************************************************************/

pU32 pS_OsGetTimeout(  pU16 TaskId, pU16 EventId )
{

  pU32 rtrn = 0;
  pS_OsTimerRec_T *tmr;


  tmr = pS_OsFindTimer( TaskId, EventId );

  if ( tmr )
  {
    rtrn = tmr->Timeout.Time32;
  }


  return rtrn;
}

/******************************************************************************************
*函数名:		pS_OsTimerNumActive
*参数:			无
*返回值:		无
*描述:			当前任务所用的计时器个数
*编写:			
*版本信息:		
******************************************************************************************/


pU8 pS_OsTimerNumActive( void )
{

  pU8 num_timers = 0;
  pS_OsTimerRec_T *srchTimer;



  // Head of the timer list
  srchTimer = TimerHead;

  // Count timers in the list
  while ( srchTimer != NULL )
  {
    num_timers++;
    srchTimer = srchTimer->Next;
  }

  return num_timers;
}


/******************************************************************************************
*函数名:		pS_OsTimerUpdate
*参数:			无
*返回值:		无
*描述:			更新任务计时器
*编写:			
*版本信息:		
******************************************************************************************/

void pS_OsTimerUpdate( pU32 nMSec )//更新事件定时器
{

  pS_OsTimerRec_T *srchTimer;
  pS_OsTimerRec_T *prevTimer;

  pS_OsTime_T timeUnion;
  timeUnion.Time32 = nMSec;

  	//关闭中断
	IRQ_DISABLE_SAVE();
  // Update the system time
  pS_OsSystemClock += nMSec;
  //开启中断
	IRQ_ENABLE_RESTORE();

  // Look for open timer slot
  if ( TimerHead != NULL )
  {
    // Add it to the end of the timer list
    srchTimer = TimerHead;
    prevTimer = (void *)NULL;

    // Look for open timer slot
    while ( srchTimer )
    {
      pS_OsTimerRec_T *freeTimer = NULL;

      // Hold off interrupts.

      // To minimize time in this critical section, avoid 32-bit math
      if ((timeUnion.Time16[1] == 0) && (timeUnion.Time8[1] == 0))
      {
        // If upper 24 bits are zero, check lower 8 bits for roll over
        if (srchTimer->Timeout.Time8[0] >= timeUnion.Time8[0])
        {
          // 8-bit math
          srchTimer->Timeout.Time8[0] -= timeUnion.Time8[0];
        }
        else
        {
          // 32-bit math
          if (srchTimer->Timeout.Time32 > timeUnion.Time32)
          {
            srchTimer->Timeout.Time32 -= timeUnion.Time32;
          }
          else
          {
            srchTimer->Timeout.Time32 = 0;
          }
        }
      }
      else
      {
          // 32-bit math
          //每次都将定时器任务里的时间计数值减去定时周期，也是20ms
        if (srchTimer->Timeout.Time32 > timeUnion.Time32)
        {
          srchTimer->Timeout.Time32 -= timeUnion.Time32;
        }
		//当定时器任务里面的的时间计数值小于20ms的时候，说明定时时间到了。
        else
        {
          srchTimer->Timeout.Time32 = 0;
        }
      }

      // Check for reloading
      if ( (srchTimer->Timeout.Time16[0] == 0) && (srchTimer->Timeout.Time16[1] == 0) &&
           (srchTimer->ReloadTimeout) && (srchTimer->EventFlag) )
      {
        // Notify the task of a timeout
        pS_OsSetTaskEvent( srchTimer->TaskId, srchTimer->EventFlag );

        // Reload the timer timeout value
        srchTimer->Timeout.Time32 = srchTimer->ReloadTimeout;
      }

      // When timeout or delete (event_flag == 0)
      //定时器超时判断
      if ( ((srchTimer->Timeout.Time16[0] == 0) && (srchTimer->Timeout.Time16[1] == 0)) ||
            (srchTimer->EventFlag == 0) )
      {
        // Take out of list
        if ( prevTimer == NULL )
        {
          TimerHead = srchTimer->Next;
        }
        else
        {
          prevTimer->Next = srchTimer->Next;
        }
		
        // Setup to free memory
        //如果定时时间到，或者这个定时器没有事件了，则要将这个定时器的事件加载到对应的任务链表上去。
        freeTimer = srchTimer;

        // Next，当到最后一个定时器的时候,next为NULL，退出while循环
        srchTimer = srchTimer->Next;
      }
	  //如果这个定时器没有超时，则不用将定时器的事件加载到任务链表上去，继续运行下一个定时器
      else
      {
        // Get next
        prevTimer = srchTimer;
        srchTimer = srchTimer->Next;
      }

       // Re-enable interrupts.

      if ( freeTimer )
      {
      	//超时处理，另一种是没有事件，没有事件不处理，直接释放这个定时器
        if ( (freeTimer->Timeout.Time16[0] == 0) && (freeTimer->Timeout.Time16[1] == 0) )
        {
			//如果是app任务
			if ((freeTimer->TaskId == GetAppTaskId())&& (freeTimer->EventFlag !=0))
                SendMsg((MsgType)freeTimer->EventFlag);
            else
                pS_OsSetTaskEvent( freeTimer->TaskId, freeTimer->EventFlag );
        }
        pS_OsFree( freeTimer );//释放定时器内存
      }
    }
  }  
}




