#ifndef _OSAL_MEMORY_H_
#define _OSAL_MEMORY_H_



#include "stdio.h"
#include "Typedef.h"

#ifdef   __cplusplus 
extern   " C "   { 
#endif 



#define osal_mem_alloc(size )       pS_OsMalloc(size)
#define osal_mem_free(ptr )          pS_OsFree(ptr)


/***********************************************
*Data types
************************************************/
void osal_memset( void *dest, uint8 value, int len );
pU8 MemoryCompare(void *psmem,void *pdmem,pU32 len);
void * MemoryCopy(void *pdmem,void *psmem,pU32 len);
pS8 * StringCopy(pS8 *dest_str,pS8 *src_str);
pU32 StringLen(const pU8 *pstr);
pU32 WStringLen(pU16 *str);
void *pS_OsMalloc(pU16 mSize);
void pS_OsFree(void *pFreeBuffer);



#ifdef __cplusplus 
} 
#endif 

#endif
