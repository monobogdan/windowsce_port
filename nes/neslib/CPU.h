#ifndef _CPU_H_
#define _CPU_H_
#include "mytype.h"

#define	NMI_FLAG	0x01		//不可屏蔽中断
#define	IRQ_FLAG	0x02		//可屏蔽中断
#define	NMI_VECTOR	0xFFFA		//不可屏蔽中断向量
#define	RES_VECTOR	0xFFFC		//Reset向量
#define	IRQ_VECTOR	0xFFFE		//IRQ向量

//6502标志位
#define	C_FLAG		0x01		// 1: Carry
#define	Z_FLAG		0x02		// 1: Zero
#define	I_FLAG		0x04		// 1: Irq disabled
#define	D_FLAG		0x08		// 1: Decimal mode flag (NES unused)
#define	B_FLAG		0x10		// 1: Break
#define	R_FLAG		0x20		// 1: Reserved (Always 1)
#define	V_FLAG		0x40		// 1: Overflow
#define	N_FLAG		0x80		// 1: Negative

void CPU_NMI(void); 


typedef struct 
{
	BYTE A;
	BYTE X;
	BYTE Y;
	WORD PC;
	BYTE S;
	BYTE P;
	BYTE INT_pending;
	long m_BaseCycle;
	long m_EmuCycle;
	int m_DMACycle;
}CPU6502;
extern CPU6502 CPU;

void CPU_Reset(void);
BYTE CPU_Read(WORD addr);
WORD CPU_ReadW(WORD addr);
void CPU_Write(WORD addr, BYTE val);
BYTE * CPU_GetRAM(WORD addr);
void CPU_ExecOnBaseCycle(int BaseCycle);
int CPU_Exec(int CpuCycle);

#endif
