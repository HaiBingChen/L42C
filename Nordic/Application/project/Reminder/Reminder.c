
#include "headfile.h"


const uint8 WeekMaskTable[7]={0x40,0x01,0x02,0x04,0x08,0x10,0x20};  
static REMIND_T RDTable[REMINDER_MAX_CNT];
uint8 gRemindsCnt=0;			//提醒总数 
UploadRemindsStateT gUploadRemindsDataState = UPLOAD_REMIND_NO;
uint8 gUploadRemindsCnt=0;
static uint8 NowRemindsID=0;			//当前响应的事件


void RemindsInit(void)
{
	gRemindsCnt=0;
	NowRemindsID =0;
	uint8 RemindsTemp[PRE_REMINDER_BYTES]={0x00};
	Force_NordicFlash();
	for(uint8 i=0;i<REMINDER_MAX_CNT;i++) 
	{
		GetRemindData(RemindsTemp,i);
		if(RemindsTemp[0]!=0xFF)
		{
			memcpy(&RDTable[gRemindsCnt],&RemindsTemp,sizeof(REMIND_T));
			gRemindsCnt++;
		}
	}
	Free_NordicFlash();
}


bool IsRemindsCanBeAdd(REMIND_T Rdadd ,REMIND_T RdExcept, bool ExcepFlag)
{
	if((Rdadd.RD.Hour<24)&&(Rdadd.RD.Minute<60))  
	{
		if(gRemindsCnt>0)
		{
			for(uint8 i=0;i<gRemindsCnt;i++)
			{
				if(ExcepFlag)
				{
					if((RdExcept.RD.Hour==RDTable[i].RD.Hour)&&(RdExcept.RD.Minute==RDTable[i].RD.Minute)&&(RdExcept.RD.RemindTimes==RDTable[i].RD.RemindTimes))
						continue;	
				}
				if((Rdadd.RD.Hour==RDTable[i].RD.Hour)
				  &&(Rdadd.RD.Minute==RDTable[i].RD.Minute))
				{
					uint8 add=0,Compare=0;
					for(uint8 j=0; j<7;j++)
					{
						add =((Rdadd.RD.RemindTimes)>>j)&0x01;
						Compare = ((RDTable[i].RD.RemindTimes)>>j)&0x01;
						if(add >0)
						{
							if((add^Compare)==0) /*该位同为1*/
								return false;
						}
					}					
				}
			}	
		}
		return true;
	}
	return false;
}


bool RemindsAdd(uint8 *pData, uint16 len)
{ 
	if(gRemindsCnt>=REMINDER_MAX_CNT)
		return false;
	memcpy(&RDTable[gRemindsCnt],pData,sizeof(REMIND_DATA_T));	
	if(IsRemindsCanBeAdd(RDTable[gRemindsCnt],RDTable[gRemindsCnt],false))
	{	
		uint8 page=0;
		if(GetRemindFreePage(&page))
		{
			uint8 RemindsTemp[PRE_REMINDER_BYTES]={0x00};
			RDTable[gRemindsCnt].page = page;
			memcpy(&RemindsTemp[0],&RDTable[gRemindsCnt],sizeof(REMIND_T));	
			if(*pData == 0x07 && len!=0x00)
			{
				if(len>24)
					len =24;
				memcpy(&RemindsTemp[sizeof(REMIND_T)],pData+sizeof(REMIND_DATA_T),len);		
			}
			SetRemindData(RemindsTemp,page);
			gRemindsCnt++;
			return true;
		}
	}
	else
		memset(&RDTable[gRemindsCnt],0,sizeof(REMIND_T));
	return false;
}

bool RemindsEdit(uint8*pData,uint16 len)
{
	REMIND_T RdOld,RdNew;
	bool found =false;
	uint8 offse=0;
	memcpy(&RdOld, pData,sizeof(REMIND_DATA_T));
	memcpy(&RdNew, pData+5,sizeof(REMIND_DATA_T));
/*检测是否能修改成新的提醒*/	
	for(uint8 i=0;i<gRemindsCnt;i++)
	{
		if((RdOld.RD.Hour==RDTable[i].RD.Hour)&&(RdOld.RD.Minute==RDTable[i].RD.Minute)&&(RdOld.RD.RemindTimes==RDTable[i].RD.RemindTimes))
		{
			found =true;
			offse =i;
			break;
		}
	}
	if((!found) ||(!IsRemindsCanBeAdd(RdNew,RdOld,true)))
		return false;
/*修改旧提醒*/	
	uint8 RemindsTemp[PRE_REMINDER_BYTES]={0x00};
	memcpy(&RDTable[offse],&RdNew,sizeof(REMIND_DATA_T));
	memcpy(&RemindsTemp[0],&RDTable[offse],sizeof(REMIND_T));	
	if(RdNew.RD.Id==0x07&& len!=0x00)
	{
		if(len>24)
			len =24;
		memcpy(&RemindsTemp[sizeof(REMIND_T)],pData+5+sizeof(REMIND_DATA_T),len);
	}
	SetRemindData(RemindsTemp,RDTable[offse].page);
	return true;
}


bool RemindsDel(uint8 *pData)
{
	if(gRemindsCnt>0)
	{
		uint8 ClearRemindsCnt=0;
		uint8 hour = *(pData+1);
		uint8 minute = *(pData+2);
		uint8 remindTimes = *(pData+3); 
		for(uint8 i=0;i<gRemindsCnt;i++)
		{
			if((hour==RDTable[i].RD.Hour)&&(minute==RDTable[i].RD.Minute)&&(remindTimes==RDTable[i].RD.RemindTimes))
			{
				uint8 RemindsTemp[PRE_REMINDER_BYTES]={0xFF};	
				SetRemindData(RemindsTemp,RDTable[i].page);
				memset(&RDTable[i],0,sizeof(REMIND_T));		  		               
				memcpy(&RDTable[i],&RDTable[i+1],(gRemindsCnt-i-1)*sizeof(REMIND_T));
				ClearRemindsCnt++;
				pS_OsWatchDogFeed(); /*喂狗*/
			}
		}
		if(ClearRemindsCnt>0)
		{
			gRemindsCnt-=ClearRemindsCnt;
			return true;
		}	
	}
	return false;
}


bool RemindsUploadAll(uint8 idx)
{
	if(idx>0){
		gUploadRemindsCnt = idx-1;
		gUploadRemindsDataState = UPLOAD_REMIND_OK;
	}
	if(gUploadRemindsCnt<gRemindsCnt)
	{
		uint8 RemindsTemp[PRE_REMINDER_BYTES]={0x00};
		uint8 SendTemp[PRE_REMINDER_BYTES+1]={0x00};
		GetRemindData(RemindsTemp,RDTable[gUploadRemindsCnt].page);
		
		memcpy(&SendTemp[0],RemindsTemp,sizeof(REMIND_DATA_T));
		memcpy(&SendTemp[sizeof(REMIND_DATA_T)],RemindsTemp+sizeof(REMIND_T),PRE_REMINDER_BYTES-sizeof(REMIND_T));
		
	       ReminderSendData((gUploadRemindsCnt+1),SendTemp);	
		gUploadRemindsCnt++;
	}
	else{ 
		gUploadRemindsDataState = UPLOAD_REMIND_OK;
		return false;
	}
	return true;
}


void StopSecondRemind(void)
{
        RDTable[NowRemindsID].SecondRemind=0x00;
}

bool RemindsCompCheck(UTCTimeStruct uTime,uint8 *pNowCntPos)
{
	uint8 lWeek=0;
	uint8 RemindsCntTemp=0;
	bool Remids_Found =false,SecRemids_Found= false;
	if(gRemindsCnt>0)
	{
		lWeek=GetMonthWeekDays(uTime.year,uTime.month,uTime.day);//0~6(日~六)//
		for(RemindsCntTemp=0;RemindsCntTemp<gRemindsCnt;RemindsCntTemp++)
		{
			if(RDTable[RemindsCntTemp].SecondRemind>0)
				RDTable[RemindsCntTemp].SecondRemind--;		
			
			if((!Remids_Found)&&(uTime.minutes==RDTable[RemindsCntTemp].RD.Minute)&&(uTime.hour==RDTable[RemindsCntTemp].RD.Hour))
			{
				if(RDTable[RemindsCntTemp].RD.RdSwitch==0x01)
				{
					if((RDTable[RemindsCntTemp].RD.RemindTimes==0x80)
					||(RDTable[RemindsCntTemp].RD.RemindTimes&WeekMaskTable[lWeek])
					)
					{
						RDTable[RemindsCntTemp].SecondRemind=0x03;
						*pNowCntPos=RemindsCntTemp;                 
						Remids_Found =true;
					}       
				}
			}
			if((!Remids_Found)&&(RDTable[RemindsCntTemp].SecondRemind==0x01))		/*二次提醒:0:不提醒 1:二次提醒 2:中间过度分钟间隔 3:一次提醒*/
			{
				if(RDTable[RemindsCntTemp].RD.RdSwitch==0x01)
				{			
					RDTable[RemindsCntTemp].SecondRemind=0x00;
					*pNowCntPos=RemindsCntTemp;   
					SecRemids_Found=true;
				}
			}
		}
	}
	if(Remids_Found||SecRemids_Found)
		return true;
	else
		return false;	
}

void SendFixedRemindMsg(uint8 type)
{
	ChargeDisplayStateChange(false);
	switch(type)
	{
		case REMIND_EAT:
		case REMIND_MEDICINE:
		case REMIND_MOVING:
		case REMIND_SLEEP:
		case REMIND_WAKEUP:
		case REMIND_MEETING:
		case REMIND_USER:
			SendMsg(MSG_USER_REMIND);
			break;
		default:
			break;
	}
}

bool CheckMinuteRemindEvent(void)
{
	UTCTimeStruct uTime;	  				//当前时间
	gOS_GetUTCTime(&uTime);			//检测Reminds事件
	if(GetNoSameTime(uTime))
	{
		if(RemindsCompCheck(uTime,&NowRemindsID))
		{
			SendFixedRemindMsg(RDTable[NowRemindsID].RD.Id);
			return true;
		}
	} 
	return false;
}


void ClearAllReminders(void)
{	
	ClearRemindData();
	pS_OsWatchDogFeed(); /*喂狗*/ 
	memset(RDTable,0,sizeof(RDTable));
	gRemindsCnt=0;
}




void GetRemindsNote(uint8* pNote)
{
       uint8 RemindsTemp[PRE_REMINDER_BYTES]={0x00};
	GetRemindData(RemindsTemp,RDTable[NowRemindsID].page);
	memcpy(pNote,RemindsTemp+sizeof(REMIND_T),PRE_REMINDER_BYTES-sizeof(REMIND_T));
	pS_OsWatchDogFeed(); /*喂狗*/
}

void GetRemindsEvent(REMIND_DATA_T *pNowRemindsEvent)
{
	memcpy(pNowRemindsEvent,&(RDTable[NowRemindsID]),sizeof(REMIND_DATA_T));
}




/******************************************************************************************
静坐提醒
******************************************************************************************/
static uint16 CheckSitCount =0;
static uint32 CheckSitSteps =0;
void ClearSitRemindEvent(void)
{
	CheckSitSteps = 0;
	CheckSitCount = 0;
}

void CheckSitRemindEvent(void)
{
	if(gSitRemind.SrSwitch==0x01)
	{
		UTCTimeStruct uTime;	  				//当前时间
		gOS_GetUTCTime(&uTime);			//检测Reminds事件
		uint8 lWeek=GetMonthWeekDays(uTime.year,uTime.month,uTime.day);
		if((gSitRemind.SrDayFrequency&WeekMaskTable[lWeek])!=0)
		{
			uint16 now = uTime.hour*60+uTime.minutes;
			uint16 begin = gSitRemind.SrBeginHour*60 +gSitRemind.SrBeginMinute;
			uint16 end = gSitRemind.SrEndHour*60 +gSitRemind.SrEndMinute;
			if(begin!=end) /*启动结束时间不能相等*/
			{
				bool CheckSit =false;
				if(begin>end)
					CheckSit=(now>=begin && now<=1439) ||(now<=end);	
				else
					CheckSit=(now>=begin && now<=end);
				if(CheckSit)
				{
					if(CheckSitCount ==0) /*第一次进入*/
					{
						CheckSitSteps= gSportDisplay.SportSteps;
					}
					if((CheckSitCount!=0)&&(gSitRemind.SrFrequency!=0)&&((CheckSitCount%gSitRemind.SrFrequency) == 0))
					{
						uint32 StepTmp = gSportDisplay.SportSteps-CheckSitSteps;
						if(StepTmp<=(gSitRemind.SrStepThresholdH*256+gSitRemind.SrStepThresholdL))
						{
							osal_start_timerEx(GetAppTaskId(), MSG_SIT_REMIND, 1000);
						}
						CheckSitSteps= gSportDisplay.SportSteps;
					}
					CheckSitCount++;					
				}
				else
					ClearSitRemindEvent();
			}
		}
		else
			ClearSitRemindEvent();
	}
}


uint8 GetRemindDisplay(REMIND_DATA_T* RDTableTemp)
{
	uint8 Pages = 0;
	if(gRemindsCnt==0)
	{
		Pages = 0;
	}
	else
	{
		for(uint8 i=0,j=0;i<gRemindsCnt;i++)
		{		
			if(RDTable[i].RD.RdSwitch==0x01)
			{
				memcpy(&RDTableTemp[j],&(RDTable[i].RD),sizeof(REMIND_DATA_T));
				if(j++%3==0)
				{
					Pages++;
				}
			}
		}
	}
	return Pages;
}

uint8 CheckRemindNum(void)
{
	uint8 num = 0;
	if(gRemindsCnt==0)
	{
		num = 0;
	}
	else
	{
		for(uint8 i=0;i<gRemindsCnt;i++)
		{		
			if(RDTable[i].RD.RdSwitch==0x01)
			{
				num++;
			}
		}
	}
	return num;
}

	
