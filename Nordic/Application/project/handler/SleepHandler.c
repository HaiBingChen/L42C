#include "headfile.h"




SleepHadlerTAG SleepHadlerId;
bool SleepGoalOverRemindEnable = false;
uint8 sleep_index = 0;
static uint8 sleep_lastday =0;
uint8 SleepKey=0;

void SetSleepGoalOverRemind(void)
{
	SleepGoalOverRemindEnable =true;
}

uint8 CheckSleepGoalOverRemind(void)
{
	if ((DailyGoalOverRemindEnable & (1<<3)) && (((MinuteCount/60)) >= DailyGoal[3].Goal)) {
		DailyGoalOverRemindEnable &= ~(1<<3); // 今天不再提醒了
		return DAILYGOAL_OVER_TYPE_SLEEP;
	}
	return 0;
}


static void change_sleep_mode(void)
{
	if(SLEEP_MODE_DISPLAY_FLAG)
	{
		if(!SLEEP_MODE_FLAG)
		{
			osal_start_timerEx(GetAppTaskId(), MSG_REMOVE_SLEEP_MONITOR, 200);
			GotoSleepMode();
		}
	}
	else
	{
		if(SLEEP_MODE_FLAG)
		{
			GotoNormalMode(true);
		}
	}
}

uint16 SleepStart_Handler(MsgType msg, int iParam, void *pContext)
{
	uint8 SleepFlagTemp = 0;
	switch(msg)
	{
		case MSG_HANDLER_BEGIN:
			SleepHadlerId.SleepStartId = iParam;   
			SleepKey=0;
			osal_start_timerEx(GetAppTaskId(), MSG_SLEEP_MODE_CHANGE, 200);
			break;

		case MSG_HANDLER_END:
			SleepHadlerId.SleepStartId = 0;
			break;

		case MSG_SLEEP_MODE_START:
		case MSG_SLEEP_MODE_EXIT:
			if(CheckpCurrentHandler(SleepHadlerId.SleepStartId))
			{
				ClearScreen();
				if(!SLEEP_MODE_DISPLAY_FLAG&&SleepKey==0)
					MotoRun(MOTO_DEFAULT);
				SleepKey =0;
				osal_start_timerEx(GetAppTaskId(), MSG_SLEEP_MODE_CHANGE, 100);
				SetSystemSuspendTime(DISPLAY_TIME_10S_FLAG);
			}
			else
			{
				UnloadHandler(SleepHadlerId.SleepStartId);
				SendMsg(msg); 	
				return RET_MSG_HANDLED;
			}
		case MSG_GOAL_CHANGED:	
		case MSG_DISPLAY:
			if(!SLEEP_MODE_DISPLAY_FLAG)					/*睡眠按钮界面*/
			{
				//ShowSleepStart();
				if(!SLEEP_MODE_FLAG)
					UnloadHandler(SleepHadlerId.SleepStartId);				
			}
			else											/*进入睡眠界面*/
			{
				if(!SLEEP_MODE_FLAG)
					MinuteCount=0;
				ShowSleepEnd(MinuteCount);
				SetSystemSuspendTime(DISPLAY_TIME_10S_FLAG);				
				if((!SLEEP_MODE_FLAG)&&(BC5_State != BC5_BT_POWEROFF))
				{
					osal_start_timerEx(GetAppTaskId(), MSG_SLEEP_CLOSE_BT, 300);	/*睡眠关闭BC5，节省功耗*/
				}
			}
			break;
			
		case MSG_SLEEP_MODE_FORCE_EXIT:			/*--->强制唤醒睡眠模式:如插USB*/
			SLEEP_MODE_DISPLAY_FLAG=false;
		case MSG_SLEEP_MODE_CHANGE:
			SleepFlagTemp = SLEEP_MODE_FLAG;
			change_sleep_mode();
			if(SleepFlagTemp&&(!SLEEP_MODE_FLAG))
				UnloadHandler(SleepHadlerId.SleepStartId);
			break;
/*用户操作*/			
		case MSG_LEFTDOWN_SHORTKEY:				/*左键 单击*/	
		case MSG_LEFTDOWN_DOUBLE_SHORTKEY:		/*左键 双击*/
		case MSG_LEFTDOWN_LONGKEY:           			/*左键 长按*/
		case MSG_RIGHTDOWN_DOUBLE_SHORTKEY:   	/*右键 双击*/	
		case MSG_RIGHTDOWN_LONGKEY:				/*右键 长按*/	
		case MSG_RIGHTDOWN_LONGKEY_UP:			/*右键 长按松开*/		
		case MSG_TOUCH_UP_FLICK:					/*上*/
		case MSG_TOUCH_DOWN_FLICK:				/*下*/
		case MSG_TOUCH_UP_DOUBLE_SHORTKEY:		/*触摸 双击*/	
		case MSG_TOUCH_LONGKEY:					/*触摸 长按*/
			break;
			
		case MSG_TOUCH_UP_SHORTKEY: 	/*单击*/		
			{
				uint8 TouchID = Get_Screen_Locaton(3,3);
				switch(TouchID)
				{
					case 0:
					case 1:
					case 2:
					case 3:		
					case 4:
					case 5:
					case 7:
					case 8:
						return RET_MSG_HANDLED;
					case 6:
						if(SLEEP_MODE_DISPLAY_FLAG)
						{
							CleanAutoSleepFlag();			
		                    			ChargeDisplayStateChange(false);
							SLEEP_MODE_DISPLAY_FLAG=false;
							SendMsg(MSG_SLEEP_MODE_EXIT);						
						}		
						break;

					default:
						return RET_MSG_HANDLED;                        
				}
				break;
			}	

		case MSG_TOUCH_DOWN_UP_FLICK:		/*左*/ 
			break;	

		case MSG_TOUCH_UP_DOWN_FLICK: 	/*右*/
#if 0
			if (!SLEEP_MODE_FLAG)
			{
				UnloadHandler_WithoutDisplay(SleepHadlerId.SleepStartId);
				LoadHandler(SleepMonitor_Handler, 0);
			}
#endif
			break;
			
		case MSG_RIGHTDOWN_SHORTKEY: 				/*右键 单击*/
#if 0
			if(!SLEEP_MODE_FLAG)	
				UnloadHandler(SleepHadlerId.SleepStartId);
#endif
			if(SLEEP_MODE_DISPLAY_FLAG)
			{
				CleanAutoSleepFlag();			
				ChargeDisplayStateChange(false);
				SLEEP_MODE_DISPLAY_FLAG=false;
				osal_start_timerEx(GetAppTaskId(), MSG_SLEEP_MODE_EXIT, 100);							
			}
			break;

		case MSG_FORCE_BACK_HOME:
			break;

		default:
			return RET_MSG_UNHANDLED;
    }
    return RET_MSG_HANDLED;
}

uint16 SleepDetail_Handler(MsgType msg, int iParam, void *pContext)
{
	switch(msg)
	{
		case MSG_HANDLER_BEGIN:
			SleepHadlerId.SleepDetailId = iParam;
			break;

		case MSG_HANDLER_END:
			SleepHadlerId.SleepDetailId = 0;
			break;
/*用户操作*/
		case MSG_LEFTDOWN_SHORTKEY:				/*左键 单击*/	
		case MSG_LEFTDOWN_DOUBLE_SHORTKEY:		/*左键 双击*/
		case MSG_LEFTDOWN_LONGKEY:           			/*左键 长按*/
		case MSG_RIGHTDOWN_DOUBLE_SHORTKEY:   	/*右键 双击*/	
		case MSG_RIGHTDOWN_LONGKEY:				/*右键 长按*/	
		case MSG_RIGHTDOWN_LONGKEY_UP:			/*右键 长按松开*/		
		case MSG_TOUCH_UP_DOWN_FLICK: 			/*右*/
		case MSG_TOUCH_UP_DOUBLE_SHORTKEY:		/*触摸 双击*/	
		case MSG_TOUCH_LONGKEY:					/*触摸 长按*/
			break;
			
		case MSG_RIGHTDOWN_SHORTKEY: 				/*右键 单击*/	

			UnloadHandler(SleepHadlerId.SleepDetailId);
			break;

		case MSG_TOUCH_DOWN_UP_FLICK:	/*左*/ 
			UnloadHandler_WithoutDisplay(SleepHadlerId.SleepMonitorId);
			break;
			
		case MSG_TOUCH_UP_FLICK:			/*上*/
			break;
			
		case MSG_TOUCH_DOWN_FLICK:		/*下*/
			break;
		
		case MSG_TOUCH_UP_SHORTKEY: 	/*单击*/
			break;
	
		case MSG_DISPLAY:
			ShowSleepDetail(sleep_lastday);
			break;

		case MSG_REMOVE_SLEEP_MONITOR:
		//	UnloadHandler(SleepHadlerId.SleepMonitorId);
			break;

		default:
			return RET_MSG_UNHANDLED;
	}
	return RET_MSG_HANDLED;
}

uint16 SleepOverview_Handler(MsgType msg, int iParam, void *pContext)
{
	switch(msg)
	{
		case MSG_HANDLER_BEGIN:
			SleepHadlerId.SleepOverviewId = iParam;
			sleep_lastday =10;
			sleep_index=0;
			break;

		case MSG_HANDLER_END:
			SleepHadlerId.SleepOverviewId = 0;
			break;
/*用户操作*/
		case MSG_LEFTDOWN_SHORTKEY:				/*左键 单击*/	
		case MSG_LEFTDOWN_DOUBLE_SHORTKEY:		/*左键 双击*/
		case MSG_LEFTDOWN_LONGKEY:           			/*左键 长按*/
		case MSG_RIGHTDOWN_DOUBLE_SHORTKEY:   	/*右键 双击*/	
		case MSG_RIGHTDOWN_LONGKEY:				/*右键 长按*/	
		case MSG_RIGHTDOWN_LONGKEY_UP:			/*右键 长按松开*/		
		case MSG_TOUCH_UP_DOWN_FLICK: 			/*右*/
		case MSG_TOUCH_UP_DOUBLE_SHORTKEY:		/*触摸 双击*/	
		case MSG_TOUCH_LONGKEY:					/*触摸 长按*/
			break;
			
		case MSG_RIGHTDOWN_SHORTKEY: 				/*右键 单击*/	
			UnloadHandler(SleepHadlerId.SleepOverviewId);
			break;

		case MSG_TOUCH_DOWN_UP_FLICK:	/*左*/ 
			UnloadHandler_WithoutDisplay(SleepHadlerId.SleepMonitorId);
			break;
			
		case MSG_TOUCH_UP_FLICK:			/*上*/
			if(sleep_index==0&&SleepDayCount>3)
			{
				sleep_index=1;
				SendMsg(MSG_DISPLAY);
			}
			else if(sleep_index==1&&SleepDayCount>6)
			{
				sleep_index=2;
				SendMsg(MSG_DISPLAY);
			}
			break;
			
		case MSG_TOUCH_DOWN_FLICK:		/*下*/
			if(sleep_index==2)
			{
				sleep_index=1;
				SendMsg(MSG_DISPLAY);
			}
			else if(sleep_index==1)
			{
				sleep_index=0;
				SendMsg(MSG_DISPLAY);
			}
			break;
		
		case MSG_TOUCH_UP_SHORTKEY: 	/*单击*/
			{
				uint8 TouchID = Get_Screen_Locaton(3,1);
				if (sleep_index == 0)
				{
					switch(TouchID)
					{
						case 0:
							if(SleepDayCount>0)
								sleep_lastday =0;
							else
								sleep_lastday =10;
							break;
						case 1:
							if(SleepDayCount>1)
								sleep_lastday =1;
							else
								sleep_lastday =10;		
							break;
						case 2:
							if(SleepDayCount>2)
								sleep_lastday =2;
							else
								sleep_lastday =10;							
							break;
						default:
							return RET_MSG_HANDLED;
					}
				}
				else if (sleep_index == 1)
				{
					switch(TouchID)
					{
						case 0:
							if(SleepDayCount>3)
								sleep_lastday =3;
							else
								sleep_lastday =10;							
							break;
						case 1:
							if(SleepDayCount>4)
								sleep_lastday =4;
							else
								sleep_lastday =10;	
							break;
						case 2:
							if(SleepDayCount>5)
								sleep_lastday =5;
							else
								sleep_lastday =10;	
							break;
						default:
							return RET_MSG_HANDLED;
					}
				}
				else if (sleep_index == 2)
				{
					switch(TouchID)
					{
						case 0:
							if(SleepDayCount>6)
								sleep_lastday =6;
							else
								sleep_lastday =10;		
							break;
						case 1:
							sleep_lastday =10;	
							break;
						case 2:
							sleep_lastday =10;	
							break;
						default:
							return RET_MSG_HANDLED;
					}
				}
				//if(sleep_lastday<=6)
				//	LoadHandler(SleepDetail_Handler,0);
				break;
			}	
	
		case MSG_DISPLAY:
			{
				if (sleep_index == 0)
				{
					ShowSleepOverviewOne();
				}
				else if (sleep_index == 1)
				{
					ShowSleepOverviewTwo();;
				}
				else if (sleep_index == 2)
				{
					ShowSleepOverviewThree();
				}
			}
			break;

		case MSG_REMOVE_SLEEP_MONITOR:
			UnloadHandler(SleepHadlerId.SleepOverviewId);
			break;

		default:
			return RET_MSG_UNHANDLED;
	}
	return RET_MSG_HANDLED;
}

uint16 ExitSleep_Handler(MsgType msg, int iParam, void *pContext)
{
	switch(msg)
	{
		case MSG_HANDLER_BEGIN:
			SleepHadlerId.ExitSleepId = iParam;
			ExitSleepKey=false;
			break;
			
		case MSG_HANDLER_END:
			SleepHadlerId.ExitSleepId = 0;
			StopMotorRunTime();
			break;
			
		case MSG_LEFTDOWN_SHORTKEY:				/*左键 单击*/
		case MSG_LEFTDOWN_DOUBLE_SHORTKEY:		/*左键 双击*/
		case MSG_LEFTDOWN_LONGKEY:           			/*左键 长按*/
		case MSG_RIGHTDOWN_DOUBLE_SHORTKEY:   	/*右键 双击*/	
		case MSG_RIGHTDOWN_LONGKEY:				/*右键 长按*/	
		case MSG_RIGHTDOWN_LONGKEY_UP:			/*右键 长按松开*/		
		case MSG_TOUCH_DOWN_UP_FLICK:				/*左*/ 	
		case MSG_TOUCH_UP_DOWN_FLICK: 			/*右*/
		case MSG_TOUCH_UP_FLICK:					/*上*/
		case MSG_TOUCH_DOWN_FLICK:				/*下*/
		case MSG_TOUCH_UP_DOUBLE_SHORTKEY:		/*触摸 双击*/	
		case MSG_TOUCH_LONGKEY:					/*触摸 长按*/
		case MSG_TOUCH_UP_SHORTKEY: 				/*单击*/			
			break;
			
		case MSG_EXIT_SLEEP_HANDLER_TIMEOUT:		
		case MSG_RIGHTDOWN_SHORTKEY: 				/*右键 单击*/	
			UnloadHandler(SleepHadlerId.ExitSleepId);
			break;
		
		case MSG_DISPLAY:
			ShowExitSleep();
			MotoRun(MOTO_1S);
			osal_start_timerEx(GetAppTaskId(), MSG_EXIT_SLEEP_HANDLER_TIMEOUT, 3000);
			break;
			
		default:
			return RET_MSG_UNHANDLED;			
	}
	return RET_MSG_HANDLED;
}

uint16 SleepMonitor_Handler(MsgType msg, int iParam, void *pContext)
{
	switch(msg)
	{
		case MSG_HANDLER_BEGIN:
			SleepHadlerId.SleepMonitorId = iParam;
			ExitSleepKey=false;
			break;

		case MSG_HANDLER_END:
			SleepHadlerId.SleepMonitorId = 0;
			break;
/*用户操作*/
		case MSG_LEFTDOWN_SHORTKEY:				/*左键 单击*/
		case MSG_LEFTDOWN_DOUBLE_SHORTKEY:		/*左键 双击*/
		case MSG_LEFTDOWN_LONGKEY:           			/*左键 长按*/
		case MSG_RIGHTDOWN_DOUBLE_SHORTKEY:   	/*右键 双击*/	
		case MSG_RIGHTDOWN_LONGKEY:				/*右键 长按*/	
		case MSG_RIGHTDOWN_LONGKEY_UP:			/*右键 长按松开*/		
		case MSG_TOUCH_UP_DOWN_FLICK: 			/*右*/
		case MSG_TOUCH_UP_DOUBLE_SHORTKEY:		/*触摸 双击*/	
		case MSG_TOUCH_LONGKEY:					/*触摸 长按*/
			break;
		
		case MSG_RIGHTDOWN_SHORTKEY: 				/*右键 单击*/	
			if(!SLEEP_MODE_FLAG)	
				UnloadHandler(SleepHadlerId.SleepMonitorId);
			else
				LoadHandler(ExitsleepmodeHandler,0);
			break;
		case MSG_FORCE_BACK_HOME:
			if(!SLEEP_MODE_FLAG)	
				return RET_MSG_UNHANDLED;
			else
			{
				if(CheckpCurrentHandler(SleepHadlerId.SleepMonitorId))
				{
					IsLeftRight = 1;
					LoadHandler(ExitsleepmodeHandler,0);
					break;
				}
				else
				{
					return RET_MSG_UNHANDLED;
				}
					
			}
		case MSG_TOUCH_DOWN_UP_FLICK:		/*左*/ 
			break;
			
		case MSG_TOUCH_UP_FLICK:			/*上*/
			break;
			
		case MSG_TOUCH_DOWN_FLICK:		/*下*/
			break;
		
		case MSG_TOUCH_UP_SHORTKEY: 		/*单击*/
			{
				uint8 TouchID = Get_Screen_Locaton(3,3);
				switch(TouchID)
				{
					case 0:
					case 1:
					case 2:
					case 3:		
					case 4:
					case 5:
					case 7:					
						break;
					case 6:
						if(!SLEEP_MODE_FLAG)
						{
							AutoEnterSleep = false;
							SendMsg(MSG_SLEEP_MODE_START);	
						}
						else
						{
							ExitSleepKey=true;
							ShowSleepMonitor(0, MinuteCount,SecondCount, 0, 0);
							osal_start_timerEx(GetAppTaskId(), MSG_SLEEP_MODE_EXIT, 100);
						}
						break;
					case 8:
						LoadHandler(SleepOverview_Handler,0);
						break;

					default:
						return RET_MSG_HANDLED;
				}
				break;
			}	
		case MSG_SLEEP_MODE_EXIT:					/*--->退出睡眠模式*/	
			if(SLEEP_MODE_FLAG)
			{
				GotoNormalMode(true);
				if(!ExitSleepKey)
				{
					LoadHandler(ExitSleep_Handler,0);
				}
				else if(CheckpCurrentHandler(SleepHadlerId.SleepMonitorId))
					SendMsg(MSG_DISPLAY);
				ExitSleepKey=false;
			}			
			break;
		case MSG_SLEEP_MODE_START:					/*--->进入睡眠模式*/
			if(!SLEEP_MODE_FLAG)
			{
				GotoSleepMode();
				if(CheckpCurrentHandler(SleepHadlerId.SleepMonitorId))
					SendMsg(MSG_DISPLAY);
			}			
			break;
			
		case MSG_SLEEP_MODE_FORCE_EXIT:			/*--->强制唤醒睡眠模式:如插USB*/		
			if(SLEEP_MODE_FLAG && USB_GO_IN_FLAG)
			{
				ChargeDisplayStateChange(false);
				SLEEP_MODE_DISPLAY_FLAG=false;
				osal_start_timerEx(GetAppTaskId(), MSG_SLEEP_MODE_EXIT, 100);
			}
			break;	
		case MSG_GOAL_CHANGED:
		case MSG_DATE_TIME_FORMAT_UPDATE:	
		case MSG_DISPLAY:
			{
				SLUMBER_DATA_T SleepData;
				//GetSleepStatistic(&SleepData);
				if(SecondCount>59)
					SecondCount = 0;
				//ShowSleepMonitor(SleepData.SleepQuality, sleep_info.total_sleep, SecondCount,0);
				ShowSleepMonitor(SleepData.SleepQuality, MinuteCount, SecondCount,0,SLEEP_MODE_FLAG);
			}
			break;

		case MSG_REMOVE_SLEEP_MONITOR:
			UnloadHandler(SleepHadlerId.SleepMonitorId);
			break;

		default:
			return RET_MSG_UNHANDLED;
	}
	return RET_MSG_HANDLED;
}

uint16 SleepChange_Handler(MsgType msg, int iParam, void *pContext)
{
	static uint32 min = 420;	//7 hour
	
	switch(msg)
	{
		case MSG_HANDLER_BEGIN:
			SleepHadlerId.SleepChangeId = iParam;
			break;

		case MSG_HANDLER_END:
			SleepHadlerId.SleepChangeId = 0;
			break;
/*用户操作*/
		case MSG_LEFTDOWN_SHORTKEY:				/*左键 单击*/	
		case MSG_LEFTDOWN_DOUBLE_SHORTKEY:		/*左键 双击*/
		case MSG_LEFTDOWN_LONGKEY:           			/*左键 长按*/
		case MSG_RIGHTDOWN_SHORTKEY: 				/*右键 单击*/	
		case MSG_RIGHTDOWN_DOUBLE_SHORTKEY:   	/*右键 双击*/	
		case MSG_RIGHTDOWN_LONGKEY:				/*右键 长按*/	
		case MSG_RIGHTDOWN_LONGKEY_UP:			/*右键 长按松开*/		
		case MSG_TOUCH_UP_FLICK:					/*上*/
		case MSG_TOUCH_DOWN_FLICK:				/*下*/
		case MSG_TOUCH_UP_DOUBLE_SHORTKEY:		/*触摸 双击*/	
		case MSG_TOUCH_LONGKEY:					/*触摸 长按*/
			break;

		case MSG_TOUCH_DOWN_UP_FLICK:	/*左*/ 
			UnloadHandler(SleepHadlerId.SleepChangeId);
			break;

		case MSG_TOUCH_UP_DOWN_FLICK: 	/*右*/
			LoadHandler(SleepMonitor_Handler,0);
			break;

		case MSG_TOUCH_UP_SHORTKEY: 	/*单击*/		
			{
				uint8 TouchID = Get_Screen_Locaton(3,3);
				switch(TouchID)
				{
					case 0:
					case 1:
					case 2:
						break;

					case 3:		//-
						min -= 10;
						break;

					case 4:
						break;

					case 5:		//+
						min += 10;
						break;
					
					case 6:
						break;
					
					case 7:		//OK
						ShowUpdateDone();
						osal_start_timerEx(GetAppTaskId(), MSG_DISPLAY, 2000);
						return RET_MSG_HANDLED;

					case 8:
						break;

					default:
						return RET_MSG_HANDLED;
				}
				SendMsg(MSG_DISPLAY);
				break;
			}	


		case MSG_DISPLAY:
			ShowSleep(min);
			break;


		default:
		return RET_MSG_UNHANDLED;
	}
	return RET_MSG_HANDLED;
}



