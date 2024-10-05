#include "APU.h"

BYTE APU_Read(WORD addr)
{
	BYTE data=0;
	if(addr==0x4017)data=(1<<6);
	return data;
}
