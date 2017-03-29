//#include "headfile.h"
// Standard C Included Files
#include "fsl_spi_dma.h"
#include "fsl_dmamux.h"
#include "board.h"
#include "fsl_debug_console.h"

#include "pin_mux.h"
//#include "clock_config.h"
#include "fsl_common.h"
#include "fsl_port.h"
#include "pin_mux.h"
#include "fsl_gpio.h"
#include "head_dma.h"

#include "ExtSpiFlash.h"
#include "seeed_lcd.h"



#define EFLASH_PAGE_SIZE	256





#define Flash_CS  0

#define FLASH_CS_L			GPIO_ClearPinsOutput(GPIOB, 1U << Flash_CS) 
#define FLASH_CS_H			GPIO_SetPinsOutput(GPIOB, 1U << Flash_CS)


#define MASTER_TRANSFER_TIMEOUT1     (5000U)             /*! Transfer timeout of master - 5s */


#define SPI_FLASH_BUSY		0x01

#define E_JEDEC		0x9f
#define E_RDSR		0x05
#define E_WREN		0x06
#define E_WRDI		0x04
#define E_ULBPR		0x98
#define E_CE			0xc7
#define E_SE			0x20
#define E_BE			0xd8
#define E_PP			0x02
#define E_READ		0x03
#define E_DPD		0xb9
#define E_RDPD		0xab

void u_Delay(int l)
{
	while(l--); 
}
static void delay(uint32_t mtime)
{
	uint32_t i = 0;
	uint32_t j = 0;
	
	for(i=0; i<mtime; i++)
	{
		for(j=0; j<1000; j++)
		{
			__asm("nop");
		}
	}

}

void Select_Flash_CS(void)
{
	FLASH_CS_L;
}

void Release_Flash_CS(void)
{
	FLASH_CS_H;
}

/**********************************************
//
//写命令函数

**********************************************/

#define MASTER_TRANSFER_TIMEOUT0     (5000U)             /*! Transfer timeout of master - 5s */
void SPI_Write(uint8_t data)
{
	spi_transfer_t xfer = {0};
	uint8_t *pdata;
	pdata=&data;
	xfer.txData = pdata;
	xfer.rxData = NULL;
	xfer.dataSize = 1;
	master1Finished = false;
	SPI_MasterTransferDMA(SPI1, &master1Handle, &xfer);
	while (master1Finished != true)
	{
		// waiting until the buff1 full, and ready to be send to SPI LCD.
	}
}


/**********************************************
//
//读命令函数

**********************************************/
uint8_t SPI_Read(uint8_t *data, uint32_t len)
{
	spi_transfer_t xfer = {0};
	xfer.txData = NULL;
	xfer.rxData = data;
	xfer.dataSize = len;
	master1Finished = false;
	SPI_MasterTransferDMA(SPI1, &master1Handle, &xfer);
	while (master1Finished != true)
	{
		// waiting until the buff1 full, and ready to be send to SPI LCD.
	}	
	return 1;
}

uint8_t DMA_SPI_Read(uint8_t *data, uint32_t len)
{
	spi_transfer_t xfer = {0};
	xfer.txData = NULL;
	xfer.rxData = data;
	xfer.dataSize = len;
	master1Finished = false;
	SPI_MasterTransferDMA(SPI1, &master1Handle, &xfer);
	return 1;
}

void DMA_SpiFlash_Read_WaitFor_Finish(void)
{
	while (master1Finished != true)
	{
		// waiting until the buff1 full, and ready to be send to SPI LCD.
	}	
	Release_Flash_CS(); 
	Power_Suspend();
}

void DMA_Clean_SpiFlash_Read_WaitFor_Finish(void)
{
	while (master1Finished != true)
	{
		// waiting until the buff1 full, and ready to be send to SPI LCD.
		if(NeedFillData)
		{
			//PRINTF("----------------------------------Time BG \r\n");
			Dma_GetFlashData();
		}
	}	
}



void Power_Suspend(void)
{
	Select_Flash_CS();
	SPI_Write(E_DPD);
	Release_Flash_CS();	
}    

void Power_Resume(void)
{
	Select_Flash_CS();
	SPI_Write(E_RDPD);
	Release_Flash_CS();	
}    

void WriteEnable(void)
{
	Select_Flash_CS();
	SPI_Write(E_WREN);
	Release_Flash_CS();	
}
void WriteDisable(void)
{
	Select_Flash_CS();
	SPI_Write(E_WRDI);
	Release_Flash_CS();	
}

void UnlockGlobalBlockProtection(void)
{
	Power_Resume();
	WriteEnable();
	Select_Flash_CS();
	SPI_Write(E_ULBPR);
	Release_Flash_CS();		
	Power_Suspend();
}

void WaitForStatusRegister(uint8_t status)
{
	uint8_t FLASH_Status = 0;
	uint32_t ECount=0;
	do
	{
		Select_Flash_CS();
		SPI_Write(E_RDSR); 
		SPI_Read(&FLASH_Status,1);
		delay(3); 	
		ECount++;
		Release_Flash_CS();
	}	
	while ((FLASH_Status & status)&&(ECount<300000));//若没有写完则一直等待，保持CS=0	
}

uint32_t SpiFlash_ReadJedecID(void)
{
	uint32_t DeviceID=0;
	uint8_t DeviceID_8[4]={0};
	Power_Resume();
	Select_Flash_CS();
	SPI_Write(E_JEDEC);    	
	u_Delay(5);
	SPI_Read(DeviceID_8,4);
	DeviceID |= ((uint32_t)DeviceID_8[0] << 16);
	DeviceID |= ((uint16_t)DeviceID_8[1] << 8);
	DeviceID |= (uint16_t)DeviceID_8[2];
	Release_Flash_CS();
	Power_Suspend();
	return DeviceID;
}

uint8_t SpiFlash_GetBusyBit(void)
{
	uint8_t FLASH_Status = 0;
	Power_Resume();
	Select_Flash_CS();
	SPI_Write(E_RDSR); 	
	SPI_Read(&FLASH_Status,1);
	Release_Flash_CS();
	Power_Suspend();
	return (FLASH_Status & SPI_FLASH_BUSY);
}

void SpiFlash_4KSector_Erase(uint32_t EraseAddr)
{
	Power_Resume();
	WriteEnable();
	Select_Flash_CS();
    	SPI_Write(E_SE);
	SPI_Write((uint8_t)(EraseAddr >> 16));
	SPI_Write((uint8_t)(EraseAddr>> 8));
	SPI_Write((uint8_t)(EraseAddr));	
	Release_Flash_CS();	
	WaitForStatusRegister(SPI_FLASH_BUSY);
	Power_Suspend();
}

void SpiFlash_64KBlock_Erase(uint32_t EraseAddr)
{
	Power_Resume();
	WriteEnable();
	Select_Flash_CS();
    	SPI_Write(E_BE);
	SPI_Write((uint8_t)(EraseAddr >> 16));
	SPI_Write((uint8_t)(EraseAddr>> 8));
	SPI_Write((uint8_t)(EraseAddr));	
	Release_Flash_CS();
	WaitForStatusRegister(SPI_FLASH_BUSY);
	Power_Suspend();
}  

void SpiFlash_ChipErase(void)
{
	Power_Resume();
	WriteEnable();
	Select_Flash_CS();
       SPI_Write(E_CE);
	Release_Flash_CS();		
	WaitForStatusRegister(SPI_FLASH_BUSY);
	Power_Suspend();
}

void SpiFlash_PageWrite(uint32_t addr,uint8_t* pBuf,uint16_t len)
{  
	WriteEnable();
	Select_Flash_CS();
	SPI_Write(E_PP);//页写指令
	SPI_Write((uint8_t)(addr >> 16));
	SPI_Write((uint8_t)(addr>> 8));
	SPI_Write((uint8_t)(addr));
	while(len--)
	{
		SPI_Write(*pBuf);
		pBuf++;
	}
	Release_Flash_CS();	
	WaitForStatusRegister(SPI_FLASH_BUSY);
}

void SpiFlash_Write(uint32_t addr,uint8_t*pBuf, uint16_t len)
{
	uint8_t NumOfPage = 0, NumOfSingle = 0, Addr = 0, count = 0, temp = 0;
	Addr = addr%EFLASH_PAGE_SIZE;					/*	确定地址在单页内位置*/
	count = EFLASH_PAGE_SIZE - Addr;					/*	确定地址在单页剩余可写字节数*/
	NumOfPage =  len/EFLASH_PAGE_SIZE;				/*	确定要写入数据需要多少整页*/
	NumOfSingle = len%EFLASH_PAGE_SIZE;				/*	确定要写入数据除去整页的剩余字节数*/
	Power_Resume();
	if (Addr == 0) 									/* 写入的地址是页对齐*/
	{
		if (NumOfPage == 0)							/* 写入数据量小于一页*/
			SpiFlash_PageWrite(addr, pBuf, len);
		else 										/* 写入数据量大于一页*/
		{
			while (NumOfPage--)						/* 写整页的*/
			{
				SpiFlash_PageWrite(addr,pBuf, EFLASH_PAGE_SIZE);
				addr +=  EFLASH_PAGE_SIZE;
				pBuf += EFLASH_PAGE_SIZE;
			}
			SpiFlash_PageWrite(addr, pBuf,NumOfSingle);  	/* 写剩余的*/
		}
	}
	else 											/* 写入的地址不是页对齐*/
	{
		if (NumOfPage == 0) 							/* 写入数据量小于一页*/
		{
			if (NumOfSingle > count)					/* 写入数据量大于本页剩余字节数*/
			{ 				
				temp = NumOfSingle - count;
				SpiFlash_PageWrite(addr,pBuf, count);		/* 先写完本页剩余*/
				addr +=  count;
				pBuf += count;
				SpiFlash_PageWrite(addr, pBuf, temp);		/* 剩下的写到下一页*/
			}
			else
				SpiFlash_PageWrite(addr, pBuf,len);	 	/* 本页就能装下*/
		}
		else 										/* 写入数据量大于一页*/
		{
			len -= count;
			NumOfPage =  len / EFLASH_PAGE_SIZE;
			NumOfSingle = len % EFLASH_PAGE_SIZE;
			SpiFlash_PageWrite(addr, pBuf, count);		/* 先写完本页剩余，后面就按页对齐的来*/	
			addr +=  count;
			pBuf += count;
			while (NumOfPage--)
			{
				SpiFlash_PageWrite(addr,pBuf, EFLASH_PAGE_SIZE);
				addr +=  EFLASH_PAGE_SIZE;
				pBuf += EFLASH_PAGE_SIZE;
			}
			if (NumOfSingle != 0)
				SpiFlash_PageWrite(addr,pBuf,NumOfSingle);
		}
	}
	Power_Suspend();
}

void SpiFlash_Read( uint32_t addr,uint8_t*pBuf , uint32_t len)
{
	Power_Resume();
	Select_Flash_CS();
	
	SPI_Write(E_READ);//写读取数据指令    
	SPI_Write((uint8_t)(addr >> 16));
	SPI_Write((uint8_t)(addr>> 8));
	SPI_Write((uint8_t)(addr));
	SPI_Read(pBuf,len);
	Release_Flash_CS(); 
	Power_Suspend();
}

void DMA_SpiFlash_Read( uint32_t addr,uint8_t*pBuf , uint32_t len)
{
	Power_Resume();
	Select_Flash_CS();
	
	SPI_Write(E_READ);//写读取数据指令    
	SPI_Write((uint8_t)(addr >> 16));
	SPI_Write((uint8_t)(addr>> 8));
	SPI_Write((uint8_t)(addr));
	DMA_SPI_Read(pBuf,len);	
}

void DMA_Clean_SpiFlash_Read( uint32_t addr,uint8_t*pBuf , uint32_t len)
{
	Select_Flash_CS();
	SPI_Write(E_READ);//写读取数据指令    
	SPI_Write((uint8_t)(addr >> 16));
	SPI_Write((uint8_t)(addr>> 8));
	SPI_Write((uint8_t)(addr));
	CleanDisplayMode = true;
	DMA_SPI_Read(pBuf,len);	
}


void SpiFlash_IO_Init(void)
{         
	GPIO_PinInit(GPIOB, 0u, \
                 &(gpio_pin_config_t){kGPIO_DigitalOutput, (1)}) ;
}

void SpiFlash_Init(void)
{
	SpiFlash_IO_Init();
	UnlockGlobalBlockProtection();
	uint32_t b =SpiFlash_ReadJedecID();
	PRINTF("JedecID=0x%x \r\n",b);
}


