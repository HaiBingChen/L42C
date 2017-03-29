#include "Typedef.h"







#define CHINESE_FONT_SIZE_W 16
#define CHINESE_FONT_SIZE_H 16

#define EN_FONT_SIZE_W 9
#define EN_FONT_SIZE_H 16




#if (DISPLAY_WATCH_TIME_TYPE==0)
#define DISP_SIZE_LEN  6
#else
#define DISP_SIZE_LEN  9
#endif



#define SSD13XX_ENTER_SLEEP 0x00
#define SSD13XX_EXIT_SLEEP  0x01
extern uint8 OLED_VERSION_CS;
extern uint8 Y_OFFSET;





void OledIoIint(void);
void Ssd13xxInit(void);
void Write_Command(unsigned char Data);
void Write_Data(unsigned char Data);
void Set_Contrast_Control(unsigned char d);
void DisplayMap(const uint8 *Map,uint8 x,uint8 y,uint8 w,uint8 h);
void Ssd13xxSleep(unsigned char a);
void ClearLcdDisplay(uint8 x,uint8 y,uint8 w,uint8 h);
void SetLcdDisplay(uint8 x,uint8 y,uint8 w,uint8 h);
void DrawLine(uint8 x,uint8 y,uint8 w,uint8 h);






