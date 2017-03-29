
#include "Typedef.h"

typedef struct 
{
	uint16 msgCrc;
	uint16 dateCrc;
}ANCS_CRC;

void AncsSaveToFlash(void);

