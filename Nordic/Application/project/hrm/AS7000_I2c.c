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
*������:		I2cStart
*����:			��
*����ֵ:		��
*����:			��ʼ
*��д:			stardream
*�汾��Ϣ:		V1.0			2011��11��
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
*������:		I2cStop
*����:			��
*����ֵ:		��
*����:			ֹͣ����
*��д:			stardream
*�汾��Ϣ:		V1.0			2011��11��
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
*������:		I2cAck
*����:			��
*����ֵ:		��
*����:			������ݷ���Ӧ���־λ
*��д:			stardream
*�汾��Ϣ:		V1.0			2011��11��
******************************************************************************************/
void HRM_I2cAck(void)			                                        //����������ݷ���Ӧ��
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
*������:		I2cNoAck
*����:			��
*����ֵ:		��
*����:			����������ݷ��ͷ�Ӧ��
*��д:			stardream
*�汾��Ϣ:		V1.0			2011��11��
******************************************************************************************/
void HRM_I2cNoAck(void)			                                        //����������ݷ��ͷ�Ӧ��
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
*������:		I2cSendByte
*����:			Ҫ���͵�����
*����ֵ:		bool
*����:			�������ݺ���
*��д:			stardream
*�汾��Ϣ:		V1.0			2011��11��
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
      HRM_Delayus(1);//��������
  } 
  
  HRM_SCL_OUT_LOW;

  
  HRM_SDA_DIR_IN;					                                //set data line input
  
  HRM_Delayus(1);
  HRM_SCL_OUT_HIGH;
  HRM_Delayus(1);
  if(HRM_SDA_IN_DATA)
  {
    AckSingle=FALSE;	                                                        //���ص���Ӧ���ź� ����Ӧ���ź� �������Բ����
  }
  else
  {
    AckSingle=TRUE;                                                             //���ص��Ƿ�Ӧ���ź� ����Ӧ���ź� �������Բ����
  }

  HRM_SCL_OUT_LOW;
  HRM_SDA_DIR_OUT;  
  HRM_Delayus(1);
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
uint8 HRM_I2cReceive(void)
{
  int8  i;
  uint8 retc=0;

  HRM_SCL_DIR_OUT;
  HRM_SDA_DIR_IN;					//set data line input
  
  HRM_SCL_OUT_LOW;				    //set colok L 	׼�����������ź�
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
*������:		HRM_I2cSingleByteWrite
*����:			Address���ӻ���ַ  RegAddress���Ĵ�����ַ Data��Ҫ���͵�����
*����ֵ:		bool
*����:			����һ���ֽڵ����ݳɹ�����TRUE
*��д:			stardream
*�汾��Ϣ:		V1.0			2011��11��
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
*������:		I2cMultipleByteWrite
*����:			Address���ӻ���ַ  RegAddress���Ĵ�����ַ 
*                       Data��Ҫ���͵����ݵĵ�ַ nByte�����Ͷ��ٸ�����
*����ֵ:		bool
*����:			us��ʱ
*��д:			stardream
*�汾��Ϣ:		V1.0			2011��11��
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
*������:		I2cSingleByteRead
*����:			Address���ӻ���ַ  RegAddress���Ĵ�����ַ Value�����ܵ�����ָ��
*����ֵ:		bool
*����:			����һ���ֽڵ����ݳɹ�����TRUE���洢��Value��ָ�ĵ�ַ
*��д:			stardream
*�汾��Ϣ:		V1.0			2011��11��
******************************************************************************************/
bool HRM_I2cSingleByteRead(uint8 Address,uint8 RegAddress,uint8 *Value)			//���ֽڶ�
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
  HRM_I2cStart();								        //�ٴη�����ʼ�ź�
  if(!HRM_I2cSendByte(Address+1))				     //���Ͷ�����
  {
   HRM_I2cStop();
   return (FALSE);
  }
  HRM_Delayus(5);
  Temp=HRM_I2cReceive();		                        //������
  *Value=Temp;
  HRM_Delayus(5);
  HRM_I2cNoAck();
  HRM_I2cStop();
  return (TRUE);
}
/******************************************************************************************
*������:		HRM_I2cMultipleByteRead
*����:			Address���ӻ���ַ  RegAddress���Ĵ�����ַ 
*                       Value���������ݵĵ�ַ nByte�����ܶ��ٸ�����
*����ֵ:		bool
*����:			us��ʱ
*��д:			stardream
*�汾��Ϣ:		V1.0			2011��11��
******************************************************************************************/
bool HRM_I2cMultipleByteRead(uint8 Address,uint8 RegAddress,uint8 *Value,uint16 nByte)		//���������������
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

  HRM_I2cStart();								   //�ٴη�����ʼ�ź�
  if(!HRM_I2cSendByte(Address+1))				//���Ͷ�����
  {
    HRM_I2cStop();
    return (FALSE);
  }
 
  while(i++ != nByte-1)
  {
    *Value=HRM_I2cReceive();					                //������
	
	HRM_I2cAck();
    Value++;
	
  }

  *Value=HRM_I2cReceive();						                //���һ�ζ�����
  HRM_I2cNoAck();
  HRM_I2cStop();
 
  return (TRUE);
}






