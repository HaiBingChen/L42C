/*************************************************/

/**************类型定义头文件********************/

/*************2013年  *********************/

/************ pseudo-system BY STARDREAM********************/


#ifndef _OSAL_TIMER_H_
#define _OSAL_TIMER_H_



#include "stdio.h"
#include "Typedef.h"

#ifdef   __cplusplus 
extern   " C "   { 
#endif 
/*********************************************************************
 * TYPEDEFS
 */
   
   
#define PS_OS_TIMERS_MAX_TIMEOUT 0x28f5c28e /* unit is ms*/
#define TICK_COUNT  1
   



void pS_OsTimerInit(void);
pU32 pS_OsGetSystemClock( void );
pU8 pS_OsStartTimer( pU16 TaskId, pU16 EventId, pU32 TimeoutValue );  
pU8 pS_OsReloadTimer( pU16 TaskId, pU16 EventId, pU32 TimeoutValue );  
pU8 pS_OsStopTimer(  pU16 TaskId, pU16 EventId );
void pS_OsTimerUpdate( pU32 nMSec );//更新事件定时
pU8 osal_start_timerEx( pU16 TaskId, pU16 EventId, pU32 TimeoutValue );
pU8 osal_stop_timerEx(  pU16 TaskId, pU16 EventId );


#ifdef __cplusplus 
} 
#endif 

#endif
