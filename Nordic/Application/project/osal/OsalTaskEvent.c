#include "headfile.h"




 pS_OsTaskREC_T   * TaskHead;//the point to link'head
 pS_OsTaskREC_T   * TaskActive;

 pU16 TaskId;



 
/******************************************************************************************
*函数名:		pS_OsTaskHeadInit
*参数:			无
*返回值:		无
*描述:			初始化系统任务头
*编写:			
*版本信息:		
******************************************************************************************/

void  pS_OsTaskHeadInit(void )
{
  TaskHead    = (pS_OsTaskREC_T *)NULL;
  TaskActive  = (pS_OsTaskREC_T *)NULL;
  TaskId = 0;
}
/******************************************************************************************
*函数名:		pS_OsTaskInit
*参数:			无
*返回值:		无
*描述:			初始化任务，在注册任务后调用
*编写:			
*版本信息:		
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
*函数名:		pS_OsRegTask
*参数:			无
*返回值:		无
*描述:			任务注册
*编写:			
*版本信息:		
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
		
		TaskPTR = &TaskHead;//改变TaskHead
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
*函数名:		pS_OsNextActiveTask
*参数:			无
*返回值:		无
*描述:			查找下个激活的任务
*编写:			
*版本信息:		
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
*函数名:		pS_OsFindTask
*参数:			无
*返回值:		无
*描述:			查找任务
*编写:			
*版本信息:		
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
*函数名:		pS_OsSetTaskEvent
*参数:			无
*返回值:		无
*描述:			设置任务,在初始化系统之后调用
*编写:			
*版本信息:		
******************************************************************************************/

pU8 pS_OsSetTaskEvent(pU16 TaskId,pU16 EventsFlag)
{

  pS_OsTaskREC_T  * srchTask;

  srchTask = pS_OsFindTask( TaskId );
  if ( srchTask ) 
  {
    //关闭中断
		IRQ_DISABLE_SAVE();
    // Stuff the event bit(s)
    srchTask->events |= EventsFlag;
    //开启中断
		IRQ_ENABLE_RESTORE();

  }
  else
  return ( INVALID_TASK );

  return ( SUCCESS );
}

/******************************************************************************************
*函数名:		osal_set_event
*参数:			
*返回值:		
*描述:			设置任务
*编写:			
*版本信息:		
******************************************************************************************/
pU8 osal_set_event( pU16 task_id, uint16 event_flag )
{
        pS_OsSetTaskEvent(task_id,event_flag);
        
        return ( SUCCESS );
}

/******************************************************************************************
*函数名:		pS_OsClearTaskEvent
*参数:			无
*返回值:		无
*描述:			清除任务
*编写:			
*版本信息:		
******************************************************************************************/

pU8 pS_OsClearTaskEvent(pU16 TaskId,pU16 EventsFlag)
{

  pS_OsTaskREC_T  * srchTask;

  srchTask = pS_OsFindTask( TaskId );
  if ( srchTask ) 
  {
  		//关闭中断
			IRQ_DISABLE_SAVE();
      // Stuff the event bit(s)
      srchTask->events &= ~EventsFlag;
      //开启中断
			IRQ_ENABLE_RESTORE();
  }
  else
  return ( INVALID_TASK );

  return ( SUCCESS );
}

/******************************************************************************************
*函数名:		osal_clear_event
*参数:			
*返回值:		
*描述:			清除任务
*编写:			
*版本信息:		
******************************************************************************************/
pU8 osal_clear_event( pU16 task_id, uint16 event_flag )
{
        pS_OsClearTaskEvent(task_id,event_flag);

        return ( SUCCESS );
}






