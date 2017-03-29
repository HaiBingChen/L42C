/*
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of Freescale Semiconductor, Inc. nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "fsl_debug_console.h"
#include "board.h"
#include "fsl_tpm.h"

#include "fsl_common.h"
#include "pin_mux.h"
#include "tpm_timer.h"
#include "ui.h"


#define TPM_PWM_START				1U
#define TPM_PWM_STOP				2U
#define TPM_TIMER_START				1U
#define TPM_TIMER_STOP				2U

static uint8 tpm_value = 20;
static uint8 count = 0;
static uint8 tpm_pwm_status = TPM_PWM_STOP;
static uint8 tpm_timer_status = TPM_TIMER_STOP;

static uint8 timer_count = 0;
uint8 TimerFlag = 0;
uint8 DynDisplay = 0;

#define LCD_BL_H()		(FGPIOC_PSOR = 1 << 10)
#define LCD_BL_L()		(FGPIOC_PCOR = 1 << 10)


void TPM_TIMER_HANDLER(void)
{
	if (++count <= tpm_value)
	{
		LCD_BL_H();
	}
	else {
		LCD_BL_L();
	}

	if (count == 20) 
		count = 0;
	
	/* Clear interrupt flag.*/
	TPM_ClearStatusFlags(TPM_TIMER_BASEADDR, kTPM_TimeOverflowInterruptEnable);
}

void tpm_init(void)
{
    tpm_config_t tpmInfo;
    CLOCK_SetTpmClock(1U); 
    TPM_GetDefaultConfig(&tpmInfo);
	//tpmInfo.prescale = kTPM_Prescale_Divide_128;   // 分频值 ： 48M/128 = 3/8 MHz
  tpmInfo.prescale = kTPM_Prescale_Divide_64;
     /* Initialize TPM module */
    TPM_Init(TPM_TIMER_BASEADDR, &tpmInfo);
    /* Set the timer to be in free-running mode */
    TPM_TIMER_BASEADDR->MOD = 187;   // 200 Hz interrupt.   3/8M / 200 = 187.5
    TPM_EnableInterrupts(TPM_TIMER_BASEADDR, kTPM_TimeOverflowInterruptEnable); // counter 计数 1875 后产生 overflow 中断。
}

void tpm_start(void)
{
	if (tpm_pwm_status == TPM_PWM_STOP)
	{
    	EnableIRQ(TPM_INTERRUPT_NUMBER);
    	TPM_StartTimer(TPM_TIMER_BASEADDR, kTPM_SystemClock);		
		tpm_pwm_status = TPM_PWM_START;
	}
}

void tpm_stop(void)
{
	if (tpm_pwm_status == TPM_PWM_START)
	{
		DisableIRQ(TPM_INTERRUPT_NUMBER);
		TPM_StopTimer(TPM_TIMER_BASEADDR);
		tpm_pwm_status = TPM_PWM_STOP;
	}
}

void tpm_change_value(uint8 value)
{
	uint8 Setvale=12;
	switch(value)
	{
		case 0:
		case 1:
			Setvale = 2;
			break;
		case 2:
		case 3:	
			Setvale = 4;
			break;
		case 4:
		case 5:
			Setvale = 6;
			break;
		case 6:
		case 7:	
			Setvale = 9;
			break;
		case 8:
		case 9:	
			Setvale = 12;
			break;
		case 10:
			Setvale = 20;
			break;	
		default:
			Setvale = 12;
			break;
	}
	if (tpm_pwm_status == TPM_PWM_START)
	{
		if ((Setvale <= 20) && (Setvale > 0))
			tpm_value = Setvale;
	}
}



void TIMER2_HANDLER (void)
{

	timer_count++;
	if (timer_count > 10)
	{
		timer_count = 0;
		DynDisplay = 0xFF;
	}
	
	/* Clear interrupt flag.*/
	TPM_ClearStatusFlags(TIMER2_BASEADDR, kTPM_TimeOverflowInterruptEnable);
}


void timer2_init(void)
{
    tpm_config_t tpmInfo;
    CLOCK_SetTpmClock(1U); 
    TPM_GetDefaultConfig(&tpmInfo);
    tpmInfo.prescale = kTPM_Prescale_Divide_128;   // 分频值 ： 48M/128 = 3/8 MHz
     /* Initialize TPM module */
    TPM_Init(TIMER2_BASEADDR, &tpmInfo);
    /* Set the timer to be in free-running mode */
    TIMER2_BASEADDR->MOD = 18750;   // 200 Hz interrupt.   3/8M / 200 = 187.5
    TPM_EnableInterrupts(TIMER2_BASEADDR, kTPM_TimeOverflowInterruptEnable); // counter 计数 1875 后产生 overflow 中断。
}


void timer2_start(void)
{
	if (tpm_timer_status == TPM_TIMER_STOP)
	{
    	EnableIRQ(TIMER2_INTERRUPT_NUMBER);
    	TPM_StartTimer(TIMER2_BASEADDR , kTPM_SystemClock);		
		tpm_timer_status = TPM_TIMER_START;
	}
}

void timer2_stop(void)
{
	if (tpm_timer_status == TPM_TIMER_START)
	{
		DisableIRQ(TIMER2_INTERRUPT_NUMBER);
		TPM_StopTimer(TIMER2_BASEADDR);
		tpm_timer_status = TPM_TIMER_STOP;
	}
}


