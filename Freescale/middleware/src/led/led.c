#include "led.h"
#include "fsl_interrupt_manager.h"
#include "fsl_pit_driver.h"
#include "fsl_debug_console.h"
#include "Fsl_port_hal.h"

uint16_t LedTwinkle=0,LedR=0,LedG=0,LedB=0;
uint32_t R_LED_PINS[12]={R_LED0,R_LED1,R_LED2,R_LED3,R_LED4,R_LED5,R_LED6,R_LED7,R_LED8,R_LED9,R_LED10,R_LED11};
uint32_t G_LED_PINS[12]={G_LED0,G_LED1,G_LED2,G_LED3,G_LED4,G_LED5,G_LED6,G_LED7,G_LED8,G_LED9,G_LED10,G_LED11};
uint32_t B_LED_PINS[12]={B_LED0,B_LED1,B_LED2,B_LED3,B_LED4,B_LED5,B_LED6,B_LED7,B_LED8,B_LED9,B_LED10,B_LED11};

void LED_INIT(void)
{
	LedTwinkle=0;	
  PORT_HAL_SetPinIntMode((PORT_Type *)PORTA_BASE, 1,kPortIntDisabled);
	PORT_HAL_SetMuxMode(PORTA, 1u, kPortMuxAsGpio);				/*PTA1*/		
#if !defined (DEBUG_ENABLE) 
	PORT_HAL_SetMuxMode(PORTE, 0u, kPortMuxAsGpio);				/*PTE0*/	
#endif
	PORT_HAL_SetMuxMode(PORTE, 19u, kPortMuxAsGpio);			/*PTE19*/
	PORT_HAL_SetMuxMode(PORTE, 16u, kPortMuxAsGpio);			/*PTE16*/
	PORT_HAL_SetMuxMode(PORTC, 11u, kPortMuxAsGpio);			/*PTC11*/
	PORT_HAL_SetMuxMode(PORTE, 22u, kPortMuxAsGpio);			/*PTE22*/
	PORT_HAL_SetMuxMode(PORTE, 30u, kPortMuxAsGpio);			/*PTE30*/	
	PORT_HAL_SetMuxMode(PORTE, 25u, kPortMuxAsGpio);			/*PTE25*/
	PORT_HAL_SetMuxMode(PORTA, 12u, kPortMuxAsGpio);			/*PTA12*/
	PORT_HAL_SetMuxMode(PORTB, 0u, kPortMuxAsGpio);				/*PTB0*/
	PORT_HAL_SetMuxMode(PORTB, 16u, kPortMuxAsGpio);			/*PTB16*/
	PORT_HAL_SetMuxMode(PORTB, 19u, kPortMuxAsGpio);			/*PTB19*/	
	
	PORT_HAL_SetMuxMode(PORTC, 9u, kPortMuxAsGpio);				/*PTC9*/	
	PORT_HAL_SetMuxMode(PORTE, 1u, kPortMuxAsGpio);				/*PTE1*/
	PORT_HAL_SetMuxMode(PORTE, 17u, kPortMuxAsGpio);			/*PTE17*/
	PORT_HAL_SetMuxMode(PORTA, 0u, kPortMuxAsGpio);				/*PTA0*/
	PORT_HAL_SetMuxMode(PORTA, 3u, kPortMuxAsGpio);				/*PTA3*/
	PORT_HAL_SetMuxMode(PORTE, 23u, kPortMuxAsGpio);			/*PTE23*/
	PORT_HAL_SetMuxMode(PORTE, 31u, kPortMuxAsGpio);			/*PTE31*/
	PORT_HAL_SetMuxMode(PORTA, 4u, kPortMuxAsGpio);				/*PTA4*/
	PORT_HAL_SetMuxMode(PORTA, 13u, kPortMuxAsGpio);			/*PTA13*/
	PORT_HAL_SetMuxMode(PORTB, 2u, kPortMuxAsGpio);				/*PTB2*/
	PORT_HAL_SetMuxMode(PORTB, 17u, kPortMuxAsGpio);			/*PTB17*/
	PORT_HAL_SetMuxMode(PORTC, 0u, kPortMuxAsGpio);				/*PTC0*/

	PORT_HAL_SetMuxMode(PORTC, 10u, kPortMuxAsGpio);			/*PTC10*/
	PORT_HAL_SetMuxMode(PORTD, 2u, kPortMuxAsGpio);				/*PTD2*/
	PORT_HAL_SetMuxMode(PORTE, 18u, kPortMuxAsGpio);			/*PTE18*/
	PORT_HAL_SetMuxMode(PORTE, 21u, kPortMuxAsGpio);			/*PTE21*/
	PORT_HAL_SetMuxMode(PORTE, 20u, kPortMuxAsGpio);			/*PTE20*/
	PORT_HAL_SetMuxMode(PORTE, 29u, kPortMuxAsGpio);			/*PTE29*/
	PORT_HAL_SetMuxMode(PORTE, 24u, kPortMuxAsGpio);			/*PTE24*/
	PORT_HAL_SetMuxMode(PORTA, 5u, kPortMuxAsGpio);				/*PTA5*/
	PORT_HAL_SetMuxMode(PORTB, 1u, kPortMuxAsGpio);				/*PTB1*/
	PORT_HAL_SetMuxMode(PORTB, 3u, kPortMuxAsGpio);				/*PTB3*/
	PORT_HAL_SetMuxMode(PORTB, 18u, kPortMuxAsGpio);			/*PTB18*/
	PORT_HAL_SetMuxMode(PORTC, 2u, kPortMuxAsGpio);				/*PTC2*/


    // Define gpio output pin config structure POWER AND RESET.
    gpio_output_pin_user_config_t outputPin[] = {
        {
            .pinName              = R_LED0,
            .config.outputLogic   = 1,
#if FSL_FEATURE_PORT_HAS_SLEW_RATE
            .config.slewRate      = kPortFastSlewRate,
#endif
#if FSL_FEATURE_PORT_HAS_DRIVE_STRENGTH
            .config.driveStrength = kPortHighDriveStrength,
#endif
        },
#if !defined (DEBUG_ENABLE) 
        {
            .pinName              = R_LED1,
            .config.outputLogic   = 1,
#if FSL_FEATURE_PORT_HAS_SLEW_RATE
            .config.slewRate      = kPortFastSlewRate,
#endif
#if FSL_FEATURE_PORT_HAS_DRIVE_STRENGTH
            .config.driveStrength = kPortHighDriveStrength,
#endif
        },
#endif        
        {
            .pinName              = R_LED2,
            .config.outputLogic   = 1,
#if FSL_FEATURE_PORT_HAS_SLEW_RATE
            .config.slewRate      = kPortFastSlewRate,
#endif
#if FSL_FEATURE_PORT_HAS_DRIVE_STRENGTH
            .config.driveStrength = kPortHighDriveStrength,
#endif
        },
        {
            .pinName              = R_LED3,
            .config.outputLogic   = 1,
#if FSL_FEATURE_PORT_HAS_SLEW_RATE
            .config.slewRate      = kPortFastSlewRate,
#endif
#if FSL_FEATURE_PORT_HAS_DRIVE_STRENGTH
            .config.driveStrength = kPortHighDriveStrength,
#endif
        },
        {
            .pinName              = R_LED4,
            .config.outputLogic   = 1,
#if FSL_FEATURE_PORT_HAS_SLEW_RATE
            .config.slewRate      = kPortFastSlewRate,
#endif
#if FSL_FEATURE_PORT_HAS_DRIVE_STRENGTH
            .config.driveStrength = kPortHighDriveStrength,
#endif
        },
        {
            .pinName              = R_LED5,
            .config.outputLogic   = 1,
#if FSL_FEATURE_PORT_HAS_SLEW_RATE
            .config.slewRate      = kPortFastSlewRate,
#endif
#if FSL_FEATURE_PORT_HAS_DRIVE_STRENGTH
            .config.driveStrength = kPortHighDriveStrength,
#endif
        },
        {
            .pinName              = R_LED6,
            .config.outputLogic   = 1,
#if FSL_FEATURE_PORT_HAS_SLEW_RATE
            .config.slewRate      = kPortFastSlewRate,
#endif
#if FSL_FEATURE_PORT_HAS_DRIVE_STRENGTH
            .config.driveStrength = kPortHighDriveStrength,
#endif
        },
        {
            .pinName              = R_LED7,
            .config.outputLogic   = 1,
#if FSL_FEATURE_PORT_HAS_SLEW_RATE
            .config.slewRate      = kPortFastSlewRate,
#endif
#if FSL_FEATURE_PORT_HAS_DRIVE_STRENGTH
            .config.driveStrength = kPortHighDriveStrength,
#endif
        },
        {
            .pinName              = R_LED8,
            .config.outputLogic   = 1,
#if FSL_FEATURE_PORT_HAS_SLEW_RATE
            .config.slewRate      = kPortFastSlewRate,
#endif
#if FSL_FEATURE_PORT_HAS_DRIVE_STRENGTH
            .config.driveStrength = kPortHighDriveStrength,
#endif
        },
        {
            .pinName              = R_LED9,
            .config.outputLogic   = 1,
#if FSL_FEATURE_PORT_HAS_SLEW_RATE
            .config.slewRate      = kPortFastSlewRate,
#endif
#if FSL_FEATURE_PORT_HAS_DRIVE_STRENGTH
            .config.driveStrength = kPortHighDriveStrength,
#endif
        },
        {
            .pinName              = R_LED10,
            .config.outputLogic   = 1,
#if FSL_FEATURE_PORT_HAS_SLEW_RATE
            .config.slewRate      = kPortFastSlewRate,
#endif
#if FSL_FEATURE_PORT_HAS_DRIVE_STRENGTH
            .config.driveStrength = kPortHighDriveStrength,
#endif
        },
        {
            .pinName              = R_LED11,
            .config.outputLogic   = 1,
#if FSL_FEATURE_PORT_HAS_SLEW_RATE
            .config.slewRate      = kPortFastSlewRate,
#endif
#if FSL_FEATURE_PORT_HAS_DRIVE_STRENGTH
            .config.driveStrength = kPortHighDriveStrength,
#endif
        },
        {
            .pinName              = G_LED0,
            .config.outputLogic   = 1,
#if FSL_FEATURE_PORT_HAS_SLEW_RATE
            .config.slewRate      = kPortFastSlewRate,
#endif
#if FSL_FEATURE_PORT_HAS_DRIVE_STRENGTH
            .config.driveStrength = kPortHighDriveStrength,
#endif
        },
        {
            .pinName              = G_LED1,
            .config.outputLogic   = 1,
#if FSL_FEATURE_PORT_HAS_SLEW_RATE
            .config.slewRate      = kPortFastSlewRate,
#endif
#if FSL_FEATURE_PORT_HAS_DRIVE_STRENGTH
            .config.driveStrength = kPortHighDriveStrength,
#endif
        },
        {
            .pinName              = G_LED2,
            .config.outputLogic   = 1,
#if FSL_FEATURE_PORT_HAS_SLEW_RATE
            .config.slewRate      = kPortFastSlewRate,
#endif
#if FSL_FEATURE_PORT_HAS_DRIVE_STRENGTH
            .config.driveStrength = kPortHighDriveStrength,
#endif
        },
        {
            .pinName              = G_LED3,
            .config.outputLogic   = 1,
#if FSL_FEATURE_PORT_HAS_SLEW_RATE
            .config.slewRate      = kPortFastSlewRate,
#endif
#if FSL_FEATURE_PORT_HAS_DRIVE_STRENGTH
            .config.driveStrength = kPortHighDriveStrength,
#endif
        },
        {
            .pinName              = G_LED4,
            .config.outputLogic   = 1,
#if FSL_FEATURE_PORT_HAS_SLEW_RATE
            .config.slewRate      = kPortFastSlewRate,
#endif
#if FSL_FEATURE_PORT_HAS_DRIVE_STRENGTH
            .config.driveStrength = kPortHighDriveStrength,
#endif
        },
        {
            .pinName              = G_LED5,
            .config.outputLogic   = 1,
#if FSL_FEATURE_PORT_HAS_SLEW_RATE
            .config.slewRate      = kPortFastSlewRate,
#endif
#if FSL_FEATURE_PORT_HAS_DRIVE_STRENGTH
            .config.driveStrength = kPortHighDriveStrength,
#endif
        },
        {
            .pinName              = G_LED6,
            .config.outputLogic   = 1,
#if FSL_FEATURE_PORT_HAS_SLEW_RATE
            .config.slewRate      = kPortFastSlewRate,
#endif
#if FSL_FEATURE_PORT_HAS_DRIVE_STRENGTH
            .config.driveStrength = kPortHighDriveStrength,
#endif
        },
        {
            .pinName              = G_LED7,
            .config.outputLogic   = 1,
#if FSL_FEATURE_PORT_HAS_SLEW_RATE
            .config.slewRate      = kPortFastSlewRate,
#endif
#if FSL_FEATURE_PORT_HAS_DRIVE_STRENGTH
            .config.driveStrength = kPortHighDriveStrength,
#endif
        },
        {
            .pinName              = G_LED8,
            .config.outputLogic   = 1,
#if FSL_FEATURE_PORT_HAS_SLEW_RATE
            .config.slewRate      = kPortFastSlewRate,
#endif
#if FSL_FEATURE_PORT_HAS_DRIVE_STRENGTH
            .config.driveStrength = kPortHighDriveStrength,
#endif
        },
        {
            .pinName              =G_LED9,
            .config.outputLogic   = 1,
#if FSL_FEATURE_PORT_HAS_SLEW_RATE
            .config.slewRate      = kPortFastSlewRate,
#endif
#if FSL_FEATURE_PORT_HAS_DRIVE_STRENGTH
            .config.driveStrength = kPortHighDriveStrength,
#endif
        },
        {
            .pinName              = G_LED10,
            .config.outputLogic   = 1,
#if FSL_FEATURE_PORT_HAS_SLEW_RATE
            .config.slewRate      = kPortFastSlewRate,
#endif
#if FSL_FEATURE_PORT_HAS_DRIVE_STRENGTH
            .config.driveStrength = kPortHighDriveStrength,
#endif
        },
        {
            .pinName              = G_LED11,
            .config.outputLogic   = 1,
#if FSL_FEATURE_PORT_HAS_SLEW_RATE
            .config.slewRate      = kPortFastSlewRate,
#endif
#if FSL_FEATURE_PORT_HAS_DRIVE_STRENGTH
            .config.driveStrength = kPortHighDriveStrength,
#endif
        },
        {
            .pinName              = B_LED0,
            .config.outputLogic   = 1,
#if FSL_FEATURE_PORT_HAS_SLEW_RATE
            .config.slewRate      = kPortFastSlewRate,
#endif
#if FSL_FEATURE_PORT_HAS_DRIVE_STRENGTH
            .config.driveStrength = kPortHighDriveStrength,
#endif
        },
        {
            .pinName              = B_LED1,
            .config.outputLogic   = 1,
#if FSL_FEATURE_PORT_HAS_SLEW_RATE
            .config.slewRate      = kPortFastSlewRate,
#endif
#if FSL_FEATURE_PORT_HAS_DRIVE_STRENGTH
            .config.driveStrength = kPortHighDriveStrength,
#endif
        },
        {
            .pinName              = B_LED2,
            .config.outputLogic   = 1,
#if FSL_FEATURE_PORT_HAS_SLEW_RATE
            .config.slewRate      = kPortFastSlewRate,
#endif
#if FSL_FEATURE_PORT_HAS_DRIVE_STRENGTH
            .config.driveStrength = kPortHighDriveStrength,
#endif
        },
        {
            .pinName              = B_LED3,
            .config.outputLogic   = 1,
#if FSL_FEATURE_PORT_HAS_SLEW_RATE
            .config.slewRate      = kPortFastSlewRate,
#endif
#if FSL_FEATURE_PORT_HAS_DRIVE_STRENGTH
            .config.driveStrength = kPortHighDriveStrength,
#endif
        },
        {
            .pinName              = B_LED4,
            .config.outputLogic   = 1,
#if FSL_FEATURE_PORT_HAS_SLEW_RATE
            .config.slewRate      = kPortFastSlewRate,
#endif
#if FSL_FEATURE_PORT_HAS_DRIVE_STRENGTH
            .config.driveStrength = kPortHighDriveStrength,
#endif
        },
        {
            .pinName              = B_LED5,
            .config.outputLogic   = 1,
#if FSL_FEATURE_PORT_HAS_SLEW_RATE
            .config.slewRate      = kPortFastSlewRate,
#endif
#if FSL_FEATURE_PORT_HAS_DRIVE_STRENGTH
            .config.driveStrength = kPortHighDriveStrength,
#endif
        },
        {
            .pinName              = B_LED6,
            .config.outputLogic   = 1,
#if FSL_FEATURE_PORT_HAS_SLEW_RATE
            .config.slewRate      = kPortFastSlewRate,
#endif
#if FSL_FEATURE_PORT_HAS_DRIVE_STRENGTH
            .config.driveStrength = kPortHighDriveStrength,
#endif
        },
        {
            .pinName              = B_LED7,
            .config.outputLogic   = 1,
#if FSL_FEATURE_PORT_HAS_SLEW_RATE
            .config.slewRate      = kPortFastSlewRate,
#endif
#if FSL_FEATURE_PORT_HAS_DRIVE_STRENGTH
            .config.driveStrength = kPortHighDriveStrength,
#endif
        },
        {
            .pinName              = B_LED8,
            .config.outputLogic   = 1,
#if FSL_FEATURE_PORT_HAS_SLEW_RATE
            .config.slewRate      = kPortFastSlewRate,
#endif
#if FSL_FEATURE_PORT_HAS_DRIVE_STRENGTH
            .config.driveStrength = kPortHighDriveStrength,
#endif
        },
        {
            .pinName              = B_LED9,
            .config.outputLogic   = 1,
#if FSL_FEATURE_PORT_HAS_SLEW_RATE
            .config.slewRate      = kPortFastSlewRate,
#endif
#if FSL_FEATURE_PORT_HAS_DRIVE_STRENGTH
            .config.driveStrength = kPortHighDriveStrength,
#endif
        },
        {
            .pinName              = B_LED10,
            .config.outputLogic   = 1,
#if FSL_FEATURE_PORT_HAS_SLEW_RATE
            .config.slewRate      = kPortFastSlewRate,
#endif
#if FSL_FEATURE_PORT_HAS_DRIVE_STRENGTH
            .config.driveStrength = kPortHighDriveStrength,
#endif
        },
        {
            .pinName              = B_LED11,
            .config.outputLogic   = 1,
#if FSL_FEATURE_PORT_HAS_SLEW_RATE
            .config.slewRate      = kPortFastSlewRate,
#endif
#if FSL_FEATURE_PORT_HAS_DRIVE_STRENGTH
            .config.driveStrength = kPortHighDriveStrength,
#endif
        },        
        {
            .pinName = GPIO_PINS_OUT_OF_RANGE,
        }
    };

    // Init SPI CS
    GPIO_DRV_Init(NULL, outputPin);
	  PORT_HAL_SetPinIntMode((PORT_Type *)PORTA_BASE, 1,kPortIntDisabled);   
}


void LED_ON(uint8_t idx)
{
	if(LedR & (0x01<<idx))
	{
#if defined (DEBUG_ENABLE) 	
		if(R_LED_PINS[idx]!=R_LED1)
#endif			
		GPIO_DRV_ClearPinOutput(R_LED_PINS[idx]);
	}
	else
	{
#if defined (DEBUG_ENABLE) 	
		if(R_LED_PINS[idx]!=R_LED1)
#endif			
		GPIO_DRV_SetPinOutput(R_LED_PINS[idx]);
	}
	if(LedG & (0x01<<idx))		
		GPIO_DRV_ClearPinOutput(G_LED_PINS[idx]);
	else
		GPIO_DRV_SetPinOutput(G_LED_PINS[idx]);
	
	if(LedB & (0x01<<idx))		
		GPIO_DRV_ClearPinOutput(B_LED_PINS[idx]);	
	else
		GPIO_DRV_SetPinOutput(B_LED_PINS[idx]);
}
	

void LED_OFF(uint8_t idx)
{
#if defined (DEBUG_ENABLE) 	
	if(R_LED_PINS[idx]!=R_LED1)
#endif
	GPIO_DRV_SetPinOutput(R_LED_PINS[idx]);

	GPIO_DRV_SetPinOutput(G_LED_PINS[idx]);
	GPIO_DRV_SetPinOutput(B_LED_PINS[idx]);
}

bool LED_Control_Light(uint16_t Light)
{	
	for(uint8_t i =0;i<12;i++)
	{
		if(Light & (0x01<<i))
			LED_ON(i);
		else
			LED_OFF(i);
	}
	return true;
}

bool LED_Control_Twinkle(uint16_t Twinkle)
{
	if(Twinkle==0)
	{
		PIT_DRV_StopTimer(0, 0);
		for(uint8_t i =0;i<12;i++)
		{
			if(LedTwinkle & (0x01<<i))
				LED_OFF(i);
		}		
	}
	else
		PIT_DRV_StartTimer(0, 0); 
	LedTwinkle = Twinkle;
	return true;
}

