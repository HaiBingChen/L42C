

#ifndef _SECONDSTIMER_H
#define _SECONDSTIMER_H

#include "Res.h"
#include "Typedef.h"
#include <stdbool.h>
#include <stdint.h>
#include "nrf.h"
#include "bsp.h"

#include "nrf_drv_timer.h"
#include "bsp.h"
#include "app_error.h"


//**********************************************
#ifdef   __cplusplus 
extern   "C "   { 
#endif 
//**********************************************
#define MAX_SW_mSEC (99ul*60*1000 + 59*1000 + 900)

extern uint32 gStopWatchLastMs;
extern uint32 gStopWatchLastMsSave;
extern uint32 gStopWatchSysTick_start;

void DisplayEnString(const uint8 *pCn,uint8 x,uint8 y);
	
int SecondsTimer_init(void);
void SecondsTimer(uint8 STflag);
void Secondstimer_run(void);
void Secondstimer_stop(void);
void Secondstimer_clear(void);
void timer_enable(void);
void SecondsTimerStart(void);
void SecondsTimerOff(void);
void SecondsTimerStop(void);
//**********************************************
#ifdef __cplusplus 
} 
#endif 
//*
#endif

