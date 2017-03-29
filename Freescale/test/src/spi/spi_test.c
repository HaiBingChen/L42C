#include "spi_test.h"
#include "timer.h"

void delay(uint32_t delay_time)
{
    uint32_t wait;
    for(wait=0;wait<delay_time;wait++)
    {
        __asm("NOP");
    }
}

void test_spi(void)
{
	uint8_t  tx_buffer[16]={0};
	uint8_t  rx_buffer[16]={0};
	SPI_Type * spiBaseAddr = g_spiBase[SPI_MASTER_INSTANCE];
	int len = 7;
	
	tx_buffer[0] = 0xFF;
	tx_buffer[1] = 0x02;
	tx_buffer[2] = 0x02;
	tx_buffer[3] = 0x02;
	tx_buffer[4] = 0x00;
	tx_buffer[5] = 0x0A;
	tx_buffer[6] = 0x0D;
	
	//SPI_Init(spiBaseAddr);

	
	while(1)
	{
		while(!SPI_INT_FLAG);
		SPI_INT_FLAG = 0;
		PRINTF("read data:%d\r\n", len);
		SPI_ReadData(spiBaseAddr, rx_buffer, len);
		len = rx_buffer[4];
		delay(30000);
		PRINTF("write data\r\n");
		SPI_WriteData(spiBaseAddr, tx_buffer, tx_buffer, 7);
		
		for(int i = 0; i< 16; i++)
		{
			PRINTF("BUFFER[%d]:0x%x\r\n", i,rx_buffer[i] );
		}
		
		/////////////////////////////////////////////
		while(!SPI_INT_FLAG);
		SPI_INT_FLAG = 0;
		PRINTF("read data:%d\r\n", len);
		SPI_ReadData(spiBaseAddr, rx_buffer, len);
		len = 7;
		delay(30000);
		PRINTF("write data\r\n");
		SPI_WriteData(spiBaseAddr, tx_buffer, tx_buffer, 7);
		
		for(int i = 0; i< 16; i++)
		{
			PRINTF("BUFFER[%d]:0x%x\r\n", i,rx_buffer[i] );
		}
	}
}
