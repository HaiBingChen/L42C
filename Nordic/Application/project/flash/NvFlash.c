

#include "headfile.h"





//128 page 每个page  2k

//128K字节之后
//#define NV_FLASH_USED_START_ADD  (0x30000)
//186k之后
#define NV_FLASH_USED_START_ADD  (0x2E800)

//#define NV_FLASH_USED_START_PAGE (64)

//0x20000-8*1024=1E000 (用户可用116k数据)
//#define NV_FLASH_USED_MAX_ADD    (0x1D000)
//4k
//#define NV_FLASH_USED_MAX_ADD    (0x1000)
//50k
#define NV_FLASH_USED_MAX_ADD  (0xC800)


// 0xB9 flash	深度睡眠
#define FLASH_DEEP_POWER_DOWN     0xB9    
//退出休眠
#define FLASH_RELEASE_DEEP_POWER_DOWN     0xAB 

#define CS              P1_2
#define CS_DISABLED     1
#define CS_ENABLED      0

#define xNV_FLASH_TEST


void WriteByte(uint8 write)
{ 
  uint32 TimeOut=0; 
 // U0CSR &= ~0x02;                 // Clear TX_BYTE
 // U0DBUF = write;
  
  //SET_32KHZ_OSC();
 // while (!(U0CSR & 0x02))        // Wait for TX_BYTE to be set
  {
    //SetPowerMode(0);
    //if(++TimeOut>200000);
  }
  //SET_OSC_TO_XOSC(); 
}


inline void  Select_Flash(void)
{
  //CS = CS_ENABLED;
}
inline void  NotSelect_Flash(void)
{
  //CS = CS_DISABLED;
}

uint8 Mx25LxxFlashSendByte(uint8 byte)
{  
   WriteByte(byte);
   return (1);
}

/******************************************************************************************
*函数名:		Mx25LxxDeepPower
*参数:			
*返回值:		
*描述:			FLASH   进入休眠
*编写:			statdream 
*版本信息:	    v1.0
******************************************************************************************/ 

void Mx25LxxResume(void)
{
  Select_Flash();	
  //Mx25LxxFlashSendByte(FLASH_DEEP_POWER_DOWN);
  NotSelect_Flash();	 
//关闭IO口
	//FlashSpiGpioDisable();
}

/******************************************************************************************
*函数名:		Mx25LxxReleaseDeepPower
*参数:			
*返回值:		
*描述:			FLASH   退出休眠
*编写:			statdream 
*版本信息:	    v1.0
******************************************************************************************/ 

void Mx25LxxSuspend(void)
{
//开启IO
  Select_Flash();	
  //Mx25LxxFlashSendByte(FLASH_RELEASE_DEEP_POWER_DOWN);
  NotSelect_Flash();	
}



uint8 NvFlashWrite( uint32 add,void *pBuf ,  uint16 len)
{	
	if((add>=0x9000UL && add<0xC800UL)||(add>=0xF800UL && add<0x10000UL)) //在允许可写的地址范围内
       {
       	uint16 t16 ;//= (add+NV_FLASH_USED_START_ADD)/HAL_FLASH_WORD_SIZE;//擦出CRC flag区
		Mx25LxxResume();
		if ((t16 % (512)) == 0)
		{
		   // HalFlashErase(t16 / 512);
		}
		
		//HalFlashWrite((uint16)((add+NV_FLASH_USED_START_ADD)/HAL_FLASH_WORD_SIZE), (uint8 *)pBuf, len>>2);
		Mx25LxxSuspend();
	        return len;
       }
	else
		return 0;
}

uint8 NvFlashRead( uint32 add,void *pBuf ,  uint16 len)
{
	uint16 t16;// = (add+NV_FLASH_USED_START_ADD)/HAL_FLASH_WORD_SIZE;//擦出CRC flag区

	//if(add>=NV_FLASH_USED_MAX_ADD)
	//	return 0;

        Mx25LxxResume();
	//HalFlashRead((t16 / 512),(t16 % (HAL_FLASH_PAGE_SIZE / HAL_FLASH_WORD_SIZE)) << 2, (uint8 *)pBuf, len);
        Mx25LxxSuspend();
        return len;	

}

void NvFlashErase( uint32 add)
{
	if(add == 0x9000UL ||add == 0x9800UL ||add == 0xF800UL )
	{
		uint16 t16; //= (add+NV_FLASH_USED_START_ADD)/HAL_FLASH_WORD_SIZE;//擦出CRC flag区

		Mx25LxxResume();
		if ((t16 % (512)) == 0)
		{
		    //HalFlashErase(t16 / 512);
		}
		Mx25LxxSuspend();
	}
}

#ifdef NV_FLASH_TEST
void NvFlashTest(void)
{
  static uint8 buf[16]={10,11,12,13,14,15,16,17};
  NvFlashWrite( 0,8,buf );
  memset(buf,0,8);
  NvFlashRead( 0,buf ,8);
  if(buf[1])
    InitWatchDog();
  
  memset(buf,20,8);
  NvFlashWrite( 8,  8,buf );
  memset(buf,0,8);
  NvFlashRead( 0,buf ,  16);
}
#endif

