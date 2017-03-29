/******************************************************************************
 * Copyright Gemalto, unpublished work, created 2014. This computer program
 * includes Confidential, Proprietary Information and is a Trade Secret of
 * Gemalto. All use, disclosure, and/or reproduction is prohibited unless
 * authorised in writing by an officer of Gemalto. All Rights Reserved.
 *****************************************************************************/

/**
 * @file	ble_ccid.h
 * @author	Aurelien Couvert
 *
 * @brief 	Device Information Service module.
 *
 * @details This module implements the CCID Service.
 *          During initialization it adds the CCID Service to the BLE stack database.
 *          It then encodes the supplied information, and adds the corresponding characteristics.
 */
 
#ifndef BLE_CCIDS_H__
#define BLE_CCIDS_H__

#include <stdint.h>

#include "ble.h"
#include "ble_srv_common.h"

#define BLE_PACKETS_SUCCED		0x0
#define BLE_PACKETS_TIMEOUT		0x1


/** 
 * @brief UUID128 CCID Service definition.
 */
static const ble_uuid128_t BLE_UUID_CCID_SERVICE = { 0x81, 0xDC, 0xB1, 0xFE, 0x31, 0xD2, 0x13, 0xBF, 0xE3, 0x11, 0x7B, 0x62, 0x48, 0x5E, 0xE3, 0x71 };
static const uint16_t BLE_UUID16_CCID_SERVICE = 0x5E48;

typedef struct {
	uint8_t  mode;
} ble_ccids_io_t;

/**
 * @brief CCID Service structure. 
 *        This contains various status information for the service. 
 */
typedef struct ble_ccids_s {
    uint16_t                  service_handle;  // Handle of CCID Service (as provided by the BLE stack).
    ble_gatts_char_handles_t  tx_handles;      // Handles related to TX CHANNEL characteristic.
    ble_gatts_char_handles_t  rx_handles;      // Handles related to RX CHANNEL characteristic.
    uint16_t                  conn_handle;     // Handle of the current connection (as provided by the BLE stack, is BLE_CONN_HANDLE_INVALID if not in a connection).
	uint8_t	                  is_notification_enabled;
	ble_ccids_io_t            io;
} ble_ccids_t;


typedef struct log_data_s {
	uint32_t				Balance;
	uint32_t				Amount;
	uint8_t					Date[6];
	uint8_t 				Time[6];
} log_data_t;


extern log_data_t log_data;


/**
 * @brief Function for initializing the CCID Service.
 *
 * @return      NRF_SUCCESS on successful initialization of service, otherwise an error code.
 */
uint32_t ble_ccids_init(void);

/**
 * @brief Function for handling the Application's BLE Stack events.
 *
 * @details Handles all events from the BLE stack of interest to the CCID Service.
 *
 * @param[in]   p_ble_evt  Event received from the BLE stack.
 */
void ble_ccids_on_ble_evt(ble_evt_t* p_ble_evt);


void process_power_on_cmd(void);
void process_power_off_cmd(void);
void process_apdu_cmd(void);
void process_vibrate_cmd(void);
void process_app_block_unblock(void);
void process_read_transaction_log(void);
void swtich_to_idleMode(void);
uint8_t ble_wait_packet_timeout(void); 
void process_notify_acc_balance(void); 
void process_set_booster_parameters_cmd(void);
void process_get_booster_parameters_cmd(void);
void get_balance(uint32_t balance);
void process_active_nfc_cmd(void);




#endif // BLE_CCIDS_H__
