#include "headfile.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>




#define ERR_NONE 0
#define ERR_END  1
#define ERR_NOMEM 2

#define uint8_t uint8
#define uint16_t uint16
#define uint32_t uint32

#define Transfer_Len   16

#define MSB_HEX(h) ((h&0xf0)>>4)
#define LSB_HEX(h) ((h&0x0f)>>0)
#define HEX_ASCII(h) hex_tab[h]

static const char hex_tab[] = "0123456789ABCDEF";

 uint8  hex_begin[] =":020000040000FA";
 uint8  hex_ends_2[]=":047FFC0072756C75B9";
 uint8  hex_ends_3[]=":04000005000000698E";
 uint8  hex_ends_4[]=":00000001FF";

void mdelay(uint32_t ms)
{
	nrf_delay_ms(ms);
}


void AS7000_PowerON_Loader(void)
{

	HRM_PWR_PIN_OUT;
	HRM_PWR_PIN_H;
	for(uint8 i=0;i<10;i++)
	{	delay_10ms;	}
	
}


bool WriteHexStrToAS7000(uint8 * Hexdatastr)
{
	uint8 ack_1;	
	uint16 RetryTimes=0;
		do
		{
			ack_1 =0xFF;			        
			ReadData_AS7000(0x30,0x00, &ack_1,1);
            
			mdelay(1);
		   if((++RetryTimes)>100)
			{	
				return FALSE;	
			}
		}while(ack_1 !=0);
	

	if(ack_1 ==0)	
	{
		if(WriteData_AS7000(0x30, 0x00, Hexdatastr, strlen((char*)Hexdatastr)) ==0)
			return FALSE;		
	}
	return TRUE;
}



uint8 AS7000_upgrade(int mode)
{
    uint8 upgrade_success=0;
	uint8_t binbuffer[16];
	uint8_t hexbuffer[21];
	uint8_t hexforAS7000buffer[43];
    uint8_t app_id;
	uint8_t  ack;	
	uint8_t ackdata[3];
	uint8_t i;
	uint16_t cc=0;
    uint16_t offset = 0;
	uint32_t addr_Flash;
    uint8 SendHexLineTimes;
    uint8 ReadAckTimes;
    uint8 RetryTimes;

	SendHexLineTimes=0;
	ReadAckTimes =0;
	RetryTimes=0;
	
//1.Master 发送 进入Boot升级指令

	AS7000DeviceSettings();
	AS7000_PowerON_Loader();


    app_id = 0x01;
	WriteData_AS7000(0x30,0x04, &app_id, 1);
    mdelay(35);

    do
	{   
        ack= 0xFF;
		ReadData_AS7000(0x30,0x00 , &ack, 1);
	}while(ack !=0);

	 upgrade_success = WriteHexStrToAS7000(hex_begin);


		offset = 0x0000;
        hexbuffer[0]=0x10;
		hexbuffer[3]=0x00;

		hexforAS7000buffer[0]=0x3A;
		do{
			addr_Flash = Ota_Hr_Base + offset;
			SpiFlash_Read(addr_Flash, binbuffer, Transfer_Len);

		//bin文件生成hex格式

			hexbuffer[1]=(uint8)(offset>>8);
			hexbuffer[2]=(uint8)(offset);

			for(i=4;i<20;i++)
			{	
				hexbuffer[i]=binbuffer[i-4];
			}
			cc = 0;
			for(i=0;i<20;i++)
			 {	cc +=hexbuffer[i];	}
			cc =cc%256;
			cc =0x100 - cc;
			hexbuffer[20]=(uint8)cc;

		//hex格式生成hex文件字符串

			for(i=0;i<21;i++)
			{
				hexforAS7000buffer[2*i+1]=HEX_ASCII(MSB_HEX(hexbuffer[i]));
				hexforAS7000buffer[2*i+2]=HEX_ASCII(LSB_HEX(hexbuffer[i]));
			}
		
			
				do
		        {					
                    ack =0xFF;
					ReadData_AS7000(0x30,0x00, &ack,1);
										
					if(++ReadAckTimes>=200)
					{
						ReadAckTimes =0;
						break;
					}
				}while(ack !=0);


			for(i=0; i<3; i++)
			{	ackdata[i]=0xFF;	
				ReadData_AS7000(0x30, 0x00, &ackdata[i],1);
			}

				if( (ackdata[0]==0)&&(ackdata[1]==0)&&(ackdata[2]==0) )	
				{
					if(WriteData_AS7000(0x30, 0x00, hexforAS7000buffer,43))
				    {
					    
						offset +=0x10;
						SendHexLineTimes =0;
						ReadAckTimes=0;
					}
				}
             //mdelay(1); 
        			  				
			if(++SendHexLineTimes>=50)
			{	
				SendHexLineTimes =0;
				RetryTimes++;
			}

		    if(RetryTimes>=30)
		    {	
				ReadData_AS7000(0x30,0x04 , &app_id, 1);
				RetryTimes =0;
				break;
			}
		}while(offset !=0x8000);
		
		WriteHexStrToAS7000(hex_begin);
		WriteHexStrToAS7000(hex_ends_2);
		WriteHexStrToAS7000(hex_ends_3);
		WriteHexStrToAS7000(hex_ends_4);
		
	
		return upgrade_success;
}


