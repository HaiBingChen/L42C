#ifndef _OSAL_PORT_H_
#define _OSAL_PORT_H_

#include "Typedef.h"
#include "app_util.h"
#include "app_util_platform.h"

//**********************************************
#ifdef   __cplusplus 
extern   " C "   { 
#endif 
//********************************************** 


#define  pOS_ASSERT(CON)    if (!(CON)) { \
								volatile pU8 dummy = 0; \
								while (dummy == 0); \
							}

#define IRQ_DISABLE_SAVE()          CRITICAL_REGION_ENTER()
#define IRQ_ENABLE_RESTORE()     CRITICAL_REGION_EXIT()
#define pOS_DisableSche()          CRITICAL_REGION_ENTER()
#define pOS_EnableSche()     CRITICAL_REGION_EXIT()



void pS_OsWatchDogFeed(void);

//**********************************************
#ifdef __cplusplus 
} 
#endif 
//*
#endif

