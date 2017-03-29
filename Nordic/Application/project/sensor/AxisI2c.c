#include "headfile.h"


	


#define SDA_DIR_OUT	    nrf_gpio_pin_dir_set(SDA_PIN, NRF_GPIO_PIN_DIR_OUTPUT)
#define SDA_DIR_IN	    nrf_gpio_pin_dir_set(SDA_PIN, NRF_GPIO_PIN_DIR_INPUT)

#define SCL_DIR_OUT	    nrf_gpio_pin_dir_set(SCL_PIN, NRF_GPIO_PIN_DIR_OUTPUT)
#define SCL_DIR_IN      nrf_gpio_pin_dir_set(SCL_PIN, NRF_GPIO_PIN_DIR_INPUT)

#define SDA_OUT_HIGH    nrf_gpio_pin_set(SDA_PIN)
#define SDA_OUT_LOW     nrf_gpio_pin_clear(SDA_PIN)

#define SCL_OUT_HIGH    nrf_gpio_pin_set(SCL_PIN)
#define SCL_OUT_LOW	    nrf_gpio_pin_clear(SCL_PIN)

#define SDA_IN_DATA	    nrf_gpio_pin_read(SDA_PIN)
#define SCL_IN_DATA	    nrf_gpio_pin_read(SCL_PIN)







void Delayus(uint16 us)
{
  uint8 i=0;
  uint16 j=0;
  
  for(;j<us;j++)
        i = 1;
	j=i;
}

/******************************************************************************************
*函数名:		I2cStart
*参数:			无
*返回值:		无
*描述:			开始
*编写:			stardream
*版本信息:		V1.0			2011年11月
******************************************************************************************/
void I2cStart(void)
{
  SDA_DIR_OUT;
  SDA_OUT_HIGH;
  SCL_DIR_OUT;
  SCL_OUT_HIGH;
  Delayus(5);

  SDA_OUT_LOW;
  Delayus(2);
  SCL_OUT_LOW;
  Delayus(2);
}
/******************************************************************************************
*函数名:		I2cStop
*参数:			无
*返回值:		无
*描述:			停止传送
*编写:			stardream
*版本信息:		V1.0			2011年11月
******************************************************************************************/
void I2cStop(void)
{
  SCL_DIR_OUT;
  SDA_DIR_OUT;

  SCL_OUT_HIGH;
  Delayus(2);
  SDA_OUT_LOW;
  Delayus(2);
  SDA_OUT_HIGH;
  Delayus(5);
}

void UnlockI2c(void)
{
	SCL_DIR_OUT;
	for(int i = 0; i<9; i++)
	{
  		SCL_OUT_HIGH;
    		Delayus(4);
  		SCL_OUT_LOW;
		Delayus(4);
	}
}
/******************************************************************************************
*函数名:		I2cAck
*参数:			无
*返回值:		无
*描述:			主机身份发送应答标志位
*编写:			stardream
*版本信息:		V1.0			2011年11月
******************************************************************************************/
void I2cAck(void)			                                        //以主机的身份发送应答
{
  SDA_DIR_OUT;
  SCL_DIR_OUT;       

  SDA_OUT_LOW;
  SCL_OUT_HIGH;
  //Delayus(5);
  Delayus(2);
  SCL_OUT_LOW;
  SDA_OUT_HIGH;
  Delayus(1);	
}
/******************************************************************************************
*函数名:		I2cNoAck
*参数:			无
*返回值:		无
*描述:			以主机的身份发送非应答
*编写:			stardream
*版本信息:		V1.0			2011年11月
******************************************************************************************/
void I2cNoAck(void)			                                        //以主机的身份发送非应答
{
  SDA_DIR_OUT;
  SCL_DIR_OUT;

  SDA_OUT_HIGH;
  SCL_OUT_HIGH;
  //Delayus(5);
  Delayus(2);
  SCL_OUT_LOW;
  SDA_OUT_LOW;
  Delayus(1);
}
/******************************************************************************************
*函数名:		I2cSendByte
*参数:			要发送的数据
*返回值:		bool
*描述:			发送数据函数
*编写:			stardream
*版本信息:		V1.0			2011年11月
******************************************************************************************/
bool I2cSendByte(uint8 value)
{
  uint8 i;
  bool AckSingle;
  SDA_DIR_OUT;
  SCL_DIR_OUT;
  
  for(i=0;i<8;i++)
  {
      SCL_OUT_LOW;	//enable Send
      if((value & 0x80)==0x80)
      {
        SDA_OUT_HIGH;
      }
      else
      {
        SDA_OUT_LOW;
      }
      value <<= 1;
      Delayus(1);
      SCL_OUT_HIGH;
      Delayus(1);//波形完整
  } 
  //SCL_OUT_LOW;
  //SDA_OUT_HIGH;
  SCL_OUT_LOW;
  SDA_DIR_IN;					                                //set data line input
  
  Delayus(1);
  SCL_OUT_HIGH;
  Delayus(1);
  if(SDA_IN_DATA)
  {
    AckSingle=FALSE;	                                                        //返回的是应答信号 接受应答信号 主机可以不理会
  }
  else
  {
    AckSingle=TRUE;                                                             //返回的是非应答信号 接受应答信号 主机可以不理会
  }
  SCL_OUT_LOW;
  SDA_DIR_OUT;
  Delayus(1);
  return(AckSingle);
}
/******************************************************************************************
*函数名:		I2cReceive
*参数:			无
*返回值:		得到数据
*描述:			us延时
*编写:			stardream
*版本信息:		V1.0			2011年11月
******************************************************************************************/
uint8 I2cReceive(void)
{
  int8  i;
  uint8 retc=0;

  SCL_DIR_OUT;
  SDA_DIR_IN;					                                //set data line input
  
  SCL_OUT_LOW;				                                        //set colok L 	准备接受数据信号
  Delayus(1);  
  for(i=7;i>=0;i--)
  { 
    SCL_OUT_HIGH;
    Delayus(1);  
    if(SDA_IN_DATA)
    {
     retc|=(0x01<<i); 
    }
    SCL_OUT_LOW;
    Delayus(1);                                                             //
  }
  return(retc);
}
/******************************************************************************************
*函数名:		I2cSingleByteWrite
*参数:			Address：从机地址  RegAddress：寄存器地址 Data：要发送的数据
*返回值:		bool
*描述:			发送一个字节的数据成功返回TRUE
*编写:			stardream
*版本信息:		V1.0			2011年11月
******************************************************************************************/

bool I2cSingleByteWrite(uint8 Address,uint8 RegAddress,uint8 Data)
{
  I2cStart();
  if(!I2cSendByte(Address))
  {
    I2cStop();
    return (FALSE);
  }
  if(!I2cSendByte(RegAddress))
  {
    I2cStop();
    return (FALSE);
  }
  if(!I2cSendByte(Data))
  {
    I2cStop();
    return (FALSE);
  }
  I2cStop();
  return (TRUE);
}
/******************************************************************************************
*函数名:		I2cMultipleByteWrite
*参数:			Address：从机地址  RegAddress：寄存器地址 
*                       Data：要发送的数据的地址 nByte：发送多少个数据
*返回值:		bool
*描述:			us延时
*编写:			stardream
*版本信息:		V1.0			2011年11月
******************************************************************************************/
bool I2cMultipleByteWrite(uint8 Address,uint8 RegAddress,uint8 *Data,uint16 nByte)
{
  uint8 *pData;
  uint16 i;
  pData=Data;

  uint8 j,frist_act;
  j = 3;
  do
  {
    I2cStart();
    frist_act = I2cSendByte(Address);

    if(frist_act == 0)
    {
      I2cStop();
      j--;
      if(j == 0)
      {
        return (FALSE);
      }
      else
      {
        Delayus(5);
      }
    }
  }while(frist_act != 1);
  
  if(!I2cSendByte(RegAddress))
  {
    I2cStop();
    return (FALSE);
  }
  for(i=0;i<nByte;i++)
  {
    if(!I2cSendByte(*pData))
    {
       I2cStop();
       return (FALSE);
    }
    pData++;
  }
  I2cStop();
  return (TRUE);
}
/******************************************************************************************
*函数名:		I2cSingleByteRead
*参数:			Address：从机地址  RegAddress：寄存器地址 Value：接受的数据指针
*返回值:		bool
*描述:			接受一个字节的数据成功返回TRUE，存储到Value所指的地址
*编写:			stardream
*版本信息:		V1.0			2011年11月
******************************************************************************************/
bool I2cSingleByteRead(uint8 Address,uint8 RegAddress,uint8 *Value)			//单字节读
{
  uint8 Temp=0;
  I2cStart();
//  Delayms(1);
  if(!I2cSendByte(Address))
  {
    I2cStop();
    return (FALSE);
  }
  if(!I2cSendByte(RegAddress))
  {
    I2cStop();
    return (FALSE);
  }
  I2cStart();								        //再次发送起始信号
  if(!I2cSendByte(Address+1))				                        //发送读操作
  {
   I2cStop();
   return (FALSE);
  }
  Delayus(5);
  Temp=I2cReceive();		                                                //读数据
  *Value=Temp;
  Delayus(5);
  I2cNoAck();
  I2cStop();
  return (TRUE);
}
/******************************************************************************************
*函数名:		I2cMultipleByteRead
*参数:			Address：从机地址  RegAddress：寄存器地址 
*                       Value：接受数据的地址 nByte：接受多少个数据
*返回值:		bool
*描述:			us延时
*编写:			stardream
*版本信息:		V1.0			2011年11月
******************************************************************************************/
bool I2cMultipleByteRead(uint8 Address,uint8 RegAddress,uint8 *Value,uint16 nByte)		//连续读出多个数据
{
  uint16 i=0;
  uint8 j,frist_act;
  j = 3;
  do
  {
    I2cStart();
    frist_act = I2cSendByte(Address);

    if(frist_act == 0)
    {
      I2cStop();
      j--;
      if(j == 0)
      {
        return (FALSE);
      }
      else
      {
        Delayus(5);
      }
    }
  }while(frist_act != 1);

  if(!I2cSendByte(RegAddress))
  {
    I2cStop();
    return (FALSE);
  }

  I2cStart();								        //再次发送起始信号
  if(!I2cSendByte(Address+1))				                        //发送读操作
  {
    I2cStop();
    return (FALSE);
  }

  while(i++ != nByte-1)
  {
    *Value=I2cReceive();					                //读数据
    I2cAck();
    Value++;
  }

  *Value=I2cReceive();						                //最后一次读数据
  I2cNoAck();
  I2cStop();
 
  return (TRUE);
}





