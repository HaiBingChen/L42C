#ifndef _AS7000_H
#define _AS7000_H


//#define HRM_PWR_PIN   24

#define HRM_PWR_PIN_OUT          nrf_gpio_cfg_output(HRM_PWR_PIN)
#define HRM_PWR_PIN_H            nrf_gpio_pin_set(HRM_PWR_PIN)
#define HRM_PWR_PIN_L            nrf_gpio_pin_clear(HRM_PWR_PIN)
#define HRM_PWR_PIN_SLEEP        nrf_gpio_cfg_input(HRM_PWR_PIN,NRF_GPIO_PIN_PULLDOWN)

//#define HRM_INT_PIN  12
#define HRM_INT_DIR_INPUT           nrf_gpio_pin_dir_set(HRM_INT_PIN, NRF_GPIO_PIN_DIR_INPUT)
#define HRM_INT_DIR_OUTPUT          nrf_gpio_pin_dir_set(HRM_INT_PIN, NRF_GPIO_PIN_DIR_OUTPUT)
#define HRM_INT_IN                   nrf_gpio_pin_read(HRM_INT_PIN)
#define HRM_INT_HI                   nrf_gpio_pin_set(HRM_INT_PIN)
#define HRM_INT_LO                   nrf_gpio_pin_clear(HRM_INT_PIN)


// AS7000 I2C DEFAULT SLAVE ADDRESS

#define AS7000_I2C_SLAVE_ADDRESS 0x30


/*********************** AS7000 REGISTERS *************************************/

#define AS7000_DEVICE_INFO     0x00
/*
#define Major_Number_Protocol_Version_0     0
#define Minor_Number_Protocol_Version_1     1
#define Major_Number_SW_Version_2          	2
#define Minor_Number_SW_Version_3      		3
#define ApplicationID_4         				0x04
#define HW_Version_5         					5
*/

#define Version_0     0x00
/*
#define Version_1     0x01
#define Version_2     0x02
#define Version_3     0x03
*/
#define ApplicationID_4        0x04

/*
#define TransactionID_8       8
#define Status_9               9
#define HeartRate_10          10
#define SignalQuality_11      11
#define CurrentMSB_12         12
#define CurrentLSB_13         13
#define SYNC_14                14

#define RAW_VALUE_OFE_MSB_24      24
#define RAW_VALUE_OFE_LSB_25      25
#define RAW_VALUE_TIA_MSB_26      26
#define RAW_VALUE_TIA_LSB_27      27
*/

#define TransactionID_18      	18

/*
#define Status_19             		19
#define HeartRate_20           	20
#define SignalQuality_21      	21
#define CurrentMSB_22             22
#define CurrentLSB_23           	23

#define X_AXIS_MSB_28         28
#define X_AXIS_LSB_29      	29
#define Y_AXIS_MSB_30         30
#define Y_AXIS_LSB_31      	31
#define Z_AXIS_MSB_32         32
#define Z_AXIS_LSB_33      	33
*/

/*
#define GSR_MSB_34      34
#define GSR_LSB_35      35
#define GSR_MSB_36      36
#define GSR_LSB_37      37
*/
#define SYNC_38         38

/////////////////////////////////////////////////////////////
#define PRO_20_Version_0     		0
#define PRO_20_ApplicationID		4
#define PRO_20_ACC_SAM_FRE_ADDR		8
#define PRO_20_Host_One_Sec_Time	10
#define PRO_20_ACC_ADDR		 		20
#define PRO_20_Minimum_Heartrate	39
#define PRO_20_Maximum_Heartrate	40
#define PRO_20_Activity_selection	41
#define PRO_20_TransactionID		52
#define PRO_20_SYNC					57

#define ACC_SAM_FRE   16000			//10Hz = 0.001Hz * 10000
#define ACC_SAM_FRE_MSB	    (uint8)(ACC_SAM_FRE>>8)
#define ACC_SAM_FRE_LSB	    (uint8)ACC_SAM_FRE

#define Host_One_Sec_Time	11000 		//1s = 0.1ms * 10000
#define Host_One_Sec_MSB	    (uint8)(Host_One_Sec_Time>>8)
#define Host_One_Sec_LSB	    (uint8)Host_One_Sec_Time


extern int16	ALG_data[32][3];
extern bool Accelerrometer_position;
extern bool HR_PRO_20;
extern uint8 Acc_SportType;
extern uint32 Acc_EnergySum;



void AS7000_PowerON(void);
void AS7000_PowerOFF(void);
void AS7000_Setmode(uint8 AppID_Mode);

bool AS7000_GetHeartRateData(uint8 *hr_dat);
bool AS7000_GetHeartRateData_One(uint8 *hr_dat);
bool ReadData_AS7000(uint8 device_addr, uint8 register_addr, uint8 * data, uint8 length);
bool WriteData_AS7000(uint8 device_addr, uint8 register_addr, uint8 * data, uint8 length);
void AS7000DeviceSettings(void);



void AS7000_PowerON_ExtALG(void);
//void AS7000_PowerOFF_ExtALG(void);
void AS7000_SwapData_ExtALG(void);

bool Get_HR_AS7000_Version(void);


#define ACC_ENERGY_BUF_LEN 5
#define SLIGHT_TYPE 0
#define NORMAL_TYPE 1
#define FIERCE_TYPE 2
uint8 SportLevelJudge(void);



#endif //_AS7000_H

