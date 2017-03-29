

#ifndef BLE_MEDIA_H__
#define BLE_MEDIA_H__

#include <stdint.h>

#include "ble.h"
#include "ble_srv_common.h"
#include "Typedef.h"
#define MEDIA_BLOCK_SIZE 20
typedef struct ble_media_s {
	uint16_t                  		service_handle;  	// Handle of CCID Service (as provided by the BLE stack).
	ble_gatts_char_handles_t  	tx_handles;      // Handles related to TX CHANNEL characteristic.
	ble_gatts_char_handles_t  	rx_handles;      // Handles related to RX CHANNEL characteristic.
	uint16_t					conn_handle;   		  // Handle of the current connection (as provided by the BLE stack, is BLE_CONN_HANDLE_INVALID if not in a connection).
	uint8_t	                  		is_notification_enabled;
} ble_media_t;

typedef enum 
{
    MS_PLAY = 0,
    MS_STOP,
    MS_PREVIOUS,
    MS_NEXT,
    MS_VOL_ADD,
    MS_VOL_REDUCE,
    MS_CHECK=0xFB,
    MS_RESUME = 0xFC,
    MS_CONNECT=0xFD,
    MS_CONNECT_FAIL=0xFE,
    MS_NO = 0xFF
}MusicSet;

typedef enum 
{
    SS_START= 0,
    SS_STOP,
    SS_ERROR = 0xFF
}SearchSet;

extern uint8 MusicBuf[128];
extern MusicSet MusicStates;
extern uint8 CameraStates;
extern uint8 SearchStates;
extern uint8 MusicVol;
void ble_media_on_ble_evt(ble_evt_t* p_ble_evt); 
uint32_t ble_media_init(void);
uint32_t ble_media_string_send(uint8_t* buffer, uint16_t p_length);
void MusicSetting(MusicSet set);
void  CameraControl(uint8 Ca);
void SearchForMobilePhones(SearchSet set);
void MediaDataParse(void);
void SetSmsSenderAddr(uint32 addr);
void SendSms(uint8 SmsIdx);
void SendSms_Number(uint8 SmsIdx, uint8*Num);
#endif 
