
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
#include "string.h"
#include "protocol.h" 
#include "spi_slave.h"
#include "app_error.h"
#include "bsp.h"
#include "stdio.h"
#include "app_uart_fifo.h"
#include "nrf_delay.h"
#include "headfile.h"
  
#define RX_BUF_SIZE   TX_BUF_SIZE       /**< SPI RX buffer size. */      
#define DEF_CHARACTER 0xAAu             /**< SPI default character. Character clocked out in case of an ignored transaction. */      
#define ORC_CHARACTER 0x55u             /**< SPI over-read character. Character clocked out after an over-read of the transmit buffer. */      

uint8_t m_tx_buf[SPI_BLOCK_SIZE];   /**< SPI TX buffer. */      
uint8_t m_rx_buf[SPI_BLOCK_SIZE];   /**< SPI RX buffer. */          
uint8_t KL17_VER[2]={0,0};
uint8_t HR_VER[2]={0,0};

uint8_t nfc_status = NFC_STATUS_FIELD_IDLE;
uint8_t slot_status = SLOT_STATUS_IDLE;
static uint32 spi_timeout_value = DEFAULT_TIMEOUT_VALUE*10;		//800ms

uint16_t m_rx_len = CMD_LEN_8BYTE;


spi_buffer_t spi_buffer;
spi_ctx_t m_spi_ctx;


typedef struct
{
	uint8_t cmd;
	uint16_t len;
	uint8_t * buf;
} spi_cmd_t;

spi_cmd_t spi_cmd;

char NFC_ACTIVE_FLAG = 1;
char NFC_FIELD_FLAG = 0;
char DATA_TRAN_FLAG = 0;
char check_state = 0;
#define SPI_RX_DONE		1
#define SPI_TX_DONE		2


static bool WaitForTimeout(uint8_t * pattern, const uint8_t value, uint32_t timeout)
{
	int i=0;
	
	while(*pattern != value)
	{
		nrf_delay_us(100);	//Timeout
		if (i++ == timeout)
		{
			return false;
		}
	}
	return true;
}

/**@brief Function for initializing buffers.
 *
 * @param[in] p_tx_buf  Pointer to a transmit buffer.
 * @param[in] p_rx_buf  Pointer to a receive  buffer.
 * @param[in] len       Buffers length.
 */
 #define SPI_TX_PATTERN		0x7F
static __INLINE void spi_slave_buffers_init(uint8_t * const buf, const uint16_t len)
{
    uint16_t i=0;
	
    for (i = 0; i < len; i++)
    {
        buf[i] = 0x7F;
    }
}

/**@brief Function for checking if received data is valid.
 *
 * @param[in] p_rx_buf  Pointer to a receive  buffer.
 * @param[in] len       Buffers length.
 *
 * @retval true     Buffer contains expected data.
 * @retval false    Data in buffer are different than expected.
 */
 #define CHECK_PATTERN 0x5A
static bool __INLINE spi_slave_buffer_check(uint8_t * const p_rx_buf, const uint16_t len)
{
	uint32_t i=0;
	
	for(i=0; i<len; i++)
	{
		if (p_rx_buf[i] != CHECK_PATTERN)
		{
			return false;
		}
	}
	
    return true;
}

/**@brief Function for SPI slave event callback.
 *
 * Upon receiving an SPI transaction complete event, LED1 will blink and the buffers will be set.
 *
 * @param[in] event SPI slave driver event.
 */
static void spi_slave_event_handle(spi_slave_evt_t event)
{
    uint32_t err_code;
	bool status = false;

    if (event.evt_type == SPI_SLAVE_XFER_DONE)
    {
		status = spi_slave_buffer_check(m_rx_buf, SPI_BLOCK_SIZE);
		if (status)	
		{
			//All Data is 0x5A pattern, Just for Check sending data done.
			m_spi_ctx.pattern_check = 1;
		} else {
			//Received valid data
			spi_receive_package();
		}
		err_code = spi_slave_buffers_set(m_tx_buf, m_rx_buf, SPI_BLOCK_SIZE,SPI_BLOCK_SIZE);
		APP_ERROR_CHECK(err_code); 
    }
}



/**@brief Function for initializing SPI slave.
 *
 *  Function configures a SPI slave and sets buffers.
 *
 * @retval NRF_SUCCESS  Initialization successful.
 */
uint32_t spi_init(void)
{
    uint32_t           err_code;
    spi_slave_config_t spi_slave_config;
        
    err_code = spi_slave_evt_handler_register(spi_slave_event_handle);
    APP_ERROR_CHECK(err_code);    

    spi_slave_config.pin_miso         = SPI_KL17_MISO_PIN;
    spi_slave_config.pin_mosi         = SPI_KL17_MOSI_PIN;
    spi_slave_config.pin_sck          = SPI_KL17_SCK_PIN;
    spi_slave_config.pin_csn          = SPI_KL17_CS_PIN;
    spi_slave_config.mode             = SPI_MODE_0;
    spi_slave_config.bit_order        = SPIM_MSB_FIRST;
    spi_slave_config.def_tx_character = DEF_CHARACTER;
    spi_slave_config.orc_tx_character = ORC_CHARACTER;
    
    err_code = spi_slave_init(&spi_slave_config);
    APP_ERROR_CHECK(err_code);
    
    //Initialize buffers.
    spi_slave_buffers_init(m_tx_buf, (uint16_t)SPI_BLOCK_SIZE);
    spi_slave_buffers_init(m_rx_buf, (uint16_t)SPI_BLOCK_SIZE);
	
    //Set buffers.
	m_rx_len = SPI_BLOCK_SIZE;
    err_code = spi_slave_buffers_set(m_tx_buf, m_rx_buf, SPI_BLOCK_SIZE, SPI_BLOCK_SIZE);
    APP_ERROR_CHECK(err_code);
	
    return NRF_SUCCESS;
}



int spi_send_data(uint8_t * p_tx_buf, uint32_t tx_len, uint32_t rx_len)
{
	int i=0;
	uint32_t	err_code;
	
	//Set INT as LOW level
	KL17_INT_LOW(KL17_INT_PIN);
	
	 if ((p_tx_buf == NULL) || (tx_len == 0) || rx_len  == 0)
    {
        return -1;
    }
	
	for(i=0; i<tx_len; i++)
	{
		//m_tx_buf[i] = p_tx_buf[i];
	}
	
	for(i=0; i<rx_len; i++)
	{
		m_rx_buf[i] = 0;
	}
	
	//Set buffers.
	m_rx_len = rx_len;
    err_code = spi_slave_buffers_set(m_tx_buf, m_rx_buf, SPI_BLOCK_SIZE, SPI_BLOCK_SIZE);
    APP_ERROR_CHECK(err_code);   
	
	//Set INT as HIGH level
	KL17_INT_HIGH(KL17_INT_PIN);
	return 0;
}


static bool spi_send_data_enable(void)
{	
	bool status = false;
	KL17_INT_LOW(KL17_INT_PIN);
	nrf_delay_ms(1);
	KL17_INT_HIGH(KL17_INT_PIN);

	status = WaitForTimeout(&m_spi_ctx.pattern_check, 1, 1000);	//10ms timeout
	m_spi_ctx.pattern_check = 0;	//clear pattern
	return status;
}

 void spi_init_package(void) 
{

	spi_buffer.buf_notification_write_idx = 0;
	spi_buffer.buf_notification_size = 0;
	spi_buffer.buf_command_response_read_idx = 0;
	spi_buffer.buf_command_response_write_idx = 0;
	spi_buffer.buf_command_response_size = 0;
	spi_buffer.spi_packet_to_receive = 0;
	
	m_spi_ctx.first_cmd = 1;
}

static void notify_event_message(void)
{
	uint8_t event = 0;
	
	if (((spi_command_t*) spi_buffer.buf_command_response)->header.tags == 0xFF) {
		if (((spi_command_t*) spi_buffer.buf_command_response)->header.type == SE_NOTIFY_EVENT) {
			if (((spi_command_t*) spi_buffer.buf_command_response)->header.len ==  1) {
				memcpy(&event,&((spi_command_t*) spi_buffer.buf_command_response)->payload[0], 1);
				if (event == NFC_NOTIFY_EVENT_IN) {
					nfc_status = NFC_STATUS_FIELD_IN;
					m_spi_ctx.rx_done = 0;
				}
				else if (event == NFC_NOTIFY_EVENT_OUT) {
					nfc_status = NFC_STATUS_FIELD_OUT;
					#if EMV_TEST
					slot_status = SLOT_STATUS_BUSY;
					#endif
					m_spi_ctx.rx_done = 0;
					#if EMV_TEST
					SendMsg(MSG_NFC_ACTION_CLOSE);
					#endif
				}
			}
		}
	}	
}

static void spi_receive_package(void)
{
	uint8_t quotients = 0;
	uint8_t remainders = 0;


	memcpy(&spi_buffer.buf_command_response[(spi_buffer.buf_command_response_read_idx)*SPI_BLOCK_SIZE], m_rx_buf, SPI_BLOCK_SIZE);
	if (spi_buffer.buf_command_response_read_idx == 0)
	{
		if (((spi_command_t*) spi_buffer.buf_command_response)->header.len <= 16)		//always one package
		{
			m_spi_ctx.last_num = 0;
		} 
		else {	// more than one package
			quotients = (((spi_command_t*) spi_buffer.buf_command_response)->header.len - 16)/20;	//quotients
			remainders = (((spi_command_t*) spi_buffer.buf_command_response)->header.len - 16)%20;	//remainders
			if (remainders)
			{
				m_spi_ctx.last_num = quotients + 1;
			}
			else {
				m_spi_ctx.last_num = quotients;
			}
		}
	}

	if (spi_buffer.buf_command_response_read_idx == m_spi_ctx.last_num)		//finished all package
	{
		spi_buffer.buf_command_response_read_idx = 0;
		m_spi_ctx.last_num = 0;
		m_spi_ctx.rx_done = 1;
		notify_event_message();

	} else {
		spi_buffer.buf_command_response_read_idx++;		//continue receiving package
	}
	
}

bool spi_send_package(uint8_t * p_buf, uint16_t offset, uint16_t len)
{
	uint8_t quotients = 0;
	uint8_t remainders = 0;
	bool status = false;

	memcpy(m_tx_buf, &p_buf[spi_buffer.buf_command_response_write_idx* SPI_BLOCK_SIZE], SPI_BLOCK_SIZE);
	
	if (spi_buffer.buf_command_response_write_idx == 0)
	{
		if (len <= 20)		//always send one package
		{
			m_spi_ctx.last_num = 0;
		}
		else {		//more than one package
			quotients = (len - 20)/20;
			remainders = (len -20)%20;
			if (remainders)
			{
				m_spi_ctx.last_num = quotients + 1;
			} 
			else {
				m_spi_ctx.last_num = quotients;
			}
			
		}
	}
	
	if (spi_buffer.buf_command_response_write_idx == m_spi_ctx.last_num)
	{
		spi_buffer.buf_command_response_write_idx = 0;
		m_spi_ctx.last_num = 0;
	}
	else {
		spi_buffer.buf_command_response_write_idx++;
	}
	status = spi_send_data_enable();
	//nrf_delay_ms(10);
       spi_slave_buffers_init(m_tx_buf, (uint16_t)SPI_BLOCK_SIZE);		//init buffer after send data done	
	return status;
}

uint8_t spi_send_cmd(uint8_t type, uint8_t * buf,  uint16_t  len )
{
	bool status = false;

	KL17_Power_Resume();

	spi_init_package();	//TBD
	
	((spi_command_t*) spi_buffer.buf_command_response)->header.tags = 0xFF;
	((spi_command_t*) spi_buffer.buf_command_response)->header.type = type;
	((spi_command_t*) spi_buffer.buf_command_response)->header.len = len;

	memcpy(&((spi_command_t*) spi_buffer.buf_command_response)->payload[0], buf, len );

	do {
		status = spi_send_package(&spi_buffer.buf_command_response[0], 0, (len+4));
		if (!status)
		{
			spi_buffer.buf_command_response_write_idx = 0;
			KL17_Power_Suspend();
			return SPI_FAILED;
		}
	}while(spi_buffer.buf_command_response_write_idx);

	KL17_Power_Suspend();
	return SPI_SUCCED;
}

uint8_t spi_receive_data(uint8_t * buf,  uint8_t * len )
{

	bool status = false;
	uint16_t length = 0;


	uint8_t spi_packet_to_receive = 1;
	
	length = ((spi_command_t*) spi_buffer.buf_command_response)->header.len;
	while(length > SPI_BLOCK_SIZE) {
		spi_packet_to_receive++;
		length -= SPI_BLOCK_SIZE;
	}

	//1200000
	status = WaitForTimeout(&m_spi_ctx.rx_done, 1, spi_timeout_value);		//800ms
	
	if (!status)
	{
		return SPI_RET_TIMEOUT;
	}
	
	m_spi_ctx.rx_done = 0;		//clear flags



	if ((((spi_command_t*) spi_buffer.buf_command_response)->header.tags != 0xFF))
	{
		return SPI_RET_INV_CMD;
	}

	if (((spi_command_t*) spi_buffer.buf_command_response)->header.len == 0)
	{
		return SPI_RET_INV_LEN;
	}

	if(buf!=NULL)
	{
		memcpy(buf,&((spi_command_t*) spi_buffer.buf_command_response)->payload[1], (((spi_command_t*) spi_buffer.buf_command_response)->header.len-1));
		if(len!=NULL)
		{
			((spi_command_t*) spi_buffer.buf_command_response)->header.len -= 1;
			memcpy(len, (uint8_t *)&((spi_command_t*) spi_buffer.buf_command_response)->header.len, 2);	
		}
	}	
	return ((spi_command_t*) spi_buffer.buf_command_response)->payload[0];
}


void NFC_Active_Disable(void)
{
	uint32_t	err_code;
	
	m_rx_len =20;
    err_code = spi_slave_buffers_set(m_tx_buf, m_rx_buf, 0, m_rx_len);
	APP_ERROR_CHECK(err_code); 
	NFC_ACTIVE_FLAG = 0;
}


void NFC_Active_Enable(void)
{
	uint32_t	err_code;
	
	m_rx_len =20;
    err_code = spi_slave_buffers_set(m_tx_buf, m_rx_buf, 0, m_rx_len);
	APP_ERROR_CHECK(err_code); 
	NFC_ACTIVE_FLAG = 1;
}

void KL17_Power_Enble(void)
{
	KL17_INT_CONFIGURE(KL17_INT_PIN);
	KL17_INT_HIGH(KL17_INT_PIN);
}


void KL17_Power_Disable(void)
{
      nrf_gpio_pin_dir_set(KL17_INT_PIN, NRF_GPIO_PIN_DIR_INPUT);
}


void KL17_Power_Suspend(void)
{
        nrf_gpio_cfg_sense_input(KL17_INT_PIN, NRF_GPIO_PIN_NOPULL, NRF_GPIO_PIN_SENSE_LOW);
}


void KL17_Power_Resume(void)
{
        KL17_INT_CONFIGURE(KL17_INT_PIN);
        KL17_INT_HIGH(KL17_INT_PIN);
}


void OtaSendCmd(OtaCmd cmd,uint8_t data)
{
	uint8_t Sdata[5];
	Sdata[0]=0xff;
	Sdata[1]=(uint8_t)cmd;
	Sdata[2]=data;
	Sdata[3]=0x0a;
	Sdata[4]=0x0d;
	UART_Send(Sdata,5);
}

void ChangeTimeoutValue(uint32_t mSecond)
{
	if (mSecond > 0)
		spi_timeout_value = mSecond*10;
}

