
#ifndef DISPLAY_ANCS_H__
#define DISPLAY_ANCS_H__

#include <stdint.h>
#include <stdbool.h>
#include "NordicCmdPackage.h"

typedef enum
{	
	De_INIT,
	De_NEXT,
	De_PRE
}DetailPageDir;

extern uint32_t ANCS_READ_ADDR;
extern uint8_t   LayOut[5];
extern uint8_t   LayOutIndex;
extern uint8_t   DetailIndex;
extern DetailPageDir   DetailDir;
extern uint8_t   SmsFromAndroid; 
extern uint8_t   ReadEnd; 
extern uint8_t   UnknownCheck;
extern const uint32_t SmallMapAddr[18];
uint8_t Display_Ancs_Bigview(MsgUI mu ,uint8_t count);
uint8_t Display_Ancs_LayOut(void);
void AncsTest(void);
void InitDetailParameter(void);

#endif

