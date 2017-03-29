#ifndef	_TPM_TIMER_H
#define _TPM_TIMER_H

#include "typedef.h"
#include "FGPIO.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define TPM_TIMER_BASEADDR TPM1
#define TPM_TIMER_HANDLER TPM1_IRQHandler
#define TPM_INTERRUPT_NUMBER TPM1_IRQn


#define TIMER2_BASEADDR TPM2
#define TIMER2_HANDLER TPM2_IRQHandler
#define TIMER2_INTERRUPT_NUMBER TPM2_IRQn

extern uint8 DynDisplay;
extern uint8 TimerFlag;


void tpm_init(void);
void tpm_start(void);
void tpm_stop(void);
void tpm_change_value(uint8 value);
void timer2_init(void);
void timer2_start(void);
void timer2_stop(void);


#endif


