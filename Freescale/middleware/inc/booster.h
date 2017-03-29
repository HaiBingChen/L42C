#ifndef _BOOSTER_H_
#define _BOOSTER_H_

#include "fsl_spi_master_driver.h"
#include "fsl_gpio_driver.h"
#include "fsl_clock_manager.h"
#include "fsl_debug_console.h"

#define SPI_BOOSTER_INSTANCE          (0)
#define BOOSTER_TRANSFER_SIZE               (16)
#define BOOSTER_TRANSFER_BAUDRATE           (4000000U)           /*! Transfer baudrate - 500k */
#define BOOSTER_TRANSFER_HIGH_BAUDRATE      (8000000U)           /*! Transfer baudrate - 500k */
#define BOOSTER_MASTER_TRANSFER_TIMEOUT     (5000U)             /*! Transfer timeout of master - 5s */

#define BOOSTER_CS_HIGH()			GPIO_DRV_SetPinOutput(BOOSTER_CS)
#define BOOSTER_CS_LOW()			GPIO_DRV_ClearPinOutput(BOOSTER_CS)

enum _booster_pins_pinNames{
  BOOSTER_CS = GPIO_MAKE_PIN(GPIOC_IDX, 4U),
};

enum booster_states {
	BOOSTER_IDLE_MODE = 0x01,
	BOOSTER_CONTACT_MODE,
	BOOSTER_CONTACTLESS_IF_MODE,
	BOOSTER_CONTACTLESS_OF_MODE,
};

enum nfc_states {
	NFC_ACTIVE_AND_INIT = 0x1,
	NFC_ACTIVE_AND_UNINIT,
	NFC_INACTIVE,
	NFC_BLOCK_STATE,
	NFC_UNBLOCK_STATE,
};

typedef struct {
	uint8_t  mode;
	uint8_t  nfc_state;
	uint8_t  block;
} booster_state_t;

typedef struct {
	uint8_t  bNumOfCommands;
	uint8_t  * abSPICommands;
	uint8_t  bForceLoadCommand;
	uint8_t  length;
} booster_para_t;

/* Table of SPI FIFO sizes per instance. */
extern const uint32_t g_spiFifoSize[SPI_INSTANCE_COUNT];
extern booster_state_t booster_state;

#endif

void Booster_Init(void);
void Booster_WriteData( uint8_t * Buffer,  uint32_t SIZE);
void Booster_WriteReadData( uint8_t * Txbuf, uint8_t * Rxbuf,  uint32_t SIZE);
void BoosterIdleMode(void);
void BoosterContactMode(void);
void BoosterContactlessMode(void);
void BoosterIrqEnable(void);
void BoosterIrqDisable(void);
void BoosterIrqClear(void);
uint8_t BoosterReadStatus(void);
void BoosterSetting(void);
void BoosterReadSetting(void);
void BoosterSettingParameter(uint8_t * buf, uint32_t len);
void BoosterReadParameter(uint8_t * para);
bool BoosterSetParameter(uint8_t * buf, uint32_t len);
void BoosterGetParameter(uint8_t *buf, uint32_t len, uint8_t *para);




