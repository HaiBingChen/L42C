/** @file
 * @defgroup nrf_dev_timer_example_main main.c
 * @{
 * @ingroup nrf_dev_timer_example
 * @brief Timer Example Application main file.
 *
 * This file contains the source code for a sample application using Timer0.
 *
 */
 
#include "headfile.h"


uint32 gStopWatchLastMs = 0;
uint32 gStopWatchLastMsSave = 0;
uint32 gStopWatchSysTick_start = 0;


uint8 STHours=0;
uint8 STMinutes=0;
uint8 STSeconds=0;
uint16 STmSeconds=0;
uint8 mSecond=0;
extern uint8 STflag;

const nrf_drv_timer_t TIMER_ST = NRF_DRV_TIMER_INSTANCE(1);

/**
 * @brief Handler for timer events.
 */
void timer_led_event_handler(nrf_timer_event_t event_type, void* p_context)
{
#if 0
	switch(event_type)
	{
            case NRF_TIMER_EVENT_COMPARE0:
                if(STflag==1)
                {
                        mSecond++;
                        if ((mSecond % 10)==0)
                        {
                                Secondstimer_run();
                                if(CheckpCurrentHandler(DifferentRemindId.TimerHandlerId)&&LCD_DISPLAY_FLAG)
                                {
                                        DisplayMap(SeocndsTimerMapRes24x24, 18, 0, 24, 24);
                                        DisplaySecondTimerTime(STMinutes,STSeconds,STmSeconds,3,24,32);
                                }
                        }
                }
                else
                {
                        Secondstimer_stop();
                }
                break;

            default:		
                break;
	}
#endif
}

void SecondsTimerStop(void)
{	
	STui = ST_PAGE;		
}

void SecondsTimerStart(void)
{
	if(STflag==1)
	{
		Secondstimer_run();
		if(STHours>99)
		{
			STHours=99;
			STMinutes=59;
			STSeconds=59;
		}		
		if(CheckpCurrentHandler(DifferentRemindId.TimerHandlerId))
		{
				SecondsTimer(1);
		}
	}
}

/**
 * @brief Function for main application entry.
 */
int SecondsTimer_init(void)
{
#if 0
	uint32_t time_ms = 1; //Time(in miliseconds) between consecutive compare events.
	uint32_t time_ticks;
	uint32_t err_code = NRF_SUCCESS;
    //Configure TIMER_LED for generating simple light effect - leds on board will invert his state one after the other.
	err_code = nrf_drv_timer_init(&TIMER_ST, NULL, timer_led_event_handler);
	APP_ERROR_CHECK(err_code);
    
	time_ticks = nrf_drv_timer_ms_to_ticks(&TIMER_ST, time_ms);
    
	nrf_drv_timer_extended_compare(&TIMER_ST, NRF_TIMER_CC_CHANNEL0, time_ticks, NRF_TIMER_SHORT_COMPARE0_CLEAR_MASK, true);
#endif
	return 0;
}


void SecondsTimerOff(void)
{
	STui = ST_PAGE;
	STflag =0 ;
	STMinutes=0;
	STSeconds=0;
	STmSeconds=0;	
}
void SecondsTimer(uint8 STflag)
{
    
	STMinutes = gStopWatchLastMs/1000/60;
	STSeconds = gStopWatchLastMs/1000%60;
	STmSeconds = (gStopWatchLastMs%1000)/10;
	switch(STflag)
	{
		case 0:
			STHours=0;
			STMinutes=0;
			STSeconds=0;
			STmSeconds=0;
			ShowTimerStatus(TIMER_START,STMinutes,STSeconds,STmSeconds);
			break;
		case 1:
			ShowTimerStatus(TIMER_RUN,STMinutes,STSeconds,STmSeconds);
			break;
		case 2:
			ShowTimerStatus(TIMER_STOP,STMinutes,STSeconds,STmSeconds);
			break;
		default:
			break;
	}
}	

void Secondstimer_run(void)
{
	STmSeconds++;
	if (STmSeconds > 4)
	{
		STSeconds++;
		STmSeconds = 0;
	}

	if(STSeconds>59)
	{
		STSeconds=0;
		STMinutes=STMinutes+1;
		if(STMinutes>59)
		{
			STMinutes=0;
			STHours++;
		}
	}
}

void Secondstimer_stop(void)
{
	//nrf_drv_timer_pause(&TIMER_ST);
	//nrf_drv_timer_disable(&TIMER_ST);
}
void timer_enable(void)
{
	//nrf_drv_timer_enable(&TIMER_ST);
}


/** @} */

