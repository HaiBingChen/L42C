
#include <string.h>

#include "ble_media.h"
#include "nordic_common.h"
#include "ble_srv_common.h"
#include "headfile.h"

#define BLE_UUID_MEDIA_SERVICE 0x7006
#define BLE_UUID_MEDIA_TX_CHARACTERISTIC 0x8004 
#define BLE_UUID_MEDIA_RX_CHARACTERISTIC 0x8003


static ble_media_t m_medias;
static uint8_t tx_media_buf[MEDIA_BLOCK_SIZE];
static uint8_t rx_media_buf[MEDIA_BLOCK_SIZE];

uint8 MediaDataRevBuf[DATA_REV_MAX]={0x00};
uint16 MediaRevValueOffset=0;
uint16 MediaTotalDataLen =0;

uint8 MusicBuf[128]={0x00};
MusicSet MusicStates = MS_NO;
uint8 MusicVol=0;
uint8 CameraStates = 0xFF;
uint8 SearchStates =0xFF;
uint32 SmsSenderAddr=0x00;

void SetSmsSenderAddr(uint32 addr)
{
	SmsSenderAddr = addr;
}

void SendSms(uint8 SmsIdx)
{
	uint8 SendBuf[128]={0x00};
	uint8 datalen = 0,datotal=0,Spos=0;
	SendBuf[0]=0x6F;
	SendBuf[1]=0xD4;
	SendBuf[2]=0x71;
	SendBuf[4]=0x00;
	SendBuf[5]=SmsIdx;
	Force_NordicFlash();
	SpiFlash_Read(SmsSenderAddr+1,&datalen,1);
	if(datalen<100)
	{
		SpiFlash_Read(SmsSenderAddr+4,SendBuf+6,datalen);
	}
	else
		return;	
	SendBuf[3]=datalen+1;
	SendBuf[datalen+6]=0x8F;
	Free_NordicFlash();
	datotal =datalen+7;
	while(datotal>20)
	{
		ble_media_string_send(SendBuf+Spos,20);
		datotal-=20;
		Spos+=20;
	}
	ble_media_string_send(SendBuf+Spos,datotal);
}

void SendSms_Number(uint8 SmsIdx, uint8*Num)
{
	uint8 SendBuf[128]={0x00};
	uint8 datalen = 0,datotal=0,Spos=0;
	SendBuf[0]=0x6F;
	SendBuf[1]=0xD4;
	SendBuf[2]=0x71;
	SendBuf[4]=0x00;
	SendBuf[5]=SmsIdx;
	datalen = strlen((char*)Num);
	if(datalen<100)
	{
		memcpy(SendBuf+6,Num,datalen);
	}
	else
		return;	
	SendBuf[3]=datalen+1;
	SendBuf[datalen+6]=0x8F;
	datotal =datalen+7;
	while(datotal>20)
	{
		ble_media_string_send(SendBuf+Spos,20);
		datotal-=20;
		Spos+=20;
	}
	ble_media_string_send(SendBuf+Spos,datotal);
}
void MusicSetting(MusicSet set)
{
	uint8 SendBufCtl[7]={0x6F,0xD0,0x71,0x01,0x00,0x00,0x8F};
	uint8 SendBufVol[8]={0x6F,0xD0,0x71,0x02,0x00,0x04,0x00,0x8F};
	switch(set)
	{
		case MS_PLAY:
		case MS_STOP:
		case MS_PREVIOUS:
		case MS_NEXT:
			SendBufCtl[5]=(uint8)set;
			ble_media_string_send(SendBufCtl,7);
			break;
		case MS_CHECK:
			SendBufCtl[2]=0x70;
			ble_media_string_send(SendBufCtl,7);
			break;
			
		case MS_VOL_ADD:
			{
				MusicVol+=10;
				if(MusicVol>100)
					MusicVol=100;
				SendBufVol[6]=MusicVol;
				ble_media_string_send(SendBufVol,8);
			}
			break;
			
		case MS_VOL_REDUCE:
			{
				if(MusicVol>=10)
					MusicVol-=10;
				else
					MusicVol=0;
				SendBufVol[6]=MusicVol;
				ble_media_string_send(SendBufVol,8);
			}
			break;
		default:
			return;
	}
}

void  CameraControl(uint8 Ca)
{
	if(Ca==0x00)
	{
		uint8 SendBuf[7]={0x6F,0xD1,0x71,0x01,0x00,0x00,0x8F};
		ble_media_string_send(SendBuf,7);
	}
	else if(Ca==0xFF)
	{
		uint8 SendBuf[7]={0x6F,0xD1,0x71,0x01,0x00,0x02,0x8F};
		ble_media_string_send(SendBuf,7);
	}
	else if(Ca==0x03)
	{
		uint8 SendBuf[7]={0x6F,0xD1,0x71,0x01,0x00,0x03,0x8F};
		ble_media_string_send(SendBuf,7);
	}
}

void SearchForMobilePhones(SearchSet set)
{
	uint8 SendBuf[7]={0x6F,0xD2,0x71,0x01,0x00,0x00,0x8F};
	SendBuf[5] = set;
	ble_media_string_send(SendBuf,7);
}

void PhbCB(uint8 cmd,uint8 result)
{
	uint8 SendBuf[8]={0x6F,0x01,0x81,0x02,0x00,0x00,0x00,0x8F};
	SendBuf[5]=cmd;
	SendBuf[6]=result;
	ble_media_string_send(SendBuf,8);	
}
void MediaDataParse(void)
{
	uint8 DataRsp=DataParseHandler(MediaDataRevBuf); 
	if(DataRsp !=E_RSP_NULL && CurrentDataParseCmd==0xD5)
		PhbCB(CurrentDataParseCmd,(RSP_VAULE_E)DataRsp);	
	MediaTotalDataLen =0;
	MediaRevValueOffset =0;
	osal_memset(MediaDataRevBuf,0,DATA_REV_MAX);		
}
 
void MediaDataParseHandler(uint8 *p_data, uint16 length)
{	
	uint8* Data = p_data;
	if((MediaTotalDataLen==0)&&(*Data==DATA_START_CMD))
		MediaTotalDataLen = ((*(Data+4))<<8 | (*(Data+3)))+6 ; 
	if(MediaTotalDataLen>0)	
	{
		if((MediaRevValueOffset+length)  < DATA_REV_MAX)
		{
			osal_memcpy(&MediaDataRevBuf[MediaRevValueOffset], p_data, length);
			MediaRevValueOffset += length;
		}
		else
		{
			MediaTotalDataLen =0;
			MediaRevValueOffset =0;
			osal_memset(MediaDataRevBuf,0,DATA_REV_MAX);
		}
		
		if(MediaRevValueOffset>=MediaTotalDataLen)
		{		
			osal_set_event(GetClockTaskId(), CLOCK_MEDIA_DATA_PARSE); 
		}
	}
}


uint32_t ble_media_string_send(uint8_t* buffer, uint16_t p_length)
{
	uint32_t err_code;
	uint16_t len;
	ble_gatts_hvx_params_t  hvx_params;
	if((m_medias.conn_handle == BLE_CONN_HANDLE_INVALID) || (m_medias.is_notification_enabled == 0))
	{
		err_code = NRF_ERROR_INVALID_STATE;
	}
	else{
		len =  (p_length > 20) ? 20 : p_length;
		memset(&hvx_params, 0, sizeof(hvx_params));
		hvx_params.handle = m_medias.tx_handles.value_handle;
		hvx_params.type = BLE_GATT_HVX_NOTIFICATION;
		hvx_params.offset = 0;
		hvx_params.p_len = &len;
		hvx_params.p_data = &buffer[0];
		err_code = sd_ble_gatts_hvx(m_medias.conn_handle, &hvx_params);	
	}
	return err_code;
}

static void on_connect(ble_evt_t* p_ble_evt) {
	m_medias.conn_handle = p_ble_evt->evt.gap_evt.conn_handle;
	
}
static void on_disconnect(ble_evt_t* p_ble_evt) {
	UNUSED_PARAMETER(p_ble_evt);
	m_medias.conn_handle = BLE_CONN_HANDLE_INVALID;
}

static void on_write(ble_evt_t* p_ble_evt) {
	ble_gatts_evt_write_t* p_evt_write;
	p_evt_write = &p_ble_evt->evt.gatts_evt.params.write;
	
	if(p_evt_write->handle == m_medias.rx_handles.value_handle)
	{
		//UART_Send((uint8_t*)p_evt_write->data, (p_evt_write->len));
		MediaDataParseHandler(p_evt_write->data, p_evt_write->len);	
	}
	else if(p_evt_write->handle == m_medias.tx_handles.cccd_handle)
	{
		
		if(ble_srv_is_notification_enabled(p_evt_write->data)) {
			m_medias.is_notification_enabled = 1;
		}
		else {
			m_medias.is_notification_enabled = 0;
		}
	}
}

void ble_media_on_ble_evt(ble_evt_t* p_ble_evt) {
	switch(p_ble_evt->header.evt_id) {

		case BLE_GAP_EVT_CONNECTED:
			on_connect(p_ble_evt);
			break;


		case BLE_GAP_EVT_DISCONNECTED:
			on_disconnect(p_ble_evt);
			break;


		case BLE_GATTS_EVT_WRITE:
			on_write(p_ble_evt);
			break;

		//case BLE_EVT_TX_COMPLETE:		
			//on_send_complete(p_ble_evt);
			//break;

		default:
			break;
	}
}



static uint32_t tx_media_char_add(ble_media_t * p_medias)
{
	ble_uuid_t           ble_uuid;
	ble_gatts_char_md_t  char_md;
	ble_gatts_attr_md_t  cccd_md;
	ble_gatts_attr_md_t  attr_md;
	ble_gatts_attr_t     attr_char_value;    

	// Add custom base UUID
	ble_uuid.type = BLE_UUID_TYPE_BLE;
    	ble_uuid.uuid = BLE_UUID_MEDIA_TX_CHARACTERISTIC;

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
	attr_char_value.max_len   = sizeof(tx_media_buf);
	attr_char_value.p_value   = tx_media_buf;

	return sd_ble_gatts_characteristic_add(p_medias->service_handle, &char_md, &attr_char_value, &(p_medias->tx_handles));
	
}


static uint32_t rx_media_char_add(ble_media_t * p_medias)
{
	ble_uuid_t           ble_uuid;
	ble_gatts_attr_md_t  attr_md;
	ble_gatts_char_md_t  char_md;    
	ble_gatts_attr_t     attr_char_value;    

	// Add custom base UUID
	ble_uuid.type = BLE_UUID_TYPE_BLE;
    	ble_uuid.uuid = BLE_UUID_MEDIA_RX_CHARACTERISTIC;

	// Attribute metadata
	memset(&attr_md, 0, sizeof(attr_md));
	BLE_GAP_CONN_SEC_MODE_SET_NO_ACCESS(&attr_md.read_perm);
	BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.write_perm);
	attr_md.vloc = BLE_GATTS_VLOC_STACK;

	// GATT Characteristic metadata
	memset(&char_md, 0, sizeof(char_md));
	char_md.char_props.write         = 1;
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
	attr_char_value.max_len   = sizeof(rx_media_buf);
	attr_char_value.p_value   = rx_media_buf;

	return sd_ble_gatts_characteristic_add(p_medias->service_handle, &char_md, &attr_char_value, &(p_medias->rx_handles));	
}


uint32_t ble_media_init(void)
{
	uint32_t      err_code;
	ble_uuid_t    ble_uuid;

	m_medias.conn_handle = BLE_CONN_HANDLE_INVALID;
	
	BLE_UUID_BLE_ASSIGN(ble_uuid, BLE_UUID_MEDIA_SERVICE);
	err_code = sd_ble_gatts_service_add(BLE_GATTS_SRVC_TYPE_PRIMARY,
										&ble_uuid,
										&(m_medias.service_handle));
	if (err_code != NRF_SUCCESS) 
	{
		return err_code;
	}

	// Add characteristics
	err_code = rx_media_char_add(&m_medias);
	if (err_code != NRF_SUCCESS) { 
		return err_code;	
	}
	
	err_code = tx_media_char_add(&m_medias);
	
	if (err_code != NRF_SUCCESS) {
		return err_code;
	}

	return NRF_SUCCESS;
}


