/***************************************************************************
*����汾��1.0.9

*�㷨�汾��A1.1.1 (step v1.1.1 && sleep v1.0.1)
*****************************************************************************/
#ifndef _ALG_INTERFACE_H_
#define _ALG_INTERFACE_H_

#include "Typedef.h"
#include "clockdriver.h"
/*****************************************************************************
* ���Ͷ��� 
*****************************************************************************/


typedef struct
{
	uint8  SleepQuality;  //˯������,ȡֵ��Χ: 0-100(����100), Խ��Խ��
	uint16 PreparedCnt;	  //��˯ʱ��,��λ: ��

	uint16 AwakeCnt;	  //����ʱ��,��λ: ��
	uint16 LightCnt;	  //ǳ˯ʱ��,��λ: ��

	uint16 ActualCnt;	  //��˯ʱ��,��λ: ��
	uint16 WakeTimesCnt;  //���Ѵ���,��λ: ��

	uint16 TotalCnt;	  //�ܳ�,��λ: ��
	//uint16 REV;	//    //����

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
* ����ӿ�
*****************************************************************************/
//�����汾
void vGetAlogrithmVersion(int8* version, uint8* version_len);

//initial
void vStepCounterInit(void);

//interface 
void vStepCounter(int16 acc[32][3], uint16 acc_num//32
#ifdef _DEBUG_PROJ_QLJ_
	, FILE*	fpout
#endif
	);



//��ȡ��ǰ�ܲ���
uint32 GetSteps(void);

//���õ�ǰ�ܲ���::�ɹ����ط�0�����򷵻�0.
uint8 SetSteps(uint32 steps);

// ����ͳ��������
void vClearAllStat(void);

/***********************************************************************************/
/************˯********��************************************************************/
//ADD-2014-12-5
void vSetAutoSleepTime(uint16 BeginTime, uint16 EndTime);// ���ֽ�Сʱ�����ֽڷ��ӣ������Զ�˯��ģʽʱ������(BeginTime��EndTime)

//ADD-2014-8-22
void IsCharging(void);//������ͬ��������ɺ󣬵���

//ADD-2014-8-8
uint32 GetTimeStamp(void);//��ǰʱ���

//ADD-2014-8-8
uint8 AutoSleepMode(void);//��ȡ�Զ���˯��ʶ//��0Ϊ��ʼ��˯

//ADD-2014-8-8
uint8 IsSleepValid(void);//���ط�0Ϊ��Ч˯�ߣ��������ݣ�������������

void SleepMonitorInit(void);//��ʼ������ʼ˯��ģʽ�ȵ���

void SleepMonitor(int16 *acc, uint32 CurrTime);//˯�߼�غ�����1s����һ�Σ�����ʱ��Ϊ˯�����ʱ�䣨��λ��s��

uint8 GetSleepState(void);//��ȡ��ǰ˯��״̬

void gOS_GetUTCTime(UTCTimeStruct *p_store);//��ȡ�ײ�ʱ��

void SetSensitivity(uint8 sensitivity);//�������жȣ�0Ϊ����������Ϊ����

void vSleepQuitModeSupport(uint8 flag);//�����Զ��˳�˯��ģʽ��0Ϊ���������Ϊ����

uint8 GetSensitivity(void);//��ȡ���ж�

void SetButtonPress(uint8 flag);//��ť�Ƿ�̰�

void QuitSleepMode(int16 *acc, uint32 CurrTime);//˯�߽������ã���������ͳ��������

//uint8 GetSleepTimeNode(uint32 *NodeBuff, uint32 StartTime, uint32 QuitTime);//ʱ��ڵ�ת��

uint8 GetSleepTimeNodeSingle(uint32 *NodeBuff, uint32 StartTime, uint32 QuitTime);//ʱ��ڵ�ת��

void GetSleepStatistic(SLUMBER_DATA_T *SleepStatisticT);//��ȡ˯��ͳ����

/************************************************************************************/

#endif


