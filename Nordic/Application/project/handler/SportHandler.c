#include "headfile.h"


SportHadlerTAG SportHadlerId;
bool Syncing_Handler_flag =FALSE;
uint8 DailyGoalOverRemindEnable = 0;
uint8 DailyGoalOverType = 0;

void SetDefaultGoal(void)
{
	uint8 GoalTemp[DAILY_GOAL_LEN]={0x00};
	DailyGoal[0].Goal = 70;
	DailyGoal[0].MainGoalFlag = 1;
	DailyGoal[1].Goal = 350;
	DailyGoal[1].MainGoalFlag = 1;
	DailyGoal[2].Goal = 5;
	DailyGoal[2].MainGoalFlag = 1;
	DailyGoal[3].Goal = 8;
	DailyGoal[3].MainGoalFlag = 1;	
	DailyGoal[4].Goal = 30;
	DailyGoal[4].MainGoalFlag = 1;	
	
	GoalTemp[0]=DailyGoal[0].Goal & 0xFF;
	GoalTemp[1]=(DailyGoal[0].Goal>>8) & 0xFF;
	GoalTemp[2]=DailyGoal[0].MainGoalFlag;
	
	GoalTemp[3]=DailyGoal[1].Goal & 0xFF;
	GoalTemp[4]=(DailyGoal[1].Goal>>8) & 0xFF;
	GoalTemp[5]=DailyGoal[1].MainGoalFlag;
	
	GoalTemp[6]=DailyGoal[2].Goal & 0xFF;
	GoalTemp[7]=(DailyGoal[2].Goal>>8) & 0xFF;
	GoalTemp[8]=DailyGoal[2].MainGoalFlag;
	
	GoalTemp[9]=DailyGoal[3].Goal & 0xFF;
	GoalTemp[10]=(DailyGoal[3].Goal>>8) & 0xFF;
	GoalTemp[11]=DailyGoal[3].MainGoalFlag;	
	
	GoalTemp[12]=DailyGoal[4].Goal & 0xFF;
	GoalTemp[13]=(DailyGoal[4].Goal>>8) & 0xFF;
	GoalTemp[14]=DailyGoal[4].MainGoalFlag;	
	
	WRITE_USER_PARAMETER(GoalTemp,DAILY_GOAL_LOC,DAILY_GOAL_LEN);
	SetGoalOverRemind(DailyGoal,false);	
}


void SetGoalOverRemind(DAILY_GOAL* set ,bool DFFlag)
{
	uint32 goal[5];
	goal[0] = gSportDisplay.SportSteps/100;
	goal[1] = gSportDisplay.SportKcal/1000;
	if(SetValue.DistanceFormat == 1)
		goal[2] = (gSportDisplay.SportDistance/1.609)/1000;
	else	
		goal[2] = gSportDisplay.SportDistance/1000;
	goal[3] = 0x00;
	goal[4] = gSportDisplay.SportDuration;
	if(DFFlag)   /*ֻ�����*/
	{
		if(set[2].MainGoalFlag==1 && set[2].Goal>goal[2])
			DailyGoalOverRemindEnable|=(1<<2);
		else
			DailyGoalOverRemindEnable&=~(1<<2);		
	}
	else
	{
		for(uint8 i =0; i<5;i++)
		{
			if(set[i].MainGoalFlag==1 && set[i].Goal>goal[i])
				DailyGoalOverRemindEnable|=(1<<i);
			else
				DailyGoalOverRemindEnable&=~(1<<i);
			
		}
	}
}

uint8 CheckSportGoalOverRemind(void)
{
	// �жϲ���Ŀ���Ƿ���
	if ((DailyGoalOverRemindEnable & (1<<0)) && ((gSportDisplay.SportSteps/100) >= DailyGoal[0].Goal)) {
		DailyGoalOverRemindEnable &= ~(1<<0); // ���첻��������
		return DAILYGOAL_OVER_TYPE_STEP;
	}
	// �жϿ�·��Ŀ���Ƿ���
	if ((DailyGoalOverRemindEnable & (1<<1)) && ((gSportDisplay.SportKcal / 1000) >=  DailyGoal[1].Goal)) {
		DailyGoalOverRemindEnable &= ~(1<<1); // ���첻��������
		return DAILYGOAL_OVER_TYPE_CALO;
	}
	// �жϾ���Ŀ���Ƿ���
	if (DailyGoalOverRemindEnable & (1<<2)) {
		if(SetValue.DistanceFormat == 1)/*Ӣ��*/
		{
			if(((gSportDisplay.SportDistance/1.609)/1000) >= DailyGoal[2].Goal)
			{
				DailyGoalOverRemindEnable &= ~(1<<2); // ���첻��������
				return DAILYGOAL_OVER_TYPE_DIST;
			}
		}
		else  						/*����*/
		{
			if((gSportDisplay.SportDistance/1000) >= DailyGoal[2].Goal)
			{
				DailyGoalOverRemindEnable &= ~(1<<2); // ���첻��������
				return DAILYGOAL_OVER_TYPE_DIST;
			}			
		}
	}
	// �ж��˶�ʱ��Ŀ���Ƿ���
	if ((DailyGoalOverRemindEnable & (1<<4)) && ((gSportDisplay.SportDuration) >=  DailyGoal[4].Goal)) {
		DailyGoalOverRemindEnable &= ~(1<<4); // ���첻��������
		return DAILYGOAL_OVER_TYPE_DURATION;
	}		
	return 0;
}

uint16 GoalOver_Handler(MsgType msg, int iParam, void *pContext)
{
	switch(msg)
	{
		case MSG_HANDLER_BEGIN:
			SportHadlerId.GoaloverId= iParam;
			MotoRun(MOTO_SIT);
			SetSystemSuspendTime(DISPLAY_TIME_10S_FLAG);
			break;

		case MSG_HANDLER_END:
			break;
			
		/*�û�����*/
		case MSG_LEFTDOWN_SHORTKEY:				/*��� ����*/	
		case MSG_LEFTDOWN_DOUBLE_SHORTKEY:		/*��� ˫��*/
		case MSG_LEFTDOWN_LONGKEY:           			/*��� ����*/
		case MSG_RIGHTDOWN_DOUBLE_SHORTKEY:   	/*�Ҽ� ˫��*/	
		case MSG_RIGHTDOWN_LONGKEY:				/*�Ҽ� ����*/	
		case MSG_RIGHTDOWN_LONGKEY_UP:			/*�Ҽ� �����ɿ�*/		
		case MSG_TOUCH_UP_FLICK:					/*��*/
		case MSG_TOUCH_DOWN_FLICK:				/*��*/
		case MSG_TOUCH_DOWN_UP_FLICK:				/*��*/ 
		case MSG_TOUCH_UP_DOWN_FLICK: 			/*��*/
		case MSG_TOUCH_UP_SHORTKEY:				/*���� ����*/  
		case MSG_TOUCH_UP_DOUBLE_SHORTKEY:		/*���� ˫��*/	
		case MSG_TOUCH_LONGKEY:					/*���� ����*/
			break;

		case MSG_RIGHTDOWN_SHORTKEY: 				/*�Ҽ� ����*/	
			StopMotorRunTime();
			UnloadHandler(SportHadlerId.GoaloverId);
			break;	

		case MSG_GOAL_OVER_NOTIFY:
			if(CheckpCurrentHandler(SportHadlerId.GoaloverId))
			{
				MotoRun(MOTO_SIT);
				SetSystemSuspendTime(DISPLAY_TIME_10S_FLAG);
			}
			else
			{
				UnloadHandler(SportHadlerId.GoaloverId);
				SendMsg(msg);
				return RET_MSG_HANDLED;
			}
		case MSG_DISPLAY:
			ShowGoalOver();
			break;

		default:
			return RET_MSG_UNHANDLED;
	}
	return RET_MSG_HANDLED;
}


uint16 Syncing_Handler(MsgType msg, int iParam, void *pContext)
{
	switch(msg)
	{
		case MSG_HANDLER_BEGIN:
			SportHadlerId.SycningId= iParam;
			Syncing_Handler_flag = TRUE;
			break;

		case MSG_HANDLER_END:
			Syncing_Handler_flag =FALSE;
			break;

		case MSG_SYNCING_START:
			break;

		case MSG_SYNCING_STOP:
			if(!STEP_ON_FLAG)//�Ʋ�ֹͣ
			{
				StartStep(); //���Ʋ�
			}
			osal_stop_timerEx(GetAppTaskId(), MSG_OPEN_STEP);
			osal_clear_event(GetAppTaskId(), MSG_OPEN_STEP);		 
			UnloadHandler(SportHadlerId.SycningId);
			break;

		case MSG_DISPLAY:
			{
				ShowSyncingIcon();
			}
			break;

		case MSG_SYNCING_CHECK_CMD:
			if (SyncingStartFlag)
			{
				SyncingStartFlag = false;
				osal_start_timerEx(GetAppTaskId(), MSG_SYNCING_CHECK_CMD, 4000);
			}
			else
				osal_start_timerEx(GetAppTaskId(), MSG_SYNCING_STOP, 100);
			break;

		case MSG_OPEN_STEP:
		case MSG_SYSTEM_SUSPEND:

		/*�û�����*/
		case MSG_LEFTDOWN_SHORTKEY:				/*��� ����*/	
		case MSG_LEFTDOWN_DOUBLE_SHORTKEY:		/*��� ˫��*/
		case MSG_LEFTDOWN_LONGKEY:           			/*��� ����*/
		case MSG_RIGHTDOWN_SHORTKEY: 				/*�Ҽ� ����*/	
		case MSG_RIGHTDOWN_DOUBLE_SHORTKEY:   	/*�Ҽ� ˫��*/	
		case MSG_RIGHTDOWN_LONGKEY:				/*�Ҽ� ����*/	
		case MSG_RIGHTDOWN_LONGKEY_UP:			/*�Ҽ� �����ɿ�*/		
		case MSG_TOUCH_UP_FLICK:					/*��*/
		case MSG_TOUCH_DOWN_FLICK:				/*��*/
		case MSG_TOUCH_DOWN_UP_FLICK:				/*��*/ 
		case MSG_TOUCH_UP_DOWN_FLICK: 			/*��*/
		case MSG_TOUCH_UP_SHORTKEY:				/*���� ����*/  
		case MSG_TOUCH_UP_DOUBLE_SHORTKEY:		/*���� ˫��*/	
		case MSG_TOUCH_LONGKEY:					/*���� ����*/
		break;

		case MSG_FORCE_BACK_HOME:
			break;

		default:
			return RET_MSG_UNHANDLED;
	}
	return RET_MSG_HANDLED;
}



static void UpdateSportUI(uint8 ui_index)
{
	ShowSportOverview(ui_index);
#if 0
	switch(ui_index)
	{
		case 0:
			ShowSportOverview();
			return;
		case 1:
			ShowSteps(gSportDisplay.SportSteps);
			break;

		case 2:
			{
				uint32 metre =gSportDisplay.SportDistance;
				if(SetValue.DistanceFormat==0)                    			//����
				    ShowDistance(metre);
			       else if(SetValue.DistanceFormat==1){				//Ӣ��
				    metre=metre/1.609;
				    ShowDistance(metre);
			       }
			}	
			break;	

		case 3:
			ShowCalor(gSportDisplay.SportKcal);	
			break;
			
		case 4:
			ShowActivityDuration(gSportDisplay.SportDuration);
			break;
			
		default:
			break;
	}
#endif
	if (LCD_ENABLE)
		NeedSetDisplySportData(ui_index);
}

uint8 show_index = 0;
/*һ������:�˶���������ͼHandler*/
uint16 SportOverviewHandler(MsgType msg, int iParam, void *pContext)
{
	switch(msg)
	{
		case MSG_HANDLER_BEGIN:
			SportHadlerId.SportOverviewHandlerId = iParam;
			show_index=0;
			SetSystemSuspendTime(DISPLAY_TIME_10S_FLAG);
			break;

		case MSG_HANDLER_END:
			SportHadlerId.SportOverviewHandlerId = 0;
			break;

/*�û�����*/
		case MSG_LEFTDOWN_SHORTKEY:				/*��� ����*/	
		case MSG_LEFTDOWN_DOUBLE_SHORTKEY:		/*��� ˫��*/
		case MSG_LEFTDOWN_LONGKEY:           			/*��� ����*/	
		case MSG_RIGHTDOWN_DOUBLE_SHORTKEY:   	/*�Ҽ� ˫��*/	
		case MSG_RIGHTDOWN_LONGKEY:				/*�Ҽ� ����*/	
		case MSG_RIGHTDOWN_LONGKEY_UP:			/*�Ҽ� �����ɿ�*/
		case MSG_TOUCH_DOWN_UP_FLICK:				/*��*/
		case MSG_TOUCH_UP_DOWN_FLICK: 			/*��*/ 		
		case MSG_TOUCH_UP_SHORTKEY:				/*���� ����*/  	
		case MSG_TOUCH_UP_DOUBLE_SHORTKEY:		/*���� ˫��*/	
		case MSG_TOUCH_LONGKEY:					/*���� ����*/
			break;
			
		case MSG_RIGHTDOWN_SHORTKEY: 				/*�Ҽ� ����*/	
			UnloadHandler(SportHadlerId.SportOverviewHandlerId );
			//LoadMenuHandler(MENU_FORWARD);
			break;

		case MSG_TOUCH_UP_FLICK:					/*��*/
			if(show_index <1)
			{
				show_index++;	
				SendMsg(MSG_DISPLAY);
			}
			break;
		case MSG_TOUCH_DOWN_FLICK:				/*��*/ 
			if(show_index>0)
			{
				show_index--;
				SendMsg(MSG_DISPLAY);
			}				
			break;
			
		case MSG_REFRESH_STEP:
			if(!CheckpCurrentHandler(SportHadlerId.SportOverviewHandlerId))
				break;
		case MSG_UNIT_CHANGED:
		case MSG_DISPLAY:
			UpdateSportUI(show_index);			
			break;

		default:
			return RET_MSG_UNHANDLED;
	}
	return RET_MSG_HANDLED;
}
