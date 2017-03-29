#include "Osal.h"
#include "OsalMessage.h"
#include "OsalMemory.h"
#include "OsalClock.h"
#include "OsalTaskEvent.h"
#include "OsalTimer.h"
#include "OsalPort.h"











#define TASK_NO_TASK      0xFFFF
// Message Pool Definitions
pS_OsMsgQ_T pS_Os_qHead;

pU16 TasksCnt=0;




/*********************************************************************
 * API FUNCTIONS
 *********************************************************************/
 /******************************************************************************************
*函数名:		pS_OsMsgInit
*参数:			无
*返回值:		无
*描述:			系统消息初始化
*编写:			stardream
*版本信息:		V1.0			2013年
******************************************************************************************/
void pS_OsMsgInit(void)
{
	extern pU16 TaskId;
	
	pS_Os_qHead = NULL;
	TasksCnt=TaskId+1;
}
/******************************************************************************************
*函数名:		pS_OsMsgMalloc
*参数:			无
*返回值:		无
*描述:			创建消息内存
*编写:			stardream
*版本信息:		V1.0			2013年
******************************************************************************************/

pU8 * pS_OsMsgMalloc( pU16 Len )
{
  pS_OsMsgHdr_T *pHdr;

  if ( Len == 0 )
    return ( NULL );
//  pHdr =(pS_OsMsgHdr_T*)malloc( (short)(Len + sizeof( pS_OsMsgHdr_T )) );
  pHdr = (pS_OsMsgHdr_T *) pS_OsMalloc( (short)(Len + sizeof( pS_OsMsgHdr_T )) );
  if ( pHdr )
  {
    pHdr->Next = NULL;
    pHdr->Len = Len;
    pHdr->DestId = TASK_NO_TASK;
    return ( (pU8 *) (pHdr + 1) );
  }
  else
    return ( NULL );
}
/******************************************************************************************
*函数名:		pS_OsMsgDeallocate
*参数:			无
*返回值:		无
*描述:			删除消息
*编写:			stardream
*版本信息:		V1.0			2013年
******************************************************************************************/

pU8 pS_OsMsgDeallocate( pU8 *pMsg )
{
  pU8 *pX;

  if ( pMsg == NULL )
    return ( INVALID_MSG_POINTER );

  // don't deallocate queued buffer
  if ( OSAL_MSG_ID( pMsg ) != TASK_NO_TASK )
    return ( MSG_BUFFER_NOT_AVAIL );

  pX = (pU8 *)((pU8 *)pMsg - sizeof( pS_OsMsgHdr_T ));

  pS_OsFree( (void *)pX );

  return ( SUCCESS );
}

/******************************************************************************************
*函数名:		pS_OsMsgEnqueuePush
*参数:			无
*返回值:		无
*描述:			
*编写:			stardream
*版本信息:		V1.0			2013年
******************************************************************************************/

static pU8 pS_OsMsgEnqueuePush( pU16 DesTaskId, pU8 *pMsg, pU8 Push )
{
  if ( pMsg == NULL )
  {
    return ( INVALID_MSG_POINTER );
  }

  if ( DesTaskId >= TasksCnt )
  {
    pS_OsMsgDeallocate( pMsg );
    return ( INVALID_TASK );
  }

  // Check the message header
  if ( OSAL_MSG_NEXT( pMsg ) != NULL ||
       OSAL_MSG_ID( pMsg ) != TASK_NO_TASK )
  {
    pS_OsMsgDeallocate( pMsg );
    return ( INVALID_MSG_POINTER );
  }

  OSAL_MSG_ID( pMsg ) = DesTaskId;

  if ( Push == TRUE )
  {
    // prepend the message
    pS_OsMsgPush( &pS_Os_qHead, pMsg );
  }
  else
  {
    // append the message
    pS_OsMsgEnqueue( &pS_Os_qHead, pMsg );
  }

  // Signal the task that a message is waiting
  pS_OsSetTaskEvent( DesTaskId, SYS_EVENT_MSG );

  return ( SUCCESS );
}
/******************************************************************************************
*函数名:		pS_OsMsgPushFront
*参数:			无
*返回值:		无
*描述:			
*编写:			stardream
*版本信息:		V1.0			2013年
******************************************************************************************/

pU8 pS_OsMsgPushFront( pU16 DesTaskId, pU8 *pMsg )
{
  return ( pS_OsMsgEnqueuePush( DesTaskId, pMsg, TRUE ) );
}
/******************************************************************************************
*函数名:		pS_OsMsgSend
*参数:			无
*返回值:		无
*描述:			发送消息
*编写:			stardream
*版本信息:		V1.0			2013年
******************************************************************************************/

pU8 pS_OsMsgSend( pU16 DesTaskId, pU8 *pMsg )
{
  return ( pS_OsMsgEnqueuePush( DesTaskId, pMsg, FALSE ) );
}

/******************************************************************************************
*函数名:		pS_OsMsgReceive
*参数:			无
*返回值:		无
*描述:			接收到消息
*编写:			stardream
*版本信息:		V1.0			2013年
******************************************************************************************/


pU8 *pS_OsMsgReceive( pU16 TaskId )
{
  pS_OsMsgHdr_T *pListHdr;
  pS_OsMsgHdr_T *pPrevHdr = NULL;
  pS_OsMsgHdr_T *pFoundHdr = NULL;


  // Hold off interrupts
  IRQ_DISABLE_SAVE();

  // Point to the top of the queue
  pListHdr = pS_Os_qHead;

  // Look through the queue for a message that belongs to the asking task
  while ( pListHdr != NULL )
  {
    if ( (pListHdr - 1)->DestId == TaskId )
    {
      if ( pFoundHdr == NULL )
      {
        // Save the first one
        pFoundHdr = pListHdr;
      }
      else
      {
        // Second msg found, stop looking
        break;
      }
    }
    if ( pFoundHdr == NULL )
    {
      pPrevHdr = pListHdr;
    }
    pListHdr = OSAL_MSG_NEXT( pListHdr );
  }

  // Is there more than one?
  if ( pListHdr != NULL )
  {
    // Yes, Signal the task that a message is waiting
    pS_OsSetTaskEvent( TaskId, SYS_EVENT_MSG );
  }
  else
  {
    // No more
    pS_OsClearTaskEvent( TaskId, SYS_EVENT_MSG );
  }

  // Did we find a message?
  if ( pFoundHdr != NULL )
  {
    // Take out of the link List
    pS_OsMsgExtract( &pS_Os_qHead, pFoundHdr, pPrevHdr );
  }

  // Release interrupts
  IRQ_ENABLE_RESTORE();

  return ( (pU8*) pFoundHdr );
}

/******************************************************************************************
*函数名:		pS_OsMsgFind
*参数:			无
*返回值:		无
*描述:			找消息
*编写:			stardream
*版本信息:		V1.0			2013年
******************************************************************************************/

pS_OsEventHdr_T *pS_OsMsgFind(pU16 TaskId, pU8 Event)
{
  pS_OsMsgHdr_T *pHdr;

  IRQ_DISABLE_SAVE();  // Hold off interrupts.

  pHdr = pS_Os_qHead;  // Point to the top of the queue.

  // Look through the queue for a message that matches the TaskId and Event parameters.
  while (pHdr != NULL)
  {
    if (((pHdr-1)->DestId == TaskId) && (((pS_OsEventHdr_T *)pHdr)->Event == Event))
    {
      break;
    }

    pHdr = OSAL_MSG_NEXT(pHdr);
  }

  IRQ_ENABLE_RESTORE();  // Release interrupts.

  return (pS_OsEventHdr_T *)pHdr;
}

/******************************************************************************************
*函数名:		pS_OsMsgEnqueue
*参数:			无
*返回值:		无
*描述:			进队列
*编写:			stardream
*版本信息:		V1.0			2013年
******************************************************************************************/

void pS_OsMsgEnqueue( pS_OsMsgQ_T *pMsgQ, void *pMsg )
{
  void *List;


  // Hold off interrupts
  IRQ_DISABLE_SAVE();

  OSAL_MSG_NEXT( pMsg ) = NULL;
  // If first message in queue
  if ( *pMsgQ == NULL )
  {
    *pMsgQ = pMsg;
  }
  else
  {
    // Find end of queue
    for ( List = *pMsgQ; OSAL_MSG_NEXT( List ) != NULL; List = OSAL_MSG_NEXT( List ) );

    // Add message to end of queue
    OSAL_MSG_NEXT( List ) = pMsg;
  }

  // Re-enable interrupts
  IRQ_ENABLE_RESTORE();
}

/******************************************************************************************
*函数名:		pS_OsMsgDequeue
*参数:			无
*返回值:		无
*描述:			出队列
*编写:			stardream
*版本信息:		V1.0			2013年
******************************************************************************************/

void *pS_OsMsgDequeue( pS_OsMsgQ_T *pMsgQ )
{
  void *pMsg = NULL;

  // Hold off interrupts
  IRQ_DISABLE_SAVE();

  if ( *pMsgQ != NULL )
  {
    // Dequeue message
    pMsg = *pMsgQ;
    *pMsgQ = OSAL_MSG_NEXT( pMsg );
    OSAL_MSG_NEXT( pMsg ) = NULL;
    OSAL_MSG_ID( pMsg ) = TASK_NO_TASK;
  }

  // Re-enable interrupts
  IRQ_ENABLE_RESTORE();

  return pMsg;
}

/******************************************************************************************
*函数名:		pS_OsMsgPush
*参数:			无
*返回值:		无
*描述:			push消息
*编写:			stardream
*版本信息:		V1.0			2013年
******************************************************************************************/

void pS_OsMsgPush( pS_OsMsgQ_T *pMsgQ, void *pMsg )
{

  // Hold off interrupts
  IRQ_DISABLE_SAVE();

  // Push message to head of queue
  OSAL_MSG_NEXT( pMsg ) = *pMsgQ;
  *pMsgQ = pMsg;

  // Re-enable interrupts
  IRQ_ENABLE_RESTORE();
}

/******************************************************************************************
*函数名:		pS_OsMsgExtract
*参数:			无
*返回值:		无
*描述:			
*编写:			stardream
*版本信息:		V1.0			2013年
******************************************************************************************/

void pS_OsMsgExtract( pS_OsMsgQ_T *pMsgQ, void *pMsg, void *pPrev )
{

  // Hold off interrupts
  IRQ_DISABLE_SAVE();

  if ( pMsg == *pMsgQ )
  {
    // remove from first
    *pMsgQ = OSAL_MSG_NEXT( pMsg );
  }
  else
  {
    // remove from middle
    OSAL_MSG_NEXT( pPrev ) = OSAL_MSG_NEXT( pMsg );
  }
  OSAL_MSG_NEXT( pMsg ) = NULL;
  OSAL_MSG_ID( pMsg ) = TASK_NO_TASK;

  // Re-enable interrupts
  IRQ_ENABLE_RESTORE();
}

/******************************************************************************************
*函数名:		pS_OsMsgEnqueueMax
*参数:			无
*返回值:		无
*描述:			
*编写:			stardream
*版本信息:		V1.0			2013年
******************************************************************************************/

pU8 pS_OsMsgEnqueueMax( pS_OsMsgQ_T *pMsgQ, void *pMsg, pU8 Max )
{
  void *List;
  pU8 bReturn = FALSE;

  // Hold off interrupts
  IRQ_DISABLE_SAVE();

  // If first message in queue
  if ( *pMsgQ == NULL )
  {
    *pMsgQ = pMsg;
    bReturn = TRUE;
  }
  else
  {
    // Find end of queue or Max
    List = *pMsgQ;
    Max--;
    while ( (OSAL_MSG_NEXT( List ) != NULL) && (Max > 0) )
    {
      List = OSAL_MSG_NEXT( List );
      Max--;
    }

    // Add message to end of queue if Max not reached
    if ( Max != 0 )
    {
      OSAL_MSG_NEXT( List ) = pMsg;
      bReturn = TRUE;
    }
  }

  // Re-enable interrupts
  IRQ_ENABLE_RESTORE();

  return bReturn;
}


