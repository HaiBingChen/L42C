#include "Headfile.h"



uint8 BtState=BT_CLOSE;
uint16 RevValueOffset=0;
uint8 DataRevBuf[DATA_REV_MAX]={0};

ble_nus_t                 m_nus;



void SendSettingOkRspCmd(void)
{
        osal_set_event(GetBleTaskId(), BLE_SET_OK_RSP_EVENT);
}

void SendSettingFailRspCmd(void)
{
        osal_set_event(GetBleTaskId(), BLE_SET_FAIL_RSP_EVENT);
}

void nus_data_init(void)
{
	RevValueOffset=0;
	osal_memset(DataRevBuf,0,DATA_REV_MAX);	
}

void nus_data_Parse(void)
{
	BLE_BOND_REQUIRED_FLAG = true;	
	uint8 DataRsp=DataParseHandler(DataRevBuf); 
	BLE_BOND_REQUIRED_FLAG = false;				
	if(DataRsp !=E_RSP_NULL)
		SendRspCmd(CurrentDataParseCmd,(RSP_VAULE_E)DataRsp);
	RevValueOffset=0;
	osal_memset(DataRevBuf,0,DATA_REV_MAX);
}

void nus_data_handler(ble_nus_t *p_nus, uint8 *p_data, uint16 length, uint16 handle)
{
        switch(handle)
        {
            case 0x000B:
                if((RevValueOffset+length)  < DATA_REV_MAX)
                {
                        osal_memcpy(&DataRevBuf[RevValueOffset], p_data, length);
                        RevValueOffset += length;
                }
                break;

            case 0x000D:
                if(*p_data == 0x03)
                {
                	//UART_Send("03", 2);
			osal_set_event(GetClockTaskId(), CLOCK_NUS_DATA_PARSE); 
                }
		  else if(*p_data == 0x04)
		  {
		  	nus_data_init();
		  }				
                break;

            default:
                break;
                
        }
        
}




uint8 HwSendDataToBt(uint8 *pData, uint16 DataLen)
{
        if(BtState==BT_CONNECTED)
        {
        	  while(DataLen > 20)
        	  {
        	  	ble_nus_string_send(&m_nus, pData, 20);
			DataLen-=20;	
			pData+=20;
        	  }
		  ble_nus_string_send(&m_nus, pData, DataLen);
		  BlePktSendComplete();//上传完成消息		
        }
        else
        {
                UART_Send(pData, DataLen);
		  BlePktSendComplete();//上传完成消息			
        }
            
        
        return 0;

}









