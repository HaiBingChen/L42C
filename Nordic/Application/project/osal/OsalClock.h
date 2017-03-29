#ifndef _OSAL_CLOCK_H_
#define _OSAL_CLOCK_H_



#include "stdio.h"
#include "stdint.h"
#include "Typedef.h"

#ifdef   __cplusplus 
extern   " C "   { 
#endif 
/*********************************************************************
 * TYPEDEFS
 */
#define	IsLeapYear(yr)	(!((yr) % 400) || (((yr) % 100) && !((yr) % 4)))
   
#define	YearLength(yr)	(IsLeapYear(yr) ? 366 : 365)
#define	BEGYEAR	        2000     // UTC started at 00:00:00 January 1, 2000
#define	DAY             86400UL  // 24 hours * 60 minutes * 60 seconds
   
typedef uint32_t   UTCTime;

typedef struct
{
    uint8_t seconds;  // 0-59
    uint8_t minutes;  // 0-59
    uint8_t hour;     // 0-23
    uint8_t day;      // 0-30
    uint8_t month;    // 0-11
    uint16_t year;    // 2000+
} UTCTimeStruct;

typedef struct
{
	uint8_t HR_refresh_first;
	uint8_t HR_mode; 
	uint8_t HR_Run_State;
    uint8_t HR_Value_Val; 	
    uint8_t HR_Value;         
    uint8_t HR_Running;
	uint8_t DHM_Flag;
	uint8_t DHM_Time;
	uint8_t HR_Level;
	uint8_t HR_ALARM; //Low--0x01,High--0x02
} UiHRStruct;

#define HR_ALARM_LOW 	0x00
#define HR_ALARM_HIGH 	0x01


/***********************************************
*Data types
************************************************/
void pS_OsTimeClockUpdate(void);
void pS_OsClockUpdate(pU32 MSec);
void pS_OsHwTimerUpdate(void);
pU32 pS_OsGetSysTick(void);
void pS_OsHwTimerAdjust_1min(void);

#ifdef __cplusplus 
} 
#endif 

#endif
