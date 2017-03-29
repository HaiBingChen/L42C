#include <string.h>
 
#include "headfile.h"


#define RESET_PIN				GPIO_MAKE_PIN(HW_GPIOB, 2)
#define BOOT_PIN				GPIO_MAKE_PIN(HW_GPIOB, 3)



void sys_enter_bootloader_mode(void)
{
    //GPIO_DRV_SetPinOutput(BOOT_PIN);
}

void sys_enter_app_mode(void)
{
    //GPIO_DRV_ClearPinOutput(BOOT_PIN);
}

void sys_reset_device(void)
{
	KL17_INT_CONFIGURE(KL17_RESET_PIN);
	KL17_INT_LOW(KL17_RESET_PIN);
	nrf_delay_ms(10);
	KL17_INT_HIGH(KL17_RESET_PIN);
	nrf_delay_ms(10);
}

void sys_delay_in_ms(uint32_t ms)
{
	ms = ms + 1;
	nrf_delay_ms(ms);
}

void *sys_malloc(uint32_t size)
{
	return malloc(size);
}

void sys_free(void *p)
{
	free(p);
}


#if (CONFIG_LINKLAYER == I2C)

#include "fsl_i2c_master_driver.h"

#define I2C_INSTANCE					1

static i2c_master_state_t i2c_master_state;
static const i2c_device_t i2c_device = 
{
	.address	   = 0x10, /* 7-bit address, 0x20 for w, 0x21 for r */
	.baudRate_kbps = 400,
};

uint32_t i2c_send_blocking(const uint8_t *p_send_buffer, uint32_t len)
{
    uint32_t status;
    
    if (I2C_DRV_MasterSendDataBlocking(I2C_INSTANCE,
                                       &i2c_device,
                                       NULL,
                                       0,
                                       (uint8_t *)p_send_buffer,
                                       len,
                                       OSA_WAIT_FOREVER) == kStatus_I2C_Success)
    {
        status = LINK_COMM_SUCCESS;
    }
    else
    {
        status = LINK_COMM_FAIL;
    }
    
    return status;
}

uint32_t i2c_recv_blocking(uint8_t *p_recv_buffer, uint32_t len, uint32_t timeout)
{
    uint32_t status;
	(void)timeout;
    
    if (I2C_DRV_MasterReceiveDataBlocking(I2C_INSTANCE,
                                          &i2c_device,
                                          NULL,
                                          0,
                                          p_recv_buffer,
                                          len,
                                          OSA_WAIT_FOREVER) == kStatus_I2C_Success)
    {
        status = LINK_COMM_SUCCESS;
    }
    else
    {
        status = LINK_COMM_FAIL;
    }
    
    return status;
}

#elif (CONFIG_LINKLAYER == SPI)

#include "fsl_dspi_master_driver.h"

#define SPI_INSTANCE			0

static dspi_master_state_t spi_master_state;
static const dspi_device_t spi_device = 
{
	.bitsPerSec 				= 500 * 1000,
	.dataBusConfig.bitsPerFrame = 8,
	.dataBusConfig.clkPolarity  = kDspiClockPolarity_ActiveHigh,
	.dataBusConfig.clkPhase     = kDspiClockPhase_FirstEdge,
	.dataBusConfig.direction    = kDspiMsbFirst,
};

uint32_t spi_send_blocking(const uint8_t *p_send_buffer, uint32_t len)
{
    uint32_t status;
    
    if (DSPI_DRV_MasterTransferDataBlocking(SPI_INSTANCE,
                                       		&spi_device,
                                       		p_send_buffer,
                                       		NULL,	/* ignore incoming data */
                                       		len,
                                       		OSA_WAIT_FOREVER) == kStatus_DSPI_Success)
    {
        status = LINK_COMM_SUCCESS;
    }
    else
    {
        status = LINK_COMM_FAIL;
    }
    
    return status;
}

uint32_t spi_recv_blocking(uint8_t *p_recv_buffer, uint32_t len, uint32_t timeout)
{
    uint32_t status;
	(void)timeout;
    
    if (DSPI_DRV_MasterTransferDataBlocking(SPI_INSTANCE,
                                            &spi_device,
                                            NULL,	/* send 0x00 */
                                            p_recv_buffer,
                                            len,
                                            OSA_WAIT_FOREVER) == kStatus_DSPI_Success)
    {
        status = LINK_COMM_SUCCESS;
    }
    else
    {
        status = LINK_COMM_FAIL;
    }
    
    return status;
}

#elif (CONFIG_LINKLAYER == UART)


static uint32_t UART_DRV_SendDataBlocking(const uint8_t *p_buffer, uint32_t len)
{
    for (int i = 0; i < len; i++)
    {
        NRF_UART0->TXD = p_buffer[i];

        while (NRF_UART0->EVENTS_TXDRDY != 1)
        {
            /* do nothing */
        }
        NRF_UART0->EVENTS_TXDRDY = 0;
    }

	return NRF_SUCCESS;
}

uint32_t uart_send_blocking(const uint8_t *p_send_buffer, uint32_t len)
{
    uint32_t status;
    
    if (UART_DRV_SendDataBlocking(p_send_buffer, len) == NRF_SUCCESS)
    {
        status = LINK_COMM_SUCCESS;
    }
    else
    {
        status = LINK_COMM_FAIL;
    }
    
    return status;
}

static __INLINE uint32_t fifo_length(app_fifo_t * p_fifo)
{
    uint32_t tmp = p_fifo->read_pos;
    return p_fifo->write_pos - tmp;
}

static uint32_t UART_DRV_ReceiveDataBlocking(uint8_t *p_buffer, uint32_t len, uint32_t timeout)
{
	uint32_t i = 0;
	uint32_t status;
	uint32_t length = 0;

	while(timeout)
	{
		length = fifo_length(&app_fifo);
		if (length >= len) {
			break;
		}
		else {
			nrf_delay_ms(1);
			timeout--;
		}
	}

	if (timeout == 0) {		//timeout
		return NRF_ERROR_TIMEOUT;
	}

	for(i=0; i<len; i++)
	{
		status = app_fifo_get(&app_fifo, &p_buffer[i]);
		if (status != NRF_SUCCESS) {
			return status;
		}
	}
	return status;
}


uint32_t uart_recv_blocking(uint8_t *p_recv_buffer, uint32_t len, uint32_t timeout)
{
	uint32_t status;

	if (UART_DRV_ReceiveDataBlocking(p_recv_buffer,
									 len,
									 timeout) == NRF_SUCCESS)
	{
		status = LINK_COMM_SUCCESS;
	}
	else
	{
		status = LINK_COMM_FAIL;
	}
	
	return status;
}

#endif

void bootloader_init(void)
{
	Update_flags = true;
	BootloaderUartInitial();
	app_fifo_init(&app_fifo, DataRevBuf, 64);
}

