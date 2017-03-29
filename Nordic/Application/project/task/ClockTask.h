
#include "Typedef.h"
#include "OsalClock.h"


 
#define BEGYEAR_STAMP 946656000UL//2000,1,1, 0,0 Ê±¼ä´Á

#define CLOCK_EVENT                                                       	0x0001
#define CLOCK_SUSPEND_TIMEOUT_EVENT		     		0x0002
#define CLOCK_1S_DISPLAY_EVENT		                     0x0004
#define CLOCK_NUS_DATA_PARSE						0x0008
#define CLOCK_MEDIA_DATA_PARSE					0x0010
#define STOP_WATCH_CNT_EVENT                    0x0020

#define CLOCK_EVENT_PERIOD  60000





extern uint8 CountForCloseBLE;
extern UTCTimeStruct gCurrentUtcTime;



uint8 GetClockTaskId(void);
int GetMonthWeekDays(int y,int m,int z);
int GetMonthDays(int y,int m);
void ClockInitTask(uint8 task_id);
uint16 ClockEventHandler(uint8 task_id, uint16 events);
void ClockEvtInit(void);
uint32 GetTimeStamp(void);
UTCTime pS_OsGetUTCSecs( UTCTimeStruct *tm );
void StartSecondEvent(void);
void StopSecondEvent(void);
bool GetNoSameDay(UTCTimeStruct uTime);
bool GetNoSameTime(UTCTimeStruct uTime);
void gOS_SetUTCTime( UTCTimeStruct *p_newTime );









