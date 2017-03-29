#include "headfile.h"






#define LCD_SPI_CLK_PIN                  29
#define LCD_SPI_DATA_PIN                30
#define LCD_SPI_DC_PIN                    31
#define LCD_SPI_CS_PIN                    28
#define LCD_REST_PIN                        25
#define LCD_OLED_BK_EN_PIN      

#define LCD_SPI_CLK_OUT                 nrf_gpio_cfg_output(LCD_SPI_CLK_PIN)
#define LCD_SPI_DATA_OUT              nrf_gpio_cfg_output( LCD_SPI_DATA_PIN) 
#define LCD_SPI_DC_OUT                   nrf_gpio_cfg_output( LCD_SPI_DC_PIN) 
#define LCD_SPI_CS_OUT                   nrf_gpio_cfg_output(LCD_SPI_CS_PIN)
#define LCD_SPI_REST_OUT               nrf_gpio_cfg_output( LCD_REST_PIN)

#define SPI_CLK_H                               nrf_gpio_pin_set(LCD_SPI_CLK_PIN)
#define SPI_CLK_L                                nrf_gpio_pin_clear(LCD_SPI_CLK_PIN)
#define SPI_DATA_H                             nrf_gpio_pin_set(LCD_SPI_DATA_PIN)
#define SPI_DATA_L                              nrf_gpio_pin_clear(LCD_SPI_DATA_PIN)
#define SPI_CS_H                                  nrf_gpio_pin_set(LCD_SPI_CS_PIN)
#define SPI_CS_L                                  nrf_gpio_pin_clear(LCD_SPI_CS_PIN)
#define SPI_DC_H                                 nrf_gpio_pin_set(LCD_SPI_DC_PIN)
#define SPI_DC_L                                  nrf_gpio_pin_clear(LCD_SPI_DC_PIN)
#define SPI_RESET_H                            nrf_gpio_pin_set(LCD_REST_PIN)
#define SPI_RESET_L                            nrf_gpio_pin_clear(LCD_REST_PIN)




uint8 Y_OFFSET=4;
uint8 OLED_VERSION_CS=3;



void Delay(unsigned char n);
extern void BleCommunicationPoll(void);






/**********************************************
//
//写命令函数

**********************************************/
void Write_Command(uint8 command)
{
    SPI_CS_L;
    SPI_DC_L;
    
    SPI_CLK_L;
    if((command>>7)& 0x01)
        SPI_DATA_H;
    else
        SPI_DATA_L;
    SPI_CLK_H;

    SPI_CLK_L;
    if((command>>6)& 0x01)
        SPI_DATA_H;
    else
        SPI_DATA_L;
    SPI_CLK_H;


    SPI_CLK_L;
    if((command>>5)& 0x01)
        SPI_DATA_H;
    else
        SPI_DATA_L;
    SPI_CLK_H;

    
    SPI_CLK_L;
    if((command>>4)& 0x01)
        SPI_DATA_H;
    else
        SPI_DATA_L;
    SPI_CLK_H;


    SPI_CLK_L;
    if((command>>3)& 0x01)
        SPI_DATA_H;
    else
        SPI_DATA_L;
    SPI_CLK_H;


    SPI_CLK_L;
    if((command>>2)& 0x01)
        SPI_DATA_H;
    else
        SPI_DATA_L;
    SPI_CLK_H;


    SPI_CLK_L;
    if((command>>1)& 0x01)
        SPI_DATA_H;
    else
        SPI_DATA_L;
    SPI_CLK_H;


    SPI_CLK_L;
    if((command)& 0x01)
        SPI_DATA_H;
    else
        SPI_DATA_L;
    SPI_CLK_H;

    SPI_CS_H;
    
}


void Write_Data(uint8 date)
{
    SPI_CS_L;
    SPI_DC_H;
    
    SPI_CLK_L;
    if((date>>7)& 0x01)
        SPI_DATA_H;
    else
        SPI_DATA_L;
    SPI_CLK_H;

    SPI_CLK_L;
    if((date>>6)& 0x01)
        SPI_DATA_H;
    else
        SPI_DATA_L;
    SPI_CLK_H;


    SPI_CLK_L;
    if((date>>5)& 0x01)
        SPI_DATA_H;
    else
        SPI_DATA_L;
    SPI_CLK_H;

    
    SPI_CLK_L;
    if((date>>4)& 0x01)
        SPI_DATA_H;
    else
        SPI_DATA_L;
    SPI_CLK_H;


    SPI_CLK_L;
    if((date>>3)& 0x01)
        SPI_DATA_H;
    else
        SPI_DATA_L;
    SPI_CLK_H;


    SPI_CLK_L;
    if((date>>2)& 0x01)
        SPI_DATA_H;
    else
        SPI_DATA_L;
    SPI_CLK_H;


    SPI_CLK_L;
    if((date>>1)& 0x01)
        SPI_DATA_H;
    else
        SPI_DATA_L;
    SPI_CLK_H;


    SPI_CLK_L;
    if((date)& 0x01)
        SPI_DATA_H;
    else
        SPI_DATA_L;
    SPI_CLK_H;

    SPI_CS_H;
    
}







/******************************************************************************************
*函数名:		uDelay
*参数:			无
*返回值:		无
*描述:			延时
*编写:			
*版本信息:		
******************************************************************************************/
void uDelay(unsigned char l)
{
	while(l--);
}
/******************************************************************************************
*函数名:		Delay
*参数:			无
*返回值:		无
*描述:			延时
*编写:			
*版本信息:		
******************************************************************************************/
void Delay(unsigned char n)
{
  unsigned char i,j,k;

  for(k=0;k<n;k++)
  {
    for(i=0;i<131;i++)
    {
      for(j=0;j<15;j++)
      {
        uDelay(203);	
      }
    }
  }
}

/******************************************************************************************
*函数名:		Set_Start_Column
*参数:			无
*返回值:		无
*描述:			
*编写:			
*版本信息:		
******************************************************************************************/
void Set_Start_Column(unsigned char d)
{
  Write_Command(0x00+d%16);		// Set Lower Column Start Address for Page Addressing Mode
                                        //   Default => 0x00
  Write_Command(0x10+d/16);		// Set Higher Column Start Address for Page Addressing Mode
                                        //   Default => 0x10
}

/******************************************************************************************
*函数名:		Set_Contrast_Control
*参数:			无
*返回值:		无
*描述:			
*编写:			
*版本信息:		
******************************************************************************************/
void Set_Contrast_Control(unsigned char d)
{
  Write_Command(0x81);			// Set Contrast Control
  Write_Command(d);			//   Default => 0x7F
}

/******************************************************************************************
*函数名:		Set_Entire_Display
*参数:			无
*返回值:		无
*描述:			
*编写:			
*版本信息:		
******************************************************************************************/
void Set_Entire_Display(unsigned char d)
{
  Write_Command(0xA4|d);			
                                          //   Default => 0xA4
                                          //     0xA4 (0x00) => Normal Display
                                          //     0xA5 (0x01) => Entire Display On
}
/******************************************************************************************
*函数名:		Set_Inverse_Display
*参数:			无
*返回值:		无
*描述:			
*编写:			
*版本信息:		
******************************************************************************************/
void Set_Inverse_Display(unsigned char d)
{
  Write_Command(0xA6|d);			// Set Inverse Display On/Off
                                          //   Default => 0xA6
                                          //     0xA6 (0x00) => Normal Display
                                          //     0xA7 (0x01) => Inverse Display On
}

/******************************************************************************************
*函数名:		Set_Display_On_Off
*参数:			无
*返回值:		无
*描述:			
*编写:			
*版本信息:		
******************************************************************************************/
void Set_Display_On_Off(unsigned char d)	
{
  Write_Command(0xAE|d);			// Set Display On/Off
                                          //   Default => 0xAE
                                          //     0xAE (0x00) => Display Off
                                          //     0xAF (0x01) => Display On
}
/******************************************************************************************
*函数名:		Set_Start_Page
*参数:			无
*返回值:		无
*描述:			
*编写:			
*版本信息:		
******************************************************************************************/
void Set_Start_Page(unsigned char d)
{
    Write_Command(0xB0|d);			// Set Page Start Address for Page Addressing Mode
                                                                //   Default => 0xB0 (0x00)
}

/******************************************************************************************
*函数名:		Fill_RAM
*参数:			无
*返回值:		无
*描述:			
*编写:			
*版本信息:		
******************************************************************************************/
void Fill_RAM(unsigned char Data)
{

if(OLED_VERSION_CS==3) //96*64屏
{
    unsigned char i,j;
    
    for(i=0;i<8;i++)
    {
        Set_Start_Page(i);
        Set_Start_Column(0x00);


        for(j=0;j<128;j++)
        {
            Write_Data(Data);
        }     
    }
}  
else if(OLED_VERSION_CS==4) //96*96屏
{
    uint16 i;
    
    Write_Command(0x15);    /*Set column address*/       
    Write_Command(0x08);    /*Column Start Address*/
    Write_Command(0x37);    /*Column End Address*/
    Write_Command(0x75);    /*Set row address*/  
    Write_Command(0x00);    /*Row Start Address*/  
    Write_Command(0x5f);    /*Row End Address*/  

    for(i=0;i<(96*96/2);i++)
    {
            Write_Data(0x00);   
    }
}

}

/******************************************************************************************
*函数名:		Ssd13xxSleep
*参数:			0：进入休眠  1：退出休眠
*返回值:		无
*描述:			休眠模式
*编写:			
*版本信息:		
******************************************************************************************/
void Ssd13xxSleep(unsigned char a)
{
  uint8 OledX =64;
  switch(a)
  {
    case SSD13XX_ENTER_SLEEP:
        if(DISPLAY_GOTO_SLEEP_FLAG == false)
        {          
		if(OLED_VERSION_CS==4)
		{
			  OledX = 72;
			  Y_OFFSET=0;
		}
	       //ClearLcdDisplay(0,0,96,OledX);
              Y_OFFSET=4;	
              Set_Display_On_Off(0x00);
              Set_Entire_Display(0x01);
             // LCD_OLED_BK_EN_L;//进入休眠模式
              DISPLAY_GOTO_SLEEP_FLAG= true;
              SetSystemSuspendTime(DISPLAY_STATE_NO_CHANGE);
        }
    break;
    
    case SSD13XX_EXIT_SLEEP:
        if(DISPLAY_GOTO_SLEEP_FLAG == true)
        {
                OledIoIint();
		  if(OLED_VERSION_CS==4)
		  {
			  OledX = 72;
			  Y_OFFSET=0;
		  }
	         //ClearLcdDisplay(0,0,96,OledX);
                Y_OFFSET=4;
                Set_Entire_Display(0x00);
                Set_Display_On_Off(0x01);
                StartKeyScan();
                DISPLAY_GOTO_SLEEP_FLAG=false;
                SetSystemSuspendTime(DISPLAY_TIME_10S_FLAG);
        }
        break;
        
  }
  
}

/******************************************************************************************
*函数名:		OLED_IO_Iint
*参数:			无
*返回值:		无
*描述:			初始IO口
*编写:			
*版本信息:		
******************************************************************************************/
void OLED_IO_Iint(void)
{
  LCD_SPI_CLK_OUT;          
  LCD_SPI_DATA_OUT;        
  LCD_SPI_DC_OUT;       
  LCD_SPI_CS_OUT;     
  LCD_SPI_REST_OUT;   
  
  SPI_CLK_H;           
  SPI_DATA_H;           
  SPI_CS_H;             
  SPI_DC_H;   
}

/******************************************************************************************
*函数名:		OledIoIint
*参数:			无
*返回值:		无
*描述:			初始IO口
*编写:			
*版本信息:	
******************************************************************************************/
void OledIoIint(void)
{
  LCD_SPI_CLK_OUT;		  
  LCD_SPI_DATA_OUT;		 
  LCD_SPI_DC_OUT; 	  
  LCD_SPI_CS_OUT;  
}

/******************************************************************************************
*函数名:		OLED_Init_I
*参数:			无
*返回值:		无
*描述:			内部升压初始化
*编写:			
*版本信息:		
******************************************************************************************/
void OLED_Init_I()				// VCC Generated by Internal DC/DC Circuit
{  
        unsigned char i;

        SPI_RESET_L;
        for(i=0;i<200;i++)
        {
        	uDelay(200);
        }
        SPI_RESET_H;
        for(i=0;i<200;i++)
        {
        	uDelay(200);
        }
        
        if(OLED_VERSION_CS==3) //96*64屏
        {
           Write_Command(0xAE);     //Set Display Off  

           Write_Command(0xd5);     //display divide ratio/osc. freq. mode   
           Write_Command(0x80);     //  

           Write_Command(0xA8);     //multiplex ration mode:63   
           Write_Command(0x3F);  

           Write_Command(0xD3);     //Set Display Offset     
           Write_Command(0x00);  

           Write_Command(0x40);     //Set Display Start Line   

           Write_Command(0xAD);     //DC-DC Control Mode Set   
           Write_Command(0x8B);     // 8A:disable.8B:Built-in DC-DC is used, 
           Write_Command(0x33);     //Set Pump voltage value    30:6.4V;01:7.4V;10:8V;11:9V  

           Write_Command(0xA1);     //Segment Remap    

           Write_Command(0xC8);     //Sst COM Output Scan Direction   

           Write_Command(0xDA);     //common pads hardware: alternative   
           Write_Command(0x12);  

           Write_Command(0x81);     //contrast control   
           Write_Command(0xff); 

           Write_Command(0xD9);     //set pre-charge period     
           Write_Command(0x22);  


           Write_Command(0xDB);     //VCOM deselect level mode   
           Write_Command(0x14);   

           Write_Command(0xA4);     //Set Entire Display On/Off   

           Write_Command(0xA6);     //Set Normal Display   

           Fill_RAM(0x00); 			// Clear Screen
           
           Write_Command(0xAF);     //Set Display On  
        }

}
/******************************************************************************************
*函数名:		Ssd13xxInit
*参数:			无
*返回值:		无
*描述:			SSD13xx初始化
*编写:			
*版本信息:		
******************************************************************************************/
void Ssd13xxInit(void)
{
  OLED_IO_Iint();
  OLED_Init_I();  
}

/******************************************************************************************
*函数名:		DisplayMap
*参数:			无
*返回值:		无
*描述:			显示图片
*编写:			
*版本信息:		
******************************************************************************************/
#if 0
void DisplayMap(const uint8 *Map,uint8 x,uint8 y,uint8 w,uint8 h)
{
    if(OLED_VERSION_CS==3)  //96*64屏
    {
      x=x+18;
      uint8 EndPage=(h)>>3;
      uint8 StartPage=(y)>>3;


      for(uint8 i=0;i<EndPage;i++)
      {
        Set_Start_Page(StartPage+i); 
        Set_Start_Column(x);

        for(uint8 j=0;j<w;j++)
        Write_Data(Map[i*w+j]);   
      }
    }

    else if(OLED_VERSION_CS==4) //96*96屏
    {
        uint8 j;
        uint8 ConversionDataBuff[1];

        y = y+Y_OFFSET;
        
       
        Write_Command(0x15);    /*Set column address*/       
        Write_Command(0x08+x/2);    /*Column Start Address*/
        if((w%2)==0)
                j=w;
        else
                j=w+1;
        Write_Command(0x08+x/2-1+j/2);    /*Column End Address*/
        Write_Command(0x75);    /*Set row address*/  
        Write_Command(y);    /*Row Start Address*/  
        Write_Command(y+h-1);    /*Row End Address*/  


        
        for(uint8 m=0;m<(h>>3);m++)
        {
                for(uint8 n=0;n<8;n++)
                {
                        for(uint8 k=0;k<w;k++)
                        {
                                if(Map[m*w+k] & (1<<n))
                                        ConversionDataBuff[0] |=0x0f;
                                else
                                        ConversionDataBuff[0] &=0xf0;
                                k++;
                                if((Map[m*w+k] & (1<<n))&&(k<w))
                                        ConversionDataBuff[0] |=0xf0;
                                else
                                        ConversionDataBuff[0] &=0x0f;
                                Write_Data(ConversionDataBuff[0]);
                        }
                }

        }

        
    }

}
#endif
void DrawLine(uint8 x,uint8 y,uint8 w,uint8 h)
{
if(OLED_VERSION_CS==3)  //96*64屏
{
    x=x+18;
    uint8 EndPage=((h-1)+(y%8))>>3;
    uint8 StartPage=(y)>>3;


    for(uint8 i=0;i<=EndPage;i++)
    {
        Set_Start_Page(StartPage+i); 
        Set_Start_Column(x);

        for(uint8 j=0;j<w;j++)
        {
            Write_Data((0xff>>(8-h))<<(y%8));
        }
    }
}

else if(OLED_VERSION_CS==4) //96*96屏
{
    uint16 i,j;

    y = y+Y_OFFSET;
    
    Write_Command(0x15);    /*Set column address*/       
    Write_Command(0x08+x/2);    /*Column Start Address*/
    if((w%2)==0)
            j=w;
    else
            j=w+1;
    Write_Command(0x08+x/2-1+j/2);    /*Column End Address*/
    Write_Command(0x75);    /*Set row address*/  
    Write_Command(y);    /*Row Start Address*/  
    Write_Command(y+h-1);    /*Row End Address*/  

    for(i=0;i<(w*h/2);i++)
    {
            Write_Data(0xFF);   
    }

}

}

/******************************************************************************************
*函数名:		SetLcdDisplay
*参数:			无
*返回值:		无
*描述:			清除LCD函数
*编写:			
*版本信息:		
******************************************************************************************/
void SetLcdDisplay(uint8 x,uint8 y,uint8 w,uint8 h)
{
    if(OLED_VERSION_CS==3)  //96*64屏
    {
        x=x+18;
        uint8 EndPage=(h)>>3;
        uint8 StartPage=(y)>>3;
             
      
      for(uint8 i=0;i<EndPage;i++)
      {
          Set_Start_Page(StartPage+i); 
          Set_Start_Column(x);
          
          for(uint8 j=0;j<w;j++)
            Write_Data(0xFF);   
      }
    }

    else if(OLED_VERSION_CS==4) //96*96屏
    {
        uint16 i,j;

        y = y+Y_OFFSET;
        
        Write_Command(0x15);    /*Set column address*/       
        Write_Command(0x08+x/2);    /*Column Start Address*/
        if((w%2)==0)
                j=w;
        else
                j=w+1;
        Write_Command(0x08+x/2-1+j/2);    /*Column End Address*/
        Write_Command(0x75);    /*Set row address*/  
        Write_Command(y);    /*Row Start Address*/  
        Write_Command(y+h-1);    /*Row End Address*/  

        for(i=0;i<(w*h/2);i++)
        {
                Write_Data(0xFF);   
        }
    }
  
}

/******************************************************************************************
*函数名:		ClearLcdDisplay
*参数:			无
*返回值:		无
*描述:			清除LCD函数
*编写:			
*版本信息:		
******************************************************************************************/
#if 0
void ClearLcdDisplay(uint8 x,uint8 y,uint8 w,uint8 h)
{
    if(OLED_VERSION_CS==3) //96*64屏
    {
        x=x+18;
        uint8 EndPage=(h)>>3;
        uint8 StartPage=(y)>>3;
             
      
      for(uint8 i=0;i<EndPage;i++)
      {
          Set_Start_Page(StartPage+i); 
          Set_Start_Column(x);
          
          for(uint8 j=0;j<w;j++)
            Write_Data(0x00);   
      }
    }

    else if(OLED_VERSION_CS==4) //96*96屏
    {
        uint8 k;

        y = y+Y_OFFSET;
        
        Write_Command(0x15);    /*Set column address*/       
        Write_Command(0x08+x/2);    /*Column Start Address*/
        if((w%2)==0)
                k=w;
        else
                k=w+1;
        Write_Command(0x08+x/2-1+k/2);    /*Column End Address*/
        Write_Command(0x75);    /*Set row address*/  
        Write_Command(y);    /*Row Start Address*/  
        Write_Command(y+h-1);    /*Row End Address*/  

        for(uint8 i=0;i<h;i++)
        {
                for(uint8 j=0;j<k/2;j++)
                {
                        Write_Data(0x00);   
                }

        }
                    
    }
      
}
#endif





