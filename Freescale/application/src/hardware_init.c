/*
 * Copyright (c) 2013-2014, Freescale Semiconductor, Inc.
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

#include "board.h"
#include "pin_mux.h"
#include "fsl_port_hal.h"
#include "fsl_clock_manager.h"
#include "fsl_debug_console.h"

void configure_pwm_pins(uint32_t instance)
{
  switch(instance) {    
    case 0:
      PORT_HAL_SetMuxMode(PORTD,0u,kPortMuxAlt4);
      break;
    default:
      break;
  }
}

void configure_uart_pins(uint32_t instance)
{
  switch(instance) {    
    case 0:
      PORT_HAL_SetMuxMode(PORTD,3u,kPortMuxAlt3);	
      break;
    default:
      break;
  }
}


void hardware_init(void) {

	/* enable clock for PORTs */
	CLOCK_SYS_EnablePortClock(PORTA_IDX);
	CLOCK_SYS_EnablePortClock(PORTB_IDX);
	CLOCK_SYS_EnablePortClock(PORTC_IDX);
	CLOCK_SYS_EnablePortClock(PORTD_IDX);
	CLOCK_SYS_EnablePortClock(PORTE_IDX);

	/* Init board clock */
	BOARD_ClockInit();
	dbg_uart_init();
	CLOCK_SYS_EnableUartClock(0);
	
	configure_pwm_pins(0U);		//7816 clk
	configure_uart_pins(0U);		//7816 IO
	configure_spi_pins(1U);		//spi
	configure_spi_pins(0U);
	Configure_Uart0_To_Gpio();
	Configure_BL_To_Gpio();
	Configure_POWEROFF_To_Gpio();
}

void disable_unused_pins(void)
{
  /* Disable debug pins when MCU sleeps */
  //setup_debug_pins(kPortMuxAlt4);

  /* Disable uart pins */
  //setup_uart_pins(kPortPinDisabled);
}

void enable_unused_pins(void)
{
  /* Enable debug pins when MCU sleeps */
 // setup_debug_pins(kPortMuxAlt7);

  /* Enable uart pins */
  //setup_uart_pins(kPortMuxAlt2);

}

/*!
** @}
*/
/*
** ###################################################################
**
**     This file was created by Processor Expert 10.4 [05.10]
**     for the Freescale Kinetis series of microcontrollers.
**
** ###################################################################
*/
