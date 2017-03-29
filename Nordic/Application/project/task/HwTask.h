#include "Typedef.h"

#if 0
/**************L53B PIN Config***************/
#define	KL17_RESET_PIN			0
#define	GPIO_MOTOR_PIN		1
/*PIN2使用为AIN3功能, 连接BAT_AD,测量电压值*/
#define	USB_PLUG_IN_PIN		3
#define	KEY_PIN					4				/*Key*/
#define	C_SCL_PIN				5				/*I2C Touch*/
/*PIN6 PIN7悬空*/
/*PIN8 G-sensor中断 没用到*/
/*PIN9 悬空*/
#define	UART_RX_KL17_PIN		10				/*UART Update KL17*/
#define	UART_TX_KL17_PIN		11				/*UART Update KL17*/
#define	KL17_INT_PIN			12
#define	TOUCH_READY_PIN		13
#define	SCL_PIN					14				/*I2C G-Sensor*/		
#define	SDA_PIN				15				/*I2C G-Sensor*/
#define	UART_RX_PIN			16				/*UART */
#define	UART_TX_PIN			17				/*UART */
#define	C_SDA_PIN				18				/*I2C Touch*/
/*PIN19 悬空*/
#define	C_TP_POWER_PIN          	20				/*RST Touch*/
#define	SPI_KL17_CS_PIN		21				/*SPI*/
#define	SPI_KL17_SCK_PIN		22				/*SPI*/
#define	SPI_KL17_MOSI_PIN		23				/*SPI*/
#define	SPI_KL17_MISO_PIN		24				/*SPI*/
#define 	FLASH_CS_PIN			25 				/*SPI Flash*/
/*PIN26 PIN27作为32K晶振输入*/
/*PIN28 悬空*/
#define	FLASH_DO_PIN			29				/*SPI Flash*/
#define	FLASH_DI_PIN			30				/*SPI Flash*/
#define	FLASH_CLK_PIN			31				/*SPI Flash*/

#endif
#if 1
/**************L40 PIN Config***************/
#define	KL17_RESET_PIN			0
#define	GPIO_MOTOR_PIN		1
/*PIN2使用为AIN3功能, 连接BAT_AD,测量电压值*/
#define	USB_PLUG_IN_PIN		3
#define	KEY_PIN					4				/*Key*/
#define	C_SCL_PIN				5				/*I2C Touch*/	
#define	UART_RX_BC5_PIN		6				/*UART BC5*/
#define	UART_TX_BC5_PIN		7				/*UART BC5*/
#define	FLASH_CLK_PIN			8				/*SPI Flash*/
#define	BC5_INT_PIN			9
#define	UART_RX_KL17_PIN		10				/*UART Update KL17*/
#define	UART_TX_KL17_PIN		11				/*UART Update KL17*/
#define	KL17_INT_PIN			12
#define	TOUCH_READY_PIN		13
#define	SCL_PIN					14				/*I2C G-Sensor*/		
#define	SDA_PIN				15				/*I2C G-Sensor*/
#define	FLASH_DO_PIN			16				/*SPI Flash*/
#define	FLASH_DI_PIN			17				/*SPI Flash*/
#define	C_SDA_PIN				18				/*I2C Touch*/
#define	HRM_INT_PIN			19				/*HR*/
#define	C_TP_POWER_PIN          	20				/*RST Touch*/
#define	SPI_KL17_CS_PIN		21				/*SPI*/
#define	SPI_KL17_SCK_PIN		22				/*SPI*/
#define	SPI_KL17_MOSI_PIN		23				/*SPI*/
#define	SPI_KL17_MISO_PIN		24				/*SPI*/
#define 	FLASH_CS_PIN			25 				/*SPI Flash*/
/*PIN26 PIN27作为32K晶振输入*/
#define	BC5_POWER_PIN			28
#define	HRM_PWR_PIN			29				/*HR*/
#define	HRM_SDA_PIN			30				/*HR*/
#define	HRM_SCL_PIN			31				/*HR*/
/*****************复用引脚******************/
#define	UART_RX_PIN			10				/*UART */
#define	UART_TX_PIN			11				/*UART */
/******************************************/
#endif

#if 0
/**************L53A PIN Config***************/
#define	KL17_RESET_PIN			0
#define	GPIO_MOTOR_PIN		1
/*PIN2使用为AIN3功能, 连接BAT_AD,测量电压值*/
#define	USB_PLUG_IN_PIN		3
#define	FLASH_DO_PIN			4				/*SPI Flash*/
#define	C_SCL_PIN				5				/*I2C Touch*/			
#define	FLASH_CLK_PIN			6				/*SPI Flash*/
#define	FLASH_DI_PIN			7				/*SPI Flash*/
/*PIN8并无使用,连接G-Sensor中断*/
#define	FLASH_CS_PIN			9				/*SPI Flash*/ 			
#define	UART_RX_KL17_PIN		10				/*UART Update KL17*/
#define	UART_TX_KL17_PIN		11				/*UART Update KL17*/
#define	KL17_INT_PIN			12
#define	TOUCH_READY_PIN		13
#define	SCL_PIN					14				/*I2C G-Sensor*/		
#define	SDA_PIN				15				/*I2C G-Sensor*/
#define	UART_RX_PIN			16				/*UART */
#define	UART_TX_PIN			17				/*UART */
#define	C_SDA_PIN				18				/*I2C Touch*/
#define	KL17_POWER_PIN		19
#define	C_TP_POWER_PIN          	20				/*RST Touch*/
#define	SPI_KL17_CS_PIN		21				/*SPI*/
#define	SPI_KL17_SCK_PIN		22				/*SPI*/
#define	SPI_KL17_MOSI_PIN		23				/*SPI*/
#define	SPI_KL17_MISO_PIN		24				/*SPI*/
#define	LCD_RST				25				/*LCD*/
/*PIN26 PIN27作为32K晶振输入*/
#define	LCD_SPI_CS				28				/*LCD*/
#define	LCD_SPI_SCL			29				/*LCD*/
#define	LCD_SPI_SI				30				/*LCD*/
#define	LCD_SPI_A0				31				/*LCD*/
/**************************************/
#endif

#define NO_KEY								0x00
#define HAL_KEY_STATE_LONG_5S				0x01
#define HAL_KEY_STATE_SHORT				0x02
#define HAL_KEY_STATE_LONG				0x04
#define HAL_KEY_STATE_HOLD				0x08
#define HAL_KEY_STATE_HOLD_UP				0x10
#define HAL_KEY_STATE_DOUBLE_SHORT_UP	0x20
#define HAL_KEY_STATE_LONG_UP				0x40
#define HAL_KEY_STATE_SHORT_UP			0x80

#define KEY_SCAN_TIMES      			       	0		//按键防抖的扫描次数
#define KEY_SHORT_TIMES     					3		//短按键的次数
#define KEY_LONG_TIMES     					50		//长按键的次数
#define KEY_HOLD_TIMES     					25		//连接的频率次数
#define KEY_LONG_5S_TIMES     				125		//连接的频率次数

#define HAL_KEY_SW_1 						0x01	// key1
#define HAL_KEY_SW_2						0x02	// key1
#define HAL_KEY_SW_3 						0x04	// key1
#define HAL_KEY_SW_4 						0x08	// key1

typedef uint8 BAT_VAULE_E;


#define ADC_BAT_CHANNEL  	ADC_CONFIG_PSEL_AnalogInput3
#define ADC_KEY_CHANNEL  	ADC_CONFIG_PSEL_AnalogInput5
#define ADC_RESOLUTION  	(ADC_CONFIG_RES_10bit)
  
  
#define HW_CHECK_BAT_EVENT   	                        		0x0001
#define BAT_CALCULATE_CAPACITY_EVENT   	        	0x0002
#define BATTERY_VERY_LOW_EVENT                         		0x0004
#define BAT_CHARGE_DISCONNET_EVENT                 		0x0008
#define BAT_CHARGE_CONNET_EVENT   	                	0x0010
#define HW_FACTORY_SETTING_EVENT  	                	0x0020
#define HW_POWER_ON_USB_EVENT	                		0x0040
#define HW_POWER_OFF_USB_EVENT	                		0x0080
#define HW_USB_IN_DEBOUNCE_EVENT                   		0x0100
#define HW_CHECK_CONNECT_USB_EVENT 	        		0x0200
#define HW_MOTOR_REPEAT_WORK_EVENT 	        		0x0400
#define HW_SYSTEM_UPDATE_RESET_EVENT 	        	0x0800
#define PAY_RECORD_UPLOAD_EVENT 	        			0x1000
#define PAY_BALANCES_UPLOAD_EVENT 	        		0x2000
#define PAY_CARDID_UPLOAD_EVENT 	        			0x4000
#define HW_UART0_POLL            						0x8000


typedef enum{
  UPLOAD_RECORD_NO=0,
  UPLOAD_RECORD_PREPARE,
  UPLOAD_RECORD_ING,
  UPLOAD_RECORD_OK,
  UPLOAD_RECORD_FAIL,
}UploadPayRecordStateT;

  
  
extern uint8   BatteryTempPercent;
extern bool    SlideReadyPinNoWait;
extern UploadPayRecordStateT gUploadPayRecordDataState;



uint16 AdcRead(uint8 channel);
void GetBatVelue(void);
void BatteryAdcMeasure(void); 
void HwInitTask(uint8 task_id);
uint8 GetHwTaskId(void);
uint16 HwEventHandler(uint8 task_id, uint16 events);
void Bc5PowerPinInit(void);
void SetBc5Power(bool is_on);
void BtPowerEn(bool enable);
bool BatChargeInit(void);
BAT_VAULE_E BatCheck(void);
void ResetBoard(void);
void DisableScanKeyPinInt(void);
void EnableScanKeyPinInt(void);
void SetUsbConnctedEvent(void);
void SetUsbDisconnctedEvent(void);
void StartScanKeyToWakeUp(void);
void StopScanKeyToSleep(void);
void CheckUsbInDebounce(void);
bool CheckChargerConnectEvent(void);
void EnableUsbPinInt(uint8 Edge);
void DisableUsbPinInt(void);
void SetBatteryChargeEvent(void);
void CheckPowerOnUsbStateEvent(void);
void CheckPowerOffUsbStateEvent(void);
void SendChargeMessage(void);
void EnablePowerKeyInt(void);
void DisablePowerKeyInt(void);
void StartKeyScan(void);
void StopKeyScan(void);
void SetFactorySettingEvent(void);
void StopTPInt(void);
void StartTPInt(void);
void Gpio_Interrupt_Initial(void);
void ADC_disable(void);
bool CheckTouchKeyEvent(void);
bool CheckCypressEvent(void);
void DisableReset(void);
void DisableKl17(void);
void GPIOTE_IRQ_TOUCH_Recovery(void);
void GPIOTE_IRQ_USB_Recovery(void);
