#ifndef _SPI_H
#define _SPI_H

#include "fsl_spi_master_driver.h"
#include "fsl_gpio_driver.h"
#include "fsl_clock_manager.h"
#include "fsl_debug_console.h"

#define SPI_MASTER_INSTANCE          (0)                 /*! User change define to choose SPI instance */
#define TRANSFER_SIZE               (16)
#define TRANSFER_BAUDRATE           (8000000U)           /*! Transfer baudrate - 500k */
#define MASTER_TRANSFER_TIMEOUT     (5000U)             /*! Transfer timeout of master - 5s */
#define SPI_COMMAND_PAYLOAD_MAX_SIZE  270
#define BLOCK_SIZE 					20
#define SPI_HEADER_COMMAND_SIZE 	4
#define CS_HIGH()			GPIO_DRV_SetPinOutput(SPI_CS)
#define CS_LOW()			GPIO_DRV_ClearPinOutput(SPI_CS)


#define SPI_IO_TX_MODE       0x10
#define SPI_IO_RX_CMD_MODE   0x20
#define SPI_IO_RX_APDU_MODE  0x40
#define SPI_IO_PROCESS_MODE  0x80

/** 
 * @defgroup CCID Event / Command / Response message type definitions
 */
#define SPI_CCID_NOTIFY_SLOT_CHANGE_CMD_ID  0x50
#define SPI_CCID_ICC_POWER_ON_CMD_ID        0x62
#define SPI_CCID_ICC_POWER_OFF_CMD_ID       0x63
#define SPI_CCID_GET_SLOT_STATUS_CMD_ID     0x65
#define SPI_CCID_XFR_BLOCK_CMD_ID           0x6F
#define SPI_CCID_DATA_BLOCK_CMD_ID          0x80
#define SPI_CCID_SLOT_STATUS_CMD_ID         0x81
			
#define SPI_CCID_HEADER_COMMAND_SIZE 10
#define SPI_CCID_PACKET_SIZE 20
#define SPI_CCID_COMMAND_PAYLOAD_MAX_SIZE 260
			
#define CCID_SE_PRESENT_AND_ACTIVE   0
#define CCID_SE_PRESENT_AND_INACTIVE 1
#define CCID_NO_SE_PRESENT           2
#define CCID_SLOT_STATUS_RFU         3
			
#define CCID_SLOT_NUMBER 0
			
#define CCID_ERROR_CMD_NOT_SUPPORTED 0x00
#define CCID_ERROR_BAD_LENGTH_SUPPORTED 0x01
#define CCID_ERROR_SLOT_NOT_SUPPORTED 0x05
#define CCID_ERROR_BLK_NO_BLOCK		0x81
#define CCID_ERROR_BLK_ERR_SEQUENCE 0x83
#define CCID_ERROR_BLK_RTT_LENGTH	0x85
#define CCID_ERROR_BLK_ERR_CCID_LEN 0x86
#define CCID_ERROR_CMD_SLOT_BUSY 0xE0
#define CCID_ERROR_ICC_MUTE 0xFE
			
#define CCID_SLOT_STATUS_CMD_FAILED (1 << 6)

#define SPI_RSP_MSG_SUCCED			0x00
#define SPI_RSP_MSG_FAILED			0x01


enum _spi_pins_pinNames{
  SPI_CS = GPIO_MAKE_PIN(GPIOC_IDX, 9U),
  SPI_INT= GPIO_MAKE_PIN(GPIOC_IDX, 3U),
  NFC_INT= GPIO_MAKE_PIN(GPIOC_IDX, 1U)
};

typedef struct {
	uint8_t  mode;
} spi_io_t;

typedef struct spi_s {
	uint8_t	            is_notification_enabled;
	spi_io_t            io;
} spi_t;


typedef struct {
	uint8_t type;
	uint8_t status;
} spi_notification_t;

typedef struct __attribute__((__packed__)) {
	uint8_t   tags;
	uint8_t   type;
	uint16_t  len;
} spi_command_header_t;

typedef struct {
	spi_command_header_t header;
	uint8_t payload[SPI_COMMAND_PAYLOAD_MAX_SIZE];
} spi_command_t;

typedef struct {
	uint16_t buf_notification_size;
	uint16_t buf_notification_write_idx;
	uint16_t buf_command_response_read_idx;
	uint16_t buf_command_response_write_idx;
	uint16_t buf_command_response_size;
	uint16_t spi_packet_to_receive;
	uint8_t  buf_notification[sizeof(spi_notification_t)];
	uint8_t  buf_command_response[sizeof(spi_command_t)];
} spi_buffer_t;

typedef struct {
	uint8_t first_cmd;
	uint8_t last_num;
	uint8_t status;
	uint8_t pattern_check;
	bool rx_done;
	bool tx_done;
} spi_ctx_t;



/* Table of SPI FIFO sizes per instance. */
extern const uint32_t g_spiFifoSize[SPI_INSTANCE_COUNT];
/*! @brief Table of base pointers for SPI instances. */
extern SPI_Type * const g_spiBase[SPI_INSTANCE_COUNT];

extern char SPI_INT_FLAG;
extern char NFC_INT_FLAG;
extern char NFC_UPDATE_FLAG;
extern char NFC_Field_FLAG;
extern spi_buffer_t spi_buffer;
extern spi_ctx_t m_spi_ctx;
extern spi_t m_spis;

void SPI_Init(void );
void SPI_ReadData(SPI_Type * spiBaseAddr, uint8_t * Buffer, uint32_t SIZE);
void SPI_WriteData(SPI_Type * spiBaseAddr, uint8_t * TxBuffer, uint8_t * RxBuffer, uint32_t SIZE);
bool spi_send_packets(uint8_t * P_buf,  uint16_t len);

void SPI_INT_CFG_INPUT(void);
void SPI_INT_CFG_OUTPUT_LOW(void);
void SPI_INT_CFG_OUTPUT_HIGH(void);
void SPI_INT_CFG_OUTPUT(void);
#endif
