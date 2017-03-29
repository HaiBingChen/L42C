#ifndef _OSAL_TASK_EVENT_H_
#define _OSAL_TASK_EVENT_H_

#include "Typedef.h"

//**********************************************
#ifdef   __cplusplus 
extern   " C "   { 
#endif 
//********************************************** 

typedef void (*pTaskInitFn)( pU8 TaskId ); 


typedef pU16 (*pTaskEventHandlerFn)(pU8 TaskId,pU16 TaskEvent);


typedef struct pS_OsTaskREC
{
  struct  pS_OsTaskREC  *next;
  pTaskInitFn            pfnInit;
  pTaskEventHandlerFn    pfnEventProcessor;
  pU16                    taskID;
  pU8                    taskPriority;
  pU16                   events;
	
} pS_OsTaskREC_T;


void  pS_OsTaskInit(void);
void  pS_OsTaskHeadInit(void );
pS_OsTaskREC_T *pS_OsNextActiveTask( void );
pS_OsTaskREC_T *pS_OsFindTask( pU16 TaskId );
void  pS_OsRegTask(pTaskInitFn pfnInit,pTaskEventHandlerFn pfnEventProcessor,pU8 taskPriority);
pU8 pS_OsSetTaskEvent(pU16 TaskId,pU16 EventsFlag);
pU8 pS_OsClearTaskEvent(pU16 TaskId,pU16 EventsFlag);
pU8 osal_set_event( pU16 task_id, uint16 event_flag );
pU8 osal_clear_event( pU16 task_id, uint16 event_flag );



extern pS_OsTaskREC_T   * TaskHead;//the point to link'head
extern pS_OsTaskREC_T   * TaskActive;

//**********************************************
#ifdef __cplusplus 
} 
#endif 
//*
#endif


