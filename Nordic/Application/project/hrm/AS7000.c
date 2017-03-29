

#include "headfile.h"

//Application ID Mode

#define Handoff_Time   35   //35s

uint16 Count_HrGetValue ;
uint16 HR_Data_Old=75,HR_Data_New=75;
//uint16 HR_Data_Old,HR_Data_New;

uint8 HR_Rate_change;
uint8 Handoff_TimeOut;

static uint16 Hr_value_buff[10]={0};
static uint8 Hr_value_valid=0;
static uint8 Allow_alarm_count=0;
static uint8 Hr_value_buff_Full =false;
static uint8 dat_Invalid_count=0;

uint8 AS7000_Data_Buff[5]={0};
int16 ALG_data[32][3]={0};
bool Accelerrometer_position=0;  //0: Internal-Accelerometer; 1:	External-Accelerometer
bool HR_PRO_20=0;
uint8 Acc_SportType=0;
uint32 Acc_EnergySum=0;


typedef enum 
{
	AppID_Reserved=0,
    AppID_HexLOAD,
    AppID_HRM	
}AS7000_APPLICATION_MODE;

uint8 AS7000_ApplicationID_Mode[3]={0,1,2};



void AS7000_PowerON(void)
{
	HRM_PWR_PIN_OUT;
	HRM_PWR_PIN_H;
	for(uint8 i=0;i<10;i++)
	{	delay_10ms;	}
	AS7000_Setmode(AppID_HRM);
	Count_HrGetValue = 0;	
	Handoff_TimeOut = 0;

	Hr_value_valid=0;
	Allow_alarm_count=0;
	Hr_value_buff_Full =false;
	dat_Invalid_count=0;
	isHrAlertChecked = false;
}


void AS7000_PowerOFF(void)
{
	HRM_PWR_PIN_OUT;
	HRM_PWR_PIN_L;

	HRM_Comms_init();
	if(HR_PRO_20)
		nrf_gpio_pin_dir_set(HRM_INT_PIN,NRF_GPIO_PIN_DIR_INPUT);
    delay_10ms;
	HRM_PWR_PIN_SLEEP;
	isHrAlertChecked = false;
}


void AS7000_Setmode(uint8 AppID_Mode)
{
	WriteData_AS7000(AS7000_I2C_SLAVE_ADDRESS, ApplicationID_4 , &AS7000_ApplicationID_Mode[AppID_Mode], 1);
	
}



bool AS7000_GetHeartRateData(uint8 *hr_dat)
{
	bool dat_valid=true;
	uint8 dat_temp=0;
	uint16 AS7000_CURRENT =0;
	uint8  Dat_SignalQuality_21=0;
	uint8 AS7000_18_23_Buff[6];
	uint8 AS7000_SYNC_38_Buff[1];
	static uint8 AllowUpdatebuff=true;
//	static uint8 OverChange=0;
	uint16 Hr_value_change[3]={0};
//	uint16 Hr_value_agv;


	if(HR_PRO_20)
	{
		AS7000_CURRENT = AS7000_Data_Buff[0]*256 + AS7000_Data_Buff[1];
		//Status =	AS7000_Data_Buff[2];
		dat_temp = AS7000_Data_Buff[3];
		Dat_SignalQuality_21 = AS7000_Data_Buff[4];
	}
	else
	{
		
		ReadData_AS7000(AS7000_I2C_SLAVE_ADDRESS, TransactionID_18, AS7000_18_23_Buff, 6);
		ReadData_AS7000(AS7000_I2C_SLAVE_ADDRESS, SYNC_38, AS7000_SYNC_38_Buff, 1);
		
		AS7000_CURRENT = AS7000_18_23_Buff[4]*256 + AS7000_18_23_Buff[5];
		dat_temp = AS7000_18_23_Buff[2];
		Dat_SignalQuality_21 = AS7000_18_23_Buff[3];
	}
	//TRACE("befor:Count =%d,HR =%d,SQ =%d,CURRENT = %d\r\n",Count_HrGetValue,dat_temp,Dat_SignalQuality_21,AS7000_CURRENT);

	Acc_SportType =SportLevelJudge();
	
	if(	(AS7000_CURRENT<= 5)||(AS7000_CURRENT>= 180)||(Dat_SignalQuality_21>12)	)
	{
		
		dat_Invalid_count++;
		if(Hr_value_buff_Full)
		{
			if(dat_Invalid_count>3)
			{
				dat_valid = false;
			}
			else
			{
				if(dat_temp>Hr_value_buff[9] )
					dat_temp = Hr_value_buff[9] + Count_HrGetValue%3;
				else
					dat_temp = Hr_value_buff[9] - Count_HrGetValue%3;
			}
		}
		else
		{

			dat_valid = false;
		}
		

	}
	else
	{
		dat_Invalid_count =0;
	}

	if(dat_temp<53)
	{
		if(Hr_value_buff_Full)
			dat_temp = Hr_value_buff[9] - Count_HrGetValue%3;
		else
			dat_temp = HR_Data_Old;
		if(dat_temp<53)
			dat_temp =53;
		AllowUpdatebuff=false;
	}
	else if(dat_temp>180)
	{
		if(Hr_value_buff_Full)
			dat_temp = Hr_value_buff[9] + Count_HrGetValue%3;
		else
			dat_temp = HR_Data_Old;
		if(dat_temp>180)
			dat_temp =180;		
		AllowUpdatebuff=false;
	}

	if(	++Count_HrGetValue <=10)
	{					
		dat_valid = false;
		if(dat_temp >=60 || dat_temp <=90)
		{ 
			HR_Data_Old = dat_temp;
			HR_Data_New = dat_temp;
		}
	}
	else if((Count_HrGetValue>10)&&(dat_valid ==true))
	{
			
		if(Hr_value_buff_Full)
		{				
			//变化率过大，降低变化率
			for(uint8 i=0;i<3;i++)
			{
				if(dat_temp>Hr_value_buff[9-i])
				{	Hr_value_change[i] = dat_temp - Hr_value_buff[9-i];	}
				else {	Hr_value_change[i] = Hr_value_buff[9-i] - dat_temp;	}
				Hr_value_change[i] = (Hr_value_change[i]*100)/Hr_value_buff[9-i];
				
				//TRACE("change[%d]=%d,",i,Hr_value_change[i]);
			}
			//TRACE("\r\n");
			if(Hr_value_change[0]>35)
			{
				
				if(dat_temp>Hr_value_buff[9] )
					dat_temp = Hr_value_buff[9] + Hr_value_buff[9]*30/10/100;
				else
					dat_temp = Hr_value_buff[9] - Hr_value_buff[9]*30/10/100;
				AllowUpdatebuff=true;

			}
			else if(Hr_value_change[0]>30)	//变化率过大，舍弃去最后一个buff的值
			{
				//OverChange++;//TRACE("OverChange=%d\r\n",OverChange);
					if(dat_temp>Hr_value_buff[9] )
						dat_temp = Hr_value_buff[9] + Hr_value_buff[9]*Hr_value_change[0]/10/100;//Count_HrGetValue%3 - Count_HrGetValue%2;//2*OverChange;
					else
						dat_temp = Hr_value_buff[9] - Hr_value_buff[9]*Hr_value_change[0]/10/100;//Count_HrGetValue%3 - Count_HrGetValue%2;//2*OverChange;
					AllowUpdatebuff=true;
			}
			else if(Hr_value_change[0]>25)  //变化率过大，取平均值
			{				
				if(dat_temp>Hr_value_buff[9] )
					dat_temp = Hr_value_buff[9] + Hr_value_buff[9]*Hr_value_change[0]/10/100;//Count_HrGetValue%3 - Count_HrGetValue%2;//2*OverChange;
				else
					dat_temp = Hr_value_buff[9] - Hr_value_buff[9]*Hr_value_change[0]/10/100;//Count_HrGetValue%3 - Count_HrGetValue%2;//2*OverChange;
				AllowUpdatebuff=true;				
			}
			else if(Hr_value_change[0]>20)
			{
						
				if(dat_temp>Hr_value_buff[9] )
					dat_temp = Hr_value_buff[9] + Hr_value_buff[9]*Hr_value_change[0]/10/100;//Count_HrGetValue%3 - Count_HrGetValue%2;//2*OverChange;
				else
					dat_temp = Hr_value_buff[9] - Hr_value_buff[9]*Hr_value_change[0]/10/100;//Count_HrGetValue%3 - Count_HrGetValue%2;//2*OverChange;
				AllowUpdatebuff=true;

			}
			else if(Hr_value_change[0]>15) 
			{
				if(dat_temp>Hr_value_buff[9] )
					dat_temp = Hr_value_buff[9] + Hr_value_buff[9]*Hr_value_change[0]/10/100;//Count_HrGetValue%3 - Count_HrGetValue%2;//2*OverChange;
				else
					dat_temp = Hr_value_buff[9] - Hr_value_buff[9]*Hr_value_change[0]/10/100;//Count_HrGetValue%3 - Count_HrGetValue%2;//2*OverChange;
				
				AllowUpdatebuff=true;

			}
			else  if((Hr_value_change[0]>10)||(Hr_value_change[1]>15)||(Hr_value_change[2]>20))
			{
								
				if(dat_temp>Hr_value_buff[9] )
					dat_temp = Hr_value_buff[9] + Hr_value_buff[9]*Hr_value_change[0]/5/100;//Count_HrGetValue%3 - Count_HrGetValue%2;//2*OverChange;
				else
					dat_temp = Hr_value_buff[9] - Hr_value_buff[9]*Hr_value_change[0]/5/100;//Count_HrGetValue%3 - Count_HrGetValue%2;//2*OverChange;
				AllowUpdatebuff=true;

			}
			else  if((Hr_value_change[0]>4)||(Hr_value_change[1]>6)||(Hr_value_change[2]>10))
			{
			
				if(dat_temp>Hr_value_buff[9] )
					dat_temp = Hr_value_buff[9] + Hr_value_buff[9]*Hr_value_change[0]/2/100;//Count_HrGetValue%3 - Count_HrGetValue%2;//2*OverChange;
				else
					dat_temp = Hr_value_buff[9] - Hr_value_buff[9]*Hr_value_change[0]/2/100;//Count_HrGetValue%3 - Count_HrGetValue%2;//2*OverChange;
				AllowUpdatebuff=true;

			}

			if((Hr_value_change[0]==0)&&(Hr_value_change[1]==0)&&(Hr_value_change[2]==0))  //心率值需要有微小变化
			{
				dat_temp = Hr_value_buff[9] + Count_HrGetValue%3 - Count_HrGetValue%2;
				AllowUpdatebuff=false;
			}
			
			
		//TRACE("beforealarm:Count =%d,HR =%d,AlaLower =%d,AlaUpper =%d\r\n",Count_HrGetValue-1,dat_temp,hr_alarm.HeartRateAlaLower,hr_alarm.HeartRateAlaUpper);
		
			//报警值出现5次内做处理
			if( ( dat_temp <=hr_alarm.HeartRateAlaLower)&&(Allow_alarm_count<5))
			{
				dat_temp = hr_alarm.HeartRateAlaLower + 2 + Count_HrGetValue%4 - Allow_alarm_count%2;
				Allow_alarm_count++;
			}
			else if( ( dat_temp >=hr_alarm.HeartRateAlaUpper)&&(Allow_alarm_count<5))
			{
				dat_temp = hr_alarm.HeartRateAlaUpper - 2 - Count_HrGetValue%4 + Allow_alarm_count%2;
				Allow_alarm_count++;
			}
			else if(dat_temp >hr_alarm.HeartRateAlaLower && dat_temp <hr_alarm.HeartRateAlaUpper)
			{
				Allow_alarm_count =0;
			}
		
		//TRACE("afteralarm:Count =%d,HR =%d,Allow_alarm_count = %d\r\n",Count_HrGetValue-1,dat_temp,Allow_alarm_count);
		}
		else
		{
			HR_Data_New = dat_temp;
			if( HR_Data_New>=HR_Data_Old )
			{	Hr_value_change[0] = (HR_Data_New - HR_Data_Old)*100/HR_Data_Old;	}
			else
			{	Hr_value_change[0] = (HR_Data_Old - HR_Data_New)*100/HR_Data_Old;	}
			
			if(Hr_value_change[0] >= 10) //心率前后变化超过10%，舍弃
			{	
				dat_temp = HR_Data_Old;	
				AllowUpdatebuff=false;
			}
			else
				AllowUpdatebuff=true;

		}
			
	}
	
	if(Count_HrGetValue >=3600 ) 
	{
		Count_HrGetValue = 11;
	}
	
	if(dat_temp<53)
	{
		dat_temp =53 + Count_HrGetValue%3 - Count_HrGetValue%2;
	}
	else if(dat_temp>180)
	{
		dat_temp =180 + Count_HrGetValue%3 - Count_HrGetValue%2;
	} 
	
	if(dat_valid==true)
	{ 
		*hr_dat = dat_temp;			
		Handoff_TimeOut =0;
		
		//生成10个心率值的buff	
		if((Hr_value_valid<10)&&(Hr_value_buff_Full==false))
		{
			Hr_value_buff[Hr_value_valid]=dat_temp;
			Hr_value_valid++;
			if(Hr_value_valid>=10)
				Hr_value_buff_Full=true;	
		}
		//取得有效值，更新buff内容
		if(Hr_value_buff_Full && AllowUpdatebuff )
		{
			for(uint8 i=0;i<9;i++)
			{
				Hr_value_buff[i]=Hr_value_buff[i+1];	
			}
			Hr_value_buff[9]= dat_temp;
		}
		else if(!Hr_value_buff_Full)
		{
			HR_Data_Old =HR_Data_New;
		}
	}
	else
	{	
		Handoff_TimeOut++;
		if(Handoff_TimeOut >= Handoff_Time)
		{ 
			osal_start_timerEx(GetHrTaskId(), HR_CLOSE_EVENT, 1000);
			Handoff_TimeOut =0;
		}		
	}
	
	
	//TRACE("after:Count =%d,HR =%d,SQ =%d,CURRENT = %d\r\n",Count_HrGetValue-1,dat_temp,Dat_SignalQuality_21,AS7000_CURRENT);
	return dat_valid;

}

//return true if good wear
bool AS7000_GetHeartRateData_One(uint8 *hr_dat)
{
	bool dat_valid=true;
	uint8 dat_temp=0;
	uint16 AS7000_CURRENT =0;
	uint8  Dat_SignalQuality_21=0;
	uint8 AS7000_18_23_Buff[6];
	uint8 AS7000_SYNC_38_Buff[1];

	static uint16 Hr_value_agv=0;

	
	if(HR_PRO_20)
	{
		AS7000_CURRENT = AS7000_Data_Buff[0]*256 + AS7000_Data_Buff[1];
		//Status =	AS7000_Data_Buff[2];
		dat_temp = AS7000_Data_Buff[3];
		Dat_SignalQuality_21 = AS7000_Data_Buff[4];
	}
	else
	{
		
		ReadData_AS7000(AS7000_I2C_SLAVE_ADDRESS, TransactionID_18, AS7000_18_23_Buff, 6);
		ReadData_AS7000(AS7000_I2C_SLAVE_ADDRESS, SYNC_38, AS7000_SYNC_38_Buff, 1);
		
		AS7000_CURRENT = AS7000_18_23_Buff[4]*256 + AS7000_18_23_Buff[5];
		dat_temp = AS7000_18_23_Buff[2];
		Dat_SignalQuality_21 = AS7000_18_23_Buff[3];
	}
	//TRACE("befor:Count =%d,HR =%d,SQ =%d,CURRENT = %d\r\n",Count_HrGetValue,dat_temp,Dat_SignalQuality_21,AS7000_CURRENT);
		
	if(	(AS7000_CURRENT<= 5)||(AS7000_CURRENT>= 180)||(Dat_SignalQuality_21>12)	)
	{

		if(Hr_value_buff_Full)
		{	
			if(++dat_Invalid_count>3)
			{
				dat_valid = false;
			}
			else
			{
				if(dat_temp>Hr_value_buff[9] )
					dat_temp = Hr_value_buff[9] + Count_HrGetValue%3;
				else
					dat_temp = Hr_value_buff[9] - Count_HrGetValue%3;
			}
		}
		else
		{
				dat_valid = false;
		}
		
	}
	else
	{
		dat_Invalid_count =0;
	}

	if(dat_temp<53)
	{
		if(Hr_value_buff_Full)
			dat_temp = Hr_value_buff[9] - Count_HrGetValue%3;
		else
			dat_temp = HR_Data_Old;
		if(dat_temp<53)
			dat_temp =53;
		//AllowUpdatebuff=false;
	}
	else if(dat_temp>180)
	{
		if(Hr_value_buff_Full)
			dat_temp = Hr_value_buff[9] + Count_HrGetValue%3;
		else
			dat_temp = HR_Data_Old;
		if(dat_temp>180)
			dat_temp =180;		
		//AllowUpdatebuff=false;
	}
	
	if(	Count_HrGetValue<=15)
	{					
		Hr_value_agv=0;
		if(Count_HrGetValue<14)
			dat_valid = false;		
		//TRACE("Count_HrGetValue = %d,dat_valid =%d,dat_temp =%d\r\n",Count_HrGetValue,dat_valid,dat_temp);
		if(Count_HrGetValue>=10)
		{
			if((Hr_value_valid<5)&&(Hr_value_buff_Full==false))
			{
				Hr_value_buff[Hr_value_valid]=dat_temp;
				//TRACE("Count_HrGetValue = %d,Hr_value_buff[%d] = %d\r\n",Count_HrGetValue,Hr_value_valid,dat_temp);
				Hr_value_valid++;
				if(Hr_value_valid>=5)
				{
					Hr_value_buff_Full=true;

					//数组排序排序
					uint8 i,j,t;
					for(j=0;j<5;j++) 
					    for(i=0;i<5-1-j;i++)
					    if(Hr_value_buff[i]>Hr_value_buff[i+1]) /* 由小到大,由大到小时改为< */
					    {
					        t=Hr_value_buff[i];
					        Hr_value_buff[i]=Hr_value_buff[i+1];
					        Hr_value_buff[i+1]=t;
					    }
					
					for(i=1;i<4;i++)
					{
						Hr_value_agv += Hr_value_buff[i];
					}		
					//dat_valid=true;
					*hr_dat=Hr_value_agv/3;
					//TRACE("Hr_value_agv/5 = %d\r\n",Hr_value_agv/3);
					osal_start_timerEx(GetHrTaskId(), HR_CLOSE_EVENT, 1000);
				}
				
			}
		}
		Count_HrGetValue++;
	}
	
	
	//TRACE("after:Count =%d,HR =%d,SQ =%d,CURRENT = %d\r\n",Count_HrGetValue,dat_temp,Dat_SignalQuality_21,AS7000_CURRENT);
	return dat_valid;
}




bool ReadData_AS7000(uint8 device_addr, uint8 register_addr, uint8 * data, uint8 length)
{

	if(HRM_I2cMultipleByteRead(device_addr<<1,register_addr,data,length)==false)
		return false;
	return true;
}

bool WriteData_AS7000(uint8 device_addr, uint8 register_addr, uint8 * data, uint8 length)
{

	if(HRM_I2cMultipleByteWrite(device_addr<<1,register_addr,data,length)==false)
		return false;
	return true;
}



void AS7000DeviceSettings(void)
{
	AS7000_PowerOFF();
}


void AS7000_PowerON_ExtALG(void)
{
	uint8 ACC_SAM_FRE_buff[2]={0};
	uint8 Host_One_Sec_Time_buf[2]={0};
	//uint8 Host_One_Sec_buff[2]={0};
	uint8 ApplicationID_Mode[1] = {2};
	uint8 Activity_selection[1] = {1};
	uint8 PRO_20_SYNC_buff[1]={0}; 
	uint32 retry_set =100;
	uint32 retry =0;
		
	ACC_SAM_FRE_buff[0]= ACC_SAM_FRE_MSB;
	ACC_SAM_FRE_buff[1]= ACC_SAM_FRE_LSB;
	Host_One_Sec_Time_buf[0]= Host_One_Sec_MSB;
	Host_One_Sec_Time_buf[1]= Host_One_Sec_LSB;
	
	HRM_PWR_PIN_OUT;
	HRM_PWR_PIN_H;	
	
	for(uint8 i=0;i<10;i++)
	{	delay_10ms;	}
	
	nrf_gpio_cfg_sense_input(HRM_INT_PIN, NRF_GPIO_PIN_NOPULL, NRF_GPIO_PIN_SENSE_LOW);
	delay_10ms;
	retry =retry_set;
	do
	{	
		if(!nrf_gpio_pin_read(HRM_INT_PIN))
		{
			if(!nrf_gpio_pin_read(HRM_INT_PIN))
			{	
				
				WriteData_AS7000(AS7000_I2C_SLAVE_ADDRESS, PRO_20_ApplicationID , &ApplicationID_Mode[0], 1);
				delay_1ms;
				WriteData_AS7000(AS7000_I2C_SLAVE_ADDRESS, PRO_20_ACC_SAM_FRE_ADDR , &ACC_SAM_FRE_buff[0], 2);
				delay_1ms;
				WriteData_AS7000(AS7000_I2C_SLAVE_ADDRESS, PRO_20_Host_One_Sec_Time , &Host_One_Sec_Time_buf[0], 2);
				delay_1ms;
				WriteData_AS7000(AS7000_I2C_SLAVE_ADDRESS, PRO_20_Activity_selection , &Activity_selection[0], 1);
				delay_1ms;
				ReadData_AS7000(AS7000_I2C_SLAVE_ADDRESS, PRO_20_ApplicationID , &ApplicationID_Mode[0], 1);
				delay_1ms;
				ReadData_AS7000(AS7000_I2C_SLAVE_ADDRESS, PRO_20_SYNC , &PRO_20_SYNC_buff[0], 1);
				break;
			}
		}		
							
	}while(retry--);
	Count_HrGetValue = 0;	
	Handoff_TimeOut = 0;

	Hr_value_valid=0;
	Allow_alarm_count=0;
	Hr_value_buff_Full =false;
	dat_Invalid_count=0;
}


void AS7000_SwapData_ExtALG(void)
{
	
	uint8 ALG_DataForAS7000[7]={0};
	uint8 AS7000_SYNC_Buff[1] ={0};
	uint8 Activity_selection[1] = {0}; 
	uint8 Activity_14[1] = {16};
	uint16 ALG_data_to_AS7000[32][3] = {0};
	
	if(Acc_SportType==FIERCE_TYPE)
		Activity_selection[0]=0;
	else
		Activity_selection[0]=1;

	for(uint8 i=0;i<32;i++)
		{
			ALG_data_to_AS7000[i][0] =(ALG_data[i][0]<<5);
			ALG_data_to_AS7000[i][1] =(ALG_data[i][1]<<5);
			ALG_data_to_AS7000[i][2] =(ALG_data[i][2]<<5);			
		}
	WriteData_AS7000(AS7000_I2C_SLAVE_ADDRESS, 14 , &Activity_14[0], 1);
	if(Accelerrometer_position)
	{
		for(uint8 i=0;i<16;i++)
		{
			ALG_DataForAS7000[0]=i+1;
			ALG_DataForAS7000[1]=(uint8)(ALG_data_to_AS7000[i*2][0]>>8);
			ALG_DataForAS7000[2]=(uint8)ALG_data_to_AS7000[i*2][0];
			ALG_DataForAS7000[3]=(uint8)(ALG_data_to_AS7000[i*2][1]>>8);
			ALG_DataForAS7000[4]=(uint8)ALG_data_to_AS7000[i*2][1];
			ALG_DataForAS7000[5]=(uint8)(ALG_data_to_AS7000[i*2][2]>>8);
			ALG_DataForAS7000[6]=(uint8)ALG_data_to_AS7000[i*2][2];
			WriteData_AS7000(AS7000_I2C_SLAVE_ADDRESS, PRO_20_ACC_ADDR , &ALG_DataForAS7000[0],7);
		}
	}
	
	
	WriteData_AS7000(AS7000_I2C_SLAVE_ADDRESS, PRO_20_Activity_selection , &Activity_selection[0], 1);
	ReadData_AS7000(AS7000_I2C_SLAVE_ADDRESS, PRO_20_TransactionID, AS7000_Data_Buff, 5);
	ReadData_AS7000(AS7000_I2C_SLAVE_ADDRESS, PRO_20_SYNC, AS7000_SYNC_Buff, 1);
	
}


bool Get_HR_AS7000_Version(void)
{
	uint8 AS7000_Version_0_3_Buff[4];

	nrf_delay_ms(100);

	AS7000_PowerON();	
	ReadData_AS7000(AS7000_I2C_SLAVE_ADDRESS, Version_0, AS7000_Version_0_3_Buff, 4);
	HR_VER[0] = (AS7000_Version_0_3_Buff[3]>>4)&0x0F;
	HR_VER[1] = AS7000_Version_0_3_Buff[3]&0x0F;

	if((AS7000_Version_0_3_Buff[0]==0x02)&&((AS7000_Version_0_3_Buff[1]&0x3F)==0x00))
		HR_PRO_20 =1;
	if((AS7000_Version_0_3_Buff[1]&0xC0)==0x40)
		Accelerrometer_position =1;	

	//TRACE("Version:::%d,%d,%d,%d\r\n",AS7000_Version_0_3_Buff[0],AS7000_Version_0_3_Buff[1],AS7000_Version_0_3_Buff[2],AS7000_Version_0_3_Buff[3]);
	nrf_delay_ms(100);

	AS7000_PowerOFF();
	
	return true;
}

uint8 SportLevelJudge(void)
{

	uint32 acc_energy[3]={0};
	static uint32 AccEnergyBuf[ACC_ENERGY_BUF_LEN]={0};
	uint32 EnergySum = 0;
	int32  avg[3]={0};
	uint32 sum[3]={0};
	uint8  i=0;	
	uint8 SportType = SLIGHT_TYPE;

	for(i = 0; i < 32; ++i)
	{
		avg[0]+=ALG_data[i][0];
		avg[1]+=ALG_data[i][1];
		avg[2]+=ALG_data[i][2];
	}
	avg[0]=avg[0]>>5;
	avg[1]=avg[1]>>5;
	avg[2]=avg[2]>>5;

	for(i = 0; i < 32; ++i)
	{
		sum[0]+=(	(ALG_data[i][0]-avg[0])*(ALG_data[i][0]-avg[0])	>>3);
		sum[1]+=(	(ALG_data[i][1]-avg[1])*(ALG_data[i][1]-avg[1])	>>3);
		sum[2]+=(	(ALG_data[i][2]-avg[2])*(ALG_data[i][2]-avg[2])	>>3);
	}

	acc_energy[0] = sum[0];
	acc_energy[1] = sum[1];
	acc_energy[2] = sum[2];

	
	for (i = 0; i < ACC_ENERGY_BUF_LEN - 1; ++i)
	{
		AccEnergyBuf[i] = AccEnergyBuf[i + 1];
		EnergySum += AccEnergyBuf[i];
	}
	AccEnergyBuf[ACC_ENERGY_BUF_LEN-1] = acc_energy[0] + acc_energy[1] + acc_energy[2];
	EnergySum += AccEnergyBuf[i];
	
	Acc_EnergySum = EnergySum;


	//TRACE("Acc_EnergySum = %d\r\n",Acc_EnergySum);
	if (EnergySum < 8000)
	{
		SportType = SLIGHT_TYPE;
	}
	else if (EnergySum >= 8000 && EnergySum < 50000)
	{
		SportType = NORMAL_TYPE;
	}
	else
	{
		SportType = FIERCE_TYPE;
	}
	return SportType;
}

