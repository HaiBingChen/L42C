#ifndef __SCANNER_PARSE_H
#define __SCANNER_PARSE_H

#include "Typedef.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
	uint8_t 		uuid[16];
	uint16_t 		manuf_id;
	uint16_t      	major;
	uint16_t      	minor;
} ble_scanner_parse;

typedef enum 
{
    SPMD_STANDBY = 0x01,
    SPMD_ADD_FRIENDS,
    SPMD_DEL_FRIENDS,
    SPMD_MOTO,
    SPMD_EXP1,
    SPMD_EXP2,
    SPMD_EXP3,
    SPMD_EXP4,
    SPMD_EXP5
}SP_CMD;

typedef enum
{
    SP_STANDBY = 0,
    SP_REQ,
    SP_RECEIVE,
    SP_RESPOND_OK,
    SP_RESPOND_FAIL
}SP_ACTION;

typedef enum 
{
    SP_FRIEND = 0,
    SP_STRANGE,
    SP_BACK,
    SP_NULL
}SP_FRIENDS;

typedef enum 
{
    SP_FORWARD = 0,
    SP_BACKWARD,
    SP_INIT
}SP_DIR;

#define FRIENDS_MAX 	10
#define SCAN_MAX		10

extern ble_scanner_parse Bsp;
extern uint8 mWatchid[5];
extern uint8 mName[13];
extern uint8 rWatchid[5];
extern uint8 sWatchid[5];
extern MsgType mScannerMsg;
extern uint8 mFriends[FRIENDS_MAX][6];
extern uint8 device_exp;
extern uint8 device_exp_count;
extern uint8 exp_count;
extern uint8 device_exp_rsp;
extern uint8 device_moto_count;
extern uint8 moto_count;

void Scanner_Parse_init(void);
void StopDisplay(void);
void CleanScanerBuf(void);
SP_FRIENDS DisplayRname(void);
SP_FRIENDS DisplayScanName(SP_DIR dir);
bool AddToFriendsBuff(uint8_t* wid);
bool DelFromScanBuff(uint8_t* wid);
void DelFromFriendsBuff(uint8_t* wid);
void SP_Parse(uint8_t* uuid, uint16_t major, uint16_t minor);
void SP_Generate(uint8* wid, uint8*name, uint16 cmd, SP_ACTION action, uint8 len);

#ifdef __cplusplus
}
#endif

#endif



