#include "headfile.h"








void power_manage(void);


/******************************************************************************************
*������:		pS_OsHalProcessPoll
*����:			��
*����ֵ:		��
*����:			ѭ��������
*��д:			
*�汾��Ϣ:		
******************************************************************************************/

  void pS_OsHalProcessPoll(void);
/******************************************************************************************
*������:		pS_OsWatchDogFeed
*����:			��
*����ֵ:		��
*����:			ι������
*��д:			stardream
*�汾��Ϣ:		V1.0			2013��
******************************************************************************************/
  void pS_OsWatchDogFeed(void);


/******************************************************************************************
*������:		pS_OsTaskRegList
*����:			��
*����ֵ:		��
*����:			��������б�
*��д:			
*�汾��Ϣ:		
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
*������:		pS_OsRunSystem
*����:			��
*����ֵ:		��
*����:			����αϵͳ
*��д:			
*�汾��Ϣ:		
******************************************************************************************/
void pS_OsRunSystem(void)
{ 
	pU16  events;
	pU16  retEvents;

	pS_OsHalProcessPoll();
	//ÿ20ms����һ�������¼�
	pS_OsTimeClockUpdate();
	pS_OsWatchDogFeed();

	//ÿ�ζ������������в��Ҽ���������������������ȼ����У����Ը����ȼ�����������ȱ����ҵ�Ȼ��ִ��
	TaskActive = pS_OsNextActiveTask();
	if ( TaskActive )
	{
		//�ر��ж�
		IRQ_DISABLE_SAVE();
		events = TaskActive->events;
		// Clear the Events for this task
		//���¼���գ������´β�ѯ�������ʱ�����������û���¼����£��Ͳ��ᱻִ��
		TaskActive->events = 0;
		//�����ж�
		IRQ_ENABLE_RESTORE();

		if ( events != 0 )
		{
			// Call the task to process the event(s)
			if ( TaskActive->pfnEventProcessor )
			{
				retEvents = (TaskActive->pfnEventProcessor)( TaskActive->taskID, events );
                                //�ر��ж�
                                IRQ_DISABLE_SAVE();
                                TaskActive->events |= retEvents;
                                //�����ж�
                                IRQ_ENABLE_RESTORE();

			}
		}
	}
	else/*  û���¼�CPU  ��������*/
	{
		power_manage();
	}
}

/******************************************************************************************
*������:		pS_OsInitSystem
*����:			��
*����ֵ:		��
*����:			��ʼ��αϵͳ
*��д:			
*�汾��Ϣ:		
******************************************************************************************/
void pS_OsInitSystem(void)
{
  //��ʼ����ʱ��
  pS_OsTimerInit();
  //��ʼ������ͷ
  pS_OsTaskHeadInit();
  //��ʼ��ע�������б�
  pS_OsTaskRegList();
  //��ʼ��ϵͳ��Ϣ����
  pS_OsMsgInit();
  //ʹע��������Ч
  pS_OsTaskInit();
}

















