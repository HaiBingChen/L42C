#include "Typedef.h"
#include "OsalClock.h"

extern pU32 gPs_SysTicks;

void  Clock_init(void);
void Clock_start(void);
void ResetRtcClock(void);
void osal_setClock( UTCTime newTime );
UTCTime osal_getClock( void );
void gOS_SetUTCTime( UTCTimeStruct *p_newTime );
void gOS_GetUTCTime(UTCTimeStruct * p_store);
UTCTime osal_ConvertUTCSecs( UTCTimeStruct *tm );
void osal_ConvertUTCTime( UTCTimeStruct *tm, UTCTime secTime );
void Clock_second_handle(void *p_context);
void SetFlashRtcClock(UTCTimeStruct* Ut);

