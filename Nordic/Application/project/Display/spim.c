#include "headfile.h"


#define TIMEOUT_COUNTER          0x3000UL

void HwSpi1IoPortInit(void)
{
/*
    NRF_SPI0->ENABLE = 0;
    NRF_SPI0->PSELSCK = LCD_SPI_SCL;
    //NRF_SPI0->PSELMISO = SPIM1_MISO_PIN;
    NRF_SPI0->PSELMOSI = LCD_SPI_SI;
    NRF_SPI0->FREQUENCY = (uint32_t) 0x80000000;
    NRF_SPI0->CONFIG = 0x0;
    NRF_SPI0->EVENTS_READY = 0;
    NRF_SPI0->ENABLE = (SPI_ENABLE_ENABLE_Enabled << SPI_ENABLE_ENABLE_Pos);

    nrf_gpio_cfg_output(LCD_SPI_SI);
    nrf_gpio_cfg_output(LCD_SPI_SCL);
    nrf_delay_us(5);
*/
}

void HwSpi1IoPortUnInit(void)
{
/*
    NRF_SPI0->ENABLE = 0;

    nrf_gpio_cfg_input(LCD_SPI_SI, NRF_GPIO_PIN_NOPULL);
    nrf_gpio_cfg_input(LCD_SPI_SCL, NRF_GPIO_PIN_NOPULL);
    //nrf_gpio_cfg_input(SPIM1_MISO_PIN, NRF_GPIO_PIN_NOPULL);

    //nrf_gpio_cfg_input(SPIM0_CS_PIN, NRF_GPIO_PIN_NOPULL);
    nrf_gpio_cfg_input(LCD_SPI_CS, NRF_GPIO_PIN_NOPULL);
    nrf_delay_us(5);
 */   
}

void hw_SpiWriteByte(uint8_t data)
{
	volatile uint8_t dummy = 0;
	
	NRF_SPI0->TXD = data;
	while(NRF_SPI0->EVENTS_READY != 1);
	NRF_SPI0->EVENTS_READY = 0U;
	dummy = NRF_SPI0->RXD;
}

void SpiMultipleByteWrite(uint8_t *pData, uint16_t nByte)
{
    uint16_t i = 0;
	volatile uint8_t dummy = 0;
	
    for(; i < nByte; i++)
    {
		NRF_SPI0->TXD = *pData++;
		while(NRF_SPI0->EVENTS_READY != 1);
		NRF_SPI0->EVENTS_READY = 0U;
		dummy = NRF_SPI0->RXD;
    }

}

void SpiMultipleByteWriteZero(uint8_t Data, uint16_t nByte)
{
    uint16_t i = 0;
	volatile uint8_t dummy = 0;
	
    for(; i < nByte; i++)
    {
		NRF_SPI0->TXD = Data;
		while(NRF_SPI0->EVENTS_READY != 1);
		NRF_SPI0->EVENTS_READY = 0U;
		dummy = NRF_SPI0->RXD;
    }

}



