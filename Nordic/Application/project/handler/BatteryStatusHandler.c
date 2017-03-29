#include "headfile.h"


BatteryStateTAG BatteryState;
uint16 ChargeTime =0;
uint16 BATTERY_FULL_SENCOND;

static void BatteryStatusHandlerInit(int iParam,void *pContext)
{
        BatteryState.MyId = iParam;
        BatteryState.BatteryCharging = 0;
        BatteryState.DisplayTime = BATTERY_0_SENCOND;
        BatteryState.BatteryDispalyType = *((uint8 *)pContext);
}


void SimulatePowerOffKey(uint8 time1, uint16 time2)
{
        osal_start_timerEx(GetAppTaskId(), MSG_RIGHTDOWN_HOLDKEY, time1);
        osal_start_timerEx(GetAppTaskId(), MSG_RIGHTDOWN_HOLDKEY_UP, time2);
}

void ChargeDisplayStateChange(bool flag)
{
#if 0
        if(USB_GO_IN_FLAG == true)
        {
                if(flag == true)
                {
                        if(Powerstate.chargeinterface == true)
                        {
                                SendMsg(MSG_CHARGE_START);
                                Powerstate.chargeinterface = false;
                        }
                }
                else
                {
                        if(Powerstate.chargeinterface == false)
                        {
                                SendMsg(MSG_CHARGE_STOP);
                                Powerstate.chargeinterface = true;
                        }
                        
                }
        }
#endif		
}

void UpdateBatteryStatus(bool SizeFlag,bool OneTime)
{
	BAT_VAULE_E CapacityTemp = BatteryTempPercent;
	if(CapacityTemp >= 100)
		CapacityTemp = 100;
	else if(CapacityTemp >= 80)
		CapacityTemp = 80;
	else if(CapacityTemp >= 60)
		CapacityTemp = 60;
	else if (CapacityTemp >= 40)
		CapacityTemp = 40;
	else if(CapacityTemp >= 20)
		CapacityTemp = 20;
	else 
		CapacityTemp = 5;

	if((USB_GO_IN_FLAG == true) ||(OneTime == 1))
	{
		if(ChargeTime>=BATTERY_FULL_SENCOND)
		{
			BatteryState.BatteryCharging=5;
			ShowBatteryFull();
			return;
		}
		if(SizeFlag)
		{
			ShowCharging(BatteryState.BatteryCharging, CapacityTemp);
		}
		else
		{
			if(G_HOUR_FORMAT==1||G_HOUR_FORMAT==2)
			DisplayMap(BattaryMapRes24x8[BatteryState.BatteryCharging],20,40,24,8);
			else if(G_HOUR_FORMAT==7||G_HOUR_FORMAT==8)
			DisplayMap(BattaryMapRes24x8[BatteryState.BatteryCharging],20,32,24,8);				
		}

		if(BatteryState.BatteryCharging < 5)
			BatteryState.BatteryCharging += 1;
		else
			BatteryState.BatteryCharging = 0;
	}
	else
	{
		if(G_HOUR_FORMAT==1||G_HOUR_FORMAT==2)
			DisplayMap(BattaryMapRes24x8[CapacityTemp - 1],20,40,24,8);
		else if(G_HOUR_FORMAT==7||G_HOUR_FORMAT==8)	
			DisplayMap(BattaryMapRes24x8[CapacityTemp - 1],20,32,24,8);
	}
}

uint16 BatteryStatusHandler(MsgType msg, int iParam, void *pContext)
{
	switch(msg)
	{
		case MSG_HANDLER_BEGIN:
			BatteryStatusHandlerInit(iParam,pContext);
			ShowLEDTime();	 
			return RET_MSG_HANDLED;

		case MSG_HANDLER_END:
			BatteryState.MyId = 0;
			BatteryState.BatteryCharging = 0;
			ShowLEDTime();			
			if(USB_GO_IN_FLAG == false)
				StopSecondEvent();
			return RET_MSG_HANDLED;
#if 0
		case MSG_SYSTEM_SUSPEND:
			if(BatteryState.BatteryDispalyType == BATTERY_LOW_WARNING)
				if(!CheckpCurrentHandler(BatteryState.MyId))
					break;
			return RET_MSG_HANDLED;
#endif
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
		case MSG_TOUCH_UP_SHORTKEY:				/*´¥Ãþ µ¥»÷*/  
		case MSG_TOUCH_UP_DOUBLE_SHORTKEY:		/*´¥Ãþ Ë«»÷*/	
		case MSG_TOUCH_LONGKEY:					/*´¥Ãþ ³¤°´*/
			return RET_MSG_HANDLED;

		case MSG_RIGHTDOWN_SHORTKEY: 				/*ÓÒ¼ü µ¥»÷*/
			if(BatteryState.BatteryDispalyType == BATTERY_LOW_WARNING)
				UnloadHandler(BatteryState.MyId);
			return RET_MSG_HANDLED;
		case MSG_DATE_TIME_FORMAT_UPDATE:	
		case MSG_DISPLAY:            
			SendMsg(MSG_DISPLAY_BATTERY_ROLL_STATE);
		case MSG_LOW_BATTERY:
			if(BatteryState.BatteryDispalyType == BATTERY_CHARGE_NOTICE)
			{
				if(CheckpCurrentHandler(BatteryState.MyId) == true)
					UpdateBatteryStatus(1,1);
				BatteryState.DisplayTime = BATTERY_0_SENCOND;
			}
			else if(BatteryState.BatteryDispalyType == BATTERY_LOW_WARNING)
			{
				ClearScreen();
				if(CheckpCurrentHandler(BatteryState.MyId) == true)
					ShowBatteryLow();
				BatteryState.DisplayTime = BATTERY_0_SENCOND+1;
			}
			return RET_MSG_HANDLED;

		case MSG_CHARGE_START:
			ChargeTime =0;
			ClearScreen();
			if(CheckpCurrentHandler(BatteryState.MyId) == true)		
				UpdateBatteryStatus(1,1);
			BatteryState.DisplayTime = BATTERY_0_SENCOND;
			BatteryState.BatteryDispalyType = BATTERY_CHARGE_NOTICE;
			return RET_MSG_HANDLED;

		case MSG_CHARGE_STOP:	
#if 0			
	              if(!Powerstate.chargeinterface)
	              {
	                    if(Powerstate.chargepowerstate)
	                    {
	                            if(Powerstate.chargepowerstate == 3)
	                            {
	                                    if(USB_GO_IN_FLAG == false){						  
	                                            StopSecondEvent();
	                                    }
	                                    SimulatePowerOffKey(10,100);
	                            }
	                            else
	                            {
	                                    osal_start_timerEx(GetAppTaskId(), MSG_DISPLAY_POWEROFF_STATE, 2000);
	                            }
	                            Powerstate.chargepowerstate = false;
	                            return RET_MSG_HANDLED;
	                    }
	              }
#endif				  
			if(ChargeTime>=BATTERY_FULL_SENCOND)
				osal_start_timerEx(GetAppTaskId(), MSG_DISPALY_BATTERY_FULL_QUIT, 3000);
			else
				UnloadHandler(BatteryState.MyId);
			return RET_MSG_HANDLED;

		case MSG_DISPLAY_POWEROFF_STATE:
			SimulatePowerOffKey(100,500);
			UnloadMultiHandler();
			return RET_MSG_HANDLED;

		case MSG_DISPALY_BATTERY_FULL_QUIT:
			UnloadHandler(BatteryState.MyId);
			return RET_MSG_HANDLED;

		case MSG_DISPLAY_BATTERY_ROLL_STATE:
			StartSecondEvent();
			return RET_MSG_HANDLED;

		case MSG_DISPLAY_BATTERY_STATE:    
			if(BatteryState.BatteryDispalyType == BATTERY_CHARGE_NOTICE)
			{	     
				if(ChargeTime <= BATTERY_FULL_SENCOND)
				{
					if(CheckpCurrentHandler(BatteryState.MyId) == true)
						UpdateBatteryStatus(1,1);
					ChargeTime++;
					if((Powerstate.chargepowerstate > 0)&&(Powerstate.chargepowerstate < 3))
						Powerstate.chargepowerstate += 1;
				}
				else if(BatteryState.DisplayTime < BATTERY_60_SENCOND)
				{
					BatteryState.DisplayTime += 1;
				}
				else if(BatteryState.DisplayTime == BATTERY_60_SENCOND)
				{
					BatteryState.DisplayTime+=1;
					SendSuspendEvent();
					if(Powerstate.chargepowerstate > 0)
						Powerstate.chargepowerstate = 1;
				}
			}
			else if(BatteryState.BatteryDispalyType == BATTERY_LOW_WARNING)
			{
				if(BatteryState.DisplayTime < BATTERY_10_SENCOND)
				{
					if(CheckpCurrentHandler(BatteryState.MyId) == true)
					{
						BatteryState.DisplayTime += 1;
						if((BatteryState.DisplayTime % 2) == 0)
							ShowBatteryLow();
						else
							ClearScreen();
					}
				}
				else if(BatteryState.DisplayTime == BATTERY_10_SENCOND)
				{
					if(CheckpCurrentHandler(BatteryState.MyId))
					{
						SendSuspendEvent();
					}
					BatteryState.DisplayTime += 1;
				}
			}
			return RET_MSG_HANDLED;

		case MSG_BIND_END:
			BindEndDealWith();
			return RET_MSG_HANDLED;
			
		default:
			break;
	}  
	return RET_MSG_UNHANDLED;
}

