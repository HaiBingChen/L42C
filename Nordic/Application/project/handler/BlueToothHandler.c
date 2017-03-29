#include "headfile.h"

BluetoothStateTAG BluetoothState;


uint8 BTPairResultIdex=0;
/*����������Խ��Handler*/
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
/*�û�����*/
		case MSG_LEFTDOWN_SHORTKEY:				/*��� ����*/	
		case MSG_LEFTDOWN_LONGKEY:           			/*��� ����*/
		case MSG_RIGHTDOWN_DOUBLE_SHORTKEY:   	/*�Ҽ� ˫��*/	
		case MSG_RIGHTDOWN_LONGKEY:				/*�Ҽ� ����*/	
		case MSG_RIGHTDOWN_LONGKEY_UP:			/*�Ҽ� �����ɿ�*/		
		case MSG_TOUCH_UP_FLICK:					/*��*/
		case MSG_TOUCH_DOWN_FLICK:				/*��*/
		case MSG_TOUCH_DOWN_UP_FLICK:				/*��*/ 
		case MSG_TOUCH_UP_DOWN_FLICK: 			/*��*/
		case MSG_TOUCH_UP_SHORTKEY:				/*���� ����*/  
		case MSG_TOUCH_UP_DOUBLE_SHORTKEY:		/*���� ˫��*/	
		case MSG_TOUCH_LONGKEY:					/*���� ����*/
			break;

		case MSG_LEFTDOWN_DOUBLE_SHORTKEY:		/*��� ˫��*/
			osal_stop_timerEx(GetAppTaskId(), MSG_BT_PAIR_RESULT_HANDLER_TIMEOUT);  
			UnloadHandler_WithoutDisplay(BluetoothState.BTPairResultId);
			LoadHandler(BTConnecting_Handler,0);		
			break;

		case MSG_RIGHTDOWN_SHORTKEY: 				/*�Ҽ� ����*/
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

/*�����������Handler*/
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
/*�û�����*/
		case MSG_LEFTDOWN_SHORTKEY:				/*��� ����*/	
		case MSG_LEFTDOWN_DOUBLE_SHORTKEY:		/*��� ˫��*/
		case MSG_LEFTDOWN_LONGKEY:           			/*��� ����*/
		case MSG_RIGHTDOWN_DOUBLE_SHORTKEY:   	/*�Ҽ� ˫��*/	
		case MSG_RIGHTDOWN_LONGKEY:				/*�Ҽ� ����*/	
		case MSG_RIGHTDOWN_LONGKEY_UP:			/*�Ҽ� �����ɿ�*/		
		case MSG_TOUCH_UP_FLICK:					/*��*/
		case MSG_TOUCH_DOWN_FLICK:				/*��*/
		case MSG_TOUCH_DOWN_UP_FLICK:				/*��*/ 
		case MSG_TOUCH_UP_DOWN_FLICK: 			/*��*/
		case MSG_TOUCH_UP_SHORTKEY:				/*���� ����*/  
		case MSG_TOUCH_UP_DOUBLE_SHORTKEY:		/*���� ˫��*/	
		case MSG_TOUCH_LONGKEY:					/*���� ����*/
			break;

		case MSG_RIGHTDOWN_SHORTKEY: 				/*�Ҽ� ����*/
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
				/*������ӳɹ�*/
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
				/*�������ʧ��*/
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

