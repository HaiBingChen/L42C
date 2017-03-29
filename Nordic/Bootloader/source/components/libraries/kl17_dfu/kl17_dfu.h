#ifndef __KL17_DFU_H
#define __KL17_DFU_H

#include <stdint.h>
#include <string.h>
#include "nrf_gpio.h"
#include "ble_dfu.h"

#define UserData_Base						0x3F6000UL
/* ** SPI PIN CONFIGURATION***/
#define FLASH_CS_PIN			25 		
#define FLASH_CLK_PIN 			8		
#define FLASH_DI_PIN 			17		
#define FLASH_DO_PIN 			16	
/* ** SPI PIN CONFIGURATION END***/
#define FLASH_CS_OUT		nrf_gpio_pin_dir_set(FLASH_CS_PIN, NRF_GPIO_PIN_DIR_OUTPUT)
#define FLASH_CLK_OUT		nrf_gpio_pin_dir_set(FLASH_CLK_PIN, NRF_GPIO_PIN_DIR_OUTPUT)
#define FLASH_DI_OUT		nrf_gpio_pin_dir_set(FLASH_DI_PIN, NRF_GPIO_PIN_DIR_OUTPUT)
#define FLASH_DO_OUT		nrf_gpio_pin_dir_set(FLASH_DO_PIN, NRF_GPIO_PIN_DIR_OUTPUT)
#define FLASH_DO_IN		       nrf_gpio_pin_dir_set(FLASH_DO_PIN, NRF_GPIO_PIN_DIR_INPUT)

#define FLASH_CS_H			nrf_gpio_pin_set(FLASH_CS_PIN)
#define FLASH_CS_L			nrf_gpio_pin_clear(FLASH_CS_PIN)

#define FLASH_CLK_H			nrf_gpio_pin_set(FLASH_CLK_PIN)
#define FLASH_CLK_L			nrf_gpio_pin_clear(FLASH_CLK_PIN)

#define FLASH_DI_H			nrf_gpio_pin_set(FLASH_DI_PIN)
#define FLASH_DI_L			nrf_gpio_pin_clear(FLASH_DI_PIN)

#define FLASH_DO_H			nrf_gpio_pin_set(FLASH_DO_PIN)
#define FLASH_DO_L			nrf_gpio_pin_clear(FLASH_DO_PIN)
#define FLASH_DO_DATA	       nrf_gpio_pin_read(FLASH_DO_PIN)

#define EFLASH_PAGE_SIZE		256
#define SPI_FLASH_BUSY		       0x01

#define E_JEDEC		0x9f
#define E_RDSR		0x05
#define E_WREN		0x06
#define E_WRDI		0x04
#define E_ULBPR		0x98
#define E_CE			0xc7
#define E_SE			0x20
#define E_BE			0xd8
#define E_PP			0x02
#define E_READ		0x03
#define E_DPD		0xb9
#define E_RDPD		0xab

typedef enum{
    O_PROGRESS=0x01,
    O_RESULT,
}OtaCmd;

typedef enum
{
    CUSTOM_PKT_TYPE_INVALID,                                                                               
    CUSTOM_PKT_TYPE_START,                                                                                  
    CUSTOM_PKT_TYPE_INIT,                                                                                   
    CUSTOM_PKT_TYPE_FIRMWARE_DATA,
    CUSTOM_PKT_TYPE_USER
} custom_pkt_type_t;

#define UPDATE_DFU_FLAG			0x8000
#define UPDATE_NORDIC			       0x04
#define UPDATE_KL17			              0x08
#define UPDATE_TP		              	0x10
#define UPDATE_HR		              	0x20
#define UPDATE_EXFLASH		              0x40

extern uint8_t CustomUpdate;
extern uint32_t  CustomNordic_Addr;						/*Nordic的地址*/
extern uint32_t  CustomNordic_Size;						/*Nordic的大小*/
extern uint32_t  CustomNordic_Crc;							/*Nordic的crc值*/
extern uint16_t  OtaResult;

void UartInitial(void);
void UART_Send( uint8_t *buf, int size);
void OtaSendCmd(OtaCmd cmd,uint8_t data);

void SpiFlash_Init(void);
void SpiFlash_64KBlock_Erase(uint32_t EraseAddr);
void SpiFlash_Write(uint32_t addr,uint8_t*pBuf, uint16_t len);
void SpiFlash_Read(uint32_t addr,uint8_t*pBuf , uint16_t len);
void ReadWatchId(uint8_t *dat);

uint8_t IsCustomDFU(uint8_t type);
void Custom_OTA_handler(ble_dfu_t * p_dfu, ble_dfu_evt_t * p_evt);
void StoreOtaResult(void);

#endif


