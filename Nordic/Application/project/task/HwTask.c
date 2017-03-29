#include "headfile.h"


typedef struct 
{
	 bool		 TwentyPrcent;
        bool               TenpPrcent; 
        bool               FivePercent; 
} BatteryLOWTAG;

UploadPayRecordStateT gUploadPayRecordDataState=UPLOAD_RECORD_NO;
BatteryLOWTAG BatteryLowAlarm;

uint8   HwTaskId;
uint8   BatteryCheckTimes=0xFF;
uint8   BatteryTempPercent=0;
bool    TouchInterruptFlag=false;
bool    SlideReadyPinNoWait=false;
bool    SendChargeMessageFlag=false;
bool    SkipTouchIntFlag = false;
uint16 AdcResult=0;


extern uint16 BATTERY_FULL_SENCOND;


/******************************************************************************************	
ADC初始化	
******************************************************************************************/
uint8 ADC_LastChannel =0;
uint16 AdcRead(uint8 channel)
{
	uint16 result =0,AdcCount=0;
	if(channel!=ADC_LastChannel)
	{
		uint8 ADC_INPSEL = 0;
		NRF_ADC->ENABLE     = ADC_ENABLE_ENABLE_Disabled;
		if(channel==ADC_KEY_CHANNEL)
			ADC_INPSEL = ADC_CONFIG_INPSEL_AnalogInputOneThirdPrescaling;
		else if (channel == ADC_BAT_CHANNEL)
			ADC_INPSEL = ADC_CONFIG_INPSEL_AnalogInputNoPrescaling;
		
		NRF_ADC->CONFIG = ADC_RESOLUTION << ADC_CONFIG_RES_Pos    |
		                                   ADC_INPSEL << ADC_CONFIG_INPSEL_Pos |
		                                   ADC_CONFIG_REFSEL_VBG << ADC_CONFIG_REFSEL_Pos |
		                                   channel << ADC_CONFIG_PSEL_Pos;		
		ADC_LastChannel =channel;
		NRF_ADC->ENABLE     = ADC_ENABLE_ENABLE_Enabled;
	}
	
	NRF_ADC->TASKS_START = 1;
	while((NRF_ADC->BUSY & 1)&&((AdcCount++)<=10000))
	{
		nrf_delay_us(1);
	}
	if(AdcCount>10000)
		return 0xFFFF;	
	result =  NRF_ADC->RESULT;
	return result;	
}

void GetBatVelue(void)
{
	uint16 result = AdcRead(ADC_BAT_CHANNEL);
	if(result!=0xFFFF)
	{
		AdcResult = result;
		BatCheck();
		osal_set_event(GetHwTaskId(), BAT_CALCULATE_CAPACITY_EVENT); 	
	}
}

/******************************************************************************************
*函数名:		BatteryAdcMeasure
*参数:			
*返回值:		
*描述:			初始化硬件
*编写:			 
*版本信息:		
******************************************************************************************/
void BatteryAdcMeasure(void)
{
        osal_set_event(GetHwTaskId(), HW_CHECK_BAT_EVENT);  
}
/******************************************************************************************
*函数名:		HwInitTask
*参数:			
*返回值:		
*描述:			硬件管理任务初始化
*编写:			 
*版本信息:		
******************************************************************************************/
void HwInitTask(uint8 task_id)
{
        HwTaskId = task_id;
}

/******************************************************************************************
*函数名:		GetHwTaskId
*参数:			
*返回值:		
*描述:			得到硬件任务的ID
*编写:			
*版本信息:	   
******************************************************************************************/
uint8 GetHwTaskId(void)
{
        return HwTaskId;
}

/******************************************************************************************
*函数名:		BatChargeInit
*参数:			
*返回值:		
*描述:			电池充电初始化
*编写:			luohuigui 
*版本信息:		v1.0
******************************************************************************************/
bool BatChargeInit(void)
{
	return CheckChargerConnectEvent();
}

void Bc5PowerPinInit(void)
{
	//nrf_gpio_cfg_output(BC5_POWER_PIN);
}
void SetBc5Power(bool is_on)
{
	if(is_on)
	{
		//UartInit_ChoosePin(BC5_PINS);
		//nrf_gpio_pin_set(BC5_POWER_PIN);	/*开电*/
	}	
	else								
	{
		//nrf_gpio_pin_clear(BC5_POWER_PIN);	/*关电*/
		UartInit_ChoosePin(USB_PINS);
	}
}


void ResetBoard(void)
{
	NVIC_SystemReset();//硬复位
}


BAT_VAULE_E BatCheck(void)
{
        uint16 Sample;
        BAT_VAULE_E  Capacity=0;        

        Sample = AdcResult;
        Sample = Sample * 4+Sample*2/10+Sample*2/100;
        
        if(Sample < 3498u)
                Sample = 3498u;
        
        Capacity = (((Sample - 3498u)* 10) /((4098u - 3498u)/10)); 

        if(Capacity > 0x2c)
                Capacity = Capacity + 3;
        else if(Capacity > 0x03)
                Capacity = Capacity-3;
        else 
                Capacity = 0;
        
        if(Capacity > 0x64)
                Capacity = 0x64;

        if(BatteryTempPercent == 0)
                BatteryTempPercent = Capacity;
        else if(Capacity < BatteryTempPercent)
        {
                if((BatteryTempPercent -Capacity) < 3)
                        BatteryTempPercent -= 1;
                else
                        BatteryTempPercent -= 2;
        }
        
        return BatteryTempPercent;
}


uint16 HwEventHandler(uint8 task_id, uint16 events)
{
		
        if (events & HW_CHECK_BAT_EVENT)
	{ 
	        if((USB_GO_IN_FLAG==false) && (Powerstate.powerstate== true))     
		{
                        GetBatVelue();
                }
                
                return ( events ^ HW_CHECK_BAT_EVENT );
	}

        
	if (events & BAT_CALCULATE_CAPACITY_EVENT)//电池检测
	{
		if((USB_GO_IN_FLAG==false) && (Powerstate.powerstate== true))     /*低电报警检测*/
		{
			   if(((BatteryTempPercent <= 21) && (BatteryTempPercent >= 19)) && (BatteryLowAlarm.TwentyPrcent == false))
			   {
				   BatteryCheckTimes = 0;
				   BatteryLowAlarm.TwentyPrcent = true;
			   }
                        if(((BatteryTempPercent <= 11) && (BatteryTempPercent >= 9)) && (BatteryLowAlarm.TenpPrcent == false))
                        {
                                BatteryCheckTimes = 0;
                                BatteryLowAlarm.TenpPrcent = true;
                        }
                        else if(((BatteryTempPercent <= 6) && (BatteryTempPercent >= 3)) && (BatteryLowAlarm.FivePercent == false))
                        {
                                BatteryCheckTimes = 0;
                                BatteryLowAlarm.FivePercent = true;
                        }
                        
    			if(BatteryCheckTimes==1)
    			{
    			        BatteryCheckTimes = 0xFF;
                                SendMsg(MSG_LOW_BATTERY);                                            
    				MotoRun(MOTO_LOW_POWER);
    			}
                        else if(BatteryCheckTimes==0)
                        {
                                BatteryCheckTimes = 1;
                        }
                        else 
                        {
                                if(BatteryTempPercent < 2)  /*低电关机检测*/
                                        osal_start_timerEx(GetHwTaskId(), BATTERY_VERY_LOW_EVENT,5000); 
                        }
                        
		}
          
                return ( events ^ BAT_CALCULATE_CAPACITY_EVENT );
	}


        if (events & BATTERY_VERY_LOW_EVENT)
	{ 
                SendMsg(MSG_POWER_OFF_BATTERY);
    
                return ( events ^ BATTERY_VERY_LOW_EVENT );
	}
        

        if (events & HW_USB_IN_DEBOUNCE_EVENT)
	{                
                CheckChargerConnectEvent();
                SendChargeMessage();
                if(USB_GO_IN_FLAG==true)
                        BATTERY_FULL_SENCOND = ((100 - BatteryTempPercent)*30 + 600);
    
                return ( events ^ HW_USB_IN_DEBOUNCE_EVENT );
	}


	if (events & BAT_CHARGE_CONNET_EVENT)//充电连接
	{
                BatteryCheckTimes = 0xFF;
                BatteryLowAlarm.FivePercent=false;
                BatteryLowAlarm.TenpPrcent=false;
                BatteryLowAlarm.TwentyPrcent=false;
				
                if(Powerstate.powerstate== false)
                {
                        Powerstate.powerstate= true;
                        //Powerstate.chargepowerstate = 3;
                        Powerstate.chargepowerstate = false;//此项目不做关机充电。
                        SystemPowerOn();
                }
                    		
		return ( events ^ BAT_CHARGE_CONNET_EVENT );
	}


	if (events & BAT_CHARGE_DISCONNET_EVENT)//电池充电usb拿开
	{        
                BatteryTempPercent = 0;
                GetBatVelue();
                return ( events ^ BAT_CHARGE_DISCONNET_EVENT );
	}


        if(events & HW_POWER_ON_USB_EVENT)     /*  充电图标显示或取消*/
	{        
                if(Powerstate.chargepowerstate == 3)
                {
        	        if(SendChargeMessageFlag)
                        {   
                                if(!CheckChargerConnectEvent())
                                        SendChargeMessage();
                        }
                        else
                                CheckPowerOnUsbStateEvent();
                }
                else
                {
                        if(CheckChargerConnectEvent())
                                SendChargeMessage();
                }
		
        	return ( events ^ HW_POWER_ON_USB_EVENT );
        }


        if(events & HW_POWER_OFF_USB_EVENT)     /*  充电图标显示或取消*/
	{
	        if(Powerstate.powerstate== false)
                {   
	                if(CheckChargerConnectEvent())
                                SendChargeMessage();
                }
                else
                        CheckPowerOffUsbStateEvent();
		
        	return ( events ^ HW_POWER_OFF_USB_EVENT );
        }
        

	if(events & HW_CHECK_CONNECT_USB_EVENT)     /*  充电图标显示或取消*/
	{
		if(USB_GO_IN_FLAG == true)
        {      
			SendMsg(MSG_CHARGE_START);
			CleanAutoSleepFlag();
            SendChargeMessageFlag = true;
        }
		else
        {      
            if(SendChargeMessageFlag)
            {
				SendMsg(MSG_CHARGE_STOP);
                SendChargeMessageFlag = false;
            }
        }
		
        	return ( events ^ HW_CHECK_CONNECT_USB_EVENT );
	}


        if (events & HW_FACTORY_SETTING_EVENT)
	{              
                uint8 InitMark=0;

                FLP_TurnOffBle();
                WRITE_USER_FLAG(&InitMark,INIT_MARK_LOC,INIT_MARK_LEN);
                InitBandInfo();
                ResetBoard();
                return ( events ^ HW_FACTORY_SETTING_EVENT );
	}


        if (events & HW_SYSTEM_UPDATE_RESET_EVENT)
        {        
        	  if(!FORCE_NORDIC_FLASH_FLAG)
		  	Force_NordicFlash();          
                ResetBoard();
                return ( events ^ HW_SYSTEM_UPDATE_RESET_EVENT );
        }


        if (events & HW_MOTOR_REPEAT_WORK_EVENT)
	{              
                MotorRepeatRun();
                
                return ( events ^ HW_MOTOR_REPEAT_WORK_EVENT );
	}
		
	if(events & HW_UART0_POLL) 
	{
		Uart0PacketParser();

		return ( events ^ HW_UART0_POLL );
	}
	

        return ( 0 );
    
}


void CheckUsbInDebounce(void)
{
        osal_start_timerEx(GetHwTaskId(), HW_USB_IN_DEBOUNCE_EVENT,300); 
}

void SetFactorySettingEvent(void)
{
        osal_start_timerEx(GetHwTaskId(), HW_FACTORY_SETTING_EVENT,1500); 
}

void SetBatteryChargeEvent(void)
{
	osal_set_event(GetHwTaskId(), HW_CHECK_CONNECT_USB_EVENT); 
}

void CheckPowerOnUsbStateEvent(void)
{
	osal_start_timerEx(GetHwTaskId(), HW_POWER_ON_USB_EVENT,5000); 
}
void CheckPowerOffUsbStateEvent(void)
{
	osal_start_timerEx(GetHwTaskId(), HW_POWER_OFF_USB_EVENT,5000); 
}
void SetUsbConnctedEvent(void)
{
	osal_set_event(GetHwTaskId(), BAT_CHARGE_CONNET_EVENT);
}
void SetUsbDisconnctedEvent(void)
{
	osal_set_event(GetHwTaskId(), BAT_CHARGE_DISCONNET_EVENT);
}

void SendChargeMessage(void)
{
        SetBatteryChargeEvent();
      
        if(USB_GO_IN_FLAG==true)//设置连接USB事件
        {
                SetUsbConnctedEvent(); 
        }
        else
        {
                SetUsbDisconnctedEvent();
        }

}

void EnableUsbPinInt(uint8 Edge)
{
        if(Edge == 1)
                nrf_gpio_cfg_sense_input(USB_PLUG_IN_PIN, NRF_GPIO_PIN_NOPULL, NRF_GPIO_PIN_SENSE_LOW);
        else
                nrf_gpio_cfg_sense_input(USB_PLUG_IN_PIN, NRF_GPIO_PIN_NOPULL, NRF_GPIO_PIN_SENSE_HIGH);

	//CYPRESS_INT_FLAG = false;	
	//nrf_gpio_cfg_sense_input(TOUCH_READY_PIN, NRF_GPIO_PIN_NOPULL, NRF_GPIO_PIN_SENSE_LOW);  //下降沿触发
	//CYPRESS_INT_FLAG = false;		
}

void DisableReset(void)
{
	KL17_Power_Enble();
	nrf_gpio_pin_dir_set(KEY_PIN, NRF_GPIO_PIN_DIR_OUTPUT);
	nrf_gpio_pin_set(KEY_PIN);	
}

void DisableKl17(void)
{
	nrf_gpio_pin_dir_set(KL17_RESET_PIN, NRF_GPIO_PIN_DIR_OUTPUT);
	nrf_gpio_pin_clear(KL17_RESET_PIN);
	nrf_delay_ms(10);
	nrf_gpio_pin_set(KL17_RESET_PIN);
	nrf_delay_ms(600);
}

void DisableUsbPinInt(void)
{

}

void EnablePowerKeyInt(void)
{
}

void DisablePowerKeyInt(void)
{
}

//唤醒电源键、并且启动按键扫描
void StartKeyScan(void)
{
	POWER_KEY_ENABLE= false;
	SCAN_KEY_ENABLE =true;	
	nrf_gpio_pin_dir_set(KEY_PIN,NRF_GPIO_PIN_DIR_INPUT);
	osal_start_timerEx(GetTouchKeyTaskId(), HAL_KEY_EVENT, 100);
}

//停止电源键、并且设置为中断唤醒
void StopKeyScan(void)
{ 
	osal_stop_timerEx(GetTouchKeyTaskId(), HAL_KEY_EVENT);	
	AdcRead(ADC_BAT_CHANNEL);
	nrf_gpio_cfg_sense_input(KEY_PIN, NRF_GPIO_PIN_NOPULL, NRF_GPIO_PIN_SENSE_LOW);	
	SCAN_KEY_ENABLE =false;
	POWER_KEY_ENABLE=true;
}

void StopTPInt(void)
{
	SkipTouchIntFlag = true;
}
void StartTPInt(void)
{
	SkipTouchIntFlag = false;
}


void EnableScanKeyPinInt(void)
{
}

void DisableScanKeyPinInt(void)
{
}

//唤醒按键、并且启动按键扫描
void StartScanKeyToWakeUp(void)
{
}

void StopScanKeyToSleep(void)
{ 
}

bool CheckTouchKeyEvent(void)
{
	if(nrf_gpio_pin_read(TOUCH_READY_PIN))
	{
	        TOUCHKEY_PRESS_FLAG = false;
	}
	else
	{
	        TOUCHKEY_PRESS_FLAG = true;
	}
        return TOUCHKEY_PRESS_FLAG;
}

bool CheckChargerConnectEvent(void)
{
	GPIOTE_IRQ_TOUCH_Recovery();
	if(nrf_gpio_pin_read(USB_PLUG_IN_PIN))
	{	
		EnableUsbPinInt(1); //下降沿触发
		nrf_delay_us(2);
		USB_GO_IN_FLAG = false;
	}
	else
	{
		EnableUsbPinInt(0); //上升沿触发
		nrf_delay_us(2);
		USB_GO_IN_FLAG = true;
	}
        return USB_GO_IN_FLAG;
}

bool CheckCypressEvent(void)
{
	if(CypressI2C_Init)
	{
		if(nrf_gpio_pin_read(TOUCH_READY_PIN)==0)
		{
			zinitix_touchinfo();
		}
	}
        return 0;
}

void Gpio_Interrupt_Initial(void)
{
        nrf_gpio_cfg_sense_input(TOUCH_READY_PIN, NRF_GPIO_PIN_NOPULL, NRF_GPIO_PIN_SENSE_LOW);
        nrf_gpio_cfg_sense_input(USB_PLUG_IN_PIN, NRF_GPIO_PIN_NOPULL, NRF_GPIO_PIN_SENSE_LOW);
        //nrf_gpio_cfg_sense_input(BC5_INT_PIN,NRF_GPIO_PIN_NOPULL,NRF_GPIO_PIN_SENSE_HIGH);
        
        NRF_GPIOTE->INTENSET = GPIOTE_INTENSET_PORT_Msk;
        sd_nvic_SetPriority(GPIOTE_IRQn, NRF_APP_PRIORITY_HIGH);
        sd_nvic_ClearPendingIRQ(GPIOTE_IRQn);
        NRF_GPIOTE->EVENTS_PORT=0;
        sd_nvic_EnableIRQ(GPIOTE_IRQn);
}


void GPIOTE_IRQ_TOUCH_Recovery(void)
{
	if(nrf_gpio_pin_read(TOUCH_READY_PIN) == 0)
	{
		ZT75xx_I2cWriteCMD(ZINITIX_CLEAR_INT_STATUS_CMD);
		nrf_delay_us(2);
	}  	
}

void GPIOTE_IRQ_USB_Recovery(void)
{
	if((nrf_gpio_pin_read(USB_PLUG_IN_PIN)==1)&&(USB_GO_IN_FLAG))
	{	
		EnableUsbPinInt(1); //下降沿触发
		CheckUsbInDebounce();
	}
	if((nrf_gpio_pin_read(USB_PLUG_IN_PIN)==0)&&(!USB_GO_IN_FLAG))
	{
		EnableUsbPinInt(1); //下降沿触发
		CheckUsbInDebounce();
	}
}

//GPIO总中断
void GPIOTE_IRQHandler(void)
{
	NRF_GPIOTE->EVENTS_PORT=0;

	if((nrf_gpio_pin_read(TOUCH_READY_PIN) == 0))
	{	
		osal_set_event(GetTouchKeyTaskId(),HAL_CYPRESS_ONE_TOUCH_EVENT);
	}

	if(nrf_gpio_pin_read(KL17_INT_PIN) == 0)
	{
		if (Powerstate.powerstate)	{
			SendMsg(MSG_WAKEUP_ACTION);
		}
	}

	if(nrf_gpio_pin_read(USB_PLUG_IN_PIN) == 0)
	{
		if(!USB_GO_IN_FLAG)
			CheckUsbInDebounce();
	}
	else
	{
		if(USB_GO_IN_FLAG)
			CheckUsbInDebounce();
	}

#if 0
	if((nrf_gpio_pin_read(BC5_INT_PIN) == 1))									/*BC5*/
	{
		if((Powerstate.powerstate) && (Powerstate.standbystate))  //休眠唤醒   
		{
			Powerstate.InterruptType = false;
			SendMsg(MSG_SYSTEM_RESUME);
		}
	}
#endif

	if((POWER_KEY_ENABLE)&&(nrf_gpio_pin_read(KEY_PIN) == 0))				/*按键*/
	{
		if(Powerstate.powerstate)    		//休眠唤醒   
		{
			POWER_KEY_ENABLE=false;
			Powerstate.InterruptType = true;
			SendMsg(MSG_SYSTEM_RESUME);	
			if(SLEEP_MODE_FLAG)
			{                                    
				osal_start_timerEx(GetSleepTaskId(), SLEEP_BUTTON_PRESS_EVENT,200);
			}
		}
		else if(!Powerstate.powerstate)	//开机
		{
			StartKeyScan();                 
		}
	}


  	if((nrf_gpio_pin_read(HRM_INT_PIN) == 0)&&(hrRunning ==true) )
	{
		AS7000_SwapData_ExtALG();
	}
	
}



