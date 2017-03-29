#include "Typedef.h"
#include "AppHandler.h"

enum DeviceMode_t
{
	DeviceIdleMode,
	DeviceAdvMode,
	DeviceScanMode,
};

enum DeviceStatusMode_t
{
	DeviceOffStatus,
	DeviceOnStatus,

};

enum DeviceStageMode_t
{
	DeviceIdleStatus,
	DeviceAdvStopStatus,
	DeviceAdvStartStatus,
	DeviceScanStopStatus,
	DeviceScanStartStatus,	
};

enum DeviceSearchType_t
{
	DeviceHasSearch=1,
	DeviceNull,
	DeviceFriend,
	DeviceStrange,
	DeviceBack,
};

typedef enum 
{
	Dev_Init=0,
	Dev_Init_Delay_6000ms,
	Dev_AddFriend,
	Dev_AddFriend_Rec,
	Dev_AddFriend_Accept,
	Dev_AddFriend_Reject,
	Dev_Moto,
	Dev_Moto_Accept,
	Dev_Emotion,
	Dev_Emotion_Accept,
	Dev_DelFriend,
	Dev_DelFriend_Accept
}DeviceAdvertise_t;

typedef struct 
{
	int			   Id;
	int 			   DevicesSearchHandlerId;
	int 			   DevicesAddFriendHandlerId;	
	int 			   DevicesReceiveAddFriendId;
	int 			   DevicesAddFriendRspHandlerId;	
	int 			   SendEmotionHandlerId;
	int 			   ReceiveMoToHandlerId;	
	int 			   EmotionChooseHandlerId;
	int 			   ReceiveEXPHandlerId;
	int 			   DevicesDelHandlerId;
	int			   ReceiveDelHandlerId;

} DevicesComHadlerTAG;

typedef struct 
{
	uint8 mode;
	uint8 on_off;
	uint8 stage;	
} DevicesCom_t;


extern DevicesCom_t DevicesCom;
extern uint8 DevAddFriendRspFlag;
extern DeviceAdvertise_t NextDevAdv;
void  ChangeDeviceAdvertise(DeviceAdvertise_t DevA);
uint16 DevicesCom_Handler(MsgType msg, int iParam, void *pContext);
uint16 DevicesSearchHandler(MsgType msg, int iParam, void *pContext);
uint16 DevicesAddFriendHandler(MsgType msg, int iParam, void *pContext);
uint16 DevicesReceiveAddFriend_Handler(MsgType msg, int iParam, void *pContext);
uint16 DevicesReceiveAddFriendRsp_Handler(MsgType msg, int iParam, void *pContext);
uint16 SendEmotionHandler(MsgType msg, int iParam, void *pContext);
uint16 ReceiveMoToHandler(MsgType msg, int iParam, void *pContext);
uint16 EmotionChooseHandler(MsgType msg, int iParam, void *pContext);
uint16 ReceiveEXPHandler(MsgType msg, int iParam, void *pContext);
uint16 DevicesDelHandler(MsgType msg, int iParam, void *pContext);
uint16 ReceiveDelHandler(MsgType msg, int iParam, void *pContext);


