#include "headfile.h"


void NoticeModeChange(bool flag)
{
	bc5_ring_mode = BC5_RING;
	BC5_SetRingModeCmd(bc5_ring_mode);
}




