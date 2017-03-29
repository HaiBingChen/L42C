
#include "headfile.h"





pU8 *msgPtr=NULL;
keyChange_t *  TouchKeyPmsg=NULL;
uint8 TouchKeyPressCouter=0;
uint8 TouchKeyDoublePressCouter=0;
static uint8 TouchKeyTaskId=0;
static uint8 halKeyLastShortUp = 0;
static halKeyCBack_t pHalKeyProcessFunction;
bool TouchKeyDoubleEvent=false;













/******************************************************************************************
*函数名:		TouchKeyInitTask
*参数:			
*返回值:		
*描述:			触摸或者按键任务初始化
*编写:			 
*版本信息:	    
******************************************************************************************/ 
void TouchKeyInitTask(uint8 task_id)
{
	TouchKeyTaskId=task_id;
        pHalKeyProcessFunction = SendTouchKeyMsg;
}
/******************************************************************************************
*函数名:		GetTouchKeyTaskId
*参数:			
*返回值:		
*描述:			得到触摸或者按键事件Id
*编写:			 
*版本信息:	   
******************************************************************************************/ 
uint8 GetTouchKeyTaskId(void)
{
	return TouchKeyTaskId;
}


uint8 SendTouchKeyMsg( uint8 keys, uint8 state )
{
        keyChange_t *msgPtr;

        msgPtr = (keyChange_t *)pS_OsMsgMalloc( sizeof(keyChange_t) );
        if ( msgPtr )
        {
                msgPtr->state = state;
                msgPtr->keys = keys;

                pS_OsMsgSend(GetTouchKeyTaskId(), (uint8 *)msgPtr );
        }
        
        return ( SUCCESS );
}


uint8 GetTouchKeyMsg(void)
{
        msgPtr = pS_OsMsgReceive(GetTouchKeyTaskId());

        if (msgPtr)
        {
            TouchKeyPmsg = (keyChange_t *)msgPtr;
            
            return TRUE;
        }

        return FALSE;        
 }


uint16 TouchKeyEventHandler(uint8 task_id,uint16 events)
{    
	if ( events & SYS_EVENT_MSG )
	{
		if(GetTouchKeyMsg()==TRUE)
		{
			HandleTouchKey( TouchKeyPmsg->state, TouchKeyPmsg->keys);
			pS_OsMsgDeallocate(msgPtr);
			return events; 
		}
		else
			return ( events ^ SYS_EVENT_MSG );
	}

	if (events & HAL_PRESS_TYPE_EVENT)
	{
		SetSystemSuspendTime(DISPLAY_TIME_10S_FLAG);
		if(CheckTouchKeyEvent())
		{
			TouchKeyPressCouter ++;
			if(TouchKeyPressCouter>=100) 	/*长按住超过2秒未松开*/
			{
				TouchKeyPressCouter =0;
				TOUCHKEY_PRESS_FLAG = false;	
				if(!DISPLAY_GOTO_SLEEP_FLAG)	
				SendMsg(MSG_TOUCH_LONGKEY);
			}
			else
				osal_start_timerEx(GetTouchKeyTaskId(), HAL_PRESS_TYPE_EVENT,20); 			   
		}
		else
		{
			if(TouchKeyPressCouter<100)			
			{
				if(TouchKeyDoubleEvent==false)
					TouchKeyDoublePressCouter++;
				TouchKeyPressCouter =0;								   			
			}					
			else							/*长按超过2秒并松开*/
			{
				TouchKeyPressCouter =0;
				if(!DISPLAY_GOTO_SLEEP_FLAG)				
					SendMsg(MSG_TOUCH_LONGKEY);					
			}					
		}
		return events ^ HAL_PRESS_TYPE_EVENT;
	}

	if (events & HAL_DOUBLE_KEY_EVENT)
	{
		HalKeyDoubleCheckTimeout();
		return (events ^ HAL_DOUBLE_KEY_EVENT);
	}

	if (events & HAL_SLIDER_EVENT)
	{
		SliderDispatch();
		return events ^ HAL_SLIDER_EVENT;
	}

	if (events & HAL_CYPRESS_ONE_TOUCH_EVENT)
	{
		CheckCypressEvent();
		return events ^ HAL_CYPRESS_ONE_TOUCH_EVENT;
	}

	if (events & HAL_CYPRESS_DOUBLE_TOUCH_EVENT)
	{
		return events ^ HAL_CYPRESS_DOUBLE_TOUCH_EVENT;
	}

	if (events & HAL_CYPRESS_LONG_TOUCH_EVENT)
	{
		CypressI2CLongTouch();
		return events ^ HAL_CYPRESS_LONG_TOUCH_EVENT;
	}

	if (events & HAL_KEY_EVENT)
	{
		HalKeyPoll();
		osal_start_timerEx(GetTouchKeyTaskId(), HAL_KEY_EVENT, 40);
		return events ^ HAL_KEY_EVENT;
	}
	return ( 0 );     
}

static uint8 halKeySavedKeys;  
uint8 HalKeyRead(void)
{
	uint16 adc=AdcRead(ADC_KEY_CHANNEL);
	uint8 ksave0 = 0, keys= 0;
	if(adc<100)
	{
		ksave0 = HAL_KEY_SW_3;		/*右键*/
	}
	else if(adc<680)	
	{
		ksave0 = HAL_KEY_SW_4;		/*左键*/
	}
	keys|=ksave0;
	return keys;
}

void HalKeyPoll(void)
{
	uint8 KeysValue=0;
	uint8 KeyState=0;
	static uint16 keyCounter=0;
	uint8 keyTmp = NO_KEY;
	keyTmp= HalKeyRead();
	if (keyTmp == NO_KEY || keyTmp != halKeySavedKeys)			//无按键按下，按键松开
	{
		if (keyCounter >= (KEY_LONG_TIMES + KEY_HOLD_TIMES) )  		//连按抬键
		{
			KeysValue=halKeySavedKeys;
			KeyState=HAL_KEY_STATE_HOLD_UP;
			SetSystemSuspendTime(DISPLAY_TIME_10S_FLAG);
		}
		else if (keyCounter >= KEY_LONG_TIMES )  					//长按抬键
		{
			KeysValue=halKeySavedKeys;
			KeyState=HAL_KEY_STATE_LONG_UP;
			SetSystemSuspendTime(DISPLAY_TIME_10S_FLAG);
		}
		else if (keyCounter > KEY_SCAN_TIMES )  				 	//短按抬键
		{
			KeysValue=halKeySavedKeys;
			KeyState=HAL_KEY_STATE_SHORT_UP;
			SetSystemSuspendTime(DISPLAY_TIME_10S_FLAG);
		}

		halKeySavedKeys = keyTmp;
		keyCounter = 0;
	}
	else
	{
		keyCounter++;
		if (keyCounter == KEY_SCAN_TIMES)							//去抖
		{
			KeysValue=halKeySavedKeys;
			KeyState=HAL_KEY_STATE_SHORT;
		}
		else if (keyCounter == KEY_LONG_TIMES)						//长按
		{
			KeysValue=halKeySavedKeys;
			KeyState=HAL_KEY_STATE_LONG;
		}
		else if (keyCounter == (KEY_LONG_TIMES + KEY_HOLD_TIMES) )	//连按
		{
			KeysValue=halKeySavedKeys;
			KeyState=HAL_KEY_STATE_HOLD; 
		}
		else if (keyCounter == (KEY_LONG_5S_TIMES) )					//长按7秒
		{
			KeysValue=halKeySavedKeys;
			KeyState=HAL_KEY_STATE_LONG_5S; 
		}				
		SetSystemSuspendTime(DISPLAY_STATE_NO_CHANGE);

	}
	HalKeyDistribute(KeysValue, KeyState);   
}


/*******************************************************************************
 *     This function handles double key .
 ******************************************************************************/
void HalKeyDistribute(uint8 KeysValue, uint8 KeyState)
{
	if((KeysValue != 0)&&(KeyState!=0)) /*   发送按键消息*/
	{
		if(HAL_KEY_STATE_SHORT_UP == KeyState)
		{
			osal_stop_timerEx(GetTouchKeyTaskId(), HAL_DOUBLE_KEY_EVENT);
			if(halKeyLastShortUp == KeysValue) // two same shortup events happen before timeout
			{
				if ((pHalKeyProcessFunction))
				{
					(pHalKeyProcessFunction) (KeysValue, HAL_KEY_STATE_DOUBLE_SHORT_UP);
				}
				halKeyLastShortUp = 0; // halKeyLastShortUp will be clear when double shortup event assert
			}
			else
			{
				if(halKeyLastShortUp>0) // report previous shortup event since two different shortup events assert
				{
					if((pHalKeyProcessFunction))
					{
						(pHalKeyProcessFunction) (halKeyLastShortUp, HAL_KEY_STATE_SHORT_UP);
					}
				}
				// save new shortup event and set a timer to check if there is a double shortup event
				halKeyLastShortUp = KeysValue;
				osal_start_timerEx(GetTouchKeyTaskId(), HAL_DOUBLE_KEY_EVENT, 450);
			}
		}
		else
		{
			if ((pHalKeyProcessFunction))
			{
				(pHalKeyProcessFunction) (KeysValue, KeyState);
			}
		}
	}
}

void HalKeyDoubleCheckTimeout(void)
{
	if ((pHalKeyProcessFunction))
	{
		(pHalKeyProcessFunction) (halKeyLastShortUp, HAL_KEY_STATE_SHORT_UP);
	}
	
	halKeyLastShortUp = 0; 
}

void HandleTouchKey( uint8 state, uint8 keys )
{
	if ( (keys & HAL_KEY_SW_1)) 							/*触摸*/
	{
		if((state==HAL_KEY_STATE_SHORT_UP))
		{
			SendMsg(MSG_TOUCH_UP_SHORTKEY);
		}
		else if((state==HAL_KEY_STATE_DOUBLE_SHORT_UP))
			SendMsg(MSG_TOUCH_UP_DOUBLE_SHORTKEY);
	}
	else if ( (keys & HAL_KEY_SW_2))
	{
		if((state==HAL_KEY_STATE_SHORT_UP))
		{
			SendMsg(MSG_TOUCH_DOWN_SHORTKEY);
		}
		else if((state==HAL_KEY_STATE_DOUBLE_SHORT_UP))
			SendMsg(MSG_TOUCH_DOWN_DOUBLE_SHORTKEY);
	}
	else if(SCAN_KEY_VALID_FLAG==true)					/*按键*/	
	{
		if(SLEEP_MODE_FLAG)
		{                                     
			osal_start_timerEx(GetSleepTaskId(), SLEEP_BUTTON_PRESS_EVENT,200);
		}

		if( (keys & HAL_KEY_SW_4))   	/*物理右键*/
		{
			if((state==HAL_KEY_STATE_SHORT_UP))
				SendMsg(MSG_RIGHTDOWN_SHORTKEY);
			else if((state==HAL_KEY_STATE_DOUBLE_SHORT_UP))
				SendMsg(MSG_RIGHTDOWN_DOUBLE_SHORTKEY);
			else if((state==HAL_KEY_STATE_LONG))
			{
				if ( !(keys & HAL_KEY_SW_4))
					SendMsg(MSG_RIGHTDOWN_LONGKEY); 	
			}
		}
		else if ( (keys & HAL_KEY_SW_3))	/*物理左键*/
		{
			if((state==HAL_KEY_STATE_SHORT_UP))
				SendMsg(MSG_FORCE_BACK_HOME);
				//SendMsg(MSG_LEFTDOWN_SHORTKEY);
			else if((state==HAL_KEY_STATE_DOUBLE_SHORT_UP))
				SendMsg(MSG_LEFTDOWN_DOUBLE_SHORTKEY);
			else if((state==HAL_KEY_STATE_LONG))
			{
				if ( !(keys & HAL_KEY_SW_3))
					SendMsg(MSG_LEFTDOWN_LONGKEY);
			}
			else if((state==HAL_KEY_STATE_LONG_5S))
			{
				SendMsg(MSG_LEFTDOWN_LONGKEY_5S);
			}
			else if((state==HAL_KEY_STATE_LONG_UP))
				SendMsg(MSG_RIGHTDOWN_LONGKEY_UP);
			else if((state==HAL_KEY_STATE_HOLD))
				SendMsg(MSG_RIGHTDOWN_HOLDKEY);
			else if((state==HAL_KEY_STATE_HOLD_UP))
				SendMsg(MSG_RIGHTDOWN_HOLDKEY_UP);			
		}
	}
}





