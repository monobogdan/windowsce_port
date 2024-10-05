#ifndef _JOYPAD_H_
#define _JOYPAD_H_
#include "mytype.h"

#define JOY_PAD_1		0
#define JOY_PAD_2		1
#define JOY_PAD_3		2
#define JOY_PAD_4		3

#define JOY_PAD_A		0x01
#define JOY_PAD_B		0x02
#define JOY_PAD_SELECT	0x04
#define JOY_PAD_START	0x08
#define JOY_PAD_UP		0x10
#define JOY_PAD_DOWN	0x20
#define JOY_PAD_LEFT	0x40
#define JOY_PAD_RIGHT	0x80

typedef struct 
{
	BOOL  m_bStrobe;
	int   m_CurIndex[2];
	DWORD m_PadBit[2];
}JOYPAD;

extern JOYPAD JoyPad;
void JOYPAD_SetState(int pad, int index, BYTE val);
void JOYPAD_InputBursh(BYTE burst);
BYTE JOYPAD_GetValue(int padbit);
void Get_key(void);

#endif
