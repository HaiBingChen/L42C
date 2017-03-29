



#include "Typedef.h"
#include "app_fifo.h"

extern app_fifo_t  app_fifo;
extern bool Update_flags;	
extern uint16 UartRevValueOffset;

#define DEBUG_ENABLE 	1
#define PRINTF         TRACE





void UartInitial(void);
void UartInit_ChoosePin(uint8 cPin);
void UART_Send(uint8 *buf, int size);
void Uart0PacketParser(void);
void UartPowerOff(void);
void UartPowerOn(void);
void  BootloaderUartInitial(void);
void TRACE(const char *fmt, ...);
void InitUartBuffer(void);




