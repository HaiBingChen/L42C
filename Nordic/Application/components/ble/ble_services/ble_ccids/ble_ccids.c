/******************************************************************************
 * Copyright Gemalto, unpublished work, created 2014. This computer program
 * includes Confidential, Proprietary Information and is a Trade Secret of
 * Gemalto. All use, disclosure, and/or reproduction is prohibited unless
 * authorised in writing by an officer of Gemalto. All Rights Reserved.
 *****************************************************************************/

/**
 * @file	ble_ccid.c
 * @author	Aurelien Couvert
 *
 * @brief 	Device Information Service module.
 *
 * @details This module implements the CCID Service.
 *          During initialization it adds the CCID Service to the BLE stack database.
 *          It then encodes the supplied information, and adds the corresponding characteristics.
 */
 
#include <string.h>

#include "ble_ccids.h"
#include "nordic_common.h"
#include "ble_srv_common.h"
#include "headfile.h"

#define DEBUG 0


#define BLOCK_SIZE 20

#define BLE_CCID_IO_TX_MODE       0x10
#define BLE_CCID_IO_RX_CMD_MODE   0x20
#define BLE_CCID_IO_RX_APDU_MODE  0x40
#define BLE_CCID_IO_PROCESS_MODE  0x80
#define BLE_CCID_IO_RX_SPI_MODE   0x01


 /** 
  * @brief Set .type and .uuid fields of ble_uuid128_struct to specified uuid value. 
  * 
  * @note  SHOULD be moved to ble_type.h file.
  */
#define GET_BLE_UUID_FROM_UUID128(uuid, uuid128) do {\
            uuid = ((uuid128[13] << 8) | uuid128[12]); \
			} while(0)

/** 
 * @defgroup CCID Event / Command / Response message type definitions
 */
#define BLE_CCID_NOTIFY_SLOT_CHANGE_CMD_ID  		0x50
#define BLE_CCID_NOTIFY_ACCUMULATOR_BALANCE_CMD_ID	0x52
#define BLE_CCID_ICC_POWER_ON_CMD_ID        		0x62
#define BLE_CCID_ICC_POWER_OFF_CMD_ID       		0x63
#define BLE_CCID_GET_SLOT_STATUS_CMD_ID     		0x65
#define BLE_CCID_GET_BOOSTER_PARAMETERS				0x66
#define BLE_CCID_SET_BOOSTER_PARAMETERS				0x67
#define BLE_CCID_VIBRATE_CMD_ID						0x68
#define BLE_CCID_APP_BLOCK_UNBLOCK_CMD_ID			0x69
#define BLE_CCID_READ_TRANSACTION_LOG_CMD_ID		0x6A
#define BLE_CCID_ACTIVE_NFC_CMD_ID					0x6B
#define BLE_CCID_XFR_BLOCK_CMD_ID           		0x6F
#define BLE_CCID_DATA_BLOCK_CMD_ID          		0x80
#define BLE_CCID_SLOT_STATUS_CMD_ID         		0x81
			
#define BLE_CCID_HEADER_COMMAND_SIZE 10
#define BLE_CCID_PACKET_SIZE 20
#define BLE_CCID_COMMAND_PAYLOAD_MAX_SIZE 260
#define BLE_CCID_COMMAND_ACC_MAX_SIZE	6
			
#define CCID_SE_PRESENT_AND_ACTIVE   0
#define CCID_SE_PRESENT_AND_INACTIVE 1
#define CCID_NO_SE_PRESENT           2
#define CCID_SLOT_STATUS_RFU         3
			
#define CCID_SLOT_NUMBER 0
			
#define CCID_ERROR_CMD_NOT_SUPPORTED 0x00
#define CCID_ERROR_BAD_LENGTH_SUPPORTED 0x01
#define CCID_ERROR_SLOT_NOT_SUPPORTED 0x05
#define CCID_ERROR_BLK_ERR_NO_BLOCK	0x81
#define CCID_ERROR_BLK_ERR_SEQUENCE 0x83
#define CCID_ERROR_BLK_ERR_LENGTH	0x85
#define CCID_ERROR_BLK_ERR_CCID_LEN 0x86
#define CCID_ERROR_CMD_SLOT_BUSY 0xE0
#define CCID_ERROR_ICC_MUTE 0xFE
			
#define CCID_SLOT_STATUS_CMD_FAILED 	(1 << 6)
#define CCID_SLOT_STATUS_CMD_ASM_ERROR 	(3 << 6)


#define BLOCK_APPLICATION			0x00
#define UNBLOCK_APPLICATION			0x01

#define PACKETS_KEEPING				0x00
#define	PACKETS_IGNORED				0x01


typedef struct {
	uint8_t type;
	uint8_t status;
} ble_ccid_notification_t;

typedef struct {
	uint8_t type;
	uint8_t acc[BLE_CCID_COMMAND_ACC_MAX_SIZE];
} ble_ccid_notification_acc_t;

			
typedef struct __attribute__((__packed__)) {
	uint8_t  type;
	uint32_t length;
	uint8_t  slot;
	uint8_t  seq_num;
	uint8_t  status;
	uint8_t  error;
	uint8_t  chain_parameter;
} ble_ccid_command_header_t;

typedef struct {
	ble_ccid_command_header_t header;
	uint8_t payload[BLE_CCID_COMMAND_PAYLOAD_MAX_SIZE];
} ble_ccid_command_t;

typedef struct {
	uint16_t buf_notification_size;
	uint16_t buf_notification_write_idx;
	uint16_t buf_command_response_read_idx;
	uint16_t buf_command_response_write_idx;
	uint16_t buf_command_response_size;
	uint16_t ble_packet_to_receive;
	uint8_t  buf_notification[sizeof(ble_ccid_notification_t)];
	uint8_t  buf_command_response[sizeof(ble_ccid_command_t)];
	uint8_t  buf_notification_acc[sizeof(ble_ccid_notification_acc_t)];
} ble_ccid_buffer_t;
			
typedef struct {
	uint8_t first_cmd;
	uint8_t last_seq_num;
	uint8_t status;
	uint8_t ignored;
} ble_ccid_ctx_t;

/** 
 * @brief UUID128 CCID Characteristics definitions.
 * 
 * @note  Byte array in reverse order.
 */
static const ble_uuid128_t BLE_UUID_CCID_RX_CHANNEL_CHARACTERISTIC = {  0xC3, 0x2B, 0xEE, 0x0F, 0x12, 0xE2, 0xF2, 0x91, 0x34, 0x41, 0x74, 0xDE, 0x5A, 0xBF, 0x72, 0xFC };
static const ble_uuid128_t BLE_UUID_CCID_TX_CHANNEL_CHARACTERISTIC = {  0xD8, 0x9A, 0x3D, 0xDD, 0x55, 0x29, 0x6F, 0x9F, 0x53, 0x40, 0xFE, 0x75, 0x75, 0x26, 0x7C, 0x02 };

/**
 * @brief Structure used to identify the CCID service.
 */
static ble_ccids_t m_ccids;

static uint8_t tx_buffer[BLOCK_SIZE];
static uint8_t rx_buffer[BLOCK_SIZE];

static ble_ccid_buffer_t ble_ccid_buffer;
static ble_ccid_ctx_t m_ccid_ctx;

static uint8_t bCommand = 0xFF;
static uint16_t Received_Packets_Cnt = 0x0;
static uint8_t cmd_type = 0;

log_data_t log_data;

//static app_timer_id_t timer;

/**
* @brief Function fo UART Output
*
*/

static void xprintf(uint8_t * buf, uint8_t flag)
{
	#if DEBUG
	int len = 0;
	uint8_t temp[2]={0};

	if (flag  == 0)
	{
		len = strlen((const char *)buf);
		UART_Send(buf, len);
	} else {
		if (((buf[0] >> 4) & 0x0F) < 0x0A)
			temp[0] = ((buf[0] >> 4) & 0x0F) + 0x30;
		else
			temp[0] = ((buf[0] >> 4) & 0x0F) + 0x37;
		if (((buf[0] & 0x0F)) < 0x0A)
			temp[1] = (buf[0] & 0x0F) + 0x30;
		else
			temp[1] = (buf[0] & 0x0F) + 0x37;
		UART_Send("0x", 2);
		UART_Send(temp, 2);
	}
	#endif
}


/**
 * @brief Function for handling the data sending.
 *
 * @details Handles all send events from the CCID Service to the BLE stack.
 *
 * @return      NRF_SUCCESS on successful data sending, otherwise an error code.
 */
static uint32_t ccid_send_packet(uint8_t* buffer, uint16_t* p_offset, uint16_t* p_length) {
	uint32_t                err_code;
	uint16_t				len;
	ble_gatts_hvx_params_t  hvx_params;
	
	m_ccids.io.mode |= BLE_CCID_IO_TX_MODE;
		
	if((m_ccids.conn_handle == BLE_CONN_HANDLE_INVALID) || (m_ccids.is_notification_enabled == 0)){
		err_code = NRF_ERROR_INVALID_STATE;
	}
	else {
		//do {
			len = (*p_length > BLE_CCID_PACKET_SIZE) ? BLE_CCID_PACKET_SIZE : *p_length;
			
			memset(&hvx_params, 0, sizeof(hvx_params));

			hvx_params.handle = m_ccids.tx_handles.value_handle;
			hvx_params.type = BLE_GATT_HVX_NOTIFICATION;
			hvx_params.offset = 0;
			hvx_params.p_len = &len;
			hvx_params.p_data = &buffer[*p_offset];

			#if DEBUG
			xprintf("[BLE][CCID] >: ", 0);
			{
				uint16_t i;
				for(i=0; i<*(hvx_params.p_len); i++) {
					if(i>0) {
						xprintf(" ", 0);
						if((i % 10) == 0) {
							xprintf("\r\n               ", 0);
						}
					}
					//xprintf("%02X", hvx_params.p_data[i]);
					xprintf(&hvx_params.p_data[i], 1);
					xprintf(" ", 0);
				}
			}
			xprintf("\r\n", 0);
			//usart_flush();
			#endif
		
			err_code = sd_ble_gatts_hvx(m_ccids.conn_handle, &hvx_params);
			
			*p_offset += len;
			*p_length -= len;
		//} while((err_code == NRF_SUCCESS) && (*p_length > 0));

		if(err_code == BLE_ERROR_NO_TX_BUFFERS) {
			#if DEBUG
			xprintf("[BLE][CCID] _: BLE_ERROR_NO_TX_BUFFERS\r\n", 0);
			#endif
			
			// Just wait BLE_EVT_TX_COMPLETE to retry
			err_code = NRF_SUCCESS;
			*p_offset -= len;
			*p_length += len;
		}
	}
	
	if(err_code != NRF_SUCCESS) {
		#if DEBUG
		//xprintf("[BLE][CCID] error on sending %d\r\n", err_code);
		xprintf("[BLE][CCID] error on sending ", 0);
		xprintf((uint8_t *)&err_code, 1);
		xprintf("\r\n", 0);
		#endif
		
		*p_offset = 0;
		*p_length = 0;
		m_ccids.io.mode &= ~BLE_CCID_IO_TX_MODE;
	}
	
	return err_code;
}

static void ccid_send_response(uint8_t* p_data, uint16_t len) {
	m_ccids.io.mode &= ~BLE_CCID_IO_PROCESS_MODE;
	m_ccids.io.mode |= BLE_CCID_IO_RX_CMD_MODE;
	
	// Header
	((ble_ccid_command_t*) ble_ccid_buffer.buf_command_response)->header.length = (len & 0xFF) | (((len >> 8) & 0xFF) << 8);
	((ble_ccid_command_t*) ble_ccid_buffer.buf_command_response)->header.slot = CCID_SLOT_NUMBER;
	((ble_ccid_command_t*) ble_ccid_buffer.buf_command_response)->header.status = m_ccid_ctx.status;
	((ble_ccid_command_t*) ble_ccid_buffer.buf_command_response)->header.error = 0;
	((ble_ccid_command_t*) ble_ccid_buffer.buf_command_response)->header.chain_parameter = 0;

	// Payload: p_data already point to ((ble_ccid_command_t*) ble_ccid_buffer.data)->payload
	if(p_data == &((ble_ccid_command_t*) ble_ccid_buffer.buf_command_response)->payload[0]) {
		ble_ccid_buffer.buf_command_response_write_idx = 0;
		ble_ccid_buffer.buf_command_response_size = BLE_CCID_HEADER_COMMAND_SIZE + len;
		
		// If no ongoing transmission
		if((m_ccids.io.mode & BLE_CCID_IO_TX_MODE) == 0) {
			ccid_send_packet(ble_ccid_buffer.buf_command_response, &ble_ccid_buffer.buf_command_response_write_idx, &ble_ccid_buffer.buf_command_response_size);
		}
	}
}

static void ccid_send_error(ble_ccid_command_header_t* header, uint8_t error) {
	uint8_t status = 0;
	
	if ((header->type == BLE_CCID_ICC_POWER_OFF_CMD_ID) || (header->type == BLE_CCID_GET_SLOT_STATUS_CMD_ID)) {  //Add by LBQ
		header->type = BLE_CCID_SLOT_STATUS_CMD_ID;         
	} 
	else if ((header->type == BLE_CCID_ICC_POWER_ON_CMD_ID) || (header->type == BLE_CCID_XFR_BLOCK_CMD_ID) ||  (header->type == BLE_CCID_VIBRATE_CMD_ID) || (header->type == BLE_CCID_APP_BLOCK_UNBLOCK_CMD_ID) || (header->type == BLE_CCID_READ_TRANSACTION_LOG_CMD_ID)) {
		header->type = BLE_CCID_DATA_BLOCK_CMD_ID;
	}

	if ((error == CCID_ERROR_BLK_ERR_NO_BLOCK) || (error == CCID_ERROR_BLK_ERR_SEQUENCE ) || (error == CCID_ERROR_BLK_ERR_LENGTH) || (error == CCID_ERROR_BLK_ERR_CCID_LEN )) {
		status = CCID_SLOT_STATUS_CMD_ASM_ERROR;
	} else {
		status = CCID_SLOT_STATUS_CMD_FAILED;
	}
	
	// Prepare response header
	((ble_ccid_command_t*) ble_ccid_buffer.buf_command_response)->header.type = header->type;
	((ble_ccid_command_t*) ble_ccid_buffer.buf_command_response)->header.length = 0;
	((ble_ccid_command_t*) ble_ccid_buffer.buf_command_response)->header.slot = header->slot;
	((ble_ccid_command_t*) ble_ccid_buffer.buf_command_response)->header.seq_num = header->seq_num;
	((ble_ccid_command_t*) ble_ccid_buffer.buf_command_response)->header.status = ((header->slot == CCID_SLOT_NUMBER) ? m_ccid_ctx.status : CCID_NO_SE_PRESENT) | status;
	((ble_ccid_command_t*) ble_ccid_buffer.buf_command_response)->header.error = error;
	((ble_ccid_command_t*) ble_ccid_buffer.buf_command_response)->header.chain_parameter = 0;
	
	// Send
	ble_ccid_buffer.buf_command_response_write_idx = 0;
	ble_ccid_buffer.buf_command_response_size = BLE_CCID_HEADER_COMMAND_SIZE;
	
	// If no ongoing transmission
	if((m_ccids.io.mode & BLE_CCID_IO_TX_MODE) == 0) {
		ccid_send_packet(ble_ccid_buffer.buf_command_response, &ble_ccid_buffer.buf_command_response_write_idx, &ble_ccid_buffer.buf_command_response_size);
	}
}

static void ccid_send_atr_response(uint8_t* p_data, uint16_t len) {
	m_ccid_ctx.status = CCID_SE_PRESENT_AND_ACTIVE;
	ccid_send_response(p_data, len);
}

/**
 * @brief Function to notify the host of events that may occur asynchronously and 
 *        outside the context of a command-response exchange.
 */

static void notify_slot_change(void) {
	((ble_ccid_notification_t*) ble_ccid_buffer.buf_notification)->type = BLE_CCID_NOTIFY_SLOT_CHANGE_CMD_ID;
	// 3 = 0b11 : Secure Element present and status changed since last time.
	((ble_ccid_notification_t*) ble_ccid_buffer.buf_notification)->status = (3 << CCID_SLOT_NUMBER);
	
	ble_ccid_buffer.buf_notification_write_idx = 0;
	ble_ccid_buffer.buf_notification_size = 2;
	
	// If no ongoing transmission
	if((m_ccids.io.mode & BLE_CCID_IO_TX_MODE) == 0) {
		ccid_send_packet(ble_ccid_buffer.buf_notification, &ble_ccid_buffer.buf_notification_write_idx, &ble_ccid_buffer.buf_notification_size);
	}
}

static void notify_accumulator_balance(uint8_t* p_data, uint16_t len) {
	((ble_ccid_notification_acc_t*) ble_ccid_buffer.buf_notification_acc)->type = BLE_CCID_NOTIFY_ACCUMULATOR_BALANCE_CMD_ID;
	
	ble_ccid_buffer.buf_notification_write_idx = 0;
	ble_ccid_buffer.buf_notification_size = 1 + len;

	if(p_data == &((ble_ccid_notification_acc_t*) ble_ccid_buffer.buf_notification_acc)->acc[0]) {
		// If no ongoing transmission
		if((m_ccids.io.mode & BLE_CCID_IO_TX_MODE) == 0) {
			ccid_send_packet(ble_ccid_buffer.buf_notification_acc, &ble_ccid_buffer.buf_notification_write_idx, &ble_ccid_buffer.buf_notification_size);
		}
	}
}


static void ccid_send_status(ble_ccid_command_header_t* header) {
	
	// Prepare response header = Secure Element present and active
	((ble_ccid_command_t*) ble_ccid_buffer.buf_command_response)->header.type = BLE_CCID_SLOT_STATUS_CMD_ID;
	((ble_ccid_command_t*) ble_ccid_buffer.buf_command_response)->header.length = 0;
	((ble_ccid_command_t*) ble_ccid_buffer.buf_command_response)->header.slot = CCID_SLOT_NUMBER;
	((ble_ccid_command_t*) ble_ccid_buffer.buf_command_response)->header.seq_num = header->seq_num;
	((ble_ccid_command_t*) ble_ccid_buffer.buf_command_response)->header.status = m_ccid_ctx.status;
	((ble_ccid_command_t*) ble_ccid_buffer.buf_command_response)->header.error = 0;
	((ble_ccid_command_t*) ble_ccid_buffer.buf_command_response)->header.chain_parameter = 0;

	// Send
	ble_ccid_buffer.buf_command_response_write_idx = 0;
	ble_ccid_buffer.buf_command_response_size = BLE_CCID_HEADER_COMMAND_SIZE;

	m_ccids.io.mode |= BLE_CCID_IO_RX_CMD_MODE;

	// If no ongoing transmission
	if((m_ccids.io.mode & BLE_CCID_IO_TX_MODE) == 0) {
		ccid_send_packet(ble_ccid_buffer.buf_command_response, &ble_ccid_buffer.buf_command_response_write_idx, &ble_ccid_buffer.buf_command_response_size);
	}

}

void process_power_on_cmd(void)
{
	uint8_t buf[1] = {0};
	uint8_t status = 0;

	ChangeTimeoutValue(9000);

	status = spi_send_cmd(APDU_POWER_ON, buf,1);
	if	(status == SPI_FAILED)
	{
		xprintf("process_power_on_cmd: spi_send_cmd()\r\n", 0);
	}
	status = spi_receive_data(&((ble_ccid_command_t*) ble_ccid_buffer.buf_command_response)->payload[0],(uint8_t *)&((ble_ccid_command_t*) ble_ccid_buffer.buf_command_response)->header.length);
	if (status != SPI_RSP_DATA)
	{
		xprintf("process_power_on_cmd: spi_receive_data()\r\n", 0);
	}
	
	ccid_send_atr_response(&((ble_ccid_command_t*) ble_ccid_buffer.buf_command_response)->payload[0], (uint16_t)((ble_ccid_command_t*) ble_ccid_buffer.buf_command_response)->header.length);

	ChangeTimeoutValue(DEFAULT_TIMEOUT_VALUE);
}
void process_power_off_cmd(void)
{
	uint8_t buf[1] = {0};
	uint8_t status = 0;
	uint8_t len = 0;

	ChangeTimeoutValue(9000);

	status = spi_send_cmd(APDU_POWER_OFF, buf,1);
	if	(status == SPI_FAILED)
	{
		xprintf("process_power_off_cmd: spi_send_cmd()\r\n", 0);
	}
	status = spi_receive_data(&((ble_ccid_command_t*) ble_ccid_buffer.buf_command_response)->payload[0], &len);
	if (status != SPI_RSP_STATUS)
	{
		xprintf("process_power_off_cmd: spi_receive_data()\r\n", 0);
	}
	// If no ongoing transmission
	if((m_ccids.io.mode & BLE_CCID_IO_TX_MODE) == 0) {
		ccid_send_packet(ble_ccid_buffer.buf_command_response, &ble_ccid_buffer.buf_command_response_write_idx, &ble_ccid_buffer.buf_command_response_size);
	}

	ChangeTimeoutValue(DEFAULT_TIMEOUT_VALUE);

}

void process_apdu_cmd(void)
{
	uint8_t status = 0;

	ChangeTimeoutValue(600000);
	
	status = spi_send_cmd(APDU_TX_DATA, &((ble_ccid_command_t*) ble_ccid_buffer.buf_command_response)->payload[0],((ble_ccid_command_t*) ble_ccid_buffer.buf_command_response)->header.length);
	if	(status == SPI_FAILED)
	{
		xprintf("process_apdu_cmd: spi_send_cmd()\r\n", 0);
	}
	status = spi_receive_data(&((ble_ccid_command_t*) ble_ccid_buffer.buf_command_response)->payload[0],(uint8_t *)&((ble_ccid_command_t*) ble_ccid_buffer.buf_command_response)->header.length);
	if (status != SPI_RSP_DATA)
	{
		
		xprintf("process_apdu_cmd: spi_receive_data()\r\n", 0);
	}

	ccid_send_response(&((ble_ccid_command_t*) ble_ccid_buffer.buf_command_response)->payload[0], (uint16_t)((ble_ccid_command_t*) ble_ccid_buffer.buf_command_response)->header.length);

	ChangeTimeoutValue(DEFAULT_TIMEOUT_VALUE);

}

void process_set_booster_parameters_cmd(void)
{
	uint8_t status = 0;

	ChangeTimeoutValue(9000);
	
	status = spi_send_cmd(BOOSTER_SET, &((ble_ccid_command_t*) ble_ccid_buffer.buf_command_response)->payload[0],((ble_ccid_command_t*) ble_ccid_buffer.buf_command_response)->header.length);
	if	(status == SPI_FAILED)
	{
		xprintf("set_booster_parameters: spi_send_cmd()\r\n", 0);
	}
	status = spi_receive_data(&((ble_ccid_command_t*) ble_ccid_buffer.buf_command_response)->payload[0],(uint8_t *)&((ble_ccid_command_t*) ble_ccid_buffer.buf_command_response)->header.length);
	if (status != SPI_RSP_SUCCED)
	{
		
		xprintf("set_booster_parameters: spi_receive_data()\r\n", 0);
		((ble_ccid_command_t*) ble_ccid_buffer.buf_command_response)->payload[0] = 0xFF;
		((ble_ccid_command_t*) ble_ccid_buffer.buf_command_response)->header.length = 1;
	} else {
		((ble_ccid_command_t*) ble_ccid_buffer.buf_command_response)->payload[0] = 0x00;
		((ble_ccid_command_t*) ble_ccid_buffer.buf_command_response)->header.length = 1;
	}
	ccid_send_response(&((ble_ccid_command_t*) ble_ccid_buffer.buf_command_response)->payload[0], (uint16_t)((ble_ccid_command_t*) ble_ccid_buffer.buf_command_response)->header.length);

	ChangeTimeoutValue(DEFAULT_TIMEOUT_VALUE);
}

void process_get_booster_parameters_cmd(void)
{
	uint8_t status = 0;

	ChangeTimeoutValue(9000);
	
	status = spi_send_cmd(BOOSTER_GET, &((ble_ccid_command_t*) ble_ccid_buffer.buf_command_response)->payload[0],((ble_ccid_command_t*) ble_ccid_buffer.buf_command_response)->header.length);
	if	(status == SPI_FAILED)
	{
		xprintf("get_booster_parameters: spi_send_cmd()\r\n", 0);
	}
	status = spi_receive_data(&((ble_ccid_command_t*) ble_ccid_buffer.buf_command_response)->payload[0],(uint8_t *)&((ble_ccid_command_t*) ble_ccid_buffer.buf_command_response)->header.length);
	if (status != SPI_RSP_DATA)
	{
		
		xprintf("get_booster_parameters: spi_receive_data()\r\n", 0);
	}
	ccid_send_response(&((ble_ccid_command_t*) ble_ccid_buffer.buf_command_response)->payload[0], (uint16_t)((ble_ccid_command_t*) ble_ccid_buffer.buf_command_response)->header.length);

	ChangeTimeoutValue(DEFAULT_TIMEOUT_VALUE);
}

void process_vibrate_cmd(void)
{
	uint8_t bVibrateResult = 0x00;

	memcpy(&((ble_ccid_command_t*) ble_ccid_buffer.buf_command_response)->payload[0], &bVibrateResult, 1);
	ccid_send_response(&((ble_ccid_command_t*) ble_ccid_buffer.buf_command_response)->payload[0], (uint16_t)((ble_ccid_command_t*) ble_ccid_buffer.buf_command_response)->header.length);
}



void process_active_nfc_cmd(void)
{
	uint8_t bNfcStatus = 0x00;

	memcpy(&((ble_ccid_command_t*) ble_ccid_buffer.buf_command_response)->payload[0], &bNfcStatus, 1);
	ccid_send_response(&((ble_ccid_command_t*) ble_ccid_buffer.buf_command_response)->payload[0], (uint16_t)((ble_ccid_command_t*) ble_ccid_buffer.buf_command_response)->header.length);
}



void process_app_block_unblock(void)
{
	uint8_t status = 0;
	uint16_t len = 0;
	uint8_t bCommandResult = 0;

	ChangeTimeoutValue(9000);
	
	status = spi_send_cmd(APDU_BLOCK_UNBLOCK, &bCommand,1);
	if	(status == SPI_FAILED) {
		bCommandResult = 0x01;
		xprintf("process_app_block_unblock: spi_send_cmd()\r\n", 0);
		goto CCID_RSP;
	}
	status = spi_receive_data(&bCommandResult, (uint8_t *)&len);
	if (status > SPI_SUCCED) {
		bCommandResult = 0x01;
	}

CCID_RSP:

	memcpy(&((ble_ccid_command_t*) ble_ccid_buffer.buf_command_response)->payload[0], &bCommandResult, 1);
	ccid_send_response(&((ble_ccid_command_t*) ble_ccid_buffer.buf_command_response)->payload[0], (uint16_t)((ble_ccid_command_t*) ble_ccid_buffer.buf_command_response)->header.length);

	ChangeTimeoutValue(DEFAULT_TIMEOUT_VALUE);

}

static uint32_t BCDtoDec(const uint8_t *bcd, int length)
{
	int i, tmp;
	uint32_t dec = 0;

	for(i=0; i<length; i++)
	{
		tmp = ((bcd[i]>>4)&0x0F)*10 + (bcd[i]&0x0F);   
		dec += tmp * pow(100, length-1-i);          
	}

	return dec;
}

static void BCDtoAscii(const uint8_t *bcd, uint8_t length, uint8_t *ascii)
{
	int i;

	for(i=0; i<length; i++)
	{
		ascii[2*i] = (bcd[i]>>4) + 0x30;
		ascii[2*i+1] = (bcd[i]&0x0F) + 0x30;
	}
}

static void fill_log_data(const uint8 *buf)
{
	log_data.Amount = BCDtoDec(&buf[1], 6);
	BCDtoAscii(&buf[9] ,3, log_data.Date);
	BCDtoAscii(&buf[21],3,log_data.Time);
}

void get_balance(uint32_t balance)
{
	balance *= 100; 
	log_data.Balance = balance;
}

void process_read_transaction_log(void)
{
	uint8_t status = 0;
	uint8_t index = 0;
	uint8_t i = 0;
	uint8_t	bCommandResult = 0;
	uint8_t bNumOfLog = 0;
	uint8_t cmd[5]={0};
	uint8_t buf[SPI_COMMAND_PAYLOAD_MAX_SIZE] = {0};
	//uint8_t record_log[45]={0x00,0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x15, 0x09, 0x21, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x12, 0x49, 0x07, 0x00};
	uint16 len = 0;

	ChangeTimeoutValue(9000);

	//POWER ON
	status = spi_send_cmd(APDU_POWER_ON, buf,1);
	if	(status == SPI_FAILED) {
		xprintf("process_read_transaction_log 1 \r\n", 0);
		bCommandResult = 0x01;
		goto POWER_OFF;
	}

	status = spi_receive_data(buf, (uint8_t *)&len);
	if (status != SPI_RSP_DATA) {
		xprintf("process_read_transaction_log 2\r\n", 0);
		bCommandResult = 0x01;
		goto POWER_OFF;
	}


	//select AID
	status = spi_send_cmd(APDU_TX_DATA, "\x00\xA4\x04\x00\x07\xA0\x00\x00\x00\x04\x10\x10", 12);
	if	(status == SPI_FAILED) {
		xprintf("process_read_transaction_log 3\r\n", 0);
		bCommandResult = 0x01;
		goto POWER_OFF;
	}

	status = spi_receive_data(buf, (uint8_t *)&len);
	if (status != SPI_RSP_DATA) {
		xprintf("process_read_transaction_log 4\r\n", 0);
		bCommandResult = 0x01;
		goto POWER_OFF;
	}

	//read record
	for (i = 0; i < 10; i++) {
		cmd[0] = 0x00;
		cmd[1] = 0xB2;
		cmd[2] = ++index;
		cmd[3] = 0x5C;
		cmd[4] = 0x00;
		status = spi_send_cmd(APDU_TX_DATA, cmd, 5);
		if	(status == SPI_FAILED) {
			xprintf("process_read_transaction_log 5\r\n", 0);
			bCommandResult = 0x01;
			goto POWER_OFF;
		}

		status = spi_receive_data(buf, (uint8_t *)&len);
		if (status == SPI_RSP_DATA) {
			if ((buf[0] == 0x6A) && (buf[1] == 0x83)) {
				//fill_log_data(record_log);
				break;
			} else {
				fill_log_data(buf);
				memcpy(&((ble_ccid_command_t*) ble_ccid_buffer.buf_command_response)->payload[2 + i*24], buf, 24);
				bNumOfLog ++;
			}
			
		}
		else {
			bCommandResult = 0x01;
			xprintf("process_read_transaction_log 6\r\n", 0);
		}
	}


POWER_OFF:
	if (bCommandResult == 0x01) {
		bNumOfLog = 0;
	}

	//POWER OFF
	status = spi_send_cmd(APDU_POWER_OFF, buf,1);
	if	(status == SPI_FAILED) {
		xprintf("process_read_transaction_log 7\r\n", 0);
	}
	status = spi_receive_data(buf,(uint8_t *)&len);
	if (status != SPI_RSP_STATUS) {
		xprintf("process_read_transaction_log 8\r\n", 0);
	}

	slot_status = SLOT_STATUS_IDLE;
	ChangeTimeoutValue(DEFAULT_TIMEOUT_VALUE);
}

void swtich_to_idleMode(void) {
	uint8_t buf[1] = {0};
	uint8_t status = 0;
	uint8_t len = 0;

	if(m_ccid_ctx.status == CCID_SE_PRESENT_AND_ACTIVE) {
		if((m_ccids.io.mode & BLE_CCID_IO_RX_CMD_MODE) > 0) {
			status = spi_send_cmd(APDU_POWER_OFF, buf,1);
			if	(status == SPI_FAILED)
			{
				xprintf("swtich_to_idleMode: spi_send_cmd()\r\n", 0);
			}
			status = spi_receive_data(&((ble_ccid_command_t*) ble_ccid_buffer.buf_command_response)->payload[0], &len);
			if (status != SPI_RSP_STATUS)
			{
				xprintf("swtich_to_idleMode: spi_receive_data()\r\n", 0);
			}
			m_ccid_ctx.status = CCID_SE_PRESENT_AND_INACTIVE;
		}
	}

}


uint8_t ble_wait_packet_timeout(void) {
	
	if ((Received_Packets_Cnt == ble_ccid_buffer.ble_packet_to_receive) && (Received_Packets_Cnt > 0)) {
		m_ccids.io.mode &= ~BLE_CCID_IO_RX_APDU_MODE;
		m_ccids.io.mode |= BLE_CCID_IO_RX_CMD_MODE;
		ccid_send_error((ble_ccid_command_header_t*)&ble_ccid_buffer.buf_command_response[0], CCID_ERROR_BLK_ERR_NO_BLOCK);
		return BLE_PACKETS_TIMEOUT;
	}
	else if (Received_Packets_Cnt > ble_ccid_buffer.ble_packet_to_receive) {
		Received_Packets_Cnt = ble_ccid_buffer.ble_packet_to_receive;
		if (Received_Packets_Cnt) {
			osal_start_timerEx(GetAppTaskId(), MSG_WAIT_BLE_PACKETS_TIMEOUT, 500);
		}
	}
	return BLE_PACKETS_SUCCED;
}



void process_notify_acc_balance(void) {
	uint8_t status = 0;
	uint8_t i = 0;
	uint8_t cmd[5]={0};
	uint8_t buf[SPI_COMMAND_PAYLOAD_MAX_SIZE] = {0};
	uint16 len = 0;

	if (m_ccids.conn_handle == BLE_CONN_HANDLE_INVALID) {
		slot_status = SLOT_STATUS_IDLE;
		return;
	}

	ChangeTimeoutValue(9000);

	//POWER ON
	status = spi_send_cmd(APDU_POWER_ON, buf,1);
	if	(status == SPI_FAILED) {
		xprintf("process_notify_acc_balance 1 \r\n", 0);
		goto POWER_OFF;
	}
	status = spi_receive_data(buf, (uint8_t *)&len);
	if (status != SPI_RSP_DATA) {
		xprintf("process_notify_acc_balance 2\r\n", 0);
		goto POWER_OFF;
	}

	
	//select AID
	status = spi_send_cmd(APDU_TX_DATA, "\x00\xA4\x04\x00\x07\xA0\x00\x00\x00\x04\x10\x10", 12);
	if	(status == SPI_FAILED) {
		xprintf("process_notify_acc_balance 3\r\n", 0);
		goto POWER_OFF;
	}
	status = spi_receive_data(buf, (uint8_t *)&len);
	if (status != SPI_RSP_DATA) {
		xprintf("process_notify_acc_balance 4\r\n", 0);
		goto POWER_OFF;
	}

	//read record
	for (i = 0; i < 1; i++) {
		cmd[0] = 0x80;
		cmd[1] = 0xCA;
		cmd[2] = 0x9F;
		cmd[3] = 0x50;
		cmd[4] = 0x09;
		status = spi_send_cmd(APDU_TX_DATA, cmd, 5);
		if	(status == SPI_FAILED) {
			xprintf("process_notify_acc_balance 5\r\n", 0);
			goto POWER_OFF;
		}
		status = spi_receive_data(buf, (uint8_t *)&len);
		if (status == SPI_RSP_DATA) {
			memcpy(&((ble_ccid_command_t*) ble_ccid_buffer.buf_command_response)->payload[2 + i*24], buf, 24);
		}
		else {
			xprintf("process_notify_acc_balance 6\r\n", 0);
		}
	}


POWER_OFF:
	memcpy(&((ble_ccid_notification_acc_t*) ble_ccid_buffer.buf_notification_acc)->acc[0], &buf[3], BLE_CCID_COMMAND_ACC_MAX_SIZE);

	//POWER OFF
	status = spi_send_cmd(APDU_POWER_OFF, buf,1);
	if	(status == SPI_FAILED) {
		xprintf("process_notify_acc_balance 7\r\n", 0);
	}
	status = spi_receive_data(buf,(uint8_t *)&len);
	if (status != SPI_RSP_STATUS) {
		xprintf("process_notify_acc_balance 8\r\n", 0);
	}

	notify_accumulator_balance(&((ble_ccid_notification_acc_t*) ble_ccid_buffer.buf_notification_acc)->acc[0], BLE_CCID_COMMAND_ACC_MAX_SIZE); 

	ChangeTimeoutValue(DEFAULT_TIMEOUT_VALUE);

	ble_ccid_buffer.buf_notification_size = 0;
	m_ccids.io.mode &= ~BLE_CCID_IO_TX_MODE;
	slot_status = SLOT_STATUS_IDLE;
}

static void process_ccid_command(uint8_t* p_data, uint16_t p_len) {
	ble_ccid_command_header_t* header;
	
	header = (ble_ccid_command_header_t*) p_data;

	#if DEBUG
		uint32_t i = 0;
		xprintf("process_ccid_command: \r\n",0);
		for(i=0; i<p_len; i++) {
			xprintf(&p_data[i],1);
			xprintf(" ",0);
		}
		xprintf("\r\n",0);
	#endif

	if (m_ccid_ctx.ignored == PACKETS_IGNORED) {
		ble_ccid_buffer.ble_packet_to_receive--; // one BLE CCID packet less to received.
		if(ble_ccid_buffer.ble_packet_to_receive == 0) {
			m_ccid_ctx.ignored = PACKETS_KEEPING;
		}
		return;
	}

	if(header->length > BLE_CCID_COMMAND_PAYLOAD_MAX_SIZE) {
		ccid_send_error(header, CCID_ERROR_BLK_ERR_CCID_LEN);
		m_ccid_ctx.ignored = PACKETS_IGNORED;
		// Find number of BLE CCID packets expected to be received minus the current one.
		uint16_t len = header->length + BLE_CCID_HEADER_COMMAND_SIZE;
		ble_ccid_buffer.ble_packet_to_receive = 0;				
		while(len > BLOCK_SIZE) {
			ble_ccid_buffer.ble_packet_to_receive++;
			len -= BLOCK_SIZE;
		}
		return;
	}

	if(header->slot != CCID_SLOT_NUMBER) {
		ccid_send_error(header, CCID_ERROR_SLOT_NOT_SUPPORTED);
		return;
	}
	
	switch(header->type) {
		case BLE_CCID_ICC_POWER_ON_CMD_ID:
			if ((nfc_status == NFC_STATUS_START) || (nfc_status == NFC_STATUS_FIELD_IN) || (slot_status == SLOT_STATUS_BUSY)) {
				ccid_send_error(header, CCID_ERROR_CMD_SLOT_BUSY);
				return;
			}
			break;
		case BLE_CCID_ICC_POWER_OFF_CMD_ID:
			if ((nfc_status == NFC_STATUS_START) || (nfc_status == NFC_STATUS_FIELD_IN) || (slot_status == SLOT_STATUS_BUSY)) {
				ccid_send_error(header, CCID_ERROR_CMD_SLOT_BUSY);
				return;
			}
			// A power on must have been sent previously
			if(m_ccid_ctx.status != CCID_SE_PRESENT_AND_ACTIVE) {
				ccid_send_status(header);
				return;
			}
			break;
		case BLE_CCID_GET_SLOT_STATUS_CMD_ID:
		case BLE_CCID_VIBRATE_CMD_ID:
		case BLE_CCID_APP_BLOCK_UNBLOCK_CMD_ID:
		case BLE_CCID_GET_BOOSTER_PARAMETERS:
		case BLE_CCID_SET_BOOSTER_PARAMETERS:
		case BLE_CCID_ACTIVE_NFC_CMD_ID:
			break;
		case BLE_CCID_READ_TRANSACTION_LOG_CMD_ID:
			if ((nfc_status == NFC_STATUS_START) || (nfc_status == NFC_STATUS_FIELD_IN) || (slot_status == SLOT_STATUS_BUSY)) {
				ccid_send_error(header, CCID_ERROR_CMD_SLOT_BUSY);
				return;
			}
			break;
		
		case BLE_CCID_XFR_BLOCK_CMD_ID:
			if ((nfc_status == NFC_STATUS_START) || (nfc_status == NFC_STATUS_FIELD_IN) || (slot_status == SLOT_STATUS_BUSY)) {
				ccid_send_error(header, CCID_ERROR_CMD_SLOT_BUSY);
				return;
			}
			// A power on must have been sent previously
			if(m_ccid_ctx.status != CCID_SE_PRESENT_AND_ACTIVE) {
				ccid_send_error(header, CCID_ERROR_ICC_MUTE);
				return;
			}
			
			// Must be at least 4 bytes long to be a valid APDU
			if(header->length < 4) {
				ccid_send_error(header, CCID_ERROR_BAD_LENGTH_SUPPORTED);
				return;
			}
			break;
			
		default:
			ccid_send_error(header, CCID_ERROR_CMD_NOT_SUPPORTED);
			return;
	}


	if ((header->length + BLE_CCID_HEADER_COMMAND_SIZE) > BLOCK_SIZE) {		//Add by LBQ
		//osal_start_timerEx(GetAppTaskId(), MSG_WAIT_BLE_PACKETS_TIMEOUT, 500);
	}

	
	/*
	if((m_ccid_ctx.first_cmd == 0) && (header->seq_num != ((m_ccid_ctx.last_seq_num + 1) & 0xFF))) {
		ccid_send_error(header, CCID_ERROR_BLK_ERR_SEQUENCE);
		return;
	}	
	*/
	m_ccid_ctx.last_seq_num = header->seq_num;

	switch(header->type) {
		case BLE_CCID_ICC_POWER_ON_CMD_ID:
			// Activate Secure Element and send DATA_BLOCK containing ATR
			{				
				// Prepare response header in advance
				((ble_ccid_command_t*) ble_ccid_buffer.buf_command_response)->header.type = BLE_CCID_DATA_BLOCK_CMD_ID;
				((ble_ccid_command_t*) ble_ccid_buffer.buf_command_response)->header.seq_num = header->seq_num;
							
				m_ccids.io.mode &= ~BLE_CCID_IO_RX_CMD_MODE;
				m_ccids.io.mode |= BLE_CCID_IO_PROCESS_MODE;
				
				// TODO: Perform a POWER ON on the secure element once done call ccid_send_atr_response(uint8_t* p_data, uint16_t len)
				//       where p_data is raw ATR and len is ATR length
				xprintf("POWER ON\r\n", 0);
				SendMsg(MSG_POWER_ON_CMD);
			}
			break;
			
		case BLE_CCID_ICC_POWER_OFF_CMD_ID:
			// De-activate Secure Element and send SLOT_STATUS
			{
				m_ccid_ctx.status = CCID_SE_PRESENT_AND_INACTIVE;
				
				// Prepare response header = Secure Element present and inactive
				((ble_ccid_command_t*) ble_ccid_buffer.buf_command_response)->header.type = BLE_CCID_SLOT_STATUS_CMD_ID;
				((ble_ccid_command_t*) ble_ccid_buffer.buf_command_response)->header.length = 0;
				((ble_ccid_command_t*) ble_ccid_buffer.buf_command_response)->header.slot = CCID_SLOT_NUMBER;
				((ble_ccid_command_t*) ble_ccid_buffer.buf_command_response)->header.seq_num = header->seq_num;
				((ble_ccid_command_t*) ble_ccid_buffer.buf_command_response)->header.status = m_ccid_ctx.status;
				((ble_ccid_command_t*) ble_ccid_buffer.buf_command_response)->header.error = 0;
				((ble_ccid_command_t*) ble_ccid_buffer.buf_command_response)->header.chain_parameter = 0;
				
				// Send
				ble_ccid_buffer.buf_command_response_write_idx = 0;
				ble_ccid_buffer.buf_command_response_size = BLE_CCID_HEADER_COMMAND_SIZE;
								
				m_ccids.io.mode |= BLE_CCID_IO_RX_CMD_MODE;
				
				// Power off eSE
				// TODO: Perform a POWER OFF on the secure element.
				//       There is no callback for this, it assumes it will be successful so it directly sends a successfull BLE CCID response.
				xprintf("POWER OFF\r\n", 0);
				SendMsg(MSG_POWER_OFF_CMD);
				//process_power_off_cmd();
			}
			break;
			
		case BLE_CCID_GET_SLOT_STATUS_CMD_ID:
			// Get Secure Element SLOT_STATUS
			{
				// Prepare response header = Secure Element present and active
				((ble_ccid_command_t*) ble_ccid_buffer.buf_command_response)->header.type = BLE_CCID_SLOT_STATUS_CMD_ID;
				((ble_ccid_command_t*) ble_ccid_buffer.buf_command_response)->header.length = 0;
				((ble_ccid_command_t*) ble_ccid_buffer.buf_command_response)->header.slot = CCID_SLOT_NUMBER;
				((ble_ccid_command_t*) ble_ccid_buffer.buf_command_response)->header.seq_num = header->seq_num;
				((ble_ccid_command_t*) ble_ccid_buffer.buf_command_response)->header.status = m_ccid_ctx.status;
				((ble_ccid_command_t*) ble_ccid_buffer.buf_command_response)->header.error = 0;
				((ble_ccid_command_t*) ble_ccid_buffer.buf_command_response)->header.chain_parameter = 0;
				
				// Send
				ble_ccid_buffer.buf_command_response_write_idx = 0;
				ble_ccid_buffer.buf_command_response_size = BLE_CCID_HEADER_COMMAND_SIZE;
				
				m_ccids.io.mode |= BLE_CCID_IO_RX_CMD_MODE;
				
				// If no ongoing transmission
				if((m_ccids.io.mode & BLE_CCID_IO_TX_MODE) == 0) {
					ccid_send_packet(ble_ccid_buffer.buf_command_response, &ble_ccid_buffer.buf_command_response_write_idx, &ble_ccid_buffer.buf_command_response_size);
				}
			}
			break;
			
		case BLE_CCID_XFR_BLOCK_CMD_ID:
			// Receive APDU
			{
				uint32_t len;
				
				memcpy(ble_ccid_buffer.buf_command_response, p_data, p_len);
				ble_ccid_buffer.buf_command_response_read_idx = p_len - BLE_CCID_HEADER_COMMAND_SIZE;
							
				// Find number of BLE CCID packets expected to be received minus the current one.
				len = ((ble_ccid_command_t*) ble_ccid_buffer.buf_command_response)->header.length + BLE_CCID_HEADER_COMMAND_SIZE;
				ble_ccid_buffer.ble_packet_to_receive = 0;				
				while(len > BLOCK_SIZE) {
					ble_ccid_buffer.ble_packet_to_receive++;
					len -= BLOCK_SIZE;
				}

				Received_Packets_Cnt = ble_ccid_buffer.ble_packet_to_receive;	//Add by LBQ
				
				m_ccids.io.mode &= ~BLE_CCID_IO_RX_CMD_MODE;
				m_ccids.io.mode |= BLE_CCID_IO_RX_APDU_MODE;
								
				// Prepare response header in advance
				((ble_ccid_command_t*) ble_ccid_buffer.buf_command_response)->header.type = BLE_CCID_DATA_BLOCK_CMD_ID;
				((ble_ccid_command_t*) ble_ccid_buffer.buf_command_response)->header.seq_num = header->seq_num;
				
				// All BLE packet composing the long ccid command have been received.
				if(ble_ccid_buffer.ble_packet_to_receive == 0) {

					if (((ble_ccid_command_t*) ble_ccid_buffer.buf_command_response)->header.length != ble_ccid_buffer.buf_command_response_read_idx) {
						m_ccids.io.mode &= ~BLE_CCID_IO_RX_APDU_MODE;
						m_ccids.io.mode |= BLE_CCID_IO_RX_CMD_MODE;
						ccid_send_error(header, CCID_ERROR_BLK_ERR_LENGTH);
						return;
					}
					
					// APDU Case 1, auto add P3 = 0
					if(((ble_ccid_command_t*) ble_ccid_buffer.buf_command_response)->header.length == 4) {
						((ble_ccid_command_t*) ble_ccid_buffer.buf_command_response)->payload[4] = 0;
						((ble_ccid_command_t*) ble_ccid_buffer.buf_command_response)->header.length++;
					}
				
					// APDU Case 4 is not take into account, 
					// send APDU Case 3 form instead.
					if(((ble_ccid_command_t*) ble_ccid_buffer.buf_command_response)->header.length == (((ble_ccid_command_t*) ble_ccid_buffer.buf_command_response)->payload[4] + 5 + 1)) {
						// Remove Le from reception
						((ble_ccid_command_t*) ble_ccid_buffer.buf_command_response)->header.length = (((ble_ccid_command_t*) ble_ccid_buffer.buf_command_response)->payload[4] + 5);
					}
										
					m_ccids.io.mode &= ~BLE_CCID_IO_RX_APDU_MODE;
					m_ccids.io.mode |= BLE_CCID_IO_PROCESS_MODE;
					
					// TODO: Transmit an APDU to the secure element, once done call ccid_send_response(uint8_t* p_data, uint16_t len)
					//       where p_data is raw APDU RESPONSE and len is APDU RESPONSE length
					xprintf("APDU 1\r\n", 0);
					SendMsg(MSG_APDU_CMD);
					//process_apdu_cmd();
				}
			}
			break;

			case BLE_CCID_VIBRATE_CMD_ID:
				{
					// Prepare response header in advance
					((ble_ccid_command_t*) ble_ccid_buffer.buf_command_response)->header.type = BLE_CCID_DATA_BLOCK_CMD_ID;
					((ble_ccid_command_t*) ble_ccid_buffer.buf_command_response)->header.seq_num = header->seq_num;
					((ble_ccid_command_t*) ble_ccid_buffer.buf_command_response)->header.length = 0x1;
								
					m_ccids.io.mode &= ~BLE_CCID_IO_RX_CMD_MODE;
					m_ccids.io.mode |= BLE_CCID_IO_PROCESS_MODE;
					
					// TODO: Perform a  VIBRATE Action once done call ccid_send_response(uint8_t* p_data, uint16_t len)
					xprintf("Vibrate Action\r\n", 0);
					uint8 Duration  = p_data[10];
					uint8 Interval  = p_data[11];
					uint8 Count =  p_data[12];
					MotorRunTime(Count, Duration, Interval,false);
					SendMsg(MSG_VIBRATE_CMD);
				}
				break;
			case BLE_CCID_APP_BLOCK_UNBLOCK_CMD_ID:
				{
					// Prepare response header in advance
					((ble_ccid_command_t*) ble_ccid_buffer.buf_command_response)->header.type = BLE_CCID_DATA_BLOCK_CMD_ID;
					((ble_ccid_command_t*) ble_ccid_buffer.buf_command_response)->header.seq_num = header->seq_num;
					((ble_ccid_command_t*) ble_ccid_buffer.buf_command_response)->header.length = 0x1;
								
					m_ccids.io.mode &= ~BLE_CCID_IO_RX_CMD_MODE;
					m_ccids.io.mode |= BLE_CCID_IO_PROCESS_MODE;
					
					// TODO: Perform a BLOCK_AND_UNBLOCK once done call ccid_send_response(uint8_t* p_data, uint16_t len)
					xprintf("BLOCK AND UNBLOCK\r\n", 0);
				  	bCommand = header->status ;
					SendMsg(MSG_APPLICATION_BLOCK_UNBLOCK);

				}
				break;

			case BLE_CCID_READ_TRANSACTION_LOG_CMD_ID:
				{
					// Prepare response header in advance
					((ble_ccid_command_t*) ble_ccid_buffer.buf_command_response)->header.type = BLE_CCID_DATA_BLOCK_CMD_ID;
					((ble_ccid_command_t*) ble_ccid_buffer.buf_command_response)->header.seq_num = header->seq_num;
								
					m_ccids.io.mode &= ~BLE_CCID_IO_RX_CMD_MODE;
					m_ccids.io.mode |= BLE_CCID_IO_PROCESS_MODE;
					
					// TODO: Perform a  READ TRANSACTION LOG  once done call ccid_send_response(uint8_t* p_data, uint16_t len)
					xprintf("READ TRANSACTION LOG\r\n", 0);

				}
				break;
				
			case BLE_CCID_GET_BOOSTER_PARAMETERS:
				cmd_type = BLE_CCID_GET_BOOSTER_PARAMETERS;
				{
					uint32_t len;
					
					memcpy(ble_ccid_buffer.buf_command_response, p_data, p_len);
					ble_ccid_buffer.buf_command_response_read_idx = p_len - BLE_CCID_HEADER_COMMAND_SIZE;
								
					// Find number of BLE CCID packets expected to be received minus the current one.
					len = ((ble_ccid_command_t*) ble_ccid_buffer.buf_command_response)->header.length + BLE_CCID_HEADER_COMMAND_SIZE;
					ble_ccid_buffer.ble_packet_to_receive = 0;				
					while(len > BLOCK_SIZE) {
						ble_ccid_buffer.ble_packet_to_receive++;
						len -= BLOCK_SIZE;
					}
				
					Received_Packets_Cnt = ble_ccid_buffer.ble_packet_to_receive; //Add by LBQ
					
					m_ccids.io.mode &= ~BLE_CCID_IO_RX_CMD_MODE;
					m_ccids.io.mode |= BLE_CCID_IO_RX_SPI_MODE;
									
					// Prepare response header in advance
					((ble_ccid_command_t*) ble_ccid_buffer.buf_command_response)->header.type = BLE_CCID_DATA_BLOCK_CMD_ID;
					((ble_ccid_command_t*) ble_ccid_buffer.buf_command_response)->header.seq_num = header->seq_num;
					
					// All BLE packet composing the long ccid command have been received.
					if(ble_ccid_buffer.ble_packet_to_receive == 0) {
				
						if (((ble_ccid_command_t*) ble_ccid_buffer.buf_command_response)->header.length != ble_ccid_buffer.buf_command_response_read_idx) {
							m_ccids.io.mode &= ~BLE_CCID_IO_RX_SPI_MODE;
							m_ccids.io.mode |= BLE_CCID_IO_RX_CMD_MODE;
							ccid_send_error(header, CCID_ERROR_BLK_ERR_LENGTH);
							return;
						}
											
						m_ccids.io.mode &= ~BLE_CCID_IO_RX_SPI_MODE;
						m_ccids.io.mode |= BLE_CCID_IO_PROCESS_MODE;
						
						// TODO: Transmit an APDU to the secure element, once done call ccid_send_response(uint8_t* p_data, uint16_t len)
						//		 where p_data is raw APDU RESPONSE and len is APDU RESPONSE length
						xprintf("GET BOOSTER 1\r\n", 0);
						SendMsg(MSG_GET_BOOSTER_PARAMETERS);
						//process_apdu_cmd();
					}
				}
				break;
			
			
			case BLE_CCID_SET_BOOSTER_PARAMETERS:
				cmd_type = BLE_CCID_SET_BOOSTER_PARAMETERS;
				{
					uint32_t len;
					
					memcpy(ble_ccid_buffer.buf_command_response, p_data, p_len);
					ble_ccid_buffer.buf_command_response_read_idx = p_len - BLE_CCID_HEADER_COMMAND_SIZE;
								
					// Find number of BLE CCID packets expected to be received minus the current one.
					len = ((ble_ccid_command_t*) ble_ccid_buffer.buf_command_response)->header.length + BLE_CCID_HEADER_COMMAND_SIZE;
					ble_ccid_buffer.ble_packet_to_receive = 0;				
					while(len > BLOCK_SIZE) {
						ble_ccid_buffer.ble_packet_to_receive++;
						len -= BLOCK_SIZE;
					}
				
					Received_Packets_Cnt = ble_ccid_buffer.ble_packet_to_receive; //Add by LBQ
					
					m_ccids.io.mode &= ~BLE_CCID_IO_RX_CMD_MODE;
					m_ccids.io.mode |= BLE_CCID_IO_RX_SPI_MODE;
									
					// Prepare response header in advance
					((ble_ccid_command_t*) ble_ccid_buffer.buf_command_response)->header.type = BLE_CCID_DATA_BLOCK_CMD_ID;
					((ble_ccid_command_t*) ble_ccid_buffer.buf_command_response)->header.seq_num = header->seq_num;
					
					// All BLE packet composing the long ccid command have been received.
					if(ble_ccid_buffer.ble_packet_to_receive == 0) {
				
						if (((ble_ccid_command_t*) ble_ccid_buffer.buf_command_response)->header.length != ble_ccid_buffer.buf_command_response_read_idx) {
							m_ccids.io.mode &= ~BLE_CCID_IO_RX_SPI_MODE;
							m_ccids.io.mode |= BLE_CCID_IO_RX_CMD_MODE;
							ccid_send_error(header, CCID_ERROR_BLK_ERR_LENGTH);
							return;
						}
						
											
						m_ccids.io.mode &= ~BLE_CCID_IO_RX_SPI_MODE;
						m_ccids.io.mode |= BLE_CCID_IO_PROCESS_MODE;
						
						// TODO: Transmit an APDU to the secure element, once done call ccid_send_response(uint8_t* p_data, uint16_t len)
						//		 where p_data is raw APDU RESPONSE and len is APDU RESPONSE length
						xprintf("SET BOOSTER 1\r\n", 0);
						SendMsg(MSG_SET_BOOSTER_PARAMETERS);
						//process_apdu_cmd();
					}
				}
				break;

			case BLE_CCID_ACTIVE_NFC_CMD_ID:
				{
					// Prepare response header in advance
					((ble_ccid_command_t*) ble_ccid_buffer.buf_command_response)->header.type = BLE_CCID_DATA_BLOCK_CMD_ID;
					((ble_ccid_command_t*) ble_ccid_buffer.buf_command_response)->header.seq_num = header->seq_num;
					((ble_ccid_command_t*) ble_ccid_buffer.buf_command_response)->header.length = 0x1;
								
					m_ccids.io.mode &= ~BLE_CCID_IO_RX_CMD_MODE;
					m_ccids.io.mode |= BLE_CCID_IO_PROCESS_MODE;
					
					xprintf("ACTIVE NFC\r\n", 0);
					if ((p_data[10] == 1) || (p_data[10] == 2))
						NFC_STATUS	= p_data[10];
					SendMsg(MSG_ACTIVE_NFC);
				}
				break;				

	}
}

/**
 * @brief Function for handling the Connect event.
 *
 * @param[in]   p_ble_evt   Event received from the BLE stack.
 */
static void on_connect(ble_evt_t* p_ble_evt) {
    m_ccids.conn_handle = p_ble_evt->evt.gap_evt.conn_handle;
	m_ccids.is_notification_enabled = 0;
	m_ccids.io.mode = BLE_CCID_IO_RX_CMD_MODE;
	
	ble_ccid_buffer.buf_notification_write_idx = 0;
	ble_ccid_buffer.buf_notification_size = 0;
	ble_ccid_buffer.buf_command_response_read_idx = 0;
	ble_ccid_buffer.buf_command_response_write_idx = 0;
	ble_ccid_buffer.buf_command_response_size = 0;
	ble_ccid_buffer.ble_packet_to_receive = 0;
	
	m_ccid_ctx.first_cmd = 1;
	m_ccid_ctx.last_seq_num = 0;
	m_ccid_ctx.status = CCID_SE_PRESENT_AND_INACTIVE;
}

/**
 * @brief Function for handling the Disconnect event.
 *
 * @param[in]   p_ble_evt   Event received from the BLE stack.
 */
static void on_disconnect(ble_evt_t* p_ble_evt) {
    UNUSED_PARAMETER(p_ble_evt);
    m_ccids.conn_handle = BLE_CONN_HANDLE_INVALID;
	m_ccids.is_notification_enabled = 0;
}

/**
 * @brief Function for handling the write events.
 *
 * @details Handles all write events from the BLE stack of interest to the CCID Service.
 *
 * @param[in]   p_ble_evt  Event received from the BLE stack.
 */
static void on_write(ble_evt_t* p_ble_evt) {
    ble_gatts_evt_write_t* p_evt_write;

	p_evt_write = &p_ble_evt->evt.gatts_evt.params.write;
	
	#if DEBUG
	if((p_evt_write->handle == m_ccids.rx_handles.value_handle) || (p_evt_write->handle == m_ccids.tx_handles.cccd_handle)) {
		xprintf("[BLE][CCID] <: ", 0);
		{
			uint16_t i;
			for(i=0; i<p_evt_write->len; i++) {
				if(i>0) {
					xprintf(" ", 0);
					if((i % 10) == 0) {
						xprintf("\r\n               ",0);
					}
				}
				//xprintf("%02X", p_evt_write->data[i]);
				xprintf(&p_evt_write->data[i], 1);
				xprintf(" ", 0);
			}
		}
		xprintf("\r\n", 0);		
		//usart_flush();
	}
	#endif
	
	// Handle CCID command
	if(p_evt_write->handle == m_ccids.rx_handles.value_handle) {
		// If expecting to receive new CCID command.
		if((m_ccids.io.mode & BLE_CCID_IO_RX_CMD_MODE) > 0) {
			if(p_evt_write->len >= sizeof(ble_ccid_command_header_t)) {
				process_ccid_command(p_evt_write->data, p_evt_write->len);
				m_ccid_ctx.first_cmd = 0;
			}
		}
		
		// Else if expecting to receive part of APDU command.
		else if((m_ccids.io.mode & BLE_CCID_IO_RX_APDU_MODE) > 0) {
			memcpy(&((ble_ccid_command_t*) ble_ccid_buffer.buf_command_response)->payload[ble_ccid_buffer.buf_command_response_read_idx], p_evt_write->data, p_evt_write->len);
						
			ble_ccid_buffer.buf_command_response_read_idx += p_evt_write->len;
			ble_ccid_buffer.ble_packet_to_receive--; // one BLE CCID packet less to received.
			
			// All BLE packet composing the long ccid command have been received.
			if(ble_ccid_buffer.ble_packet_to_receive == 0) {
				if (((ble_ccid_command_t*) ble_ccid_buffer.buf_command_response)->header.length != ble_ccid_buffer.buf_command_response_read_idx) {
					m_ccids.io.mode &= ~BLE_CCID_IO_RX_APDU_MODE;
					m_ccids.io.mode |= BLE_CCID_IO_RX_CMD_MODE;
					ccid_send_error( (ble_ccid_command_header_t *)&ble_ccid_buffer.buf_command_response[0], CCID_ERROR_BLK_ERR_LENGTH);
					return;
				}
				
				// APDU Case 4 is not take into account, 
				// send APDU Case 3 form instead.
				if(((ble_ccid_command_t*) ble_ccid_buffer.buf_command_response)->header.length == (((ble_ccid_command_t*) ble_ccid_buffer.buf_command_response)->payload[4] + 5 + 1)) {
					// Remove Le from reception
					((ble_ccid_command_t*) ble_ccid_buffer.buf_command_response)->header.length = (((ble_ccid_command_t*) ble_ccid_buffer.buf_command_response)->payload[4] + 5);
				}
				
				m_ccids.io.mode &= ~BLE_CCID_IO_RX_APDU_MODE;
				m_ccids.io.mode |= BLE_CCID_IO_PROCESS_MODE;
				
				// TODO: Transmit an APDU to the secure element, once done call ccid_send_response(uint8_t* p_data, uint16_t len)
				//       where p_data is raw APDU RESPONSE and len is APDU RESPONSE length
				xprintf("APDU 2\r\n", 0);
				SendMsg(MSG_APDU_CMD);
			}
		}
		

		// Else if expecting to receive part of SPI BOOSTER command.
		else if((m_ccids.io.mode & BLE_CCID_IO_RX_SPI_MODE) > 0) {
			memcpy(&((ble_ccid_command_t*) ble_ccid_buffer.buf_command_response)->payload[ble_ccid_buffer.buf_command_response_read_idx], p_evt_write->data, p_evt_write->len);
						
			ble_ccid_buffer.buf_command_response_read_idx += p_evt_write->len;
			ble_ccid_buffer.ble_packet_to_receive--; // one BLE CCID packet less to received.
			
			// All BLE packet composing the long ccid command have been received.
			if(ble_ccid_buffer.ble_packet_to_receive == 0) {
				if (((ble_ccid_command_t*) ble_ccid_buffer.buf_command_response)->header.length != ble_ccid_buffer.buf_command_response_read_idx) {
					m_ccids.io.mode &= ~BLE_CCID_IO_RX_SPI_MODE;
					m_ccids.io.mode |= BLE_CCID_IO_RX_CMD_MODE;
					ccid_send_error( (ble_ccid_command_header_t *)&ble_ccid_buffer.buf_command_response[0], CCID_ERROR_BLK_ERR_LENGTH);
					return;
				}
				
				m_ccids.io.mode &= ~BLE_CCID_IO_RX_SPI_MODE;
				m_ccids.io.mode |= BLE_CCID_IO_PROCESS_MODE;
				
				// TODO: Transmit an APDU to the secure element, once done call ccid_send_response(uint8_t* p_data, uint16_t len)
				//       where p_data is raw APDU RESPONSE and len is APDU RESPONSE length
				if (cmd_type == BLE_CCID_SET_BOOSTER_PARAMETERS) {
					xprintf("SET BOOSTER 2\r\n", 0);
					SendMsg(MSG_SET_BOOSTER_PARAMETERS);
				}
				else if (cmd_type == BLE_CCID_GET_BOOSTER_PARAMETERS) {
					xprintf("GET BOOSTER 2\r\n", 0);
					SendMsg(MSG_GET_BOOSTER_PARAMETERS);
				}	

			}
		}

		// Else currently processing a command
		else if((m_ccids.io.mode & BLE_CCID_IO_PROCESS_MODE) > 0) {
			ccid_send_error((ble_ccid_command_header_t*) p_evt_write->data, CCID_ERROR_CMD_SLOT_BUSY);
		}
	}
	
	// Handle descriptor changes.
	else if(p_evt_write->handle == m_ccids.tx_handles.cccd_handle) {
		if(ble_srv_is_notification_enabled(p_evt_write->data)) {
			m_ccids.is_notification_enabled = 1;
			notify_slot_change();
		}
		else {
			m_ccids.is_notification_enabled = 0;
		}
	}
}

static void on_send_complete(ble_evt_t* p_ble_evt) {
	ble_gatts_evt_write_t* p_evt_write;

	p_evt_write = &p_ble_evt->evt.gatts_evt.params.write;
	
	#if DEBUG
	//xprintf("[BLE][HANDLE] : %02X\r\n", p_evt_write->handle);
	//usart_flush();
	xprintf("[BLE][HANDLE] : ", 0);
	xprintf((uint8_t *)&p_evt_write->handle, 1);
	xprintf("\r\n", 0);
	#endif

	if((p_evt_write->handle == m_ccids.rx_handles.value_handle) || (p_evt_write->handle == m_ccids.tx_handles.cccd_handle)) {
		// Continue to send if there is some available packets.

		if(ble_ccid_buffer.buf_command_response_size > 0) {
			ccid_send_packet(ble_ccid_buffer.buf_command_response, &ble_ccid_buffer.buf_command_response_write_idx, &ble_ccid_buffer.buf_command_response_size);
		}
		else {
			ble_ccid_buffer.buf_command_response_write_idx = 0;
			// Any pending notification to send
			if(ble_ccid_buffer.buf_notification_size > 0) {
				ccid_send_packet(ble_ccid_buffer.buf_notification, &ble_ccid_buffer.buf_notification_write_idx, &ble_ccid_buffer.buf_notification_size);
			}
			else {
				ble_ccid_buffer.buf_notification_write_idx = 0;
				m_ccids.io.mode &= ~BLE_CCID_IO_TX_MODE;
			}
		}
	}
}

/**
 * @brief Function for handling the Application's BLE Stack events.
 *
 * @details Handles all events from the BLE stack of interest to the CCID Service.
 *
 * @param[in]   p_ble_evt  Event received from the BLE stack.
 */
void ble_ccids_on_ble_evt(ble_evt_t* p_ble_evt) {
	switch(p_ble_evt->header.evt_id) {
		// 0x10
        case BLE_GAP_EVT_CONNECTED:
            on_connect(p_ble_evt);
            break;

		// 0x11
        case BLE_GAP_EVT_DISCONNECTED:
            on_disconnect(p_ble_evt);
            break;

		// 0x50
        case BLE_GATTS_EVT_WRITE:
            on_write(p_ble_evt);
            break;
		
		// 0x01
		case BLE_EVT_TX_COMPLETE:		
			on_send_complete(p_ble_evt);
			break;
    }
}

/**@brief Function for adding the RX Channel characteristic.
 *
 * @param[in]   p_ccids  CCID Service structure.
 *
 * @return      NRF_SUCCESS on success, otherwise an error code.
 */
static uint32_t rx_channel_char_add(ble_ccids_t* p_ccids) {
	uint32_t             err_code;
	ble_uuid_t           ble_uuid;
    ble_gatts_attr_md_t  attr_md;
	ble_gatts_char_md_t  char_md;    
	ble_gatts_attr_t     attr_char_value;    

	// Add custom base UUID
    err_code = sd_ble_uuid_vs_add(&BLE_UUID_CCID_RX_CHANNEL_CHARACTERISTIC, &(ble_uuid.type));
    if (err_code != NRF_SUCCESS) {
        return err_code;
    }
	GET_BLE_UUID_FROM_UUID128(ble_uuid.uuid, BLE_UUID_CCID_RX_CHANNEL_CHARACTERISTIC.uuid128);
	
	// Attribute metadata
    memset(&attr_md, 0, sizeof(attr_md));
    BLE_GAP_CONN_SEC_MODE_SET_NO_ACCESS(&attr_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.write_perm);
    attr_md.vloc = BLE_GATTS_VLOC_STACK;
	
	// GATT Characteristic metadata
    memset(&char_md, 0, sizeof(char_md));
    char_md.char_props.write_wo_resp = 1;
    char_md.p_char_user_desc = NULL;
    char_md.p_char_pf = NULL;
    char_md.p_user_desc_md = NULL;
    char_md.p_cccd_md = NULL;
    char_md.p_sccd_md = NULL;   

	// GATT Attribute
    memset(&attr_char_value, 0, sizeof(attr_char_value));
    attr_char_value.p_uuid = &ble_uuid;
    attr_char_value.p_attr_md = &attr_md;
	attr_char_value.init_len  = 0;
    attr_char_value.init_offs = 0;
    attr_char_value.max_len   = sizeof(rx_buffer);
    attr_char_value.p_value   = rx_buffer;

    return sd_ble_gatts_characteristic_add(p_ccids->service_handle, &char_md, &attr_char_value, &(p_ccids->rx_handles));
}

/**@brief Function for adding the TX Channel characteristic.
 *
 * @param[in]   p_ccids  CCID Service structure.
 *
 * @return      NRF_SUCCESS on success, otherwise an error code.
 */
static uint32_t tx_channel_char_add(ble_ccids_t* p_ccids) {
	uint32_t             err_code;
	ble_uuid_t           ble_uuid;
	ble_gatts_char_md_t  char_md;
	ble_gatts_attr_md_t  cccd_md;
    ble_gatts_attr_md_t  attr_md;
	ble_gatts_attr_t     attr_char_value;    

	// Add custom base UUID
    err_code = sd_ble_uuid_vs_add(&BLE_UUID_CCID_TX_CHANNEL_CHARACTERISTIC, &(ble_uuid.type));
    if (err_code != NRF_SUCCESS) {
        return err_code;
    }
	GET_BLE_UUID_FROM_UUID128(ble_uuid.uuid, BLE_UUID_CCID_TX_CHANNEL_CHARACTERISTIC.uuid128);
	
	// Attribute metadata
	memset(&cccd_md, 0, sizeof(cccd_md));
	BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.read_perm);
	BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.write_perm);
	cccd_md.vloc = BLE_GATTS_VLOC_STACK;
	
    memset(&attr_md, 0, sizeof(attr_md));
    BLE_GAP_CONN_SEC_MODE_SET_NO_ACCESS(&attr_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_NO_ACCESS(&attr_md.write_perm);
    attr_md.vloc = BLE_GATTS_VLOC_STACK;
	attr_md.vlen = 1;
	
    // GATT Characteristic metadata
    memset(&char_md, 0, sizeof(char_md));
	char_md.char_props.notify = 1;
    char_md.p_char_user_desc = NULL;
    char_md.p_char_pf = NULL;
    char_md.p_user_desc_md = NULL;
    char_md.p_cccd_md = &cccd_md;
    char_md.p_sccd_md = NULL;

	// GATT Attribute
    memset(&attr_char_value, 0, sizeof(attr_char_value));
    attr_char_value.p_uuid    = &ble_uuid;
    attr_char_value.p_attr_md = &attr_md;
    attr_char_value.init_len  = 0;
    attr_char_value.init_offs = 0;
    attr_char_value.max_len   = sizeof(tx_buffer);
    attr_char_value.p_value   = tx_buffer;

    return sd_ble_gatts_characteristic_add(p_ccids->service_handle, &char_md, &attr_char_value, &(p_ccids->tx_handles));
}

 /**
 * @brief Function for initializing the CCID Service.
 *
 * @return      NRF_SUCCESS on successful initialization of service, otherwise an error code.
 */
uint32_t ble_ccids_init() {
	uint32_t    err_code;
    ble_uuid_t  ble_uuid;
	
	// Initiliaze internal state machine
	m_ccids.is_notification_enabled = 0;
	
	// Initialize internal buffer
	ble_ccid_buffer.buf_notification_size = 0;
	ble_ccid_buffer.buf_command_response_read_idx = 0;
	ble_ccid_buffer.buf_command_response_write_idx = 0;
	ble_ccid_buffer.buf_command_response_size = 0;
	ble_ccid_buffer.ble_packet_to_receive = 0;

    // Initialize service structure
    m_ccids.conn_handle = BLE_CONN_HANDLE_INVALID;
		
	// Add custom base UUID
    err_code = sd_ble_uuid_vs_add(&BLE_UUID_CCID_SERVICE, &(ble_uuid.type));
    if (err_code != NRF_SUCCESS) {
        return err_code;
    }
	// Add service
    GET_BLE_UUID_FROM_UUID128(ble_uuid.uuid, BLE_UUID_CCID_SERVICE.uuid128);
	err_code = sd_ble_gatts_service_add(BLE_GATTS_SRVC_TYPE_PRIMARY, &ble_uuid, &(m_ccids.service_handle));
    if (err_code != NRF_SUCCESS) {
        return err_code;
    }
	
	// Add characteristics
	err_code = tx_channel_char_add(&m_ccids);
    if (err_code != NRF_SUCCESS) {
        return err_code;
    }
	
	err_code = rx_channel_char_add(&m_ccids);
    if (err_code != NRF_SUCCESS) {
        return err_code;
    }

	return NRF_SUCCESS;
}
