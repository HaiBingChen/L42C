
#include "headfile.h"
#include "app_pwm.h"




static bool bInited = true;

APP_PWM_INSTANCE(PWM1,1);                   // Create the instance "PWM1" using TIMER1.
app_pwm_config_t pwm1_cfg = APP_PWM_DEFAULT_CONFIG_1CH(PWM_TS, GPIO_MOTOR_PIN);

static volatile bool ready_flag=false;            // A flag indicating PWM status.


void PWM_ready_callback(uint32_t pwm_id)    // PWM callback function
{
    ready_flag = true;
}

void PWM_init(void)
{
	ret_code_t err_code;

	pwm1_cfg.pin_polarity[0] = APP_PWM_POLARITY_ACTIVE_LOW;   
	err_code = app_pwm_init(&PWM1,&pwm1_cfg,PWM_ready_callback);
	APP_ERROR_CHECK(err_code);
	app_pwm_enable(&PWM1);
	while(app_pwm_channel_duty_set(&PWM1, 0, 100) == NRF_ERROR_BUSY);
}

void PWM_uninit(void)
{
	app_pwm_uninit(&PWM1);
}


void PWM_SetValue(uint32 value)
{
	while(app_pwm_channel_duty_set(&PWM1, 0, 100-value) == NRF_ERROR_BUSY);
	nrf_delay_ms(20); 
}

void PWM_Enable(void)
{
	if(bInited == false)
	{
		PWM_init();
		bInited = true;
	}
}

void PWM_Disable(void)
{
	if(bInited == true)
	{
		PWM_uninit();
		bInited = false;
	}
}

void Set_Motor_Level(void)
{
	PWM_SetValue(shortcut_info.vibration_level);
}

void Set_Motor_LevelLow(void)
{
	PWM_SetValue(0);
}
void Save_Motor_Level(uint8 value)
{
	PWM_SetValue(value);
	shortcut_info.vibration_level = value;
	WRITE_USER_PARAMETER((uint8 *)&shortcut_info, SHORTCUT_LOC, SHORTCUT_LEN);
}
uint8 Get_Motor_Level(void)
{
	return shortcut_info.vibration_level;
}

