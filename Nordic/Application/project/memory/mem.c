
#include "stdint.h"


void *osal_memcpy( void *dst,  void  *src, unsigned int len )
{
    uint8_t *pDst;
    const uint8_t *pSrc;

    pSrc = src;
    pDst = dst;

    while ( len-- )
        *pDst++ = *pSrc++;

    return ( pDst );
}















