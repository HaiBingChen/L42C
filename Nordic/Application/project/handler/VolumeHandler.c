#include "headfile.h"



bool VolSkipKeyMsg = false;  
bool Vol_AutoExit =false;
bool VolSkipDisplayMsg = false;  
bool SiriSkipKeyMsg = FALSE;  
bool SiriConnect = FALSE;  
VolumeHadlerTAG VolumeHadlerId;
bool VolumeHandlerExist = FALSE;

#define  VOLUME_SETTING_EXIT_TIMEOUT   5000

uint8 get_volume_level(void)
{
       uint8 levelval[6] = {0,6,9,11,13,15};
	uint8 ret = 0;	
	if(SetValue.Volume<=BC5_VOL_MAX)
	    ret = levelval[SetValue.Volume];
	return ret;
}

/*ÓÃÓÚ»¥³â £¬²»ÔÊÐíLoad¶àhandler*/
void LoadVolumeHandler(bool skipkey,bool AutoExit)
{
	if(VolumeHandlerExist==TRUE)
	{
		UnloadHandler(VolumeHadlerId.VolumeId);	
	}
	VolSkipKeyMsg= skipkey;	
	Vol_AutoExit =AutoExit;
	LoadHandler(Volume_Handler,0);  	
	if(VolSkipKeyMsg)
	{
		VolSkipDisplayMsg = true;
		osal_start_timerEx(GetAppTaskId(), MSG_VOL_SET, 200);	
	}
}

uint16 Volume_Handler(MsgType msg, int iParam, void *pContext)
{
	switch(msg)
	{
		case MSG_HANDLER_BEGIN:
			VolumeHadlerId.VolumeId = iParam;
			VolumeHandlerExist = TRUE;
			osal_start_timerEx(GetAppTaskId(), MSG_VOLUME_HANDLER_TIMEOUT, VOLUME_SETTING_EXIT_TIMEOUT);
			break;

		case MSG_HANDLER_END:
			{
			VolumeHandlerExist = FALSE;
			VolSkipKeyMsg = false; 
			uint8 TempData =SetValue.Volume; 
			WRITE_USER_PARAMETER(&TempData,SET_VALUE_LOC+1,1);
			}
			break;

		case MSG_VOL_SET:
			osal_stop_timerEx(GetAppTaskId(), MSG_VOLUME_HANDLER_TIMEOUT);	
			BC5_SetVolumeCmd(get_volume_level());
			if(VolSkipDisplayMsg)
				VolSkipDisplayMsg =false;
			else
				ShowVolumeLevel(SetValue.Volume);
			SetSystemSuspendTime(DISPLAY_TIME_10S_FLAG);	 
			osal_start_timerEx(GetAppTaskId(), MSG_VOLUME_HANDLER_TIMEOUT, VOLUME_SETTING_EXIT_TIMEOUT);
			break;

		case MSG_DISPLAY:
			ShowVolume();
			ShowVolumeLevel(SetValue.Volume);
			break;
/*ÓÃ»§²Ù×÷*/
		case MSG_LEFTDOWN_DOUBLE_SHORTKEY:		/*×ó¼ü Ë«»÷*/
		case MSG_LEFTDOWN_LONGKEY:           			/*×ó¼ü ³¤°´*/
		case MSG_RIGHTDOWN_DOUBLE_SHORTKEY:   	/*ÓÒ¼ü Ë«»÷*/	
		case MSG_RIGHTDOWN_LONGKEY:				/*ÓÒ¼ü ³¤°´*/	
		case MSG_RIGHTDOWN_LONGKEY_UP:			/*ÓÒ¼ü ³¤°´ËÉ¿ª*/		
		case MSG_TOUCH_UP_FLICK:					/*ÉÏ*/
		case MSG_TOUCH_DOWN_FLICK:				/*ÏÂ*/	
		case MSG_TOUCH_UP_DOUBLE_SHORTKEY:		/*´¥Ãþ Ë«»÷*/	
		case MSG_TOUCH_LONGKEY:					/*´¥Ãþ ³¤°´*/
		case MSG_LEFTDOWN_SHORTKEY:				/*×ó¼ü µ¥»÷*/	
			break;
	
		case MSG_RIGHTDOWN_SHORTKEY: 				/*ÓÒ¼ü µ¥»÷*/
			UnloadHandler(VolumeHadlerId.VolumeId);
			break;
#if 1
		case MSG_TOUCH_DOWN_UP_FLICK:				/*×ó*/ 
			if(!VolSkipKeyMsg)
			{
				osal_stop_timerEx(GetAppTaskId(), MSG_VOLUME_HANDLER_TIMEOUT);													
				if(SetValue.Volume <= BC5_VOL_MIN)
					SetValue.Volume = BC5_VOL_MIN;
				else
					SetValue.Volume--;
				BC5_SetVolumeCmd(get_volume_level());
				ShowVolumeLevel(SetValue.Volume);
				osal_start_timerEx(GetAppTaskId(), MSG_VOLUME_HANDLER_TIMEOUT, VOLUME_SETTING_EXIT_TIMEOUT);
			}				
			break;
				
		case MSG_TOUCH_UP_DOWN_FLICK: 			/*ÓÒ*/
			if(!VolSkipKeyMsg)
			{
				osal_stop_timerEx(GetAppTaskId(), MSG_VOLUME_HANDLER_TIMEOUT);
				if(SetValue.Volume >= BC5_VOL_MAX)
					SetValue.Volume = BC5_VOL_MAX;
				else
					SetValue.Volume++;
				BC5_SetVolumeCmd(get_volume_level());
				ShowVolumeLevel(SetValue.Volume);
				osal_start_timerEx(GetAppTaskId(), MSG_VOLUME_HANDLER_TIMEOUT, VOLUME_SETTING_EXIT_TIMEOUT);
			}				
			break;
#endif			
		case MSG_TOUCH_UP_SHORTKEY:				/*´¥Ãþ µ¥»÷*/
			{
				uint8 TouchID = Get_Screen_Locaton(3,3);
				if(TouchID==8)	//Ôö¼ÓÒôÁ¿
				{
					if(!VolSkipKeyMsg)
					{
						osal_stop_timerEx(GetAppTaskId(), MSG_VOLUME_HANDLER_TIMEOUT);
						if(SetValue.Volume >= BC5_VOL_MAX)
							SetValue.Volume = BC5_VOL_MAX;
						else
							SetValue.Volume++;
						BC5_SetVolumeCmd(get_volume_level());
						ShowVolumeLevel(SetValue.Volume);
						osal_start_timerEx(GetAppTaskId(), MSG_VOLUME_HANDLER_TIMEOUT, VOLUME_SETTING_EXIT_TIMEOUT);
					}					
				}
				else if(TouchID==6)
				{
					if(!VolSkipKeyMsg)
					{
						osal_stop_timerEx(GetAppTaskId(), MSG_VOLUME_HANDLER_TIMEOUT);													
						if(SetValue.Volume <= BC5_VOL_MIN)
							SetValue.Volume = BC5_VOL_MIN;
						else
							SetValue.Volume--;
						BC5_SetVolumeCmd(get_volume_level());
						ShowVolumeLevel(SetValue.Volume);
						osal_start_timerEx(GetAppTaskId(), MSG_VOLUME_HANDLER_TIMEOUT, VOLUME_SETTING_EXIT_TIMEOUT);
					}				
				}
			}
			break;

		case MSG_VOLUME_HANDLER_TIMEOUT:
			if(!Vol_AutoExit)
				break;
			UnloadHandler(VolumeHadlerId.VolumeId);
			break;
			
		case MSG_PHONECALLOUT_HANDLER_NORSP:
		case MSG_CALL_STATE_IDLE:
			UnloadHandler(VolumeHadlerId.VolumeId);
			SendMsg(MSG_CALL_STATE_IDLE);
			break;

		default:
			return RET_MSG_UNHANDLED;
	}
	return RET_MSG_HANDLED;
}


uint16 Siri_Handler(MsgType msg, int iParam, void *pContext)
{
	switch(msg)
	{
		case MSG_HANDLER_BEGIN:
			VolumeHadlerId.SiriId = iParam;       
			SiriSkipKeyMsg = FALSE; 
			SiriConnect = FALSE; 
			if(BC5_State!=BC5_BT_CONNECT_OK){
				SiriSkipKeyMsg = TRUE;
				osal_start_timerEx(GetAppTaskId(), MSG_SIRI_HANDLER_TIMEOUT, 2000);
			}
			break;

		case MSG_HANDLER_END:
			osal_stop_timerEx(GetAppTaskId(), MSG_SIRI_SEND_CMD_AGAIN);
			break;
			
		/*ÓÃ»§²Ù×÷*/
		case MSG_LEFTDOWN_SHORTKEY:				/*×ó¼ü µ¥»÷*/	
		case MSG_LEFTDOWN_DOUBLE_SHORTKEY:		/*×ó¼ü Ë«»÷*/
		case MSG_LEFTDOWN_LONGKEY:           			/*×ó¼ü ³¤°´*/
		case MSG_RIGHTDOWN_DOUBLE_SHORTKEY:   	/*ÓÒ¼ü Ë«»÷*/	
		case MSG_RIGHTDOWN_LONGKEY:				/*ÓÒ¼ü ³¤°´*/	
		case MSG_RIGHTDOWN_LONGKEY_UP:			/*ÓÒ¼ü ³¤°´ËÉ¿ª*/		
		case MSG_TOUCH_UP_FLICK:					/*ÉÏ*/
		case MSG_TOUCH_DOWN_FLICK:				/*ÏÂ*/
		case MSG_TOUCH_DOWN_UP_FLICK:				/*×ó*/ 
		case MSG_TOUCH_UP_DOWN_FLICK: 			/*ÓÒ*/
		case MSG_TOUCH_UP_DOUBLE_SHORTKEY:		/*´¥Ãþ Ë«»÷*/	
		case MSG_TOUCH_LONGKEY:					/*´¥Ãþ ³¤°´*/
			break;		

		case MSG_TOUCH_UP_SHORTKEY:				/*´¥Ãþ µ¥»÷*/ 
			if(SiriSkipKeyMsg == FALSE){
				BC5_SetSiri_Cmd(BC5_SIRI_OPEN);
			}
			break;

		case MSG_RIGHTDOWN_SHORTKEY: 				/*ÓÒ¼ü µ¥»÷*/
			if (SiriConnect)
				BC5_SetSiri_Cmd(BC5_SIRI_OPEN);
			else 
				SendMsg(MSG_SIRI_HANDLER_TIMEOUT);	
			break;

			
		case MSG_BC5_SIRI_OPEN:  
			if(CheckpCurrentHandler(VolumeHadlerId.SiriId)){
				SiriSkipKeyMsg = TRUE;	
				SiriConnect = TRUE;
				osal_start_timerEx(GetAppTaskId(), MSG_BC5_SIRI_DISPLAY_OPEN, 1000);
			}
			break;

		case MSG_BC5_SIRI_DISPLAY_OPEN:          
			if(CheckpCurrentHandler(VolumeHadlerId.SiriId)){
				ShowSiri(BC5_SIRI_OPEN);
			}
			SiriSkipKeyMsg = FALSE;
			break;

		case MSG_BC5_SIRI_CLOSE:
			if(CheckpCurrentHandler(VolumeHadlerId.SiriId)){
				SiriConnect=FALSE;
				ShowSiri(BC5_SIRI_CLOSE);
				SetSystemSuspendTime(DISPLAY_TIME_10S_FLAG);
			}
			break;

		case MSG_CALL_STATE_OUT_GOING:
			SendMsg(MSG_SIRI_HANDLER_TIMEOUT);
			return RET_MSG_UNHANDLED;
			
		case MSG_SIRI_DISPLAY_OPEN:
			if(CheckpCurrentHandler(VolumeHadlerId.SiriId))
				ShowSiri(BC5_SIRI_OPEN);
			break;
			
		case MSG_DISPLAY:  	       		
			SiriConnect=FALSE;
			ShowSiri(BC5_SIRI_CLOSE);
			SetSystemSuspendTime(DISPLAY_TIME_10S_FLAG);
			break;

		case MSG_SIRI_HANDLER_TIMEOUT:
			UnloadHandler(VolumeHadlerId.SiriId);
			break;

		case MSG_SIRI_SEND_CMD_AGAIN:
			BC5_SetSiri_Cmd(BC5_CALL_AGAIN);	  	
			break;	

		case MSG_SYSTEM_SUSPEND:
			if(SiriConnect==TRUE && CheckpCurrentHandler(VolumeHadlerId.SiriId))
				return RET_MSG_HANDLED;
			else
				return RET_MSG_UNHANDLED;	

		default:
			return RET_MSG_UNHANDLED;
	}
	return RET_MSG_HANDLED;
}





