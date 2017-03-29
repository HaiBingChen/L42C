#ifndef _7816_H
#define _7816_H

#include "spi.h"

#define EMV_TEST				0x01

//command
#define APDU_POWER_ON			0x20
#define APDU_POWER_OFF			0x21
#define APDU_GET_STATUS			0x22
#define APDU_TX_DATA			0x23
#define NFC_ACTION				0x24
#define LED_LIGHT				0x25
#define APDU_BLOCK_UNBLOCK		0x26
#define APDU_READ_LOG			0x27
#define BOOSTER_SET				0x28
#define BOOSTER_GET				0x29
#define GET_KL17_VERSION		0x2A



#define SE_RET_DATA				0x60
#define SE_RSP_MSG				0x61
#define SE_RET_STATUS			0x62

#define SE_NOTIFY_EVENT			0xA0

//NFC start/stop
#define NFC_START				0x01
#define NFC_STOP				0x02

#define BLOCK_APPLICATION		0x00
#define UNBLOCK_APPLICATION		0x01

#define NFC_NOTIFY_EVENT_IN		0x01
#define NFC_NOTIFY_EVENT_OUT	0x02

//CMD List
#define SET_DATA_LEN_CMD		0x01
#define RESP_DATA_CMD			0x02
#define TRAN_DATA_CMD			0x03
#define TRAN_BALANCES_CMD		0x04
#define TRAN_STATUS_CMD			0x05

//DATA len
#define RESP_DATA_2BYTE			0x02
#define LEN_DATA_2BYTE			0x02

//CMD len
#define CMD_LEN_8BYTE			0x08
#define RESP_LEN_8BYTE			0x08

//respone state
#define RESP_SUCCESS			0x00
#define RESP_FAIL				0x01
#define RESP_INVALID			0x03

//PIT 
#define BOARD_PIT_INSTANCE  	0

extern spi_buffer_t spi_buffer_temp;

uint8_t Process_7816(void);
void S7816_Init(void);
void S7816_Deinit(void);
uint8_t Update_balances(void);
uint8_t Update_NFC_Status(uint8_t IN_OUT);
void printf_buf(uint8_t * buf, uint32_t len);
void process_spi_command(uint8_t* p_data, uint16_t p_len);
void notify_nfc_change(uint8_t event);
void switch_idle_mode(void);
uint8_t Spi_Send_MSG(uint8_t cmd, uint8_t rsp, uint8_t* data, uint16_t len);


#endif


