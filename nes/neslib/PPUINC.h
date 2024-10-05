#ifndef _PPUINC_H_
#define _PPUINC_H_
#include "mytype.h"

#define GetBit(val,i)  (((val)&(1<<i))>>i)

typedef struct 
{
	BYTE * current;
	BYTE x;
	BYTE y;
} SCREENOFFSET;

typedef struct 
{
	BYTE * current;
	BYTE LowAddr;
	BYTE HeightAddr;
} ADDRESS;

void SCREENOFFSET_Reset(SCREENOFFSET* obj);
void SCREENOFFSET_SetValue(SCREENOFFSET* obj,BYTE val);
BOOL SCREENOFFSET_AtX(SCREENOFFSET* obj);

void ADDRESS_Reset(ADDRESS* obj);
void ADDRESS_SetAddress(ADDRESS* obj,BYTE val);
WORD ADDRESS_GetAddress(ADDRESS* obj);
void ADDRESS_Step(ADDRESS* obj,int add);
BOOL ADDRESS_AtLow(ADDRESS* obj);

#endif

