#ifndef __CODE_2_STR_H__
#define __CODE_2_STR_H__

#include <stdint.h>

#define GENERIC_STATUS_BEGIN		0
#define GENERIC_STATUS_END			99

#define FLASH_DRIVER_STATUS_BEGIN	100
#define FLASH_DRIVER_STATUS_END		199

#define I2C_DRIVER_STATUS_BEGIN		200
#define I2C_DRIVER_STATUS_END		299

#define SPI_DRIVER_STATUS_BEGIN		300
#define SPI_DRIVER_STATUS_END		399

#define BOOTLOADER_STATUS_BEGIN	    10000
#define BOOTLOADER_STATUS_END		10099

#define SBLOADER_STATUS_BEGIN		10100
#define SBLOADER_STATUS_END			10199

#define MEMORY_STATUS_BEGIN			10200
#define MEMORY_STATUS_END			10299

#define PROPERTY_STATUS_BEGIN		10300
#define PROPERTY_STATUS_END			10399

#define HOST_STATUS_BEGIN			50000
#define HOST_STATUS_END				50099

const char *code_2_str(uint32_t status_code);

#endif