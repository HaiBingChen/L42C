/******************************************************************************************
*文件名：			KL17CmdPackage.c
*说明：			KL17通讯协议数据解析实现文件
*编写:			CHENHAO
*版本信息:		V1.0
******************************************************************************************/
#include "headfile.h"
DISPLAY_STATUS_T gDisplayStatus;


uint8 Spi_Send_MSG(uint8 cmd,uint8*sdata,uint16 slen, uint8* rdata, uint8* rlen)
{
	uint8 status =0,ReSendCount =3;
	/*发送*/
	for(uint8 i=0;i<ReSendCount;i++)
	{
		status = spi_send_cmd(cmd,sdata,slen);
		if(status>SPI_SUCCED)
		{
			nrf_delay_us(500);	
		}
		else 
			break;
	}
	/*接收*/
	for(uint8 i=0;i<ReSendCount;i++)
	{
		status = spi_receive_data(rdata,rlen);
		if(status>SPI_SUCCED)
		{
			nrf_delay_us(500);	
			spi_send_cmd(cmd,sdata,slen);
		}
		else 
			break;
	}	
	return status;
}


bool LED_Control_Light(uint16_t Light,uint16_t Twinkle,uint16_t R,uint16_t G,uint16_t B)
{	
	uint16_t Sbuf[5];
	Sbuf[0] = Light;
	Sbuf[1] = Twinkle;
	Sbuf[2] = R;
	Sbuf[3] = G;
	Sbuf[4] = B;
	if(Spi_Send_MSG(LED_LIGHT, (uint8_t*)(&Sbuf),10,NULL,NULL)!=SPI_RSP_SUCCED)
		return false;
	return true;	
}

bool NFC_Action(NfcAction action)
{
	nrf_delay_ms(400);
	
	uint8_t Sbuf=action;
	if(Spi_Send_MSG(NFC_ACTION, &Sbuf,1,NULL,NULL)!=SPI_RSP_SUCCED)
		return false;

	if (action == NFC_OPEN) {
		nfc_status = NFC_STATUS_START;
	} 
	else if (action == NFC_CLOSE) {
		nfc_status = NFC_STATUS_STOP;
	}

	nrf_delay_ms(400);
	return true;
}


bool Get_KL17_Version(void)
{
	uint8_t buf=0;
	nrf_delay_ms(400);
	
	if(Spi_Send_MSG(GET_KL17_VERSION, &buf,1,KL17_VER,NULL)!=SPI_RSP_DATA)
		return false;
	return true;
}



bool IsOTAFontRight= false;
bool Set_OTALanguageInfo(void)
{
	uint8_t buf[4]={0x00};
	uint8 Ota_buf[4]={0x00};
	uint8 Resp_cmd = SPI_RSP_SUCCED;
	uint8 len = 1;
	buf[0]=OTALanguagetype;
	buf[1]=LastLanguage;
	buf[2]=OTALanguageVersion[0];
	buf[3]=OTALanguageVersion[1];
	Spi_Send_MSG(OTA_LANGUAGE_INFO, buf,4,&Resp_cmd,&len);
	if(Resp_cmd !=SPI_RSP_SUCCED)
	{
		if(SetValue.Language<=LAN_KOREAN && SetValue.Language>=LAN_CHINESE_SIMPLIFID)
			SetValue.Language = LAN_ENGLISH;
		memset(Ota_buf,0,4);
		Force_NordicFlash();
		SpiFlash_4KSector_Erase(Font_Base);
		SpiFlash_Write(Font_Base, Ota_buf, 4);
		GetOTALanguageInfo();
		Free_NordicFlash();
		IsOTAFontRight= false;
		return false;
	}
	IsOTAFontRight = true;
	return true;
}

bool PowerOffSystem(void)
{
	uint8_t buf =0x00;
	if(Spi_Send_MSG(POWER_OFF_SYSTEM, &buf,1,NULL,NULL)!=SPI_RSP_SUCCED)
		return false;
	return true;	
}

bool ResumeBacklight(void)
{
	if(OPEN_BACKLIGHT_FLAG)
	{
		uint8_t buf =0x00;
		if(Spi_Send_MSG(RESUME_BACKLIGHT, &buf,1,NULL,NULL)==SPI_RSP_SUCCED)
		{
			OPEN_BACKLIGHT_FLAG=false;
			return true;	
		}
	}
	return false;
}

bool NordicPM(NordicPmType pm)
{	
	uint8_t Sbuf=pm;
	if(pm==N_SUSPEND)
		Force_NordicFlash();
	else
		OPEN_BACKLIGHT_FLAG =true;
	if(Spi_Send_MSG(NORDIC_PM, &Sbuf,1,NULL,NULL)!=SPI_RSP_SUCCED)
		return false;
	if(pm==N_SUSPEND)
	{
		LCD_ENABLE=false;
		OPEN_BACKLIGHT_FLAG =false;
		if(FORCE_NORDIC_FLASH_FLAG)
			SUSPEND_NORDIC_USE_FLASH=true;
	}
	else if(pm==N_RESUME)	
	{
		if(FORCE_NORDIC_FLASH_FLAG)
			SUSPEND_NORDIC_USE_FLASH=false;	
		Free_NordicFlash();
		LCD_ENABLE=true;
		if(SCAN_KEY_ENABLE==false)
		{
			zinitix_resume();
			StartKeyScan();
			SetScanKeyValidFlag();
			CancelSendSuspendEvent();
			SetSystemSuspendTime(DISPLAY_TIME_10S_FLAG);	
		}		
	}
	return true;
}


bool SPI_LCD_Enable(void)
{
	if(DISPLAY_GOTO_SLEEP_FLAG==false)			/*系统唤醒，才显示*/
		LCD_DISPLAY_FLAG = true;
	else
		return false;
	
	if(!LCD_ENABLE)
	{
		if(!NordicPM(N_RESUME))					/*系统唤醒，背光关闭，先打开背光*/
			return false;
	}	
	return true;
}

/*Flash操作*/
bool Flash_Write(uint32 addr,uint8*pBuf, uint16 len)
{
	uint8 buf[260]={0x00};
	uint32_encode(addr,buf);
	if(len>256)
		len=256;
	memcpy((buf+4),pBuf,len);
	if(Spi_Send_MSG(FLASH_WRITE,buf,(len+4),NULL,NULL)!=SPI_RSP_SUCCED)
		return false;
	return true;	
}

bool Flash_Read(uint32 addr,uint8*pBuf, uint16 len)
{
	uint8 buf[6]={0x00};
	uint8 rlen  =0;
	uint32_encode(addr,buf);
       uint16_encode(len,(buf+4));
	if(Spi_Send_MSG(FLASH_READ,buf,6,pBuf,&rlen)!=SPI_RSP_DATA)
		return false;
	return true;	
}

bool Flash_Erase(uint32 addr,FlashErase Fe)
{
	uint8 buf[5]={0x00};
	uint32_encode(addr,buf);	
	buf[4]=Fe;
	if(Spi_Send_MSG(FLASH_ERASE,buf,5,NULL,NULL)!=SPI_RSP_SUCCED)
		return false;
	return true;	
}

bool Flash_Set_Access(FlashAccess Fa)
{
	uint8 buf = Fa;
	if(Spi_Send_MSG(FLASH_ACCESS,&buf,1,NULL,NULL)!=SPI_RSP_SUCCED)
		return false;
	return true;
}

bool Flash_Display_Ancs_LayOut(uint32 addr, uint8*LayOut, uint8 LayOutIndex,uint8 Detail,DetailPageDir dir)
{
	if(SPI_LCD_Enable()==false)		/*SPI LCD显示前需调用*/
		return false;
	uint8 buf[4+ANCS_LAYOUT_MAX+3]={0x00};
	uint8 Dend =0;
	uint32_encode(addr,buf);
	memcpy((buf+4),LayOut,ANCS_LAYOUT_MAX);
	buf[4+ANCS_LAYOUT_MAX]=LayOutIndex;
	buf[4+ANCS_LAYOUT_MAX+1]=Detail;
	buf[4+ANCS_LAYOUT_MAX+2]=(uint8)dir;
	if(Spi_Send_MSG(FLASH_ANCS_DISPLAY,buf,4+ANCS_LAYOUT_MAX+3,NULL,NULL)!=SPI_RSP_DATA)
		return false;
	if(Dend==0x01)
		ReadEnd = 1;	
	ResumeBacklight();
	return true;	
}


/*显示主界面*/
bool LCD_UpdateStatus(void)
{
	if(SPI_LCD_Enable()==false)		/*SPI LCD显示前需调用*/
		return false;
	uint16 buf=gDisplayStatus.u16DMS;
	if(Spi_Send_MSG(LCD_UPDATE_STATUS, (uint8*)&buf,2,NULL,NULL)!=SPI_RSP_SUCCED)
		return false;
	ResumeBacklight();
	return true;	
}

bool LCD_UpdateTime(UTCTimeStruct* NTime)
{
	uint8 buf[7]={0x00};
	uint16_encode(NTime->year,buf);
	buf[2] = NTime->month;
	buf[3] = NTime->day;
	buf[4] = NTime->hour;
	buf[5] = NTime->minutes;
	buf[6] =GetMonthWeekDays(NTime->year,NTime->month,NTime->day);  //0~6(日~六)//	
	if(Spi_Send_MSG(LCD_UPDATE_TIME, buf,7,NULL,NULL)!=SPI_RSP_SUCCED)
		return false;
	return true;	
}

bool LCD_UpdateTimeFormat(void)
{
	uint8 buf[3]={0x00};
	buf[1]=TimeOrder.TimeFormat;
	buf[2]=TimeOrder.DateFormat;
	if(Spi_Send_MSG(LCD_UPDATE_TIME_FORMAT, buf,3,NULL,NULL)!=SPI_RSP_SUCCED)
		return false;
	return true;
}

bool LCD_MainDataInterface(MainDataI Mi,uint8* Data, uint32 len)
{
	if(SPI_LCD_Enable()==false)		/*SPI LCD显示前需调用*/
		return false;
	uint8 buf[128]={0x00};
	
	if(len!=0)
	{
		if(len>127)
			len =127;
		memcpy(buf+1,Data,len);
	}
	buf[0]=Mi;
	if(Spi_Send_MSG(LCD_MAIN_DATA_INTERFACE, buf,(len+1),NULL,NULL)!=SPI_RSP_SUCCED)
		return false;
	ResumeBacklight();
	return true;	
}
/*分步--准备好显示--再开背光*/
bool LCD_MainDataInterface_SetBL_Step(MainDataI Mi,uint8* Data, uint32 len,uint8 Step)
{
	if(SPI_LCD_Enable()==false)		/*SPI LCD显示前需调用*/
		return false;
	uint8 buf[128]={0x00};
	
	if(len!=0)
	{
		if(len>127)
			len =127;
		memcpy(buf+1,Data,len);
	}
	buf[0]=Mi;
	if(Spi_Send_MSG(LCD_MAIN_DATA_INTERFACE, buf,(len+1),NULL,NULL)!=SPI_RSP_SUCCED)
		return false;
	
	if(Step==1)
		ResumeBacklight();
	return true;	
}

/*不需要显示，但是需要在屏幕唤醒时调用*/
bool LCD_MainDataInterface_NoDisplay(MainDataI Mi,uint8* Data, uint32 len)
{
	if(LCD_ENABLE)
	{
		uint8 buf[128]={0x00};
		if(len!=0)
		{
			if(len>127)
				len =127;
			memcpy(buf+1,Data,len);
		}
		buf[0]=Mi;
		if(Spi_Send_MSG(LCD_MAIN_DATA_INTERFACE, buf,(len+1),NULL,NULL)!=SPI_RSP_SUCCED)
			return false;
	}
	return true;	
}

bool LCD_MusicInterface(MusicUI Mi)
{
	if(SPI_LCD_Enable()==false)		/*SPI LCD显示前需调用*/
		return false;
	ResumeBacklight();
	return true;		
}

bool LCD_PictureInterface(PictureUI Pi)
{
	if(SPI_LCD_Enable()==false)		/*SPI LCD显示前需调用*/
		return false;
	uint8 buf=Pi;
	if(Spi_Send_MSG(LCD_PICTURE_INTERFACE, &buf,1,NULL,NULL)!=SPI_RSP_SUCCED)
		return false;
	ResumeBacklight();
	return true;		
}

bool LCD_PhoneInterface(PhoneUI Pi)
{
	if(SPI_LCD_Enable()==false)		/*SPI LCD显示前需调用*/
		return false;
	ResumeBacklight();
	return true;	
}


bool LCD_MsgInterface(MsgUI Mi,uint8 count)
{
	if(SPI_LCD_Enable()==false)		/*SPI LCD显示前需调用*/
		return false;
	uint8 buf[7]={0x00};
	buf[0]=Mi;
	buf[1]=count;

	UTCTimeStruct uTime;
	gOS_GetUTCTime(&uTime); 
	RefreshBatteryInfo();
	
	buf[2] = uTime.hour;
	buf[3] = uTime.minutes;
	buf[4] = TimeOrder.TimeFormat;	
	buf[5] = shortcut_info.battery_level;
	buf[6] = shortcut_info.battery_percent;

	if(Spi_Send_MSG(LCD_MSG_INTERFACE, buf,7,NULL,NULL)!=SPI_RSP_SUCCED)
		return false;
	ResumeBacklight();
	return true;	
}


bool LCD_MsgSecInterface(MsgSecUI Msi,uint8* data)
{
	if(SPI_LCD_Enable()==false)		/*SPI LCD显示前需调用*/
		return false;
	ResumeBacklight();
	return true;		
}

bool LCD_DevCommInterface(DevCommUI Di,uint8* data)
{
	if(SPI_LCD_Enable()==false)		/*SPI LCD显示前需调用*/
		return false;
	ResumeBacklight();
	return true;		
}

bool LCD_StopWatchInterface(StopWatchUI Si)
{
	if(SPI_LCD_Enable()==false)		/*SPI LCD显示前需调用*/
		return false;
	uint8 buf=Si;
	if(Spi_Send_MSG(LCD_STOP_WATCH_INTERFACE, &buf,1,NULL,NULL)!=SPI_RSP_SUCCED)
		return false;
	ResumeBacklight();
	return true;		
}

bool LCD_VoIumeInterface(uint8  Vol)
{
	if(SPI_LCD_Enable()==false)		/*SPI LCD显示前需调用*/
		return false;
	uint8 buf=Vol;
	if(Spi_Send_MSG(LCD_VOLUME_INTERFACE, &buf,1,NULL,NULL)!=SPI_RSP_SUCCED)
		return false;
	ResumeBacklight();
	return true;		
}

bool LCD_HeartRateInterface(uint8  Hr)
{
	if(SPI_LCD_Enable()==false)		/*SPI LCD显示前需调用*/
		return false;
	uint8 buf=Hr;
	if(Spi_Send_MSG(LCD_HEART_RATE_INTERFACE, &buf,1,NULL,NULL)!=SPI_RSP_SUCCED)
		return false;
	ResumeBacklight();
	return true;		
}


bool LCD_CameraAndSearchInterface(CameraAndSearchUI Ci)
{
	if(SPI_LCD_Enable()==false)		/*SPI LCD显示前需调用*/
		return false;
	uint8 buf=Ci;
	if(Spi_Send_MSG(LCD_CAS_INTERFACE, &buf,1,NULL,NULL)!=SPI_RSP_SUCCED)
		return false;
	ResumeBacklight();
	return true;		
}


bool Flash_Display_PhBook_Inf(uint32 Inf_addr,uint8 Inf_Cnt,uint32 Grp_addr,uint8 Grp_Cnt,uint8 Percent_Showed)
{
	if(SPI_LCD_Enable()==false)		/*SPI LCD显示前需调用*/
		return false;
	uint8 buf[15]={0x00};
	uint32_encode(Inf_addr,buf);
	uint32_encode(Grp_addr,buf+5);
	buf[4]=Inf_Cnt;
	buf[9]=Grp_Cnt;
	buf[10] = Percent_Showed;
	if(Spi_Send_MSG(FLASH_PHONEBOOK_DISPLAY,buf,11,NULL,NULL)!=SPI_RSP_SUCCED)
		return false;
	ResumeBacklight();
	return true;	
}

bool Flash_Display_PhonePrepare(uint32 Paddr,uint8 NameLen,uint8 NumLen, uint8*Num, bool IsAndroid)
{
	if(SPI_LCD_Enable()==false)		/*SPI LCD显示前需调用*/
		return false;
	uint8 buf[32]={0x00};
	if(IsAndroid)
		buf[0] =0x01;
	else
		buf[0] =0x00;
	buf[1] = NameLen;
	buf[2] = NumLen;
	uint32_encode(Paddr,buf+3);
	if((Num!=NULL)&&(NumLen>0)&&(NumLen<20))
		memcpy((buf+7),Num,NumLen);
	if(Spi_Send_MSG(FLASH_PHONEPREPARE,buf,(7+NumLen),NULL,NULL)!=SPI_RSP_SUCCED)
		return false;
	ResumeBacklight();
	return true;	
}

