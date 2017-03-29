
#include "Typedef.h"

  
#define REMIND_TYPE_MAX 6  
#define REMIND_MAX  REMINDER_MAX_CNT  

typedef enum{
  UPLOAD_REMIND_NO=0,
  UPLOAD_REMIND_PREPARE,
  UPLOAD_REMIND_ING,
  UPLOAD_REMIND_OK,
  UPLOAD_REMIND_FAIL,
}UploadRemindsStateT; 

extern UploadRemindsStateT gUploadRemindsDataState;
extern uint8 gUploadRemindsCnt;
extern uint8 gRemindsCnt;
void RemindsInit(void);  
bool RemindsAdd(uint8 * uData, uint16 len);
bool RemindsEdit(uint8*pData,uint16 len);
bool RemindsDel(uint8 *pData);
bool RemindsCompCheck(UTCTimeStruct uTime,uint8 *pNowCntPos);
bool RemindsUploadAll(uint8 idx);

void GetRemindsNote(uint8* pNote);
void GetRemindsEvent(REMIND_DATA_T *pNowRemindsEvent);

void SendFixedRemindMsg(uint8 type);
bool CheckMinuteRemindEvent(void);
void ClearAllReminders(void);
void ClearSitRemindEvent(void);
void CheckSitRemindEvent(void);
void StopSecondRemind(void);
uint8 GetRemindDisplay(REMIND_DATA_T* RDTableTemp);
uint8 CheckRemindNum(void);

