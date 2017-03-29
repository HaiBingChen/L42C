#include "Typedef.h"


  


uint16 SleepEventHandler(uint8 task_id, uint16 events);
void SleepInitTask(uint8 task_id);
uint8 GetSleepTaskId(void);
bool UploadSlumberData(void);
void GotoSleepMode(void);
void GotoNormalMode(bool force);
void ClearSleepData(bool force);
void SetSleepMemoryFullFlag(bool flag);
bool SleepUploadData(uint8 idx);
void SleepDisplayData(void);
void ClearSleepDataWhenDateTimeSetting(uint32 times);
void InitSleepDetailForDisplay(void);
void GotoNormalModeWhenBond(void);	
extern uint32 Sleep_UpLoad_Logs;		
extern uint32 Sleep_Store_Logs;		

extern uint16 MinuteCount;        	/*统计睡眠时候的分钟数*/
extern uint8   SecondCount;        	/*统计睡眠时候的秒数*/

extern UploadSportStateT gSleepDataUploadState;


#define SLEEP_ALG_RUNNING_EVENT      0x0001
#define SLEEP_CNT_UPLOAD_EVENT       0x0002
#define SLEEP_ALG_RUN_SLEEP_VALID_EVENT    0x0004
#define SLEEP_STORE_DATA_EVENT    0x0008
#define SLEEP_BUTTON_PRESS_EVENT    0x0010
#define SLEEP_REFRESH_SLEEPDATA_EVENT    0x0020










