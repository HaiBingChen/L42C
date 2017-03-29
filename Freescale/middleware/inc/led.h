#ifndef _LED_H
#define _LED_H

#include "fsl_spi_master_driver.h"
#include "fsl_gpio_driver.h"
#include "fsl_clock_manager.h"
#include "fsl_debug_console.h"

enum _led_pins_pinNames_R{
	R_LED0 = GPIO_MAKE_PIN(GPIOA_IDX, 1U),				/*PTA1*/
	R_LED1 = GPIO_MAKE_PIN(GPIOE_IDX, 0U),				/*PTE0*/		
	R_LED2 = GPIO_MAKE_PIN(GPIOE_IDX, 19U),			/*PTE19*/
	R_LED3 = GPIO_MAKE_PIN(GPIOE_IDX, 16U),			/*PTE16*/
	R_LED4 = GPIO_MAKE_PIN(GPIOC_IDX, 11U),			/*PTC11*/
	R_LED5 = GPIO_MAKE_PIN(GPIOE_IDX, 22U),			/*PTE22*/
	R_LED6 = GPIO_MAKE_PIN(GPIOE_IDX, 30U),			/*PTE30*/
	R_LED7 = GPIO_MAKE_PIN(GPIOE_IDX, 25U),			/*PTE25*/
	R_LED8 = GPIO_MAKE_PIN(GPIOA_IDX, 12U),			/*PTA12*/
	R_LED9 = GPIO_MAKE_PIN(GPIOB_IDX, 0U),				/*PTB0*/
	R_LED10 = GPIO_MAKE_PIN(GPIOB_IDX, 16U),			/*PTB16*/
	R_LED11 = GPIO_MAKE_PIN(GPIOB_IDX, 19U),			/*PTB19*/	
};


enum _led_pins_pinNames_G{
	G_LED0 = GPIO_MAKE_PIN(GPIOC_IDX, 9U),				/*PTC9*/	
	G_LED1 = GPIO_MAKE_PIN(GPIOE_IDX, 1U),				/*PTE1*/
	G_LED2 = GPIO_MAKE_PIN(GPIOE_IDX, 17U),			/*PTE17*/
	G_LED3 = GPIO_MAKE_PIN(GPIOA_IDX, 0U),				/*PTA0*/
	G_LED4 = GPIO_MAKE_PIN(GPIOA_IDX, 3U),				/*PTA3*/
	G_LED5 = GPIO_MAKE_PIN(GPIOE_IDX, 23U),			/*PTE23*/
	G_LED6 = GPIO_MAKE_PIN(GPIOE_IDX, 31U),			/*PTE31*/
	G_LED7 = GPIO_MAKE_PIN(GPIOA_IDX, 4U),				/*PTA4*/
	G_LED8 = GPIO_MAKE_PIN(GPIOA_IDX, 13U),			/*PTA13*/
	G_LED9 = GPIO_MAKE_PIN(GPIOB_IDX, 2U),				/*PTB2*/
	G_LED10 = GPIO_MAKE_PIN(GPIOB_IDX, 17U),			/*PTB17*/
	G_LED11 = GPIO_MAKE_PIN(GPIOC_IDX, 0U),			/*PTC0*/
};

enum _led_pins_pinNames_B{
	B_LED0 = GPIO_MAKE_PIN(GPIOC_IDX, 10U),			/*PTC10*/
	B_LED1 = GPIO_MAKE_PIN(GPIOD_IDX, 2U),				/*PTD2*/
	B_LED2 = GPIO_MAKE_PIN(GPIOE_IDX, 18U),			/*PTE18*/
	B_LED3 = GPIO_MAKE_PIN(GPIOE_IDX, 21U),			/*PTE21*/
	B_LED4 = GPIO_MAKE_PIN(GPIOE_IDX, 20U),			/*PTE20*/
	B_LED5 = GPIO_MAKE_PIN(GPIOE_IDX, 29U),			/*PTE29*/
	B_LED6 = GPIO_MAKE_PIN(GPIOE_IDX, 24U),			/*PTE24*/
	B_LED7 = GPIO_MAKE_PIN(GPIOA_IDX, 5U),				/*PTA5*/
	B_LED8 = GPIO_MAKE_PIN(GPIOB_IDX, 1U),				/*PTB1*/
	B_LED9 = GPIO_MAKE_PIN(GPIOB_IDX, 3U),				/*PTB3*/
	B_LED10 = GPIO_MAKE_PIN(GPIOB_IDX, 18U),			/*PTB18*/
	B_LED11 = GPIO_MAKE_PIN(GPIOC_IDX, 2U),			/*PTC2*/
};

extern uint16_t LedTwinkle;
extern uint16_t LedR;
extern uint16_t LedG;
extern uint16_t LedB;
#endif

void LED_OFF(uint8_t idx);
void LED_ON(uint8_t idx);
void LED_INIT(void);
void LED_INIT(void);
bool LED_Control_Light(uint16_t Light);
bool LED_Control_Twinkle(uint16_t Twinkle);
