#ifndef _OSAL_MESSAGE_H_
#define _OSAL_MESSAGE_H_



#include "stdio.h"
#include "Typedef.h"
#include "OsalConfig.h"

#ifdef   __cplusplus 
extern   " C "   { 
#endif 

/***********************************************
*Data types
************************************************/
#define OSAL_MSG_NEXT(pMsg)      ((pS_OsMsgHdr_T *) (pMsg) - 1)->Next
	
#define OSAL_MSG_Q_INIT(pMsgQ)      *(pMsgQ) = NULL
	
#define OSAL_MSG_Q_EMPTY(pMsgQ)     (*(pMsgQ) == NULL)
	
#define OSAL_MSG_Q_HEAD(pMsgQ)      (*(pMsgQ))
	
#define OSAL_MSG_LEN(pMsg)      ((pS_OsMsgHdr_T *) (pMsg) - 1)->Len
	
#define OSAL_MSG_ID(pMsg)      ((pS_OsMsgHdr_T *) (pMsg) - 1)->DestId
	
	/*********************************************************************
	 * CONSTANTS
	 */
#define SYS_EVENT_MSG               0x8000  // A message is waiting event


	/*** Interrupts ***/
#define INTS_ALL    0xFF
	
	/*********************************************************************
	 * TYPEDEFS
	 */
	typedef struct
	{
		void	 *Next;
		pU16 Len;
                pU16  DestId;
	} pS_OsMsgHdr_T;
	
	
	typedef struct
	{
		pU8  Event;
		pU8  Status;
	} pS_OsEventHdr_T;
	
	typedef void * pS_OsMsgQ_T;
	
	/*********************************************************************
	 * GLOBAL VARIABLES
	 */
	
	/*********************************************************************
	 * FUNCTIONS
	 */
	
	/*** Message Management ***/
	
		/*
		 * Task Message Allocation
		 */
		extern pU8 * pS_OsMsgMalloc(pU16 len );
	
		/*
		 * Task Message Deallocation
		 */
		extern pU8 pS_OsMsgDeallocate( pU8 *pMsg );
	
		/*
		 * Send a Task Message
		 */
		extern pU8 pS_OsMsgSend( pU16 DesTask, pU8 *pMsg );
	
		/*
		 * Push a Task Message to head of queue
		 */
		extern pU8 pS_OsMsgPushFront( pU16 DesTask, pU8 *pMsg );
	
		/*
		 * Receive a Task Message
		 */
		extern pU8 *pS_OsMsgReceive( pU16 TaskId );
	
		/*
		 * Find in place a matching Task Message / Event.
		 */
		extern pS_OsEventHdr_T *pS_OsMsgFind(pU16 TaskId, pU8 event);
	
		/*
		 * Enqueue a Task Message
		 */
		extern void pS_OsMsgEnqueue( pS_OsMsgQ_T *pMsgQ, void *pMsg );
	
		/*
		 * Enqueue a Task Message Up to Max
		 */
		extern pU8 pS_OsMsgEnqueueMax( pS_OsMsgQ_T *pMsgQ, void *pMsg, pU8 max );
	
		/*
		 * Dequeue a Task Message
		 */
		extern void *pS_OsMsgDequeue( pS_OsMsgQ_T *pMsgQ );
	
		/*
		 * Push a Task Message to head of queue
		 */
		extern void pS_OsMsgPush( pS_OsMsgQ_T *pMsgQ, void *pMsg );
	
		/*
		 * Extract and remove a Task Message from queue
		 */
		extern void pS_OsMsgExtract( pS_OsMsgQ_T *pMsgQ, void *pMsg, void *prev_ptr );

	extern void pS_OsMsgInit(void);
	

#ifdef __cplusplus 
} 
#endif 

#endif

