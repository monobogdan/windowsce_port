
#include "JOYPAD.h"
int8 KEY1,KEY2,KEY3,KEY4,KEY5,KEY6,KEY7,KEY8;
extern int8 KEY1,KEY2,KEY3,KEY4,KEY5,KEY6,KEY7,KEY8;

JOYPAD JoyPad;
void JOYPAD_SetState(int pad, int index, BYTE val)
{
	DWORD* PadBit=&JoyPad.m_PadBit[pad&0x1];
	DWORD mask=index;
	if(pad&0x2)mask<<=8;
	if(val)*PadBit|=mask;
	else *PadBit&=~mask;
}

//ÊäÈëÂö³å
void JOYPAD_InputBursh(BYTE burst)
{
	if(burst&0x1&&JoyPad.m_bStrobe==FALSE)JoyPad.m_bStrobe=TRUE;
	else if(!(burst&0x1)&&JoyPad.m_bStrobe)
	{
		JoyPad.m_bStrobe=FALSE;
		JoyPad.m_CurIndex[0]=JoyPad.m_CurIndex[1]=0;
	}
}

BYTE JOYPAD_GetValue(int padbit)
{
	BYTE ret=(BYTE)((JoyPad.m_PadBit[padbit]>>JoyPad.m_CurIndex[padbit])&0x1);
	JoyPad.m_CurIndex[padbit]++;
	if(JoyPad.m_CurIndex[padbit]>=24)//memset(JoyPad.m_CurIndex,0,sizeof(JoyPad.m_CurIndex));
	{
		JoyPad.m_CurIndex[0]=0;
		JoyPad.m_CurIndex[1]=0;
	}
	return ret;
}

void Get_key(void)
{
	DWORD* PadBit=&JoyPad.m_PadBit[0];
	*PadBit=(KEY1<<7)|(KEY2<<6)|(KEY3<<5)|(KEY4<<4)|(KEY5<<3)|(KEY6<<2)|(KEY7<<1)|KEY8;
}
			//  R       L		 D   	  U		   ST		SE		 B		A
//	keyvalue=(KEY<<7)|(KEY<<6)|(KEY<<5)|(KEY<<4)|(KEY<<3)|(KEY<<2)|(KEY<<1)|KEY
