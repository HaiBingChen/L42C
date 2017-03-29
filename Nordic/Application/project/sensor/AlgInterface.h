/***************************************************************************
*软件版本：1.0.9

*算法版本：A1.1.1 (step v1.1.1 && sleep v1.0.1)
*****************************************************************************/
#ifndef _ALG_INTERFACE_H_
#define _ALG_INTERFACE_H_

#include "Typedef.h"
#include "clockdriver.h"
/*****************************************************************************
* 类型定义 
*****************************************************************************/


typedef struct
{
	uint8  SleepQuality;  //睡眠质量,取值范围: 0-100(满分100), 越高越好
	uint16 PreparedCnt;	  //入睡时长,单位: 分

	uint16 AwakeCnt;	  //清醒时间,单位: 分
	uint16 LightCnt;	  //浅睡时间,单位: 分

	uint16 ActualCnt;	  //深睡时间,单位: 分
	uint16 WakeTimesCnt;  //唤醒次数,单位: 次

	uint16 TotalCnt;	  //总长,单位: 分
	//uint16 REV;	//    //对齐

}SLUMBER_DATA_T;


typedef enum
{
	ASLEEP = 0,
	LIGHT = 1,
	AWAKE = 2,
	PREPARED = 3,
	QUIT =4,
	START_SLEEP=0x10,
	STOP_SLEEP=0x11,
	AUTO_SLEEP=0x12,
}SLEEP_STATE;

typedef enum
{
	NORMAL = 0,
	SENSITIVE = 1,
}SENSITIVITY_LEVEL;


/*****************************************************************************
* 对外接口
*****************************************************************************/
//函数版本
void vGetAlogrithmVersion(int8* version, uint8* version_len);

//initial
void vStepCounterInit(void);

//interface 
void vStepCounter(int16 acc[32][3], uint16 acc_num//32
#ifdef _DEBUG_PROJ_QLJ_
	, FILE*	fpout
#endif
	);



//获取当前总步数
uint32 GetSteps(void);

//设置当前总步数::成功返回非0，否则返回0.
uint8 SetSteps(uint32 steps);

// 所有统计量清零
void vClearAllStat(void);

/***********************************************************************************/
/************睡********眠************************************************************/
//ADD-2014-12-5
void vSetAutoSleepTime(uint16 BeginTime, uint16 EndTime);// 高字节小时，低字节分钟，设置自动睡眠模式时间区域(BeginTime，EndTime)

//ADD-2014-8-22
void IsCharging(void);//充电或者同步数据完成后，调用

//ADD-2014-8-8
uint32 GetTimeStamp(void);//当前时间戳

//ADD-2014-8-8
uint8 AutoSleepMode(void);//获取自动入睡标识//非0为开始入睡

//ADD-2014-8-8
uint8 IsSleepValid(void);//返回非0为有效睡眠，保存数据，否则丢弃不保存

void SleepMonitorInit(void);//初始化，开始睡眠模式先调用

void SleepMonitor(int16 *acc, uint32 CurrTime);//睡眠监控函数，1s调用一次，传入时间为睡眠相对时间（单位：s）

uint8 GetSleepState(void);//获取当前睡眠状态

void gOS_GetUTCTime(UTCTimeStruct *p_store);//获取底层时间

void SetSensitivity(uint8 sensitivity);//设置敏感度，0为正常，其他为敏感

void vSleepQuitModeSupport(uint8 flag);//激活自动退出睡眠模式，0为不激活，其他为激活

uint8 GetSensitivity(void);//获取敏感度

void SetButtonPress(uint8 flag);//按钮是否短按

void QuitSleepMode(int16 *acc, uint32 CurrTime);//睡眠结束调用，以完成相关统计量计算

//uint8 GetSleepTimeNode(uint32 *NodeBuff, uint32 StartTime, uint32 QuitTime);//时间节点转换

uint8 GetSleepTimeNodeSingle(uint32 *NodeBuff, uint32 StartTime, uint32 QuitTime);//时间节点转换

void GetSleepStatistic(SLUMBER_DATA_T *SleepStatisticT);//获取睡眠统计量

/************************************************************************************/

#endif


