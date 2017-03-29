#ifndef _TYPEDEF_H_
#define _TYPEDEF_H_



#include "stdio.h"
#ifdef   __cplusplus 
extern   " C "   { 
#endif 

/***********************************************
*Data types
************************************************/

//#define NULL	0L

#undef FALSE
#undef TRUE
#undef false
#undef true
#define FALSE 0
#define TRUE 1
#define false 0
#define true 1



typedef signed          char 	pS8;
typedef signed short	int     pS16;
typedef signed      int    pS32;
typedef signed     long long    pS64;

typedef unsigned        char    pU8;
typedef unsigned short	int     pU16;
typedef unsigned   	int    pU32;
typedef unsigned    long long   pU64;



typedef signed   char   int8;
typedef unsigned char   uint8;

typedef signed   short  int16;
typedef unsigned short  uint16;

typedef signed   long   int32;
typedef unsigned long   uint32;




#define SUCCESS                   0x00
#define FAILURE                   0x01
#define INVALIDPARAMETER          0x02
#define INVALID_TASK              0x03
#define MSG_BUFFER_NOT_AVAIL      0x04
#define INVALID_MSG_POINTER       0x05
#define INVALID_EVENT_ID          0x06
#define INVALID_INTERRUPT_ID      0x07
#define NO_TIMER_AVAIL            0x08
#define NV_ITEM_UNINIT            0x09
#define NV_OPER_FAILED            0x0A
#define INVALID_MEM_SIZE          0x0B
#define NV_BAD_ITEM_LEN           0x0C




#ifdef __cplusplus 
} 
#endif 

#endif
