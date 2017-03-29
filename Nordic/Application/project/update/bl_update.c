#include <stdint.h>
#include <string.h>
#include <stdio.h>

#include "headfile.h"

typedef struct
{
    uint32_t   baseaddr;
	uint32_t   size;
	uint32_t   offset;
	uint8_t   *p_buffer;
} firmware_info_t;

#define MIN(a, b)   		((a) < (b) ? (a) : (b))
#define ALIGN_DOWN(x, a)    ((x) & -(a))
#define ALIGN_UP(x, a)      (-(-(x) & -(a)))

firmware_info_t firmware_info;

int32_t do_update(void)
{
	uint32_t retries = 3;
	uint32_t rt_code;
	uint32_t len;
	uint32_t sector_size=512;;
	uint32_t fw_size;
	firmware_info_t *p_fw_info;
	uint8_t bl_version[4];
	uint8_t sector_buf[1024];
	uint8_t buffer[4] = {0};
	
	//init buf
	for(int i=0; i<1024; i++)
		sector_buf[i] = 0x00;

	/* Open file and retrieve info. */
	p_fw_info = &firmware_info;
	if (p_fw_info == NULL)
	{
		goto ERROR_EXIT3; /* goto EXIT3 because .fd and .p_buffer are not ready */
	}

	//firmware size
	READ_OTA_INFO(buffer,OTA_KL17_SIZE_LOC,OTA_KL17_SIZE_LEN);
	fw_size = (buffer[0] << 24) | (buffer[1] << 16) | (buffer[2] << 8) | (buffer[3]); 

	p_fw_info->baseaddr = 0x00000000;
	p_fw_info->offset   = 0x0;
	p_fw_info->p_buffer = NULL;
	p_fw_info->size = fw_size;

	
	/* force device to enter bootloader mode */
	sys_enter_bootloader_mode();
	sys_reset_device();

	/* try to ping bootloader */
	for( ;	retries > 0; retries--)
	{
		rt_code = bl_ping(bl_version);
		if (rt_code == kStatus_Success) 
		{
			break;
			
		}
	}

	/*error exit*/
	if ((rt_code != kStatus_Success))
	{
		goto ERROR_EXIT1;
	}

	
	/* get property */
	//rt_code = bl_getproerty(SECTOR_SIZE, &sector_size);
	//rt_code = bl_getproerty(RESEVED_REGION, &flash_addr);


	p_fw_info->p_buffer = sector_buf;
	if (p_fw_info->p_buffer == NULL) 
	{
		goto ERROR_EXIT1; /* goto EXIT2 because p_fw_info->p_buffer is not ready to support free() */
	}
	
	/* erase memory, size must be 4 or 8 bytes aligned */
	if(p_fw_info->size>0xF000)
	{
		uint32 kl17addr=p_fw_info->baseaddr;
		uint32 kl17size= p_fw_info->size-0xF000;
		rt_code = bl_flash_erase_region(kl17addr, ALIGN_UP((uint32_t)0xF000, (uint32_t)CONFIG_BYTES_ALIGN));
		if (rt_code != kStatus_Success) 
		{
			goto ERROR_EXIT1;
		}	
		rt_code = bl_flash_erase_region(kl17addr+0xF000, ALIGN_UP(kl17size, (uint32_t)CONFIG_BYTES_ALIGN));
		if (rt_code != kStatus_Success) 
		{
			goto ERROR_EXIT1;
		}			
	}
	else
	{
		rt_code = bl_flash_erase_region(p_fw_info->baseaddr, ALIGN_UP(p_fw_info->size, (uint32_t)CONFIG_BYTES_ALIGN));
		if (rt_code != kStatus_Success) 
		{
			goto ERROR_EXIT1;
		}
	}
	/* write memory */
	len = 1024;
	while (p_fw_info->size > 0) 
	{
		len = MIN(sector_size, p_fw_info->size);

		/* assume we can get exact bytes as expect to make things simple */
		SpiFlash_Read(Ota_Kl17_Base + p_fw_info->offset, p_fw_info->p_buffer, len);

		rt_code = bl_write_memory(p_fw_info->baseaddr + p_fw_info->offset, p_fw_info->p_buffer, len);
		if (rt_code != kStatus_Success) 
		{
			goto ERROR_EXIT1;
		}

		p_fw_info->offset += len;
		p_fw_info->size   -= len;
	}

	sys_reset_device();
	return 0; /* success */

ERROR_EXIT1:
ERROR_EXIT3:
	return -1;
}


