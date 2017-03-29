
#include "headfile.h"


ble_scanner_parse Bsp;
uint8 mWatchid[5]={0x00};
uint8 mName[13]={0x00};
uint8 rWatchid[5]={0x00};
uint8 sWatchid[5]={0x00};
uint8 iWatchid[5]={0x00};

uint8 mFriends[FRIENDS_MAX][6]={0x00};
uint8 mScan[SCAN_MAX+1][20]={0x00};
MsgType mScannerMsg =MSG_HANDLER_BEGIN; 

uint8 device_exp = 0;
uint8 device_exp_count = 0;
uint8 exp_count = 0x00;
uint8 device_moto_count = 0;
uint8 moto_count = 0x00;


uint8 device_exp_rsp = 0;


bool IsWatchIdEqual(uint8* wid0,uint8* wid1)
{
	uint8 same =0;
	for(uint8 i =0; i<5;i++)
	{
		if(wid0[i] != wid1[i])
			break;
		else
			same++;
	}
	if(same == 5)
		return true;	
	else
		return false;
}

void StopDisplay(void)
{
	DisplayUnicode_Stop();
	DevComClearFontBuf();
}

void CleanScanerBuf(void)
{
	StopDisplay();
	memset(rWatchid, 0, 5);
	memset(sWatchid, 0, 5);
	memset(iWatchid, 0, 5);
	for(uint8 i =0; i<SCAN_MAX; i++)
		memset(mScan[i], 0, 20);
}

SP_FRIENDS DisplayRname(void)
{
	StopDisplay();
	for(uint8 i =0; i<SCAN_MAX;i++)
	{
		if(IsWatchIdEqual(rWatchid,mScan[i]))
		{
			DevComWriteFontBuf((mScan[i]+5),mScan[i][18]);
			DevComDisplayFontBuf();
			return (SP_FRIENDS)mScan[i][17];
		}
	}
	return SP_NULL;
}

SP_FRIENDS DisplayScanName(SP_DIR dir)
{
	StopDisplay();
	if(dir==SP_INIT)	
	{
		if(IsWatchIdEqual(sWatchid,iWatchid))
		{
			for(uint8 i =0; i<SCAN_MAX;i++)
			{
				if(mScan[i][18]>0)
				{
					memcpy(sWatchid,mScan[i],5);	
					DevComWriteFontBuf((mScan[i]+5),mScan[i][18]);
					DevComDisplayFontBuf();
					return (SP_FRIENDS)mScan[i][17];				
				}
			}
		}
		else
		{
			if(sWatchid[0]==0x42)
				return SP_BACK;
			else
			{
				for(uint8 i =0; i<SCAN_MAX;i++)
				{
					if(mScan[i][18]>0)
					{
						if(IsWatchIdEqual(sWatchid,mScan[i]))
						{
							DevComWriteFontBuf((mScan[i]+5),mScan[i][18]);
							DevComDisplayFontBuf();
							return (SP_FRIENDS)mScan[i][17];		
						}
					}
				}			
			}
				
		}
	}
	else
	{
		uint8 back=0xff,forward=0xff;
		bool backallow=true,forwardallow=false;
		uint8 offset= 0xff;
		for(uint8 i =0; i<(SCAN_MAX+1);i++)
		{
			if(mScan[i][18]>0)
			{
				if(IsWatchIdEqual(sWatchid,mScan[i]))
				{
					if(back!=0xff)
						backallow = false;

					forwardallow =true;
				}
				else
				{
					if(backallow)
						back = i;
					if(forwardallow)
					{
						forward=i;
						forwardallow = false;
					}
				}
			}
		}
		if(back==0xff&&forward==0xff)
		{
			return SP_NULL;
		}
		else if(forward==0xff)
			forward =back;
		
		if(dir==SP_FORWARD)	
			offset =forward;
		else if (dir==SP_BACKWARD)	
			offset =back;
		
		memcpy(sWatchid,mScan[offset],5);
		if(sWatchid[0]==0x42)
			return SP_BACK;		
			
		DevComWriteFontBuf((mScan[offset]+5),mScan[offset][18]);
		DevComDisplayFontBuf();
		return (SP_FRIENDS)mScan[offset][17];				
	}			
	return SP_NULL;
}

bool AddToScanBuff(uint8_t* wid, uint8_t*name, uint8_t len)
{
	bool myfriends =false;
	for(uint8 i =0; i<SCAN_MAX;i++)
	{
		if(IsWatchIdEqual(wid,mScan[i]))
			return false;
	}
	for(uint8 i =0; i<FRIENDS_MAX;i++)
	{
		if(IsWatchIdEqual(wid,mFriends[i]))
		{
			myfriends=true;
		}
	}	
	for(uint8 i =0; i<SCAN_MAX;i++)
	{
		if(mScan[i][18]==0)
		{
			mScan[i][18] = len;
			if(myfriends) {
				mScan[i][17] = SP_FRIEND;
			}
			else {
				mScan[i][17] = SP_STRANGE;
			}
			memcpy(mScan[i],wid,5);	
			memcpy((mScan[i]+5),name,len);	
			return true;				
		}		
	}	
	return false;
}

bool DelFromScanBuff(uint8_t* wid)
{
	uint8 Temp[20]={0x00};
	for(uint8 i =0; i<SCAN_MAX;i++)
	{
		if(IsWatchIdEqual(wid,mScan[i]))
		{
			memcpy(mScan[i],Temp,20);	
			return true;
		}
	}	
	return false;
}


bool AddToFriendsBuff(uint8_t* wid)
{
	for(uint8 i =0; i<FRIENDS_MAX;i++)
	{
		if(IsWatchIdEqual(wid,mFriends[i]))
			return false;
	}	
	for(uint8 i=0; i<SCAN_MAX;i++)
	{
		if(IsWatchIdEqual(wid,mScan[i]))
		{
			mScan[i][17] = SP_FRIEND;
			break;
		}
	}
	for(uint8 i =0; i<FRIENDS_MAX;i++)
	{
		if(mFriends[i][5]==0)
		{
			mFriends[i][5]= 0x01;
			memcpy(mFriends[i],wid,5);	
			WriteDeviceComm_Friends();
			return true;				
		}		
	}	
	
	return false;
}


void DelFromFriendsBuff(uint8_t* wid)
{
	uint8 Temp[6]={0x00};
	for(uint8 i=0; i<SCAN_MAX;i++)
	{
		if(IsWatchIdEqual(wid,mScan[i]))
		{
			mScan[i][17] = SP_STRANGE;
			break;
		}
	}	
	for(uint8 i =0; i<FRIENDS_MAX;i++)
	{
		if(IsWatchIdEqual(wid,mFriends[i]))
		{
			memcpy(mFriends[i],Temp,6);	
			WriteDeviceComm_Friends();
			return ;
		}
	}		
}




void SP_Generate(uint8* wid, uint8*name, uint16 cmd, SP_ACTION action, uint8 len)
{
	for(uint8 i =0; i<16;i++)
	{
		if(i<5)
			Bsp.uuid[i]=wid[i];
		else
		{
			if(len>0)
			{
				Bsp.uuid[i] = 	name[i-5];
				len--;
			}
			else
				Bsp.uuid[i] = 0xaa;
		}
	}
	if(len==1)
		Bsp.minor = (name[11])& 0x00ff;
	else
		Bsp.minor = 0x00aa;

	Bsp.minor |= ((uint16)action)<<8;
	Bsp.major =(uint16)cmd;
	Bsp.manuf_id = 0x0059;
}


void SP_Parse(uint8_t* uuid, uint16_t major, uint16_t minor)
{
	uint8 i=0,len=0;
	uint8 sWatchId[5]={0x00};
	uint8 sName[12]={0x00};
	uint16 sCmd =(major&0x00ff);
	uint8 sAction =minor>>8;	
	memcpy(sWatchId,uuid,5);

	for(i=0;i<16;i++)
	{
		if(uuid[i]==0xaa)
			break;
	}
	if(i==16)
	{
		if((minor&0x00ff)==0xaa)
			len =11;
		else	
			len =12;
	}
	else if(i>5)
		len=i-5;
	
	if(len>=12)
	{
		memcpy(sName,uuid+5,11);	
		sName[11] =minor&0x00ff; 
		len =12;
	}
	else
		memcpy(sName,uuid+5,len);	

	if(sAction==SP_STANDBY && sCmd ==SPMD_STANDBY)	/*添加到扫描列表*/
	{
		if(AddToScanBuff(sWatchId,sName,len)) {
			mScannerMsg = MSG_GET_SCAN_NAME;								/*搜到新用户*/
		}
	}
	else if(IsWatchIdEqual(sWatchId,mWatchid))  /*发给我的*/
	{
		memcpy(rWatchid,sName,5);	
		if(sAction==SP_REQ)	/*是一个请求*/
		{
			switch(sCmd)
			{
				case SPMD_ADD_FRIENDS:
					for(uint8 i =0; i<FRIENDS_MAX;i++)
					{
						if(IsWatchIdEqual(rWatchid,mFriends[i]))
							return;
					}	
					mScannerMsg = MSG_GET_SCAN_ADD_FRIENDS;					/*收到好友请求*/
					break;
					
				case SPMD_DEL_FRIENDS:
					#if 0
					for(uint8 i =0; i<FRIENDS_MAX;i++)
					{
						if(IsWatchIdEqual(rWatchid,mFriends[i]))					/*收到删除好友请求*/
							mScannerMsg = MSG_GET_SCAN_DEL_FRIENDS;
					}	
					#else
					mScannerMsg = MSG_GET_SCAN_DEL_FRIENDS;				/*收到删除好友请求*/
					#endif
					break;
					
				case SPMD_MOTO:
					mScannerMsg = MSG_GET_SCAN_MOTO;						/*收到震动*/
					device_moto_count = major>>8;
					break;
					
				case SPMD_EXP1:
					mScannerMsg = MSG_GET_SCAN_EXP;						/*收到表情*/
					device_exp_count = major>>8;
					device_exp = 1;
					break;
					
				case SPMD_EXP2:
					mScannerMsg = MSG_GET_SCAN_EXP;
					device_exp_count = major>>8;
					device_exp = 2;
					break;
					
				case SPMD_EXP3:
					mScannerMsg = MSG_GET_SCAN_EXP;
					device_exp_count = major>>8;
					device_exp = 3;
					break;
					
				case SPMD_EXP4:
					mScannerMsg = MSG_GET_SCAN_EXP;
					device_exp_count = major>>8;
					device_exp = 4;
					break;
					
				case SPMD_EXP5:
					mScannerMsg = MSG_GET_SCAN_EXP;
					device_exp_count = major>>8;
					device_exp = 5;
					break;
					
				default:
					break;
			}
		}
		else if(sAction==SP_RESPOND_OK)		/*回应成功*/
		{
			switch(sCmd)
			{
				case SPMD_ADD_FRIENDS:
					mScannerMsg = MSG_GET_SCAN_ADD_FRIENDS_OK;			/*收到好友请求回应成功*/
					break;
					
				case SPMD_DEL_FRIENDS:
					mScannerMsg = MSG_GET_SCAN_DEL_FRIENDS_OK;		/*收到删除好友请求回应成功*/
					break;
					
				case SPMD_MOTO:
					mScannerMsg = MSG_GET_SCAN_MOTO_OK;				/*收到震动回应成功*/					
					break;
					
				case SPMD_EXP1:
					mScannerMsg = MSG_GET_SCAN_EXP_OK;
					device_exp_rsp =1;
					break;
					
				case SPMD_EXP2:
					mScannerMsg = MSG_GET_SCAN_EXP_OK;
					device_exp_rsp =2;
					break;
					
				case SPMD_EXP3:
					mScannerMsg = MSG_GET_SCAN_EXP_OK;
					device_exp_rsp =3;
					break;
					
				case SPMD_EXP4:
					mScannerMsg = MSG_GET_SCAN_EXP_OK;
					device_exp_rsp =4;
					break;
					
				case SPMD_EXP5:
					mScannerMsg = MSG_GET_SCAN_EXP_OK;
					device_exp_rsp =5;
					break;
					
				default:
					break;
			}			
		}
		else if(sAction==SP_RESPOND_FAIL)		/*回应失败*/
		{
			switch(sCmd)
			{
				case SPMD_ADD_FRIENDS:
					mScannerMsg = MSG_GET_SCAN_ADD_FRIENDS_FAIL;		/*收到好友请求回应失败*/
					break;
					
				case SPMD_DEL_FRIENDS:
					mScannerMsg = MSG_GET_SCAN_DEL_FRIENDS_FAIL;		/*收到删除好友请求回应失败*/
					break;
					
				case SPMD_MOTO:
					mScannerMsg = MSG_GET_SCAN_MOTO_FAIL;				/*收到震动回应失败*/
					break;
					
				case SPMD_EXP1:
					mScannerMsg = MSG_GET_SCAN_EXP_FAIL;
					device_exp_rsp =1;
					break;
					
				case SPMD_EXP2:
					mScannerMsg = MSG_GET_SCAN_EXP_FAIL;
					device_exp_rsp =2;
					break;
					
				case SPMD_EXP3:
					mScannerMsg = MSG_GET_SCAN_EXP_FAIL;
					device_exp_rsp =3;
					break;
					
				case SPMD_EXP4:
					mScannerMsg = MSG_GET_SCAN_EXP_FAIL;
					device_exp_rsp =4;
					break;
					
				case SPMD_EXP5:
					mScannerMsg = MSG_GET_SCAN_EXP_FAIL;
					device_exp_rsp =5;
					break;
					
				default:
					break;
			}		
		}
		else if(sAction==SP_RECEIVE)
		{
			mScannerMsg = MSG_GET_SCAN_ADD_FRIENDS_REC ; 			/*收到请求好友*/
		}
	}

	
	
}


void Scanner_Parse_init(void)
{
	uint8 ReadWatchIdTemp[20]={0x00};
	uint8 i,temp;		        
	bool hasmName = false;
	ReadWatchId(ReadWatchIdTemp);
	if(mName[0]!=0x00)
		hasmName =true;
	temp=0;
	for(i=7;i<12;i++)
	{
		mWatchid[temp]=ReadWatchIdTemp[i];
		if(!hasmName)
			mName[temp]=ReadWatchIdTemp[i];		
		temp++;
	}
	mScan[SCAN_MAX][0]=0x42;
	mScan[SCAN_MAX][18]=0x42;
	SP_Generate(mWatchid,mName,SPMD_STANDBY,SP_STANDBY,strlen((char*)(mName)));
}


