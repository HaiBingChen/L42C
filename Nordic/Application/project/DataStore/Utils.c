#include "headfile.h"




bool VerifyWatchId_old(uint8 * inWatchId, uint8 len)
{
	bool result = false;

	if(inWatchId != NULL && len >= L30_WATCH_ID_LEN)
	{
		if(strncmp((char*)inWatchId, L30_WATCH_ID_PREFIX, strlen(L30_WATCH_ID_PREFIX)) == 0)
		{
			result = true;
		}
	}

	return result;
}







