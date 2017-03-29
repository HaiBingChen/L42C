
#include "Typedef.h"


  


  
#define DISPLAY_SUSPEND_EVENT                    	0x0001
#define DISPLAY_RESUME_EVENT                       	0x0002
#define CLEAR_SCREEN_EVENT          			0x0004
#define CLEAR_INIT_MARK_DISPLAY_EVENT    	0x0008
#define DISPLAY_BC5_CONNECT_STATE            	0x0010
#define SCAN_KEY_VALID_EVENT                       	0x0020
#define DISPLAY_BC5_SUSPEND_STATE            	0x0040


extern   uint8 DisplayTaskId;
extern   uint8 BledisconnectFlag;
extern   uint8 Bc5ConnectToneFlag;
extern   bool  BleReconnectFlag;


uint8 GetDisplayTaskId(void);  
void DisplayLogo(bool VibrateFlag); 
void DisplayInitTask(uint8 task_id);
uint16 DisplayEventHandler(uint8 task_id, uint16 events);
void ClearInitMarkEvent(void);
void SendSuspendEvent(void);
void SendResumeEvent(void);
void CancelSendSuspendEvent(void);
void SetScanKeyValidFlag(void);
bool IsPowerReduce(void);
void PowerStateInit(void);
void SystemSuspend(void);
void SystemResume(void);
void SystemPowerOn(void);
void SystemPowerOff(void);
void SystemFactoryModeSet(void);
void StartBc5ReconnectFlag(void);
void DelayMSecond(uint8 second);





