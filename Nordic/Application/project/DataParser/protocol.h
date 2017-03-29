/* Copyright (c) 2013 Nordic Semiconductor. All Rights Reserved.
 *
 * The information contained herein is property of Nordic Semiconductor ASA.
 * Terms and conditions of usage are described in detail in NORDIC
 * SEMICONDUCTOR STANDARD SOFTWARE LICENSE AGREEMENT.
 *
 * Licensees are granted free, non-transferable use of the information. NO
 * WARRANTY of ANY KIND is provided. This heading must NOT be removed from
 * the file.
 *
 */

#ifndef PROTOCOL_H__
#define PROTOCOL_H__

#include <stdint.h>
#include "nrf_gpio.h"

#define EMV_TEST	1

#define SE_NOTIFY_EVENT		0xA0

#define NFC_NOTIFY_EVENT_IN	0x01
#define NFC_NOTIFY_EVENT_OUT	0x02

#define NFC_STATUS_FIELD_IDLE	0x00
#define NFC_STATUS_FIELD_IN	0x01
#define NFC_STATUS_FIELD_OUT	0x02
#define NFC_STATUS_START		0x03
#define NFC_STATUS_STOP		0x04

#define SLOT_STATUS_IDLE		0x01
#define SLOT_STATUS_BUSY		0x02


//CMD len
#define CMD_LEN_8BYTE			0x08


#define DEFAULT_TIMEOUT_VALUE		800

typedef enum{
    NFC_OPEN =1,
    NFC_CLOSE
}NfcAction;

#define KL17_INT_LOW(gpio_pin) do { NRF_GPIO->OUTCLR = (1 << gpio_pin); } while (0)

#define KL17_INT_HIGH(gpio_pin) do { NRF_GPIO->OUTSET = (1 << gpio_pin); } while (0)

#define KL17_INT_CONFIGURE(gpio_pin) do {nrf_gpio_cfg_output(gpio_pin); } while (0)


#define SPI_COMMAND_PAYLOAD_MAX_SIZE  260
#define SPI_BLOCK_SIZE 		20

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
	uint8_t error;
	uint8_t pattern_check;
	uint8_t rx_done;
	uint8_t tx_done;
} spi_ctx_t;

typedef enum{
    O_PROGRESS=0x01,
    O_RESULT,
}OtaCmd;


extern char NFC_ACTIVE_FLAG;
extern char NFC_FIELD_FLAG;
extern char DATA_TRAN_FLAG;
extern char check_state;
extern uint8_t m_tx_buf[];   /**< SPI TX buffer. */      
extern uint8_t m_rx_buf[];   /**< SPI RX buffer. */     
extern uint8_t KL17_VER[];
extern uint8_t HR_VER[];

extern spi_buffer_t spi_buffer;
extern spi_ctx_t m_spi_ctx;
extern uint8_t nfc_status;
extern uint8_t slot_status;

uint32_t spi_init(void);
int spi_send_data(uint8_t * p_tx_buf, uint32_t tx_len, uint32_t rx_len);
void KL17_Power_Enble(void);
void KL17_Power_Disable(void);
void KL17_Power_Suspend(void);
void KL17_Power_Resume(void);
void NFC_Active_Enable(void);
void NFC_Active_Disable(void);
bool spi_send_package(uint8_t * P_buf, uint16_t offset, uint16_t len);
static void spi_receive_package(void);
uint8_t spi_send_cmd(uint8_t type, uint8_t * buf,  uint16_t  len );
uint8_t spi_receive_data(uint8_t * buf,  uint8_t * len );
void OtaSendCmd(OtaCmd cmd,uint8_t data);
void ChangeTimeoutValue(uint32_t mSecond);


#endif 

/** @} */
