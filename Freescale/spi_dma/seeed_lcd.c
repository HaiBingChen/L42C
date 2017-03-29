/* Copyright (c) 2014 Nordic Semiconductor. All Rights Reserved.
 *
 * The information contained herein is property of Nordic Semiconductor ASA.
 * Terms and conditions of usage are described in detail in NORDIC
 * SEMICONDUCTOR STANDARD SOFTWARE LICENSE AGREEMENT.
 *
 * Licensees are granted free, non-transferable use of the information. NO
 * WARRANTY of ANY KIND is provided. This heading must NOT be removed from
 * the file.
 *
 */

/** @file
 *
 * @defgroup blinky_example_main main.c
 * @{
 * @ingroup blinky_example
 * @brief Blinky Example Application main file.
 *
 */

#include "fsl_spi_dma.h"
#include "fsl_dmamux.h"
#include "board.h"
#include "fsl_debug_console.h"
#include "pin_mux.h"
#include "fsl_common.h"
#include "fsl_port.h"
#include "pin_mux.h"
#include "fsl_gpio.h"
#include "head_dma.h"
#include "seeed_lcd.h"
#include "ExtSpiFlash.h"
#include "FGPIO.h"
#include "display_font.h"
#include "ResMap.h"

#define _RST_H()		(FGPIOB_PSOR = 1 << 17)
#define _RST_L()		(FGPIOB_PCOR = 1 << 17)

#define _CS_H()		(FGPIOB_PSOR = 1 << 1)
#define _CS_L()		(FGPIOB_PCOR = 1 << 1)

#define _DC_H()		(FGPIOB_PSOR = 1 << 2)
#define _DC_L()		(FGPIOB_PCOR = 1 << 2)

#define _WR_H()		(FGPIOB_PSOR = 1 << 3)
#define _WR_L()		(FGPIOB_PCOR = 1 << 3)

#define _RD_H()		(FGPIOB_PSOR = 1 << 16)
#define _RD_L()		(FGPIOB_PCOR = 1 << 16)
#define _BL_H()		(FGPIOC_PSOR = 1 << 10)
#define _BL_L()		(FGPIOC_PCOR = 1 << 10)

#define DELAY	0xF30000
#define DELAY1	0xa00000
#define DELAY2	0xf0000

#define Color_N	0xdc
#define Color_L	0xF8

uint8_t tft_data[480]={0x00};
uint8_t tft_All_data[1024]={0};

#define Maxcolumns  10
uint8_t PP_Buffer0[480*Maxcolumns];
uint8_t PP_Buffer1[480*Maxcolumns];
uint8_t PP_Buffer2[480*Maxcolumns];
#define GetCDBuf(CDBuf) ((CDBuf)==PP_Buffer1)? (PP_Buffer2):(PP_Buffer1)
/*addr:4bytes ; x y w h :1bytes*/
#define DISPLAY_ADDR_MAX	512
uint32_t Display_Addr[DISPLAY_ADDR_MAX];
uint8_t   Display_XYWH[DISPLAY_ADDR_MAX*4];
uint16_t  TotalAddr =0;
#define AddrOffSet  4;	
uint8_t* gDpos = Display_XYWH+(DISPLAY_ADDR_MAX-1)*AddrOffSet;
uint8_t Now_X=0;
uint8_t *DmaFillBuf=NULL;
uint8_t *DmaDisplayBuf=NULL;
bool CleanDisplayMode=false;
bool NeedFillData=false;
bool NeedBackground=false;
uint8_t RowsPos =0;
uint8_t ColumnsPos =0;
int Dis_Offset= 0;
void LCD_Lcear(void);

#define MASTER_TRANSFER_TIMEOUT1		(5000U)             /*! Transfer timeout of master - 5s */
#define _WR_DATA8bit(val)				(FGPIOE_PDOR = (val) << 16)
#define LCD_WRITE_GRAM_CONTINOUS(val)		do {                    \
												_WR_L();            \
												FGPIOE_PDOR = (val)<<16;            \
												_WR_H();   \
												_WR_L();   \
												FGPIOE_PDOR = (val<<8) ;      \
												_WR_H();            \
												} while (0)  

void wait_ms(volatile uint32_t number_of_ms)
{
	while(number_of_ms != 0)
	{
		number_of_ms--;
		while(number_of_ms != 0)
		{
			number_of_ms--;
			while(number_of_ms != 0)
			{
				number_of_ms--;
			}
		}
	}
}

void LCD_InitPins2(void)
{
	uint32_t i = 0;
	gpio_pin_config_t led_config = 
	{
		kGPIO_DigitalOutput, 0,
	};		 
	for(i = 16; i < 24; i++)
	{
		PORT_SetPinMux(PORTE, i, kPortMuxAsGpio);
		GPIO_PinInit(GPIOE, i, &led_config);
	}

	PORT_SetPinMux(PORTB,17, kPortMuxAsGpio);
	GPIO_PinInit(GPIOB, 17, &led_config);

	PORT_SetPinMux(PORTB,1, kPortMuxAsGpio);
	GPIO_PinInit(GPIOB, 1, &led_config);

	PORT_SetPinMux(PORTB,2, kPortMuxAsGpio);
	GPIO_PinInit(GPIOB, 2, &led_config);

	PORT_SetPinMux(PORTB,3, kPortMuxAsGpio);
	GPIO_PinInit(GPIOB, 3, &led_config);

	PORT_SetPinMux(PORTB,16, kPortMuxAsGpio);
	GPIO_PinInit(GPIOB, 16, &led_config);

	PORT_SetPinMux(PORTC,10, kPortMuxAsGpio);
	GPIO_PinInit(GPIOC, 10, &led_config);	
	_BL_H();
}


void WriteComm(uint8_t command)
{ 
	_RD_H(); 
	_CS_L(); 
	_DC_L(); 
	_WR_L(); 
	_WR_DATA8bit(command);
	_WR_H();
	_CS_H();
}

void WriteData(uint8_t data)
{  
	_RD_H();
	_CS_L(); 
	_DC_H();
	_WR_L();
	_WR_DATA8bit(data);
	_WR_H();
	_CS_H();
}

static void _fill_lcd_from_dma_buffer(uint8_t *p_dma_buf, uint32_t frame_len, uint32_t offset)
{
	uint16_t *p = (uint16_t *)p_dma_buf;
	uint32_t dot_num = frame_len / 2;
	offset /= 2;
	_RD_H(); 
	_CS_L(); 
	_DC_H();
	_WR_L();
	__nop();__nop();
	__nop();__nop();
	__nop();__nop();

	for (uint32_t i = 0; i < dot_num; i++)
	{
		LCD_WRITE_GRAM_CONTINOUS(p[offset + i]); 
		if(NeedFillData)
		{
			Dma_GetFlashData();
		}
	}
	__nop();__nop();
	_WR_H();
	__nop();__nop();
	_CS_H();
}								

static void _fill_lcd_from_dma_buffer_Background(uint32_t frame_len, uint32_t offset)
{
	uint32_t dot_num = frame_len / 2;
	offset /= 2;
	_RD_H(); 
	_CS_L(); 
	_DC_H();
	_WR_L();
	__nop();__nop();
	__nop();__nop();
	__nop();__nop();

	for (uint32_t i = 0; i < dot_num; i++)
	{
		LCD_WRITE_GRAM_CONTINOUS(0x0421); 
		if(NeedFillData)
		{
			Dma_GetFlashData();
		}
	}
	__nop();__nop();
	_WR_H();
	__nop();__nop();
	_CS_H();
}

void _clear_lcd(void)
{
	_RD_H(); 
	_CS_L(); 
	_DC_H();
	_WR_L();
	__nop();__nop();
	__nop();__nop();
	__nop();__nop();

	for (uint32_t i = 0; i < 240*240; i++)
	{
		LCD_WRITE_GRAM_CONTINOUS(0x00);   
	}
	__nop();__nop();
	_WR_H();
	__nop();__nop();
	_CS_H();
}

void _set_lcd(uint16_t colour)
{
	_RD_H(); 
	_CS_L(); 
	_DC_H();
	_WR_L();
	__nop();__nop();
	__nop();__nop();
	__nop();__nop();

	for (uint32_t i = 0; i < 240*240; i++)
	{
		LCD_WRITE_GRAM_CONTINOUS(colour);   
	}
	__nop();__nop();
	_WR_H();
	__nop();__nop();
	_CS_H();
}

void _Draw_One_line(uint16_t colour)
{
	_RD_H(); 
	_CS_L(); 
	_DC_H();
	_WR_L();
	__nop();__nop();
	__nop();__nop();
	__nop();__nop();

	for (uint32_t i = 0; i < 240; i++)
	{
		LCD_WRITE_GRAM_CONTINOUS(colour);   
	}
	__nop();__nop();
	_WR_H();
	__nop();__nop();
	_CS_H();
}

static void Reset_IC()
{
	_RST_H();
	wait_ms(200);
	_RST_L();
	wait_ms(200);
	_RST_H();
	wait_ms(500);
}


void Init_IC(void)
{
	Reset_IC();
	WriteComm(0x11);
	wait_ms(600);
	WriteComm(0x36);
	WriteData(0x40); ////////////

	WriteComm(0x3a);
	WriteData(0x05);
	WriteComm(0x21);
	WriteComm(0xE7);
	WriteData(0x00);	 // 2 data;00-1data
	WriteComm(0x2a);
	WriteData(0x00);
	WriteData(0x00);
	WriteData(0x00);
	WriteData(0xef);
	WriteComm(0x2b);
	WriteData(0x00);
	WriteData(0x00);
	WriteData(0x00);
	WriteData(0xef);
	//--------------------------------ST7789V Frame rate setting----------------------------------//
	WriteComm(0xb2);
	WriteData(0x0c);
	WriteData(0x0c);
	WriteData(0x00);
	WriteData(0x33);
	WriteData(0x33);
	WriteComm(0xb7);
	WriteData(0x35);
	//---------------------------------ST7789V Power setting--------------------------------------//
	WriteComm(0xbb);
	WriteData(0x1f);
	WriteComm(0xc0);
	WriteData(0x2c);
	WriteComm(0xc2);
	WriteData(0x01);
	WriteComm(0xc3);
	WriteData(0x12);
	WriteComm(0xc4);
	WriteData(0x20);
	WriteComm(0xc6);
	WriteData(0x0f);
	WriteComm(0xd0);
	WriteData(0xa4);
	WriteData(0xa1);
	//--------------------------------ST7789V
	WriteComm(0xe0);
	WriteData(0xd0);
	WriteData(0x08);
	WriteData(0x11);
	WriteData(0x08);
	WriteData(0x0c);
	WriteData(0x15);
	WriteData(0x39);
	WriteData(0x33);
	WriteData(0x50);
	WriteData(0x36);
	WriteData(0x13);
	WriteData(0x14);
	WriteData(0x29);
	WriteData(0x2d);
	WriteComm(0xe1);
	WriteData(0xd0);
	WriteData(0x08);
	WriteData(0x10);
	WriteData(0x08);
	WriteData(0x06);
	WriteData(0x06);
	WriteData(0x39);
	WriteData(0x44);
	WriteData(0x51);
	WriteData(0x0b);
	WriteData(0x16);
	WriteData(0x14);
	WriteData(0x2f);
	WriteData(0x31);
	wait_ms(120);
	WriteComm(0x29);
}


static void LCD_SetRamAddr(uint8_t x,uint8_t y,uint8_t w,uint8_t h)
{
	uint8_t width = 0; 
	uint8_t height = 0;
	width = w -1;
	height = h -1;
	if ((x+ width) > LCD_SIZE_W - 1)
	{
		return;
	}
	if ((y+ height) > LCD_SIZE_H - 1)
	{
		return;
	}
#if 0
	uint16_t xtemp = x + 0x50;
	uint16_t xtempend =x+width + 0x50;
	uint8_t xtemp_L =(uint8_t)(xtemp & 0X00FF);
	uint8_t xtemp_H =(uint8_t)((xtemp & 0XFF00)>>8);
	uint8_t xtempend_L =(uint8_t)(xtempend & 0X00FF);
	uint8_t xtempend_H =(uint8_t)((xtempend & 0XFF00)>>8);
	WriteComm(0x2A); 
	WriteData(xtemp_H);
	WriteData(xtemp_L);
	WriteData(xtempend_H);
	WriteData(xtempend_L);
#else
	WriteComm(0x2A); 
	WriteData(0x00);
	WriteData(x);
	WriteData(0x00);
	WriteData(x+width);
#endif
	_DC_H();
	WriteComm(0x2B); 
	WriteData(0x00);
	WriteData(y);
	WriteData(0x00);
	WriteData(y+height);
	_DC_H();
} 


/**********************************************
**********************************************/
void RAM_ADDRESS(void)
{ 
	WriteComm(0x2A);
	WriteData(0x00);
	WriteData(0x00);
	WriteData(0x00);
	WriteData(0xEF);

	WriteComm(0x2B);
	WriteData(0x00);
	WriteData(0x00);
	WriteData(0x00);
	WriteData(0xEF);

	WriteComm(0x2C);//Memory Write
}

void LCD_xy_Adress(LCD_XY_POSITION PST)
{
	switch (PST)
	{
		case POSIOTN_0DGR:
		{
			/*MY=0; MX=1; MV=0 RGB=1*/
			WriteComm(0x36);
			WriteData(0x48);
			break;
		}
		case POSIOTN_90DGR:
		{
			/*MY=1; MX=1; MV=1 RGB=1*/
			WriteComm(0x36);
			WriteData(0xE8);
			break;
		}
		case POSIOTN_180DGR:
		{
			/*MY=1; MX=0; MV=0 RGB=1*/
			WriteComm(0x36);
			WriteData(0x88);
			break;
		}
		case POSIOTN_270DGR:
		{
			/*MY=0; MX=0; MV=1 RGB=1*/
			WriteComm(0x36);
			WriteData(0x28);
			break;
		}
		default:
			break;
	}
}

void Seeed_lcd_init (void)
{
       LCD_InitPins2();
	Init_IC();
	RAM_ADDRESS();
#if 0
	LCD_xy_Adress(POSIOTN_90DGR);
#else
	LCD_xy_Adress(POSIOTN_270DGR);
#endif
	LCD_SetRamAddr(0,0,240,240);
}

void ClearLcdDisplay(uint8_t x,uint8_t y,uint8_t w,uint8_t h)
{
	uint16_t i;
	if(w==0||h==0)
		return;
	LCD_SetRamAddr(y,x,h,w);
	WriteComm(0x2C);
	for (i = 0; i < w; i++)
	{
		if(NeedBackground)
			_fill_lcd_from_dma_buffer_Background(h*2,0);
		else
			_fill_lcd_from_dma_buffer(tft_data, h*2,0);
	}
	_DC_H();
}

void ClearFullScreen(void)
{
	StopRoll();
	LCD_SetRamAddr(0,0,240,240);
	WriteComm(0x2C);
	_clear_lcd();
	_DC_H();
}
void SetFullScreen(uint16_t colour)
{
	StopRoll();
	LCD_SetRamAddr(0,0,240,240);
	WriteComm(0x2C);
	_set_lcd(colour);
	_DC_H();
}

void SetScreen(uint8_t posx, uint8_t posy, uint8_t posw,uint8_t posh,uint16_t colour)
{
	StopRoll();
	LCD_SetRamAddr(posy,posx,posh,posw);
	WriteComm(0x2C);
	_set_lcd(colour);
	_DC_H();
}

void LCD_Sleep(uint8_t state)
{
	
	if (state == 0)		//state = 0, sleep in
	{
		WriteComm(0x10);
	}
	else if(state == 1)	//state = 1, sleep out
	{
		WriteComm(0x11);
	}
}



void DisplayMap(uint8_t *Map, uint8_t posx, uint8_t posy, uint32_t width, uint32_t height)
{
	uint16_t i;
	LCD_SetRamAddr(posy,posx,height,width);
	WriteComm(0x2C);
	for (i = 0; i < width; i++)  // 120
	{
		_fill_lcd_from_dma_buffer(&Map[(i*height*2)], 2*height,0);
	}
	_DC_H();
}

uint8_t GetCharParameter(uint8_t c, uint16_t CharType, uint16_t color, uint32_t * c_addr,uint32_t * c_w,uint32_t * c_h,uint8_t*c_X,uint8_t*c_Y)
{
	uint32_t baseAddr  =0;
/**************************************************************************/	
	if(CharType==CHAR_7X14)
	{
							
			if(c>=0x41 && c<=0x5a)		/*大写*/
			{
				baseAddr =	Char_14pxAZ_10x10;
				uint8_t cmap[26]={	10,8,9,9,8,8,9,
									9,3,8,9,7,11,9,
									9,8,9,9,9,9,
									9,10,13,9,10,9};		
					uint8_t len =  c-0x41;
					uint32_t addrTemp =baseAddr;
					for(uint8_t i =0; i<len; i++)
					{
						addrTemp += (2*10*(uint32_t)(cmap[i]));
					}
					*c_addr = addrTemp;
					*c_w = (uint32_t)cmap[len];
					*c_h = 10;
		            *c_Y +=1;				
			}
			
			else if (c>=0x61 && c<=0x7a)		/*小写*/
			{
				baseAddr =	Char_14pxazs_7x14;
				uint8_t cmap[26]={	7,7,8,7,8,6,7,
									7,3,4,7,3,12,7,
									8,7,7,5,7,5,
									7,8,11,8,8,7};		
				uint8_t len =  c-0x61;
				uint32_t addrTemp =baseAddr;
				for(uint8_t i =0; i<len; i++)
				{
					addrTemp += (2*14*(uint32_t)(cmap[i]));
				}
				*c_addr = addrTemp;
				*c_w = (uint32_t)cmap[len];
				*c_h = 14;
				*c_Y +=0;
			}
			else if(c>=0x30 && c<=0x39)			/*数字*/
			{
				*c_addr = remindnumber14px_9x10+(2*9*10)*(c-0x30);
				*c_w = 9;
				*c_h = 10;
				*c_Y +=2;
			}
			else if(c == 0x3A)					/*:*/
			{
				*c_addr = Char_14pxazs_7x14+0x000013CC;
				*c_w = 3;
				*c_h = 14;	
				*c_Y +=0;		
			}
			else if(c == 0x2C)					/*,*/
			{
				*c_addr = Char_14pxazs_7x14+0x00001420;
				*c_w = 4;
				*c_h = 14;	
				*c_Y +=0;		
			}
			else if(c == 0x2e)					/*.*/
			{
				*c_addr = Char_14pxazs_7x14+0x00001490;
				*c_w = 3;
				*c_h = 14;	
				*c_Y +=0;		
			}
			else if(c == 0x21)					/*!*/
			{
				*c_addr = Char_14pxazs_7x14+0x000014E4;
				*c_w = 3;
				*c_h = 14;	
				*c_Y +=0;		
			}
			else if(c == 0x2D)					/*-*/
			{
				*c_addr = Char_14pxazs_7x14+0x00001538;
				*c_w = 5;
				*c_h = 14;	
				*c_Y +=0;		
			}
			else if(c == 0x2B)					/*+*/
			{
				*c_addr = Char_14pxazs_7x14+0x000015C4;
				*c_w = 7;
				*c_h = 14;	
				*c_Y +=0;		
			}
			else if(c == 0x3f)					/*?*/
			{
				*c_addr = Char_14pxazs_7x14+0x00001688;
				*c_w = 6;
				*c_h = 16;	
				*c_Y +=0;		
			}
			else if(c == 0x27)					/*'*/
			{
				*c_addr = Char_14pxazs_7x14+0x00001730;
				*c_w = 3;
				*c_h = 14;	
				*c_Y +=0;		
			}
			else if(c == 0x2f)					/*斜杠*/		
			{
				*c_addr = Char_14pxazs_7x14+0x00001784;
				*c_w = 5;
				*c_h = 14;	
				*c_Y +=0;	
			}
			
			else if(c == 0x20)					/*空格*/		
			{
				*c_addr = Char_14pxazs_7x14+0x00001810;
				*c_w = 3;
				*c_h = 14;	
				*c_Y -=0;

			}	
			else 
			{
				*c_X+=4;
				return 4;
			}
	}
	else if(CharType==CHAR_11X20)
	{
							
			if(c>=0x41 && c<=0x5a)		/*大写*/
			{
				baseAddr =	Char_21pxAZ_15x16;
				uint8_t cmap[26]={	15,12,13,12,11,11,13,
									13,4,11,13,11,16,13,
									14,12,12,12,12,13,
									12,14,19,14,14,12};			
					uint8_t len =  c-0x41;
					uint32_t addrTemp =baseAddr;
					for(uint8_t i =0; i<len; i++)
					{
						addrTemp += (2*16*(uint32_t)(cmap[i]));
					}
					*c_addr = addrTemp;
					*c_w = (uint32_t)cmap[len];
					*c_h = 16;
		            *c_Y +=0;				
			}
			
			else if (c>=0x61 && c<=0x7a)		/*小写*/
			{
				baseAddr =	Char_21pxazs_11x20;
				uint8_t cmap[26]={	11,11,11,11,11,8,11,
									10,4,6,11,4,17,10,
									12,11,11,7,10,8,
									10,11,16,11,11,10};		
				uint8_t len =  c-0x61;
				uint32_t addrTemp =baseAddr;
				for(uint8_t i =0; i<len; i++)
				{
					addrTemp += (2*20*(uint32_t)(cmap[i]));
				}
				*c_addr = addrTemp;
				*c_w = (uint32_t)cmap[len];
				*c_h = 20;
				*c_Y +=0;
			}
			else if(c>=0x30 && c<=0x39)			/*数字*/
			{
				*c_addr = socialnumber21px_12x16+(2*12*16)*(c-0x30);
				*c_w = 12;
				*c_h = 16;
				*c_Y +=0;
			}
			else if(c == 0x3A)					/*:*/
			{
				*c_addr = Char_21pxazs_11x20+0x00002940;
				*c_w = 4;
				*c_h = 20;	
				*c_Y +=0;		
			}
			else if(c == 0x2C)					/*,*/
			{
				*c_addr = Char_21pxazs_11x20+0x000029E0;
				*c_w = 4;
				*c_h = 20;	
				*c_Y +=0;		
			}
			else if(c == 0x2e)					/*.*/
			{
				*c_addr = Char_21pxazs_11x20+0x00002A80;
				*c_w = 4;
				*c_h = 20;	
				*c_Y +=0;		
			}
			else if(c == 0x21)					/*!*/
			{
				*c_addr = Char_21pxazs_11x20+0x00002B20;
				*c_w = 4;
				*c_h = 20;	
				*c_Y +=0;		
			}
			else if(c == 0x2D)					/*-*/
			{
				*c_addr = Char_21pxazs_11x20+0x00002BC0;
				*c_w = 7;
				*c_h = 20;	
				*c_Y +=0;		
			}
			else if(c == 0x2B)					/*+*/
			{
				*c_addr = Char_21pxazs_11x20+0x00002CD8;
				*c_w = 12;
				*c_h = 20;	
				*c_Y +=0;		
			}
			else if(c == 0x3f)					/*?*/
			{
				*c_addr = Char_21pxazs_11x20+0x00002EB8;
				*c_w = 8;
				*c_h = 20;	
				*c_Y +=0;		
			}
			else if(c == 0x27)					/*'*/
			{
				*c_addr = Char_21pxazs_11x20+0x00002FF8;
				*c_w = 4;
				*c_h = 20;	
				*c_Y +=0;		
			}
			else if(c == 0x2f)					/*斜杠*/		
			{
				*c_addr = Char_21pxazs_11x20+0x00003098;
				*c_w = 9;
				*c_h = 20;	
				*c_Y +=0;	
			}
			
			else if(c == 0x20)					/*空格*/		
			{
				*c_addr = Char_21pxazs_11x20+0x00003200;
				*c_w = 3;
				*c_h = 20;	
				*c_Y -=0;

			}	
			else 
			{
				*c_X+=4;
				return 4;
			}
	}
	else if(CharType==CHAR_12X24)
	{
							
			if(c>=0x41 && c<=0x5a)		/*大写*/
			{
				baseAddr =	Char_24pxAZ_16x18;
				uint8_t cmap[26]={	16,13,14,14,12,12,14,
									14,4,12,14,11,18,14,
									15,13,14,14,14,15,
									14,16,22,15,15,14};		
					uint8_t len =  c-0x41;
					uint32_t addrTemp =baseAddr;
					for(uint8_t i =0; i<len; i++)
					{
						addrTemp += (2*18*(uint32_t)(cmap[i]));
					}
					*c_addr = addrTemp;
					*c_w = (uint32_t)cmap[len];
					*c_h = 18;
		            *c_Y +=1;				
			}
			
			else if (c>=0x61 && c<=0x7a)		/*小写*/
			{
				baseAddr =	Char_24pxazs_12x24;
				uint8_t cmap[26]={	12,12,12,12,12,9,12,
									11,4,6,11,4,19,11,
									13,12,12,7,11,8,
									11,12,18,12,12,11}; 		
				uint8_t len =  c-0x61;
				uint32_t addrTemp =baseAddr;
				for(uint8_t i =0; i<len; i++)
				{
					addrTemp += (2*24*(uint32_t)(cmap[i]));
				}
				*c_addr = addrTemp;
				*c_w = (uint32_t)cmap[len];
				*c_h = 24;
				*c_Y +=0;
			}
			else if(c>=0x30 && c<=0x39)			/*数字*/
			{
				*c_addr = number24px_14x18+(2*14*18)*(c-0x30);
				*c_w = 14;
				*c_h = 18;
				*c_Y +=1;
			}
			else if(c == 0x3A)					/*:*/
			{
				*c_addr = Char_24pxazs_12x24+0x000035A0;
				*c_w = 4;
				*c_h = 24;	
				*c_Y +=0;		
			}
			else if(c == 0x2C)					/*,*/
			{
				*c_addr = Char_24pxazs_12x24+0x00003660;
				*c_w = 4;
				*c_h = 24;	
				*c_Y +=0;		
			}
			else if(c == 0x2e)					/*.*/
			{
				*c_addr = Char_24pxazs_12x24+0x00003720;
				*c_w = 4;
				*c_h = 24;	
				*c_Y +=0;		
			}
			else if(c == 0x21)					/*!*/
			{
				*c_addr = Char_24pxazs_12x24+0x000037E0;
				*c_w = 4;
				*c_h = 24;	
				*c_Y +=0;		
			}
			else if(c == 0x2D)					/*-*/
			{
				*c_addr = Char_24pxazs_12x24+0x000038A0;
				*c_w = 7;
				*c_h = 24;	
				*c_Y +=0;		
			}
			else if(c == 0x2B)					/*+*/
			{
				*c_addr = Char_24pxazs_12x24+0x000039F0;
				*c_w = 12;
				*c_h = 24;	
				*c_Y +=0;		
			}
			else if(c == 0x3f)					/*?*/
			{
				*c_addr = Char_24pxazs_12x24+0x00003C30;
				*c_w = 9;
				*c_h = 24;	
				*c_Y +=0;		
			}
			else if(c == 0x27)					/*'*/
			{
				*c_addr = Char_24pxazs_12x24+0x00003DE0;
				*c_w = 4;
				*c_h = 24;	
				*c_Y +=0;		
			}
			else if(c == 0x2f)					/*斜杠*/		
			{
				*c_addr = Char_24pxazs_12x24+0x00003EA0;
				*c_w = 9;
				*c_h = 24;	
				*c_Y +=0;	
			}
			else if(c == 0x23)					/*#*/		
			{
				*c_addr = Char_pound_11x19;
				*c_w = 11;
				*c_h = 19;	
				*c_Y +=0;	
			}
			else if(c == 0x20)					/*空格*/		
			{
#if 0
				*c_addr = Char_24pxazs_12x24+0x00004050;
				*c_w = 4;
				*c_h = 24;	
				*c_Y -=0;
#endif
				*c_X+=6;
				return 6;

			}	
			else 
			{
				*c_X+=4;
				return 4;
			}
	}
	return 0;	
}

static uint8_t GetFontParameter(uint8_t c,uint32_t FontType,uint32_t * c_addr,uint32_t * c_w,uint32_t * c_h)
{
	uint32_t baseAddr  =FontType;

	if(c>=0x30 && c<=0x39)			/*数字*/
	{		
		switch (FontType)
		{	
/***********************************************************/
			case number12px_gray_7x9:
				{
				*c_addr = baseAddr+0x07EUL*(c-0x30);
				*c_w = 7;
				*c_h = 9;
				break;
				}
			case number36px_18x26:
				{
				*c_addr = baseAddr+0x3A8UL*(c-0x30);
				*c_w = 18;
				*c_h = 26;
				break;
				}
			case remindnumber14px_9x10:
				{
				*c_addr = baseAddr+0x0B4UL*(c-0x30);
				*c_w = 9;
				*c_h = 10;
				break;
				}
			case socialnumber21px_12x16:
				{
				*c_addr = baseAddr+0x180UL*(c-0x30);
				*c_w = 12;
				*c_h = 16;
				break;
				}
			case timenumber14px_8x11:
				{
				*c_addr = baseAddr+0x0B0UL*(c-0x30);
				*c_w = 8;
				*c_h = 11;
				break;
				}
			case timernumber32px_18x24:
				{
				*c_addr = baseAddr+0x360UL*(c-0x30);
				*c_w = 18;
				*c_h = 24;
				break;
				}
			case timeblack_date_9x15:
				{
				*c_addr = baseAddr+0x10EUL*(c-0x30);
				*c_w = 9;
				*c_h = 15;
				break;
				}				
			case timeblack_timeblue_35x55:
				{
				*c_addr = baseAddr+0xF0AUL*(c-0x30);
				*c_w = 35;
				*c_h = 55;
				break;
				}
			case timeblack_timewhite_35x55:
				{
				*c_addr = baseAddr+0xF0AUL*(c-0x30);
				*c_w = 35;
				*c_h = 55;
				break;
				}
			case timestar_date_8x10:
				{
				*c_addr = baseAddr+0xA0UL*(c-0x30);
				*c_w = 8;
				*c_h = 10;
				break;
				}
			case timestar_time_32x46:
				{
				*c_addr = baseAddr+0xB80UL*(c-0x30);
				*c_w = 32;
				*c_h = 46;
				break;
				}
			case timesunup_date_9x15:
				{
				*c_addr = baseAddr+0x10EUL*(c-0x30);
				*c_w = 9;
				*c_h = 15;
				break;
				}
			case timesunup_number_34x50:
				{
				*c_addr = baseAddr+0xD48UL*(c-0x30);
				*c_w = 34;
				*c_h = 50;
				break;
				}
			case timemain_number_11x16:
				{
				*c_addr = baseAddr+0x160UL*(c-0x30);
				*c_w = 11;
				*c_h = 16;
				break;
				}
			case timeblue_time_30x45:
				{
				*c_addr = baseAddr+0xA8CUL*(c-0x30);
				*c_w = 30;
				*c_h = 45;
				break;
				}
			case timeblue_date_9x15:
				{
				*c_addr = baseAddr+0x10EUL*(c-0x30);
				*c_w = 9;
				*c_h = 15;
				break;
				}
			case timeblue_number_11x18:
				{
				*c_addr = baseAddr+0x18CUL*(c-0x30);
				*c_w = 11;
				*c_h = 18;
				break;
				}
			case timesierra_date_9x15:
				{
				*c_addr = baseAddr+0x10EUL*(c-0x30);
				*c_w = 9;
				*c_h = 15;
				break;
				}
			case timesierra_time_32x50:
				{
				*c_addr = baseAddr+0xC80UL*(c-0x30);
				*c_w = 32;
				*c_h = 50;
				break;
				}
			default:
				break;
		}
	}
	else if(c>=0x41 && c<=0x5a) 	/*大写*/
	{
		switch (FontType)
		{
/*********************************************************************/
			case Char_14pxAZ_10x10:
			{
				baseAddr =	Char_14pxAZ_10x10;
				uint8_t cmap[26]={	10,8,9,9,8,8,9,
									9,3,8,9,7,11,9,
									9,8,9,9,9,9,
									9,10,13,9,10,9};		
				uint8_t len =  c-0x41;
				uint32_t addrTemp =baseAddr;
				for(uint8_t i =0; i<len; i++)
				{
					addrTemp += (2*10*(uint32_t)(cmap[i]));
				}
				*c_addr = addrTemp;
				*c_w = (uint32_t)cmap[len];
				*c_h = 10;
				break;
			}
			case Char_21pxAZ_15x16:
			{
				baseAddr =	Char_21pxAZ_15x16;
				uint8_t cmap[26]={	15,12,13,12,11,11,13,
									13,4,11,13,11,16,13,
									14,12,12,12,12,13,
									12,14,19,14,14,12};		
				uint8_t len =  c-0x41;
				uint32_t addrTemp =baseAddr;
				for(uint8_t i =0; i<len; i++)
				{
					addrTemp += (2*16*(uint32_t)(cmap[i]));
				}
				*c_addr = addrTemp;
				*c_w = (uint32_t)cmap[len];
				*c_h = 16;
				break;
			}
			case Char_24pxAZ_16x18:
			{
				baseAddr =	Char_24pxAZ_16x18;
				uint8_t cmap[26]={	16,13,14,14,12,12,14,
									14,4,12,14,11,18,14,
									15,13,14,14,14,15,
									14,16,22,15,15,14};		
				uint8_t len =  c-0x41;
				uint32_t addrTemp =baseAddr;
				for(uint8_t i =0; i<len; i++)
				{
					addrTemp += (2*24*(uint32_t)(cmap[i]));
				}
				*c_addr = addrTemp;
				*c_w = (uint32_t)cmap[len];
				*c_h = 24;
				break;
			}
			default:
				break;
		}
	}
	else if (c>=0x61 && c<=0x7a)	/*小写*/
	{
		switch (FontType)
		{
/*********************************************************************/
			case Char_14pxazs_7x14:
			{
				baseAddr =	Char_14pxazs_7x14;
				uint8_t cmap[26]={	7,7,8,7,8,6,7,
									7,3,4,7,3,12,7,
									8,7,7,5,7,5,
									7,8,11,8,8,7};		
				uint8_t len =  c-0x61;
				uint32_t addrTemp =baseAddr;
				for(uint8_t i =0; i<len; i++)
				{
					addrTemp += (2*14*(uint32_t)(cmap[i]));
				}
				*c_addr = addrTemp;
				*c_w = (uint32_t)cmap[len];
				*c_h = 14;
				break;
			}
			case Char_21pxazs_11x20:
			{
				baseAddr =	Char_21pxazs_11x20;
				uint8_t cmap[26]={	11,11,11,11,11,8,11,
									10,4,6,11,4,17,10,
									12,11,11,7,10,8,
									10,11,16,11,11,10}; 	
				uint8_t len =  c-0x61;
				uint32_t addrTemp =baseAddr;
				for(uint8_t i =0; i<len; i++)
				{
					addrTemp += (2*20*(uint32_t)(cmap[i]));
				}
				*c_addr = addrTemp;
				*c_w = (uint32_t)cmap[len];
				*c_h = 20;
				break;
			}
			case Char_24pxazs_12x24:
			{
				baseAddr =	Char_24pxazs_12x24;
				uint8_t cmap[26]={	12,12,12,12,12,9,12,
									11,4,6,11,4,19,11,
									13,12,12,7,11,8,
									11,12,18,12,12,11}; 	
				uint8_t len =  c-0x61;
				uint32_t addrTemp =baseAddr;
				for(uint8_t i =0; i<len; i++)
				{
					addrTemp += (2*24*(uint32_t)(cmap[i]));
				}
				*c_addr = addrTemp;
				*c_w = (uint32_t)cmap[len];
				*c_h = 24;
				break;
			}

			default:
				break;
		}
	}
	else if(c == 0x2E)				/*.*/
	{
		switch (FontType)
		{
/***********************************************************/
			case number36px_18x26:
			{
				*c_addr = baseAddr+0x00002698;
				*c_w = 10;
				*c_h = 26;
				break;
			}
			case timernumber32px_18x24:
			{
				*c_addr = Char_24pxazs_12x24+0x00003720;
				*c_w = 4;
				*c_h = 24;	
				break;

			}
			case timeblue_number_11x18:
			{
			*c_addr = baseAddr+0x00000F78;
			*c_w = 5;
			*c_h = 18;
			break;
			}
			default:
				break;
		}
	}
	else if(c == 0x2F)					/* 斜杠'/' */		
	{
		switch (FontType)
		{
				
			default:
				break;
		}
	}
	else if(c==0x3A)					/*:*/
	{
		switch (FontType)
		{
/***********************************************************/
			case number12px_gray_7x9:
			{
				*c_addr = baseAddr+0x000004EC;
				*c_w = 3;
				*c_h = 9;
				break;
			}
			case number36px_18x26:
			{
				*c_addr = baseAddr+0x00002490;
				*c_w = 10;
				*c_h = 26;
				break;
			}
			case timenumber14px_8x11:
			{
				*c_addr = baseAddr+0x000006E0;
				*c_w = 4;
				*c_h = 11;
				break;
			}	
			case timernumber32px_18x24:
			{
				*c_addr = baseAddr+0x000021C0;
				*c_w = 8;
				*c_h = 24;
				break;
			}
			
			case timestar_time_32x46:
			{
				*c_addr = timestar_time_colon_15x46;
				*c_w = 15;
				*c_h =46;
				break;
			}
			case timesunup_number_34x50:
			{
				*c_addr = timesunup_number_colon_17x50;
				*c_w = 17;
				*c_h = 50;
				break;
			}
			case timemain_number_11x16:
			{
			*c_addr = baseAddr+0x00000DC0;
			*c_w = 4;
			*c_h = 16;
			break;
			}
			case timeblack_timewhite_35x55:
			{
			*c_addr = timeblack_timewhite_colon_12x55;
			*c_w = 12;
			*c_h = 55;
			break;
			}
			case timeblue_time_30x45:
			{
			*c_addr = baseAddr+0x00006978;
			*c_w = 13;
			*c_h = 45;
			break;
			}
			case timesierra_time_32x50:
				{
				*c_addr = baseAddr+0x00007D00;
				*c_w = 12;
				*c_h = 50;
				break;
				}
			default:
				break;
		}
	}
	else if(c==0x25)					/*%*/
	{
		switch (FontType)
		{
			case number36px_18x26:
			{
				*c_addr = char_percent_22x24;
				*c_w = 22;
				*c_h = 24;
				break;
			}	
			default:
				break;
		}
	}
	else if(c==0x27)			/*'*/
	{
		switch (FontType)
		{
			case timesunup_date_9x15:
			{
				*c_addr = timesunup_singlequote_3x15;
				*c_w = 3;
				*c_h = 15;
				break;
			}
			default:
				break;
		}
	}
	else if(c==0x22)			/*"*/
	{
		switch (FontType)
		{
			default:
				break;
		}
	}
	else if(c==0x2A)			//'*'
	{
		switch (FontType)
		{
			default:
				break;
		}
	}
	else if(c==0x23)			//'#'
	{
		switch (FontType)
		{
			default:
				break;
		}
	}
	else if(c==0x2C)		//','
	{
		switch (FontType)
		{
			case timesierra_date_9x15:
				{
				*c_addr = baseAddr+0x00000A8C;
				*c_w = 3;
				*c_h = 15;
				break;
				}
			default:
				break;
		}

		
	}
	return 0;	
}

void DisplayMapCleanAdd(DISPLAYADDRADD ad,uint32_t addr, uint8_t posx, uint8_t posy, uint32_t width, uint32_t height)
{
	uint8_t* Dpos=NULL;
	uint8_t XAreaB=0,XAreaE =0,XAreaU =0,in=0;
	uint32_t addroff =0;
	if(ad==D_INIT)
	{	
		StopRoll();
		memset((uint8_t*)Display_Addr,0,DISPLAY_ADDR_MAX*4);
		memset(Display_XYWH,0,DISPLAY_ADDR_MAX*4);
		TotalAddr =0;
	}
	for(uint16_t i =0; i<DISPLAY_ADDR_MAX; i++)
	{
		Dpos = Display_XYWH+i*AddrOffSet;
		if(Dpos[2]==0&&Dpos[3]==0)
		{
			XAreaB = (posx/Maxcolumns+1);
			XAreaE = ((posx+width-1)/Maxcolumns+1);
			XAreaU = (XAreaE-XAreaB+1);	
			if(XAreaU==1)
			{
				Display_Addr[i] = addr;
				Dpos[0] =posx;	 
				Dpos[1] =posy;
				Dpos[2] =(posx+width-1);
				Dpos[3] =height;	
				TotalAddr++;
			}
			else
			{
				Display_Addr[i] = addr;
				addroff = addr;
				Dpos[0] =posx;	 
				Dpos[1] =posy;
				Dpos[2] =(XAreaB*Maxcolumns-1);
				Dpos[3] =height;	
				TotalAddr++;
				for(uint8_t j=0;j<(XAreaU-2);j++)
				{
					Display_Addr[i+j+1] = addroff+(Dpos[2]-Dpos[0]+1)*Dpos[3]*2;
					addroff = Display_Addr[i+j+1]; 
					Dpos = Display_XYWH+(i+j+1)*AddrOffSet;		
					Dpos[0] =(XAreaB+j)*Maxcolumns;	 
					Dpos[1] =posy;
					Dpos[2] =(XAreaB+j+1)*Maxcolumns-1;
					Dpos[3] =height;	
					TotalAddr++;	
					in++;
				}			
				Display_Addr[i+in+1] = addroff+(Dpos[2]-Dpos[0]+1)*Dpos[3]*2;
				Dpos = Display_XYWH+(i+in+1)*AddrOffSet;		
				Dpos[0] =(XAreaB+in)*Maxcolumns;	 
				Dpos[1] =posy;
				Dpos[2] =posx+width-1;
				Dpos[3] =height;	
				TotalAddr++;	
			}
			break;
		}	
	}
}
void DisplayCharCleanAdd(DISPLAYADDRADD ad, char*CString, uint16_t CharType, uint16_t color, uint8_t posx, uint8_t posy)
{
	uint8_t* Dpos=NULL;
	uint8_t Len = strlen((char*)CString);
	uint16_t buffOffset =0xffff;	
	uint32_t addr=0, width=0, height=0;	
	uint8_t NowposX=0,NowposY=0;
	if(ad==D_INIT)
	{	
		StopRoll();
		memset((uint8_t*)Display_Addr,0,DISPLAY_ADDR_MAX*4);
		memset(Display_XYWH,0,DISPLAY_ADDR_MAX*4);
		TotalAddr =0;
	}
	for(uint16_t i =0; i<DISPLAY_ADDR_MAX; i++)
	{
		Dpos = Display_XYWH+i*AddrOffSet;
		if(Dpos[2]==0&&Dpos[3]==0)
		{
			buffOffset = i;
			break;
		}
	}
	NowposX = posx;
	NowposY = posy;
	for(uint8_t i =0; i<Len; i++)
	{
		if(GetCharParameter(CString[i],CharType,color,&addr,&width,&height,&NowposX,&NowposY)>0)
			continue;	
		if((buffOffset>=DISPLAY_ADDR_MAX)||NowposX>(240-width))
			break;	
		DisplayMapCleanAdd(D_ADD,addr,NowposX,NowposY,width,height);
		buffOffset++;		
		NowposX += width;
		NowposY = posy;
	}
}

void DisplayFontCleanAdd(DISPLAYADDRADD ad, char*CString, uint32_t FontType,uint8_t posx, uint8_t posy)
{
	uint8_t* Dpos=NULL;
	uint8_t Len = strlen((char*)CString);
	uint16_t buffOffset =0xffff;	
	uint32_t addr=0, width=0, height=0;	
	uint8_t NowposX=0,NowposY=0;
	if(ad==D_INIT)
	{	
		StopRoll();
		memset((uint8_t*)Display_Addr,0,DISPLAY_ADDR_MAX*4);
		memset(Display_XYWH,0,DISPLAY_ADDR_MAX*4);
		TotalAddr =0;
	}
	for(uint16_t i =0; i<DISPLAY_ADDR_MAX; i++)
	{
		Dpos = Display_XYWH+i*AddrOffSet;
		if(Dpos[2]==0&&Dpos[3]==0)
		{
			buffOffset = i;
			break;
		}
	}
	NowposX = posx;
	NowposY = posy;
	for(uint8_t i =0; i<Len; i++)
	{
		if(GetFontParameter(CString[i],FontType,&addr,&width,&height)>0)
			continue;	
		if((buffOffset>=DISPLAY_ADDR_MAX)||NowposX>(240-width))
			break;	
		DisplayMapCleanAdd(D_ADD,addr,NowposX,NowposY,width,height);
		buffOffset++;		
		NowposX += width + Dis_Offset;
		NowposY = posy;
	}
}

void DisplayFontCleanAdd_DisOffset(DISPLAYADDRADD ad, char*CString, uint32_t FontType,uint8_t posx, uint8_t posy,uint8_t DisOffset)
{
	uint8_t* Dpos=NULL;
	uint8_t Len = strlen((char*)CString);
	uint16_t buffOffset =0xffff;	
	uint32_t addr=0, width=0, height=0;	
	uint8_t NowposX=0,NowposY=0;
	if(ad==D_INIT)
	{	
		StopRoll();
		memset((uint8_t*)Display_Addr,0,DISPLAY_ADDR_MAX*4);
		memset(Display_XYWH,0,DISPLAY_ADDR_MAX*4);
		TotalAddr =0;
	}
	for(uint16_t i =0; i<DISPLAY_ADDR_MAX; i++)
	{
		Dpos = Display_XYWH+i*AddrOffSet;
		if(Dpos[2]==0&&Dpos[3]==0)
		{
			buffOffset = i;
			break;
		}
	}
	NowposX = posx;
	NowposY = posy;
	for(uint8_t i =0; i<Len; i++)
	{
		if(GetFontParameter(CString[i],FontType,&addr,&width,&height)>0)
			continue;	
		if((buffOffset>=DISPLAY_ADDR_MAX)||NowposX>(240-width))
			break;	
		DisplayMapCleanAdd(D_ADD,addr,NowposX,NowposY,width,height);
		buffOffset++;		
		NowposX += width + DisOffset;
		NowposY = posy;
	}
}

void Dma_DataParse(void)
{
	uint8_t MaxW =( gDpos[2]-gDpos[0]+1);
	uint16_t offset = 240*2*(gDpos[0]%Maxcolumns)+2*gDpos[1];
	for(uint8_t i=0;i<MaxW;i++)
	{
		memcpy((DmaFillBuf+offset+480*i),PP_Buffer0+gDpos[3]*2*i, gDpos[3]*2);
	}
}

void Dma_GetFlashData(void)
{
	uint8_t* Dpos=NULL;
	for(uint16_t i =0;i<TotalAddr;i++)
	{
		Dpos = Display_XYWH+i*AddrOffSet;
		if((Dpos[2]>0)&& (Dpos[2]<(Now_X+Maxcolumns)))
		{
			gDpos[0]=Dpos[0];
			gDpos[1]=Dpos[1];
			gDpos[2]=Dpos[2];
			gDpos[3]=Dpos[3];
			NeedFillData = false;
			DMA_Clean_SpiFlash_Read(Display_Addr[i] , PP_Buffer0, 2*(Dpos[2]-Dpos[0]+1)*Dpos[3]);
			Dpos[0]=0;
			Dpos[2]=0;
			return;				
		}
	}
	Now_X+=Maxcolumns;
	master1Finished = true;	
	NeedFillData = false;
}


void DisplayMap_flash_WithCleanSreen(void)
{
	LCD_SetRamAddr(0,0,240,240);	
	WriteComm(0x2C);
	Power_Resume();	
	Now_X=0;
	memset(PP_Buffer0,0,240*2*Maxcolumns);
	memset(PP_Buffer1,0,240*2*Maxcolumns);
	memset(PP_Buffer2,0,240*2*Maxcolumns);	
	DmaFillBuf = PP_Buffer1; 
	DmaDisplayBuf = PP_Buffer2;
	RowsPos = 0;
	ColumnsPos=0;	
	Dma_GetFlashData();
	DMA_Clean_SpiFlash_Read_WaitFor_Finish();
	for (uint16_t i = 0; i < (240/Maxcolumns); i++) 
	{
		DmaFillBuf = GetCDBuf(DmaFillBuf); 
		DmaDisplayBuf = GetCDBuf(DmaDisplayBuf);
		memset(DmaFillBuf,0,240*2*Maxcolumns);
		RowsPos = 0;
		ColumnsPos=0;
		Dma_GetFlashData();
		_fill_lcd_from_dma_buffer(DmaDisplayBuf, 2*240*Maxcolumns,0);
		DMA_Clean_SpiFlash_Read_WaitFor_Finish();	
	}
	_DC_H();
	Power_Suspend();
}


void DisplayMap_flash(uint32_t addr, uint8_t posx, uint8_t posy, uint32_t width, uint32_t height)
{
	uint16_t i;
	LCD_SetRamAddr(posy,posx,height,width);
	WriteComm(0x2C);
	SpiFlash_Read((addr+0*(height*2)),PP_Buffer1,2*height);
	for (i = 1; i < width+1; i++) 
	{
		if(i%2)
		{
			DMA_SpiFlash_Read((addr+i*(height*2)),PP_Buffer2,2*height);
			_fill_lcd_from_dma_buffer(PP_Buffer1, 2*height,0);
			DMA_SpiFlash_Read_WaitFor_Finish();	
		}
		else
		{
			DMA_SpiFlash_Read((addr+i*(height*2)),PP_Buffer1,2*height);	
			_fill_lcd_from_dma_buffer(PP_Buffer2, 2*height,0);	
			DMA_SpiFlash_Read_WaitFor_Finish();	
		}
	}
	_DC_H();
}

void DisplayFontWithoutCls(char*CString, uint32_t FontType,uint8_t StrLen,uint8_t posx, uint8_t posy,uint8_t width,uint8_t height)
{
	
	uint8_t Len=StrLen;
	uint32_t addr;
	uint8_t NowposX=0,NowposY=0;
	NowposX = posx;
	NowposY = posy;
	for(uint8_t i =0; i<Len; i++)
	{			
		if((NowposX>=LCD_SIZE_W)||NowposX>(240-width))
			break;
		addr = FontType + (CString[i]-0x30)*2*width*height;
		DisplayMap_flash(addr, NowposX, NowposY, width, height);
	
		NowposX += width;
		NowposY = posy;		
	}
}

void DisplayFontWithoutCls_DisOffset(uint8_t Disffset,char*CString, uint32_t FontType,uint8_t StrLen,uint8_t posx, uint8_t posy,uint8_t width,uint8_t height)
{
	
	uint8_t Len=StrLen;
	uint32_t addr;
	uint8_t NowposX=0,NowposY=0;
	NowposX = posx;
	NowposY = posy;
	for(uint8_t i =0; i<Len; i++)
	{			
		if((NowposX>=LCD_SIZE_W)||NowposX>(240-width))
			break;
		addr = FontType + (CString[i]-0x30)*2*width*height;
		DisplayMap_flash(addr, NowposX, NowposY, width, height);
	
		NowposX += width+Disffset;
		NowposY = posy;		
	}
}

void DisplayDotFont(const uint8_t *Map,uint16_t color,uint8_t x,uint8_t y,uint8_t width,uint8_t height)
{
	uint8_t h=0;
	uint32_t m=0;
	h=height>>3;
	LCD_SetRamAddr(y,x,height,width);
	WriteComm(0x2C);
	for(uint16_t i=0; i<width; i++)
	{
		for(uint8_t n=0; n<h; n++)
		{
			for(uint8_t j=0; j<8; j++)
			{
			
				if(Map[i+n*width]&(0x01<<j))
				{
					tft_All_data[2*m]=(color>>8)&0xfF;
					tft_All_data[2*m+1]=color&0xfF;
					m++;				
				}
				else
				{
					if(NeedBackground)
					{
						tft_All_data[2*m]=0x21;
						tft_All_data[2*m+1]=0x04;
				       }
					else
					{
						tft_All_data[2*m]=0x00;
						tft_All_data[2*m+1]=0x00;
					}
					m++;
				}
			}
		}	
	}
	_fill_lcd_from_dma_buffer((uint8_t*)tft_All_data,width*height*2,0);
}

void DisplayDotMap(const uint8_t *Map,uint16_t color,uint8_t x,uint8_t y,uint8_t width,uint8_t height)
{
	uint8_t h=0;
	uint32_t m=0;
	h=height>>3;
	LCD_SetRamAddr(y,x,height,width);
	WriteComm(0x2C);

	memset(PP_Buffer0,0,480);
	for(uint16_t i=0; i<width; i++)
	{
		for(uint8_t n=0; n<h; n++)
		{
			for(uint8_t j=0; j<8; j++)
			{
			
				if(Map[i+n*width]&(0x01<<j))
				{
					PP_Buffer0[2*m]=(color>>8)&0xfF;
					PP_Buffer0[2*m+1]=color&0xfF;
					m++;				
				}
				else
				{
					PP_Buffer0[2*m]=0x00;
					PP_Buffer0[2*m+1]=0x00;
					m++;
				}
			}
		}
		_fill_lcd_from_dma_buffer((uint8_t*)PP_Buffer0,height*2,0);
		m=0;
		memset(PP_Buffer0,0,480);
	}
	
}

void DrawOneLine(uint16_t colour,uint8_t y)
{
	uint16_t Hcolour = colour&0xFF;
	uint16_t Lcolour = (colour>>8)&0xFF;
	colour = (Hcolour<<8)|Lcolour;
	LCD_SetRamAddr(y,0,1,240);
	WriteComm(0x2C);
	_Draw_One_line(colour);
}

void Lcd_Test0(void)
{
	ClearLcdDisplay(0,0,240,240);
	DisplayMap_flash(0x00000000UL,28,100,146,112);
}



void Lcd_Test1(void)
{
	ClearLcdDisplay(0,0,240,240);
	DisplayLine("Mykronoz",D_MODE3,WHITE,40,90,160,16);
	//DisplayMap_flash(0x00007FC0UL,28,100,176,112);
}

uint8_t GetCharDisXpos(char *str, uint16_t CharType, uint8_t xpos, uint8_t DisMode)
{
	char *Pdata = str;
	uint16_t StringLen = strlen((char*)str);
	uint16_t posx = xpos;

	for(uint16_t i=0; i<StringLen; i++)
	{
		char c = *Pdata;
		if(CharType==CHAR_12X24)
		{	
			if(c>=0x41 && c<=0x5a)		/*大写*/
			{
				uint8_t cmap[26]={	16,13,14,14,12,12,14,
									14,4,12,14,11,18,14,
									15,13,14,14,14,15,
									14,16,22,15,15,14};
					posx+=cmap[(c-'A')];
			}
			
			else if (c>=0x61 && c<=0x7a)		/*小写*/
			{
				uint8_t cmap[26]={	12,12,12,12,12,9,12,
									11,4,6,11,4,19,11,
									13,12,12,7,11,8,
									11,12,18,12,12,11}; 		
				posx+=cmap[(c-'a')];
			}
			else if(c>=0x30 && c<=0x39)			/*数字*/
			{
				posx+=12;
			}
			else if(c == 0x3A)					/*:*/
			{
				posx+=4;		
			}
			else if(c == 0x2C)					/*,*/
			{
				posx+=4;		
			}
			else if(c == 0x2e)					/*.*/
			{
				posx+=4;		
			}
			else if(c == 0x21)					/*!*/
			{
				posx+=4;		
			}
			else if(c == 0x2D)					/*-*/
			{
				posx+=7;		
			}
			else if(c == 0x2B)					/*+*/
			{
				posx+=12;		
			}
			else if(c == 0x3f)					/*?*/
			{
				posx+=9;		
			}
			else if(c == 0x27)					/*'*/
			{
				posx+=4;		
			}
			else if(c == 0x2f)					/*斜杠*/		
			{
				posx+=9;	
			}
			
			else if(c == 0x20)					/*空格*/		
			{
				posx+=6;
			}	
			else 
			{
				posx+=4;
			}
		}
		else if(CharType==CHAR_7X14)
		{	
			if(c>=0x41 && c<=0x5a)		/*大写*/
			{
				uint8_t cmap[26]={	10,8,9,9,8,8,9,
									9,3,8,9,7,11,9,
									9,8,9,9,9,9,
									9,10,13,9,10,9};
					posx+=cmap[(c-'A')];
			}
			
			else if (c>=0x61 && c<=0x7a)		/*小写*/
			{
				uint8_t cmap[26]={	7,7,8,7,8,6,7,
									7,3,4,7,3,12,7,
									8,7,7,5,7,5,
									7,8,11,8,8,7};			
				posx+=cmap[(c-'a')];
			}
			else if(c>=0x30 && c<=0x39)			/*数字*/
			{
				posx+=9;
			}
			else if(c == 0x3A)					/*:*/
			{
				posx+=3;		
			}
			else if(c == 0x2C)					/*,*/
			{
				posx+=4;		
			}
			else if(c == 0x2e)					/*.*/
			{
				posx+=3;		
			}
			else if(c == 0x21)					/*!*/
			{
				posx+=3;		
			}
			else if(c == 0x2D)					/*-*/
			{
				posx+=5;		
			}
			else if(c == 0x2B)					/*+*/
			{
				posx+=7;		
			}
			else if(c == 0x3f)					/*?*/
			{
				posx+=6;		
			}
			else if(c == 0x27)					/*'*/
			{
				posx+=3;		
			}
			else if(c == 0x2f)					/*斜杠*/		
			{
				posx+=5;	
			}
			
			else if(c == 0x20)					/*空格*/		
			{
				posx+=3;
			}	
			else 
			{
				posx+=4;
			}
		}	
		else if(CharType==CHAR_11X20)
		{	
			if(c>=0x41 && c<=0x5a)		/*大写*/
			{
				uint8_t cmap[26]={	15,12,13,12,11,11,13,
									13,4,11,13,11,16,13,
									14,12,12,12,12,13,
									12,14,19,14,14,12};
					posx+=cmap[(c-'A')];
			}
			
			else if (c>=0x61 && c<=0x7a)		/*小写*/
			{
				uint8_t cmap[26]={	11,11,11,11,11,8,11,
									10,4,6,11,4,17,10,
									12,11,11,7,10,8,
									10,11,16,11,11,10};			
				posx+=cmap[(c-'a')];
			}
			else if(c>=0x30 && c<=0x39)			/*数字*/
			{
				posx+=12;
			}
			else if(c == 0x3A)					/*:*/
			{
				posx+=4;		
			}
			else if(c == 0x2C)					/*,*/
			{
				posx+=4;		
			}
			else if(c == 0x2e)					/*.*/
			{
				posx+=4;		
			}
			else if(c == 0x21)					/*!*/
			{
				posx+=4;		
			}
			else if(c == 0x2D)					/*-*/
			{
				posx+=7;		
			}
			else if(c == 0x2B)					/*+*/
			{
				posx+=12;		
			}
			else if(c == 0x3f)					/*?*/
			{
				posx+=8;		
			}
			else if(c == 0x27)					/*'*/
			{
				posx+=4;		
			}
			else if(c == 0x2f)					/*斜杠*/		
			{
				posx+=9;	
			}
			
			else if(c == 0x20)					/*空格*/		
			{
				posx+=3;
			}	
			else 
			{
				posx+=4;
			}		
		}			
		Pdata++; 
	}

	if(DisMode==DIS_L_ALIGNED)
	{
		return xpos;
	}
	else if(DisMode==DIS_R_ALIGNED)
	{
		if((posx-xpos)<=LCD_SIZE_W)
		{
			uint8_t x = LCD_SIZE_W - posx + xpos;
			return ((x>xpos)?(x-xpos):x);
		}
		else
			return xpos;
	}
	else if(DisMode==DIS_CENTER)
	{
		if(posx<=LCD_SIZE_W)
			return (((LCD_SIZE_W - posx)>>1) + xpos);
		else
			return xpos;	
	}
	else
			return xpos;
}

/** @} */
