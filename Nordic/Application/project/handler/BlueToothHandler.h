
#include "Typedef.h"
#include "AppHandler.h"


typedef struct 
{
        int               BTPairResultId;            
        int               BTConnectingId;
} BluetoothStateTAG;


uint16 BTPairResult_Handler(MsgType msg, int iParam, void *pContext);
uint16 BTConnecting_Handler(MsgType msg, int iParam, void *pContext);


