
/******************************************************************************************
*�ļ�����		BC5CmdPackage.c
*˵����			L18ͨѶЭ�����ݽ���ʵ���ļ�
*��д:			statdream
*�汾��Ϣ:		V1.0
******************************************************************************************/
#include "headfile.h"

L30_CALL_STATE L30CallState = L30_CALL_STATE_FIRST_CALL_INCOMING;
bool  SkipCallerID = FALSE;
bool  GetCallerIDOnce = TRUE;
uint8  BC5_State=0xFF;
uint8 bc5_volume = 9;
uint8 bc5_ring_mode = BC5_RING;
uint8 bc5_music_play = 0;
uint8 LastCallCmd =0;
uint8 LastCallCmdCount =0;
uint8 LastSiriCmdCount =0; 
uint8 LastVolSetCount =0; 
uint8 LastRingModeCount =0; 
bool  SwitchCall = false;
uint8 oldVolset =0xFF;
static const BC5_DATA_CMD_T Bc5DataCmdTable[]={
/*������,�������BUFF*/	
	{0x14,	GetBC5AudioVolCB},		
	{0x50,	SetBC5BtStateCB},
	{0x51,	SetBC5PhoneStateCB},
	{0x52,	SetBC5AudioVollCB},
	{0x53,	SetBC5RingModeCB},
	{0x54,	SetBC5StandbyCB},
	{0x55,	SetBC5StopRingToneCB},	
	{0x56,	SetBC5StartSiriCB},
	{0x57,	SetBC5TonesCB},
	{0x58,	SetBC5MicCB},
	{0x59,	SetBC5DtmfNumberCB},
		
/*������,���뷵��ֵ��BC5*/
	{0x70,	NotifyBc5BtInfoCB},
	{0x71,	NotifyBc5PhoneInfoCB},
	{0x72,	NotifyBc5CallerInfoCB},
	{0x73,	NotifyBc5SiriInfoCB},
	{0xa0,	SppTransmissionCB}
};
const uint16 Bc5DataCmdTableLen=sizeof(Bc5DataCmdTable)/sizeof(BC5_DATA_CMD_T);

static void BC5_UartSendData(uint8 cmd,uint8*pData,uint8 len)
{
	uint8 sData[32]={0x00};
	uint8 data =0x00;
	if(pData==NULL)
	{
		pData = &data;
		len=1;
	}
	if(len>27)
		len = 27;
    	sData[0] = 0xff;
   	sData[1] = cmd;	
	sData[2] = len;
	memcpy((sData+3),pData,len);
	sData[3+len]= 0x0a;	
	sData[4+len]= 0x0d;	
	UART_Send(sData, len+5);
}

static void BC5_Response(uint8 cmd, uint8 ops, uint8 rsp)
{
	uint8 sData[7]={0x00};
    	sData[0] = 0xff;
   	sData[1] = cmd;	
	sData[2] = 2;
	sData[3] = ops;
	sData[4] = rsp;	
	sData[5]= 0x0a;	
	sData[6]= 0x0d;	
	UART_Send(sData, 7);		
}

uint8 Bc5DataParseHandler(const void *InData)
{
	uint16 i=0;
	uint8* Data = (uint8*)InData;
	uint8 DataOffset=0,DataLen =0,DataCmd=0;
	for(i=0;i<DATA_REV_MAX;i++)
	{
		if(*(Data+DataOffset)==0xff)
		{
			DataLen =*(Data+DataOffset+2); 
			if((*(Data+DataOffset+DataLen+3))==0x0a && (*(Data+DataOffset+DataLen+4))==0x0d)	
			{
				DataCmd = *(Data+DataOffset+1); 
				break;
			}
		}
		DataOffset++;
	 }
	if(DataCmd==0)
		return 0;
	for(i=0;i<Bc5DataCmdTableLen;i++)
	{
		if(DataCmd == Bc5DataCmdTable[i].Bc5Cmd)
		{
			if(Bc5DataCmdTable[i].pFn((Data+DataOffset+3),DataLen)==BC5_RSP)
				BC5_Response(DataCmd,Data[DataOffset+3],0);				
		}
	}
	return 1;
}


/*������: (��ȡ) BC5����ֵ*/
void BC5_QueryVolumeCmd(void)
{
	BC5_UartSendData(BC5_GET_VOLUME_CMD,NULL,1);
}
uint8 GetBC5AudioVolCB(uint8 *pData,uint8 len)
{
  	bc5_volume = *pData;
	SendMsg(MSG_BC5_VOLUME_CHANGE);	
	return BC5_RSP_NULL;
}

/*������:(����) ����������*/
void BC5_SetBtPairCmd(uint8 cmdData)
{
	BC5_UartSendData(BC5_SET_BT_PAIR_CMD, &cmdData,1);
}

uint8 SetBC5BtStateCB(uint8 *pData,uint8 len)
{
	return BC5_RSP_NULL;
}

/*������:(����) ͨ�����*/
void BC5_SetCallOpCmd(uint8 cmdData)
{ 
	if(cmdData == BC5_CALL_AGAIN){
		LastCallCmdCount++;
		BC5_UartSendData(BC5_SET_CALL_OP_CMD, &LastCallCmd,1);
	}
	else{
		LastCallCmd = cmdData;
		LastCallCmdCount = 0;
		if(cmdData != BC5_CALL_SWAP_ACTIVE)
		SwitchCall = false;
		BC5_UartSendData(BC5_SET_CALL_OP_CMD, &cmdData,1);
	}
	if(LastCallCmdCount<10)
		osal_start_timerEx(GetAppTaskId(), MSG_CALL_SEND_CMD_AGAIN, 500);
}

void BC5_SetCallRedialNumber(uint8 cmdData)
{ 
	uint8 SendBuf[24]={0x00};
	SendBuf[0]=BC5_CALL_REDIAL_NUMBER;
	uint8 len = strlen((char*)DialNumber);
	if(len>0)
	{
		memcpy(SendBuf+1,DialNumber,len)	;
		if(cmdData == BC5_CALL_AGAIN){
			LastCallCmdCount++;
			BC5_UartSendData(BC5_SET_CALL_OP_CMD, SendBuf,len+1);
		}
		else{
			LastCallCmd = cmdData;
			LastCallCmdCount = 0;
			BC5_UartSendData(BC5_SET_CALL_OP_CMD, SendBuf,len+1);
		}
		if(LastCallCmdCount<10)
			osal_start_timerEx(GetAppTaskId(), MSG_CALL_SEND_CMD_AGAIN, 500);	
	}
}

uint8 SetBC5PhoneStateCB(uint8 *pData,uint8 len)
{
	osal_stop_timerEx(GetAppTaskId(), MSG_CALL_SEND_CMD_AGAIN);
	if(SwitchCall)
	{
		SendMsg(MSG_SWITCH_CALL);
		SwitchCall = false;
	}
	return BC5_RSP_NULL;
}

/*������:(����) ����*/
void BC5_SetVolumeCmd(uint8 cmdData)
{
	if(cmdData == BC5_CALL_AGAIN){
		LastVolSetCount++;
		uint8 VolLev =get_volume_level();
		if(oldVolset!=VolLev)
			BC5_UartSendData(BC5_SET_VOLUME_CMD, &VolLev,1);
		else
			LastVolSetCount=10;
	}
	else
	{
		LastVolSetCount = 0;
		if(oldVolset!=cmdData)	
			BC5_UartSendData(BC5_SET_VOLUME_CMD, &cmdData,1);
		else
			LastVolSetCount=10;
	}
	if(LastVolSetCount<10)
		osal_start_timerEx(GetAppTaskId(), MSG_VOL_SET_CMD_AGAIN, 200);
}

uint8 SetBC5AudioVollCB(uint8 *pData,uint8 len)
{
	oldVolset = pData[0];
	osal_stop_timerEx(GetAppTaskId(), MSG_VOL_SET_CMD_AGAIN);
	return BC5_RSP_NULL;
}

/*������:(����) ����ģʽ*/
void BC5_SetRingModeCmd(uint8 cmdData)
{
	if(cmdData == BC5_CALL_AGAIN){
		LastRingModeCount++;
		BC5_UartSendData(BC5_SET_RING_MODE_CMD, &bc5_ring_mode,1);
	}
	else
	{
		LastRingModeCount = 0;
		BC5_UartSendData(BC5_SET_RING_MODE_CMD, &cmdData,1);
	}
	if(LastRingModeCount<10)
		osal_start_timerEx(GetAppTaskId(), MSG_RING_MODE_CMD_AGAIN, 200);
}
uint8 SetBC5RingModeCB(uint8 *pData,uint8 len)
{
	osal_stop_timerEx(GetAppTaskId(), MSG_RING_MODE_CMD_AGAIN);
	return BC5_RSP_NULL;
}

/*������:(����) ����ģʽ*/
void BC5_SetStandbyModeCmd(uint8 cmdData)
{
	BC5_UartSendData(BC5_SET_STANDBY_MODE_CMD, &cmdData,1);
}
uint8 SetBC5StandbyCB(uint8 *pData,uint8 len)
{
	if(pData[0]==0x01)
	{
		osal_stop_timerEx(GetDisplayTaskId(), DISPLAY_BC5_SUSPEND_STATE);
		//if(Powerstate.standbystate== true)
			//UartPowerOff();
	}
	else if(pData[0]==0x00)
	{
		osal_stop_timerEx(GetDisplayTaskId(), DISPLAY_BC5_CONNECT_STATE);
	}
	return BC5_RSP_NULL;
}

/*������:(����) ֹͣ����*/
void BC5_SetStopRingToneCmd(uint8 cmdData)
{
	BC5_UartSendData(BC5_SET_STOP_RING_TONE, &cmdData,1);
}
uint8 SetBC5StopRingToneCB(uint8 *pData,uint8 len)
{
	return BC5_RSP_NULL;
}

/*������:(����) �򿪹ر�siri*/
void BC5_SetSiri_Cmd(uint8 cmdData)
{
	if(cmdData == BC5_CALL_AGAIN){
		LastSiriCmdCount++;
		uint8 Bc5Siri =BC5_SIRI_OPEN;
		BC5_UartSendData(BC5_SET_SIRI_CMD, &Bc5Siri,1);
	}
	else
	{
		LastSiriCmdCount = 0;
		BC5_UartSendData(BC5_SET_SIRI_CMD, &cmdData,1);
	}
	if(LastSiriCmdCount<10)
		osal_start_timerEx(GetAppTaskId(), MSG_SIRI_SEND_CMD_AGAIN, 200);
}
uint8 SetBC5StartSiriCB(uint8 *pData,uint8 len)
{
	osal_stop_timerEx(GetAppTaskId(), MSG_SIRI_SEND_CMD_AGAIN);
	return BC5_RSP_NULL;
}

/*������:(����) ������������*/
void BC5_SetTonesCmd(uint8 cmdData)
{
	BC5_UartSendData(BC5_SET_TONES_CMD, &cmdData,1);
}
uint8 SetBC5TonesCB(uint8 *pData,uint8 len)
{
	return BC5_RSP_NULL;
}

/*������:(����) ������ֹMIC*/
void BC5_SetMicCmd(uint8 cmdData)
{
	BC5_UartSendData(BC5_SET_MIC_CMD, &cmdData,1);
}
uint8 SetBC5MicCB(uint8 *pData,uint8 len)
{
	return BC5_RSP_NULL;
}

/*������:(����) ������ֹMIC*/
void BC5_SendDtmfNumber(uint8 dtmf)
{
	BC5_UartSendData(BC5_SET_DTMF_NUMBER, &dtmf,1);
}
uint8 SetBC5DtmfNumberCB(uint8 *pData,uint8 len)
{
	return BC5_RSP_NULL;
}

/*������:(֪ͨ) ����������*/
uint8 NotifyBc5BtInfoCB(uint8 *pData,uint8 len)
{	
	uint16 time=100;
	BC5_Response(BC5_EVENT_BT_PAIR_STATE,*pData,0);
	if(BC5_State != (*pData))
	{
		BC5_State =*pData;
		if(BC5_State == BC5_BT_READY)
		{
			Bc5ConnectToneFlag += 1;
			osal_stop_timerEx(GetAppTaskId(), MSG_BC5_BT_CHANGE);
			osal_stop_timerEx(GetAppTaskId(), MSG_BC5_DISCONNECT);
			oldVolset=0xff;
			BC5_SetVolumeCmd(get_volume_level());  /* ��������*/  	
			SendMsg(MSG_UART0_READY);
		}
		else if(BC5_State == BC5_BT_CONNECT_OK)
		{
			BC5_SetRingModeCmd(bc5_ring_mode); 
			osal_stop_timerEx(GetAppTaskId(), MSG_BC5_BT_CHANGE);
			if((Bc5ConnectToneFlag==1) &&(osal_stop_timerEx(GetAppTaskId(), MSG_BC5_DISCONNECT)!=SUCCESS))
			BC5_SetTonesCmd(BC5_CONNECTED_TONE);		
			Bc5ConnectToneFlag = 0;
		}
		else if((BC5_State == BC5_BT_RECONNECTED) ||(BC5_State == BC5_BT_DISCONNECT))
		{
			time = 1000;
			Bc5ConnectToneFlag = 1;
		}
		else if(BC5_State ==BC5_BT_RECONNECTED_FAIL)
		{
			return BC5_RSP_NULL;
		}
		osal_start_timerEx(GetAppTaskId(), MSG_BC5_BT_CHANGE, time);
	}
	return BC5_RSP_NULL;
}


/*������:(֪ͨ) ͨ��״̬���*/
void BC5_TO_CC2540_CallState( BC5_CALL_STATE_E state)
{
    switch(state){
        case BC5_CALL_STATE_HANGUP:	
	     L30CallState = L30_CALL_STATE_FIRST_CALL_INCOMING;
	     SkipCallerID = FALSE;
	     GetCallerIDOnce = TRUE;
	     osal_stop_timerEx(GetAppTaskId(), MSG_CALL_STATE_LOAD_UNKNOW_CALL);
	     osal_stop_timerEx(GetAppTaskId(), MSG_STOP_INCOMING_TONE);
	     /*������ͨ��*/
	     SendMsg(MSG_CALL_STATE_IDLE);
	     break;
        case BC5_CALL_STATE_FIRST_CALLING:
	     FORCE_SUSPEND_FLAG = false;
	     if(L30CallState ==L30_CALL_STATE_FIRST_CALL_INCOMING){
	         SkipCallerID = FALSE;
		  osal_start_timerEx(GetAppTaskId(), MSG_CALL_STATE_LOAD_UNKNOW_CALL, 8000);
	     }
	     else if(L30CallState ==L30_CALL_STATE_SECOND_CALL_INCOMING)
	         SkipCallerID = TRUE;     
	     break;
        case BC5_CALL_STATE_FIRST_TALKING: 
	     if(L30CallState == L30_CALL_STATE_FIRST_CALL_INCOMING){
	          /*��ͨ��һͨ��*/	 
		  osal_stop_timerEx(GetAppTaskId(), MSG_STOP_INCOMING_TONE);
	         SendMsg(MSG_CALL_STATE_ONE_CALL_TALKING);
	     }else if(L30CallState == L30_CALL_STATE_SECOND_CALL_INCOMING){
	         if(SkipCallerID == TRUE){
		      SkipCallerID = FALSE;
                    /*�ҵ�һ�ӵڶ�*/
		      osal_start_timerEx(GetAppTaskId(), MSG_CALL_STATE_ANSWER_SECOND_CALL_FIRST_CALL_END, 800);	
		  }else{
                    /*�ҵڶ�*/
		      osal_start_timerEx(GetAppTaskId(), MSG_CALL_STATE_SECOND_CALL_INCOMING_END, 800);
		  }			 	
	     }else if(L30CallState == L30_CALL_STATE_TWO_CALL_TALKING){
	          /*�����ҵ���ǰ�򱻹ҵ�����*/
	         SendMsg(MSG_CALL_STATE_TWO_CALL_TALKING_HOLD_END);
	     }else if(L30CallState == L30_CALL_STATE_ONE_CALL_REMAINING){
	         /*���ҵ���ǰ*/
		  SendMsg(MSG_CALL_STATE_TWO_CALL_TALKING_ACTIVE_END);
	     }
	     L30CallState = L30_CALL_STATE_ONE_CALL_TALKING;
	     break;
        case BC5_CALL_STATE_TWO_CALLING:
	     L30CallState = L30_CALL_STATE_SECOND_CALL_INCOMING;
	     break;
        case BC5_CALL_STATE_TWO_TALKING:	
	     L30CallState = L30_CALL_STATE_TWO_CALL_TALKING;
	     /*��ͨ�ڶ�·ͨ��*/
	     SendMsg(MSG_CALL_STATE_TWO_CALL_TALKING);
	     break;
        case BC5_CALL_STATE_ONE_HOLDING:	
	     L30CallState = L30_CALL_STATE_ONE_CALL_REMAINING;
	     BC5_SetCallOpCmd(BC5_CALL_SWAP_ACTIVE);     //�л�
	     break;
	 case BC5_CALL_STATE_OUTGOING:
	     ChargeDisplayStateChange(false);
	     SendMsg(MSG_CALL_STATE_OUT_GOING);
	     break;
	 default:
	     break;
    }
}

uint8 NotifyBc5PhoneInfoCB(uint8 *pData,uint8 len)
{
  	uint8 uData=*pData;
	BC5_Response(BC5_EVENT_CALL_STATE,uData,0);
  	BC5_TO_CC2540_CallState((BC5_CALL_STATE_E)uData);	
	return BC5_RSP_NULL;
}


/*������:(֪ͨ) ͨ��״̬���*/
uint8 NotifyBc5CallerInfoCB(uint8 *pData,uint8 len)
{
	if(len>0)
	{
		uint8 index=*pData;
		if(index == 0){     
		if(SkipCallerID == TRUE){
			SkipCallerID = FALSE;
				/*�ڶ�����ʱ��һͨ������*/
				SendMsg(MSG_CALL_STATE_SECOND_CALL_INCOMING_FIRST_CALL_END);
			}else{
				/*Load ��һ����*/
				if(GetCallerIDOnce == TRUE){
					GetCallerIDOnce =FALSE;
					if(osal_stop_timerEx(GetAppTaskId(), MSG_CALL_STATE_LOAD_UNKNOW_CALL)==SUCCESS){
						//ChargeDisplayStateChange(false);
						SendMsg(MSG_CALL_STATE_LOAD_FIRST_CALL);
						//if(Syncing_Handler_flag == FALSE)
						LoadPhoneHandler(PHONE_RING,pData,1);
					}else{
						if(CheckAndReplaceUnknownNumber(pData)==TRUE){
							SendMsg(MSG_CALL_STATE_LOAD_FIRST_CALL);
						}
					}
				}
			}
			L30CallState = L30_CALL_STATE_FIRST_CALL_INCOMING;	
		}
		else if(index == 1){
			/*Load �ڶ�����*/
			//ChargeDisplayStateChange(false);
			SendMsg(MSG_CALL_STATE_LOAD_SECOND_CALL);
			//if(Syncing_Handler_flag == FALSE)
				LoadPhoneHandler(PHONE_RING,pData,2);	   
		}
		else if(index == 2){
			IncomingCallNoticePhoneHandlerOutHandler(pData);
		}
	}
	return BC5_RSP;
}


/*������:(֪ͨ) Siri״̬���*/
uint8 NotifyBc5SiriInfoCB(uint8 *pData,uint8 len)
{
	uint8 uData=*pData;
	if(uData ==BC5_SIRI_CLOSE)
	{
		SendMsg(MSG_BC5_SIRI_CLOSE);
	}
	else if(uData ==BC5_SIRI_OPEN)
	{
		SendMsg(MSG_BC5_SIRI_OPEN);
	}
	return BC5_RSP;
}


/*SPP͸�����*/
void BC5_SendSppData(uint8 *pData,uint8 DataLen)
{
	uint8 TempSendData[BC5_DATA_MAX_LEN]={0};

	TempSendData[0]=BC5_DATA_START_CMD;
	TempSendData[1]=BC5_SPP_DATA_CMD;
	TempSendData[2]=DataLen;

	if(DataLen>0)
		memcpy(&TempSendData[3],pData,DataLen);

	TempSendData[3+DataLen]=BC5_DATA_STOP0_CMD;
	TempSendData[4+DataLen]=BC5_DATA_STOP1_CMD;

	UART_Send(TempSendData,5+DataLen);
}

uint8 SppTransmissionCB(uint8 *pData,uint8 len)
{
	if(len>0)
	{
		uint8 DataRsp=DataParseHandler(DataRevBuf);  
		if(DataRsp !=E_RSP_NULL)
		SendRspCmd(CurrentDataParseCmd,(RSP_VAULE_E)DataRsp);
	}
	return BC5_RSP;
}



    




