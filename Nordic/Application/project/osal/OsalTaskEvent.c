#include "headfile.h"




 pS_OsTaskREC_T   * TaskHead;//the point to link'head
 pS_OsTaskREC_T   * TaskActive;

 pU16 TaskId;



 
/******************************************************************************************
*������:		pS_OsTaskHeadInit
*����:			��
*����ֵ:		��
*����:			��ʼ��ϵͳ����ͷ
*��д:			
*�汾��Ϣ:		
******************************************************************************************/

void  pS_OsTaskHeadInit(void )
{
  TaskHead    = (pS_OsTaskREC_T *)NULL;
  TaskActive  = (pS_OsTaskREC_T *)NULL;
  TaskId = 0;
}
/******************************************************************************************
*������:		pS_OsTaskInit
*����:			��
*����ֵ:		��
*����:			��ʼ��������ע����������
*��д:			
*�汾��Ϣ:		
******************************************************************************************/

void  pS_OsTaskInit(void)
{
  TaskActive = TaskHead;
  while(TaskActive)
  {
    if(TaskActive->pfnInit)
    {
      TaskActive->pfnInit(TaskActive->taskID);
    }
    TaskActive = TaskActive->next;
  }
  TaskActive = (pS_OsTaskREC_T *)NULL;
}

/******************************************************************************************
*������:		pS_OsRegTask
*����:			��
*����ֵ:		��
*����:			����ע��
*��д:			
*�汾��Ϣ:		
******************************************************************************************/

void  pS_OsRegTask(pTaskInitFn pfnInit,
                  pTaskEventHandlerFn pfnEventProcessor,
                  pU8 taskPriority)
{
	pS_OsTaskREC_T  *  TaskNew;
	pS_OsTaskREC_T  *  TaskSech;
	pS_OsTaskREC_T  ** TaskPTR;
	TaskNew = pS_OsMalloc(sizeof( pS_OsTaskREC_T));
	if(TaskNew)
	{
		TaskNew->pfnInit =                  pfnInit;
		TaskNew->pfnEventProcessor =        pfnEventProcessor;
		TaskNew->taskID =                   TaskId++;
		TaskNew->events =                   0;
		TaskNew->taskPriority =             taskPriority;
		TaskNew->next =                     (pS_OsTaskREC_T *)NULL;
		
		TaskPTR = &TaskHead;//�ı�TaskHead
	        TaskSech = TaskHead;
		
		while(TaskSech)
		{
			if(TaskNew->taskPriority > TaskSech->taskPriority)
			{
		
				TaskNew->next = TaskSech;
				*TaskPTR = TaskNew;
				return;
			}
			
			TaskPTR = &TaskSech->next;
			TaskSech = TaskSech->next;
			
		}
		*TaskPTR = TaskNew;
		
	}
	return;
	
}

/******************************************************************************************
*������:		pS_OsNextActiveTask
*����:			��
*����ֵ:		��
*����:			�����¸����������
*��д:			
*�汾��Ϣ:		
******************************************************************************************/

pS_OsTaskREC_T *pS_OsNextActiveTask( void )
{
  pS_OsTaskREC_T  * TaskSech;

  // Start at the beginning
   TaskSech = TaskHead;

  // When found or not
  while (  TaskSech ) 
  {
      if ( TaskSech->events)  
	  	{
	  		// task is highest priority that is ready
      		return  TaskSech;
  		}
      TaskSech =  TaskSech->next;
  }
  return NULL;
}


/******************************************************************************************
*������:		pS_OsFindTask
*����:			��
*����ֵ:		��
*����:			��������
*��д:			
*�汾��Ϣ:		
******************************************************************************************/


pS_OsTaskREC_T *pS_OsFindTask( pU16 taskID )
{
	  pS_OsTaskREC_T * TaskSech;
	  TaskSech = TaskHead;
	  while(TaskSech)
	  {
			if(TaskSech->taskID == taskID)
			{
				return (TaskSech);
			
			}
		TaskSech = TaskSech->next;
	  }
	  return ((pS_OsTaskREC_T *)NULL);
}

/******************************************************************************************
*������:		pS_OsSetTaskEvent
*����:			��
*����ֵ:		��
*����:			��������,�ڳ�ʼ��ϵͳ֮�����
*��д:			
*�汾��Ϣ:		
******************************************************************************************/

pU8 pS_OsSetTaskEvent(pU16 TaskId,pU16 EventsFlag)
{

  pS_OsTaskREC_T  * srchTask;

  srchTask = pS_OsFindTask( TaskId );
  if ( srchTask ) 
  {
    //�ر��ж�
		IRQ_DISABLE_SAVE();
    // Stuff the event bit(s)
    srchTask->events |= EventsFlag;
    //�����ж�
		IRQ_ENABLE_RESTORE();

  }
  else
  return ( INVALID_TASK );

  return ( SUCCESS );
}

/******************************************************************************************
*������:		osal_set_event
*����:			
*����ֵ:		
*����:			��������
*��д:			
*�汾��Ϣ:		
******************************************************************************************/
pU8 osal_set_event( pU16 task_id, uint16 event_flag )
{
        pS_OsSetTaskEvent(task_id,event_flag);
        
        return ( SUCCESS );
}

/******************************************************************************************
*������:		pS_OsClearTaskEvent
*����:			��
*����ֵ:		��
*����:			�������
*��д:			
*�汾��Ϣ:		
******************************************************************************************/

pU8 pS_OsClearTaskEvent(pU16 TaskId,pU16 EventsFlag)
{

  pS_OsTaskREC_T  * srchTask;

  srchTask = pS_OsFindTask( TaskId );
  if ( srchTask ) 
  {
  		//�ر��ж�
			IRQ_DISABLE_SAVE();
      // Stuff the event bit(s)
      srchTask->events &= ~EventsFlag;
      //�����ж�
			IRQ_ENABLE_RESTORE();
  }
  else
  return ( INVALID_TASK );

  return ( SUCCESS );
}

/******************************************************************************************
*������:		osal_clear_event
*����:			
*����ֵ:		
*����:			�������
*��д:			
*�汾��Ϣ:		
******************************************************************************************/
pU8 osal_clear_event( pU16 task_id, uint16 event_flag )
{
        pS_OsClearTaskEvent(task_id,event_flag);

        return ( SUCCESS );
}






