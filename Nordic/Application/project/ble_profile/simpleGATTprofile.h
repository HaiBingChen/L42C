#include "stdint.h"
#include "Ble_nus.h"







#define DATA_REV_MAX  280





extern uint8 BtState;
extern uint16 RevValueOffset;
extern uint8 DataRevBuf[DATA_REV_MAX];
extern ble_nus_t m_nus;











void SendSettingOkRspCmd(void);
void SendSettingFailRspCmd(void);
void nus_data_handler(ble_nus_t *p_nus, uint8 *p_data, uint16 length, uint16 handle);
uint8 HwSendDataToBt(uint8 *pData, uint16 DataLen);
void nus_data_Parse(void);
void nus_data_init(void);















