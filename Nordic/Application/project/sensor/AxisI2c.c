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
*������:		I2cStart
*����:			��
*����ֵ:		��
*����:			��ʼ
*��д:			stardream
*�汾��Ϣ:		V1.0			2011��11��
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
*������:		I2cStop
*����:			��
*����ֵ:		��
*����:			ֹͣ����
*��д:			stardream
*�汾��Ϣ:		V1.0			2011��11��
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
*������:		I2cAck
*����:			��
*����ֵ:		��
*����:			������ݷ���Ӧ���־λ
*��д:			stardream
*�汾��Ϣ:		V1.0			2011��11��
******************************************************************************************/
void I2cAck(void)			                                        //����������ݷ���Ӧ��
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
*������:		I2cNoAck
*����:			��
*����ֵ:		��
*����:			����������ݷ��ͷ�Ӧ��
*��д:			stardream
*�汾��Ϣ:		V1.0			2011��11��
******************************************************************************************/
void I2cNoAck(void)			                                        //����������ݷ��ͷ�Ӧ��
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
*������:		I2cSendByte
*����:			Ҫ���͵�����
*����ֵ:		bool
*����:			�������ݺ���
*��д:			stardream
*�汾��Ϣ:		V1.0			2011��11��
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
      Delayus(1);//��������
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
    AckSingle=FALSE;	                                                        //���ص���Ӧ���ź� ����Ӧ���ź� �������Բ����
  }
  else
  {
    AckSingle=TRUE;                                                             //���ص��Ƿ�Ӧ���ź� ����Ӧ���ź� �������Բ����
  }
  SCL_OUT_LOW;
  SDA_DIR_OUT;
  Delayus(1);
  return(AckSingle);
}
/******************************************************************************************
*������:		I2cReceive
*����:			��
*����ֵ:		�õ�����
*����:			us��ʱ
*��д:			stardream
*�汾��Ϣ:		V1.0			2011��11��
******************************************************************************************/
uint8 I2cReceive(void)
{
  int8  i;
  uint8 retc=0;

  SCL_DIR_OUT;
  SDA_DIR_IN;					                                //set data line input
  
  SCL_OUT_LOW;				                                        //set colok L 	׼�����������ź�
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
*������:		I2cSingleByteWrite
*����:			Address���ӻ���ַ  RegAddress���Ĵ�����ַ Data��Ҫ���͵�����
*����ֵ:		bool
*����:			����һ���ֽڵ����ݳɹ�����TRUE
*��д:			stardream
*�汾��Ϣ:		V1.0			2011��11��
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
*������:		I2cMultipleByteWrite
*����:			Address���ӻ���ַ  RegAddress���Ĵ�����ַ 
*                       Data��Ҫ���͵����ݵĵ�ַ nByte�����Ͷ��ٸ�����
*����ֵ:		bool
*����:			us��ʱ
*��д:			stardream
*�汾��Ϣ:		V1.0			2011��11��
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
*������:		I2cSingleByteRead
*����:			Address���ӻ���ַ  RegAddress���Ĵ�����ַ Value�����ܵ�����ָ��
*����ֵ:		bool
*����:			����һ���ֽڵ����ݳɹ�����TRUE���洢��Value��ָ�ĵ�ַ
*��д:			stardream
*�汾��Ϣ:		V1.0			2011��11��
******************************************************************************************/
bool I2cSingleByteRead(uint8 Address,uint8 RegAddress,uint8 *Value)			//���ֽڶ�
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
  I2cStart();								        //�ٴη�����ʼ�ź�
  if(!I2cSendByte(Address+1))				                        //���Ͷ�����
  {
   I2cStop();
   return (FALSE);
  }
  Delayus(5);
  Temp=I2cReceive();		                                                //������
  *Value=Temp;
  Delayus(5);
  I2cNoAck();
  I2cStop();
  return (TRUE);
}
/******************************************************************************************
*������:		I2cMultipleByteRead
*����:			Address���ӻ���ַ  RegAddress���Ĵ�����ַ 
*                       Value���������ݵĵ�ַ nByte�����ܶ��ٸ�����
*����ֵ:		bool
*����:			us��ʱ
*��д:			stardream
*�汾��Ϣ:		V1.0			2011��11��
******************************************************************************************/
bool I2cMultipleByteRead(uint8 Address,uint8 RegAddress,uint8 *Value,uint16 nByte)		//���������������
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

  I2cStart();								        //�ٴη�����ʼ�ź�
  if(!I2cSendByte(Address+1))				                        //���Ͷ�����
  {
    I2cStop();
    return (FALSE);
  }

  while(i++ != nByte-1)
  {
    *Value=I2cReceive();					                //������
    I2cAck();
    Value++;
  }

  *Value=I2cReceive();						                //���һ�ζ�����
  I2cNoAck();
  I2cStop();
 
  return (TRUE);
}





