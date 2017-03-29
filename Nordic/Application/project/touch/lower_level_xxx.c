
//****************************************************************************
// 头文件
//****************************************************************************/
#include "headfile.h"




#define RDY_PIN                         13	

#define I2C_RDY_DIR_INPUT           nrf_gpio_pin_dir_set(RDY_PIN, NRF_GPIO_PIN_DIR_INPUT)
#define I2C_RDY_DIR_OUTPUT        nrf_gpio_pin_dir_set(RDY_PIN, NRF_GPIO_PIN_DIR_OUTPUT)
#define I2C_RDY_IN                          nrf_gpio_pin_read(RDY_PIN)
#define I2C_RDY_HI                          nrf_gpio_pin_set(RDY_PIN)
#define I2C_RDY_LO                         nrf_gpio_pin_clear(RDY_PIN)

	
#define I2C_SDA_DIR_INPUT           nrf_gpio_pin_dir_set( SDA_PIN, NRF_GPIO_PIN_DIR_INPUT)
#define I2C_SCL_DIR_INPUT           nrf_gpio_pin_dir_set( SCL_PIN, NRF_GPIO_PIN_DIR_INPUT)
#define I2C_SDA_DIR_OUTPUT       nrf_gpio_pin_dir_set( SDA_PIN, NRF_GPIO_PIN_DIR_OUTPUT)
#define I2C_SCL_DIR_OUTPUT        nrf_gpio_pin_dir_set( SCL_PIN, NRF_GPIO_PIN_DIR_OUTPUT)
#define I2C_SDA_IN                          nrf_gpio_pin_read(SDA_PIN)
#define I2C_SCL_IN                          nrf_gpio_pin_read(SCL_PIN)
#define I2C_SDA_LO                         nrf_gpio_pin_clear(SDA_PIN)
#define I2C_SCL_LO                         nrf_gpio_pin_clear(SCL_PIN)
#define I2C_SDA_HI                          nrf_gpio_pin_set(SDA_PIN)
#define I2C_SCL_HI                          nrf_gpio_pin_set(SCL_PIN)




#define MCLK    48

void DELAY_US (unsigned long cycles)
{ 
	while(cycles>15)													// 15 cycles consumed by overhead
		cycles = cycles - 4;										// 6 cycles consumed each iteration
}

void DELAY_MS (unsigned long cycles)
{ 
	uint32_t cnt;
	while(cycles)
	{
		cycles = cycles - 1;
		cnt = MCLK;
		while(cnt)
		{
			DELAY_US(1000);
			cnt--;
		}
	}
}


void Poll_I2C_READY_IN(void)
{
        uint8 time=20;
        
        while (I2C_RDY_IN)
        {
                time --;
                if(time < 1)
                        break;
        }
}


void Poll_I2C_SCL_IN(void)
{
        uint8 time=120;
        
        while (!I2C_SCL_IN)
        {
                time --;
                if(time < 1)
                {
                        Comms_init();
                        break;
                }
        }
}


void Poll_I2C_SDA_IN(void)
{
        uint8 time=120;
        
        while (!I2C_SDA_IN)
        {
                time --;
                if(time < 1)
                {
                        Comms_init();
                        break;
                }
        }
}

/*
	Function name:	Comms_init
	Parameters:		none
	Return:			none
	Description:	Initializes the I2C module on the PIC16LF1827
					Note that the SSPADD acts as a counter to determine the I2C frequency. A smaller value will
					increase the frequency.
*/
void Comms_init(void)
{
        I2C_SCL_DIR_OUTPUT;
        I2C_SDA_DIR_OUTPUT;
        I2C_RDY_DIR_OUTPUT;
        delay_2bit;
        delay_2bit;
        
	I2C_SCL_DIR_INPUT;
	I2C_SDA_DIR_INPUT;
	I2C_RDY_DIR_INPUT;
	delay_2bit;
        delay_2bit;
}

/*
	Function name:	CommsIQSxxx_send
	Parameters:		uint8_t send_data - data to be sent transmitted via the I2C
	Return:			ack 1 ok 0 fail
	Description:	Transmits the data byte given as parameter via the I2C of the PIC16LF1827
					Note that the I2C communication channel must already be active before calling this function,
					as no start bits are included in this function
*/
uint8_t CommsIQSxxx_send(uint8_t send_data)
{
	uint8_t  BitCnt;
 uint8_t time1 = 100;
   uint8_t ack;
        I2C_SDA_DIR_OUTPUT;
	for (BitCnt=0; BitCnt<8; BitCnt++)  //要传送的数据长度为8位
	 {
	          
		  if (send_data&0x80)    //判断发送位(从高位起发送)
		  {
				I2C_SDA_HI; 
		 		//Poll_I2C_SDA_IN();  
		  }
		  else
		  {				
				I2C_SDA_LO;//SDA = 0;				
		  }
		  send_data = send_data<<1;
  		  nop();
		  
		  I2C_SCL_DIR_INPUT;//SCLM = 0;
	 	  //Poll_I2C_SCL_IN();     //置时钟线为高通知被控器开始接收数据位
	 		delay_2bit ;//4 us     //保证时钟高电平周期大于 4 us
		  I2C_SCL_DIR_OUTPUT;//SCLM = 1;
		  I2C_SCL_LO;//SCL = 0;
			delay_bit ;//2 us
	}
		 //nop();
		//I2C_SDA_DIR_OUTPUT;// SDAM = 0; 
		// nop();
		 
	/*Check ack*/ 

   I2C_SDA_DIR_INPUT; // sda输入 1128
	 I2C_SCL_DIR_INPUT;//SCLM = 0;             //ack
	 //Poll_I2C_SCL_IN(); 
	  nop();
	 while (I2C_SDA_IN)
	 {
	 	 time1--;
	 	 delay_ack;
	 	 if (time1==0)
	 	 {
	 		 nop();
	 		I2C_SCL_DIR_OUTPUT;//SCLM=1;
	 		I2C_SCL_LO;//SCL=0;
	 		 nop();
	   	ack = 0;
			//I2C_SDA_DIR_OUTPUT;//SDAM = 1; 
		  I2C_SDA_HI;//SDA = 1;
			return ack;
 		 }
	 }
	  //nop();
	 I2C_SCL_DIR_OUTPUT;//SCLM=1;
	 I2C_SCL_LO;//SCL=0;
	 nop();
	 ack=1;

	 return ack;
		 
}

/*
	Function name:	CommsIQSxxx_read_nack
	Parameters:		none
	Return:			uint8_t - the data received via the I2C
	Description:	Enables the Master Receive Mode of the I2C module on the PIC16LF1827. The data received is
					returned and a NACK acknowledge is sent to the IQSxxx to indicate that this was the final read
					of the current continuous read block. 
					A stop or repeated start command has be called next.
*/
uint8_t CommsIQSxxx_read_nack(void)
{
	 uint8_t BitCnt,databuf = 0;     //位
	 
	 I2C_SDA_DIR_INPUT;//SDAM = 0;         //SDA位为输入模式    
          nop();    
	 for(BitCnt=0;BitCnt<8;BitCnt++)
	 {   
		  I2C_SCL_DIR_INPUT;//SCLM = 0;             //SCL为输入模式
	 	  //nop();  
		 	//Poll_I2C_SCL_IN(); 
		 	databuf=databuf<<1;
		  delay_half_bit;  //DELAY_US(MCLK*1);//1us
	  	if(I2C_SDA_IN)
		 	{
		   		databuf=databuf+1;     //读数据位，接收的数据放入cct中
		  }
	  	delay_half_bit;  //DELAY_US(MCLK*1);//1us
			I2C_SCL_DIR_OUTPUT;//SCLM = 1; 
		  I2C_SCL_LO;//SCL = 0;       
		 	delay_half_bit ;  //DELAY_US(MCLK*1);//1us
	  }             
	 
	  
	  I2C_SDA_HI;//SDA = 1;
	  //Poll_I2C_SDA_IN();    
    //nop();
	  nop();
	  I2C_SCL_DIR_INPUT;//SCLM = 0;
	  Poll_I2C_SCL_IN();         //nACK
	 	delay_bit ;//2us
	  I2C_SCL_DIR_OUTPUT;//SCLM = 1; 
	  I2C_SCL_LO;//SCL = 0;
	  nop();
	  //nop();
	  //nop();
	  //nop();
	  
	return databuf;
}

/*
	Function name:	CommsIQSxxx_read_ack
	Parameters:		none
	Return:			uint8_t - the data received via the I2C
	Description:	Enables the Master Receive Mode of the I2C module on the PIC16LF1827. The data received is
					returned and a ACK acknowledge is sent to the IQSxxx to indicate that another read command will follow this one.
*/
uint8_t CommsIQSxxx_read_ack(void)
{
	 uint8_t BitCnt,databuf = 0;     //位
	 
	 I2C_SDA_DIR_INPUT;//SDAM = 0;         //SDA位为输入模式    
         nop();    
	 for(BitCnt=0;BitCnt<8;BitCnt++)
	 {   
		  I2C_SCL_DIR_INPUT;//SCLM = 0;             //SCL为输入模式
		 	//nop();  
		 	//Poll_I2C_SCL_IN(); 
		 	databuf=databuf<<1;
			delay_half_bit ;  //  DELAY_US(MCLK*1);//1us
	  	if(I2C_SDA_IN)
		 	{
		   		databuf=databuf+1;     //读数据位，接收的数据放入cct中
		  }
	  	delay_half_bit;  //DELAY_US(MCLK*1);//1us
			I2C_SCL_DIR_OUTPUT;//SCLM = 1; 
		  I2C_SCL_LO;//SCL = 0;       
		 	delay_half_bit;  //	DELAY_US(MCLK*1);//1us
	  }             
	 
	  I2C_SDA_DIR_OUTPUT;//SDAM = 1;         //SDA位为输入模式
	  I2C_SDA_LO;//SDA = 0;
	  //Poll_I2C_SDA_IN();   
	  nop();
	  //nop();
	  I2C_SCL_DIR_INPUT;//SCLM = 0;
	  Poll_I2C_SCL_IN();         //ACK
	 	delay_bit ;  //	DELAY_US(MCLK*2);//2us
	  I2C_SCL_DIR_OUTPUT;//SCLM = 1; 
	  I2C_SCL_LO;//SCL = 0;
	  nop();
	  //nop();
	  //nop();
	  //nop();
	  
	return databuf;
}


/*
	Function name:		CommsIQSxxx_start
	Parameters:			none
	Return:				none
	Description:		A start condition is created on the I2C.
*/
void CommsIQSxxx_start(void)
{
	        if(SlideReadyPinNoWait)
	            Poll_I2C_READY_IN();
                else
                    while(I2C_RDY_IN); //等待ready 
		I2C_SCL_DIR_INPUT;
		Poll_I2C_SCL_IN(); 

		I2C_SDA_HI;    

		Poll_I2C_SDA_IN();
		delay_half_bit;//4us
	  I2C_SDA_DIR_OUTPUT;
		I2C_SDA_LO;		
		delay_half_bit;//4us
		I2C_SCL_DIR_OUTPUT;
		I2C_SCL_LO;
                delay_half_bit;//4us
}

/*
	Function name:		CommsIQSxxx_repeat_start
	Parameters:			none
	Return:				none
	Description:		A repeated start condition is created on the I2C.
*/
void CommsIQSxxx_repeat_start(void)
{
	        if(SlideReadyPinNoWait)
	            Poll_I2C_READY_IN();
                else
                    while(I2C_RDY_IN); //等待ready 
		I2C_SCL_DIR_INPUT;
		Poll_I2C_SCL_IN(); 
	
		I2C_SDA_HI;

		Poll_I2C_SDA_IN();  
		delay_half_bit;//4us
	  I2C_SDA_DIR_OUTPUT;
		I2C_SDA_LO;		
		delay_half_bit;//4us
		I2C_SCL_DIR_OUTPUT;
		I2C_SCL_LO;
                delay_half_bit;//4us
}

void CommsIQSxxx_stop(void)
{
		I2C_SDA_DIR_OUTPUT;
		I2C_SDA_LO;
	  delay_half_bit; //	DELAY_US(MCLK*1);//1us
 		I2C_SCL_DIR_INPUT;
 		Poll_I2C_SCL_IN(); 
	  delay_half_bit; //	DELAY_US(MCLK*1);//1us  
		I2C_SDA_HI;  //发送结束条件的时钟信号
		Poll_I2C_SDA_IN();  
 		delay_2bit;   
                delay_2bit;
}  
//*****************************************************************************
//
// Mark the end of the C bindings section for C++ compilers.
//
//*****************************************************************************
//*****************************************************************************
//
// Mark the end of the C bindings section for C++ compilers.
//
//*****************************************************************************



