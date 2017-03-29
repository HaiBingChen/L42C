#include "Typedef.h"
#include "AppHandler.h"

typedef struct 
{
	int		Id;  
	int          PaySyncingId;
	int 	       NfcHadlerId;
} SmardCardHadlerTAG;

extern uint32 g_balances;
extern uint8_t SmardCard_Index;
extern uint8_t CardID[2];
extern bool PaySyncing_Handler_flag;
extern uint8 NFC_STATUS;
extern SmardCardHadlerTAG  SmardCardHadlerId;


uint16 SmardCard_Handler(MsgType msg, int iParam, void *pContext);
uint16 PaySyncing_Handler(MsgType msg, int iParam, void *pContext);
uint16 NFC_Handler(MsgType msg, int iParam, void *pContext);
int Update_CardID(uint8_t AID, uint8_t *pData, uint16 * len);
int Update_Balances(uint8_t AID,  uint8_t *pData,  uint16 * len);
int Update_Record(uint8_t AID,  uint8_t *pTxData, uint8_t *pRxData, uint16 * len);
void Init_Balances(void);
void Init_CardID(void);
void Init_Balances_PAYID(void);
void Init_CardID_PAYID(void);
void init_nfc(void);
bool nfc_contact_test(void);
void nfc_test(void);
bool get_booster_parameters(void);
void nfc_test_csn(void);
bool set_booster_parameters(void);



