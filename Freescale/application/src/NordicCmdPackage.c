#include <string.h>
#include "7816.h"
#include "spi.h"
#include "SCD_HB.h"
#include "fsl_debug_console.h"
#include "booster.h"
#include "led.h"
#include "timer.h"
#include "NordicCmdPackage.h"
#include "seeed_lcd.h"
#include "ExtSpiFlash.h"
#include "display_font.h"
#include "display_ancs.h"
#include "pin_mux.h"
#include "Ui.h"
#include "tpm_timer.h"

extern uint8_t fireware_version[2];

DISPLAY_STATUS_T gDisplayStatus;
UTCTimeStruct gDisplayTime;
UTCTimeStruct DisplayTime;
Format gFormat;
uint8 power_state = 0;
log_data_t log_data;
LanguageType LanNow=LAN_ENGLISH;
uint8 PowerOffKey =0;

uint8 DisOption;
uint8 DisState;
uint8 TimerMenu_Index;
uint32 Value;


extern void SuspendDMA(void);
extern void ReumeDMA(void);

static const NORDIC_DATA_CMD_T NordicDataCmdTable[]={
	{0x01,	GetKl17Version},
	{0x02,	LEDLight},
	{0x03,	NordicPM},
	{0x04,	Set_OTALanguageInfo},
	{0x05,	PowerOffSystem},
	{0x06,	ResumeBacklight},
	
	{0x20,	Flash_Write},
	{0x21,	Flash_Read},
	{0x22,	Flash_Erase},
	{0x23,	Flash_Set_Access}, 
	{0x24,	Flash_Display_Ancs_LayOut},
	{0x25,	Flash_Display_PhBook_Inf},
	{0x26,	Flash_Display_PhonePrepare},
	
	{0x50,	LCD_UpdateStatus},
	{0x51,	LCD_UpdateTime},
	{0x52,	LCD_UpdateTimeFormat},
	{0x53,	LCD_MainDataInterface},
	{0x54,	LCD_MusicInterface},
	{0x55,	LCD_Picturenterface},
	{0x56,	LCD_PhoneInterface},
	{0x57,	LCD_MsgInterface},
	{0x58,	LCD_MsgSecInterface},
	{0x59,	LCD_DevCommInterface},	
	{0x5A,	LCD_StopWatchInterface},
	{0x5B,	LCD_VoIumeInterface},
	{0x5C,	LCD_HeartRateInterface},
	{0x5D,	LCD_CameraAndSearchInterface},
	
	{0xA0,	NFC_Action},
	{0xA1,	APDU_PowerOn},		
	{0xA2,	APDU_PowerOff},
	{0xA3,	APDU_GetStatus},
	{0xA4,	APDU_TxData},
	{0xA5,	APDU_BlockUnblock},
	{0xA6,	APDU_ReadLog},
	{0xA7,	BoosterSet},
	{0xA8,	BoosterGet}
};

const uint16_t NordicDataCmdTableLen=sizeof(NordicDataCmdTable)/sizeof(NORDIC_DATA_CMD_T);

uint8_t NordicDataParseHandler(uint8_t cmd, uint8_t* pdata, uint16_t plen)
{
	if(PowerOffKey==1)
		return F_RSP_PARSE_ERROR;
	if(plen> SPI_COMMAND_PAYLOAD_MAX_SIZE) {
		return Spi_Send_MSG(cmd,CCID_ERR_BLK_ERR_CCID_LEN,0,0);
	}	
	for(uint16_t i=0;i<NordicDataCmdTableLen;i++)
	{
		if(cmd == NordicDataCmdTable[i].DataCmd)
		{
			return NordicDataCmdTable[i].pFn(cmd,pdata,plen);
		}
	}
	return F_RSP_PARSE_ERROR;
}

uint16_t uint16_decode(uint8_t * p_encoded_data)
{
        return ( (((uint16_t)((uint8_t *)p_encoded_data)[0])) | 
                 (((uint16_t)((uint8_t *)p_encoded_data)[1]) << 8 ));
}

static uint32_t uint32_decode(uint8_t * p_encoded_data)
{
    return ( (((uint32_t)((uint8_t *)p_encoded_data)[0]) << 0)  |
             (((uint32_t)((uint8_t *)p_encoded_data)[1]) << 8)  |
             (((uint32_t)((uint8_t *)p_encoded_data)[2]) << 16) |
             (((uint32_t)((uint8_t *)p_encoded_data)[3]) << 24 ));
}


uint8_t APDU_PowerOn(uint8_t cmd, uint8_t *pdata,uint16_t plen)
{
	uint8_t p_buf[SPI_COMMAND_PAYLOAD_MAX_SIZE] = {0};
	uint8_t status = 0;
	#if EMV_TEST
	if (booster_state.mode == BOOSTER_IDLE_MODE) {
		BoosterContactMode();
	}
	#else
	if ((booster_state.mode == BOOSTER_IDLE_MODE) || (NFC_Field_FLAG == 0)) {
		BoosterIdleMode();
		BoosterContactMode();
	}
	#endif
	else if ((booster_state.mode == BOOSTER_CONTACTLESS_IF_MODE) || (booster_state.mode == BOOSTER_CONTACTLESS_OF_MODE)) {
		PRINTF("SE BUSY\r\n");
		return Spi_Send_MSG(cmd,CCID_ERR_CMD_SLOT_BUSY,0,0);
	}
	PRINTF("POWER ON\r\n");
	status = SCD_POWER_ON(p_buf, (uint8_t *)(&plen),1,0); 
	if (!status) {
		m_spi_ctx.status = CCID_SE_PRESENT_AND_ACTIVE;
	} else {
		PRINTF("SE POWER ON FAIL\r\n");
		m_spi_ctx.status = CCID_SE_PRESENT_AND_INACTIVE;		
	}

	if (m_spi_ctx.status == CCID_SE_PRESENT_AND_ACTIVE) {
		return Spi_Send_MSG(cmd,SPI_RSP_DATA,p_buf,plen);
	}	
	else if (m_spi_ctx.status == CCID_SE_PRESENT_AND_INACTIVE) {
		return Spi_Send_MSG(cmd,CCID_STATUS,0,0);
	}


	return F_RSP_NULL;
}

uint8_t APDU_PowerOff(uint8_t cmd, uint8_t *pdata,uint16_t plen)
{
	uint8_t status = 0;
	if ((booster_state.mode == BOOSTER_CONTACTLESS_IF_MODE) || (booster_state.mode == BOOSTER_CONTACTLESS_OF_MODE)) {
		return Spi_Send_MSG(cmd,CCID_ERR_CMD_SLOT_BUSY,0,0);
	}
	m_spi_ctx.status = CCID_SE_PRESENT_AND_INACTIVE;
	PRINTF("POWER OFF\r\n");
	status = SCD_POWER_OFF();
	if (!status) {
		m_spi_ctx.status = CCID_SE_PRESENT_AND_INACTIVE;
	} else {
		PRINTF("SE POWER OFF FAIL\r\n");					
	}
	if (m_spi_ctx.status == CCID_SE_PRESENT_AND_INACTIVE) {
		BoosterIdleMode();
		#if (EMV_TEST == 0)
		BoosterContactlessMode();
		#endif
		m_spi_ctx.status = CCID_SE_PRESENT_AND_INACTIVE;
		return Spi_Send_MSG(cmd,CCID_STATUS,0,0);
	}
	else if (m_spi_ctx.status == CCID_SE_PRESENT_AND_ACTIVE) {
		return Spi_Send_MSG(cmd,CCID_STATUS,0,0);
	}
	return F_RSP_NULL;
}


uint8_t APDU_GetStatus(uint8_t cmd, uint8_t *pdata,uint16_t plen)
{
	return Spi_Send_MSG(cmd,CCID_STATUS,0,0);
}

uint8_t APDU_TxData(uint8_t cmd, uint8_t *pdata,uint16_t plen)
{
	uint8_t status = 0;
	uint8_t p_buf[SPI_COMMAND_PAYLOAD_MAX_SIZE] = {0};
	uint32_t len = 0;
	if(m_spi_ctx.status != CCID_SE_PRESENT_AND_ACTIVE) {
		return Spi_Send_MSG(cmd,CCID_ERR_ICC_MUTE,0,0);
	}
	if ((booster_state.mode == BOOSTER_CONTACTLESS_IF_MODE) || (booster_state.mode == BOOSTER_CONTACTLESS_OF_MODE)) {
		return Spi_Send_MSG(cmd,CCID_ERR_CMD_SLOT_BUSY,0,0);
	}

	PRINTF("APDU\r\n");
	status = SCD_APDU_CMD((u1 *)pdata,(uint32_t )(plen),p_buf,&len);
	if (!status) {
		return Spi_Send_MSG(cmd,SPI_RSP_DATA,p_buf,len);
	} else {
		return Spi_Send_MSG(cmd,CCID_ERR_BLK_NO_BLOCK,0,0);
	}
}

uint8_t NFC_Action(uint8_t cmd, uint8_t *pdata,uint16_t plen)
{

	uint8_t status = 0;
	if (booster_state.block == NFC_BLOCK_STATE) {
		return Spi_Send_MSG(cmd,SPI_RSP_FAILED,0,0);
	}
	memcpy(&status,pdata, 1);
	if (status == NFC_START) {
		Spi_Send_MSG(cmd,SPI_RSP_SUCCED,0,0);
		if (booster_state.mode == BOOSTER_IDLE_MODE) {
			BoosterIrqEnable();
			BoosterIrqClear();
			BoosterContactlessMode();
			booster_state.nfc_state = NFC_ACTIVE_AND_INIT;
		}
		else if (booster_state.mode == BOOSTER_CONTACT_MODE) {
			booster_state.nfc_state = NFC_ACTIVE_AND_UNINIT;
		}
	}
	else if (status == NFC_STOP) {
		Spi_Send_MSG(cmd,SPI_RSP_SUCCED,0,0);
		if (booster_state.nfc_state == NFC_ACTIVE_AND_INIT) {
			switch_idle_mode();	
			if (NFC_INT_FLAG == 1)
				NFC_UPDATE_FLAG = 1; 
		}
	}
	return F_RSP_NULL;
}

uint8_t APDU_BlockUnblock(uint8_t cmd, uint8_t *pdata,uint16_t plen)
{
	if (pdata[0] == BLOCK_APPLICATION) {
		if (booster_state.block == NFC_UNBLOCK_STATE) {
			#if EMV_TEST
			if ((booster_state.mode == BOOSTER_CONTACTLESS_IF_MODE) || ((booster_state.mode == BOOSTER_CONTACTLESS_OF_MODE))) {
				BoosterIrqClear();
				BoosterIrqDisable();
				BoosterIdleMode();
			}
			#else
			if ((booster_state.mode == BOOSTER_CONTACTLESS_IF_MODE) || ((booster_state.mode == BOOSTER_CONTACTLESS_OF_MODE)) || (NFC_Field_FLAG == 1)) {
				BoosterIrqClear();
			}
			#endif
			booster_state.block = NFC_BLOCK_STATE;
		}
	}
	else if (pdata[0] == UNBLOCK_APPLICATION) {
		if (booster_state.block == NFC_BLOCK_STATE) {
			booster_state.block = NFC_UNBLOCK_STATE;
		}
	}
	return Spi_Send_MSG(cmd,SPI_RSP_SUCCED,0,0);
}

uint8_t APDU_ReadLog(uint8_t cmd, uint8_t *pdata,uint16_t plen)
{
	return Spi_Send_MSG(cmd,SPI_RSP_SUCCED,0,0);
}

uint8_t BoosterSet(uint8_t cmd, uint8_t *pdata,uint16_t plen)
{
	PRINTF(" BoosterSet\r\n");
	bool status = BoosterSetParameter(pdata,plen);
	if (status)
		return Spi_Send_MSG(cmd,SPI_RSP_SUCCED,0,0);
	else
		return Spi_Send_MSG(cmd,SPI_RSP_FAILED,0,0);

}

uint8_t BoosterGet(uint8_t cmd, uint8_t *pdata,uint16_t plen)
{
	uint8_t p_buf[SPI_COMMAND_PAYLOAD_MAX_SIZE] = {0};

	PRINTF(" BoosterGet\r\n");
	BoosterGetParameter(pdata, plen, p_buf);
	return Spi_Send_MSG(cmd,SPI_RSP_DATA,p_buf,plen);
}

uint8_t LEDLight(uint8_t cmd, uint8_t *pdata,uint16_t plen)
{
#if 0
	uint16_t Rbuf[5]={0x00};
	uint16_t Rlight =0, Rtwinkle=0; 
	LedR=0;
	LedG=0;
	LedB=0;
	memcpy((uint8_t *)(Rbuf), &((spi_command_t*) spi_buffer.buf_command_response)->payload[0], 10);
	Rlight = Rbuf[0];
	Rtwinkle = Rbuf[1];
	LedR = Rbuf[2];
	LedG = Rbuf[3];	
	LedB = Rbuf[4];
	PRINTF("LED_LIGHT:%d,LED_TWINKLE:%d,R:%d,G:%d,B:%d\r\n",Rlight,Rtwinkle,LedR,LedG,LedB);	
	if(LED_Control_Light(Rlight))
	{
		if(LED_Control_Twinkle(Rtwinkle))
		{
			return Spi_Send_MSG(cmd,SPI_RSP_SUCCED,0,0);
		}
	}
#endif	
	Spi_Send_MSG(cmd,SPI_RSP_FAILED,0,0);
	return F_RSP_OK;
}

static void ChangePowerState(uint8 state)
{
	if (state == 0)
	{
		power_state = POWER_SUSPEND;
	}
	else if (state == 1)
	{
		power_state = POWER_RESUME;
	}
}

uint8_t NordicPM(uint8_t cmd, uint8_t *pdata,uint16_t plen)
{
	uint8_t Rbuf=0x00;
	memcpy(&Rbuf, pdata, 1);
	PRINTF("NordicPM:%d\r\n",Rbuf);
	SetBackLight(Rbuf);
	ChangePowerState(Rbuf);
	return Spi_Send_MSG(cmd,SPI_RSP_SUCCED,0,0);
}

uint8_t GetKl17Version(uint8_t cmd, uint8_t *pdata,uint16_t plen)
{
	PRINTF("KL17 Version\r\n");
	Spi_Send_MSG(cmd,SPI_RSP_DATA,fireware_version,2);
	return F_RSP_OK;
}

uint8_t Set_OTALanguageInfo(uint8_t cmd, uint8_t *pdata,uint16_t plen)
{
	uint8_t Rbuf[2]={0x00};
	uint8_t Resp_cmd = SPI_RSP_SUCCED;
	uint8_t len =1;
	memcpy(Rbuf, pdata, 2);
	if(InitDisplayFont(Rbuf))
	{
		Resp_cmd = SPI_RSP_SUCCED;
	}
	else
	{
		Resp_cmd = SPI_RSP_FAILED;
	}
	PRINTF("Resp_cmd:%d\r\n",Resp_cmd);
	PRINTF("Set_OTALanguageInfo:%d\r\n",Rbuf[0]);
	return Spi_Send_MSG(cmd,SPI_RSP_DATA,&Resp_cmd,len);
}

uint8_t PowerOffSystem(uint8_t cmd, uint8_t *pdata,uint16_t plen)
{
	uint8_t Rbuf=0x00;
	memcpy(&Rbuf, pdata, 1);
	PRINTF("PowerOffSystem:%d\r\n",Rbuf);
	SetPowerOffPin(1);
	ShowPowerOffLogo();
	PowerOffKey=1;
	return Spi_Send_MSG(cmd,SPI_RSP_SUCCED,0,0);
}

uint8_t ResumeBacklight(uint8_t cmd, uint8_t *pdata,uint16_t plen)
{
	uint8_t Rbuf=0x00;
	memcpy(&Rbuf, pdata, 1);
	PRINTF("ResumeBacklight:%d\r\n",Rbuf);
	ResumeSetBackLight();
	tpm_change_value(shortcut_info.brightness_level);	
	return Spi_Send_MSG(cmd,SPI_RSP_SUCCED,0,0);
}

uint8_t Flash_Write(uint8_t cmd, uint8_t *pdata,uint16_t plen)
{
	uint8_t Rbuf[4]={0x00};
	memcpy(Rbuf, pdata, 4);
	uint32_t addr = uint32_decode(Rbuf);
	PRINTF("Flash_Write:(0x%x %d)",addr ,plen);
	PRINTF((char*)(pdata+4));
	PRINTF("\r\n");
	SpiFlash_Write(addr,(pdata+4),(plen-4));
	return Spi_Send_MSG(cmd,SPI_RSP_SUCCED,0,0);
}

uint8_t Flash_Read(uint8_t cmd, uint8_t *pdata,uint16_t plen)
{
	uint8_t Sbuf[SPI_COMMAND_PAYLOAD_MAX_SIZE]={0x00};
	uint8_t Rbuf[6]={0x00};
	memcpy(Rbuf, &((spi_command_t*) spi_buffer.buf_command_response)->payload[0], 6);
	uint32_t addr = uint32_decode(Rbuf);
	uint16_t len =uint16_decode((Rbuf+4));
	PRINTF("Flash_Read:0x%x 0x%x\r\n",addr,len );
	SpiFlash_Read(addr,Sbuf,len);
	return Spi_Send_MSG(cmd,SPI_RSP_DATA,Sbuf,len);
}

uint8_t Flash_Erase(uint8_t cmd, uint8_t *pdata,uint16_t plen)
{
	uint8_t Rbuf[5]={0x00};
	memcpy(Rbuf, &((spi_command_t*) spi_buffer.buf_command_response)->payload[0], 5);
	uint32_t addr = uint32_decode(Rbuf);
	uint8_t Fe =Rbuf[4];
	PRINTF("Flash_Erase:0x%x 0x%x\r\n",addr,Fe);
	switch(Fe)
	{
		case F_SECTOR:
			SpiFlash_4KSector_Erase(addr);
			break;
		case F_BLOCK:
			SpiFlash_64KBlock_Erase(addr);
			break;
		case F_CHIP:
			SpiFlash_ChipErase();
			break;	
		default:
			break;
	}
	return Spi_Send_MSG(cmd,SPI_RSP_SUCCED,0,0);
}

uint8_t Flash_Set_Access(uint8_t cmd, uint8_t *pdata,uint16_t plen)
{
	uint8_t Rbuf=0x00;
	memcpy(&Rbuf, pdata, 1);
	PRINTF("Flash_Set_Access %d \r\n",Rbuf);
	if(Rbuf==0x00)		/*Freescal*/
	{
		ReumeDMA();
	}
	else if(Rbuf==0x01)	/*Nordic*/
	{
		SuspendDMA();
	}
	return Spi_Send_MSG(cmd,SPI_RSP_SUCCED,0,0);
}

uint8_t Flash_Display_Ancs_LayOut(uint8_t cmd, uint8_t *pdata,uint16_t plen)
{
	uint8_t Rbuf[20]={0x20};
	memcpy(Rbuf, &((spi_command_t*) spi_buffer.buf_command_response)->payload[0], plen);
	ANCS_READ_ADDR = uint32_decode(Rbuf);
	memcpy(LayOut,Rbuf+4,5);
	LayOutIndex = Rbuf[9];
	DetailIndex = Rbuf[10];
	DetailDir =(DetailPageDir)(Rbuf[11]);
	ReadEnd =0;
	PRINTF("Flash_Display_Ancs_LayOut 0x%x  LayOut=0x%x 0x%x 0x%x 0x%x 0x%x  LayOutIndex =0x%x DetailIndex=0x%x DetailDir=0x%x\r\n",
		ANCS_READ_ADDR,LayOut[0],LayOut[1],LayOut[2],LayOut[3],LayOut[4],LayOutIndex,DetailIndex,(uint8_t)DetailDir);
	if(DetailIndex==0x30)
	{
		DetailIndex=0;
		InitDetailParameter();
	}
	Display_Ancs_LayOut();
	Spi_Send_MSG(cmd,SPI_RSP_DATA,&ReadEnd,1);
	return F_RSP_OK;
}



uint8_t LCD_UpdateStatus(uint8_t cmd, uint8_t *pdata,uint16_t plen)
{
	uint16_t Rbuf=0x00;
	memcpy((uint8_t *)(&Rbuf), &((spi_command_t*) spi_buffer.buf_command_response)->payload[0], 2);
	gDisplayStatus.u16DMS=Rbuf;
	PRINTF("LCD_UpdateStatus:0x%x\r\n",gDisplayStatus.u16DMS);
	Spi_Send_MSG(cmd,SPI_RSP_SUCCED,0,0);
	return F_RSP_OK;
}

uint8_t LCD_UpdateTime(uint8_t cmd, uint8_t *pdata,uint16_t plen)
{
	uint8_t Rbuf[7]={0x00};
	memcpy(Rbuf, &((spi_command_t*) spi_buffer.buf_command_response)->payload[0], 7);
	gDisplayTime.year=uint16_decode(Rbuf);
	gDisplayTime.month = Rbuf[2];
	gDisplayTime.day= Rbuf[3];
	gDisplayTime.hour= Rbuf[4];
	gDisplayTime.minutes= Rbuf[5];
	gDisplayTime.week = Rbuf[6];
	PRINTF("LCD_UpdateTime:%d-%d-%d %d %d:%d\r\n",gDisplayTime.year,gDisplayTime.month,gDisplayTime.day,gDisplayTime.week,gDisplayTime.hour,gDisplayTime.minutes);	
	Spi_Send_MSG(cmd,SPI_RSP_SUCCED,0,0);
	return F_RSP_OK;
}

uint8_t LCD_UpdateTimeFormat(uint8_t cmd, uint8_t *pdata,uint16_t plen)
{
	uint8_t Rbuf[3]={0x00};
	memcpy(Rbuf, &((spi_command_t*) spi_buffer.buf_command_response)->payload[0], 3);
	gFormat.Style = Rbuf[0];
	gFormat.TimeFormat = Rbuf[1];
	gFormat.DateFormat = Rbuf[2];
	PRINTF("LCD_UpdateTimeFormat:%d-%d-%d\r\n",gFormat.Style,gFormat.TimeFormat,gFormat.DateFormat);
	Spi_Send_MSG(cmd,SPI_RSP_SUCCED,0,0);
	return F_RSP_OK;
}

uint8_t LCD_MainDataInterface(uint8_t cmd, uint8_t *pdata,uint16_t plen)
{
	uint8_t Rbuf[128]={0x00};
	uint8_t flags = 0;
	MainDataI mi;
	if(plen>128)
		plen=128;
	memcpy(Rbuf, &((spi_command_t*) spi_buffer.buf_command_response)->payload[0], plen);
	mi = (MainDataI)(Rbuf[0]);
	switch(mi)
	{
		case MI_TIME:
			{
				uint8_t TimerMenuFlag=1;	
				uint8 Distance_Format = 0;
				DisplayTime.year=uint16_decode(Rbuf+1);
				DisplayTime.month = Rbuf[3];
				DisplayTime.day= Rbuf[4];
				DisplayTime.hour= Rbuf[5];
				DisplayTime.minutes= Rbuf[6];
				DisplayTime.week = Rbuf[7];	
				TimerMenuFlag=Rbuf[9];
				//Distance_Format = Rbuf[10];
				//gSportDisplay.SportDistance = uint32_decode(Rbuf+11);	//距离单位:米
				//gSportDisplay.SportDuration= uint32_decode(Rbuf+15);	//运动时长单位:分钟
				TimeOrder.TimeFormat = Rbuf[10];
				shortcut_info.battery_level= Rbuf[11];
				shortcut_info.battery_percent= Rbuf[12];	//运动时长单位:分钟

				time_battery_info.hour = DisplayTime.hour;
				time_battery_info.minutes = DisplayTime.minutes;
				time_battery_info.TimeFormat = TimeOrder.TimeFormat;
				time_battery_info.bat_level = shortcut_info.battery_level;
				time_battery_info.bat_percent = shortcut_info.battery_percent;
				
				if(TimerMenuFlag==1) 
					ShowTimeMenuOne(Distance_Format);
				else if(TimerMenuFlag==2) 
					ShowTimeMenuTwo();
				else if(TimerMenuFlag==3)
					ShowTimeMenuThree();
				else if(TimerMenuFlag==4)
					ShowTimeMenuFour(Distance_Format);
				else if(TimerMenuFlag==5)
					ShowTimeMenuFive(Distance_Format);
				else if(TimerMenuFlag==6)
					ShowTimeMenuSix();
				else
					ShowTimeMenuOne(Distance_Format);
			}
			break;
		case MI_TOOL:
			{
			uint8 hour = Rbuf[2];
			uint8 minute = Rbuf[3];
			uint8 month = Rbuf[4];
			uint8 day = Rbuf[5];
			uint8 week = Rbuf[6];	

#if defined (L40B)	
			if(Rbuf[1]==0x01)
					ShowMenu5(hour,minute,month,day,week);					
				else if(Rbuf[1]==0x02)
					ShowMenu6(hour,minute,month,day,week); 									 /**< Name of the device. Will be included in the advertising data. */
#else
			if(Rbuf[1]==0x01)
					ShowMenu1(hour,minute,month,day,week);
				else if(Rbuf[1]==0x02)
					ShowMenu2(hour,minute,month,day,week);										/**< Name of the device. Will be included in the advertising data. */
#endif
		
			}
			break;
		case MI_CHANGE_GOALS:
			ShowChangeGoals();
			break;
		case MI_ACT_DIRATION:
			{
				uint32_t buf =uint32_decode(Rbuf+1);
				uint8_t Goal_Percent = Rbuf[5];
				SPORT_uTime.month = Rbuf[7];
				SPORT_uTime.day = Rbuf[8];
				ShowActivityTime(buf,Goal_Percent);
			}
			break;
		case MI_SPORT:
			{
				uint32_t buf =uint32_decode(Rbuf+1);
				uint8_t Goal_Percent = Rbuf[5];
				
				SPORT_uTime.month = Rbuf[7];
				SPORT_uTime.day = Rbuf[8];				
				ShowSteps(buf,Goal_Percent);
			}			
			break;
		case MI_CAL:
			{
				uint32_t buf =uint32_decode(Rbuf+1);
				uint8_t Goal_Percent = Rbuf[5];
				SPORT_uTime.month = Rbuf[7];
				SPORT_uTime.day = Rbuf[8];
				ShowCalor(buf,Goal_Percent);
			}			
			break;
		case MI_DISTANCE:
			{
				uint32_t buf =uint32_decode(Rbuf+1);
				uint8_t Goal_Percent = Rbuf[5];
				uint8_t DisFormat = Rbuf[6];			//0公里 1英里
				SPORT_uTime.month = Rbuf[8];
				SPORT_uTime.day = Rbuf[9];
				ShowDistance(buf,Goal_Percent,DisFormat);
			}			
			break;		
		case MI_CHARGING:
			{
				memcpy(&time_battery_info, &Rbuf[1], 3);
				ShowCharging(Rbuf[4]+1, Rbuf[5]);
			}			
			break;
			
		case MI_LOW_BATTERY:
			memcpy(&time_battery_info, &Rbuf[1], 3);
			ShowBatteryLow();
			break;
			
		case MI_SHORTCUT:
			{
				flags = Rbuf[1];  	//flags = 0, Only update data; flags = 1, Both update data and image.
				shortcut_info.find_me = Rbuf[2]; 
				shortcut_info.airplane_mode =  Rbuf[3];
				shortcut_info.vibration = Rbuf[4];
				shortcut_info.battery_level = Rbuf[5];
				shortcut_info.battery_percent = Rbuf[6];
				shortcut_info.BC5_Connect= Rbuf[7];
				if (flags == 1)
					ShowShortCutKey();
			}			
			break;
		case MI_BRIGHTNESS:
			{
				flags = Rbuf[1];   //flags = 0, Only update data; flags = 1, Both update data and image.
				shortcut_info.brightness_level = Rbuf[2];
				memcpy(&time_battery_info, &Rbuf[3], sizeof(time_battery_info_t));
				tpm_change_value(shortcut_info.brightness_level);
				if (flags == 1)
					ShowBrightness();
			}			
			break;
			
		case MI_BRIGHTNESS_SET:
			tpm_change_value(Rbuf[1]);
			break;
						
		case MI_MUSIC:
			{
				uint8 Vol = 0;
				if(Rbuf[2]==1)
				{	
					MusicBuf[0]=1;
					if(plen>3&&Rbuf[3]>0)
					{
						memset((MusicBuf+1),0,128);			
						memcpy((MusicBuf+1),(Rbuf+4),Rbuf[3]);
					}
					Vol = Rbuf[4+Rbuf[3]];
					memcpy(&time_battery_info, &Rbuf[5+Rbuf[3]], sizeof(time_battery_info_t));
				}
				else
				{
					MusicBuf[0]=0;
					Vol = Rbuf[3];
					memcpy(&time_battery_info, &Rbuf[4], sizeof(time_battery_info_t));
				}
				ShowMusic((MusicSet)(Rbuf[1]),Vol);
			}	
			break;				
		case MI_MUSIC_CONTROL:
			{
				memcpy(&time_battery_info, &Rbuf[3], sizeof(time_battery_info_t)); 
				ShowMusic((MusicSet)(Rbuf[1]),Rbuf[2]);
			}
			break;
		case MI_CAMERA:
			{
				memcpy(&time_battery_info, &Rbuf[2], sizeof(time_battery_info_t));
				ShowCamera(Rbuf[1]);
			}
			break;
		case MI_SEARCH:
			{
				ShowSearch(Rbuf[1]);
				memcpy(&time_battery_info, &Rbuf[2], sizeof(time_battery_info_t));
			}
			break;
		case MI_SLEEP_CHANGE:
			{
				ShowSleep(Rbuf[1],Rbuf[2]);
			}
			break;
		case MI_SLEEP_TIME:
			{
				memcpy(&time_battery_info, &Rbuf[6], sizeof(time_battery_info_t));
				ShowSleepTimes(Rbuf[1],Rbuf[2],Rbuf[3],Rbuf[4],Rbuf[5]);
			}
			break;
		case MI_SLEEP_START:
			{
				memcpy(&time_battery_info, &Rbuf[6], sizeof(time_battery_info_t));
				ShowSleepStart(Rbuf[1],Rbuf[2],Rbuf[3],Rbuf[4],Rbuf[5]);
			}
			break;
		case MI_SLLEP_END:
			{
				memcpy(&time_battery_info, &Rbuf[6], sizeof(time_battery_info_t));
				ShowSleepEnd(Rbuf[1],Rbuf[2],Rbuf[3],Rbuf[4],Rbuf[5]);
			}
			break;
		case MI_SLEEP_OVERVIEW_ONE:
			ShowSleepOverviewOne(&Rbuf[1]);
			break;
		case MI_SLEEP_OVERVIEW_TWO:
			ShowSleepOverviewOne(&Rbuf[1]);
			break;
		case MI_SLEEP_OVERVIEW_THREE:
			ShowSleepOverviewOne(&Rbuf[1]);
			break;
		case MI_SLEEP_DETAIL:
			{
				ShowSleepDetail(&Rbuf[1]);
#if 0				
				memcpy(&sleep_overview_1, &Rbuf[1], sizeof(sleep_overview_t));
				sleep_overview_1.year = uint16_decode(Rbuf+6);
				sleep_info.total_sleep= uint16_decode(Rbuf+8);
				sleep_info.fell_sleep= uint16_decode(Rbuf+10);
				sleep_info.light_sleep= uint16_decode(Rbuf+12);
				sleep_info.deep_sleep= uint16_decode(Rbuf+14);
				sleep_info.awake_sleep= uint16_decode(Rbuf+16);
				sleep_info.awake= uint16_decode(Rbuf+18);
				memcpy(&time_battery_info, &Rbuf[20], sizeof(time_battery_info_t));
				ShowSleepDetail();
#endif				
			}
			break;
		case MI_SLEEP_INFO_ONE:
			{
				memcpy(&sleep_info, &Rbuf[1], sizeof(sleep_info));
				ShowSleepInfoOne();
			}
			break;

		case MI_SLEEP_INFO_TWO:
			{
				memcpy(&sleep_info, &Rbuf[1], sizeof(sleep_info));
				ShowSleepInfoTwo();
			}
			break;

		case MI_RESET_PROGESS:
			{
				ShowResetProgress(Rbuf[1]);
			}
			break;
			
		case MI_OTA_RESULT:
			ShowOtaResult(Rbuf[1]);
			break;

		case MI_TIMER_CONTROL:
			memcpy(&time_battery_info, &Rbuf[5], sizeof(time_battery_info_t));
			if (Rbuf[1] == TIMER_START)
			{
				ShowTimer(Rbuf[1],Rbuf[2],Rbuf[3],Rbuf[4]);
			}
			else if (Rbuf[1] == TIMER_RUN)
			{
				ShowTimer(Rbuf[1],Rbuf[2],Rbuf[3],Rbuf[4]);
			}
			else if (Rbuf[1] == TIMER_STOP)
			{
				ShowTimer(Rbuf[1],Rbuf[2],Rbuf[3],Rbuf[4]);
			}
			break;
		case MI_ANCS_SEND:
			ShowAncsSend(Rbuf[1]);
			break;
		case MI_ANCS_REPLY:
			ShowAncsReply(Rbuf[1]);
			break;
		case MI_SWITCH_ON_OFF:
			ShowSwitchState(Rbuf[1]);
			break;
		case MI_REMIND_INDEX:
			memcpy(&time_battery_info, &Rbuf[2], sizeof(time_battery_info_t));
			ShowRemind(Rbuf[1]);
			break;
		case MI_PAYMENT:
			if (Rbuf[1] == PAY_LOCK)
			{
				ShowPaymentLock();
			}
			else if (Rbuf[1] == PAY_UNLOCK)
			{
				ShowPaymentUnlock();
			}
			else if (Rbuf[1] == PAY_ANIMATION_1)
			{
				timer2_start();
			}
			else if (Rbuf[1] == PAY_ANIMATION_2)
			{
				timer2_stop();
			}
			else if (Rbuf[1] == PAY_BALANCE)
			{
				log_data.Balance= (Rbuf[5] << 24 ) | (Rbuf[4] << 16) | (Rbuf[3] << 8) | (Rbuf[2]);
				ShowPaymentBalance();
			}
			else if (Rbuf[1] == PAY_CONSUME)
			{
				log_data.Amount = (Rbuf[5] << 24 ) | (Rbuf[4] << 16) | (Rbuf[3] << 8) | (Rbuf[2]);
				ShowPaymentConsume();
			}
			else if (Rbuf[1] == PAY_LOG)
			{
				memcpy(log_data.Date, &Rbuf[2], 6);	//date
				memcpy(log_data.Time, &Rbuf[8], 6);	//time
				ShowPaymentLog();
			}
			break;
		case MI_LCD_TEST:
			ShowLcdTest(Rbuf[1]);
			break;
		case MI_TP_TEST:
			ShowTpTest(Rbuf[1],Rbuf[2]);
			break;	
		case MI_PHONE_CALL:
			ShowPhoneCall(Rbuf+1);
			break;
		case MI_PHONE_CALL_TIME:
			{
				uint16_t ptime =uint16_decode((Rbuf+1));
				ShowPhoneCallTime(ptime);
			}
			break;		
		case MI_WATCH_INFO:
			memcpy(watch_info.WatchId,&Rbuf[1], sizeof(watch_info));
			ShowWatchInfo();
			break;
		case MI_HR:
			{
				DispalyHrData.HR_refresh_first=Rbuf[1];
				DispalyHrData.HR_mode=Rbuf[2];
				DispalyHrData.HR_Run_State=Rbuf[3];
				DispalyHrData.HR_Value_Val=Rbuf[4];  
			    DispalyHrData.HR_Value=Rbuf[5];         
			    DispalyHrData.HR_Running=Rbuf[6];
				DispalyHrData.DHM_Flag=Rbuf[7];
				DispalyHrData.DHM_Time=Rbuf[8];
			   	DispalyHrData.HR_Level=Rbuf[9];
				
				DisplayTime.month=Rbuf[11];
				DisplayTime.day=Rbuf[12];
				memcpy(&time_battery_info, &Rbuf[13], sizeof(time_battery_info_t));								
				ShowHeartRate(DispalyHrData);
				
			}
			break;
		case MI_HR_ALARM:
			{ 
				DispalyHrData.HR_Value=Rbuf[1];
				DispalyHrData.HR_ALARM=Rbuf[2];
				memcpy(&time_battery_info, &Rbuf[3], sizeof(time_battery_info_t));
				ShowHRAlarm(DispalyHrData.HR_Value,DispalyHrData.HR_ALARM);
			}
			
			break;
		case MI_HR_ALARM_WITHOUT_CLS:
			{
				DispalyHrData.HR_Value=Rbuf[1];
				DispalyHrData.HR_ALARM=Rbuf[2];
				memcpy(&time_battery_info, &Rbuf[3], sizeof(time_battery_info_t));
				ShowHRAlarm_Without_Cls(DispalyHrData.HR_Value,DispalyHrData.HR_ALARM);

			}
			break;
		case MI_SPORT_OVER_VIEW:
			{
				uint8 DistanceFormat =0;
				uint8 ui_index = 0;
				ui_index = Rbuf[1];
				gSportDisplay.SportSteps = uint32_decode(Rbuf+3);		//步数单位:步
				DailyGoal[0].Goal = uint16_decode(Rbuf+7);			//步数目标单位:百步
				gSportDisplay.SportDistance = uint32_decode(Rbuf+9);	//距离单位:米
				DailyGoal[2].Goal = uint16_decode(Rbuf+13);		//距离目标单位:千米	
				gSportDisplay.SportKcal = uint32_decode(Rbuf+15);		//卡路里单位:卡	
				DailyGoal[1].Goal = uint16_decode(Rbuf+19);		//卡路里目标单位:千卡
				gSportDisplay.SportDuration= uint32_decode(Rbuf+21);	//运动时长单位:分钟
				DailyGoal[4].Goal = uint16_decode(Rbuf+25);
				DistanceFormat = Rbuf[27];	//0公里 1英里	
				ShowSportOverview(ui_index , DistanceFormat);
			}
			break;
		case MI_SPORTTOTAL:
			ShowSportTotal(Rbuf[1],Rbuf[2],Rbuf[3], Rbuf[4], Rbuf[5], Rbuf[6], Rbuf[7]);
			break;
		case MI_VOL_LEV:
			{
				uint8 level;
				level = Rbuf[1];
				ShowVolumeLevel(level);
			}
			break;
		case MI_SIRI_SWITCH:
			{
				uint8 status;
				status = Rbuf[1];
				ShowSiri(status);
			}
			break;
		case MI_DIAL_NUM:
			{
				ShowDailNumber(Rbuf+1);
			}
			break;
		case MI_DIAL_NUM_INCALL:
			{
				ShowDailNumber_Incall(Rbuf+1);
			}
			break;	
		case MI_PHONE_SOUND_STATUS:
			PhoneSound.PhoneSoundStatus = Rbuf[1];
			if(Rbuf[2]==0x01)
				ShowPhoneSoundStatus(PhoneSound.PhoneSoundStatus);
			break;
			
		case MI_PHONE_Mic_STATUS:
			PhoneMic.PhoneMicStatus = Rbuf[1];
			if(Rbuf[2]==0x01)
				ShowMicstatus(PhoneMic.PhoneMicStatus);
			break;
			
		case MI_SEARCHING:
			{			
				ShowSearching(Rbuf[1]);
				memcpy(&time_battery_info, &Rbuf[2], sizeof(time_battery_info_t));
			}
			break;
			
		case MI_SMS_FROM_ANDROID:
			SmsFromAndroid = Rbuf[1];
			break;
		case MI_DIAL_NUM_FADE:
			ShowDailInterfaceManage(Rbuf[1], true);
			break;
		case MI_DIAL_NUM_RECOVER:
			ShowDailInterfaceManage(0, false);
			break;
		case MI_DIAL_INCALL_NUM_FADE:
			ShowDailInterfaceManage_Incall(Rbuf[1], true);
			break;
		case MI_DIAL_INCALL_NUM_RECOVER:
			ShowDailInterfaceManage_Incall(0, false);
			break;
		case MI_WRIST_STATE_CHANGE:
			ShowWristStateChange(Rbuf[1]);
			break;

		case MI_MAIN_MENU_ONE:
			memcpy(&time_battery_info, &Rbuf[1], sizeof(time_battery_info_t));
			ShowMainSport();
			break;			
		case MI_MAIN_MENU_TWO:
			memcpy(&time_battery_info, &Rbuf[1], sizeof(time_battery_info_t));
			ShowMainHeartrate();
			break;	
		case MI_MAIN_MENU_THREE:
			memcpy(&time_battery_info, &Rbuf[1], sizeof(time_battery_info_t));
			ShowMainRecordWorkouts();
			break;	
		case MI_MAIN_MENU_FOUR:
			memcpy(&time_battery_info, &Rbuf[1], sizeof(time_battery_info_t));
			ShowMainSleep();
			break;	
		case MI_MAIN_MENU_FIVE:
			memcpy(&time_battery_info, &Rbuf[1], sizeof(time_battery_info_t));
			//ShowMainCamera();
			ShowMainNotification();
			break;
		case MI_MAIN_MENU_SIX:
			memcpy(&time_battery_info, &Rbuf[1], sizeof(time_battery_info_t));
			//ShowMainMusic();
			ShowMainApps();
			break;
		case MI_MAIN_MENU_SEVEN:
			memcpy(&time_battery_info, &Rbuf[1], sizeof(time_battery_info_t));
			ShowMainNotification();
			break;
		case MI_MAIN_MENU_EIGHT:
			memcpy(&time_battery_info, &Rbuf[1], sizeof(time_battery_info_t));
			ShowMainApps();
			break;
		case MI_BATTERY_FULL:
			memcpy(&time_battery_info, &Rbuf[1], 3);
			ShowBatteryFull();
			break;
		case MI_SETUP:
			memcpy(watch_info.WatchId,&Rbuf[1], sizeof(watch_info));
			ShowSetup();
			break;
		case MI_BOND_QUERY:
			memcpy(&time_battery_info, &Rbuf[1], sizeof(time_battery_info_t));
			ShowPairRequest();
			break;
		case MI_BOND_OK:
			memcpy(&time_battery_info, &Rbuf[1], sizeof(time_battery_info_t));
			ShowPairSucced();
			break;
		case MI_BOND_FAIL:
			memcpy(&time_battery_info, &Rbuf[1], sizeof(time_battery_info_t));
			ShowPaidFaid();
			break;	
		case MI_BLE_DISCONNECTED:
			memcpy(&time_battery_info, &Rbuf[1], sizeof(time_battery_info_t));
			ShowPaidFaid();
			break;
		case MI_MOVE_NOW:
			memcpy(&time_battery_info, &Rbuf[1], sizeof(time_battery_info_t));
			ShowMoving();
			break;
		case MI_GOAL_OVER:
			memcpy(&time_battery_info, &Rbuf[2], sizeof(time_battery_info_t));
			ShowGoalOver(Rbuf[1]);
			break;		
		case MI_SYNCING:
			memcpy(&time_battery_info, &Rbuf[1], sizeof(time_battery_info_t));
			ShowSyncingIcon();
			break;
		case MI_NOTIFICATION_MENU_ONE:
			ShowNotificationMenuOne(Rbuf[1],Rbuf[2],Rbuf[3]);
			break;
		case MI_NOTIFICATION_MENU_TWO:
			ShowNotificationMenuTwo(Rbuf[1],Rbuf[2]);
			break;
		case MI_MEMORY_FULL:
			memcpy(&time_battery_info, &Rbuf[1], sizeof(time_battery_info_t));
			ShowMemoryFull();
			break;
		case MI_SETTING_APP1:
			ShowSettingMenuOne(Rbuf[1]);
			break;
		case MI_SETTING_APP2:
			memcpy(watch_info.WatchId,&Rbuf[4], sizeof(watch_info));			
			ShowSettingMenuTwo(Rbuf[1],Rbuf[2],Rbuf[3]);
			break;
		case MI_RESET:
			memcpy(&time_battery_info, &Rbuf[1], sizeof(time_battery_info_t));
			ShowReset();
			break;
		case MI_RESTART:
			memcpy(&time_battery_info, &Rbuf[1], sizeof(time_battery_info_t));
			ShowRestart();
			break;
		case MI_REMIND_LIST:
			ShowRemindList(&Rbuf[1]);
			break;
		case MI_REFRESH_TIME_UI_FUN:
			DisOption = Rbuf[1];
			DisState  = Rbuf[2];
			TimerMenu_Index = Rbuf[3];
			Value  = uint32_decode(Rbuf+4);			
			RefreshTimeUIFun(DisOption,DisState,Value,TimerMenu_Index );			
			break;
		case MI_VIRATE_LEVEL:
			ShowVibrateLevel(Rbuf[1]);
			break;
		case MI_CAMERA_COUNTDOWN:
			memcpy(&time_battery_info, &Rbuf[2], sizeof(time_battery_info_t));
			ShowCameraCountDown(Rbuf[1]);
			break;
		case MI_BATTERY_INFO:
			memcpy(&time_battery_info, &Rbuf[1], sizeof(time_battery_info_t));
			ShowBatteryInfo(time_battery_info.bat_level,time_battery_info.bat_percent);
			break;
		case MI_RECORD_GO:
			memcpy(&time_battery_info, &Rbuf[1], sizeof(time_battery_info_t));
			ShowRecordgo();
			break;	   
	    case MI_RECORD_RESULT:
		     	{
				memcpy(&time_battery_info, &Rbuf[20], sizeof(time_battery_info_t));
				uint8 ui_index = Rbuf[1];
				uint8 DistanceFormat= Rbuf[2];
				uint8 Hr_value= Rbuf[3];
				uint32 uSteps = uint32_decode(Rbuf+4);
				uint32 uDistance= uint32_decode(Rbuf+8);
				uint32 uCalor= uint32_decode(Rbuf+12);
				uint32 time_sec= uint32_decode(Rbuf+16); 
				
				ShowRecordResult(ui_index, DistanceFormat,Hr_value,
								uSteps,uDistance,
								uCalor,time_sec);
	    		}
			break;
		 case MI_RECORD_RUN:
		 	memcpy(&time_battery_info, &Rbuf[6], sizeof(time_battery_info_t));
			ShowRecordWorkoutsRUN(Rbuf[1],uint16_decode(Rbuf+2),Rbuf[4],Rbuf[5]);
			break;
		case MI_EARNED_MEDAL_REMIND:
			ShowEarnedMeadalRemind(Rbuf[1]);
			break;
		case MI_SOCIAL_CLASS:
			ShowSocialClass(Rbuf[1],Rbuf[2],Rbuf[3],Rbuf[4]);
			break;
		case MI_USER_REMAND:
			memcpy(&time_battery_info, &Rbuf[1], sizeof(time_battery_info_t));
			ShowUserRemind(Rbuf+1+ sizeof(time_battery_info_t));			
			break;
	}
	PRINTF("LCD_MainDataInterface %d ",mi);
	PRINTF((char*)(Rbuf+1));
	PRINTF("\r\n");
	Spi_Send_MSG(cmd,SPI_RSP_SUCCED,0,0);
	return F_RSP_OK;
}


uint8_t LCD_MusicInterface(uint8_t cmd, uint8_t *pdata,uint16_t plen)
{
	
	Spi_Send_MSG(cmd,SPI_RSP_SUCCED,0,0);
	return F_RSP_OK;
}

uint8_t LCD_Picturenterface(uint8_t cmd, uint8_t *pdata,uint16_t plen)
{
	switch(*pdata)
	{
		case P_SETUP:
			ShowSetup();
			break;
		case P_LOGO:
			ShowLogo();
			break;
		case P_BOND_QUERY:
			//ShowPairRequest();
			break;
		case P_BOND_OK:
			//ShowPairSucced();
			break;
		case P_BOND_FAIL:
			//ShowPaidFaid();
			break;
		case P_BLE_DISCONNECTED:
			//ShowBleDisconnected();
			break;
		case P_GOAL_OVER:
			//ShowGoalOver();
			break;
		case P_MOVE_NOW:
			//ShowMoving();
			break;
		case P_TOOL_APP1:
			ShowAppMenuOne();
			break;
		case P_TOOL_APP2:
			ShowAppMenuTwo();
			break;
		case P_TOOL_APP3:
			ShowAppMenuThree();
			break;
		case P_APP_BOX:
			ShowAppBox();
			break;
		case P_UPDATE_DONE:
			ShowUpdatedDone();
			break;
		case P_SETTING_APP1:
			//ShowSettingMenuOne();
			break;
		case P_SETTING_APP2:
			//ShowSettingMenuTwo();
			break;
		case P_SETTING_APP3:
			ShowSettingMenuThree();
			break;	
		case P_BLUETOOTH_ON_STATE:
			ShowBlueToothOnState();
			break;
		case P_BLUETOOTH_OFF_STATE:
			ShowBlueToothOffState();
			break;
		case P_CURRENCY_TYPE:
			ShowCurrencyType();
			break;
		case P_SOUND_STATE:
			ShowSoundState();
			break;
		case P_RESET:
			//ShowReset();
			break;
		case P_OTA_LOGO:
			ShowOtaLogo();
			break;
		case P_TIMER_MENU_ONE:
			ShowTimeMenuOne(0);//ShowTime(DisplayTime);
			break;
		case P_TIMER_MENU_TWO:
			ShowTimeMenuTwo();
			break;
		case P_TIMER_MENU_THREE:
			ShowTimeMenuThree();
			break;
		case P_TIMER_MENU_FOUR:
			ShowTimeMenuFour(0);
			break;
		case P_TIMER_MENU_FIVE:
			ShowTimeMenuFive(0);
		case P_VIRATE_ON_STATE:
			ShowVibrateOnState();
			break;
		case P_VIRATE_OFF_STATE:
			ShowVibrateOffState();
			break;
		case P_WRIST_ON_STATE:
			ShowWristState(1);
			break;
		case P_WRIST_OFF_STATE:
			ShowWristState(0);
			break;
		case P_TIMER_MENU_UPDATED:
			ShowTimeMenuUpdated();
			break;
		case P_BT_PAIRING:
			ShowBTPairing();
			break;
		case P_BT_PAIRING_SUCCESS:
			ShowBTPairingSucced();
			break;
		case P_BT_PAIRING_FAIL:
			ShowBTPairingFaid();
			break;
		case P_VOL:
			//ShowVolume();
			break;
		case P_DIAL:
			ShowDailInterface();
			break;
		case P_DIAL_INCALL:
			ShowDailInterface_Incall();
			break;
		case P_SYNCING:
			//ShowSyncingIcon();
			break;			
		case P_PAY_SET_UP:
			ShowPaySetUp();
			break;
		case P_PAY_PROGRESS:
			ShowPayProgress();
			break;
		case P_LANGUAGE_APP1:
			ShowLanguageMenuOne();
			break;
		case P_LANGUAGE_APP2:
			ShowLanguageMenuTwo();
			break;
		case P_LANGUAGE_APP3:
			ShowLanguageMenuThree();
			break;
		case P_LANGUAGE_APP4:
			ShowLanguageMenuFour();
			break;
		case P_LANGUAGE_CHOOSE_ONE:
			LanNow=LAN_ENGLISH;
			break;
		case P_LANGUAGE_CHOOSE_ONE+0x01:
			LanNow=LAN_CHINESE_SIMPLIFID;
			break;
		//case P_LANGUAGE_CHOOSE_ONE+0x02:
		//	LanNow=LAN_CHINESE_TRADITIONAL;
		//	break;
		//case P_LANGUAGE_CHOOSE_ONE+0x03:
		//	LanNow=LAN_KOREAN;
		//	break;
		//case P_LANGUAGE_CHOOSE_ONE+0x04:
		//	LanNow=LAN_THAI;
		//	break;
		//case P_LANGUAGE_CHOOSE_ONE+0x05:
		//	LanNow=LAN_JAPANESE;
		//	break;
		case P_LANGUAGE_CHOOSE_ONE+0x06:
			LanNow=LAN_ESPANOL;
			break;
		case P_LANGUAGE_CHOOSE_ONE+0x07:
			LanNow=LAN_FRANCAIS;
			break;
		case P_LANGUAGE_CHOOSE_ONE+0x08:
			LanNow=LAN_DEUTSCH;
			break;
		case P_LANGUAGE_CHOOSE_ONE+0x09:
			LanNow=LAN_ITALIANO;
			break;
		case P_LANGUAGE_CHOOSE_ONE+0x0A:
			LanNow=LAN_POLSKI;
			break;
		case P_LANGUAGE_CHOOSE_ONE+0x0B:
			LanNow=LAN_PORTUGUESE;
			break;
		case P_LANGUAGE_CHOOSE_ONE+0x0C:
			LanNow=LAN_RUSSIAN;
			break;
		case P_LANGUAGE_CHOOSE_ONE+0x0D:
			LanNow=LAN_DUTCH;
			break;
		case P_CLEARALL:
			ShowClearAll();
			break;
		case P_BATTERY_FULL:
			//ShowBatteryFull();
			break;
		case P_APPS_MENU_ONE:
			ShowAppsMenuOne();
			break;	
		case P_APPS_MENU_TWO:
			ShowAppsMenuTwo();
			break;
		case P_APPS_MENU_THREE:
			ShowAppsMenuThree();
			break;
		case P_NO_NEW_NTIFICATIONS:
			ShowNoNewNotifications();
			break;
		case P_NO_REMINDERS:
			ShowNoNewReminders();
			break;
		case P_EXITSLEEPMODE:
			ShowExitSleepMode();
			break;	
		case P_DELETE_ANCS:
			ShowDeleteAncs();
			break;
		case P_EXITWORKOUTS:
			ShowExitRecordWorkouts();
			break;
		case P_EXITSLEEP:
			ShowExitSleep();
			break;			
		default:
			break;
	}
	Spi_Send_MSG(cmd,SPI_RSP_SUCCED,0,0);
	return F_RSP_OK;
}

uint8_t LCD_PhoneInterface(uint8_t cmd, uint8_t *pdata,uint16_t plen)
{
	
	Spi_Send_MSG(cmd,SPI_RSP_SUCCED,0,0);
	return F_RSP_OK;
}

uint8_t LCD_MsgInterface(uint8_t cmd, uint8_t *pdata,uint16_t plen)
{
	uint8_t Rbuf[7]={0x00};
	memcpy(Rbuf, pdata, 7);
	PRINTF("LCD_MsgInterface %d %d\r\n",Rbuf[0],Rbuf[1]);
	memcpy(&time_battery_info, &Rbuf[2], sizeof(time_battery_info_t));
	Display_Ancs_Bigview((MsgUI)(Rbuf[0]),Rbuf[1]);
	Spi_Send_MSG(cmd,SPI_RSP_SUCCED,0,0);
	return F_RSP_OK;
}

uint8_t LCD_MsgSecInterface(uint8_t cmd, uint8_t *pdata,uint16_t plen)
{
	
	Spi_Send_MSG(cmd,SPI_RSP_SUCCED,0,0);
	return F_RSP_OK;
}

uint8_t LCD_DevCommInterface(uint8_t cmd, uint8_t *pdata,uint16_t plen)
{
	
	Spi_Send_MSG(cmd,SPI_RSP_SUCCED,0,0);
	return F_RSP_OK;
}

uint8_t LCD_StopWatchInterface(uint8_t cmd, uint8_t *pdata,uint16_t plen)
{
	
	Spi_Send_MSG(cmd,SPI_RSP_SUCCED,0,0);
	return F_RSP_OK;
}

uint8_t LCD_VoIumeInterface(uint8_t cmd, uint8_t *pdata,uint16_t plen)
{
	
	Spi_Send_MSG(cmd,SPI_RSP_SUCCED,0,0);
	return F_RSP_OK;
}

uint8_t LCD_HeartRateInterface(uint8_t cmd, uint8_t *pdata,uint16_t plen)
{
	
	Spi_Send_MSG(cmd,SPI_RSP_SUCCED,0,0);
	return F_RSP_OK;
}

uint8_t LCD_CameraAndSearchInterface(uint8_t cmd, uint8_t *pdata,uint16_t plen)
{
	
	Spi_Send_MSG(cmd,SPI_RSP_SUCCED,0,0);
	return F_RSP_OK;
}


uint8_t Flash_Display_PhBook_Inf(uint8_t cmd, uint8_t *pdata,uint16_t plen)
{
	uint8_t Rbuf[20]={0x20};
	uint32_t PhBInf_Read_Addr = 0x00000000UL;
	uint32_t PhBGrp_Read_Addr =0x00000000UL;
	uint8_t   Inf_Index = 0;
	uint8_t   Grp_Index = 0;
	uint8_t Percent_Showed = 0;
	memcpy(Rbuf, &((spi_command_t*) spi_buffer.buf_command_response)->payload[0], plen);
	PhBInf_Read_Addr = uint32_decode(Rbuf);
	PhBGrp_Read_Addr = uint32_decode(Rbuf + 5);
	Inf_Index = Rbuf[4];
	Grp_Index = Rbuf[9];
	Percent_Showed = Rbuf[10]; 
	PRINTF("Flash_Display_PhBook_Inf  PhBInf_Read_Addr 0x%x  PhBGrp_Read_Addr  0x%x Inf_Index=0x%x  Grp_Index=0x%x Percent_Showed=0x%x\r\n",
		PhBInf_Read_Addr,PhBGrp_Read_Addr,Inf_Index,Grp_Index,Percent_Showed);
	DisplayPhoneBook(PhBInf_Read_Addr,Inf_Index,PhBGrp_Read_Addr,Grp_Index,Percent_Showed);
	return Spi_Send_MSG(cmd,SPI_RSP_SUCCED,0,0);
}

uint8_t Flash_Display_PhonePrepare(uint8_t cmd, uint8_t *pdata,uint16_t plen)
{
	uint8_t Rbuf[32]={0x00};
	uint32_t Pp_Addr = 0x00000000UL;
	uint8_t   Namelen = 0;
	uint8_t   Numlen = 0;
	uint8_t   isAnd =0;
	memcpy(Rbuf, &((spi_command_t*) spi_buffer.buf_command_response)->payload[0], plen);
	isAnd =  Rbuf[0];
	Namelen =  Rbuf[1];
	Numlen =  Rbuf[2];
	Pp_Addr = uint32_decode(Rbuf+3);
	PRINTF("Flash_Display_PhonePrepare  Pp_Addr=0x%x  Namelen=%d Numlen=%d  isAnd = %d\r\n",Pp_Addr,Namelen,Numlen,isAnd);
	DisplayPhonePreparek(Pp_Addr,Namelen,Numlen,Rbuf+7,isAnd);
	return Spi_Send_MSG(cmd,SPI_RSP_SUCCED,0,0);
}


