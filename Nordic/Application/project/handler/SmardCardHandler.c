#include "headfile.h"
#include "math.h"

uint8_t SmardCard_Index = 0;
uint8 NFC_STATUS = 2;	//2=set up status, 1= normal status
uint32 g_balances=0x0;
SmardCardHadlerTAG  SmardCardHadlerId;
bool PaySyncing_Handler_flag =FALSE;

static void UpdateSmardCardUI(uint8 ui_index)
{
	switch(ui_index)
	{
		case 0:
			ShowBalances(g_balances);
		break;

		case 1:
			//ShowCardID(CardID);
		break;

		default:
			break;
	}
}

uint16 SmardCard_Handler(MsgType msg, int iParam, void *pContext)
{

	switch(msg)
	{
		case MSG_HANDLER_BEGIN:
			SmardCardHadlerId.Id = iParam;
			break;

		case MSG_HANDLER_END:
			SmardCardHadlerId.Id = 0;
			break;

		case MSG_TOUCH_DOWN_UP_FLICK:				/*×ó*/ 
			LoadMenuHandler(MENU_FORWARD);
			break;

		case MSG_TOUCH_UP_DOWN_FLICK: 			/*ÓÒ*/
			LoadMenuHandler(MENU_BACKWARD);
			break;

		case MSG_DISPLAY:
			//UpdateSmardCardUI(SmardCard_Index);
			UpdateSmardCardUI(0);
			break;
		case MSG_REPEAT_SCD_DISPLAY:
			//UpdateSmardCardUI(SmardCard_Index);
			if(CheckpCurrentHandler(SmardCardHadlerId.Id)) {
				UpdateSmardCardUI(0);
			}
			break;

		case MSG_RIGHTDOWN_SHORTKEY: 				/*ÓÒ¼ü µ¥»÷*/	
		UnloadHandler(SmardCardHadlerId.Id);
			break;

/*ÓÃ»§²Ù×÷*/
		case MSG_LEFTDOWN_SHORTKEY:				/*×ó¼ü µ¥»÷*/	
		case MSG_LEFTDOWN_DOUBLE_SHORTKEY:		/*×ó¼ü Ë«»÷*/
		case MSG_LEFTDOWN_LONGKEY:           			/*×ó¼ü ³¤°´*/
		case MSG_RIGHTDOWN_DOUBLE_SHORTKEY:   	/*ÓÒ¼ü Ë«»÷*/	
		case MSG_RIGHTDOWN_LONGKEY:				/*ÓÒ¼ü ³¤°´*/	
		case MSG_RIGHTDOWN_LONGKEY_UP:			/*ÓÒ¼ü ³¤°´ËÉ¿ª*/		
		case MSG_TOUCH_UP_FLICK:					/*ÉÏ*/
		case MSG_TOUCH_DOWN_FLICK:				/*ÏÂ*/	
		case MSG_TOUCH_UP_SHORTKEY:				/*´¥Ãþ µ¥»÷*/  
		case MSG_TOUCH_UP_DOUBLE_SHORTKEY:		/*´¥Ãþ Ë«»÷*/	
		case MSG_TOUCH_LONGKEY:					/*´¥Ãþ ³¤°´*/
			break;


		default:
			return RET_MSG_UNHANDLED;
	}
	return RET_MSG_HANDLED;
}

uint16 PaySyncing_Handler(MsgType msg, int iParam, void *pContext)
{
	switch(msg)
	{
		case MSG_HANDLER_BEGIN:
			SmardCardHadlerId.PaySyncingId= iParam;
			PaySyncing_Handler_flag = TRUE;
			osal_start_timerEx(GetAppTaskId(), MSG_PAY_SYNCING_HANDLER_TIMEOUT, 5000);	
			break;

		case MSG_HANDLER_END:
			PaySyncing_Handler_flag =FALSE;
			break;

		case MSG_PAY_SYNCING_START:
			break;

		case MSG_PAY_SYNCING_HANDLER_TIMEOUT:
		case MSG_PAY_SYNCING_STOP:
			UnloadHandler(SmardCardHadlerId.PaySyncingId);
			break;

		case MSG_DISPLAY:
			{
				ShowSyncingIcon();
			}
			break;

		case MSG_SYSTEM_SUSPEND:

		/*ÓÃ»§²Ù×÷*/
		case MSG_LEFTDOWN_SHORTKEY:				/*×ó¼ü µ¥»÷*/	
		case MSG_LEFTDOWN_DOUBLE_SHORTKEY:		/*×ó¼ü Ë«»÷*/
		case MSG_LEFTDOWN_LONGKEY:           			/*×ó¼ü ³¤°´*/
		case MSG_RIGHTDOWN_SHORTKEY: 				/*ÓÒ¼ü µ¥»÷*/	
		case MSG_RIGHTDOWN_DOUBLE_SHORTKEY:   	/*ÓÒ¼ü Ë«»÷*/	
		case MSG_RIGHTDOWN_LONGKEY:				/*ÓÒ¼ü ³¤°´*/	
		case MSG_RIGHTDOWN_LONGKEY_UP:			/*ÓÒ¼ü ³¤°´ËÉ¿ª*/		
		case MSG_TOUCH_UP_FLICK:					/*ÉÏ*/
		case MSG_TOUCH_DOWN_FLICK:				/*ÏÂ*/
		case MSG_TOUCH_DOWN_UP_FLICK:				/*×ó*/ 
		case MSG_TOUCH_UP_DOWN_FLICK: 			/*ÓÒ*/
		case MSG_TOUCH_UP_SHORTKEY:				/*´¥Ãþ µ¥»÷*/  
		case MSG_TOUCH_UP_DOUBLE_SHORTKEY:		/*´¥Ãþ Ë«»÷*/	
		case MSG_TOUCH_LONGKEY:					/*´¥Ãþ ³¤°´*/
			break;

		default:
			return RET_MSG_UNHANDLED;
	}
	return RET_MSG_HANDLED;
}

/*Ö§¸¶½çÃæHandler*/
uint16 NFC_Handler(MsgType msg, int iParam, void *pContext)
{
	bool status = false;
	static uint8 nfc_adv = 0x00;
	
	switch(msg)
	{
		case MSG_HANDLER_BEGIN:
			SmardCardHadlerId.NfcHadlerId= iParam;
			if (NFC_STATUS == 1)		//NFC_STATUS=2, SET UP STATUS; NFC_STATUS=1, NORMAL STATUS
				SmardCard_Index = 0;
			else if (NFC_STATUS == 2)
				SmardCard_Index = 4;	//SmardCard_Index=4;
			SetSystemSuspendTime(10);
			break;

		case MSG_HANDLER_END:
			SmardCardHadlerId.NfcHadlerId =0;		
			break;
/*ÓÃ»§²Ù×÷*/
		case MSG_LEFTDOWN_SHORTKEY:				/*×ó¼ü µ¥»÷*/	
		case MSG_LEFTDOWN_DOUBLE_SHORTKEY:		/*×ó¼ü Ë«»÷*/
		case MSG_LEFTDOWN_LONGKEY:           			/*×ó¼ü ³¤°´*/		
		case MSG_RIGHTDOWN_DOUBLE_SHORTKEY:   	/*ÓÒ¼ü Ë«»÷*/	
		case MSG_RIGHTDOWN_LONGKEY:				/*ÓÒ¼ü ³¤°´*/	
		case MSG_RIGHTDOWN_LONGKEY_UP:			/*ÓÒ¼ü ³¤°´ËÉ¿ª*/		
		case MSG_TOUCH_UP_FLICK:					/*ÉÏ*/
		case MSG_TOUCH_DOWN_FLICK:				/*ÏÂ*/	
		case MSG_TOUCH_UP_DOUBLE_SHORTKEY:		/*´¥Ãþ Ë«»÷*/	
			break;

	 	case MSG_TOUCH_LONGKEY:					/*´¥Ãþ ³¤°´*/
			if ((SmardCard_Index == 0) && (nfc_adv == 0))
			{
				nfc_adv = 0xff;
				MotoRun(MOTO_SIT);
				SetSystemSuspendTime(25);
				ShowPaymentUnlock();
				osal_start_timerEx(GetAppTaskId(), MSG_NFC_ACTION_OPEN, 100);
				osal_start_timerEx(GetAppTaskId(), MSG_NFC_ACTION_CLOSE, 9000);
				osal_start_timerEx(GetAppTaskId(), MSG_NFC_ANIMATION, 100);
			}
			break;

	 	case MSG_TOUCH_DOWN_UP_FLICK:				/*×ó*/ 
			if (nfc_status != NFC_STATUS_START)
			{
				if (SmardCard_Index == 0)
				{
					SmardCard_Index = 1;
					SendMsg(MSG_DISPLAY);
				}
				else if (SmardCard_Index == 1)
				{
					SmardCard_Index = 2;
					SendMsg(MSG_DISPLAY);
				}
			}
			break;
			
 	 	case MSG_TOUCH_UP_DOWN_FLICK: 			/*ÓÒ*/
			if (nfc_status != NFC_STATUS_START)
			{
				if (SmardCard_Index == 2)
				{
					SmardCard_Index = 1;
					SendMsg(MSG_DISPLAY);
				}
				else if (SmardCard_Index == 1)
				{
					SmardCard_Index = 0;
					SendMsg(MSG_DISPLAY);
				}
			}
			break;
			
 	 	case MSG_TOUCH_UP_SHORTKEY:				/*´¥Ãþ µ¥»÷*/  
			#if 0
			if ((SmardCard_Index == 0) && (nfc_adv == 0))
			{
				nfc_adv = 0xff;
				MotoRun(MOTO_SIT);
				SetSystemSuspendTime(25);
				ShowPaymentUnlock();
				osal_start_timerEx(GetAppTaskId(), MSG_NFC_ACTION_OPEN, 100);
				osal_start_timerEx(GetAppTaskId(), MSG_NFC_ACTION_CLOSE, 9000);
				osal_start_timerEx(GetAppTaskId(), MSG_NFC_ANIMATION, 100);
			}
			#endif
			break;
			
		case MSG_RIGHTDOWN_SHORTKEY: 				/*ÓÒ¼ü µ¥»÷*/
			if (nfc_status != NFC_STATUS_START)
				UnloadHandler(SmardCardHadlerId.NfcHadlerId); 
			return RET_MSG_HANDLED; 


		case MSG_NFC_ANIMATION:
			ShowPaymentAnimation1();
			break;
			
		case MSG_NFC_HANDLER_TIMEOUT:
			UnloadHandler(SmardCardHadlerId.NfcHadlerId);
			break;
			
		case MSG_NFC_ACTION_OPEN:
			swtich_to_idleMode();
			status = NFC_Action(NFC_OPEN);
			if (status == false) {
				osal_start_timerEx(GetAppTaskId(), MSG_NFC_ACTION_OPEN, 100);
			}
			break;
			
		case MSG_NFC_ACTION_CLOSE:
			osal_stop_timerEx(GetAppTaskId(),MSG_NFC_ANIMATION);
			if ((nfc_status == NFC_STATUS_FIELD_IDLE) || (nfc_status == NFC_STATUS_START)) {
				status = NFC_Action(NFC_CLOSE);
				if (status == false) {
					osal_start_timerEx(GetAppTaskId(), MSG_NFC_ACTION_CLOSE, 100);
				} else {
					if(CheckpCurrentHandler(SmardCardHadlerId.NfcHadlerId) == true)
						ShowPaymentAnimation2();
					SendMsg(MSG_NFC_HANDLER_TIMEOUT);
				}

			}
			else if ((nfc_status == NFC_STATUS_FIELD_OUT) || (nfc_status == NFC_STATUS_FIELD_IN)) {
				status = NFC_Action(NFC_CLOSE);
				if (status == false) {
					osal_start_timerEx(GetAppTaskId(), MSG_NFC_ACTION_CLOSE, 100);
				} else {
					//osal_start_timerEx(GetAppTaskId(), MSG_NFC_HANDLER_TIMEOUT, 100);
					osal_start_timerEx(GetAppTaskId(), MSG_READ_TRANSACTION_LOG,400);
					if(CheckpCurrentHandler(SmardCardHadlerId.NfcHadlerId) == true)
						ShowPaymentAnimation2();
					osal_stop_timerEx(GetAppTaskId(), MSG_NFC_ACTION_CLOSE);
					osal_start_timerEx(GetAppTaskId(), MSG_NFC_CONSUME, 500);
				}
			}
			break;

		case MSG_NFC_CONSUME:
			if(CheckpCurrentHandler(SmardCardHadlerId.NfcHadlerId) == true)
			{
				ShowPaymentConsume();
				osal_start_timerEx(GetAppTaskId(), MSG_NFC_BALANCE, 1500);
			}
			break;

		case MSG_NFC_BALANCE:
			SmardCard_Index = 1;
			log_data.Balance= log_data.Balance - log_data.Amount;
			if(CheckpCurrentHandler(SmardCardHadlerId.NfcHadlerId ) == true)
			{
				SendMsg(MSG_DISPLAY_BALANCE);
			}
			break;

		case MSG_NFC_LOG:
			break;
			
		case MSG_DISPLAY_BALANCE:
			if(CheckpCurrentHandler(SmardCardHadlerId.NfcHadlerId ) == false)
				break;
		case MSG_DISPLAY:
			if (SmardCard_Index == 0)
			{
				nfc_adv = 0;
				ShowPaymentLock();
			}
			else if (SmardCard_Index == 1)
			{
				ShowPaymentBalance();
			}
			else if (SmardCard_Index == 2)
			{
				ShowPaymentLog();
			}
			else if (SmardCard_Index == 4)
			{
				ShowPaySetUp();
			}		
			break;

		case MSG_ACTIVE_NFC:
			UnloadHandler_WithoutDisplay(SmardCardHadlerId.NfcHadlerId);
			SendMsg(MSG_ACTIVE_NFC);
			return RET_MSG_HANDLED;


		case MSG_POWER_ON_CMD:
		case MSG_POWER_OFF_CMD:
		case MSG_APDU_CMD:
		case MSG_VIBRATE_CMD:
		case MSG_APPLICATION_BLOCK_UNBLOCK:
		case MSG_READ_TRANSACTION_LOG:
		case MSG_WAIT_BLE_PACKETS_TIMEOUT:
		case MSG_NOTIFYCATION_ACC_BALANCE:
		default:
			return RET_MSG_UNHANDLED;

	}
	return RET_MSG_HANDLED;
}

void init_nfc(void)
{
	NFC_Action(NFC_OPEN);
	nrf_delay_ms(1);
	NFC_Action(NFC_CLOSE);
}


bool nfc_contact_test(void)
{
	uint8_t	bCommandResult = 0;
	uint8 status = 0;
	uint8 buf[50] = {0};
	uint16 len = 0;

	ChangeTimeoutValue(5000);
	
	//POWER ON
	status = spi_send_cmd(APDU_POWER_ON, buf,1);
	if	(status == SPI_FAILED) {
		bCommandResult = 1;
		goto POWER_OFF;
	}
	status = spi_receive_data(buf, (uint8_t *)&len);
	if (status != SPI_RSP_DATA) {
		bCommandResult = 1;
		goto POWER_OFF;
	}

	
	//select AID
	status = spi_send_cmd(APDU_TX_DATA, "\x80\xCA\x9F\x7F\x2D", 5);
	if	(status == SPI_FAILED) {
		bCommandResult = 1;
		goto POWER_OFF;
	}
	status = spi_receive_data(buf, (uint8_t *)&len);
	if (status != SPI_RSP_DATA) {
		bCommandResult = 1;
		goto POWER_OFF;
	}


POWER_OFF:
	
	//POWER OFF
	status = spi_send_cmd(APDU_POWER_OFF, buf,1);
	if	(status == SPI_FAILED) {
		bCommandResult = 1;
	}
	status = spi_receive_data(buf,(uint8_t *)&len);
	if (status != SPI_RSP_STATUS) {
		bCommandResult = 1;
	}

	ChangeTimeoutValue(DEFAULT_TIMEOUT_VALUE);

	if (bCommandResult)
		return false;

	return true;

}

bool set_booster_parameters(void)
{
	static uint8 para[]={0x04, 0x40, 0x00, 0x80, 0x00, 0x00, 0x02, 0x40, 0x08, 0x00, 0x20, 0x4A, 0xB7, 0x40, 0x10, 0x00, 0x08, 0x00, 0x00, 0x40, 0x18, 0x00, 0x00, 0x00, 0x00,0xE0};
	uint8 status = 0;
	uint8 buf[30] = {0};
	uint16 len = 0;
	uint8	bCommandResult = 0;

	ChangeTimeoutValue(9000);
	
	status = spi_send_cmd(BOOSTER_SET, para,26);
	if	(status == SPI_FAILED)
	{
		bCommandResult = 1;
		PRINTF("set_booster_parameters: spi_send_cmd()\r\n", 0);
	}
	status = spi_receive_data(buf, (uint8 *)&len);
	if (status != SPI_RSP_SUCCED)
	{
		bCommandResult = 1;
		PRINTF("set_booster_parameters: spi_receive_data()\r\n", 0);
	}

	ChangeTimeoutValue(DEFAULT_TIMEOUT_VALUE);

	
	if (bCommandResult)
		return false;

	return true;
}


bool get_booster_parameters(void)
{
	static uint8 para[18]={0x00,0x80, 0x00, 0x00, 0x02, 0x00, 0x20, 0x4A, 0xB7, 0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 0xF0, 0x00};
	uint8 status = 0;
	uint8 buf[30] = {0};
	uint16 len = 0;
	uint8	bCommandResult = 0;

	nrf_delay_ms(400);
	ChangeTimeoutValue(9000);
	
	status = spi_send_cmd(BOOSTER_GET, "\x20\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00",17);
	if	(status == SPI_FAILED)
	{
		bCommandResult = 1;
		PRINTF("get_booster_parameters: spi_send_cmd()\r\n", 0);
	}
	status = spi_receive_data(buf, (uint8 *)&len);
	if (status != SPI_RSP_DATA)
	{
		bCommandResult = 1;
		PRINTF("get_booster_parameters: spi_receive_data()\r\n", 0);
	}

	status = memcmp(para, buf, len-2);
	if (status != 0)
	{
		bCommandResult = 1;
	}

	ChangeTimeoutValue(DEFAULT_TIMEOUT_VALUE);

	if (bCommandResult)
		return false;

	return true;
}


void nfc_test(void)
{
	bool status = false;
	
	if (NFC_TEST == 0x00)
	{
		status = nfc_contact_test();
		if (status)
			SendRspCmd(CurrentDataParseCmd,E_RSP_OK);
		else
			SendRspCmd(CurrentDataParseCmd,E_RSP_FAIL);

		NfcTest = 0xff;
	}
	else if (NFC_TEST == 0x01)
	{
		status = set_booster_parameters();
		if (status)
			SendRspCmd(CurrentDataParseCmd,E_RSP_OK);
		else
			SendRspCmd(CurrentDataParseCmd,E_RSP_FAIL);
		NfcTest = 0xff;
	}
	else if (NFC_TEST == 0x02)
	{
		status = NFC_Action(NFC_OPEN);
		if (status)
			SendRspCmd(CurrentDataParseCmd,E_RSP_OK);
		else
			SendRspCmd(CurrentDataParseCmd,E_RSP_FAIL);
		NfcTest = 0xff;	
	}
	else if (NFC_TEST == 0x03)
	{
		status = NFC_Action(NFC_CLOSE);
		if (status)
			SendRspCmd(CurrentDataParseCmd,E_RSP_OK);
		else
			SendRspCmd(CurrentDataParseCmd,E_RSP_FAIL);
		NfcTest = 0xff;	

	}

}


bool nfc_get_csn(uint8 * csn)
{
	//uint8 rapdu[]={0x9F, 0x7F, 0x2A, 0x40, 0x90, 0x78, 0x89, 0x19, 0x81, 0x50, 0x99, 0x01, 0x19, 0x62, 0x37, 0x00, 0x03, 0x00, 0x45, 0x00, 0x20, 0x40, 0x82, 0x62, 0x37, 0x12, 0x93, 0x62, 0x37, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x19, 0x45, 0x62, 0x37, 0xFF, 0xFF, 0xFF, 0xFF}; 
	uint8 bCommandResult = 0;
	uint8 status = 0;
	uint8 buf[50] = {0};
	uint16 len = 0;

	ChangeTimeoutValue(5000);


	//POWER ON
	status = spi_send_cmd(APDU_POWER_ON, buf,1);
	if	(status == SPI_FAILED) {
		PRINTF("nfc_get_csn 1 \r\n");
		bCommandResult = 1;
		goto POWER_OFF;
	}
	status = spi_receive_data(buf, (uint8_t *)&len);
	if (status != SPI_RSP_DATA) {
		PRINTF("nfc_get_csn 2\r\n");
		bCommandResult = 1;
		goto POWER_OFF;
	}


	//select AID
	status = spi_send_cmd(APDU_TX_DATA, "\x80\xCA\x9F\x7F\x2D", 5);
	if	(status == SPI_FAILED) {
		PRINTF("nfc_get_csn 3\r\n");
		bCommandResult = 1;
		goto POWER_OFF;
	}
	status = spi_receive_data(buf, (uint8_t *)&len);
	if (status != SPI_RSP_DATA) {
		PRINTF("nfc_get_csn 4\r\n");
		bCommandResult = 1;
		goto POWER_OFF;
	}
	else{
		memcpy(csn, (const void *)&buf[13], 10);
	}

	POWER_OFF:

	//POWER OFF
	status = spi_send_cmd(APDU_POWER_OFF, buf,1);
	if	(status == SPI_FAILED) {
		PRINTF("nfc_get_csn 5\r\n");
		bCommandResult = 1;
	}
	status = spi_receive_data(buf,(uint8_t *)&len);
	if (status != SPI_RSP_STATUS) {
		PRINTF("nfc_get_csn 6\r\n");
		bCommandResult = 1;
	}

	ChangeTimeoutValue(DEFAULT_TIMEOUT_VALUE);
	
	if (bCommandResult)
		return false;

	return true;

}


void nfc_test_csn(void)
{
	uint8 csn_data[12]={0};
	bool status = false;
	
	status = nfc_get_csn(csn_data);
	if (status)
		SendCSNData(csn_data);
	else
		SendCSNData(csn_data);

	GetCSN = 0xff;
}

