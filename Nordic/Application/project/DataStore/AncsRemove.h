#include "Typedef.h"

/*��֤�����Ƿ�Ϸ�*/
void InitAncsSmsParameter(void);
/*�洢ANCS ����ϢUID*/
void WriteAncsUid(uint8 dat[4]);
/*��ȡANCS ����ϢUID*/
uint8 ReadAncsUid(uint8 dat1[2],uint8 dat2[2], uint8 value[1]);    
/*�洢ANCS ����Ϣ������*/
void WriteSenderInfo(char *Pdate, uint8 len);
/*��ȡANCS ����Ϣ�����ߵ�˳����*/
bool GetSenderInfo(uint8 position[1], uint8 value);
/*����Ѷ�����Ϣ��UID*/
void ResetSMSuid(uint8 QueryNumber); 
/*����δ�����ŵķ�����˳���*/
void ResetSenderID(uint8 QueryNumber);
/*����Ѷ�����Ϣ�ķ�����*/
void ResetSenderInfo(uint8 QueryNumber);
/*����UID�뷢����λ�ñ���*/
void ResetSMSOffset(void);
/*ȷ�϶����Ƿ��Ѷ���*/
bool QuerySMSOffset(void);



