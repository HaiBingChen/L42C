#include "headfile.h"
#include "math.h"

DevicesComHadlerTAG  DevicesComId;
DevicesCom_t DevicesCom;

DeviceAdvertise_t NextDevAdv=Dev_Init;
uint8 DevSearFlag = 0; 			/*记录处于 好友、非好友、搜不到、BACK界面*/
uint8 DevAddFriendFlag =0; 		/*记录添加好友是否得到回应*/
uint8 DevAddFriendRspFlag = 0; 	/*记录添加好友成功或者失败*/
uint8 DevRecAddFriendFlag =0;	/*记录添加好友、拒绝*/
uint8 EmoMenuFlag=0;			/*记录振动、表情界面*/
uint8 MotoSendFlag =0;			/*记录是否点击了MOTO按钮*/
uint8 EmoIndex=0;				/*记录某一个表情*/	
uint8 EmoCount=0x00;			/*记录某一个表情发送次数*/
uint8 MOTOCount=0x00;			/*记录某一个震动发送次数*/	


uint8 DevEmoChooseFlag=0;		/*记录是否收到表情回应*/
uint8 DevDelFlag=0;				/*记录是否已点击删除好友*/
uint8 DevExp = 0;					/*记录收到的表情*/

void  ChangeDeviceAdvertise(DeviceAdvertise_t DevA)
{
	NextDevAdv = DevA;
	switch(DevA)
	{
		case Dev_Init:
			Scanner_Parse_init();
			break;	
		case Dev_Init_Delay_6000ms:	
			osal_start_timerEx(GetAppTaskId(), MSG_DELAY_ADVERTISE_INIT, 6000);
			break;
		case Dev_AddFriend:
			SP_Generate(sWatchid, mWatchid, SPMD_ADD_FRIENDS, SP_REQ, sizeof(mWatchid));
			break;
		case Dev_AddFriend_Rec:
			SP_Generate(rWatchid, mWatchid, SPMD_ADD_FRIENDS, SP_RECEIVE, sizeof(mWatchid));
			break;	
		case Dev_AddFriend_Accept:
			SP_Generate(rWatchid, mWatchid, SPMD_ADD_FRIENDS, SP_RESPOND_OK, sizeof(mWatchid));
			break;
		case Dev_AddFriend_Reject:
			SP_Generate(rWatchid, mWatchid, SPMD_ADD_FRIENDS, SP_RESPOND_FAIL, sizeof(mWatchid));
			break;	
		case Dev_Moto:
			++MOTOCount;
			SP_Generate(sWatchid, mWatchid, (uint16)(SPMD_MOTO | (MOTOCount<<8)), SP_REQ, sizeof(mWatchid));
			break;
		case Dev_Moto_Accept:
			SP_Generate(rWatchid, mWatchid, SPMD_MOTO, SP_RESPOND_OK, sizeof(mWatchid));
			break;
		case Dev_Emotion:
			++EmoCount;
			SP_Generate(sWatchid, mWatchid, (uint16)((uint16)((SPMD_EXP1 + EmoIndex))|((uint16)(EmoCount<<8))), SP_REQ, sizeof(mWatchid));
			break;
		case Dev_Emotion_Accept:
			SP_Generate(rWatchid, mWatchid, (uint16)(SPMD_EXP1+device_exp), SP_RESPOND_OK, sizeof(mWatchid));
			break;
		case Dev_DelFriend:
			SP_Generate(sWatchid, mWatchid, SPMD_DEL_FRIENDS, SP_REQ, sizeof(mWatchid));
			break;
		case Dev_DelFriend_Accept:
			SP_Generate(rWatchid, mWatchid, SPMD_DEL_FRIENDS, SP_RESPOND_OK, sizeof(mWatchid));
			break;
		default:
			break;
	}
	beacon_adv_init();	
}

/*"ON OFF"界面*/
uint16 DevicesCom_Handler(MsgType msg, int iParam, void *pContext)
{
	switch(msg)
	{
		case MSG_HANDLER_BEGIN:
			DevicesComId.Id= iParam;
			ChangeDeviceAdvertise(Dev_Init);
			SetSystemSuspendTime(DISPLAY_TIME_10S_FLAG);
			break;

		case MSG_HANDLER_END:
			DevicesComId.Id =0;		
			break;
			
		//case MSG_TOUCH_UP_DOUBLE_SHORTKEY:			/*双击*/
			//break;
			
		case MSG_TOUCH_LONGKEY:						/*长按*/	
			UnloadHandler(DevicesComId.Id);
			CurrentMenuIParam=LoadHandler(ToolHandler,0);
			return RET_MSG_HANDLED;
		
		case MSG_TOUCH_DOWN_UP_FLICK:					/*左*/ 	
			UnloadHandler(DevicesComId.Id);
			LoadHandler(BackHandler,0);
			return RET_MSG_HANDLED;
			
		case MSG_TOUCH_UP_DOWN_FLICK: 				/*右*/
			UnloadHandler(DevicesComId.Id);
			LoadHandler(TimerHandler,0);
			return RET_MSG_HANDLED;
			
		case MSG_TOUCH_UP_SHORTKEY:					/*单击*/
			if(CheckpCurrentHandler(DevicesComId.Id))
			{
				//if(Cy>=0x0A)
				{
					if(Cx<=0x30&&Cy<0x30) 				/*ON*/
					//if(Cx<=0x20) 						/*ON*/
					{
						if (DevicesCom.on_off == DeviceOffStatus) 
						{
							DevicesCom.mode = DeviceIdleMode;
							DevicesCom.stage = DeviceIdleStatus;
							osal_start_timerEx(GetAppTaskId(), MSG_DEVICESCOM_REPEAT, 100);
							DevicesCom.on_off= DeviceOnStatus;
							ShowOnBoldIcon();
							ShowOffIcon();
						}
					}
					else if(Cx>=0x40&&Cy<0x30)         		/*OFF*/
					//else if(Cx>=0x45)         					/*OFF*/
					{
						if (DevicesCom.on_off == DeviceOnStatus)
						{	
							SendMsg(MSG_SCANNER_STOP);
							SendMsg(MSG_ADVERTISER_STOP);
							osal_stop_timerEx(GetAppTaskId(), MSG_DEVICESCOM_REPEAT);
							CleanScanerBuf();
							DevicesCom.on_off= DeviceOffStatus;
							DevicesCom.stage = DeviceIdleStatus;
							DevicesCom.mode = DeviceIdleMode;
							ShowOnIcon();
							ShowOffBlodIcon();
						}
					}
					//else if(Cx>=0x45)						/*搜索*/
					else 
					{	
						if (DevicesCom.on_off == DeviceOnStatus)
						{
							UnloadHandler_WithoutDisplay(DevicesComId.Id);
							CleanScanerBuf();
							DevSearFlag = 0;
							LoadHandler(DevicesSearchHandler,0);		/*Load----放大镜搜索及结果*/
						}

					}
				}		
			}			
			return RET_MSG_HANDLED;	
			
		case MSG_DISPLAY:
			ClearScreen();
			ShowDevicesCom();
			break;
			
		default:
			return RET_MSG_UNHANDLED;	
	}
	return RET_MSG_HANDLED;
}


/*放大镜搜索及结果*/
uint16 DevicesSearchHandler(MsgType msg, int iParam, void *pContext)
{	
	switch(msg)
	{
		case MSG_HANDLER_BEGIN:
			DevicesComId.DevicesSearchHandlerId = iParam;
			SetSystemSuspendTime(DISPLAY_TIME_10S_FLAG);
			osal_start_timerEx(GetAppTaskId(), MSG_DEVICE_SEARCH_TIMEOUT, 6000+200);	
			break;

		case MSG_HANDLER_END:
			DevSearFlag = 0;
			DevicesComId.DevicesSearchHandlerId = 0;
	     		DisplayUnicode_Stop();													/*停止字库显示*/				
			osal_stop_timerEx(GetAppTaskId(), MSG_REPEAT_DEVICE_SEARCH_DISPLAY);	/*停止放大镜动画*/
			DevComClearFontBuf();															/*删除扫描列表*/		
			break;
			
		//case MSG_TOUCH_UP_DOUBLE_SHORTKEY:		/*双击*/
			//break;			
			
		case MSG_TOUCH_LONGKEY:					/*长按*/	
			if(DevSearFlag==DeviceFriend)
				LoadHandler(DevicesDelHandler,0);	/*Load----删除好友*/
			break;
			
		case MSG_GET_SCAN_NAME:					/*扫描到新设备*/
			if(DevSearFlag!=0)
				break;
			else
				DevSearFlag=DeviceHasSearch;
		case MSG_DEVICE_SEARCH_TIMEOUT:
			SetSystemSuspendTime(DISPLAY_TIME_10S_FLAG);
		case MSG_TOUCH_DOWN_UP_FLICK:				/*左*/
		case MSG_TOUCH_UP_DOWN_FLICK: 			/*右*/
			osal_stop_timerEx(GetAppTaskId(), MSG_REPEAT_DEVICE_SEARCH_DISPLAY);		/*停止放大镜动画*/
			DevSearFlag = DeviceHasSearch;
			if(CheckpCurrentHandler(DevicesComId.DevicesSearchHandlerId))
			{
				SP_FRIENDS friends = SP_NULL;
				SP_DIR spDir =SP_INIT;
				if(msg==MSG_TOUCH_DOWN_UP_FLICK)
					spDir = SP_FORWARD;
				else if(msg==MSG_TOUCH_UP_DOWN_FLICK)
					spDir = SP_BACKWARD;
				ClearScreen();	
				friends = DisplayScanName(spDir);
				switch(friends)
				{
					case SP_NULL:
						DevSearFlag = DeviceNull;
						ShowDevicesNone();
						break;

					case SP_FRIEND:
						DevSearFlag = DeviceFriend;
						ShowDevicesAdded();
						break;

					case SP_STRANGE:
						DevSearFlag = DeviceStrange;
						ShowDevicesNonAdded();
						break;
						
					case SP_BACK:
						DevSearFlag = DeviceBack;
						ShowBack();
						break;
						
					default:
						break;
				}							
			}
			break;	
			
		case MSG_TOUCH_UP_SHORTKEY:				/*单击*/	
			if ((DevSearFlag == DeviceNull) || (DevSearFlag == DeviceBack))
			{
				UnloadHandler_WithoutDisplay(DevicesComId.DevicesSearchHandlerId);		/*Load----"ON OFF"界面*/
				LoadHandler(DevicesCom_Handler,0);	 	
			}
			else if (DevSearFlag == DeviceStrange)
			{	
				LoadHandler(DevicesAddFriendHandler,0);	 								/*Load----"发送好友请求"界面*/
			}
			else if (DevSearFlag == DeviceFriend)											
			{
				LoadHandler(SendEmotionHandler,0);	 									/*Load----选择"抖动/表情"界面*/
			}
			break;

		case MSG_DISPLAY:
			if (DevSearFlag == 0)
				ShowDevicesSea();										/*显示放大镜动画*/
			else 
				HandlerMsg(MSG_DEVICE_SEARCH_TIMEOUT,0);
			break;

		case MSG_REPEAT_DEVICE_SEARCH_DISPLAY:      						/*显示放大镜动画*/
			if (CheckpCurrentHandler(DevicesComId.DevicesSearchHandlerId))
				ShowDevicesSea();
			break;

		case MSG_FONT_BUF_DISPLAY_NEXT_LETTER:							/*显示滚屏文字*/
			if(CheckpCurrentHandler(DevicesComId.DevicesSearchHandlerId))
				DisplayFontLineUnicode();	
			break;	
			
	        case MSG_SYSTEM_SUSPEND:						
	     		DisplayUnicode_Stop();													/*停止字库显示*/				
			osal_stop_timerEx(GetAppTaskId(), MSG_REPEAT_DEVICE_SEARCH_DISPLAY);	/*停止放大镜动画*/
   	 		return RET_MSG_UNHANDLED;	
			
		default:
			return RET_MSG_UNHANDLED;
	}
	return RET_MSG_HANDLED;
}


/*"发送好友请求"界面*/		
uint16 DevicesAddFriendHandler(MsgType msg, int iParam, void *pContext)
{
	switch(msg)
	{
		case MSG_HANDLER_BEGIN:
			DevicesComId.DevicesAddFriendHandlerId = iParam;
			DevAddFriendFlag = 0;
			ChangeDeviceAdvertise(Dev_AddFriend);
			SetSystemSuspendTime(DISPLAY_TIME_10S_FLAG);
			osal_start_timerEx(GetAppTaskId(), MSG_DEVICE_ADDFRIEND_DISPLAY_TIMEOUT, 8000+200);	/*8秒后失败*/		
			break;

		case MSG_HANDLER_END:
			DevAddFriendRspFlag=0;
			ChangeDeviceAdvertise(Dev_Init);
			DevicesComId.DevicesAddFriendHandlerId = 0;
			osal_stop_timerEx(GetAppTaskId(), MSG_REPEAT_DEVICE_REQUEST_DISPLAY);   	 /*停止显示好友请求动画*/
			break;
			
		//case MSG_TOUCH_UP_DOUBLE_SHORTKEY:		/*双击*/			
		case MSG_TOUCH_LONGKEY:					/*长按*/	
		case MSG_TOUCH_DOWN_UP_FLICK:				/*左*/ 
		case MSG_TOUCH_UP_DOWN_FLICK: 			/*右*/ 
		case MSG_TOUCH_UP_SHORTKEY:				/*单击*/	
			break;
			
		case MSG_GET_SCAN_ADD_FRIENDS_OK:
		case MSG_GET_SCAN_ADD_FRIENDS_FAIL:	
			UnloadHandler_WithoutDisplay(DevicesComId.DevicesAddFriendHandlerId);	
			return RET_MSG_UNHANDLED;
			
		case MSG_GET_SCAN_ADD_FRIENDS_REC:
			DevAddFriendRspFlag=0;
			if(DevAddFriendFlag!=0)
				break;
			else
				DevAddFriendFlag =1;
		case MSG_DEVICE_ADDFRIEND_DISPLAY_TIMEOUT:
			ChangeDeviceAdvertise(Dev_Init);
			osal_stop_timerEx(GetAppTaskId(), MSG_REPEAT_DEVICE_REQUEST_DISPLAY);   	 /*停止显示好友请求动画*/
			if(CheckpCurrentHandler(DevicesComId.DevicesAddFriendHandlerId))
			{
				if(DevAddFriendFlag==0)
				{
					ClearScreen();
					ShowAddDevicesRecFail();
				}
				else if(DevAddFriendFlag==1)
				{
					ClearScreen();
					ShowAddDevicesRecSuccess();			
				}
			}
			osal_start_timerEx(GetAppTaskId(), MSG_DEVICE_ADDFRIEND_TIMEOUT, 2000);
			break;
			
		case MSG_DEVICE_ADDFRIEND_TIMEOUT:
			UnloadHandler(DevicesComId.DevicesAddFriendHandlerId);	
			break;
			
		case MSG_DISPLAY:
			if(DevAddFriendFlag==0)
			{
				ShowSendRequest();
			}
			else if(DevAddFriendFlag==1)
				HandlerMsg(MSG_DEVICE_ADDFRIEND_TIMEOUT,0);
			break;
			
		case MSG_REPEAT_DEVICE_REQUEST_DISPLAY:			/*显示发送好友请求动画*/
			if(CheckpCurrentHandler(DevicesComId.DevicesAddFriendHandlerId))
				ShowSendRequest();
			break;

	        case MSG_SYSTEM_SUSPEND:
	     		osal_stop_timerEx(GetAppTaskId(), MSG_REPEAT_DEVICE_REQUEST_DISPLAY);   	 /*停止显示好友请求动画*/
   	 		return RET_MSG_UNHANDLED;
			
		default:
			return RET_MSG_UNHANDLED;
	}

	return RET_MSG_HANDLED;
}


/*收到好友请求*/
uint16 DevicesReceiveAddFriend_Handler(MsgType msg, int iParam, void *pContext)
{
	switch(msg)
	{
		case MSG_HANDLER_BEGIN:
			DevicesComId.DevicesReceiveAddFriendId= iParam;
			ChangeDeviceAdvertise(Dev_AddFriend_Rec);
			SetSystemSuspendTime(DISPLAY_TIME_10S_FLAG);
			DevRecAddFriendFlag =0;
			break;

		case MSG_HANDLER_END:
			ChangeDeviceAdvertise(Dev_Init);
			DevicesComId.DevicesReceiveAddFriendId =0;		
			break;
			
		//case MSG_TOUCH_UP_DOUBLE_SHORTKEY:		/*双击*/				
		case MSG_TOUCH_LONGKEY:					/*长按*/	
		case MSG_TOUCH_DOWN_UP_FLICK:				/*左*/ 
		case MSG_TOUCH_UP_DOWN_FLICK: 			/*右*/	
		case MSG_GET_SCAN_ADD_FRIENDS:			/*收到好友请求*/	
			break;

		case MSG_TOUCH_UP_SHORTKEY:				/*单击*/
			//if(Cy>=0x0A)
			{
				if(Cx>0x32&&Cy<0x30) 			/*接受*/
				//if(Cx>0x32) 						/*接受*/	
				{
					osal_start_timerEx(GetAppTaskId(), MSG_DEVICE_RECEIVE_ADDFRIEND_TIMEOUT, 6000);
					ChangeDeviceAdvertise(Dev_AddFriend_Accept);		
					AddToFriendsBuff(rWatchid);
					DevRecAddFriendFlag =1;
					ClearScreen();
					ShowAddDevicesSuccess();
					DisplayRname();
				}
				else if(Cx<=0x32&&Cy<0x30) 	      		/*拒绝*/
				//else if(Cx<=0x32) 	      				/*拒绝*/	
				{
					osal_start_timerEx(GetAppTaskId(), MSG_DEVICE_RECEIVE_ADDFRIEND_TIMEOUT, 6000);
					ChangeDeviceAdvertise(Dev_AddFriend_Reject);	
					DevRecAddFriendFlag =2;
					ClearScreen();
					ShowAddDevicesFail();
					DisplayRname();
				}
			}
			break;		
			
		case MSG_DISPLAY:
			ShowReceivedReq();	
			DisplayRname();
			break;

		case MSG_DEVICE_RECEIVE_ADDFRIEND_TIMEOUT:
			UnloadHandler(DevicesComId.DevicesReceiveAddFriendId);
			break;

		case MSG_FONT_BUF_DISPLAY_NEXT_LETTER:			/*显示滚屏文字*/
			if(CheckpCurrentHandler(DevicesComId.DevicesReceiveAddFriendId))
				DisplayFontLineUnicode();	
			break;	
			
	        case MSG_SYSTEM_SUSPEND:
	     		DisplayUnicode_Stop();
   	 		return RET_MSG_UNHANDLED;
			
		default:
			return RET_MSG_UNHANDLED;	
	}
	return RET_MSG_HANDLED;
}

/*收到好友请求回应*/
uint16 DevicesReceiveAddFriendRsp_Handler(MsgType msg, int iParam, void *pContext)
{
	switch(msg)
	{
		case MSG_HANDLER_BEGIN:
			DevicesComId.DevicesAddFriendRspHandlerId = iParam;
			SetSystemSuspendTime(DISPLAY_TIME_10S_FLAG);
			MotoRun(MOTO_DEFAULT);	
			break;

		case MSG_HANDLER_END:
			DevicesComId.DevicesAddFriendRspHandlerId = 0;
			break;
		case MSG_TOUCH_UP_SHORTKEY:				/*单击*/	
			break;
		//case MSG_TOUCH_UP_DOUBLE_SHORTKEY:		/*双击*/			
		case MSG_TOUCH_LONGKEY:					/*长按*/	
		case MSG_TOUCH_DOWN_UP_FLICK:				/*左*/ 
		case MSG_TOUCH_UP_DOWN_FLICK: 			/*右*/ 
		case MSG_DEVICE_ADDFRIEND_RSP_TIMEOUT:
			StopMotorRunTime();
			UnloadHandler(DevicesComId.DevicesAddFriendRspHandlerId);	
			break;

		case MSG_GET_SCAN_ADD_FRIENDS_OK:
		case MSG_GET_SCAN_ADD_FRIENDS_FAIL:
			break;
		
		case MSG_DISPLAY:
			ClearScreen();
			switch(DevAddFriendRspFlag)
			{				
				case 1:
					ShowAddDevicesSuccess();
					AddToFriendsBuff(rWatchid);
					DisplayRname();
					break;
					
				case 2:
					ShowAddDevicesFail();
					DisplayRname();
					break;
					
				default:
					SendMsg(MSG_DEVICE_ADDFRIEND_RSP_TIMEOUT);
					break;
			}		
			break;
						
		case MSG_FONT_BUF_DISPLAY_NEXT_LETTER:			/*显示滚屏文字*/
			if(CheckpCurrentHandler(DevicesComId.DevicesAddFriendRspHandlerId))
				DisplayFontLineUnicode();	
			break;	
			
	        case MSG_SYSTEM_SUSPEND:
	     		DisplayUnicode_Stop();
   	 		return RET_MSG_UNHANDLED;
			
		default:
			return RET_MSG_UNHANDLED;
	}

	return RET_MSG_HANDLED;
}


/*选择"抖动/表情"界面*/
uint16 SendEmotionHandler(MsgType msg, int iParam, void *pContext)
{
	switch(msg)
	{
		case MSG_HANDLER_BEGIN:
			DevicesComId.SendEmotionHandlerId = iParam;
			SetSystemSuspendTime(DISPLAY_TIME_10S_FLAG);	
			EmoMenuFlag =0;
			MotoSendFlag =0;
			break;
			
		case MSG_HANDLER_END:
			DevicesComId.SendEmotionHandlerId = 0;
			ChangeDeviceAdvertise(Dev_Init);
			osal_stop_timerEx(GetAppTaskId(), MSG_REPEAT_DEVICE_MOTO_DISPLAY);		/*停止发送震动动画*/
			break;
			
		//case MSG_TOUCH_UP_DOUBLE_SHORTKEY:		/*双击*/	
		case MSG_TOUCH_LONGKEY:					/*长按*/	
		case MSG_TOUCH_UP_DOWN_FLICK: 			/*右*/
			break;  
	
		case MSG_TOUCH_UP_SHORTKEY:				/*单击*/
			if (EmoMenuFlag == 0) {
				MotoSendFlag =1;
				ChangeDeviceAdvertise(Dev_Moto);
				ShowDevicesMotoRepeat();
			}
			else if(EmoMenuFlag==1){			
				ChangeDeviceAdvertise(Dev_Init);
				LoadHandler(EmotionChooseHandler,0);		/*加载表情包*/
			}
			else if(EmoMenuFlag==2){
				UnloadHandler(DevicesComId.SendEmotionHandlerId);		
			}
			break;

			
		case MSG_TOUCH_DOWN_UP_FLICK:				/*左*/
			osal_stop_timerEx(GetAppTaskId(), MSG_REPEAT_DEVICE_MOTO_DISPLAY);
			if(EmoMenuFlag==2)
				EmoMenuFlag = 0;
			else
				EmoMenuFlag++;
		case MSG_DISPLAY:
			ClearScreen();
			switch(EmoMenuFlag)
			{
				case 0:		/*震动*/
					ShowEmotionStart();
					break;
				case 1:		/*表情包*/
					ShowEmotionHomeMenu();
					break;
				case 2:		/*BACK*/
					ShowBack();
					break;
			}			
			break;
			
		case MSG_REPEAT_DEVICE_MOTO_DISPLAY:		/*显示发送震动动画*/
			if(CheckpCurrentHandler(DevicesComId.SendEmotionHandlerId))
				ShowDevicesMotoRepeat();
			break;
			
	        case MSG_SYSTEM_SUSPEND:
	     		osal_stop_timerEx(GetAppTaskId(), MSG_REPEAT_DEVICE_MOTO_DISPLAY);		/*停止发送震动动画*/
   	 		return RET_MSG_UNHANDLED;
			
		case MSG_GET_SCAN_MOTO_OK:
		case MSG_GET_SCAN_MOTO_FAIL:	
			if(MotoSendFlag==1)
			{
				MotoSendFlag=0;
				ChangeDeviceAdvertise(Dev_Init);
				osal_stop_timerEx(GetAppTaskId(), MSG_REPEAT_DEVICE_MOTO_DISPLAY);
				if(CheckpCurrentHandler(DevicesComId.SendEmotionHandlerId))
					SendMsg(MSG_DISPLAY);
			}
			break;
		default:
			return RET_MSG_UNHANDLED;

	}

	return RET_MSG_HANDLED;

}

/*收到震动消息*/
uint16 ReceiveMoToHandler(MsgType msg, int iParam, void *pContext)
{
	switch(msg)
	{
		case MSG_HANDLER_BEGIN:
			DevicesComId.ReceiveMoToHandlerId = iParam;
			SetSystemSuspendTime(DISPLAY_TIME_10S_FLAG);
			MotoRun(MOTO_SMS);
			ChangeDeviceAdvertise(Dev_Moto_Accept);
			osal_start_timerEx(GetAppTaskId(), MSG_RECEIVE_MOTO_TIMEOUT, 6000+200);
			break;
			
		case MSG_HANDLER_END:
			StopMotorRunTime();			
			ChangeDeviceAdvertise(Dev_Init_Delay_6000ms);	
			DisplayUnicode_Stop();
			DevicesComId.ReceiveMoToHandlerId = 0;
			break;
			
		case MSG_RECEIVE_MOTO_TIMEOUT:
			ChangeDeviceAdvertise(Dev_Init);	
			break;
		case MSG_TOUCH_UP_SHORTKEY:				/*单击*/	
			break;	
		//case MSG_TOUCH_UP_DOUBLE_SHORTKEY:		/*双击*/		
		//case MSG_TOUCH_UP_SHORTKEY:				/*单击*/
		case MSG_TOUCH_LONGKEY:					/*长按*/			
		case MSG_TOUCH_DOWN_UP_FLICK:				/*左*/ 	
		case MSG_TOUCH_UP_DOWN_FLICK: 			/*右*/
			UnloadHandler(DevicesComId.ReceiveMoToHandlerId);
			break;  

		case MSG_GET_SCAN_MOTO:
			if (moto_count != device_moto_count)
			{
				if(CheckpCurrentHandler(DevicesComId.ReceiveMoToHandlerId))
				{
					MotoRun(MOTO_SMS);
					SendMsg(MSG_DISPLAY);
				}
				else
				{
					UnloadHandler(DevicesComId.ReceiveMoToHandlerId);
					SendMsg(msg);
				}
			}
			break;	
			
		case MSG_DISPLAY:
			moto_count = device_moto_count;
			ShowEmotionStart();
			DisplayRname();
			break;
						
		case MSG_FONT_BUF_DISPLAY_NEXT_LETTER:			/*显示滚屏文字*/
			if(CheckpCurrentHandler(DevicesComId.ReceiveMoToHandlerId))
				DisplayFontLineUnicode();	
			break;	
			
	        case MSG_SYSTEM_SUSPEND:
	     		DisplayUnicode_Stop();
   	 		return RET_MSG_UNHANDLED;
			
		default:
			return RET_MSG_UNHANDLED;
	}

	return RET_MSG_HANDLED;

}

/*发送表情包*/
uint16 EmotionChooseHandler(MsgType msg, int iParam, void *pContext)
{
	switch(msg)
	{
		case MSG_HANDLER_BEGIN:
			DevicesComId.EmotionChooseHandlerId = iParam;
			SetSystemSuspendTime(DISPLAY_TIME_10S_FLAG);
			DevEmoChooseFlag =1;
			EmoIndex=0;
			break;
			
		case MSG_HANDLER_END:
			DevicesComId.EmotionChooseHandlerId = 0;
			ChangeDeviceAdvertise(Dev_Init);
			osal_stop_timerEx(GetAppTaskId(), MSG_REPEAT_DEVICE_EMOTION_DISPLAY);	/*停止表情发送动画*/
			break;
		
		//case MSG_TOUCH_UP_DOUBLE_SHORTKEY:		/*双击*/					
		case MSG_TOUCH_LONGKEY:					/*长按*/
		case MSG_TOUCH_UP_DOWN_FLICK: 			/*右*/
			break;
			
		case MSG_TOUCH_UP_SHORTKEY:				/*单击*/
			if(EmoIndex<5)
			{
				DevEmoChooseFlag=0;
				device_exp_rsp =0;
				ChangeDeviceAdvertise(Dev_Emotion);
				ShowEmotionIdex(EmoIndex);
				ShowSendEmotionRepeat();
				osal_start_timerEx(GetAppTaskId(), MSG_GET_SCAN_EXP_FAIL, 6000+200);
			}
			else
				UnloadHandler(DevicesComId.EmotionChooseHandlerId);			
			break;

	
		case MSG_TOUCH_DOWN_UP_FLICK:				/*左*/ 
			if(DevEmoChooseFlag==0)
				return RET_MSG_HANDLED;
			osal_stop_timerEx(GetAppTaskId(), MSG_REPEAT_DEVICE_EMOTION_DISPLAY);	
			if(EmoIndex==5)
				EmoIndex = 0;
			else
				EmoIndex++;			
		case MSG_DISPLAY:
			ClearScreen();
			ShowEmotionIdex(EmoIndex);
			if(DevEmoChooseFlag==0)
				ShowSendEmotionRepeat();
			break;

		case MSG_GET_SCAN_EXP_OK:
			if((DevEmoChooseFlag==0) && ((device_exp_rsp-1) ==EmoIndex))
			{
				osal_stop_timerEx(GetAppTaskId(), MSG_REPEAT_DEVICE_EMOTION_DISPLAY);
				if(CheckpCurrentHandler(DevicesComId.EmotionChooseHandlerId)&&(!DISPLAY_GOTO_SLEEP_FLAG))
				{	
					SetSystemSuspendTime(DISPLAY_TIME_10S_FLAG);
					ShowEmotionIdex(EmoIndex);
					ShowEmotionOK();
					osal_start_timerEx(GetAppTaskId(), MSG_DISPLAY, 2000);	
				}
				DevEmoChooseFlag=1;
				ChangeDeviceAdvertise(Dev_Init);		
			}
			break;
			
		case MSG_GET_SCAN_EXP_FAIL:	
			if(DevEmoChooseFlag==0)
			{
				osal_stop_timerEx(GetAppTaskId(), MSG_REPEAT_DEVICE_EMOTION_DISPLAY);	
				DevEmoChooseFlag=1;
				ChangeDeviceAdvertise(Dev_Init);
				SendMsg(MSG_DISPLAY);
			}
			break;
			
		case MSG_REPEAT_DEVICE_EMOTION_DISPLAY:							/*表情发送动画*/
			if(CheckpCurrentHandler(DevicesComId.EmotionChooseHandlerId))
				ShowSendEmotionRepeat();
			break;
			
	        case MSG_SYSTEM_SUSPEND:
	     		osal_stop_timerEx(GetAppTaskId(), MSG_REPEAT_DEVICE_EMOTION_DISPLAY);	/*停止表情发送动画*/
   	 		return RET_MSG_UNHANDLED;	
			
		default:
			return RET_MSG_UNHANDLED;
	}	
	return RET_MSG_HANDLED;
}

/*收到表情*/
uint16 ReceiveEXPHandler(MsgType msg, int iParam, void *pContext)
{
	switch(msg)
	{
		case MSG_HANDLER_BEGIN:
			DevicesComId.ReceiveEXPHandlerId = iParam;		
			SetSystemSuspendTime(DISPLAY_TIME_10S_FLAG);
			ChangeDeviceAdvertise(Dev_Emotion_Accept);
			osal_start_timerEx(GetAppTaskId(), MSG_RECEIVE_EXP_TIMEOUT, 6000+200);
			MotoRun(MOTO_DEFAULT);
			break;
			
		case MSG_HANDLER_END:
			ChangeDeviceAdvertise(Dev_Init_Delay_6000ms);	
			DisplayUnicode_Stop();
			DevicesComId.ReceiveEXPHandlerId = 0;
			break;
			
		case MSG_RECEIVE_EXP_TIMEOUT:
			ChangeDeviceAdvertise(Dev_Init);	
			break;
			
		case MSG_TOUCH_UP_SHORTKEY:				/*单击*/	
			break;
			
		//case MSG_TOUCH_UP_DOUBLE_SHORTKEY:		/*双击*/		
		//case MSG_TOUCH_UP_SHORTKEY:				/*单击*/
		case MSG_TOUCH_LONGKEY:					/*长按*/				
		case MSG_TOUCH_DOWN_UP_FLICK:				/*左*/ 
		case MSG_TOUCH_UP_DOWN_FLICK: 			/*右*/
			UnloadHandler(DevicesComId.ReceiveEXPHandlerId);
			break;  
	
		case MSG_GET_SCAN_EXP:
			if((DevExp!= device_exp) || (exp_count != device_exp_count))
			{
				if(CheckpCurrentHandler(DevicesComId.ReceiveEXPHandlerId))
				{
					UART_Send(&device_exp_count, 1);
					MotoRun(MOTO_DEFAULT);
					ChangeDeviceAdvertise(Dev_Emotion_Accept);
					osal_start_timerEx(GetAppTaskId(), MSG_RECEIVE_EXP_TIMEOUT, 6000+200);
					SendMsg(MSG_DISPLAY);
				}
				else{ 
					UnloadHandler(DevicesComId.ReceiveEXPHandlerId);
					SendMsg(msg);			
				}
					
			} 
			break;
			
		case MSG_DISPLAY:
			ClearScreen();
			if (device_exp == 1)
			{
				ShowEmotionOne();
			}
			else if (device_exp == 2)
			{
				ShowEmotionTwo();
			}
			else if (device_exp == 3)
			{
				ShowEmotionThree();
			}
			else if (device_exp == 4)
			{
				ShowEmotionFour();
			}
			else if (device_exp == 5)
			{
				ShowEmotionFive();
			}
			DisplayRname();
			DevExp = device_exp;
			exp_count = device_exp_count;
			break;
					
		case MSG_FONT_BUF_DISPLAY_NEXT_LETTER:			/*显示滚屏文字*/
			if(CheckpCurrentHandler(DevicesComId.ReceiveEXPHandlerId))
				DisplayFontLineUnicode();	
			break;	
			
	        case MSG_SYSTEM_SUSPEND:
	     		DisplayUnicode_Stop();
   	 		return RET_MSG_UNHANDLED;
			
		default:
			return RET_MSG_UNHANDLED;
	}

	return RET_MSG_HANDLED;

}

/*删除好友*/
uint16 DevicesDelHandler(MsgType msg, int iParam, void *pContext)
{
	switch(msg)
	{
		case MSG_HANDLER_BEGIN:
			DevicesComId.DevicesDelHandlerId = iParam;
			SetSystemSuspendTime(DISPLAY_TIME_10S_FLAG);
			DevDelFlag =0;
			break;

		case MSG_HANDLER_END:
			DevicesComId.DevicesDelHandlerId = 0;
			osal_stop_timerEx(GetAppTaskId(), MSG_REPEAT_DELETING_DEVICES_DISPLAY);				/*停止显示删除滚动*/
			break;
			
		//case MSG_TOUCH_UP_DOUBLE_SHORTKEY:		/*双击*/						
		case MSG_TOUCH_LONGKEY:					/*长按*/	
		case MSG_TOUCH_DOWN_UP_FLICK:				/*左*/
		case MSG_TOUCH_UP_DOWN_FLICK: 			/*右*/ 
			break;
			
		case MSG_TOUCH_UP_SHORTKEY:				/*单击*/
			if(DevDelFlag==0)
			{
				//if(Cy<=0x5A)
				{
					if(Cx<=0x32&&Cy>0x30) 					/*NO*/
					//if(Cx<=0x32) 							/*NO*/
					{
						UnloadHandler(DevicesComId.DevicesDelHandlerId);
					}
					else if(Cx>0x32&&Cy>0x30)         			/*YES*/
					//else if(Cx>0x32)         						/*YES*/	
					{
						DevDelFlag =1;
						ChangeDeviceAdvertise(Dev_DelFriend);
						ShowDevicesDeleting();
						osal_start_timerEx(GetAppTaskId(), MSG_GET_SCAN_DEL_FRIENDS_FAIL, 6000);
					}
				}
			}
			break;

		case MSG_GET_SCAN_DEL_FRIENDS_OK:
			if(DevDelFlag<=1)
			{
				DevDelFlag=2;
				osal_stop_timerEx(GetAppTaskId(), MSG_REPEAT_DELETING_DEVICES_DISPLAY);				/*停止显示删除滚动*/
				osal_stop_timerEx(GetAppTaskId(), MSG_GET_SCAN_DEL_FRIENDS_FAIL);
				DelFromFriendsBuff(rWatchid);
				ChangeDeviceAdvertise(Dev_Init);
				if(CheckpCurrentHandler(DevicesComId.DevicesDelHandlerId))
				{
					ClearScreen();
					ShowDevicesDelSuccess();	
				}
				osal_start_timerEx(GetAppTaskId(), MSG_DELETE_DEVICES_TIMEOUT, 2000);
			}
			break;
			
		case MSG_GET_SCAN_DEL_FRIENDS_FAIL:
			if(DevDelFlag<=1)
			{
				DevDelFlag=3;
				ChangeDeviceAdvertise(Dev_Init);
				osal_stop_timerEx(GetAppTaskId(), MSG_REPEAT_DELETING_DEVICES_DISPLAY);				/*停止显示删除滚动*/
				if(CheckpCurrentHandler(DevicesComId.DevicesDelHandlerId))
				{	
					ClearScreen();
					ShowDevicesDelFail();
				}
				osal_start_timerEx(GetAppTaskId(), MSG_DELETE_DEVICES_TIMEOUT, 2000);
			}
			break;
			
		case MSG_DELETE_DEVICES_TIMEOUT:			
			UnloadHandler(DevicesComId.DevicesDelHandlerId);
			break;

		case MSG_DISPLAY:
			switch(DevDelFlag)
			{
				case 0:
					ShowDevicesDel();
					break;
				case 1:
					ShowDevicesDeleting();
					break;
				case 2:
					ShowDevicesDelSuccess();
					break;
				case 3:
					ShowDevicesDelFail();
					break;
				default:
					SendMsg(MSG_DELETE_DEVICES_TIMEOUT);
					break;
			}
			break;
			
		case MSG_REPEAT_DELETING_DEVICES_DISPLAY:										/*显示删除滚动*/
			if(CheckpCurrentHandler(DevicesComId.DevicesDelHandlerId))
				ShowDevicesDeleting();
			break;
			
	        case MSG_SYSTEM_SUSPEND:														
	     		osal_stop_timerEx(GetAppTaskId(), MSG_REPEAT_DELETING_DEVICES_DISPLAY);	/*停止显示删除滚动*/
   	 		return RET_MSG_UNHANDLED;
			
		default:
			return RET_MSG_UNHANDLED;;
	}	
	return RET_MSG_HANDLED;
}


/*收到删除好友请求*/
uint16 ReceiveDelHandler(MsgType msg, int iParam, void *pContext)
{
	switch(msg)
	{
		case MSG_HANDLER_BEGIN:
			DevicesComId.ReceiveDelHandlerId = iParam;
			SetSystemSuspendTime(DISPLAY_TIME_10S_FLAG);	
			DelFromFriendsBuff(rWatchid);
			ChangeDeviceAdvertise(Dev_DelFriend_Accept);	
			osal_start_timerEx(GetAppTaskId(), MSG_RECEIVE_DEL_TIMEOUT, 6000+200);
			break;

		case MSG_HANDLER_END:	
			DevicesComId.ReceiveDelHandlerId = 0;
			ChangeDeviceAdvertise(Dev_Init_Delay_6000ms);
		   	osal_stop_timerEx(GetAppTaskId(), MSG_REPEAT_REC_DELETING_DEVICES);	/*停止显示删除动画*/	
	     		DisplayUnicode_Stop();													/*停止显示滚屏文字*/			
			break;

		case MSG_RECEIVE_DEL_TIMEOUT:
			ChangeDeviceAdvertise(Dev_Init);
			break;
			
		case MSG_GET_SCAN_DEL_FRIENDS:
			break;		
			
		case MSG_TOUCH_UP_SHORTKEY:				/*单击*/	
			break;			
		//case MSG_TOUCH_UP_DOUBLE_SHORTKEY:		/*双击*/	
		case MSG_TOUCH_LONGKEY:					/*长按*/		
		case MSG_TOUCH_DOWN_UP_FLICK:				/*左*/ 
		case MSG_TOUCH_UP_DOWN_FLICK: 			/*右*/
		//case MSG_TOUCH_UP_SHORTKEY:				/*单击*/
			UnloadHandler(DevicesComId.ReceiveDelHandlerId);  	
			break; 

		case MSG_DISPLAY:
			ShowDevDel();
			DisplayRname();
			break;
		
		case MSG_REPEAT_REC_DELETING_DEVICES:										/*显示删除动画*/
			if(CheckpCurrentHandler(DevicesComId.ReceiveDelHandlerId))
				ShowDevDel();	
			break;
			
		case MSG_FONT_BUF_DISPLAY_NEXT_LETTER:										/*显示滚屏文字*/
			if(CheckpCurrentHandler(DevicesComId.ReceiveDelHandlerId))
				DisplayFontLineUnicode();	
			break;	
			
	       case MSG_SYSTEM_SUSPEND:													
		   	osal_stop_timerEx(GetAppTaskId(), MSG_REPEAT_REC_DELETING_DEVICES);	/*停止显示删除动画*/	
	     		DisplayUnicode_Stop();													/*停止显示滚屏文字*/
   	 		return RET_MSG_UNHANDLED;	
			
		default:
			return RET_MSG_UNHANDLED;
	}

	return RET_MSG_HANDLED;
 
}






