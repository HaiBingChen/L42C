/*
 *  circbuf.c
 *
 *  Generic circual buffer
 *
 *  Author:  Bell
 *  Created:   Apr. 17th 2014
 *  Copyright: Appscomm Inc.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 2 as
 *  published by the Free Software Foundation.
 */

#include "headfile.h"

/*         
 * circ_buf_write
 *
 * Copy data from a user buffer and put it into the circular buffer.
 * Restrict to the amount of space available.
 *
 * Return the number of bytes written.
 * The data may be only partially written if there isn't room in the buffer for the count bytes that were requested.
 *
 */
int32 circ_buf_write(circ_buf *cb, const uint8 *buf, int32 count)
{
	int32 c, ret = 0;
	
	while (1) {
		c = CIRC_BUF_SPACE_TO_END(cb);
		if (count < c)
		    c = count;
		
		if (c <= 0)
		    break;		
	
		memcpy(cb->buf + cb->write, buf, c);
		
		cb->write+=c;
		if(cb->write>=cb->size)
			cb->write-=cb->size;

		buf += c;
		count -= c;
		ret+= c;
	}
	return ret;
}

/*
 * circ_buf_read
 *
 * Get data from the circular buffer and copy to the given buffer.
 * Restrict to the amount of data available.
 *
 * Return the number of bytes really readed.
 * The number of bytes really readed may be less than the amount requested in the count argument to the method 
 * if the amount of data available is less than the amount requested.
 *
 */
int32 circ_buf_read(circ_buf *cb, uint8 *buf, int32 count)
{
	int32 c, ret = 0;
	
	while (1) {
		c = CIRC_BUF_CNT_TO_END(cb);
		if (count < c)
			c = count;
		
		if (c <= 0)
			break;
		
		memcpy(buf, cb->buf + cb->read, c);
		
		cb->read+=c;
		if(cb->read>=cb->size)
			cb->read-=cb->size;
		
		buf += c;
		count -= c;
		ret+= c;
	}
	return ret;
}

/*         
 * circ_buf_write_char
 *
 * Copy a char from a user buffer and put it into the circular buffer.
 * Restrict to the amount of space available.
 *
 * Return 1 if success, otherwise return 0.
 */
int8 circ_buf_write_char(circ_buf *cb, const uint8 buf)
{
	if(!CIRC_BUF_FULL(cb)) {
		cb->buf[ cb->write]=buf;
		
		cb->write++;
		if(cb->write>=cb->size)
			cb->write-=cb->size;
		
		return 1;
	} 
	
	return 0;
}

/*
 * circ_buf_read_char
 *
 * Read a char from the circular buffer and copy it to the given buffer.
 * Restrict to the amount of data available.
 *
 * Return 1 if success, otherwise return 0.
 */
int8 circ_buf_read_char(circ_buf *cb, uint8 *buf)
{
	if(!CIRC_BUF_EMPTY(cb))	{		
		*buf = cb->buf[cb->read];
		
		cb->read++;
		if(cb->read>=cb->size)
			cb->read-=cb->size;
		
		return 1;
	}
	return 0;
}


int32 circ_buf_query(circ_buf *cb, uint8 *buf, int32 count, uint32 offset)
{
	int32 c, ret = 0;
	uint32 cur_read=cb->read;

	c = CIRC_BUF_CNT(cb);

	if (offset>=c) {
		ret=0;
	} else {
		cur_read+=offset;
		if(cur_read>=cb->size)
			cur_read-=cb->size;

		while(1) {
			c=((cb->write>=cur_read)?(cb->write-cur_read):((cb->size)-cur_read));
			if (count < c)
				c = count;

			if (c <= 0)
				break;

			memcpy(buf, cb->buf + cur_read, c);

			cur_read+=c;

			if(cur_read>=cb->size)
				cur_read-=cb->size;
			
			buf += c;
			count -= c;
			ret += c;
		}
	}

	return ret;
}


int8 circ_buf_query_char(circ_buf *cb, uint8 *buf, uint32 offset)
{
	int32 c = 0;
	int8 ret = 0;
	uint32 cur_read=cb->read;

	c = CIRC_BUF_CNT(cb);

	if (offset>=c) {
		ret=0;
	} else {
		cur_read+=offset;
		if(cur_read>=cb->size)
			cur_read-=cb->size;

		*buf = cb->buf[cur_read];
		ret = 1;
	}

	return ret;
}

int32 circ_buf_skip(circ_buf *cb, int32 count)
{
	int32 c, ret = 0;
	
	while (1) {
		c = CIRC_BUF_CNT_TO_END(cb);
		if (count < c)
			c = count;
		
		if (c <= 0)
			break;
		
		//memcpy(buf, cb->buf + cb->read, c);
		
		//cb->read = (cb->read + c) & (cb->size-1);
		//cb->read=(cb->read+c)%(cb->size);
		///*
		cb->read+=c;
		if(cb->read>=cb->size)
			cb->read-=cb->size;
		//*/
		
		//buf += c;
		count -= c;
		ret+= c;
	}

	return ret;
}

int8 circ_buf_skip_char(circ_buf *cb)
{
	int8 ret=0;
	
	if(!CIRC_BUF_EMPTY(cb))	{		
		//*buf = cb->buf[cb->read];
		
		//cb->read = (cb->read + 1) & (cb->size-1);
		//cb->read=(cb->read+1)%(cb->size);
		///*
		cb->read++;
		if(cb->read==cb->size)
			cb->read-=cb->size;
		//*/
		
		ret = 1;
	}

	return ret;
}

int32 circ_buf_skip_all(circ_buf *cb)
{
	int32 c, ret = 0;
	
	while (1) {
		c = CIRC_BUF_CNT_TO_END(cb);	
		
		if (c <= 0)
			break;
		
		//memcpy(buf, cb->buf + cb->read, c);
		
		//cb->read = (cb->read + c) & (cb->size-1);
		//cb->read=(cb->read+c)%(cb->size);
		///*
		cb->read+=c;
		if(cb->read>=cb->size)
			cb->read-=cb->size;
		//*/
		
		//buf += c;
		//count -= c;
		ret+= c;
	}
	
	return ret;
}


