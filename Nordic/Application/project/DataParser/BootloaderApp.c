
#include "headfile.h"








#define ENTER_UPDATE_STATE  0x01













//����1��ʾд����������ɹ����ȴ���������
uint8 SetCRCFlag(void)
{
        sd_power_gpregret_clr(0xFF);
        sd_power_gpregret_set(ENTER_UPDATE_STATE);
        
        return 1;
}


uint8 ClearCRCFlag(void)
{
	uint32_t tag;
	sd_power_gpregret_get(&tag);
	if(tag == 0x01)
		sd_power_gpregret_clr(0xff);	
	sd_power_gpregret_set(0);
	return 1;
}





 

