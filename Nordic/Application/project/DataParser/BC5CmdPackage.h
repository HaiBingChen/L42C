

/******************************************************************************************
*文件名：		NeuL18CmdPackage.h
*说明：			L18通讯协议数据解析头文件
*编写:			statdream
*版本信息:		V1.0
******************************************************************************************/

#include "Typedef.h"
//#include "Macrodef.h"


/*从缓冲中得到数据*/

#define BC5_DATA_START_CMD  0xFF
#define BC5_DATA_STOP0_CMD   0xA
#define BC5_DATA_STOP1_CMD   0xD

#define BC5_DATA_HEADER_LEN   3
#define BC5_DATA_TERMINATOR_LEN   2

#define BC5_DATA_MIN_LEN   5

#define BC5_DATA_MAX_LEN  (64)

#define BC5_CMD_NO_DATA_LEN             0
#define BC5_CMD_1BYTE_DATA_LEN        1
#define BC5_CMD_2BYTE_DATA_LEN        2

#define BC5_EVENT_1BYTE_DATA_LEN     1
#define BC5_EVENT_2BYTE_DATA_LEN     2


#define BC5_BTADDR_DATA_LEN     12

#define BC5_DATA_LEN_NOT_FIXED     0xFF

#define BC5_RSP_CN_DOT_DATA_LEN      34


typedef enum 
{
	BC5_EVENT_CMD_RSP=0x01,
	BC5_EVENT_CMD_PHONE_RSP=0x02,
	
	BC5_GET_SW_VERSION_CMD=0x10,
	BC5_GET_BT_NAME_CMD=0x11,
	BC5_GET_BT_ADDR_CMD=0x12,
	BC5_GET_MUSIC_PLAY_CMD=0x13,
	BC5_GET_VOLUME_CMD=0x14,
	BC5_GET_RING_MODE_CMD=0x15,

	BC5_GET_CN_DOT_DATA_CMD=0x30,
	
	BC5_SET_BT_PAIR_CMD=0x50,
	BC5_SET_CALL_OP_CMD=0x51,
	BC5_SET_VOLUME_CMD=0x52,
	BC5_SET_RING_MODE_CMD=0x53,
	BC5_SET_STANDBY_MODE_CMD=0x54,
	BC5_SET_STOP_RING_TONE=0x55,
	BC5_SET_SIRI_CMD=0x56,
	BC5_SET_TONES_CMD=0x57,
	BC5_SET_MIC_CMD=0x58,
	BC5_SET_DTMF_NUMBER=0x59,
	
	BC5_EVENT_BT_PAIR_STATE=0x70,
	BC5_EVENT_CALL_STATE=0x71,
	BC5_EVENT_INCOMING_CALL_DETAILS=0x72,
	BC5_EVENT_SIRI_STATE=0x73,
	
	BC5_SPP_DATA_CMD=0xA0,
}IN_BC5_CMD_E;

typedef enum 
{
      BC5_RSP_OK=0x00,
      BC5_RSP_FAIL=0x01,
      BC5_RSP_LEN_FAIL=0x02,
      BC5_RSP_NO_TERMINATOR=0x03,
      BC5_RSP_VALUE_FAIL=0x04,
      BC5_RSP_AGAINST=0x05,
      BC5_RSP_SET_FAIL=0x06
}BC5_RSP_VAULE_E;

enum 
{
	BC5_RING=0x00,
	BC5_VIBRATE=0x01
};

enum 
{
	BC5_STOP_TONE=0x00,
	BC5_START_TONE=0x01
};

enum 
{
	BC5_SIRI_CLOSE=0x00,
	BC5_SIRI_OPEN=0x01
};

enum 
{
	BC5_NORMAL_STATE=0x00,
	BC5_STANDBY_STATE=0x01
};

enum 
{
	BC5_CONNECTED_TONE=0x00,
	BC5_DISCONNECTED_TONE=0x01
};

typedef enum 
{
	BC5_PAIR_START   =0x00,
	BC5_PAIR_STOP    =0x01,
	BC5_RE_CONNECT = 0x02
}BC5_PAIRE_CONNECT;

typedef enum 
{
	BC5_CALL_REDIAL=0x00,
	BC5_CALL_ANSWER=0x01,
	BC5_CALL_REJECT=0x02,
	BC5_CALL_HANG_ACTIVE=0x03,
	BC5_CALL_REJECT_SECOND=0x04,
	BC5_CALL_ANSWER_SECOND_HANG=0x05,
	BC5_CALL_ANSWER_SECOND_HOLD=0x06,
	BC5_CALL_SWAP_ACTIVE=0x07,
	BC5_CALL_REDIAL_NUMBER=0x08,
	BC5_CALL_AGAIN=0xFF,
}BC5_CALL_OP_E;

typedef enum 
{
	BC5_CALL_STATE_HANGUP=0x00,
	BC5_CALL_STATE_FIRST_CALLING=0x01,
	BC5_CALL_STATE_FIRST_TALKING=0x02,
	BC5_CALL_STATE_TWO_CALLING=0x03,
	BC5_CALL_STATE_TWO_TALKING=0x04,
	BC5_CALL_STATE_ONE_HOLDING=0x05,
	BC5_CALL_STATE_OUTGOING=0x06,
	BC5_CALL_STATE_INVALID=0xFF,
}BC5_CALL_STATE_E;

typedef enum 
{
       L30_CALL_STATE_FIRST_CALL_INCOMING = 0x00,
       L30_CALL_STATE_ONE_CALL_TALKING = 0x01,
       L30_CALL_STATE_SECOND_CALL_INCOMING = 0x02,
       L30_CALL_STATE_TWO_CALL_TALKING = 0x03,
       L30_CALL_STATE_ONE_CALL_REMAINING = 0x04,
}L30_CALL_STATE;

typedef enum 
{
	BC5_BT_READY=0x00,
	BC5_BT_PAIR_OK=0x02,
	BC5_BT_CONNECT_OK=0x05,
	BC5_BT_DISCONNECT=0x07,
	BC5_BT_RECONNECTED=0x08,
	BC5_BT_RECONNECTED_FAIL=0x09,
	BC5_BT_POWEROFF=0xFF
}BC5_BT_STATE_E;

enum 
{
	BC5_MUSIC_ON=0x00,
	BC5_MUSIC_OFF=0x01
};

enum 
{
	BC5_RSP_NULL=0x00,
	BC5_RSP=0x01
};


#define BC5_CALL_NUMBER_LEN   20
#define BC5_CALL_NAME_LEN     20

//incoming call notify struct
typedef struct 
{
	uint8	index;
	uint8	num_len;
	uint8	name_len;
	uint8	number[BC5_CALL_NUMBER_LEN];
	uint8	name[BC5_CALL_NAME_LEN];
}BC5_INCALL_DATA_T;


#define CN_WORD_DOT_DATA_LEN   32

typedef struct 
{
	uint16	code;
	uint8	number[CN_WORD_DOT_DATA_LEN];
}BC5_CN_WORD_DOT_DATA_T;

#define BC5_VOL_MAX   5
#define BC5_VOL_MIN   0

extern uint8  BC5_State;
extern uint8 bc5_volume;
extern uint8 bc5_ring_mode;
extern uint8 bc5_music_play;
extern char CnData[2];
extern bool  SwitchCall;

extern void BC5_QueryVolumeCmd(void);
extern void BC5_SetTonesCmd(uint8 cmdData);

extern void BC5_SetBtPairCmd(uint8 cmdData);
extern void BC5_SetCallOpCmd(uint8 cmdData);
extern void BC5_SetCallRedialNumber(uint8 cmdData);
extern void BC5_SetVolumeCmd(uint8 cmdData);
extern void BC5_SetRingModeCmd(uint8 cmdData);
extern void BC5_SetStandbyModeCmd(uint8 cmdData);
extern void BC5_SetStopRingToneCmd(uint8 cmdData);
extern void BC5_SetSiri_Cmd(uint8 cmdData);
extern void BC5_SendSppData(uint8 *pData,uint8 DataLen);
extern void BC5_TO_CC2540_CallState( BC5_CALL_STATE_E state);

uint8 Bc5DataParseHandler(const void *InData);
uint8 GetBC5AudioVolCB(uint8 *pData,uint8 len);
uint8 GetBC5AudioVolCB(uint8 *pData,uint8 len);
uint8 SetBC5BtStateCB(uint8 *pData,uint8 len);
uint8 SetBC5PhoneStateCB(uint8 *pData,uint8 len);
uint8 SetBC5AudioVollCB(uint8 *pData,uint8 len);
uint8 SetBC5RingModeCB(uint8 *pData,uint8 len);
uint8 SetBC5StandbyCB(uint8 *pData,uint8 len);
uint8 SetBC5StopRingToneCB(uint8 *pData,uint8 len);
uint8 SetBC5StartSiriCB(uint8 *pData,uint8 len);
uint8 SetBC5TonesCB(uint8 *pData,uint8 len);
void BC5_SetMicCmd(uint8 cmdData);
uint8 SetBC5MicCB(uint8 *pData,uint8 len);
void BC5_SendDtmfNumber(uint8 dtmf);
uint8 SetBC5DtmfNumberCB(uint8 *pData,uint8 len);

uint8 NotifyBc5BtInfoCB(uint8 *pData,uint8 len);
uint8 NotifyBc5PhoneInfoCB(uint8 *pData,uint8 len);
uint8 NotifyBc5CallerInfoCB(uint8 *pData,uint8 len);
uint8 NotifyBc5SiriInfoCB(uint8 *pData,uint8 len);
uint8 SppTransmissionCB(uint8 *pData,uint8 len);



