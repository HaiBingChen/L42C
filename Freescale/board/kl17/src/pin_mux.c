/* ###################################################################
**     This component module is generated by Processor Expert. Do not modify it.
**     Filename    : pin_mux.c
**     Project     : frdmk64f120m-pin
**     Processor   : MK64FN1M0VMD12
**     Component   : PinSettings
**     Version     : Component 01.007, Driver 1.2, CPU db: 3.00.000
**     Compiler    : GNU C Compiler
**     Date/Time   : 2014-06-09, 10:17, # CodeGen: 3
**     Abstract    :
**
**     Settings    :
**
**
**     Copyright : 1997 - 2014 Freescale Semiconductor, Inc. 
**     All Rights Reserved.
**     
**     Redistribution and use in source and binary forms, with or without modification,
**     are permitted provided that the following conditions are met:
**     
**     o Redistributions of source code must retain the above copyright notice, this list
**       of conditions and the following disclaimer.
**     
**     o Redistributions in binary form must reproduce the above copyright notice, this
**       list of conditions and the following disclaimer in the documentation and/or
**       other materials provided with the distribution.
**     
**     o Neither the name of Freescale Semiconductor, Inc. nor the names of its
**       contributors may be used to endorse or promote products derived from this
**       software without specific prior written permission.
**     
**     THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
**     ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
**     WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
**     DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
**     ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
**     (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
**     LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
**     ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
**     (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
**     SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**     
**     http: www.freescale.com
**     mail: support@freescale.com
** ###################################################################*/
/*!
** @file pin_mux.c
** @version 1.2
** @brief
**
*/         
/*!
**  @addtogroup pin_mux_module pin_mux module documentation
**  @{
*/         

/* MODULE pin_mux. */


#include "fsl_device_registers.h"
#include "fsl_port_hal.h"
#include "fsl_sim_hal.h"
#include "pin_mux.h"
#include "fsl_port.h"
#include "fsl_gpio.h"
#include "tpm_timer.h"
#include "fsl_debug_console.h"
#include "display_font.h"

void configure_gpio_pins(uint32_t instance)
{
  switch(instance) {    
    case PORTA_IDX:                      /* PORTA_IDX */
      /* Affects PORTA_PCR4 register */
      PORT_HAL_SetPassiveFilterCmd(PORTA,4u,false);
      PORT_HAL_SetMuxMode(PORTA,4u,kPortMuxAsGpio);
      PORT_HAL_SetPullMode(PORTA,4u,kPortPullUp);
      PORT_HAL_SetPullCmd(PORTA,4u,true);
      
      /* Affects PORTA_PCR13 register */
      PORT_HAL_SetDriveStrengthMode(PORTA,13u,kPortLowDriveStrength);  
      PORT_HAL_SetMuxMode(PORTA,13u,kPortMuxAsGpio);
      PORT_HAL_SetSlewRateMode(PORTA,13u,kPortSlowSlewRate);
      break;
    case PORTB_IDX:                      /* PORTB_IDX */
      /* Affects PORTB_PCR18 register */
      PORT_HAL_SetDriveStrengthMode(PORTB,18u,kPortLowDriveStrength);  
      PORT_HAL_SetMuxMode(PORTB,18u,kPortMuxAsGpio);
      PORT_HAL_SetSlewRateMode(PORTB,18u,kPortSlowSlewRate);
      /* Affects PORTB_PCR19 register */
      PORT_HAL_SetDriveStrengthMode(PORTB,19u,kPortLowDriveStrength);  
      PORT_HAL_SetMuxMode(PORTB,19u,kPortMuxAsGpio);
      PORT_HAL_SetSlewRateMode(PORTB,19u,kPortSlowSlewRate);
      break;
    case PORTC_IDX:                      /* PORTC_IDX */
      /* PORTC_PCR1 */
      PORT_HAL_SetMuxMode(PORTC,1u,kPortMuxAsGpio);

      /* Affects PORTC_PCR2 register */
      PORT_HAL_SetPassiveFilterCmd(PORTC,2u,false);
      PORT_HAL_SetMuxMode(PORTC,2u,kPortMuxAsGpio);
      PORT_HAL_SetPullMode(PORTC,2u,kPortPullUp);
      PORT_HAL_SetPullCmd(PORTC,2u,true);
      
      /* Affects PORTC_PCR3 register */
      PORT_HAL_SetPassiveFilterCmd(PORTC,3u,false);
      PORT_HAL_SetMuxMode(PORTC,3u,kPortMuxAsGpio);
      PORT_HAL_SetPullMode(PORTC,3u,kPortPullUp);
      PORT_HAL_SetPullCmd(PORTC,3u,true);

      break;
    case PORTE_IDX:                      /* PORTE_IDX */
     
      break;
    default:
      break;
  }
}

void configure_i2c_pins(uint32_t instance)
{
  switch(instance) {    
    case I2C0_IDX:                       /* I2C0 */
      /* Affects PORTB_PCR0 register  */
      PORT_HAL_SetMuxMode(PORTB,0u,kPortMuxAlt2);
      /* Affects PORTB_PCR1 register */
      PORT_HAL_SetMuxMode(PORTB,1u,kPortMuxAlt2);
      break;
    case I2C1_IDX:                       /* I2C1 */
      /* Affects PORTD_PCR6 register */
      PORT_HAL_SetMuxMode(PORTD,6u,kPortMuxAlt4);
      /* Affects PORTD_PCR7 register */
      PORT_HAL_SetMuxMode(PORTD,7u,kPortMuxAlt4);
      break;
    default:
      break;
  }
}

void configure_rtc_pins(uint32_t instance)
{
  /* Affects PORTE_PCR0 register */
  PORT_HAL_SetMuxMode(PORTE,0u,kPortMuxAlt4);
}

void configure_spi_pins(uint32_t instance)
{
	switch(instance) {    
		case SPI0_IDX:                       /* SPI0 */
			/* Affects PORTC_PCR4 register */
			//PORT_HAL_SetMuxMode(PORTC,4u,kPortMuxAsGpio);
			/* Affects PORTC_PCR5 register */
			PORT_HAL_SetMuxMode(PORTC,5u,kPortMuxAlt2);
			/* Affects PORTC_PCR6 register */
			PORT_HAL_SetMuxMode(PORTC,6u,kPortMuxAlt2);
			/* Affects PORTC_PCR7 register */
			PORT_HAL_SetMuxMode(PORTC,7u,kPortMuxAlt2);
			break;
			
		case SPI1_IDX:                       /* SPI1 */
			/* Affects PORTD_PCR4 register */
			//PORT_HAL_SetMuxMode(PORTD,4u,kPortMuxAsGpio);

			PORT_SetPinMux(PORTB, 0u, kPortMuxAsGpio);
			GPIO_PinInit(GPIOB, 0u, \
			&(gpio_pin_config_t){kGPIO_DigitalOutput, (1)}) ;/*!< SET OUTPUT */

			/* Affects PORTD_PCR7 register */
			PORT_HAL_SetMuxMode(PORTD,7u,kPortMuxAlt2);
			/* Affects PORTD_PCR5 register */
			PORT_HAL_SetMuxMode(PORTD,5u,kPortMuxAlt2);
			/* Affects PORTD_PCR6 register */
			PORT_HAL_SetMuxMode(PORTD,6u,kPortMuxAlt2);
			break;
			
		case SPI1_INPUT:
			PORT_SetPinMux(PORTB, 0u, kPortMuxAsGpio);
			PORT_SetPinMux(PORTD, 7u, kPortMuxAsGpio);
			PORT_SetPinMux(PORTD, 5u, kPortMuxAsGpio);
			PORT_SetPinMux(PORTD, 6u, kPortMuxAsGpio);
			GPIO_PinInit(GPIOB, 0u, &(gpio_pin_config_t){kGPIO_DigitalInput, (1)}) ;/*!< SET INPUT */
			GPIO_PinInit(GPIOD, 7u, &(gpio_pin_config_t){kGPIO_DigitalInput, (1)}) ;/*!< SET INPUT */
			GPIO_PinInit(GPIOD, 5u, &(gpio_pin_config_t){kGPIO_DigitalInput, (1)}) ;/*!< SET INPUT */
			GPIO_PinInit(GPIOD, 6u, &(gpio_pin_config_t){kGPIO_DigitalInput, (1)}) ;/*!< SET INPUT */	 
			break;
			
		default:
			break;
	}
}

void Configure_Uart0_To_Gpio(void)
{
	PORT_SetPinMux(PORTA, 1u, kPortMuxAsGpio);
	PORT_SetPinMux(PORTA, 2u, kPortMuxAsGpio);	
	GPIO_PinInit(GPIOA, 1u, \
	&(gpio_pin_config_t){kGPIO_DigitalInput, (1)}) ;/*!< SET INPUT */
	GPIO_PinInit(GPIOA, 2u, \
	&(gpio_pin_config_t){kGPIO_DigitalInput, (1)}) ;/*!< SET INPUT */
	PORT_HAL_SetPinIntMode((PORT_Type *)PORTA_BASE, 1,kPortIntDisabled);
	PORT_HAL_SetPinIntMode((PORT_Type *)PORTA_BASE, 2,kPortIntDisabled);

	/*Configuere PTA1 PTA2 to lpuart0*/
	PORT_HAL_SetMuxMode(PORTA,1u,kPortMuxAlt2);
	PORT_HAL_SetMuxMode(PORTA,2u,kPortMuxAlt2);	
}

void SetUart0Pins(uint8_t flag)
{
	if(flag==0)
	{
		PORT_SetPinMux(PORTA, 1u, kPortMuxAsGpio);
		PORT_SetPinMux(PORTA, 2u, kPortMuxAsGpio);			
	}
	else if(flag == 1)
	{
		PORT_HAL_SetMuxMode(PORTA,1u,kPortMuxAlt2);
		PORT_HAL_SetMuxMode(PORTA,2u,kPortMuxAlt2);			
	}
}

void SetBackLight(uint8_t on)
{
	if(on==0)
	{
		LCD_Sleep(0);
		GPIO_ClearPinsOutput(GPIOC, (1U << 10));
		StopRoll();
		tpm_stop();
	}
	else if(on==1)
	{
		//ClearFullScreen();
		LCD_Sleep(1);
		//GPIO_SetPinsOutput(GPIOC, (1U << 10));
		//tpm_start();
	}
}

void ResumeSetBackLight(void)
{
	GPIO_SetPinsOutput(GPIOC, (1U << 10));
	tpm_start();	
}

void Configure_BL_To_Gpio(void)
{
	PORT_SetPinMux(PORTC, 10u, kPortMuxAsGpio);
	GPIO_PinInit(GPIOC, 10u, \
		&(gpio_pin_config_t){kGPIO_DigitalOutput, (0)}) ;/*!< SET OUTPUT */	
	SetBackLight(0);
}

void SetPowerOffPin(uint8_t on)
{
	if(on==0)
	{
		GPIO_ClearPinsOutput(GPIOC, (1U << 0));
	}
	else if(on==1)
	{
		GPIO_SetPinsOutput(GPIOC, (1U << 0));
	}
}

void Configure_POWEROFF_To_Gpio(void)
{
	PORT_SetPinMux(PORTC, 0u, kPortMuxAsGpio);
	GPIO_PinInit(GPIOC, 0u, \
		&(gpio_pin_config_t){kGPIO_DigitalOutput, (0)}) ;/*!< SET OUTPUT */	
	SetPowerOffPin(0);
}

void configure_lpuart_pins(uint32_t instance)
{
  switch(instance) {    
    case 0:                      /* LPUART0 */
      /* Affects PORTA_PCR1 register */
      PORT_HAL_SetMuxMode(PORTA,1u,kPortMuxAlt2);
      /* Affects PORTA_PCR2 register */
      PORT_HAL_SetMuxMode(PORTA,2u,kPortMuxAlt2);
      break;
    case 1:                      /* LPUART0 */
      /* Affects PORTA_PCR1 register */
      PORT_HAL_SetMuxMode(PORTE,0u,kPortMuxAlt3);
      /* Affects PORTA_PCR2 register */
      PORT_HAL_SetMuxMode(PORTE,1u,kPortMuxAlt3);
      break;
    case 2:                      /* LPUART0 */
      /* Affects PORTA_PCR1 register */
      PORT_HAL_SetMuxMode(PORTE,20u,kPortMuxAlt4);
      /* Affects PORTA_PCR2 register */
      PORT_HAL_SetMuxMode(PORTE,21u,kPortMuxAlt4);
      break;
    default:
      break;
  }
}

void configure_gpio_i2c_pins(uint32_t instance)
{
  PORT_HAL_SetMuxMode(PORTD,6u,kPortMuxAsGpio);
  PORT_HAL_SetMuxMode(PORTD,7u,kPortMuxAsGpio);
}

void configure_cmp_pins(uint32_t instance)
{
    PORT_HAL_SetMuxMode(PORTC, 6u, kPortPinDisabled);
    PORT_HAL_SetMuxMode(PORTC, 5u, kPortMuxAlt6);
}


void config_adc_se0_pins(uint32_t instance)
{
    switch(instance) {
      case 0:
        PORT_HAL_SetMuxMode(PORTE, 20u, kPortPinDisabled);
        break;
      default:
        break;
    }
}

void configure_tpm_pins(uint32_t instance)
{
  switch(instance) {    
    case 0:
      PORT_HAL_SetMuxMode(PORTA,5u,kPortMuxAlt3);
      break;
    case 1:
      PORT_HAL_SetMuxMode(PORTA,13u,kPortMuxAlt3);  /* LED BLUE */
      break;
    case 2:
        
      PORT_HAL_SetMuxMode(PORTB,18u,kPortMuxAlt3);  /* LED RED */
      PORT_HAL_SetMuxMode(PORTB,19u,kPortMuxAlt3);  /* LED GREEN */
      break;
    default:
      break;
  }
}

void configure_flexio_pins(uint32_t instance, uint32_t pinIdx)
{
  switch(instance) {    
    case 0:
		switch(pinIdx)
		    {
			    case 0:
				PORT_HAL_SetMuxMode(PORTD,0u,kPortMuxAlt6);
				break;
				case 1:
				PORT_HAL_SetMuxMode(PORTD,1u,kPortMuxAlt6);
				break;
				case 2:
				PORT_HAL_SetMuxMode(PORTD,2u,kPortMuxAlt6);
				break;
				case 3:
				PORT_HAL_SetMuxMode(PORTD,3u,kPortMuxAlt6);
				break;
				case 4:
				PORT_HAL_SetMuxMode(PORTD,4u,kPortMuxAlt6);
				break;
				case 5:
				PORT_HAL_SetMuxMode(PORTD,5u,kPortMuxAlt6);
				break;
				case 6:
				PORT_HAL_SetMuxMode(PORTD,6u,kPortMuxAlt6);
				break;
				case 7:
				PORT_HAL_SetMuxMode(PORTD,7u,kPortMuxAlt6);
				break;
				default:
				  break;
			}

      break;
    default:
      break;
  }
}

/* END pin_mux. */
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
