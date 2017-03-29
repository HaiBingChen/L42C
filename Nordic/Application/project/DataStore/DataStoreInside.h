#include "Typedef.h"

/*************************�ֿ���: 952K ************************************/
#define Font_Base 					0x00272000UL
/*************************�û���: 440K ************************************/
/*OTA����-------(����չ): 2��Block, ����HR:9����(36k)��TP:12����(48k)��KL17:29����(116k)����200k*/
extern uint32 Ota_Base; 						//0x392000UL
extern uint32 Ota_Hr_Base;					//0x392000UL;				
extern uint32 Ota_Tp_Base;					//0x39B000UL;				
extern uint32 Ota_Kl17_Base;					//0x3A7000UL;			
extern uint32 Ota_Nor_Base;					//0x3C8000UL;	
	
#define Ota_Hr_SectorNum			9		// 09
#define Ota_Tp_SectorNum			12		// 0C
#define Ota_Kl17_SectorNum			29		// 1D
#define Ota_Nor_SectorNum			36		// 24
/*�˶�\˯�ߵȴ洢����-------(��ѹ��): �˶�2���� , ˯��2���� ����2����=6����:24k*/
#define StorageData_Base			0x3C4000UL
#define StorageData_Base_Sector0	(StorageData_Base+0x0000UL)	
#define StorageData_Base_Sector1	(StorageData_Base+0x1000UL)
#define StorageData_Base_Sector2	(StorageData_Base+0x2000UL)
#define StorageData_Base_Sector3	(StorageData_Base+0x3000UL)

#define StorageData_Base_Sector4	(0x3F4000UL)
#define StorageData_Base_Sector5	(0x3F5000UL)
/*Ancs���ݴ洢����-------(��ѹ��): һ������2��������������:���硢δ�����硢���š��罻+15��Ԥ��(8����+30����)=38����:152k*/
#define Ancs_Base					0x360000UL
/*�������ݴ洢����-------(����ѹ��): 10��������40k */
#define UserData_Base				0x3F6000UL
#define UserData_Base_Sector0		(UserData_Base+0x0000UL)
#define UserData_Base_Sector1		(UserData_Base+0x1000UL)	
#define UserData_Base_Sector2		(UserData_Base+0x2000UL)
#define UserData_Base_Sector3		(UserData_Base+0x3000UL)
#define UserData_Base_Sector4		(UserData_Base+0x4000UL)
#define UserData_Base_Sector5		(UserData_Base+0x5000UL)
#define UserData_Base_Sector6		(UserData_Base+0x6000UL)
#define UserData_Base_Sector7		(UserData_Base+0x7000UL)
#define UserData_Base_Sector8		(UserData_Base+0x8000UL)		/*AncsRemove.libռ��,��ֹʹ��*/
#define UserData_Base_Sector9		(UserData_Base+0x9000UL)

/*ͨѶ¼���ݴ洢������ౣ��800���绰���������*/
#define PhoneBook_Base					0x3EC000UL
#define PhoneBook_End					0x3F3FFFUL
//�豸��ͨ�ź���
#define DEVICES_COMM_BASE_ADDR              	0x3EC000UL

/************************************Sector 0 �洢  (Watch ID)************************************/
//watch id �洢
#define WATCH_ID_BASE_ADDR				UserData_Base_Sector0
#define WATCH_ID_LEN                        			(20)
/************************************Sector 1 �洢 (��Ҫ��־) ************************************/
#define USER_FLAG_BASE_ADDR				UserData_Base_Sector1

//�û���ʼ����־
#define INIT_MARK_LOC						(0)
#define INIT_MARK_LEN						(1)
//�û�BC5������־
#define BC5_RECONNECTED_LOC				(INIT_MARK_LOC+INIT_MARK_LEN)
#define BC5_RECONNECTED_LEN				(1)
#define USER_FLAG_MAX_LEN					BC5_RECONNECTED_LOC+BC5_RECONNECTED_LEN

/************************************Sector 2 �洢 (�û�����)************************************/
#define USER_PARAMETER_BASE_ADDR              	UserData_Base_Sector2

//������Ϣ����
#define USER_INFO_LOC             				(0)
#define USER_INFO_LEN                   			(5)
//һ����������
#define MENU_ORDER_LOC						(USER_INFO_LOC+USER_INFO_LEN)
#define MENU_ORDER_LEN						(9)
//��������
#define MOTO_ORDER_LOC						(MENU_ORDER_LOC+MENU_ORDER_LEN)
#define MOTO_ORDER_LEN						(10)
//ʱ������
#define TIME_ORDER_LOC						(MOTO_ORDER_LOC+MOTO_ORDER_LEN)
#define TIME_ORDER_LEN						(5)
//ʱ��������
#define TIMER_MENU_LOC						(TIME_ORDER_LOC+TIME_ORDER_LEN)
#define TIMER_MENU_LEN						(1)
//����ֵ
#define SET_VALUE_LOC						(TIMER_MENU_LOC+TIMER_MENU_LEN)
#define SET_VALUE_LEN						(5)
//Ŀ��ֵ
#define DAILY_GOAL_LOC						(SET_VALUE_LOC+SET_VALUE_LEN)
#define DAILY_GOAL_LEN						(15)
//�Զ�˯��
#define AUTO_SLEEP_LOC						(DAILY_GOAL_LOC+DAILY_GOAL_LEN)
#define AUTO_SLEEP_LEN						(5)
//����
#define SWITCH_LOC							(AUTO_SLEEP_LOC+AUTO_SLEEP_LEN)
#define SWITCH_LEN							(4)
//��ݲ˵�����
#define SHORTCUT_LOC						(SWITCH_LOC+SWITCH_LEN)
#define SHORTCUT_LEN						(6)
//˯�߲���
#define SLEEP_LOC							(SHORTCUT_LOC+SHORTCUT_LEN)
#define SLEEP_LEN							(12)
//���ò���
#define SETTING_LOC							(SLEEP_LOC+SLEEP_LEN)
#define SETTING_LEN							(2)
//�û���ͨ��ID
#define DEVICE_COMMID_LOC       				(SETTING_LOC+SETTING_LEN)
#define DEVICE_COMMID_LEN               	 		(12) 
//����ǰ�˶����ݱ���
#define UPDATE_STORE_LOC	 				(DEVICE_COMMID_LOC+DEVICE_COMMID_LEN)
#define UPDATE_STORE_LEN					(17)
//����
#define AUTO_HEARTRATE_TIME_LOC 			(UPDATE_STORE_LOC+UPDATE_STORE_LEN)
#define AUTO_HEARTRATE_TIME_LEN 			(1)
//������������
#define SIT_REMIAND_LOC					(AUTO_HEARTRATE_TIME_LOC+AUTO_HEARTRATE_TIME_LEN)
#define SIT_REMIAND_LEN					(9) 
//������·��
#define BASE_KCAL_TYPE_LOC					(SIT_REMIAND_LOC+SIT_REMIAND_LEN)
#define BASE_KCAL_TYPE_LEN					(1)

//�������LastLanguage
#define LAST_LANGUAGE_LOC					(BASE_KCAL_TYPE_LOC+BASE_KCAL_TYPE_LEN)
#define LAST_LANGUAGE_LEN					(1)
//ͨ��¼���뿪��
#define PHONE_BOOK_LOC						(LAST_LANGUAGE_LOC+LAST_LANGUAGE_LEN)
#define PHONE_BOOK_LEN						(1)

//ͨ����¼��־λ
#define PHONE_RECORD_LOC					(PHONE_BOOK_LOC+PHONE_BOOK_LEN)
#define PHONE_RECORD_LEN					(9)

#define USER_UID_LOC						(PHONE_RECORD_LOC+PHONE_RECORD_LEN)
#define USER_UID_LEN						(4)

#define USER_PARAMETER_MAX_LEN			(USER_UID_LOC+USER_UID_LEN)

/************************************Sector 3 �洢 (�˶�/����������)************************************/
#define STEP_SLEEP_SUM_BASE_ADDR   		UserData_Base_Sector3
//�˶�������
#define SPORT_TOTAL_LOC					(0)
#define SPORT_TOTAL_LEN					(4)
//˯��������
#define SLEEP_TOTAL_LOC					(SPORT_TOTAL_LOC+SPORT_TOTAL_LEN)
#define SLEEP_TOTAL_LEN					(4)
//����������
#define HEARTRATE_RECORDS_TOTAL_LOC		(SLEEP_TOTAL_LOC+SLEEP_TOTAL_LEN)
#define HEARTRATE_RECORDS_TOTAL_LEN		(4)
//���ʱ�������
#define HEARTRATE_ALARM_LOC				(HEARTRATE_RECORDS_TOTAL_LOC+HEARTRATE_RECORDS_TOTAL_LEN)
#define HEARTRATE_ALARM_LEN				(4)

//��ʱ�˶���¼������
#define RECORD_WORK_LOC					(HEARTRATE_ALARM_LOC+HEARTRATE_ALARM_LEN)
#define RECORD_WORK_LEN					(4)

#define STEP_SLEEP_SUM_MAX_LEN			RECORD_WORK_LOC+RECORD_WORK_LEN

/************************************Sector 4�洢 (����) *********************************************/
//����
#define REMINDER_BASE_ADDR              		UserData_Base_Sector4
#define PRE_REMINDER_BYTES        			(30)
#define REMINDER_MAX_CNT       				(10)

/************************************Sector 5�洢 (˯����ϸ����ͼ��)**********************************/
#define SLEEP_DETAIL_BASE_ADDR			UserData_Base_Sector5

/************************************Sector 6�洢 (OTA�洢����)**************************************/
#define OTA_INFO_BASE_ADDR				UserData_Base_Sector6

/************************************Sector 7�洢 (ʱ�䱣��)*****************************************/
#define TIMER_SAVE_BASE_ADDR				UserData_Base_Sector7

/************************************Sector 9�洢 (�˶���ʾ����)*****************************************/
#define SPORT_DIPLAY_SAVE_BASE_ADDR		UserData_Base_Sector9

//KL17��ַ
#define OTA_KL17_ADDR_LOC					(0)
#define OTA_KL17_LEN						(5)
//TP��ַ
#define OTA_TP_ADDR						(OTA_KL17_ADDR_LOC+OTA_KL17_LEN)
#define OTA_TP_LEN							(5)
//HR��ַ
#define OTA_HR_ADDR						(OTA_TP_ADDR+OTA_TP_LEN)
#define OTA_HR_LEN							(5)

//���������ɹ���־
#define OTA_FLAG_LOC						(OTA_HR_ADDR+OTA_HR_LEN)	
#define OTA_FLAG_LEN						(1)
//KL17��Ϣ
#define OTA_KL17_TAG_LOC					(OTA_FLAG_LOC+OTA_FLAG_LEN)	
#define OTA_KL17_TAG_LEN					(4)

#define OTA_KL17_SIZE_LOC					(OTA_KL17_TAG_LOC+OTA_KL17_TAG_LEN)	
#define OTA_KL17_SIZE_LEN					(4)
//TP��Ϣ
#define OTA_TP_TAG_LOC						(OTA_KL17_SIZE_LOC+OTA_KL17_SIZE_LEN)	
#define OTA_TP_TAG_LEN						(4)

#define OTA_TP_SIZE_LOC					(OTA_TP_TAG_LOC+OTA_TP_TAG_LEN)	
#define OTA_TP_SIZE_LEN					(4)

//HR��Ϣ
#define OTA_HR_TAG_LOC						(OTA_TP_SIZE_LOC+OTA_TP_SIZE_LEN)	
#define OTA_HR_TAG_LEN						(4)

#define OTA_HR_SIZE_LOC					(OTA_HR_TAG_LOC+OTA_HR_TAG_LEN)	
#define OTA_HR_SIZE_LEN					(4)

//Nor��ַ
#define OTA_NOR_ADDR						(OTA_HR_SIZE_LOC+OTA_HR_SIZE_LEN)
#define OTA_NOR_LEN						(5)

#define OTA_INFO_MAX_LEN					OTA_NOR_ADDR+OTA_NOR_LEN


/************************************�˶�\˯�ߵȴ洢����********************************************/
//�˶�����
#define SPORT_STORE_ADDR					StorageData_Base_Sector0	
#define PRE_SPORT_SAMPLE_BYTES    			(20)
#define STORE_STEP_DATA_MAX  				((uint32)(StorageData_Base_Sector1 - StorageData_Base_Sector0)/PRE_SPORT_SAMPLE_BYTES)

//˯������
#define SLEEP_STORE_ADDR					StorageData_Base_Sector1	
#define PRE_SLEEP_SAMPLE_BYTES 			(8)
#define STORE_SLEEP_DATA_MAX  				((uint32)(StorageData_Base_Sector2 - StorageData_Base_Sector1)/PRE_SLEEP_SAMPLE_BYTES)

//��������
#define HEARTRATE_STORE_ADDR				StorageData_Base_Sector2
#define PRE_HEARTRATE_SAMPLE_BYTES 		(8)
#define STORE_HEARTRATE_DATA_MAX  		((uint32)(StorageData_Base_Sector3 - StorageData_Base_Sector2)/PRE_HEARTRATE_SAMPLE_BYTES)

//��ʱ�˶���¼����
#define RECORD_WORK_STORE_ADDR			StorageData_Base_Sector4
#define PRE_RECORD_WORK_BYTES 			(24)
#define RECORD_WORK_DATA_MAX 			((uint32)(StorageData_Base_Sector5 - StorageData_Base_Sector4)/PRE_RECORD_WORK_BYTES)

/************************************ANCS��Ϣ����*************************************************/
#define ANCS_BASE_ADDR						Ancs_Base
#define ANCS_SOCIAL_BASE_ADDR				Ancs_Base+0x0000E000UL

/***********ͨѶ¼��Ϣ����*******��ౣ��800������1k����26����ĸ���׵�ַ�Ͱ����ĵ绰��������*************/

#define PHONEBOOK_BASE_ADDR								PhoneBook_Base

#define PH_GROUP_ID_BASE_ADDR								0X3F5C00UL  //��1K = 1024
#define PH_GROUP_ID_LEN										200UL

#define PH_GROUP_CNT_BASE_ADDR							PH_GROUP_ID_BASE_ADDR + PH_GROUP_ID_LEN
#define PH_GROUP_CNT_LEN									820UL

#define PH_INF_MAXCNT_BASE_ADDR							PH_GROUP_CNT_BASE_ADDR + PH_GROUP_CNT_LEN
#define PH_INF_MAXCNT_LEN									2UL

#define PH_GRP_MAXCNT_BASE_ADDR							PH_INF_MAXCNT_BASE_ADDR + PH_INF_MAXCNT_LEN
#define PH_GRP_MAXCNT_LEN									1UL

#define PH_ANDRIOD_IOS_BASE_ADDR							PH_GRP_MAXCNT_BASE_ADDR + PH_GRP_MAXCNT_LEN
#define PH_ANDRIOD_IOS_LEN									1UL

void StoreDataInit(void);
//watch id �洢
void ReadWatchId(uint8 *dat);
void WriteWatchId(uint8 *dat);
void READ_USER_FLAG(uint8 *data,uint8 Cnt,uint8 CLen);
void WRITE_USER_FLAG(uint8 *data,uint8 Cnt, uint8 CLen);
//������Ϣ�洢
uint8 SetUserInfo(void);

//��������
uint8 ReadSitRemiandSetting(SIT_REMIAND_DATA_T *dat);
uint8 WriteSitRemiandSetting(uint8 *dat);
//�û���ͨ��ID:16bytes
uint8 ReadDeviceCommID(uint8 *dat);
uint8 WriteDeviceCommID(uint8 *dat);


void READ_USER_PARAMETER(uint8 *data,uint8 Cnt,uint8 CLen);
void WRITE_USER_PARAMETER(uint8 *data,uint8 Cnt, uint8 CLen);
void READ_STEP_SLEEP_SUM(uint32 *data,uint8 Cnt,uint8 CLen);
void WRITE_STEP_SLEEP_SUM(uint32 *data,uint8 Cnt, uint8 CLen);

//����:
uint8 GetRemindData(uint8 *remin, uint8 items);
uint8 GetRemindHead(uint8 *remin, uint8 items);
bool GetRemindFreePage(uint8* page);
uint8 SetRemindData(uint8 *remin, uint8 items);
void ClearRemindData(void);

//�豸��ͨ��:
uint8 ResetDeviceComm_Friends(void);
uint8  WriteDeviceComm_Friends(void);
uint8  ReadDeviceComm_Friends(void);

void READ_OTA_INFO(uint8 *data,uint8 Cnt,uint8 CLen);
void WRITE_OTA_INFO(uint8 *data,uint8 Cnt, uint8 CLen);
void Get_Ota_Addr(void);
void Ota_Prepare(uint8 * otaD,uint8 len);
uint8  EraseOtaSuccessFlag(void);
uint8  ReadOtaSuccessFlag(void);

//�˶�����
bool GetStepData(uint32 *Logs,SPORT_DATA_T *pData);
bool StoreStepData(uint32 *Logs,SPORT_DATA_T* tData);

//˯������
bool GetSleepData(SLEEP_DATA_T *uData,uint32 *LogCnt);
bool StoreSleepData(SLEEP_DATA_T *uData,uint32 *LogCnt);

//�˶���ʾ����
void SetDisplySportData(void);
void NeedSetDisplySportData(uint8 ui_index);
void GetDisplySportData(void);

bool GetRecordWorkData(uint32 *Logs,REC_WORK_DATA_T *pData);
bool StoreRecordWorkData(uint32 *Logs,REC_WORK_DATA_T* tData);

typedef struct 
{
	uint32 wMap;
	uint32 rMap;
	uint8 wPos;
} AncsFlash;


typedef enum
{	
	De_INIT,
	De_NEXT,
	De_PRE
}DetailPageDir;

typedef struct 
{
	uint16 WPos;
	uint8 G_WPos;
} PHFlash;

typedef struct
{
    uint8	Group_ID;		
    uint16 Group_Cnt;
    uint8 PH_Inf[46];
} phonebook_data_t;

extern bool Android_ios;

#define ANCS_LAYOUT_MAX				5
void SaveAncsMsg(ANCS_SAVE_TYPE ast, uint8 *dat, uint8 len);
bool GetAncsMsgLayOut(ANCS_SAVE_TYPE ast,uint8*Lo);
bool ReadAncsMsg_ToKl17(ANCS_SAVE_TYPE ast,uint8 index, uint8 detail, uint8 line, DetailPageDir dir);
uint8 ForAncsHandlerEnd(ANCS_SAVE_TYPE ast);
uint8 DeleteAncsMsg(ANCS_SAVE_TYPE ast,uint8 index);
void ClearAncsFlash(void);
void ReflashAncsCount(void);
bool GetHeartRateData(HEARTRATE_DATA_T *uData,uint32 *LogCnt);
bool StoreHeartRateData(HEARTRATE_DATA_T *uData,uint32 *LogCnt);
void GetOTALanguageInfo(void);
void SetPhBok_Flag(uint8 IsSave);
uint8 GetPhBok_Flag(void);
void ResetWpos(void);
void ResetGWpos(void);
void Cnt_Wpos(void);
void Cnt_GWpos(void);
void SetMaxCnt(void);
uint16 GetInfMaxCnt(void);
uint8 GetGrpMaxCnt(void);
void ReadPBook_id_toFlash(uint8 *dat, uint8 len);
void ReadPBook_CNT_toFlash(uint8 *dat, uint8 len);
void ReadPBook_INF_toFlash(uint8 *dat, uint8 len);
void GetPhBook_idAddr(uint32 *PIdaddr, uint8 Grp_Index);
void GetPBook_Cnt(uint8 *dat, uint8 len);
void GetPhBook_InfAddr(uint32 *PInfAddr, uint16 Show_Index);
bool GetPhBook_GrpIdAddr(uint32 *PGrp_addr,uint8 *PGrp_Index,uint16 Show_Index);
void SavePBook_ID_toFlash(uint8 *dat, uint8 len);
void SavePBook_Cnt_toFlash(uint8 *dat, uint8 len);
void SavePBook_NumName_toFlash(uint8 *dat, uint8 len);
void SavePBookto_Flash(PHONEBOOK_SAVE_ID PH_Id, uint8 *dat, uint8 len);
bool GetPhoneBookInf(uint32 *PInf_addr,uint8 *PInf_Cnt,uint32 *PGrp_addr,uint8* PGrp_Cnt, uint16 Show_Index);
bool ReadPhoneBook_ToKl17(uint8 Percent_Showed,uint16 Show_Index);
void ClearPhoneBookFlash(void);
uint32 ReadAncsMsg(ANCS_SAVE_TYPE ast,uint8 index);
bool ReadChosePhoneBook_ToKl17(uint8 ChsGrpIndex, uint16 *PtrShowIndex);
bool GetChooseInfNum(uint32 *InfAddr_Chs,uint8 ChooseIndex);
void InitPhoneBookBuf(void);
void CNT_GroupId(void);
void Reset_GrpCnt(void);
void SavePhoneBookToBuf(uint8* Data, bool save);
void ClearPhoneRecordFlash(void);
uint8 ShowedCntPercent(uint16 ShowedCnt);
void ClearAncs_IncomingCall(void);
void ClearAncs_MissedCall(void);
void ClearAncs_Sms(void);
void ClearAncs_Social(void);
void ClearAncs_Email(void);
void GetAndroid_iosFalg(void);
void SetAndroid_iosFalg(bool IsApk_App);
void ClearAncs_PhoneRecord(void);
void BindEndDealWith(void);
uint8 Total_Count_SOCIAL(void);
void Add_Count_SOCIAL(ANCS_SAVE_TYPE ast);
void Del_Count_SOCIAL(ANCS_SAVE_TYPE ast);
void SaveAncs_SOCIAL(ANCS_SAVE_TYPE ast, uint8 *dat, uint8 len, uint8 off);
bool ReadAncsMsg_ToKl17_SOCIAL(ANCS_SAVE_TYPE ast,uint8 index, uint8 detail, uint8 line, DetailPageDir dir);
bool GetAncsMsgLayOut_SOCIAL(ANCS_SAVE_TYPE ast,uint8*Lo);
uint8 ForAncsHandlerEnd_SOCIAL(ANCS_SAVE_TYPE ast);
uint8 DeleteAncsMsg_SOCIAL(ANCS_SAVE_TYPE ast,uint8 index);
