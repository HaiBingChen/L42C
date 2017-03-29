#ifndef _PWM_H
#define _PWM_H

#define PWM_TS 		5000L //uS

#ifndef MOTOR_LEVEL_STANDARD
#define MOTOR_LEVEL_STANDARD    75L
#endif

#define MOTOR_LEVEL_NORMAL		MOTOR_LEVEL_STANDARD

void Set_Motor_Level(void);
uint8 Get_Motor_Level(void);
void Save_Motor_Level(uint8 value);
void Set_Motor_LevelLow(void);
void PWM_Enable(void);
void PWM_Disable(void);
void PWM_init(void);
#endif

