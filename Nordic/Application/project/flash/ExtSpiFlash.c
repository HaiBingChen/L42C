#include "headfile.h"


#define EFLASH_PAGE_SIZE	256


#define FLASH_CS_OUT		nrf_gpio_pin_dir_set(FLASH_CS_PIN, NRF_GPIO_PIN_DIR_OUTPUT)
#define FLASH_CLK_OUT		nrf_gpio_pin_dir_set(FLASH_CLK_PIN, NRF_GPIO_PIN_DIR_OUTPUT)
#define FLASH_DI_OUT		nrf_gpio_pin_dir_set(FLASH_DI_PIN, NRF_GPIO_PIN_DIR_OUTPUT)
#define FLASH_DO_OUT		nrf_gpio_pin_dir_set(FLASH_DO_PIN, NRF_GPIO_PIN_DIR_OUTPUT)
#define FLASH_DO_IN		       nrf_gpio_pin_dir_set(FLASH_DO_PIN, NRF_GPIO_PIN_DIR_INPUT)

#define FLASH_CS_H			nrf_gpio_pin_set(FLASH_CS_PIN)
#define FLASH_CS_L			nrf_gpio_pin_clear(FLASH_CS_PIN)

#define FLASH_CLK_H			nrf_gpio_pin_set(FLASH_CLK_PIN)
#define FLASH_CLK_L			nrf_gpio_pin_clear(FLASH_CLK_PIN)

#define FLASH_DI_H			nrf_gpio_pin_set(FLASH_DI_PIN)
#define FLASH_DI_L			nrf_gpio_pin_clear(FLASH_DI_PIN)

#define FLASH_DO_H			nrf_gpio_pin_set(FLASH_DO_PIN)
#define FLASH_DO_L			nrf_gpio_pin_clear(FLASH_DO_PIN)
#define FLASH_DO_DATA	       nrf_gpio_pin_read(FLASH_DO_PIN)



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
//д�����

**********************************************/
void SPI_Write(uint8 data)
{
	int8 i;  
	for(i=0;i<8;i++)
	{  
		FLASH_CLK_L;
		if(data&0x80)
			FLASH_DI_H;
		else
			FLASH_DI_L;
 
		FLASH_CLK_H;
  
		data<<=1;
	}  
}

/**********************************************
//
//�������

**********************************************/
uint8 SPI_Read(void)
{
	int8 i;
	uint8 data=0;    
	for(i=7;i>=0;i--)
	{
		FLASH_CLK_L;

		FLASH_CLK_H;
		if(FLASH_DO_DATA)
			data|=(0x1<<i);

	}  
	return data;
}

void SpiFlash_Enable_IO(void)
{
	FLASH_CS_OUT;          
	FLASH_CLK_OUT;        
	FLASH_DI_OUT;       
	FLASH_CS_H;
	FLASH_CLK_H;
	FLASH_DI_H;
}

void SpiFlash_Disable_IO(void)
{
	nrf_gpio_pin_dir_set(FLASH_CS_PIN, NRF_GPIO_PIN_DIR_INPUT);	
	nrf_gpio_pin_dir_set(FLASH_CLK_PIN, NRF_GPIO_PIN_DIR_INPUT);	
	nrf_gpio_pin_dir_set(FLASH_DI_PIN, NRF_GPIO_PIN_DIR_INPUT);	
	nrf_gpio_pin_dir_set(FLASH_DO_PIN, NRF_GPIO_PIN_DIR_INPUT);
}

void Force_NordicFlash(void)
{
	if(SUSPEND_NORDIC_USE_FLASH)
		return;
	if(Flash_Set_Access(F_NORDIC))
	{
		SpiFlash_Enable_IO();
		FORCE_NORDIC_FLASH_FLAG =true;
	}
}
void Free_NordicFlash(void)
{
	if(SUSPEND_NORDIC_USE_FLASH)
		return;
	if(Flash_Set_Access(F_FREESCAL))
	{
		SpiFlash_Disable_IO();
		FORCE_NORDIC_FLASH_FLAG =false;
	}	
}

bool SetFlash_Nordic(void)
{
	if(FORCE_NORDIC_FLASH_FLAG)
		return true;
	if(Flash_Set_Access(F_NORDIC))
	{
		SpiFlash_Enable_IO();
		return true;
	}
	return false;
}

bool SetFlash_Freescal(void)
{
	if(FORCE_NORDIC_FLASH_FLAG)
		return true;
	if(Flash_Set_Access(F_FREESCAL))
	{
		SpiFlash_Disable_IO();
		return true;
	}
	return false;
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

void WaitForStatusRegister(uint8 status)
{
	uint8 FLASH_Status = 0;
	uint32 ECount=0;
	Select_Flash_CS();
	SPI_Write(E_RDSR); 
	FLASH_DO_IN;
	do
	{
		FLASH_Status =  SPI_Read();
		nrf_delay_us(10); 
		ECount++;
	}	
	while ((FLASH_Status & status)&&(ECount<300000)); 
	FLASH_DO_OUT;
	Release_Flash_CS();
}


uint32 SpiFlash_ReadJedecID(void)
{
	uint32 DeviceID=0;
	Power_Resume();
	Select_Flash_CS();
	SPI_Write(E_JEDEC);    
	FLASH_DO_IN;	
	u_Delay(5);
	DeviceID |= ((uint32)SPI_Read() << 16);
	DeviceID |= ((uint16)SPI_Read() << 8);
	DeviceID |= SPI_Read();
	FLASH_DO_OUT;
	Release_Flash_CS();
	Power_Suspend();
	return DeviceID;
}

uint8 SpiFlash_GetBusyBit(void)
{
	uint8 FLASH_Status = 0;
	Power_Resume();
	Select_Flash_CS();
	SPI_Write(E_RDSR); 
	FLASH_DO_IN;
	FLASH_Status =  SPI_Read();
	FLASH_DO_OUT;
	Release_Flash_CS();
	Power_Suspend();
	return (FLASH_Status & SPI_FLASH_BUSY);
}

void SpiFlash_4KSector_Erase(uint32 EraseAddr)
{
	if(SetFlash_Nordic())
	{
		Power_Resume();
		WriteEnable();
		Select_Flash_CS();
	    	SPI_Write(E_SE);
		SPI_Write((uint8)(EraseAddr >> 16));
		SPI_Write((uint8)(EraseAddr>> 8));
		SPI_Write((uint8)(EraseAddr));	
		Release_Flash_CS();	
		WaitForStatusRegister(SPI_FLASH_BUSY);
		Power_Suspend();
		SetFlash_Freescal();
	}
	else
	{
		Flash_Erase(EraseAddr,F_SECTOR);
	}

}

void SpiFlash_64KBlock_Erase(uint32 EraseAddr)
{
	if(SetFlash_Nordic())
	{
		Power_Resume();
		WriteEnable();
		Select_Flash_CS();
	    	SPI_Write(E_BE);
		SPI_Write((uint8)(EraseAddr >> 16));
		SPI_Write((uint8)(EraseAddr>> 8));
		SPI_Write((uint8)(EraseAddr));	
		Release_Flash_CS();
		WaitForStatusRegister(SPI_FLASH_BUSY);
		Power_Suspend();
		SetFlash_Freescal();
	}
	else
	{
		Flash_Erase(EraseAddr,F_BLOCK);
	}
}  

void SpiFlash_ChipErase(void)
{
	if(SetFlash_Nordic())
	{
		Power_Resume();
		WriteEnable();
		Select_Flash_CS();
	       SPI_Write(E_CE);
		Release_Flash_CS();		
		WaitForStatusRegister(SPI_FLASH_BUSY);
		Power_Suspend();
		SetFlash_Freescal();
	}
	else
	{
		Flash_Erase(0x00000000UL,F_CHIP);
	}

}

void SpiFlash_PageWrite(uint32 addr,uint8* pBuf,uint16 len)
{  
	WriteEnable();
	Select_Flash_CS();
	SPI_Write(E_PP);//ҳдָ��
	SPI_Write((uint8)(addr >> 16));
	SPI_Write((uint8)(addr>> 8));
	SPI_Write((uint8)(addr));
	while(len--)
	{
		SPI_Write(*pBuf);
		pBuf++;
	}
	Release_Flash_CS();	
	WaitForStatusRegister(SPI_FLASH_BUSY);
}

void SpiFlash_Write(uint32 addr,uint8*pBuf, uint16 len)
{
	if(SetFlash_Nordic())
	{
		uint8 NumOfPage = 0, NumOfSingle = 0, Addr = 0, count = 0, temp = 0;
		Addr = addr%EFLASH_PAGE_SIZE;					/*	ȷ����ַ�ڵ�ҳ��λ��*/
		count = EFLASH_PAGE_SIZE - Addr;					/*	ȷ����ַ�ڵ�ҳʣ���д�ֽ���*/
		NumOfPage =  len/EFLASH_PAGE_SIZE;				/*	ȷ��Ҫд��������Ҫ������ҳ*/
		NumOfSingle = len%EFLASH_PAGE_SIZE;				/*	ȷ��Ҫд�����ݳ�ȥ��ҳ��ʣ���ֽ���*/
		Power_Resume();
		if (Addr == 0) 									/* д��ĵ�ַ��ҳ����*/
		{
			if (NumOfPage == 0)							/* д��������С��һҳ*/
				SpiFlash_PageWrite(addr, pBuf, len);
			else 										/* д������������һҳ*/
			{
				while (NumOfPage--)						/* д��ҳ��*/
				{
					SpiFlash_PageWrite(addr,pBuf, EFLASH_PAGE_SIZE);
					addr +=  EFLASH_PAGE_SIZE;
					pBuf += EFLASH_PAGE_SIZE;
				}
				SpiFlash_PageWrite(addr, pBuf,NumOfSingle);  	/* дʣ���*/
			}
		}
		else 											/* д��ĵ�ַ����ҳ����*/
		{
			if (NumOfPage == 0) 							/* д��������С��һҳ*/
			{
				if (NumOfSingle > count)					/* д�����������ڱ�ҳʣ���ֽ���*/
				{ 				
					temp = NumOfSingle - count;
					SpiFlash_PageWrite(addr,pBuf, count);		/* ��д�걾ҳʣ��*/
					addr +=  count;
					pBuf += count;
					SpiFlash_PageWrite(addr, pBuf, temp);		/* ʣ�µ�д����һҳ*/
				}
				else
					SpiFlash_PageWrite(addr, pBuf,len);	 	/* ��ҳ����װ��*/
			}
			else 										/* д������������һҳ*/
			{
				len -= count;
				NumOfPage =  len / EFLASH_PAGE_SIZE;
				NumOfSingle = len % EFLASH_PAGE_SIZE;
				SpiFlash_PageWrite(addr, pBuf, count);		/* ��д�걾ҳʣ�࣬����Ͱ�ҳ�������*/	
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
		SetFlash_Freescal();
	}
	else
	{
		Flash_Write(addr,pBuf,len);
	}
}



void SpiFlash_Read( uint32 addr,uint8*pBuf , uint16 len)
{
	if(SetFlash_Nordic())
	{
		Power_Resume();
		Select_Flash_CS();
		SPI_Write(E_READ);//д��ȡ����ָ��    
		SPI_Write((uint8)(addr >> 16));
		SPI_Write((uint8)(addr>> 8));
		SPI_Write((uint8)(addr));
		FLASH_DO_IN;
		while (len--) 
		{
			*pBuf = SPI_Read();
			pBuf++;
		}
		FLASH_DO_OUT;
		Release_Flash_CS(); 
		Power_Suspend();
		SetFlash_Freescal();
	}
	else
	{
		Flash_Read(addr,pBuf,len);
	}

}

void SpiFlash_Read_TP_Firmware( uint32 addr,uint8*pBuf , uint16 len)
{
	if(SetFlash_Nordic())
	{
	
		Select_Flash_CS();
		SPI_Write(E_READ);//д��ȡ����ָ��    
		SPI_Write((uint8)(addr >> 16));
		SPI_Write((uint8)(addr>> 8));
		SPI_Write((uint8)(addr));
		FLASH_DO_IN;
		while (len--) 
		{
			*pBuf = SPI_Read();
			pBuf++;
		}
		FLASH_DO_OUT;
		Release_Flash_CS(); 
	
		SetFlash_Freescal();
	}
	else
	{
		Flash_Read(addr,pBuf,len);
	}
}



void SpiFlash_Init(void)
{
	SpiFlash_Enable_IO();
	FORCE_NORDIC_FLASH_FLAG =true;
	UnlockGlobalBlockProtection();
}



