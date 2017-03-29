#include "headfile.h"








void power_manage(void);


/******************************************************************************************
*函数名:		pS_OsHalProcessPoll
*参数:			无
*返回值:		无
*描述:			循环处理函数
*编写:			
*版本信息:		
******************************************************************************************/

  void pS_OsHalProcessPoll(void);
/******************************************************************************************
*函数名:		pS_OsWatchDogFeed
*参数:			无
*返回值:		无
*描述:			喂狗函数
*编写:			stardream
*版本信息:		V1.0			2013年
******************************************************************************************/
  void pS_OsWatchDogFeed(void);


/******************************************************************************************
*函数名:		pS_OsTaskRegList
*参数:			无
*返回值:		无
*描述:			添加任务列表
*编写:			
*版本信息:		
******************************************************************************************/

 void pS_OsTaskRegList(void)
{
#if 0
	pS_OsRegTask(ClockInitTask,ClockEventHandler,1);
	pS_OsRegTask(HwInitTask,HwEventHandler,2);
	pS_OsRegTask(StepInitTask,StepEventHandler,3);
	pS_OsRegTask(SleepInitTask,SleepEventHandler,4);
	pS_OsRegTask(TouchKeyInitTask,TouchKeyEventHandler,5);
	pS_OsRegTask(BleInitTask,BleEventHandler,6);
	pS_OsRegTask(AppInitTask,AppEventHandler,7);
	pS_OsRegTask(DisplayInitTask,DisplayEventHandler,8);
	pS_OsRegTask(HrInitTask,HrEventHandler,9);
#else
	pS_OsRegTask(HrInitTask,HrEventHandler,1);
	pS_OsRegTask(AppInitTask,AppEventHandler,2);
	pS_OsRegTask(DisplayInitTask,DisplayEventHandler,3);
	pS_OsRegTask(BleInitTask,BleEventHandler,4);
	pS_OsRegTask(TouchKeyInitTask,TouchKeyEventHandler,5);
	pS_OsRegTask(SleepInitTask,SleepEventHandler,6);
	pS_OsRegTask(StepInitTask,StepEventHandler,7);
	pS_OsRegTask(HwInitTask,HwEventHandler,8);
	pS_OsRegTask(ClockInitTask,ClockEventHandler,9);
#endif
}

/******************************************************************************************
*函数名:		pS_OsRunSystem
*参数:			无
*返回值:		无
*描述:			运行伪系统
*编写:			
*版本信息:		
******************************************************************************************/
void pS_OsRunSystem(void)
{ 
	pU16  events;
	pU16  retEvents;

	pS_OsHalProcessPoll();
	//每20ms更新一次任务事件
	pS_OsTimeClockUpdate();
	pS_OsWatchDogFeed();

	//每次都从任务链表中查找激活的任务，任务链表按照优先级排列，所以高优先级的任务会优先被查找到然后执行
	TaskActive = pS_OsNextActiveTask();
	if ( TaskActive )
	{
		//关闭中断
		IRQ_DISABLE_SAVE();
		events = TaskActive->events;
		// Clear the Events for this task
		//将事件清空，这样下次查询该任务的时候，如果这个如果没有事件更新，就不会被执行
		TaskActive->events = 0;
		//开启中断
		IRQ_ENABLE_RESTORE();

		if ( events != 0 )
		{
			// Call the task to process the event(s)
			if ( TaskActive->pfnEventProcessor )
			{
				retEvents = (TaskActive->pfnEventProcessor)( TaskActive->taskID, events );
                                //关闭中断
                                IRQ_DISABLE_SAVE();
                                TaskActive->events |= retEvents;
                                //开启中断
                                IRQ_ENABLE_RESTORE();

			}
		}
	}
	else/*  没有事件CPU  进入休眠*/
	{
		power_manage();
	}
}

/******************************************************************************************
*函数名:		pS_OsInitSystem
*参数:			无
*返回值:		无
*描述:			初始化伪系统
*编写:			
*版本信息:		
******************************************************************************************/
void pS_OsInitSystem(void)
{
  //初始化定时器
  pS_OsTimerInit();
  //初始化任务头
  pS_OsTaskHeadInit();
  //初始化注册任务列表
  pS_OsTaskRegList();
  //初始化系统消息机制
  pS_OsMsgInit();
  //使注册任务生效
  pS_OsTaskInit();
}

















