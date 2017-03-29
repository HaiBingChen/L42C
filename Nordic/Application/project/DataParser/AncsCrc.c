
#include "headfile.h"

#define ANCS_CRC_COUNTER_MAX	1024

static ANCS_CRC Acrc[ANCS_CRC_COUNTER_MAX];
static uint16 APos =0;


static uint16 Ancs_crc16_Compute(uint8 * p_data, uint8 size)
{
    uint8 i;
    uint16 crc = 0xffff;
    for (i = 0; i < size; i++)
    {
        crc  = (unsigned char)(crc >> 8) | (crc << 8);
        crc ^= p_data[i];
        crc ^= (unsigned char)(crc & 0xff) >> 4;
        crc ^= (crc << 8) << 4;
        crc ^= ((crc & 0xff) << 4) << 1;
    }
    return crc;
}

void AncsSaveToFlash(void)
{
	ANCS_SAVE_TYPE AType =Ancs_Null;
	uint8 TitleLen=0,MsgLen=0,DateLen=0;
	uint8* Ddata =NULL;
	bool crcCheck =true;
	bool saveCheck =true;
	bool fineNull =true;
	if(AncsData.AncsData[0]==Ancs_Social_Gmail)		/*Gmail分类为邮件*/
	{
		AncsData.AncsData[0]=Ancs_Email;
		AncsData.msg=MSG_E_MAIL_NOTIFY;
	}	
	AType = (ANCS_SAVE_TYPE)(AncsData.AncsData[0]);
	TitleLen = AncsData.AncsData[1];	
	MsgLen = AncsData.AncsData[2];	
	DateLen = AncsData.AncsData[3];
	Ddata = AncsData.AncsData+TitleLen+MsgLen+4;
	if(DateLen!=15 && AType != Ancs_IncomingCall)
		return;
	switch(AType)
	{
		case Ancs_IncomingCall:						/*来电*/
			if(!SWITCH_ANCS_INCOMING_CALL || !INCOMMING_CALL_ADD)
			{
				saveCheck = false;
			}
			crcCheck = false;
			break;
		case Ancs_MissedCall:							/*未接来电*/
			if(!SWITCH_ANCS_MISSED_CALL)
			{
				saveCheck = false;
				crcCheck = false;
			}			
			break;
		case Ancs_Sms:								/*短信*/
			if(!SWITCH_ANCS_SMS)
			{
				saveCheck = false;
				crcCheck = false;
			}
			break;
		case Ancs_Email:								/*邮件*/		
			if(!SWITCH_ANCS_EMIL)
			{
				saveCheck = false;
				crcCheck = false;				
			}
			break;
		case Ancs_Social_Facebook:					/*社交*/
		case Ancs_Social_Gmail:
		case Ancs_Social_Hangouts:
		case Ancs_Social_Instagram:
		case Ancs_Social_Line:
		case Ancs_Social_Linkedin:
		case Ancs_Social_Messenger:
		case Ancs_Social_QQ:
		case Ancs_Social_Skype:
		case Ancs_Social_Snapchat:
		case Ancs_Social_Twitter:
		case Ancs_Social_Uber:
		case Ancs_Social_Viber:
		case Ancs_Social_Wechat:
		case Ancs_Social_WhatsApp:
			if(!SWITCH_ANCS_SOCIAL)	
			{
				saveCheck = false;
				crcCheck = false;					
			}
			break;
		case Ancs_Schedule:
			if(!SWITCH_ANCS_SCHEDULE)
			{
				saveCheck = false;
				crcCheck = false;
			}
			break;
	}
	
	if(crcCheck)	
	{
		uint16 msg_Crc=0x00;
		uint16 date_Crc=0x00;		
		msg_Crc =Ancs_crc16_Compute(AncsData.AncsData,TitleLen+MsgLen+DateLen+4);
		date_Crc =Ancs_crc16_Compute(Ddata,15);
		for(uint16 i =0;i<ANCS_CRC_COUNTER_MAX;i++)
		{
			if(Acrc[i].msgCrc==msg_Crc)
			{
				if(Acrc[i].dateCrc==date_Crc)
				{	
					fineNull =false;
					break;
				}
				
			}
		}
		if(fineNull)
		{
			if(APos>=ANCS_CRC_COUNTER_MAX)
			{
				APos=0;
			}
			Acrc[APos].msgCrc = msg_Crc;
			Acrc[APos].dateCrc= date_Crc; 		
			APos++;
		}		
	}
	if(fineNull)
	{
		switch(AType)
		{
			case Ancs_IncomingCall:						/*来电*/
				if(SWITCH_ANCS_INCOMING_CALL&&INCOMMING_CALL_ADD)
				{
					if(gIncomingCallCount < 255)
						gIncomingCallCount += 1;						
				}
				break;
			case Ancs_MissedCall:							/*未接来电*/
				if(SWITCH_ANCS_MISSED_CALL)
				{
					if(gMissedCallCount < 255)
						gMissedCallCount += 1;
				}			
				break;
			case Ancs_Sms:								/*短信*/
				if(SWITCH_ANCS_SMS)
				{
					if(gShortMsgCount < 255)
						gShortMsgCount += 1;
				}
				break;
			case Ancs_Email:								/*邮件*/		
				if(SWITCH_ANCS_EMIL)
				{
					if(gEMailCount < 255)
						gEMailCount += 1;			
				}
				break;
			case Ancs_Social_Facebook:					/*社交*/
			case Ancs_Social_Gmail:
			case Ancs_Social_Hangouts:
			case Ancs_Social_Instagram:
			case Ancs_Social_Line:
			case Ancs_Social_Linkedin:
			case Ancs_Social_Messenger:
			case Ancs_Social_QQ:
			case Ancs_Social_Skype:
			case Ancs_Social_Snapchat:
			case Ancs_Social_Twitter:
			case Ancs_Social_Uber:
			case Ancs_Social_Viber:
			case Ancs_Social_Wechat:
			case Ancs_Social_WhatsApp:
				if(SWITCH_ANCS_SOCIAL)	
				{
					if(gSocialCount < 255)
						gSocialCount += 1;				
				}
				break;
			case Ancs_Schedule:
				if(SWITCH_ANCS_SCHEDULE)
				{
					if(gScheduleCount<255)
						gScheduleCount+=1;
				}		
				break;
		}	
	}
	if(saveCheck && fineNull)	/*保存ANCS消息到Flash*/
	{
		SaveAncsMsg(AType,AncsData.AncsData,TitleLen+MsgLen+DateLen+4);
		/*发送消息LoadHandler*/	
		if(AncsData.TimeOut>0)
			osal_start_timerEx(GetAppTaskId(), AncsData.msg, AncsData.TimeOut);  
		else
			SendMsg((MsgType)(AncsData.msg));		
	}	
}



