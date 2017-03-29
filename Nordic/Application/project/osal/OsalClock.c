#include "headfile.h"




//���ٺ������һ��ϵͳʱ��
#define UPDATE_TIME_N_MS        (20)
#define MAX_TICKS_MASK          (0xFFFFFFFF)
#define ADJUST_TICKS_1MIN		(59)



pU32 gPs_SysTicks=0;
pU32 PreviousTimerTicks=0;//���ڱ��浱ǰʱ��Ticks
pU32 gPs_OsTimeSeconds=0;



pU32 pS_OsGetSysTick(void)
{
	return gPs_SysTicks;
}



/******************************************************************************************
*������:		pS_OsTimeClockUpdate
*����:			��
*����ֵ:		��
*����:			���ڸ���ϵͳʱ�Ӻ�����ʱ��
*��д:			
*�汾��Ϣ:		
******************************************************************************************/

void pS_OsTimeClockUpdate( void )
{
  //pU32 TicksnMs=0;
  pU32 ElapsedMSec=0;

  //gPs_SysTicksÿ20ms����һ�Σ������ڴ�ѭ����ÿ��ִ��pS_OsTimeClockUpdate���������ʱ��gPs_SysTicks��һ�����£����û����ʲô������
  if (PreviousTimerTicks!=gPs_SysTicks )
  {
    if(PreviousTimerTicks>gPs_SysTicks)//���ʱ��
    {
      ElapsedMSec=MAX_TICKS_MASK-PreviousTimerTicks+gPs_SysTicks;
    }
    else
    {
	  //PreviousTimerTicksΪ�ϴε�ʱ�������gPs_SysTicksΪ��ǰ��ʱ�����
	  ElapsedMSec=gPs_SysTicks-PreviousTimerTicks;
    }

    PreviousTimerTicks=gPs_SysTicks;
   
    //ElapsedMSec=(pU32)( TicksnMs )%0xffffffff;//n ms����һ��
    
    if ( ElapsedMSec )
    {
      pS_OsClockUpdate( ElapsedMSec );//�����û�ʱ���ã��������ӵ�
      pS_OsTimerUpdate( ElapsedMSec );//���ڸ���ϵͳ��ʱ�ӣ�����ʱ�Ӷ�ʱ��
    }
  }
  
}
/******************************************************************************************
*������:		pS_OsClockUpdate
*����:			��
*����ֵ:		��
*����:			����ϵͳʱ��
*��д:			
*�汾��Ϣ:		
******************************************************************************************/

void pS_OsClockUpdate( pU32 nMSec )//����ʱ��
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
*������:		pS_OsHwTimerUpdate
*����:			��
*����ֵ:		��
*����:			����ϵͳʱ�ӣ���Ӳ����ʱ���е���1ms ��10ms ��100ms  ��1s
*��д:			
*�汾��Ϣ:		
******************************************************************************************/
 void pS_OsHwTimerUpdate(void)
{
	//�ر��ж�
	IRQ_DISABLE_SAVE();
	
	gPs_SysTicks+=UPDATE_TIME_N_MS;
	
	//�����ж�
	IRQ_ENABLE_RESTORE();
}

void pS_OsHwTimerAdjust_1min(void)
{
	//�ر��ж�
	//IRQ_DISABLE_SAVE();
	
	gPs_SysTicks+=ADJUST_TICKS_1MIN;
	
	//�����ж�
	//IRQ_ENABLE_RESTORE();
}



