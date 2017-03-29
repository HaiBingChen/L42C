/*******************************************************************************
 *
 *		Azoteq IQS263_Example_Code
 *
 *              IQS263_Handler - IQS263 device settings and events
 *
*******************************************************************************/
#include "headfile.h"



#define TP_POWER_PIN                        20	
#define TP_POWER_DIR_OUTPUT      nrf_gpio_cfg_output(TP_POWER_PIN)
#define TP_POWER_HI                        nrf_gpio_pin_set(TP_POWER_PIN)
#define TP_POWER_LO                        nrf_gpio_pin_clear(TP_POWER_PIN)
#define LONG_TOUCH_TIME  					(120)




bool   SliderIntervalFlag = 0;
uint8  SliderEvents=0;
uint32 longTouchCnt=0;
uint8  SlideFlags=0;
uint8  longTouchFlag=0;






void PowerOnTouch(void)
{
        TP_POWER_DIR_OUTPUT;
        TP_POWER_LO;
        delay_10ms;
        TP_POWER_HI;
        delay_10ms;		
	 SlideReadyPinNoWait=false;
        Comms_init();
        delay_10ms;			
}

/*************************** IQS263 DEVICE SETTINGS ***************************/
void IQS263DeviceSettings(void)
{
    uint8 data_buffer[8];

    //for(uint8 i=0; i<8;i++) 
  	//UnlockI2c();
    
    PowerOnTouch();
        
    do
    {
        delay_10ms;
        CommsIQSxxx_start();
        CommsIQS_Read(IQS263_ADDR, DEVICE_INFO, &data_buffer[0], 2);
        CommsIQSxxx_stop();
    }while(data_buffer[0] != 0x3c);

    // Switch the IQS263 into projection mode - if necessary
    #if  0
    data_buffer[0] = SYSTEM_FLAGS_VAL;
    CommsIQSxxx_start();
    CommsIQS_Write(IQS263_ADDR, SYS_FLAGS, &data_buffer[0], 1);
    CommsIQSxxx_stop();
    #endif

    do
    {
        delay_1ms;
        // Set active channels
        data_buffer[0] = ACTIVE_CHS;
        CommsIQSxxx_start();
        CommsIQS_Write(IQS263_ADDR, ACTIVE_CHANNELS, &data_buffer[0], 1);
        CommsIQSxxx_stop();

        delay_1ms;
        CommsIQSxxx_start();
        CommsIQS_Read(IQS263_ADDR, ACTIVE_CHANNELS, &data_buffer[0], 1);
        CommsIQSxxx_stop();
    }while(data_buffer[0] != ACTIVE_CHS);
    
    do
    {
        delay_1ms;
        // Setup touch and prox thresholds for each channel
        data_buffer[0] = PROX_THRESHOLD;
        data_buffer[1] = TOUCH_THRESHOLD_CH1;
        data_buffer[2] = TOUCH_THRESHOLD_CH2;
        data_buffer[3] = TOUCH_THRESHOLD_CH3;
        data_buffer[4] = MOVEMENT_THRESHOLD;
        data_buffer[5] = RESEED_BLOCK;
        data_buffer[6] = HALT_TIME;
        data_buffer[7] = I2C_TIMEOUT;
        CommsIQSxxx_start();
        CommsIQS_Write(IQS263_ADDR, THRESHOLDS, &data_buffer[0], 8);
        CommsIQSxxx_stop();
        
        delay_10ms;
        CommsIQSxxx_start();
        CommsIQS_Read(IQS263_ADDR, THRESHOLDS, &data_buffer[0], 8);
        CommsIQSxxx_stop();
    }while(data_buffer[7] != I2C_TIMEOUT);

    do
    {
        delay_1ms;
        // Set the ATI Targets (Target Counts)
        data_buffer[0] = LOW_POWER;  //for lp mode  if it is not 0
        data_buffer[1] = ATI_TARGET_TOUCH;
        data_buffer[2] = ATI_TARGET_PROX;
        CommsIQSxxx_start();
        CommsIQS_Write(IQS263_ADDR, TIMINGS_AND_TARGETS, &data_buffer[0], 3);
        CommsIQSxxx_stop();

        delay_10ms;
        CommsIQSxxx_start();
        CommsIQS_Read(IQS263_ADDR, TIMINGS_AND_TARGETS, &data_buffer[0], 3);
        CommsIQSxxx_stop();
    }while(data_buffer[1] != ATI_TARGET_TOUCH);
    

    // Set the BASE value for each channel
    #if 0
    data_buffer[0] = MULTIPLIERS_CH0;
    data_buffer[1] = MULTIPLIERS_CH1;
    data_buffer[2] = MULTIPLIERS_CH2;
    data_buffer[3] = MULTIPLIERS_CH3;
    CommsIQSxxx_start();
    CommsIQS_Write(IQS263_ADDR, MULTIPLIERS, &data_buffer[0], 4);
    CommsIQSxxx_stop();
    #endif

    do
    {
        delay_1ms;
        // Setup prox settings
        data_buffer[0] = PROXSETTINGS0_VAL;
        data_buffer[1] = PROXSETTINGS1_VAL;
        data_buffer[2] = PROXSETTINGS2_VAL;
        data_buffer[3] = PROXSETTINGS3_VAL;
        data_buffer[4] = EVENT_MASK_VAL;
        CommsIQSxxx_start();
        CommsIQS_Write(IQS263_ADDR, PROX_SETTINGS, &data_buffer[0], 5);
        CommsIQSxxx_stop();

        delay_10ms;
        CommsIQSxxx_start();
        CommsIQS_Read(IQS263_ADDR, PROX_SETTINGS, &data_buffer[0], 5);
        CommsIQSxxx_stop();
    }while(data_buffer[3] != PROXSETTINGS3_VAL);


    
    // Setup Compensation (PCC)
    #if 0
    data_buffer[0] = COMPENSATION_CH0;
    data_buffer[1] = COMPENSATION_CH1;
    data_buffer[2] = COMPENSATION_CH2;
    data_buffer[3] = COMPENSATION_CH3;
    CommsIQSxxx_start();
    CommsIQS_Write(IQS263_ADDR, COMPENSATION, &data_buffer[0], 4);
    CommsIQSxxx_stop();
    #endif

    #if 0
    do
    {
        delay_1ms;
        // Set timings on the IQS263
        data_buffer[0] = LOW_POWER;  //for lp mode  if it is not 0
        CommsIQSxxx_start();
        CommsIQS_Write(IQS263_ADDR, TIMINGS_AND_TARGETS, &data_buffer[0], 1);
        CommsIQSxxx_stop();

        delay_10ms;
        CommsIQSxxx_start();
        CommsIQS_Read(IQS263_ADDR, TIMINGS_AND_TARGETS, &data_buffer[0], 1);
        CommsIQSxxx_stop();
    }while(data_buffer[0] != LOW_POWER);
    #endif

     do
    {
        delay_1ms;
        // Set gesture timers on IQS263
        data_buffer[0] = TAP_TIMER;
        data_buffer[1] = FLICK_TIMER;
        data_buffer[2] = FLICK_THRESHOLD;
        CommsIQSxxx_start();
        CommsIQS_Write(IQS263_ADDR, GESTURE_TIMERS, &data_buffer[0], 3);
        CommsIQSxxx_stop();

        delay_10ms;
        CommsIQSxxx_start();
        CommsIQS_Read(IQS263_ADDR, GESTURE_TIMERS, &data_buffer[0], 3);
        CommsIQSxxx_stop();
    }while(data_buffer[2] != FLICK_THRESHOLD);

    delay_1ms;
    // Redo ati
    data_buffer[0] = 0x10;
    CommsIQSxxx_start();
    CommsIQS_Write(IQS263_ADDR, PROX_SETTINGS, &data_buffer[0], 1);
    CommsIQSxxx_stop();

    // Wait untill the ATI algorithm is done
    do
    {
        delay_10ms;
        CommsIQSxxx_start();
        CommsIQS_Read(IQS263_ADDR, SYS_FLAGS, &data_buffer[0], 1);
        CommsIQSxxx_stop();
    }while((data_buffer[0] & 0x04) == 0x04);

    delay_10ms;
    delay_10ms;
    delay_10ms;	
     // Setup prox settings
    data_buffer[0] = PROXSETTINGS0_VAL;
    data_buffer[1] = (PROXSETTINGS1_VAL|(1<<6));   //go to event
    data_buffer[2] = (PROXSETTINGS2_VAL);
    data_buffer[3] = PROXSETTINGS3_VAL;
    data_buffer[4] = EVENT_MASK_VAL;
    CommsIQSxxx_start();
    CommsIQS_Write(IQS263_ADDR, PROX_SETTINGS, &data_buffer[0], 5);
    CommsIQSxxx_stop();   

    SlideReadyPinNoWait = true;
    
}
/*******************************************************************************
 *             This function will display the different event
 *             features of the IQS263 IC. This will be done by
 *             toggeling of four different LEDS, to create
 *             patterns identified with each event.
 ******************************************************************************/
void handleEvents(void)
{
    uint8 data_buffer[2];

    //for(uint8 i=0; i<8;i++) 
            //UnlockI2c();
    CommsIQSxxx_start();                                                           
    CommsIQS_Read(IQS263_ADDR, SYS_FLAGS, &data_buffer[0], 2);    
    CommsIQSxxx_stop();                                                           
    
    SliderEvents = data_buffer[1];
    osal_set_event(GetTouchKeyTaskId(), HAL_SLIDER_EVENT); 
}


void SliderDispatch(void)
{
    if(!Powerstate.powerstate) 
    {
        return;
    }
    
    SetSystemSuspendTime(DISPLAY_TIME_10S_FLAG);
    /******************************* FLICK (RIGHT) ****************************/

    if((SliderEvents & 0x80) == 0x80)
            flickRight();

    /******************************* FLICK (LEFT) *****************************/

    else if((SliderEvents & 0x40) == 0x40)
            flickLeft();

    /********************************* TAP ************************************/

    else if((SliderEvents & 0x20) == 0x20)
            tapEvent();
	
	/******************************** SLIDE ***********************************/

    else if (((SliderEvents & 0x04) == 0x04) && (SliderEvents != 0))
           slideEvent();
	
	else if (((SliderEvents & 0xFE) == 0x00))
    		longTouchFlag=0;


}

void slideEvent(void) 
{
	if (SlideFlags == 1)
	{
		SlideFlags = 0;
		longTouchCnt = 0;
	}
	
	if( (longTouchFlag == 0) && ((++longTouchCnt)>LONG_TOUCH_TIME))
	{
		SendMsg(MSG_TOUCH_LONGKEY);
		longTouchFlag=1;
		longTouchCnt=0;
	} 
}


/*******************************************************************************
 *      This function handles a tap event, each time this event is
 *      triggered ALL LEDS will flash 3 times.
 ******************************************************************************/
void tapEvent(void)
{
        uint8 KeysValue=0;
        uint8 KeyState=0;
        KeysValue = HAL_KEY_SW_1;
        KeyState = HAL_KEY_STATE_SHORT_UP;

        SlideFlags = 1;
        HalKeyDistribute(KeysValue, KeyState);
}

/*******************************************************************************
 *     This function handles a left flick event, each time this event is
 *     is triggered, LED 2 will flicker 5 times.
 ******************************************************************************/
void flickLeft(void)
{		
		SlideFlags = 1;
		if(!Powerstate.standbystate)
                //SendMsg(MSG_TOUCH_DOWN_UP_FLICK);
                SendMsg(MSG_TOUCH_UP_DOWN_FLICK);
}

/*******************************************************************************
 *     This function handles a right flick event, each time this event is
 *     is triggered, LED 1 will flicker 5 times.
 ******************************************************************************/
void flickRight(void)
{
		SlideFlags = 1;
        if(!Powerstate.standbystate)
                //SendMsg(MSG_TOUCH_UP_DOWN_FLICK);
                SendMsg(MSG_TOUCH_DOWN_UP_FLICK);
}





