
#include "headfile.h"

#define C_DEBUG 						0

#define C_SCL_PIN		              	5
#define C_SDA_PIN		              	18
#define C_TP_POWER_PIN          		20
#define C_MCLK							48

#define C_SDA_DIR_OUT	    	nrf_gpio_pin_dir_set(C_SDA_PIN, NRF_GPIO_PIN_DIR_OUTPUT)
#define C_SDA_DIR_IN	    	nrf_gpio_pin_dir_set(C_SDA_PIN, NRF_GPIO_PIN_DIR_INPUT)

#define C_SCL_DIR_OUT	    	nrf_gpio_pin_dir_set(C_SCL_PIN, NRF_GPIO_PIN_DIR_OUTPUT)
#define C_SCL_DIR_IN     	 	nrf_gpio_pin_dir_set(C_SCL_PIN, NRF_GPIO_PIN_DIR_INPUT)

#define C_SDA_OUT_HIGH    		nrf_gpio_pin_set(C_SDA_PIN)
#define C_SDA_OUT_LOW     		nrf_gpio_pin_clear(C_SDA_PIN)

#define C_SCL_OUT_HIGH    		nrf_gpio_pin_set(C_SCL_PIN)
#define C_SCL_OUT_LOW	    	nrf_gpio_pin_clear(C_SCL_PIN)

#define C_SDA_IN_DATA	    	nrf_gpio_pin_read(C_SDA_PIN)
#define C_SCL_IN_DATA	    	nrf_gpio_pin_read(C_SCL_PIN)

#define C_TP_POWER_DIR_OUTPUT	nrf_gpio_cfg_output(C_TP_POWER_PIN)
#define C_TP_POWER_HI           nrf_gpio_pin_set(C_TP_POWER_PIN)
#define C_TP_POWER_LO           nrf_gpio_pin_clear(C_TP_POWER_PIN)


#define TC_SECTOR_SZ							8
#define TSP_TYPE_COUNT							1
#define SUPPORT_TP_UPDATE_FIRMWARE			1

uint16 reg_data_version=0;
uint8 m_FirmwareIdx=0;

bool CypressI2C_Init= false;
uint8 Cxy[6] ={0x00};

uint8 Cx=0,Cy=0;
uint16 CCount = 0;

uint16 chip_code;
uint16 gesture_flag=0;
uint16	 ic_int_mask = 0;

#if C_DEBUG
static void cPrintf(uint8_t * buf, uint8_t flag)
{
	int len = 0;
	uint8_t temp[2]={0};

	if (flag  == 0)
	{
		len = strlen((const char *)buf);
		UART_Send(buf, len);
	} 
	else if(flag==1) {
		if (((buf[0] >> 4) & 0x0F) < 0x0A)
			temp[0] = ((buf[0] >> 4) & 0x0F) + 0x30;
		else
			temp[0] = ((buf[0] >> 4) & 0x0F) + 0x37;
		if (((buf[0] & 0x0F)) < 0x0A)
			temp[1] = (buf[0] & 0x0F) + 0x30;
		else
			temp[1] = (buf[0] & 0x0F) + 0x37;
		UART_Send("0x", 2);
		UART_Send(temp, 2);
	}
	else if(flag ==2)
	{
		uint8 tempc[10]={0x00};
		sprintf((char*)tempc,"%d",buf[0]);
		uint8 len = strlen((char*)tempc);
		UART_Send(tempc,len);		
	}
}
#else
static void cPrintf(uint8_t * buf, uint8_t flag) {}
#endif

void C_DELAY_US (unsigned long cycles)
{ 
	while(cycles>15)					// 15 cycles consumed by overhead
		cycles = cycles - 4;			// 6 cycles consumed each iteration
}

void C_DELAY_MS (unsigned long cycles)
{ 
	uint32_t cnt;
	while(cycles)
	{
		cycles = cycles - 1;
		cnt = C_MCLK;
		while(cnt)
		{
			C_DELAY_US(1000);
			cnt--;
		}
	}
}

void C_Delayus(uint16 us)
{
	uint8 i=0;
	uint16 j=0;
	for(;j<(us*9);j++)
		i = 1;
	j=i;
}


void C_I2cStart(void)
{
	C_SDA_DIR_OUT;
	C_SDA_OUT_HIGH;
	C_SCL_DIR_OUT;
	C_SCL_OUT_HIGH;
	C_Delayus(5);

	C_SDA_OUT_LOW;
	C_Delayus(2);
	C_SCL_OUT_LOW;
	C_Delayus(2);
}

void C_I2cStop(void)
{
	C_SDA_DIR_OUT;
	C_Delayus(1);
	C_SCL_OUT_HIGH;
	C_Delayus(2);
	C_SDA_OUT_LOW;
	C_Delayus(2);
	C_SDA_OUT_HIGH;
	C_Delayus(5);
}

void C_UnlockI2c(void)
{
	C_SCL_DIR_OUT;
	for(int i = 0; i<9; i++)
	{
  		C_SCL_OUT_HIGH;
    		C_Delayus(4);
  		C_SCL_OUT_LOW;
		C_Delayus(4);
	}
}

void C_I2cAck(void)			                                     
{
	C_SDA_OUT_LOW;
	C_SCL_OUT_HIGH;
	C_Delayus(1);
	C_SCL_OUT_LOW;
	C_Delayus(1);	
}

void C_I2cNoAck(void)			                                      
{
	C_SDA_OUT_HIGH;
	C_SCL_OUT_HIGH;
	C_Delayus(1);
	C_SCL_OUT_LOW;
	C_SDA_OUT_LOW;
	C_Delayus(1);
}

bool C_I2cSendByte(uint8 value)
{
	uint8 i;
	bool AckSingle;
	C_SDA_DIR_OUT;

	for(i=0;i<8;i++)
	{
		C_SCL_OUT_LOW;	
		if((value & 0x80)==0x80)
		{
			C_SDA_OUT_HIGH;
		}
		else
		{
			C_SDA_OUT_LOW;
		}
		value <<= 1;
		C_Delayus(1);
		C_SCL_OUT_HIGH;
		C_Delayus(1);
	} 
	C_SCL_OUT_LOW;
	C_SDA_DIR_IN;       
	C_Delayus(1);
	C_SCL_OUT_HIGH;  					                                
	C_Delayus(1);
	if(C_SDA_IN_DATA)
	{
		AckSingle=FALSE;	                                                    
	}
	else
	{
		AckSingle=TRUE;                                                      
	}
	C_SCL_OUT_LOW;
	C_SDA_DIR_OUT;
	C_Delayus(1);
	return(AckSingle);
}

uint8 C_I2cReceive(void)
{
	int8  i;
	uint8 retc=0;

	C_SDA_DIR_IN;					                                
	C_Delayus(1);  
	C_SCL_OUT_LOW;				                                  

	for(i=7;i>=0;i--)
	{ 
		C_Delayus(1); 
		C_SCL_OUT_HIGH;
		C_Delayus(1);  
		if(C_SDA_IN_DATA)
		{
			retc|=(0x01<<i); 
		}
		C_SCL_OUT_LOW;
	}
	C_SDA_DIR_OUT;
	C_Delayus(1);
	return(retc);
}


bool C_I2cTouchByteRead(uint8 Address,uint8 *Value,uint16 nByte)			
{
	uint16 i=0;
	C_I2cStart();								        
	if(!C_I2cSendByte(Address+1))			
	{
		C_I2cStop();
		return (FALSE);
	}

	C_Delayus(2);

	while(i++ != nByte-1)
	{
		*Value=C_I2cReceive();					   
		C_I2cAck();
		Value++;
	}  

	*Value=C_I2cReceive();						  
	C_I2cNoAck();
	C_I2cStop();

	return (TRUE);
}

bool ZT75xx_I2cWriteCMD(uint16 reg)			
{
	uint8 RegTmp[2];

	RegTmp[0]=reg&0xFF;
	RegTmp[1]=reg>>8;
	
	C_I2cStart();								        
	if(!C_I2cSendByte(0x40))			
	{
		cPrintf("C_I2cSendByte 0x40 fail.", 0);
		C_I2cStop();
		return (FALSE);
	}

	if(!C_I2cSendByte(RegTmp[0]))			
	{
		C_I2cStop();
		return (FALSE);
	}

	if(!C_I2cSendByte(RegTmp[1]))			
	{
		C_I2cStop();
		return (FALSE);
	}

	C_Delayus(2);
	C_I2cStop();

	return (TRUE);
}


bool ZT75xx_I2cReadData(uint16 reg,uint8 *Value,uint16 nByte)			
{
	uint16 i=0;
	uint8 RegVal[2];
	if(ZT75xx_I2cWriteCMD(reg)==false)
	{
		RegVal[0]=reg&0xFF;			
		RegVal[1]=reg>>8;
		cPrintf("ZT75xx_I2cWriteCMD fail=", 0);
		cPrintf(&RegVal[1], 1);
		cPrintf("_", 0);
		cPrintf(&RegVal[0], 1);		
		cPrintf("\r\n", 0);
	}
	
	C_I2cStart();
	
	if(!C_I2cSendByte(0x41))			
	{
		C_I2cStop();
		return (FALSE);
	}

	C_Delayus(2);

	while(i++ != nByte-1)
	{
		*Value=C_I2cReceive();					   
		C_I2cAck();
		Value++;
	}  

	*Value=C_I2cReceive();						  
	C_I2cNoAck();
	C_I2cStop();

	return (TRUE);
}


bool ZT75xx_I2cWriteDATA(uint16 reg,uint8 *Value)			
{
	uint8 RegTmp[2];

	RegTmp[0]=reg&0xFF;
	RegTmp[1]=reg>>8;
	
	C_I2cStart();								        
	if(!C_I2cSendByte(0x40))			
	{
		C_I2cStop();
		return (FALSE);
	}

	if(!C_I2cSendByte(RegTmp[0]))			
	{
		C_I2cStop();
		return (FALSE);
	}

	if(!C_I2cSendByte(RegTmp[1]))			
	{
		C_I2cStop();
		return (FALSE);
	}

	if(!C_I2cSendByte(Value[0]))			
	{
		C_I2cStop();
		return (FALSE);
	}

	if(!C_I2cSendByte(Value[1]))			
	{
		C_I2cStop();
		return (FALSE);
	}

	C_Delayus(2);
	C_I2cStop();
	return (TRUE);
}

bool ZT75xx_I2cWriteREG(uint16 reg,uint16 Value)	
{
	uint8 RegVal[2];
	if(ZT75xx_I2cWriteDATA(reg,(uint8 *)&Value)!=true)
	{
		RegVal[0]=reg&0xFF;			
		RegVal[1]=reg>>8;
		cPrintf("Register write fail=", 0);
		cPrintf(&RegVal[1], 1);
		cPrintf("_", 0);
		cPrintf(&RegVal[0], 1);		
		cPrintf("\r\n", 0);
		return false;
	}
	return true;
}

void zinitix_suspend(void)
{
#if 0
	ZT75xx_I2cWriteCMD(ZINITIX_CLEAR_INT_STATUS_CMD);
	nrf_delay_ms(10);
	ZT75xx_I2cWriteCMD(ZINITIX_SLEEP_CMD);
	nrf_delay_ms(10);
	TP_RESUME_FLAG =true;
	cPrintf("TP_suspend\r\n", 0);
#else
	int i;
	nrf_delay_ms(10);
	ZT75xx_I2cWriteCMD(0x000A);
	nrf_delay_ms(10);
	
	//ZT75xx_I2cWriteCMD(ZINITIX_CLEAR_INT_STATUS_CMD);
	//nrf_delay_ms(10);
	//ZT75xx_I2cWriteCMD(ZINITIX_SLEEP_CMD);
	//nrf_delay_ms(10);
	for(i = 0; i <ZINITIX_INIT_RETRY_CNT; i++) 
	{
		if(ZT75xx_I2cWriteCMD(ZINITIX_CLEAR_INT_STATUS_CMD)==0) 
		{
			cPrintf("tpd_resume fail to send wakeup_cmd\r\n", 0);
			nrf_delay_ms(10);
			continue;
		}
		else
			break;
	}
	for(i = 0; i <ZINITIX_INIT_RETRY_CNT; i++) 
	{
		if(ZT75xx_I2cWriteCMD(ZINITIX_SLEEP_CMD)==0)
		{
			cPrintf("tpd_resume fail to send wakeup_cmd\r\n", 0);
			nrf_delay_ms(10);
			continue;
		} 
		else
			break;
		ZT75xx_I2cWriteCMD(0x000B);
		nrf_delay_ms(10);        

	}
	TP_RESUME_FLAG =true;
	cPrintf("TP_suspend\r\n", 0);
#endif
}
void zinitix_resume(void)
{
	int i;
	ZT75xx_I2cWriteCMD(0x000A);
	nrf_delay_ms(10);
	for(i = 0; i <ZINITIX_INIT_RETRY_CNT; i++) {	   
		if(ZT75xx_I2cWriteCMD(ZINITIX_WAKEUP_CMD)==0) {
			cPrintf("tpd_resume fail to send wakeup_cmd\r\n", 0);
			nrf_delay_ms(10);
		} else
			break;
	}
	if (i==ZINITIX_INIT_RETRY_CNT) {  
		CypressI2CInit ();/*重新初始化*/
		goto reset_exit;
	}
	nrf_delay_ms(1);
	ZT75xx_I2cWriteCMD(ZINITIX_SWRESET_CMD);
	nrf_delay_ms(20);
	for(i=0; i<10; i++)
	{
		ZT75xx_I2cWriteCMD(ZINITIX_CLEAR_INT_STATUS_CMD);
		nrf_delay_ms(10);
	}
	ZT75xx_I2cWriteCMD(0x000B);
	nrf_delay_ms(10);
	TP_RESUME_FLAG =false;
	cPrintf("TP_resume\r\n", 0);

reset_exit:
	return;
}

void zinitix_touchinfo()
{
	uint8 i=0;
	uint16 x=0,y=0;
	
	if(TP_RESUME_FLAG)	
	{
		ZT75xx_I2cReadData(0x0126,(uint8 *)(&gesture_flag),2);
	       cPrintf("gesture_flag=", 0);
		cPrintf((uint8 *)&gesture_flag, 1);
		cPrintf("\r\n", 0);

		if(gesture_flag!=0)
		{
			if(gesture_flag==1)
			{
				if(Powerstate.powerstate==true)
				{
					SendMsg(MSG_TOUCH_UP_DOUBLE_SHORTKEY_FOR_RESUME);
					cPrintf("Doble Touch\r\n", 0);
				}
			}
			ZT75xx_I2cWriteREG(0x0126, 0x0000);
			
	      goto exit;
		}	
	}
	
	ZT75xx_I2cReadData(ZINITIX_POINT_STATUS_REG,(uint8 *)(&touch_info),8);
	ZT75xx_I2cReadData(ZINITIX_POINT_STATUS_REG+4,((uint8 *)&touch_info + 8),2);
	/*后续点的信息*/
	for (i = 1; i < MAX_SUPPORTED_FINGER_NUM; i++) 
	{
		if (zinitix_bit_test(touch_info.event_flag, i)) 
		{

			ZT75xx_I2cReadData(ZINITIX_POINT_STATUS_REG + 2 + (i * 4),
				(uint8 *)(&touch_info.coord[i]),sizeof(struct _ts_zinitix_coord)); 

		}
	} 

	/*down up 事件的判断*/
	for (i = 0; i < MAX_SUPPORTED_FINGER_NUM; i++) 
	{        
		if (zinitix_bit_test(touch_info.coord[i].sub_status, SUB_BIT_DOWN)
		//|| zinitix_bit_test(touch_info.coord[i].sub_status, SUB_BIT_MOVE)
		|| zinitix_bit_test(touch_info.coord[i].sub_status, SUB_BIT_EXIST))
		{			
			x = touch_info.coord[i].x;
			y = touch_info.coord[i].y;
			Cxy[0]=(uint8)x;
			Cxy[1]=(uint8)y;
			Cx = Cxy[0]; 
			Cy = Cxy[1];
			SendMsg(MSG_TOUCH_UP_SHORTKEY_PRESS);
			osal_start_timerEx(GetTouchKeyTaskId(), HAL_CYPRESS_LONG_TOUCH_EVENT, 2000);
			cPrintf("down x=", 0);
			cPrintf(&Cxy[0], 2);
			cPrintf("_y=", 0);
			cPrintf(&Cxy[1], 2);		
			cPrintf(" ", 0);			
		}
		else if (zinitix_bit_test(touch_info.coord[i].sub_status, SUB_BIT_UP)) {
			x = touch_info.coord[i].x;
			y = touch_info.coord[i].y;
			Cxy[2]=(uint8)x;
			Cxy[3]=(uint8)y;
			cPrintf("up x=", 0);
			cPrintf(&Cxy[2], 2);
			cPrintf("_y=", 0);
			cPrintf(&Cxy[3], 2);		
			cPrintf("\r\n", 0);
			CypressI2CParse();
			memset(Cxy,0,6);
		}
		else
		{
			memset(&touch_info.coord[i], 0x0, sizeof(struct _ts_zinitix_coord));
		}
		
		memcpy((char *)&old_touch_info, (char *)&touch_info, sizeof(struct _ts_zinitix_point_info)); 
	}

/*clear中断，clear后中断会被拉高*/
exit:
		ZT75xx_I2cWriteCMD(ZINITIX_CLEAR_INT_STATUS_CMD);
		GPIOTE_IRQ_USB_Recovery();
		if(Powerstate.powerstate==true)
			SetSystemSuspendTime(DISPLAY_TIME_10S_FLAG);
}


void CypressI2cPress(void)
{
	

	C_I2cTouchByteRead(0x20,Cxy,2);
	//cPrintf("x0=", 0);cPrintf(Cxy, 1);cPrintf(" y0=", 0);cPrintf((Cxy+1), 1);cPrintf("\r\n", 0);
	SetSystemSuspendTime(DISPLAY_TIME_10S_FLAG);
}
void CypressI2cRelese(void)
{
	C_I2cTouchByteRead(0x20,Cxy+2,4);
	//cPrintf("x1=", 0);cPrintf((Cxy+2), 1);cPrintf(" y1=", 0);cPrintf((Cxy+3), 1);cPrintf(" x0=", 0);cPrintf((Cxy+4), 1);cPrintf(" y0=", 0);cPrintf((Cxy+5), 1);cPrintf("\r\n", 0);
	if(LCD_DISPLAY_FLAG)
	{
		if(Cxy[0]==0xff&&Cxy[1]==0xff)
		{
			Cxy[0]=Cxy[4];
			Cxy[1]=Cxy[5];
		}
	}
	else
	{
		memset(Cxy,0,5);   	/*休眠不处理坐标*/
	}
	CypressI2CParse();
}


void CypressI2CReadxy(void)
{
	if(CypressI2C_Init)
	{
		if(Cxy[4]==0)
		{
			C_I2cTouchByteRead(0x20,Cxy,2);
			if(Cxy[0]==0xFF&&Cxy[1]==0xFF)
			{
				Cxy[0] = 0;
				Cxy[1] = 0;
			}
			else
				Cxy[4] =1;
		}
		else
			C_I2cTouchByteRead(0x20,Cxy+2,2);
		CCount++;
		if(CCount==270)
			osal_set_event(GetTouchKeyTaskId(),HAL_CYPRESS_LONG_TOUCH_EVENT);
		SetSystemSuspendTime(DISPLAY_TIME_10S_FLAG);
		osal_start_timerEx(GetTouchKeyTaskId(), HAL_CYPRESS_ONE_TOUCH_EVENT, 100);
	}
}


uint8 Get_Screen_Locaton(uint8 line, uint8 row)
{
	uint8 Pos_Val=0;
	uint8 X_Scale=0,Y_Scale=0,Line_X=0,Line_Y=0;
	uint8 Pos_X=Cy,Pos_Y=TPD_RES_MAX_X-Cx;
	//uint8 Pos_X=Cx,Pos_Y=Cy;
	if(Pos_X==TPD_RES_MAX_X)
		Pos_X-=1;
	if(Pos_Y==TPD_RES_MAX_Y)
		Pos_Y-=1;
	if((line==0)||(row==0))
	{
		return 0xFF;
	}
	X_Scale=TPD_RES_MAX_X/(row);
	Y_Scale=TPD_RES_MAX_Y/(line);

	Line_X=Pos_X/X_Scale;
	Line_Y=Pos_Y/Y_Scale;

	Pos_Val=Line_Y*row+Line_X;
	cPrintf("Locaton=", 0);
	cPrintf(&Pos_Val, 1);
	cPrintf("\r\n", 0);
	return Pos_Val;
}

void CypressI2CParse(void)
{
	uint8 xoff =0, yoff=0;
	bool left =false, right =false,up=false,down =false;
	
	Cx = Cxy[0]; 
	Cy = Cxy[1]; 

	if(Cxy[0]>Cxy[2])    		/*右*/
	{
		xoff = Cxy[0]-Cxy[2];
		Cx-=xoff/2;
		right = true;
	}
	else if(Cxy[0]<Cxy[2])		/*左*/
	{
		xoff = Cxy[2]-Cxy[0];
		Cx+=xoff/2;
		left =true;
	}
	if(Cxy[1]>Cxy[3])			/*下*/	
	{
		yoff = Cxy[1]-Cxy[3];
		Cy-=yoff/2;
		down =true;
	}
	else if(Cxy[1]<Cxy[3])		/*上*/
	{
		yoff = Cxy[3]-Cxy[1];
		Cy+=yoff/2;
		up =true;
	}

	if(osal_stop_timerEx(GetTouchKeyTaskId(), HAL_CYPRESS_LONG_TOUCH_EVENT)==SUCCESS)  /*非长按*/
	{
		if(xoff<TPD_TOUCH_X && yoff<TPD_TOUCH_Y)
		{
			 cPrintf("Touch\r\n", 0);
		        uint8 KeysValue=HAL_KEY_SW_1;
		        uint8 KeyState=HAL_KEY_STATE_SHORT_UP;	        
		        HalKeyDistribute(KeysValue, KeyState);
		}
		else if(xoff<yoff &&((yoff-xoff)>10)&& up)
		{
			cPrintf("right\r\n", 0);
			SendMsg(MSG_TOUCH_UP_DOWN_FLICK);		
		}
		else if(xoff<yoff &&((yoff-xoff)>10)&& down)
		{
			cPrintf("left\r\n", 0);
			SendMsg(MSG_TOUCH_DOWN_UP_FLICK);
		}
		else if(left)
		{
			cPrintf("up\r\n", 0);
			SendMsg(MSG_TOUCH_UP_FLICK);
		}
		else if(right)
		{
			cPrintf("down\r\n", 0);
			SendMsg(MSG_TOUCH_DOWN_FLICK);		
		}	
	}
	else
	{
		SendMsg(MSG_TOUCH_LONGKEY_UP);
	}
	memset(Cxy,0,6);
}

void CypressI2CLongTouch(void)
{
	cPrintf("Long Touch ", 0);
	Cx = Cxy[0]; 
	Cy = Cxy[1]; 
	SendMsg(MSG_TOUCH_LONGKEY);
}

bool ts_write_data(uint16 reg, uint8 *values, uint16 length)
{
	uint8 RegTmp[2],i=0;

	RegTmp[0]=reg&0xFF;
	RegTmp[1]=reg>>8;
	
	C_I2cStart();								        
	if(!C_I2cSendByte(0x40))			
	{
		C_I2cStop();
		return (FALSE);
	}

	if(!C_I2cSendByte(RegTmp[0]))			
	{
		C_I2cStop();
		return (FALSE);
	}

	if(!C_I2cSendByte(RegTmp[1]))			
	{
		C_I2cStop();
		return (FALSE);
	}

	for(i=0;i<length;i++){
		if(!C_I2cSendByte(values[i]))			
		{
			C_I2cStop();
			return (FALSE);
		}
	}	
	C_Delayus(2);					  
	C_I2cStop();

	return TRUE;
}

#ifdef SUPPORT_TP_UPDATE_FIRMWARE

void zinitix_power_control(bool enable)
{
    if(enable)   
    {
		/*eric modify 20160512*/
		C_TP_POWER_HI;
		C_DELAY_MS(10);
		C_TP_POWER_LO;
		C_DELAY_MS(20);
		C_TP_POWER_HI;
		C_DELAY_MS(CHIP_ON_DELAY);  

    }else{
		/*eric modify 20160512*/
		C_TP_POWER_LO;
        C_DELAY_MS(CHIP_OFF_DELAY);  
    }
}

static bool ts_read_firmware_data(uint16 reg, uint8 *values, uint16 length)
{
	return ZT75xx_I2cReadData(reg, values, length);
}

static bool ts_power_sequence(void)
{
	uint8 retry = 0;
	uint16 chip_code;
	
	retry_power_sequence:	
	if (ZT75xx_I2cWriteREG(0xc000, 0x0001) != I2C_SUCCESS){
		cPrintf("power sequence error (vendor cmd enable)\r\n",0);
		goto fail_power_sequence;
	}
	DELAY_US(10);
	if (ZT75xx_I2cReadData(0xcc00, (uint8 *)&chip_code, 2) != true) {
		cPrintf("fail to read chip code\r\n",0);
		goto fail_power_sequence;
	}

	DELAY_US(10);	
	if (ZT75xx_I2cWriteCMD(0xc004) != I2C_SUCCESS){
		cPrintf("power sequence error (intn clear)\r\n",0);
		goto fail_power_sequence;
	}
	DELAY_US(10);
	if (ZT75xx_I2cWriteREG(0xc002, 0x0001) != I2C_SUCCESS){
		cPrintf("power sequence error (nvm init)\r\n",0);
		goto fail_power_sequence;
	}
	C_DELAY_MS(2);
	if (ZT75xx_I2cWriteREG(0xc001, 0x0001) != I2C_SUCCESS){
		cPrintf("power sequence error (program start)\r\n",0);
		goto fail_power_sequence;
	}
	C_DELAY_MS(FIRMWARE_ON_DELAY);	//wait for checksum cal
	return true;
	
	fail_power_sequence:
	if(retry++ < 3) {
		C_DELAY_MS(CHIP_ON_DELAY);
		cPrintf("retry = ", 0);		
		cPrintf(&retry, 1);	
		cPrintf("\r\n", 0);		
		goto retry_power_sequence;
	}
	return false;
}

#if 0
static bool ts_hw_calibration(void)
{
	uint16 chip_eeprom_info;
	int time_out = 0;
	
	if (ZT75xx_I2cWriteREG(ZINITIX_TOUCH_MODE, 0x07) != I2C_SUCCESS)
		return false;
	C_DELAY_MS(10);
	ZT75xx_I2cWriteCMD(ZINITIX_CLEAR_INT_STATUS_CMD);
	C_DELAY_MS(10);
	ZT75xx_I2cWriteCMD(ZINITIX_CLEAR_INT_STATUS_CMD); 
	C_DELAY_MS(50);
	ZT75xx_I2cWriteCMD(ZINITIX_CLEAR_INT_STATUS_CMD); 
	C_DELAY_MS(10);
	if (ZT75xx_I2cWriteCMD(ZINITIX_CALIBRATE_CMD) != I2C_SUCCESS)
		return false;
	if (ZT75xx_I2cWriteCMD(ZINITIX_CLEAR_INT_STATUS_CMD) != I2C_SUCCESS)
		return false;
	C_DELAY_MS(10);
	ZT75xx_I2cWriteCMD(ZINITIX_CLEAR_INT_STATUS_CMD);
	/* wait for h/w calibration*/
	do {
		C_DELAY_MS(500);
		ZT75xx_I2cWriteCMD(ZINITIX_CLEAR_INT_STATUS_CMD); 
		if (ZT75xx_I2cReadData(ZINITIX_EEPROM_INFO_REG,(uint8 *)&chip_eeprom_info, 2) !=true)
			return false;
		cPrintf("touch eeprom info = ",0);
		cPrintf((uint8*)&chip_eeprom_info,2);
		cPrintf("\r\n",0);
		if (!zinitix_bit_test(chip_eeprom_info, 0))
			break;
		if(time_out++ == 4){
			ZT75xx_I2cWriteCMD(ZINITIX_CALIBRATE_CMD);
			C_DELAY_MS(10);
			ZT75xx_I2cWriteCMD(ZINITIX_CLEAR_INT_STATUS_CMD); 
			cPrintf("h/w calibration retry timeout.\r\n",0);
		}
		if(time_out++ > 10){
			cPrintf("[error] h/w calibration timeout.\r\n",0);
			break; 
		}
	} while (1);
#if 1 
	if (ZT75xx_I2cWriteREG(0x0019, TOUCH_POINT_MODE) != I2C_SUCCESS)/*initial touch mode */
		return false;
#endif
	if (ZT75xx_I2cWriteREG(ZINITIX_TOUCH_MODE, TOUCH_POINT_MODE) != I2C_SUCCESS)
		return false;
	if (ic_int_mask != 0)
	if (ZT75xx_I2cWriteREG(ZINITIX_INT_ENABLE_FLAG,ic_int_mask)!= I2C_SUCCESS)
		return false;
	ZT75xx_I2cWriteREG(0xc003, 0x0001);
	ZT75xx_I2cWriteREG(0xc104, 0x0001);
	DELAY_US(100);
	if (ZT75xx_I2cWriteCMD(ZINITIX_SAVE_CALIBRATION_CMD) != I2C_SUCCESS)
		return false;
	C_DELAY_MS(1000); 
	ZT75xx_I2cWriteREG(0xc003, 0x0000);
	ZT75xx_I2cWriteREG(0xc104, 0x0000);
	return true; 
}
#endif

uint8 ts_upgrade_firmware(uint32 firmware_addr, uint32 size)
{
	uint16 flash_addr;	
	uint8 verify_data[8];
	uint8 retry_cnt = 0;
	uint8 i=0;
	uint8_t	buffer[8];
	uint32    sizeIndex =size/2;
	uint32    sizeNotify =sizeIndex;
	uint8 TpProgress=5;
	retry_upgrade:
	zinitix_power_control(0);
	C_DELAY_MS(CHIP_OFF_DELAY);	
	zinitix_power_control(1);

	C_TP_POWER_HI;
	C_DELAY_MS(2);
	C_TP_POWER_LO;
	C_DELAY_MS(100);
	C_TP_POWER_HI;
	
	C_DELAY_MS(200);
	if (ZT75xx_I2cWriteREG(0xc000, 0x0001) != true){
		cPrintf("power sequence error (vendor cmd enable)\r\n",0);
		goto fail_upgrade;
	}
	DELAY_US(10);
	if (ZT75xx_I2cReadData(0xcc00, (uint8 *)&chip_code, 2) == false) {
		cPrintf("fail to read chip code\n",0);
		goto fail_upgrade;
	}

	DELAY_US(10);
	if (ZT75xx_I2cWriteCMD(0xc004) != I2C_SUCCESS){
		cPrintf("power sequence error (intn clear)\n",0);
		goto fail_upgrade;
	}
	DELAY_US(10);
	if (ZT75xx_I2cWriteREG(0xc002, 0x0001) != I2C_SUCCESS){
		cPrintf("power sequence error (nvm init)\n",0);
		goto fail_upgrade;
	}
	C_DELAY_MS(10);
	cPrintf("init flash\n",0);
	if (ZT75xx_I2cWriteREG(0xc003, 0x0001) != I2C_SUCCESS){
		cPrintf("fail to write nvm vpp on\n",0);
		goto fail_upgrade;
	}
	C_DELAY_MS(1);
	if (ZT75xx_I2cWriteREG(0xc104, 0x0001) != I2C_SUCCESS){
		cPrintf("fail to write nvm wp disable\n",0);
		goto fail_upgrade;
	}
	
	if (ZT75xx_I2cWriteCMD(ZINITIX_INIT_FLASH) != I2C_SUCCESS) {
		cPrintf("failed to init flash\n",0);
		goto fail_upgrade;
	}

	OtaSendCmd(O_PROGRESS,TpProgress);
	TpProgress++;
	C_DELAY_MS(10);

	cPrintf("writing firmware data......\r\n",0);
	Power_Resume();
	for (flash_addr = 0; flash_addr < size; ) 
	{
		for (i = 0; i < 8; i++)
		{
		SpiFlash_Read_TP_Firmware(firmware_addr+flash_addr, buffer, 8);
		if(ts_write_data(ZINITIX_WRITE_FLASH,buffer,TC_SECTOR_SZ)!=TRUE)
			{
				cPrintf("error : write zinitix tc firmare\r\n",0);
				goto fail_upgrade;
			}
			flash_addr += TC_SECTOR_SZ;

		}
		if(flash_addr>=sizeNotify)
		{
			OtaSendCmd(O_PROGRESS,TpProgress);
			TpProgress++;
			sizeNotify= sizeNotify+sizeIndex-8;
		}
		C_DELAY_MS(10); //for fuzing delay
	}
	

	cPrintf("init flash\r\n",0);
	if (ZT75xx_I2cWriteCMD(ZINITIX_INIT_FLASH) != I2C_SUCCESS) {
		cPrintf("error : failed to init flash.\r\n",0);
		goto fail_upgrade;
	}

	cPrintf("read firmware data and verify firmware data......\r\n",0);
	
#if 1
	sizeNotify = sizeIndex;
	for (flash_addr = 0; flash_addr < size; ) {
		for (i = 0; i < 8; i++) {
			if (ts_read_firmware_data(ZINITIX_READ_FLASH,verify_data, TC_SECTOR_SZ) != TRUE) {
				cPrintf("error : read zinitix tc firmare failed.\r\n",0);
				goto fail_upgrade;
			}
			SpiFlash_Read_TP_Firmware(firmware_addr+flash_addr, buffer, 8);
			if (memcmp(buffer, verify_data, 8) != 0) 
			{
				cPrintf("error : verify firmware data failed.\r\n",0);
				goto fail_upgrade;
			}
			flash_addr += TC_SECTOR_SZ;			
		}
		if(flash_addr>=sizeNotify)
		{
			OtaSendCmd(O_PROGRESS,TpProgress);
			TpProgress++;
			sizeNotify= sizeNotify+sizeIndex-8;
		}
		C_DELAY_MS(10); //for fuzing delay
	}
#endif
	Power_Suspend();

	cPrintf("upgrade finished\r\n",0);
	zinitix_power_control(0);
	C_DELAY_MS(CHIP_OFF_DELAY);	
	zinitix_power_control(1);
	C_TP_POWER_HI;
	C_DELAY_MS(2);
	C_TP_POWER_LO;
	C_DELAY_MS(100);
	C_TP_POWER_HI;
	
	C_DELAY_MS(200);
	ts_power_sequence();
	
	return true;

fail_upgrade:
	
	if (retry_cnt++ < ZINITIX_INIT_RETRY_CNT) {
		cPrintf("upgrade fail : so retry... ", 0);
		cPrintf(&retry_cnt, 1);
		cPrintf("/r/n", 0);
		goto retry_upgrade;		
	}
	
	return false;
}
#endif

static bool TpInit(void)
{
	uint16 ChipCode;
	uint8 ChipVal[2];
	uint16 firmware_version;
	uint16 minor_firmware_version;
	//uint16 reg_data_version;
	uint8 i;
	
	ZT75xx_I2cWriteREG(0xC000,0x0001);
	DELAY_US(10);
	ZT75xx_I2cReadData(0xCC00,(uint8 *)&ChipCode,2);
			
	ChipVal[0]=ChipCode&0xFF;			
	ChipVal[1]=ChipCode>>8;
	cPrintf("chip code=", 0);
	cPrintf(&ChipVal[1], 1);
	cPrintf("_", 0);
	cPrintf(&ChipVal[0], 1);		
	cPrintf("\r\n", 0);
			
	DELAY_US(10);
	ZT75xx_I2cWriteCMD(0x0004);
	DELAY_US(10);
	ZT75xx_I2cWriteREG(0xc002, 0x0001);
	C_DELAY_MS(2);
	ZT75xx_I2cWriteREG(0xc001, 0x0001);
	C_DELAY_MS(200);

	for ( i = 0; i < 10; i++) {
 		if (ZT75xx_I2cWriteCMD(ZINITIX_SWRESET_CMD) == 1)
     		break;/*return 0 mean write success then break*/
	}
 	C_DELAY_MS(30);

	/*固件主版本号u16 firmware_version;*/
	ZT75xx_I2cReadData(ZINITIX_FIRMWARE_VERSION,(uint8 *)&firmware_version, 2);
	
    ChipVal[0]=firmware_version&0xFF;			
	ChipVal[1]=firmware_version>>8;
	cPrintf("firmware_version=", 0);
	cPrintf(&ChipVal[1], 1);
	cPrintf("_", 0);
	cPrintf(&ChipVal[0], 1);		
	cPrintf("\r\n", 0);

	/*固件次版本号u16 minor_firmware_version;*/
	ZT75xx_I2cReadData(ZINITIX_MINOR_FW_VERSION,(uint8 *)&minor_firmware_version, 2);

    	ChipVal[0]=minor_firmware_version&0xFF;			
	ChipVal[1]=minor_firmware_version>>8;
	cPrintf("minor_firmware_version=", 0);
	cPrintf(&ChipVal[1], 1);
	cPrintf("_", 0);
	cPrintf(&ChipVal[0], 1);		
	cPrintf("\r\n", 0);

	/*固件寄存器版本号u16 reg_data_version;*/			
	ZT75xx_I2cReadData(ZINITIX_DATA_VERSION_REG,(uint8 *)&reg_data_version, 2) ;

    	ChipVal[0]=reg_data_version&0xFF;			
	ChipVal[1]=reg_data_version>>8;
	cPrintf("reg_data_version=", 0);
	cPrintf(&ChipVal[1], 1);
	cPrintf("_", 0);
	cPrintf(&ChipVal[0], 1);		
	cPrintf("\r\n", 0);
				
	ZT75xx_I2cWriteREG(ZINITIX_INITIAL_TOUCH_MODE, TOUCH_POINT_MODE);

	ZT75xx_I2cWriteREG(ZINITIX_TOUCH_MODE, TOUCH_POINT_MODE);

	ZT75xx_I2cWriteREG(ZINITIX_SUPPORTED_FINGER_NUM,(uint16)MAX_SUPPORTED_FINGER_NUM);

	ZT75xx_I2cWriteREG(ZINITIX_X_RESOLUTION,(uint16)(TPD_RES_MAX_X));

	ZT75xx_I2cWriteREG(ZINITIX_Y_RESOLUTION,(uint16)(TPD_RES_MAX_Y));
	
	//ZT75xx_I2cWriteREG(0x0131, 30); 

	ZT75xx_I2cWriteCMD(ZINITIX_CALIBRATE_CMD);
	
	ZT75xx_I2cWriteREG(ZINITIX_INT_ENABLE_FLAG, 0x000A); 

	  for (i = 0; i < 10; i++) {
	        ZT75xx_I2cWriteCMD(ZINITIX_CLEAR_INT_STATUS_CMD);
	      	DELAY_US(10);
	    }
		
	 CypressI2C_Init = true;

	 return true;
}

void CypressI2CInit(void)
{
	TP_RESUME_FLAG =false;

	C_DELAY_MS(10);
	C_TP_POWER_DIR_OUTPUT;
	C_TP_POWER_HI;
	C_DELAY_MS(10);
	C_TP_POWER_LO;
	C_DELAY_MS(200);
	C_TP_POWER_HI;
	C_DELAY_MS(100);

	TpInit();
}
