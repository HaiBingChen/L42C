#include "headfile.h"




uint16  TimeoutCnt=0;
int16  MotoVibrateTimes=0;
uint16	MotoVibrateDuration=0;
uint16	MotoVibrateInterval=0;
bool    MotoMix =false,MotoMixFlag =false;
/******************************************************************************************
*函数名:		InitMotorVibrate
*参数:			
*返回值:		
*描述:			停止振动
*编写:			
*版本信息:	
******************************************************************************************/
void InitMotorVibrate(void)
{
		//MOTOR_PIN_OUT;
		//MOTOR_PIN_L;
		PWM_init();
}
/******************************************************************************************
*函数名:		StartMotorVibrate
*参数:			无
*返回值:		
*描述:			启动马达振动
*编写:			
*版本信息:	
******************************************************************************************/
void StartMotorVibrate(void)
{
	//if (shortcut_info.vibration_level)	//enable vibration
		//MOTOR_PIN_H;
	Set_Motor_Level();
}

/******************************************************************************************
*函数名:		StopMotorVibrate
*参数:			
*返回值:		
*描述:			停止振动
*编写:			
*版本信息:	
******************************************************************************************/
void StopMotorVibrate(void)
{
	//if (shortcut_info.vibration_level)	//enable vibration
		//MOTOR_PIN_L;
	Set_Motor_LevelLow();        
}

uint8 GetMotorVibrateTimes(void)
{
	if(MotoVibrateTimes>0)
		return 1;
	else
		return 0;
}


void StopMotorRunTime(void)
{
	osal_stop_timerEx(GetHwTaskId(),HW_MOTOR_REPEAT_WORK_EVENT);	
	StopMotorVibrate();
	MotoVibrateTimes = 0;
}

void MotorRunTime(uint8 nTimes, uint16 Duration, uint16 Interval, bool mix)
{
	if (!nTimes) {
		return;
	}
	if(nTimes==0xFF)
		MotoVibrateTimes=0x0FFF;	
	else 
		MotoVibrateTimes = nTimes*2;
	
	MotoMix = mix;
	MotoVibrateDuration = (Duration)?(Duration*1200):(300);
	MotoVibrateInterval = (Interval)?(Interval*1200):(800);

	osal_stop_timerEx(GetHwTaskId(),HW_MOTOR_REPEAT_WORK_EVENT);	
	MotorRepeatRun();
}

void MotorRepeatRun(void)
{
	uint16  TimeoutCnt=0;
	
    if(MotoVibrateTimes%2 == 0) {
		StartMotorVibrate();
		if(!MotoMix)
			TimeoutCnt = MotoVibrateDuration;
		else
		{
			if(MotoMixFlag)
			{
				TimeoutCnt = MotoVibrateDuration*4;
				MotoMixFlag =false;
			}
			else
			{
				TimeoutCnt=MotoVibrateDuration;
				MotoMixFlag =true;
			}
		}
    }
    else {
		StopMotorVibrate();
		TimeoutCnt = MotoVibrateInterval;
    }
	
    if((--MotoVibrateTimes)> 0)
            osal_start_timerEx(GetHwTaskId(), HW_MOTOR_REPEAT_WORK_EVENT, TimeoutCnt);	
    else
	     StopMotorRunTime();	
}



void MotoRun(MOTO_T type)
{
	uint8 mTimes=0;
	uint16 mdur,mInl=0;
	bool mmix=false;
	if(MotoVibrateTimes>0x10)	/*不影响持续震动*/
		return;
	if(type == MOTO_DEFAULT)
	{
		mTimes =1;				/*单次短震动*/
		mdur=0;	
	}
	else if(type == MOTO_1S)
	{
		mTimes =1;				/*单次短震动*/
		mdur=1;	
	}	
	else
	{
		switch(MotoOrder[type])
		{
			case 0:				/*关闭不震动*/
				break;
			case 1:				/*单次长震动*/
				mTimes =1;
				mdur=1;
				break;
			case 2:				/*单次短震动*/
				mTimes =1;
				mdur=0;		
				break;
			case 3:				/*间歇2次长震动*/
				mTimes =2;
				mdur=1;		
				break;
			case 4:				/*间歇2次短震动*/
				mTimes =2;
				mdur=0;			
				break;	
			case 5:				/*长震动短震动交替*/
				mTimes =0xff;
				mdur=0;	
				mmix =true;
				MotoMixFlag = true;
				break;		
			case 6:				/*一直长震*/
				mTimes =0xff;
				mdur=1;			
				break;
			case 7:				/*一直短震*/
				mTimes =0xff;
				mdur=0;			
				break;			
			default:
				break;
		}
	}
	if(mTimes==0 && mdur==0 && mdur==0)
		StopMotorRunTime();
	else
		MotorRunTime(mTimes,mdur,mInl,mmix);
	
}

