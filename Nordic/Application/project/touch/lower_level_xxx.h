//*****************************************************************************
//
// If building with a C++ compiler, make all of the definitions in this header
// have a C binding.
//
//*****************************************************************************
#include "nrf_gpio.h"
#include "nrf_delay.h"


//****************************************************************************
// 宏定义 
//****************************************************************************/
//----------------------------------------------------



/*
  DELAY 
*/


extern void DELAY_US (unsigned long cycles);
extern void DELAY_MS (unsigned long cycles);


#define nop()			nrf_delay_us(20)
#define delay_half_bit	nrf_delay_us(30)//;nop();nop();nop();nop();nop();nop();nop();nop();nop();nop();nop();nop();nop();nop();nop()//1us =15 nop
											
												
#define delay_bit	   	nrf_delay_us(25)//;delay_half_bit;delay_half_bit       //2us
#define delay_2bit		nrf_delay_us(25)  //4us				         
#define delay_ack		nrf_delay_us(5)   //10us 
#define delay_1ms     DELAY_MS(5)//DelayUs(730)	
#define delay_10ms    DELAY_MS(10)//DelayUs(7300)	
//****************************************************************************
// 函数申明 
//****************************************************************************/
void Comms_init(void);

uint8_t CommsIQSxxx_send(unsigned char send_data);
uint8_t CommsIQSxxx_read_nack(void);
uint8_t CommsIQSxxx_read_ack(void);
void CommsIQSxxx_start(void);
void CommsIQSxxx_repeat_start(void);
void CommsIQSxxx_stop(void);



//*****************************************************************************
//
// Mark the end of the C bindings section for C++ compilers.
//
//*****************************************************************************



