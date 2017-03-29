#include "Typedef.h"

/*验证程序是否合法*/
void InitAncsSmsParameter(void);
/*存储ANCS 短信息UID*/
void WriteAncsUid(uint8 dat[4]);
/*读取ANCS 短信息UID*/
uint8 ReadAncsUid(uint8 dat1[2],uint8 dat2[2], uint8 value[1]);    
/*存储ANCS 短信息发送者*/
void WriteSenderInfo(char *Pdate, uint8 len);
/*读取ANCS 短信息发送者的顺序编号*/
bool GetSenderInfo(uint8 position[1], uint8 value);
/*清除已读短信息的UID*/
void ResetSMSuid(uint8 QueryNumber); 
/*重置未读短信的发送者顺序号*/
void ResetSenderID(uint8 QueryNumber);
/*清除已读短信息的发送者*/
void ResetSenderInfo(uint8 QueryNumber);
/*清零UID与发送者位置变量*/
void ResetSMSOffset(void);
/*确认短信是否已读完*/
bool QuerySMSOffset(void);



