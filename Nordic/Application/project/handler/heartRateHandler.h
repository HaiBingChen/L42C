#ifndef _HEART_RATE_HANDLER_H_
#define _HEART_RATE_HANDLER_H_

#define HR_INVALID (-1)
typedef enum{HR_GOING, HR_WAITING, HR_WAITING_INIT,HR_STOP,}HR_STATE;

extern UiHRStruct DispalyHrData;
extern UiHRStruct DispalyHrAlarmData;

extern uint8 bLowHR;




HR_STATE heartRate_GetState(void);
bool heartRate_open(void);
bool heartRate_close(void);
void heartRate_dat_clr(void);
uint16 heartRate_Handler(MsgType msg, int iParam, void *pContext);
bool HrDisplayIsChg(uint8 op);
extern void hearRate_state_check(void);




#endif //_HEART_RATE_HANDLER_H_


