#include "Typedef.h"
#include "AppHandler.h"

#ifndef __VolumeHandler_H
#define __VolumeHandler_H
typedef struct 
{
        int             VolumeId;  
	 int             SiriId; 
} VolumeHadlerTAG;


#define BC5_VOL_MAX   5
#define BC5_VOL_MIN   0



uint8 get_volume_level(void);
uint16 Volume_Handler(MsgType msg, int iParam, void *pContext);
uint16 Siri_Handler(MsgType msg, int iParam, void *pContext);
void LoadVolumeHandler(bool skipkey,bool AutoExit);

#endif

