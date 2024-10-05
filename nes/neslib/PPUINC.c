#include "PPUINC.h"

void SCREENOFFSET_Reset(SCREENOFFSET* obj)
{
	obj->current = &obj->x;
}

void SCREENOFFSET_SetValue(SCREENOFFSET* obj,BYTE val)
{
	*obj->current = val;
	if (obj->current == &obj->x)
		obj->current = &obj->y;
	else
		obj->current = &obj->x;
}

BOOL SCREENOFFSET_AtX(SCREENOFFSET* obj)
{
	return (obj->current == &obj->x);
}
///////////////////////////////////////////

void ADDRESS_Reset(ADDRESS* obj)
{
	obj->current = &obj->HeightAddr;
}

void ADDRESS_SetAddress(ADDRESS* obj,BYTE val)
{
	*obj->current = val;
	if (obj->current == &obj->HeightAddr)
		obj->current = &obj->LowAddr;
	else
		obj->current = &obj->HeightAddr;
}

WORD ADDRESS_GetAddress(ADDRESS* obj)
{
	return *(WORD *)&obj->LowAddr;
}

void ADDRESS_Step(ADDRESS* obj,int add)
{
	*(WORD *)&obj->LowAddr = *(WORD *)&obj->LowAddr + add;
}

BOOL ADDRESS_AtLow(ADDRESS* obj)
{
	return (obj->current == &obj->LowAddr);
}


