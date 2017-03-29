#ifndef _HEART_RATE_TASK_H_
#define _HEART_RATE_TASK_H_

typedef struct
{
	bool Alarm_en;
	uint8 HeartRateAlaUpper;
	uint8 HeartRateAlaLower;
}HR_ALARM;

#define HR_OPEN_EVENT	(1ul<<0)
#define HR_CLOSE_EVENT	(1ul<<1)
#define HR_GET_HR_VALUE	(1ul<<2)
#define HEARTRATE_CNT_UPLOAD_EVENT (1ul<<3)
#define AUTO_HEARTRATE_CHECK_EVENT (1ul<<4)
#define HR_OP_TOGGLE_EVENT	(1ul<<5)


#define REPEAT_COLLECT_TIME (5ul*60*1000) //ms
#define COLLECT_TIMEOUT (1ul*60*1000)

extern bool isHrReady;
extern bool hrRunning;


void HrOpenHW(void);
void HrCloseHW(void);
void HrGetValueHW(void);
uint8 HrGetValue(void);
bool HrValueValid(void);
void HrInitTask(uint8 task_id);
uint8 GetHrTaskId(void);
bool HrIsRunning(void);
bool HrStarting(void);
uint16 HrEventHandler(uint8 task_id, uint16 events);

void SetHeartRateMemoryFullFlag(bool flag);
void StoreHeartRateDataToFlash(HEARTRATE_DATA_T uData);
bool HeartRateUploadData(uint8 idx);

extern bool isHrAlertChecked;
extern uint32 HeartRate_Store_Logs;
extern uint32 HeartRate_UpLoad_Logs;
extern UploadSportStateT gHeartRateDataUploadState;
extern uint8 AutoHeartRateTime;
extern HR_ALARM hr_alarm;
extern bool  SyncingStartFlag;

extern void HrGetLastValueTime(UTCTimeStruct *t);


#endif //_HEART_RATE_TASK_H_


