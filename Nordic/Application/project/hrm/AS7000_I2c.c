#include "headfile.h"


	

#define HRM_SDA_DIR_OUT	    nrf_gpio_pin_dir_set(HRM_SDA_PIN, NRF_GPIO_PIN_DIR_OUTPUT)
#define HRM_SDA_DIR_IN	    nrf_gpio_pin_dir_set(HRM_SDA_PIN, NRF_GPIO_PIN_DIR_INPUT)

#define HRM_SCL_DIR_OUT	    nrf_gpio_pin_dir_set(HRM_SCL_PIN, NRF_GPIO_PIN_DIR_OUTPUT)
#define HRM_SCL_DIR_IN      nrf_gpio_pin_dir_set(HRM_SCL_PIN, NRF_GPIO_PIN_DIR_INPUT)

#define HRM_SDA_OUT_HIGH    nrf_gpio_pin_set(HRM_SDA_PIN)
#define HRM_SDA_OUT_LOW     nrf_gpio_pin_clear(HRM_SDA_PIN)

#define HRM_SCL_OUT_HIGH    nrf_gpio_pin_set(HRM_SCL_PIN)
#define HRM_SCL_OUT_LOW	    nrf_gpio_pin_clear(HRM_SCL_PIN)

#define HRM_SDA_IN_DATA	    nrf_gpio_pin_read(HRM_SDA_PIN)
#define HRM_SCL_IN_DATA	    nrf_gpio_pin_read(HRM_SCL_PIN)







void HRM_Delayus(uint16 us)
{
  uint8 i=0;
  uint16 j=0;
  
  for(;j<us;j++)
        i = 1;
	j=i;
}


void HRM_Comms_init(void)
{
        HRM_SCL_DIR_OUT;
        HRM_SDA_DIR_OUT;
        delay_2bit;
        delay_2bit;
        
	HRM_SCL_DIR_IN;
	HRM_SDA_DIR_IN;
	delay_2bit;
        delay_2bit;
}


/******************************************************************************************
*函数名:		I2cStart
*参数:			无
*返回值:		无
*描述:			开始
*编写:			stardream
*版本信息:		V1.0			2011年11月
******************************************************************************************/
void HRM_I2cStart(void)
{
  HRM_SDA_DIR_OUT;
  HRM_SDA_OUT_HIGH;
  HRM_SCL_DIR_OUT;
  HRM_SCL_OUT_HIGH;
  HRM_Delayus(5);

  HRM_SDA_OUT_LOW;
  HRM_Delayus(2);
  HRM_SCL_OUT_LOW;
  HRM_Delayus(2);
}




/******************************************************************************************
*函数名:		I2cStop
*参数:			无
*返回值:		无
*描述:			停止传送
*编写:			stardream
*版本信息:		V1.0			2011年11月
******************************************************************************************/
void HRM_I2cStop(void)
{
  HRM_SCL_DIR_OUT;
  //HRM_SDA_DIR_OUT;

  HRM_SDA_OUT_LOW;
  HRM_SCL_OUT_LOW;
  
  
  HRM_SDA_DIR_OUT;
  HRM_Delayus(2);
  HRM_SCL_OUT_HIGH;
  HRM_Delayus(2);
  HRM_SDA_OUT_HIGH;
  HRM_Delayus(5);
}

void HRM_UnlockI2c(void)
{
	HRM_SCL_DIR_OUT;
	for(int i = 0; i<9; i++)
	{
  		HRM_SCL_OUT_HIGH;
    		HRM_Delayus(4);
  		HRM_SCL_OUT_LOW;
		HRM_Delayus(4);
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
void HRM_I2cAck(void)			                                        //以主机的身份发送应答
{
  HRM_SDA_DIR_OUT;
  HRM_SCL_DIR_OUT;       

  HRM_SDA_OUT_LOW;
  HRM_SCL_OUT_HIGH;
  HRM_Delayus(2);
  HRM_SCL_OUT_LOW;
  HRM_SDA_OUT_HIGH;
  HRM_Delayus(1);
}
/******************************************************************************************
*函数名:		I2cNoAck
*参数:			无
*返回值:		无
*描述:			以主机的身份发送非应答
*编写:			stardream
*版本信息:		V1.0			2011年11月
******************************************************************************************/
void HRM_I2cNoAck(void)			                                        //以主机的身份发送非应答
{
  HRM_SDA_DIR_OUT;
  HRM_SCL_DIR_OUT;

  HRM_SDA_OUT_HIGH;
  HRM_SCL_OUT_HIGH;
  HRM_Delayus(2);
  HRM_SCL_OUT_LOW;
  HRM_SDA_OUT_LOW;
  HRM_Delayus(1);
}
/******************************************************************************************
*函数名:		I2cSendByte
*参数:			要发送的数据
*返回值:		bool
*描述:			发送数据函数
*编写:			stardream
*版本信息:		V1.0			2011年11月
******************************************************************************************/
bool HRM_I2cSendByte(uint8 value)
{
  uint8 i;
  bool AckSingle;

  
  HRM_SDA_DIR_OUT;
  HRM_SCL_DIR_OUT;
  for(i=0;i<8;i++)
  {
      HRM_SCL_OUT_LOW;	//enable Send
      if((value & 0x80)==0x80)
      {
        HRM_SDA_OUT_HIGH;
      }
      else
      {
        HRM_SDA_OUT_LOW;
      }
      value <<= 1;
      HRM_Delayus(1);
      HRM_SCL_OUT_HIGH;
      HRM_Delayus(1);//波形完整
  } 
  
  HRM_SCL_OUT_LOW;

  
  HRM_SDA_DIR_IN;					                                //set data line input
  
  HRM_Delayus(1);
  HRM_SCL_OUT_HIGH;
  HRM_Delayus(1);
  if(HRM_SDA_IN_DATA)
  {
    AckSingle=FALSE;	                                                        //返回的是应答信号 接受应答信号 主机可以不理会
  }
  else
  {
    AckSingle=TRUE;                                                             //返回的是非应答信号 接受应答信号 主机可以不理会
  }

  HRM_SCL_OUT_LOW;
  HRM_SDA_DIR_OUT;  
  HRM_Delayus(1);
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
uint8 HRM_I2cReceive(void)
{
  int8  i;
  uint8 retc=0;

  HRM_SCL_DIR_OUT;
  HRM_SDA_DIR_IN;					//set data line input
  
  HRM_SCL_OUT_LOW;				    //set colok L 	准备接受数据信号
  HRM_Delayus(1); 
  for(i=7;i>=0;i--)
  { 
    HRM_SCL_OUT_HIGH;
    HRM_Delayus(1);
    if(HRM_SDA_IN_DATA)
    {
     retc|=(0x01<<i); 
    }
    HRM_SCL_OUT_LOW;
    HRM_Delayus(1); 
  }
  return(retc);
}
/******************************************************************************************
*函数名:		HRM_I2cSingleByteWrite
*参数:			Address：从机地址  RegAddress：寄存器地址 Data：要发送的数据
*返回值:		bool
*描述:			发送一个字节的数据成功返回TRUE
*编写:			stardream
*版本信息:		V1.0			2011年11月
******************************************************************************************/

bool HRM_I2cSingleByteWrite(uint8 Address,uint8 RegAddress,uint8 Data)
{
  HRM_I2cStart();
  if(!HRM_I2cSendByte(Address))
  {
    HRM_I2cStop();
    return (FALSE);
  }
  if(!HRM_I2cSendByte(RegAddress))
  {
    HRM_I2cStop();
    return (FALSE);
  }
  if(!HRM_I2cSendByte(Data))
  {
    HRM_I2cStop();
    return (FALSE);
  }
  HRM_I2cStop();
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
bool HRM_I2cMultipleByteWrite(uint8 Address,uint8 RegAddress,uint8 *Data,uint16 nByte)
{
  uint8 *pData;
  uint16 i;
  pData=Data;

  uint8 j,frist_act;
  j = 3;
  do
  {
    HRM_I2cStart();
    frist_act = HRM_I2cSendByte(Address);

    if(frist_act == 0)
    {
      HRM_I2cStop();
      j--;
      if(j == 0)
      {
       return (FALSE);
      }
      else
      {
        HRM_Delayus(5);
      }
    }
  }while(frist_act != 1);
  
  if(!HRM_I2cSendByte(RegAddress))
  {
    HRM_I2cStop();
       return (FALSE);
  }
  for(i=0;i<nByte;i++)
  {
    if(!HRM_I2cSendByte(*pData))
    {
       HRM_I2cStop();
       return (FALSE);
    }
    pData++;
  }
  HRM_I2cStop();
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
bool HRM_I2cSingleByteRead(uint8 Address,uint8 RegAddress,uint8 *Value)			//单字节读
{
  uint8 Temp=0;
  HRM_I2cStart();
//  Delayms(1);
  if(!HRM_I2cSendByte(Address))
  {
    HRM_I2cStop();
    return (FALSE);
  }
  if(!HRM_I2cSendByte(RegAddress))
  {
    HRM_I2cStop();
    return (FALSE);
  }
  HRM_I2cStart();								        //再次发送起始信号
  if(!HRM_I2cSendByte(Address+1))				     //发送读操作
  {
   HRM_I2cStop();
   return (FALSE);
  }
  HRM_Delayus(5);
  Temp=HRM_I2cReceive();		                        //读数据
  *Value=Temp;
  HRM_Delayus(5);
  HRM_I2cNoAck();
  HRM_I2cStop();
  return (TRUE);
}
/******************************************************************************************
*函数名:		HRM_I2cMultipleByteRead
*参数:			Address：从机地址  RegAddress：寄存器地址 
*                       Value：接受数据的地址 nByte：接受多少个数据
*返回值:		bool
*描述:			us延时
*编写:			stardream
*版本信息:		V1.0			2011年11月
******************************************************************************************/
bool HRM_I2cMultipleByteRead(uint8 Address,uint8 RegAddress,uint8 *Value,uint16 nByte)		//连续读出多个数据
{
  uint16 i=0;
  uint8 j,frist_act;
  j = 3;
  do
  {
    HRM_I2cStart();
    frist_act = HRM_I2cSendByte(Address);

    if(frist_act == 0)
    {
      HRM_I2cStop();
      j--;
      if(j == 0)
      {
        return (FALSE);
      }
      else
      {
        HRM_Delayus(5);
      }
    }
  }while(frist_act != 1);
  if(!HRM_I2cSendByte(RegAddress))
  {
    HRM_I2cStop();
    return (FALSE);
  }

  HRM_I2cStart();								   //再次发送起始信号
  if(!HRM_I2cSendByte(Address+1))				//发送读操作
  {
    HRM_I2cStop();
    return (FALSE);
  }
 
  while(i++ != nByte-1)
  {
    *Value=HRM_I2cReceive();					                //读数据
	
	HRM_I2cAck();
    Value++;
	
  }

  *Value=HRM_I2cReceive();						                //最后一次读数据
  HRM_I2cNoAck();
  HRM_I2cStop();
 
  return (TRUE);
}






