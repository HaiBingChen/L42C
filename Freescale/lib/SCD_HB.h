#ifndef _SCD_HB_H
  #define _SCD_HB_H
	
	
	
	#define u1 unsigned char
	#define u2 unsigned int
	#define u4 unsigned long
	

	
	
	
	void SCD_DelayUs(u2 n);//n us
	void SCD_DelayMs(u2 n);//n ms

/******************************************
 函数名称：
		SCD_POWER_ON
 功能说明：
		给卡片上电，获取卡片ATR值、长度、是否使能PPS机制
 参数说明：
		atrbuf:卡片ATR值
		atrlen:卡片ATR值长度
		atrpps：是否使能pps机制 0-使能 1-不使能
		mode:是否获取ATR 0-获取 1-不获取
 返回值：
		0：获取ATR成功
		1：获取ATR失败 
 ******************************************/
u1 SCD_POWER_ON(u1 *atrbuf,u1 *atrlen,u1 atrpps,u1 mode);


/******************************************
 函数名称：
		SCD_POWER_OFF
 功能说明：
		给卡片下电
 参数说明：
		无
 返回值：
		0：下电成功
		1：下电失败 
 ******************************************/
u1 SCD_POWER_OFF(void);


/******************************************
 函数名称：
		SCD_POWER_APDU
 功能说明：
		读卡器与卡片APDU指令交互
 参数说明：
		txbuf：读卡器需要发送的指令缓冲区
		txlen：读卡器需要发送的指令缓冲区长度
		rxbuf：卡片返回数据的缓冲区
		rxlen：卡片返回数据的缓冲区长度
 返回值：
		0：处理成功
		1：处理失败 
 ******************************************/
u1 SCD_APDU_CMD(u1 *txbuf,u2 txlen,u1 *rxbuf,u2 *rxlen);

/******************************************
 函数名称：
		SCD_APDU_CMD_Test
 功能说明：
		读卡器与卡片APDU指令交互 测试指令
 参数说明：
		txbuf：读卡器需要发送的指令缓冲区
		txlen：读卡器需要发送的指令缓冲区长度
		rxbuf：卡片返回数据的缓冲区
		rxlen：卡片返回数据的缓冲区长度
 返回值：
		0：处理成功
		1：处理失败 
 ******************************************/
u1 SCD_APDU_CMD_Test(void);

u1 SCD_APDU_LYCMD(u1 *txbuf, u2 txlen, u1 *rxbuf, u2 *rxlen);
#endif
