#include "Typedef.h"







extern bool BondSuccessfulState;
extern bool FilterDisconnectDisplay;
extern bool StandbyTimeModifyEnable;
extern uint8 StandbyTimeCounter;
extern bool Update_Pay_Info;
extern bool IsGoingToPowerDown;
extern uint8 CurrentMenuIdx;
extern HandlerId CurrentMenuIParam;
extern uint8 BleBondSaveNotice; 
extern uint8 gTimeDisMode;

typedef enum 
{
	MENU_STAY=0x00,
	MENU_FORWARD,
	MENU_BACKWARD,
	MENU_SLEEP_FORWARD,
	MENU_SLEEP_BACKWARD,
	MENU_JUMP_SPORT_HANDLER,
	MENU_JUMP_TIME_HANDLER,
}MENU_DIRECTION;


#define BLE_BOND_SKIP_SMS				0x01
#define BLE_BOND_SKIP_MISSED_CALL		0x02
#define BLE_BOND_SKIP_EMAIL			0x04
#define BLE_BOND_SKIP_SCHEDULE		0x08
#define BLE_BOND_SKIP_SOCIAL			0x10

typedef struct 
{
        int               MyId;  
	 int		      TimeHandlerId;		
} StandbyStateTAG;

extern StandbyStateTAG StandbyState;

void SetSystemSuspendTime(uint8 time);
uint8 GetSystemSuspendTime(void);
uint16 Standby_Handler(MsgType msg, int iParam, void *pContext);
uint16 Time_Handler(MsgType msg, int iParam, void *pContext);
void LoadMenuHandler(MENU_DIRECTION Md);
bool isSystemSupend(void);

