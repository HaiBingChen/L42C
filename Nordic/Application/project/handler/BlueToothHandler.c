#include "headfile.h"

BluetoothStateTAG BluetoothState;


uint8 BTPairResultIdex=0;
/*经典蓝牙配对结果Handler*/
uint16 BTPairResult_Handler(MsgType msg, int iParam, void *pContext)
{
	switch(msg)
	{
		case MSG_HANDLER_BEGIN:
			BluetoothState.BTPairResultId = iParam;
			SetSystemSuspendTime(DISPLAY_TIME_10S_FLAG);
			osal_start_timerEx(GetAppTaskId(), MSG_BT_PAIR_RESULT_HANDLER_TIMEOUT, 2000);			
			break;

		case MSG_HANDLER_END:
			BluetoothState.BTPairResultId = 0;
			break;
/*用户操作*/
		case MSG_LEFTDOWN_SHORTKEY:				/*左键 单击*/	
		case MSG_LEFTDOWN_LONGKEY:           			/*左键 长按*/
		case MSG_RIGHTDOWN_DOUBLE_SHORTKEY:   	/*右键 双击*/	
		case MSG_RIGHTDOWN_LONGKEY:				/*右键 长按*/	
		case MSG_RIGHTDOWN_LONGKEY_UP:			/*右键 长按松开*/		
		case MSG_TOUCH_UP_FLICK:					/*上*/
		case MSG_TOUCH_DOWN_FLICK:				/*下*/
		case MSG_TOUCH_DOWN_UP_FLICK:				/*左*/ 
		case MSG_TOUCH_UP_DOWN_FLICK: 			/*右*/
		case MSG_TOUCH_UP_SHORTKEY:				/*触摸 单击*/  
		case MSG_TOUCH_UP_DOUBLE_SHORTKEY:		/*触摸 双击*/	
		case MSG_TOUCH_LONGKEY:					/*触摸 长按*/
			break;

		case MSG_LEFTDOWN_DOUBLE_SHORTKEY:		/*左键 双击*/
			osal_stop_timerEx(GetAppTaskId(), MSG_BT_PAIR_RESULT_HANDLER_TIMEOUT);  
			UnloadHandler_WithoutDisplay(BluetoothState.BTPairResultId);
			LoadHandler(BTConnecting_Handler,0);		
			break;

		case MSG_RIGHTDOWN_SHORTKEY: 				/*右键 单击*/
			osal_stop_timerEx(GetAppTaskId(), MSG_BT_PAIR_RESULT_HANDLER_TIMEOUT);  
		case MSG_BT_PAIR_RESULT_HANDLER_TIMEOUT:	
			UnloadHandler(BluetoothState.BTPairResultId);
			break;

		case MSG_DISPLAY:
			if(BTPairResultIdex==1)	
				ShowBtPairSuccess();
			else if(BTPairResultIdex==2)	 
				ShowBtPairFail();
			break;

		default:
			return RET_MSG_UNHANDLED;
	}
	return RET_MSG_HANDLED;
}

/*经典蓝牙配对Handler*/
uint16 BTConnecting_Handler(MsgType msg, int iParam, void *pContext)
{
	switch(msg)
	{
		case MSG_HANDLER_BEGIN:
			BluetoothState.BTConnectingId = iParam;
			BC5_State = BC5_BT_POWEROFF;
			osal_start_timerEx(GetAppTaskId(), MSG_BC5_OFF_RESET, 100);
			osal_start_timerEx(GetAppTaskId(), MSG_BT_CONNECTING_HANDLER_TIMEOUT, 120000);
			break;

		case MSG_HANDLER_END:
			BluetoothState.BTConnectingId = 0;
			break;
			
		case MSG_UART0_READY:    
		case MSG_SYSTEM_SUSPEND:
			break;		
/*用户操作*/
		case MSG_LEFTDOWN_SHORTKEY:				/*左键 单击*/	
		case MSG_LEFTDOWN_DOUBLE_SHORTKEY:		/*左键 双击*/
		case MSG_LEFTDOWN_LONGKEY:           			/*左键 长按*/
		case MSG_RIGHTDOWN_DOUBLE_SHORTKEY:   	/*右键 双击*/	
		case MSG_RIGHTDOWN_LONGKEY:				/*右键 长按*/	
		case MSG_RIGHTDOWN_LONGKEY_UP:			/*右键 长按松开*/		
		case MSG_TOUCH_UP_FLICK:					/*上*/
		case MSG_TOUCH_DOWN_FLICK:				/*下*/
		case MSG_TOUCH_DOWN_UP_FLICK:				/*左*/ 
		case MSG_TOUCH_UP_DOWN_FLICK: 			/*右*/
		case MSG_TOUCH_UP_SHORTKEY:				/*触摸 单击*/  
		case MSG_TOUCH_UP_DOUBLE_SHORTKEY:		/*触摸 双击*/	
		case MSG_TOUCH_LONGKEY:					/*触摸 长按*/
			break;

		case MSG_RIGHTDOWN_SHORTKEY: 				/*右键 单击*/
			SetBc5Power(false);
			UnloadHandler(BluetoothState.BTConnectingId);
			break;

		case MSG_BC5_BT_CHANGE:
			if(BC5_State == BC5_BT_READY)
			{
				BC5_SetBtPairCmd(BC5_PAIR_START);
			}
			else if(BC5_State == BC5_BT_CONNECT_OK)
			{      
				uint8 TempFlag=0x01;
				WRITE_USER_FLAG(&TempFlag,BC5_RECONNECTED_LOC,BC5_RECONNECTED_LEN);
				FilterDisconnectDisplay = true;
				osal_stop_timerEx(GetAppTaskId(), MSG_BT_CONNECTING_HANDLER_TIMEOUT);  			
				/*配对连接成功*/
				UnloadHandler_WithoutDisplay(BluetoothState.BTConnectingId);
				BTPairResultIdex=1;
				LoadHandler(BTPairResult_Handler,0);
			}
			break;
			
		case MSG_BT_CONNECTING_HANDLER_TIMEOUT:
			if(BC5_State!=BC5_BT_CONNECT_OK && BC5_State != BC5_BT_PAIR_OK)
			{
				BC5_SetBtPairCmd(BC5_PAIR_STOP);
				SetBc5Power(false);
				/*配对连接失败*/
				UnloadHandler_WithoutDisplay(BluetoothState.BTConnectingId);
				BTPairResultIdex=2;
				LoadHandler(BTPairResult_Handler,0);
			}
			break;
			
		case MSG_DISPLAY: 
			ShowBtConnect();
			break;

		default:
			return RET_MSG_UNHANDLED;
	}
	return RET_MSG_HANDLED;
}

