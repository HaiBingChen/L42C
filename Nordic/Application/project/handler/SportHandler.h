

#include "Typedef.h"
//#include "Macrodef.h"
#include "AppHandler.h"

typedef struct 
{
	 int		SycningId;  
	 int		GoaloverId;
	 int		SportOverviewHandlerId;
} SportHadlerTAG;


extern bool Syncing_Handler_flag;
extern uint8 show_index;
extern uint8 DailyGoalOverRemindEnable;
extern uint8 DailyGoalOverType; // Ŀ��������:1����,2��·��,3����

uint16 SportOverviewHandler(MsgType msg, int iParam, void *pContext);
uint16 Syncing_Handler(MsgType msg, int iParam, void *pContext);
uint16 GoalOver_Handler(MsgType msg, int iParam, void *pContext);

uint8 CheckSportGoalOverRemind(void);
void SetGoalOverRemind(DAILY_GOAL* set,bool DFFlag);
void SetDefaultGoal(void);





