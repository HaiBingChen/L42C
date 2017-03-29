#include "headfile.h"
#include "math.h"

DevicesComHadlerTAG  DevicesComId;
DevicesCom_t DevicesCom;

DeviceAdvertise_t NextDevAdv=Dev_Init;
uint8 DevSearFlag = 0; 			/*��¼���� ���ѡ��Ǻ��ѡ��Ѳ�����BACK����*/
uint8 DevAddFriendFlag =0; 		/*��¼��Ӻ����Ƿ�õ���Ӧ*/
uint8 DevAddFriendRspFlag = 0; 	/*��¼��Ӻ��ѳɹ�����ʧ��*/
uint8 DevRecAddFriendFlag =0;	/*��¼��Ӻ��ѡ��ܾ�*/
uint8 EmoMenuFlag=0;			/*��¼�񶯡��������*/
uint8 MotoSendFlag =0;			/*��¼�Ƿ�����MOTO��ť*/
uint8 EmoIndex=0;				/*��¼ĳһ������*/	
uint8 EmoCount=0x00;			/*��¼ĳһ�����鷢�ʹ���*/
uint8 MOTOCount=0x00;			/*��¼ĳһ���𶯷��ʹ���*/	


uint8 DevEmoChooseFlag=0;		/*��¼�Ƿ��յ������Ӧ*/
uint8 DevDelFlag=0;				/*��¼�Ƿ��ѵ��ɾ������*/
uint8 DevExp = 0;					/*��¼�յ��ı���*/

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

/*"ON OFF"����*/
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
			
		//case MSG_TOUCH_UP_DOUBLE_SHORTKEY:			/*˫��*/
			//break;
			
		case MSG_TOUCH_LONGKEY:						/*����*/	
			UnloadHandler(DevicesComId.Id);
			CurrentMenuIParam=LoadHandler(ToolHandler,0);
			return RET_MSG_HANDLED;
		
		case MSG_TOUCH_DOWN_UP_FLICK:					/*��*/ 	
			UnloadHandler(DevicesComId.Id);
			LoadHandler(BackHandler,0);
			return RET_MSG_HANDLED;
			
		case MSG_TOUCH_UP_DOWN_FLICK: 				/*��*/
			UnloadHandler(DevicesComId.Id);
			LoadHandler(TimerHandler,0);
			return RET_MSG_HANDLED;
			
		case MSG_TOUCH_UP_SHORTKEY:					/*����*/
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
					//else if(Cx>=0x45)						/*����*/
					else 
					{	
						if (DevicesCom.on_off == DeviceOnStatus)
						{
							UnloadHandler_WithoutDisplay(DevicesComId.Id);
							CleanScanerBuf();
							DevSearFlag = 0;
							LoadHandler(DevicesSearchHandler,0);		/*Load----�Ŵ����������*/
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


/*�Ŵ����������*/
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
	     		DisplayUnicode_Stop();													/*ֹͣ�ֿ���ʾ*/				
			osal_stop_timerEx(GetAppTaskId(), MSG_REPEAT_DEVICE_SEARCH_DISPLAY);	/*ֹͣ�Ŵ󾵶���*/
			DevComClearFontBuf();															/*ɾ��ɨ���б�*/		
			break;
			
		//case MSG_TOUCH_UP_DOUBLE_SHORTKEY:		/*˫��*/
			//break;			
			
		case MSG_TOUCH_LONGKEY:					/*����*/	
			if(DevSearFlag==DeviceFriend)
				LoadHandler(DevicesDelHandler,0);	/*Load----ɾ������*/
			break;
			
		case MSG_GET_SCAN_NAME:					/*ɨ�赽���豸*/
			if(DevSearFlag!=0)
				break;
			else
				DevSearFlag=DeviceHasSearch;
		case MSG_DEVICE_SEARCH_TIMEOUT:
			SetSystemSuspendTime(DISPLAY_TIME_10S_FLAG);
		case MSG_TOUCH_DOWN_UP_FLICK:				/*��*/
		case MSG_TOUCH_UP_DOWN_FLICK: 			/*��*/
			osal_stop_timerEx(GetAppTaskId(), MSG_REPEAT_DEVICE_SEARCH_DISPLAY);		/*ֹͣ�Ŵ󾵶���*/
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
			
		case MSG_TOUCH_UP_SHORTKEY:				/*����*/	
			if ((DevSearFlag == DeviceNull) || (DevSearFlag == DeviceBack))
			{
				UnloadHandler_WithoutDisplay(DevicesComId.DevicesSearchHandlerId);		/*Load----"ON OFF"����*/
				LoadHandler(DevicesCom_Handler,0);	 	
			}
			else if (DevSearFlag == DeviceStrange)
			{	
				LoadHandler(DevicesAddFriendHandler,0);	 								/*Load----"���ͺ�������"����*/
			}
			else if (DevSearFlag == DeviceFriend)											
			{
				LoadHandler(SendEmotionHandler,0);	 									/*Load----ѡ��"����/����"����*/
			}
			break;

		case MSG_DISPLAY:
			if (DevSearFlag == 0)
				ShowDevicesSea();										/*��ʾ�Ŵ󾵶���*/
			else 
				HandlerMsg(MSG_DEVICE_SEARCH_TIMEOUT,0);
			break;

		case MSG_REPEAT_DEVICE_SEARCH_DISPLAY:      						/*��ʾ�Ŵ󾵶���*/
			if (CheckpCurrentHandler(DevicesComId.DevicesSearchHandlerId))
				ShowDevicesSea();
			break;

		case MSG_FONT_BUF_DISPLAY_NEXT_LETTER:							/*��ʾ��������*/
			if(CheckpCurrentHandler(DevicesComId.DevicesSearchHandlerId))
				DisplayFontLineUnicode();	
			break;	
			
	        case MSG_SYSTEM_SUSPEND:						
	     		DisplayUnicode_Stop();													/*ֹͣ�ֿ���ʾ*/				
			osal_stop_timerEx(GetAppTaskId(), MSG_REPEAT_DEVICE_SEARCH_DISPLAY);	/*ֹͣ�Ŵ󾵶���*/
   	 		return RET_MSG_UNHANDLED;	
			
		default:
			return RET_MSG_UNHANDLED;
	}
	return RET_MSG_HANDLED;
}


/*"���ͺ�������"����*/		
uint16 DevicesAddFriendHandler(MsgType msg, int iParam, void *pContext)
{
	switch(msg)
	{
		case MSG_HANDLER_BEGIN:
			DevicesComId.DevicesAddFriendHandlerId = iParam;
			DevAddFriendFlag = 0;
			ChangeDeviceAdvertise(Dev_AddFriend);
			SetSystemSuspendTime(DISPLAY_TIME_10S_FLAG);
			osal_start_timerEx(GetAppTaskId(), MSG_DEVICE_ADDFRIEND_DISPLAY_TIMEOUT, 8000+200);	/*8���ʧ��*/		
			break;

		case MSG_HANDLER_END:
			DevAddFriendRspFlag=0;
			ChangeDeviceAdvertise(Dev_Init);
			DevicesComId.DevicesAddFriendHandlerId = 0;
			osal_stop_timerEx(GetAppTaskId(), MSG_REPEAT_DEVICE_REQUEST_DISPLAY);   	 /*ֹͣ��ʾ�������󶯻�*/
			break;
			
		//case MSG_TOUCH_UP_DOUBLE_SHORTKEY:		/*˫��*/			
		case MSG_TOUCH_LONGKEY:					/*����*/	
		case MSG_TOUCH_DOWN_UP_FLICK:				/*��*/ 
		case MSG_TOUCH_UP_DOWN_FLICK: 			/*��*/ 
		case MSG_TOUCH_UP_SHORTKEY:				/*����*/	
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
			osal_stop_timerEx(GetAppTaskId(), MSG_REPEAT_DEVICE_REQUEST_DISPLAY);   	 /*ֹͣ��ʾ�������󶯻�*/
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
			
		case MSG_REPEAT_DEVICE_REQUEST_DISPLAY:			/*��ʾ���ͺ������󶯻�*/
			if(CheckpCurrentHandler(DevicesComId.DevicesAddFriendHandlerId))
				ShowSendRequest();
			break;

	        case MSG_SYSTEM_SUSPEND:
	     		osal_stop_timerEx(GetAppTaskId(), MSG_REPEAT_DEVICE_REQUEST_DISPLAY);   	 /*ֹͣ��ʾ�������󶯻�*/
   	 		return RET_MSG_UNHANDLED;
			
		default:
			return RET_MSG_UNHANDLED;
	}

	return RET_MSG_HANDLED;
}


/*�յ���������*/
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
			
		//case MSG_TOUCH_UP_DOUBLE_SHORTKEY:		/*˫��*/				
		case MSG_TOUCH_LONGKEY:					/*����*/	
		case MSG_TOUCH_DOWN_UP_FLICK:				/*��*/ 
		case MSG_TOUCH_UP_DOWN_FLICK: 			/*��*/	
		case MSG_GET_SCAN_ADD_FRIENDS:			/*�յ���������*/	
			break;

		case MSG_TOUCH_UP_SHORTKEY:				/*����*/
			//if(Cy>=0x0A)
			{
				if(Cx>0x32&&Cy<0x30) 			/*����*/
				//if(Cx>0x32) 						/*����*/	
				{
					osal_start_timerEx(GetAppTaskId(), MSG_DEVICE_RECEIVE_ADDFRIEND_TIMEOUT, 6000);
					ChangeDeviceAdvertise(Dev_AddFriend_Accept);		
					AddToFriendsBuff(rWatchid);
					DevRecAddFriendFlag =1;
					ClearScreen();
					ShowAddDevicesSuccess();
					DisplayRname();
				}
				else if(Cx<=0x32&&Cy<0x30) 	      		/*�ܾ�*/
				//else if(Cx<=0x32) 	      				/*�ܾ�*/	
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

		case MSG_FONT_BUF_DISPLAY_NEXT_LETTER:			/*��ʾ��������*/
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

/*�յ����������Ӧ*/
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
		case MSG_TOUCH_UP_SHORTKEY:				/*����*/	
			break;
		//case MSG_TOUCH_UP_DOUBLE_SHORTKEY:		/*˫��*/			
		case MSG_TOUCH_LONGKEY:					/*����*/	
		case MSG_TOUCH_DOWN_UP_FLICK:				/*��*/ 
		case MSG_TOUCH_UP_DOWN_FLICK: 			/*��*/ 
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
						
		case MSG_FONT_BUF_DISPLAY_NEXT_LETTER:			/*��ʾ��������*/
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


/*ѡ��"����/����"����*/
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
			osal_stop_timerEx(GetAppTaskId(), MSG_REPEAT_DEVICE_MOTO_DISPLAY);		/*ֹͣ�����𶯶���*/
			break;
			
		//case MSG_TOUCH_UP_DOUBLE_SHORTKEY:		/*˫��*/	
		case MSG_TOUCH_LONGKEY:					/*����*/	
		case MSG_TOUCH_UP_DOWN_FLICK: 			/*��*/
			break;  
	
		case MSG_TOUCH_UP_SHORTKEY:				/*����*/
			if (EmoMenuFlag == 0) {
				MotoSendFlag =1;
				ChangeDeviceAdvertise(Dev_Moto);
				ShowDevicesMotoRepeat();
			}
			else if(EmoMenuFlag==1){			
				ChangeDeviceAdvertise(Dev_Init);
				LoadHandler(EmotionChooseHandler,0);		/*���ر����*/
			}
			else if(EmoMenuFlag==2){
				UnloadHandler(DevicesComId.SendEmotionHandlerId);		
			}
			break;

			
		case MSG_TOUCH_DOWN_UP_FLICK:				/*��*/
			osal_stop_timerEx(GetAppTaskId(), MSG_REPEAT_DEVICE_MOTO_DISPLAY);
			if(EmoMenuFlag==2)
				EmoMenuFlag = 0;
			else
				EmoMenuFlag++;
		case MSG_DISPLAY:
			ClearScreen();
			switch(EmoMenuFlag)
			{
				case 0:		/*��*/
					ShowEmotionStart();
					break;
				case 1:		/*�����*/
					ShowEmotionHomeMenu();
					break;
				case 2:		/*BACK*/
					ShowBack();
					break;
			}			
			break;
			
		case MSG_REPEAT_DEVICE_MOTO_DISPLAY:		/*��ʾ�����𶯶���*/
			if(CheckpCurrentHandler(DevicesComId.SendEmotionHandlerId))
				ShowDevicesMotoRepeat();
			break;
			
	        case MSG_SYSTEM_SUSPEND:
	     		osal_stop_timerEx(GetAppTaskId(), MSG_REPEAT_DEVICE_MOTO_DISPLAY);		/*ֹͣ�����𶯶���*/
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

/*�յ�����Ϣ*/
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
		case MSG_TOUCH_UP_SHORTKEY:				/*����*/	
			break;	
		//case MSG_TOUCH_UP_DOUBLE_SHORTKEY:		/*˫��*/		
		//case MSG_TOUCH_UP_SHORTKEY:				/*����*/
		case MSG_TOUCH_LONGKEY:					/*����*/			
		case MSG_TOUCH_DOWN_UP_FLICK:				/*��*/ 	
		case MSG_TOUCH_UP_DOWN_FLICK: 			/*��*/
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
						
		case MSG_FONT_BUF_DISPLAY_NEXT_LETTER:			/*��ʾ��������*/
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

/*���ͱ����*/
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
			osal_stop_timerEx(GetAppTaskId(), MSG_REPEAT_DEVICE_EMOTION_DISPLAY);	/*ֹͣ���鷢�Ͷ���*/
			break;
		
		//case MSG_TOUCH_UP_DOUBLE_SHORTKEY:		/*˫��*/					
		case MSG_TOUCH_LONGKEY:					/*����*/
		case MSG_TOUCH_UP_DOWN_FLICK: 			/*��*/
			break;
			
		case MSG_TOUCH_UP_SHORTKEY:				/*����*/
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

	
		case MSG_TOUCH_DOWN_UP_FLICK:				/*��*/ 
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
			
		case MSG_REPEAT_DEVICE_EMOTION_DISPLAY:							/*���鷢�Ͷ���*/
			if(CheckpCurrentHandler(DevicesComId.EmotionChooseHandlerId))
				ShowSendEmotionRepeat();
			break;
			
	        case MSG_SYSTEM_SUSPEND:
	     		osal_stop_timerEx(GetAppTaskId(), MSG_REPEAT_DEVICE_EMOTION_DISPLAY);	/*ֹͣ���鷢�Ͷ���*/
   	 		return RET_MSG_UNHANDLED;	
			
		default:
			return RET_MSG_UNHANDLED;
	}	
	return RET_MSG_HANDLED;
}

/*�յ�����*/
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
			
		case MSG_TOUCH_UP_SHORTKEY:				/*����*/	
			break;
			
		//case MSG_TOUCH_UP_DOUBLE_SHORTKEY:		/*˫��*/		
		//case MSG_TOUCH_UP_SHORTKEY:				/*����*/
		case MSG_TOUCH_LONGKEY:					/*����*/				
		case MSG_TOUCH_DOWN_UP_FLICK:				/*��*/ 
		case MSG_TOUCH_UP_DOWN_FLICK: 			/*��*/
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
					
		case MSG_FONT_BUF_DISPLAY_NEXT_LETTER:			/*��ʾ��������*/
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

/*ɾ������*/
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
			osal_stop_timerEx(GetAppTaskId(), MSG_REPEAT_DELETING_DEVICES_DISPLAY);				/*ֹͣ��ʾɾ������*/
			break;
			
		//case MSG_TOUCH_UP_DOUBLE_SHORTKEY:		/*˫��*/						
		case MSG_TOUCH_LONGKEY:					/*����*/	
		case MSG_TOUCH_DOWN_UP_FLICK:				/*��*/
		case MSG_TOUCH_UP_DOWN_FLICK: 			/*��*/ 
			break;
			
		case MSG_TOUCH_UP_SHORTKEY:				/*����*/
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
				osal_stop_timerEx(GetAppTaskId(), MSG_REPEAT_DELETING_DEVICES_DISPLAY);				/*ֹͣ��ʾɾ������*/
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
				osal_stop_timerEx(GetAppTaskId(), MSG_REPEAT_DELETING_DEVICES_DISPLAY);				/*ֹͣ��ʾɾ������*/
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
			
		case MSG_REPEAT_DELETING_DEVICES_DISPLAY:										/*��ʾɾ������*/
			if(CheckpCurrentHandler(DevicesComId.DevicesDelHandlerId))
				ShowDevicesDeleting();
			break;
			
	        case MSG_SYSTEM_SUSPEND:														
	     		osal_stop_timerEx(GetAppTaskId(), MSG_REPEAT_DELETING_DEVICES_DISPLAY);	/*ֹͣ��ʾɾ������*/
   	 		return RET_MSG_UNHANDLED;
			
		default:
			return RET_MSG_UNHANDLED;;
	}	
	return RET_MSG_HANDLED;
}


/*�յ�ɾ����������*/
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
		   	osal_stop_timerEx(GetAppTaskId(), MSG_REPEAT_REC_DELETING_DEVICES);	/*ֹͣ��ʾɾ������*/	
	     		DisplayUnicode_Stop();													/*ֹͣ��ʾ��������*/			
			break;

		case MSG_RECEIVE_DEL_TIMEOUT:
			ChangeDeviceAdvertise(Dev_Init);
			break;
			
		case MSG_GET_SCAN_DEL_FRIENDS:
			break;		
			
		case MSG_TOUCH_UP_SHORTKEY:				/*����*/	
			break;			
		//case MSG_TOUCH_UP_DOUBLE_SHORTKEY:		/*˫��*/	
		case MSG_TOUCH_LONGKEY:					/*����*/		
		case MSG_TOUCH_DOWN_UP_FLICK:				/*��*/ 
		case MSG_TOUCH_UP_DOWN_FLICK: 			/*��*/
		//case MSG_TOUCH_UP_SHORTKEY:				/*����*/
			UnloadHandler(DevicesComId.ReceiveDelHandlerId);  	
			break; 

		case MSG_DISPLAY:
			ShowDevDel();
			DisplayRname();
			break;
		
		case MSG_REPEAT_REC_DELETING_DEVICES:										/*��ʾɾ������*/
			if(CheckpCurrentHandler(DevicesComId.ReceiveDelHandlerId))
				ShowDevDel();	
			break;
			
		case MSG_FONT_BUF_DISPLAY_NEXT_LETTER:										/*��ʾ��������*/
			if(CheckpCurrentHandler(DevicesComId.ReceiveDelHandlerId))
				DisplayFontLineUnicode();	
			break;	
			
	       case MSG_SYSTEM_SUSPEND:													
		   	osal_stop_timerEx(GetAppTaskId(), MSG_REPEAT_REC_DELETING_DEVICES);	/*ֹͣ��ʾɾ������*/	
	     		DisplayUnicode_Stop();													/*ֹͣ��ʾ��������*/
   	 		return RET_MSG_UNHANDLED;	
			
		default:
			return RET_MSG_UNHANDLED;
	}

	return RET_MSG_HANDLED;
 
}






