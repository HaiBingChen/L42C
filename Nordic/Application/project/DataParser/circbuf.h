/*
 *  circbuf.h
 *
 *  General circual buffer
 *
 *  Author:  Bell
 *  Created:   Apr. 17th 2014
 *  Copyright: Appscomm Inc.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 2 as
 *  published by the Free Software Foundation.
 */

#ifndef _CIRC_BUFFER_HEADER
#define _CIRC_BUFFER_HEADER

#include "Typedef.h"

typedef struct tag_circ_buf 
{
	uint8 *buf;
	uint32 write;
	uint32 read;
	uint32 size; /* Warning: the value of size must be set to [2^n] */
}circ_buf;

/* Initialize the circ buffer by pointer and len.
 *  Warning: the len argument  must be set to [2^n]. 
 */
#define CIRC_BUF_INIT(circ, pointer, len) \
	{( circ )->write = ( circ )->read = 0; \
	(circ)->buf=(pointer); \
	(circ)->size=(len);}

/* Return count in buffer.  */
#define CIRC_BUF_CNT(circ) \
	((( circ )->write>=( circ )->read)?(( circ )->write - ( circ )->read):(( circ )->size+( circ )->write - ( circ )->read))

/* Return space available, 0..size-1.  We always leave one free char
   as a completely full buffer has write == read, which is the same as
   empty.  */
#define CIRC_BUF_SPACE(circ) \
	((( circ )->read>( circ )->write)?(( circ )->read - ( circ )->write-1):(( circ )->size-1+( circ )->read - ( circ )->write))

/* Return count up to the end of the buffer.  Carefully avoid
   accessing write and read more than once, so they can change
   underneath us without returning inconsistent results.  */
#define CIRC_BUF_CNT_TO_END(circ) \
	((( circ )->write>=( circ )->read)?(( circ )->write-( circ )->read):((( circ )->size)-( circ )->read))

/* Return space available up to the end of the buffer.  */
#define CIRC_BUF_SPACE_TO_END(circ) \
	((( circ )->read>( circ )->write)?(( circ )->read-( circ )->write-1):((( circ )->size-1)-( circ )->write+((( circ )->read>0)?1:0)))

/* Return 1 if the circ buffer is empty, otherwise return 0 */
#define CIRC_BUF_EMPTY(circ)   ( ( circ )->write == ( circ )->read )

#define CIRC_BUF_NEXT_WRITE(circ)   (( ( ( circ )->write + 1 ) >= (circ)->size )? ( ( circ )->write + 1 -(circ)->size):(( circ )->write + 1))

/* Return 1 if the circ buffer is full, otherwise return 0 */
//#define CIRC_BUF_FULL(circ)      ( ( circ )->read ==( ( ( circ )->write + 1 ) % (circ)->size ) )
#define CIRC_BUF_FULL(circ)      ( ( circ )->read ==CIRC_BUF_NEXT_WRITE(circ))

/* Clear the circ buffer only setting the write position and read position to zero, but not clear the content of the buffer */
#define CIRC_BUF_CLEAR(circ)            ( ( circ )->write = ( circ )->read = 0)

/*
 * circ_buf_read
 *
 * Get data from the circular buffer and copy to the given buffer.
 * Restrict to the amount of data available.
 *
 * Return the number of bytes really readed.
 * The number of bytes really readed may be less than the amount requested in the count argument to the method 
 * if the amount of data available is less than the amount requested.
 */
extern int32 circ_buf_read(circ_buf *cb, uint8 *buf, int32 count);

/*         
 * circ_buf_write
 *
 * Copy data from a user buffer and put it into the circular buffer.
 * Restrict to the amount of space available.
 *
 * Return the number of bytes written.
 * The data may be only partially written if there isn't room in the buffer for the count bytes that were requested.
 */
extern int32 circ_buf_write(circ_buf *cb, const uint8 *buf, int32 count);

/*
 * circ_buf_read_char
 *
 * Read a char from the circular buffer and copy it to the given buffer.
 * Restrict to the amount of data available.
 *
 * Return 1 if success, otherwise return 0.
 */
extern int8 circ_buf_read_char(circ_buf *cb, uint8 *buf);

/*         
 * circ_buf_write_char
 *
 * Copy a char from a user buffer and put it into the circular buffer.
 * Restrict to the amount of space available.
 *
 * Return 1 if success, otherwise return 0.
 */
extern int8 circ_buf_write_char(circ_buf *cb, const uint8 buf);


/*
 * circ_buf_query
 * 
 * Query data offseting certain bytes from the circular buffer and copy to the given buffer.
 * Restrict to the amount of data available.
 * 
 * Return the number of bytes really queried.
 * The number of bytes really queried may be less than the amount requested in the count argument to the method 
 * if the amount of data available is less than the amount requested.
 */
extern int32 circ_buf_query(circ_buf *cb, uint8 *buf, int32 count, uint32 offset);

/*
 * circ_buf_query_char
 *  
 * Query a char offseting certain bytes from the circular buffer and copy to the given buffer.
 * Restrict to the amount of data available.
 *
 * Return 1 if success, otherwise return 0. 
 */
extern int8 circ_buf_query_char(circ_buf *cb, uint8 *buf, uint32 offset);

extern int32 circ_buf_skip_all(circ_buf *cb);

extern int32 circ_buf_skip(circ_buf *cb, int32 count);

extern int8 circ_buf_skip_char(circ_buf *cb);

#endif
