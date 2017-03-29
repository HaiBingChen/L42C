
#include "Typedef.h"
#include "AppHandler.h"






typedef struct 
{
	int				SleepStartId;          
	int				ExitSleepId;	
	int				SleepMonitorId;
	int				SleepChangeId;
	int				SleepOverviewId;
	int				SleepDetailId;
} SleepHadlerTAG;

typedef struct 
{
	uint8 state;   //0: 正常显示；1:高亮显示
	uint8 hour;
	uint8 minute;	
	uint8 date;
	uint8 month;
	uint16 year;
}sleep_overview_t;

typedef struct 
{
	uint16 total_sleep;
	uint16 fell_sleep;
	uint16 light_sleep;
	uint16 deep_sleep;
	uint16 awake_sleep;
	uint16 awake;
}sleep_info_t;


extern uint8 SleepKey;
uint16 SleepStart_Handler(MsgType msg, int iParam, void *pContext);
uint16 SleepMonitor_Handler(MsgType msg, int iParam, void *pContext);
void SetSleepGoalOverRemind(void);
uint16 SleepChange_Handler(MsgType msg, int iParam, void *pContext);
uint8 CheckSleepGoalOverRemind(void);
uint16 ExitSleep_Handler(MsgType msg, int iParam, void *pContext);

