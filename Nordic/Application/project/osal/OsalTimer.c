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
*������:		pS_OsTimerInit
*����:			��
*����ֵ:		��
*����:			��ʼ�������ʱ��
*��д:			
*�汾��Ϣ:		
******************************************************************************************/
void pS_OsTimerInit(void)
{
  pS_OsSystemClock=0;
  TimerHead=NULL;
}
/******************************************************************************************
*������:		pS_OsGetSystemClock
*����:			��
*����ֵ:		��
*����:			�õ�ϵͳClock
*��д:			
*�汾��Ϣ:		
******************************************************************************************/
pU32 pS_OsGetSystemClock( void )
{
  return ( pS_OsSystemClock );
}

/******************************************************************************************
*������:		pS_OsFindTimer
*����:			��
*����ֵ:		��
*����:			���������ʱ��
*��д:			
*�汾��Ϣ:		
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
*������:		pS_OsAddTimer
*����:			��
*����ֵ:		��
*����:			�������
*��д:			
*�汾��Ϣ:		
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
    newTimer = pS_OsMalloc( sizeof( pS_OsTimerRec_T ) );//��̬�����ڴ桢���붨ʱ��

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
*������:		pS_OsDeleteTimer
*����:			��
*����ֵ:		��
*����:			ɾ������
*��д:			
*�汾��Ϣ:		
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
*������:		pS_OsStartTimer
*����:			��
*����ֵ:		��
*����:			��ʼ�����ʱ,�ڳ�ʼ��ϵͳ֮�����
*��д:			
*�汾��Ϣ:		
******************************************************************************************/

pU8 pS_OsStartTimer( pU16 TaskId, pU16 EventId, pU32 TimeoutValue )
{
  pS_OsTimerRec_T *newTimer;

  // Add timer
  newTimer = pS_OsAddTimer( TaskId, EventId, TimeoutValue );

  return ( (newTimer != NULL) ? SUCCESS : NO_TIMER_AVAIL );
}

/******************************************************************************************
*������:		osal_start_timerEx
*����:			
*����ֵ:		
*����:			��ʼ�����ʱ
*��д:			
*�汾��Ϣ:		
******************************************************************************************/
pU8 osal_start_timerEx( pU16 TaskId, pU16 EventId, pU32 TimeoutValue )
{
  pS_OsStartTimer(TaskId,EventId,TimeoutValue);

  return ( SUCCESS );
}

/******************************************************************************************
*������:		pS_OsReloadTimer
*����:			��
*����ֵ:		��
*����:			��װ����ʱ��
*��д:			
*�汾��Ϣ:		
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
*������:		pS_OsStopTimer
*����:			��
*����ֵ:		��
*����:			ֹͣ�����ʱ�����������
*��д:			
*�汾��Ϣ:		
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
*������:		pS_OsStopTimer
*����:			��
*����ֵ:		��
*����:			ֹͣ�����ʱ�����������
*��д:			
*�汾��Ϣ:		
******************************************************************************************/
pU8 osal_stop_timerEx(  pU16 TaskId, pU16 EventId )
{
    return (pS_OsStopTimer(TaskId,EventId));
}

/******************************************************************************************
*������:		pS_OsGetTimeout
*����:			��
*����ֵ:		��
*����:			�õ�����ʱʱ��
*��д:			
*�汾��Ϣ:		
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
*������:		pS_OsTimerNumActive
*����:			��
*����ֵ:		��
*����:			��ǰ�������õļ�ʱ������
*��д:			
*�汾��Ϣ:		
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
*������:		pS_OsTimerUpdate
*����:			��
*����ֵ:		��
*����:			���������ʱ��
*��д:			
*�汾��Ϣ:		
******************************************************************************************/

void pS_OsTimerUpdate( pU32 nMSec )//�����¼���ʱ��
{

  pS_OsTimerRec_T *srchTimer;
  pS_OsTimerRec_T *prevTimer;

  pS_OsTime_T timeUnion;
  timeUnion.Time32 = nMSec;

  	//�ر��ж�
	IRQ_DISABLE_SAVE();
  // Update the system time
  pS_OsSystemClock += nMSec;
  //�����ж�
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
          //ÿ�ζ�����ʱ���������ʱ�����ֵ��ȥ��ʱ���ڣ�Ҳ��20ms
        if (srchTimer->Timeout.Time32 > timeUnion.Time32)
        {
          srchTimer->Timeout.Time32 -= timeUnion.Time32;
        }
		//����ʱ����������ĵ�ʱ�����ֵС��20ms��ʱ��˵����ʱʱ�䵽�ˡ�
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
      //��ʱ����ʱ�ж�
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
        //�����ʱʱ�䵽�����������ʱ��û���¼��ˣ���Ҫ�������ʱ�����¼����ص���Ӧ������������ȥ��
        freeTimer = srchTimer;

        // Next���������һ����ʱ����ʱ��,nextΪNULL���˳�whileѭ��
        srchTimer = srchTimer->Next;
      }
	  //��������ʱ��û�г�ʱ�����ý���ʱ�����¼����ص�����������ȥ������������һ����ʱ��
      else
      {
        // Get next
        prevTimer = srchTimer;
        srchTimer = srchTimer->Next;
      }

       // Re-enable interrupts.

      if ( freeTimer )
      {
      	//��ʱ������һ����û���¼���û���¼�������ֱ���ͷ������ʱ��
        if ( (freeTimer->Timeout.Time16[0] == 0) && (freeTimer->Timeout.Time16[1] == 0) )
        {
			//�����app����
			if ((freeTimer->TaskId == GetAppTaskId())&& (freeTimer->EventFlag !=0))
                SendMsg((MsgType)freeTimer->EventFlag);
            else
                pS_OsSetTaskEvent( freeTimer->TaskId, freeTimer->EventFlag );
        }
        pS_OsFree( freeTimer );//�ͷŶ�ʱ���ڴ�
      }
    }
  }  
}




