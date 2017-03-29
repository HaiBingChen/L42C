#include "headfile.h"


void ClearScreen(void)
{

}

void LCD_Sleep(unsigned char mode)
{
  switch(mode)
  {
    case LCD_ENTER_SLEEP:
        if(DISPLAY_GOTO_SLEEP_FLAG == false)
        {
			ClearScreen();
			DISPLAY_GOTO_SLEEP_FLAG= true;
			LCD_DISPLAY_FLAG = false;
			NordicPM(N_SUSPEND);
			SetSystemSuspendTime(DISPLAY_STATE_NO_CHANGE);
        }
		break;
    
    case LCD_EXIT_SLEEP:
        if(DISPLAY_GOTO_SLEEP_FLAG == true)
        {
			ClearScreen();
			DISPLAY_GOTO_SLEEP_FLAG=false;
			SetSystemSuspendTime(DISPLAY_TIME_10S_FLAG);		
        }
        break; 
  }
  
}

void DisplayMap(const uint8_t *Map,uint8_t x,uint8_t y,uint8_t width,uint8_t heigh)
{

}

void SetLcdDisplay(uint8_t x,uint8_t y,uint8_t width,uint8_t heigh)
{

}
void ClearLcdDisplay(uint8_t x,uint8_t y,uint8_t width,uint8_t heigh)
{

}



