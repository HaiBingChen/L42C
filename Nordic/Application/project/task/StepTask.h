

#include "Typedef.h"

  
#define STEP_EVENT      0x0001
#define STEP_CNT_EVENT  0x0002
#define STEP_RECORD_UPLOAD_EVENT  0x0004
#define RECORDWORK_UPLOAD_EVENT  0x0008
 
#define STEP_EVENT_PERIOD       (10000/RTC_TIMEOUT) 
#define STEP_CNT_EVENT_PERIOD   (20/RTC_TIMEOUT) 



#define DAILYGOAL_OVER_TYPE_STEP                    1 // 步数目标达成
#define DAILYGOAL_OVER_TYPE_CALO                    2 // 卡路里目标达
#define DAILYGOAL_OVER_TYPE_DIST                    3 // 距离目标达成
#define DAILYGOAL_OVER_TYPE_SLEEP                  4 // 睡眠目标达成
#define DAILYGOAL_OVER_TYPE_DURATION           5 // 运动时长目标达成 

typedef enum{
  UPLOAD_NO=0,
  UPLOAD_PREPARE,
  UPLOADING,
  UPLOAD_OK,
  UPLOAD_FAIL,
}UploadSportStateT;


void StepInit(void); 
void StepInitTask(uint8 task_id);
uint16 StepEventHandler(uint8 task_id, uint16 events);
uint8 GetStepTaskId(void);
void StopStep(void);
void StartStep(void);
void CloseStep(void) ;   
uint32 GetSportKcal(uint32 Steps);
uint32 CalBaseKcal(uint32 min);
void ClearSportData(void);
void SetStepMemoryFullFlag(bool flag);
uint32 GetDistance(uint32 Steps);
bool StepUploadData(uint8 idx);
void CleanAutoSleepFlag(void);
void SetAutoSleepTime(uint16 BeginTime,uint16 EndTime);
void StoreSportDataToFlash(uint8 flag);
bool RecordWorkUploadData(uint8 idx);
void InitGesture(void);

extern UploadSportStateT gUploadSportDataState;
extern uint32 Step_UpLoad_Logs;		
extern uint32 Step_Store_Logs;		
extern uint16 AutoSleepCount;
extern bool AutoEnterSleep;
extern uint8 Base_Kcal_type;


extern UploadSportStateT gUploadRecordWorkState;
extern uint32 RecordWork_UpLoad_Logs;		
extern uint32 RecordWork_Store_Logs;		

