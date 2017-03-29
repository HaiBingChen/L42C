
/*********************************************************************
 * INCLUDES
 */
#include "Typedef.h"

/*********************************************************************
 * CONSTANTS
 */

/*********************************************************************
 * MACROS
 */

#define     MOTOR_PIN_OUT           nrf_gpio_cfg_output(GPIO_MOTOR_PIN)
#define     MOTOR_PIN_H                nrf_gpio_pin_set(GPIO_MOTOR_PIN)
#define     MOTOR_PIN_L                nrf_gpio_pin_clear(GPIO_MOTOR_PIN)
#define     MOTOR_PIN_SLEEP        nrf_gpio_cfg_input(GPIO_MOTOR_PIN,NRF_GPIO_PIN_PULLDOWN)

//其他信息提醒
typedef enum 
{
	MOTO_ANTI_LOST=0x00,
	MOTO_REMIND,
	MOTO_INCOMING_CALL,
	MOTO_MISSED_CALL,	
	MOTO_SMS,
	MOTO_SOCIAL,
	MOTO_EMIL,
	MOTO_SCHEDULE,
	MOTO_SIT,
	MOTO_LOW_POWER,
	MOTO_DEFAULT,
	MOTO_1S
}MOTO_T;

/*********************************************************************/
extern uint16  TimeoutCnt;



/*********************************************************************
*********************************************************************/
extern void InitMotorVibrate(void);
extern uint8 GetMotorVibrateTimes(void);
extern void StopMotorRunTime(void);
extern void MotorRunTime(uint8 nTimes, uint16 Duration, uint16 Interval, bool mix);
extern void MotoRun(MOTO_T type);
void MotorRepeatRun(void);










