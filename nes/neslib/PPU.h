#ifndef _PPU_H_
#define _PPU_H_
#include "mytype.h"
#include "PPUINC.h"

//PPU���ƼĴ���1����(0x2000)
#define	PPU_VBLANK_BIT		0x80
#define	PPU_SPHIT_BIT		0x40
#define	PPU_SP16_BIT		0x20
#define	PPU_BGTBL_BIT		0x10
#define	PPU_SPTBL_BIT		0x08
#define	PPU_INC32_BIT		0x04
#define	PPU_NAMETBL_BIT		0x03

//PPU���ƼĴ���2����(0x2001)
#define PPU_SHOWCOLOR		0x00	//��ɫ
#define PPU_NOCOLOR			0x01	//��ɫ
#define PPU_LEFT8COL		0x02	//��ʾ��Ļ����8��
#define PPU_SPRLEFT8COL		0x04	//������8����ʾ����
#define PPU_SHOWBG			0x08	//����ҳ��ʾ����
#define PPU_SHOWSPR			0x10	//��ͨҳ��ʾ����

//PPU״̬�Ĵ�������(0x2002)
#define	PPU_VBLANK_FLAG		0x80
#define	PPU_SPHIT_FLAG		0x40
#define	PPU_SPMAX_FLAG		0x20
#define	PPU_WENABLE_FLAG	0x10

#define SP_VREVERT			0x80	//���鴹ֱ��ת
#define SP_HREVERT			0x40	//����ˮƽ��ת
#define SP_LEVEL			0x20	//�������ʾ��
#define SP_HIGHCOLOR		0X03	//������ɫ�ĸ�2λ

typedef struct 
{
	BYTE  * m_PatternTable;
	BYTE  * m_NameTable[4];
	BYTE  m_REG[0x04];						//$2000-$2003
	BYTE  m_Reg2007Temp;

	int32   m_CurLineSprite;
	WORD  m_ByteIndex;
	WORD  m_CurByteIndex;
	BYTE  m_CurOffsetX8;
	BYTE  m_CurOffsetY8;
}PPU6528;

extern SCREENOFFSET m_ScreenOffset;
extern SCREENOFFSET m_CurLineOft;
extern ADDRESS      m_Address;
extern PPU6528 PPU;

BYTE PPU_Read(WORD addr);
void PPU_Write(WORD addr, BYTE val);
void PPU_Reset(void);
void PPU_ScanlineStart(void);
void PPU_ScanLine(BYTE* pBit, int LineNo);	//��Ⱦɨ����
void PPU_ScanHitPoint(BYTE LineNo);
void PPU_ScanSprite(BYTE* pBit, BYTE LineNo, BOOL bBackLevel);//ɨ��ǰ����̨����
void PPU_ScanBG(BYTE* pBit, BYTE LineNo);//ɨ�豳��ҳ
BYTE PPU_GetScreenBGColor(int x, int y);
void PPU_VBlankStart(void);						//VBlank��ʼ
void PPU_VBlankEnd(void);						//VBlank����
BYTE PPU_ReadFromPort(WORD addr);
void PPU_WriteToPort(WORD addr, BYTE val);

#endif
