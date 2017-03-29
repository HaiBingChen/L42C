#ifndef _OSAL_H_
#define _OSAL_H_

#include "Typedef.h"
#include "OsalMemory.h"
#include "OsalClock.h"
#include "Osal.h"
#include "OsalTaskEvent.h"
#include "OsalTimer.h"
#include "OsalMessage.h"
#include "OsalPort.h"

//**********************************************
#ifdef   __cplusplus 
extern   " C "   { 
#endif 
//********************************************** 

void pS_OsInitSystem(void);
void pS_OsRunSystem(void);
//**********************************************
#ifdef __cplusplus 
} 
#endif 
//*
#endif

