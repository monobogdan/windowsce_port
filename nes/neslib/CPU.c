#include "CPU.h"
#include "Waves'NES.h"
#include "CPUINC.h"

const BYTE gCycle[]=
{
	7, 6, 0, 0, 0, 3, 5, 0, 3, 2, 2, 0, 0, 4, 6, 0,
	2, 5, 0, 0, 0, 4, 6, 0, 2, 4, 0, 0, 0, 4, 7, 0,
	6, 2, 0, 0, 3, 3, 5, 0, 4, 2, 2, 0, 4, 4, 6, 0,
	2, 2, 0, 0, 0, 4, 6, 0, 2, 4, 0, 0, 0, 4, 7, 0,
	6, 6, 0, 0, 0, 3, 5, 0, 3, 2, 2, 0, 3, 4, 6, 0,
	2, 5, 0, 0, 0, 4, 6, 0, 2, 4, 0, 0, 0, 4, 7, 0,
	6, 6, 0, 0, 0, 3, 5, 0, 4, 2, 2, 0, 5, 4, 6, 0,
	2, 5, 0, 0, 0, 4, 6, 0, 2, 4, 0, 0, 0, 4, 7, 0,
	0, 6, 0, 0, 3, 3, 3, 0, 2, 0, 2, 0, 4, 4, 4, 0,
	2, 6, 0, 0, 4, 4, 4, 0, 2, 5, 2, 0, 0, 5, 0, 0,
	2, 6, 2, 0, 3, 3, 3, 0, 2, 2, 2, 0, 4, 4, 4, 0,
	2, 5, 0, 0, 4, 4, 4, 0, 2, 4, 2, 0, 4, 4, 4, 0,
	2, 6, 0, 0, 3, 3, 5, 0, 2, 2, 2, 0, 4, 4, 6, 0,
	2, 5, 0, 0, 0, 4, 6, 0, 2, 4, 0, 0, 0, 4, 7, 0,
	2, 6, 0, 0, 3, 3, 5, 0, 2, 2, 2, 0, 4, 4, 6, 0,
	2, 5, 0, 0, 0, 4, 6, 0, 2, 4, 0, 0, 0, 4, 7, 0
};

BYTE ZN_Table[256];
CPU6502 CPU;

void CPU_Reset()
{
	u8 i;
	CPU.A=0x00;
	CPU.X=0x00;
	CPU.Y=0x00;
	CPU.S=0xFF;
	CPU.PC=CPU_ReadW(RES_VECTOR);
	CPU.P=Z_FLAG|R_FLAG;	
	CPU.INT_pending=0;
	CPU.m_BaseCycle=0;
	CPU.m_EmuCycle=0;
	CPU.m_DMACycle=0;	
	ZN_Table[0]=Z_FLAG;
	for(i=0;i<255;i++)ZN_Table[i+1]=((i+1)&0x80)?N_FLAG:0;
}

BYTE CPU_Read(WORD addr)
{
	if(addr<0x2000)return RAM[addr&0x07FF];
	else return NES_Read(addr);
}
void CPU_NMI(void)  
{
	CPU.INT_pending|=NMI_FLAG;
}
WORD CPU_ReadW(WORD addr)
{
	if(addr<0x2000)return *((WORD*)&RAM[addr&0x07FF]);
	else if(addr<0x8000)return (WORD)NES_Read(addr)+((WORD)NES_Read(addr+1)<<8);
	return *((WORD*)&CPU_MEM_BANK[addr>>13][addr&0x1FFF]);
}

void CPU_Write(WORD addr,BYTE val)
{	
	if(addr<0x2000)RAM[addr&0x07FF]=val;
	else NES_Write(addr,val);
}

BYTE* CPU_GetRAM(WORD addr)
{
	if(addr<0x2000)return &RAM[addr&0x7FF];
	else return NES_GetRAM(addr);
}

void CPU_ExecOnBaseCycle(int BaseCycle)
{
	int Cycle;
	CPU.m_BaseCycle+=BaseCycle;
	Cycle=(int)((CPU.m_BaseCycle/12)-CPU.m_EmuCycle);
	if(Cycle>0)
	{CPU.m_EmuCycle+=CPU_Exec(Cycle);}
}

int CPU_Exec(int CpuCycle)
{
	int Cycle=0;
	BYTE OpCode;
	WORD EA;
	WORD ET;
	WORD WT;
	BYTE DT;

	while(Cycle<CpuCycle)
	{
		if(CPU.m_DMACycle>0)
		{
			if((CpuCycle-Cycle)<=CPU.m_DMACycle)
			{
				CPU.m_DMACycle-=(CpuCycle-Cycle);
				Cycle=CpuCycle;
				break;
			}
			else
			{
				Cycle+=CPU.m_DMACycle;
				CPU.m_DMACycle=0;
			}
		}
		OpCode=CPU_Read(CPU.PC++);
		switch(OpCode)
		{
			case 0x00:BRK();break;				
			case 0x01:MR_IX();ORA();break;			
			case 0x03:MR_IX();SLO();MW_EA();break;				
			case 0x05:MR_ZP();ORA();break;			
			case 0x06:MR_ZP();ASL();MW_ZP();break;				
			case 0x07:MR_ZP();SLO();MW_ZP();break;
			case 0x08:PUSH(CPU.P|B_FLAG);break;
			case 0x09:MR_IM();ORA();break;
			case 0x0A:ASL_A();break;
			case 0x0B:MR_IM();ANC();break;
			case 0x0D:MR_AB();ORA();break;
			case 0x0E:MR_AB();ASL();MW_EA();break;
			case 0x0F:MR_AB();SLO();MW_EA();break;
			case 0x10:MR_IM();BPL();break;
			case 0x11:MR_IY();ORA();CHECK_EA();break;
			case 0x13:MR_IY();SLO();MW_EA();Cycle+=8;break;
			case 0x15:MR_ZX();ORA();break;
			case 0x16:MR_ZX();ASL();MW_ZP();break;			
			case 0x17:MR_ZX();SLO();MW_ZP();Cycle+=6;break;
			case 0x18:CLC();break;
			case 0x19:MR_AY();ORA();CHECK_EA();break;
			case 0x1B:MR_AY();SLO();MW_EA();Cycle+=7;break;
			case 0x1D:MR_AX();ORA();CHECK_EA();break;
			case 0x1E:MR_AX();ASL();MW_EA();break;
			case 0x1F:MR_AX();SLO();MW_EA();Cycle+=7;break;
			case 0x20:JSR();break;
			case 0x21:MR_IX();AND();break;
			case 0x23:MR_IX();RLA();MW_EA();Cycle+=8;break;
			case 0x24:MR_ZP();BIT();break;
			case 0x25:EA=CPU_Read(CPU.PC++);DT=(RAM[(BYTE)(EA)]);AND();break;
			case 0x26:MR_ZP();ROL();MW_ZP();break;
			case 0x27:MR_ZP();RLA();MW_ZP();Cycle+=5;break;
			case 0x28:CPU.P=POP()|R_FLAG;break;
			case 0x29:MR_IM();AND();break;
			case 0x2A:ROL_A();break;
			case 0x2B:MR_IM();ANC();break;
			case 0x2C:MR_AB();BIT();break;
			case 0x2D:MR_AB();AND();break;
			case 0x2E:MR_AB();ROL();MW_EA();break;
			case 0x2F:MR_AB();RLA();MW_EA();Cycle+=6;break;
			case 0x30:MR_IM();BMI();break;
			case 0x31:MR_IY();AND();CHECK_EA();break;
			case 0x33:MR_IY();RLA();MW_EA();Cycle+=8;break;
			case 0x35:MR_ZX();AND();break;
			case 0x36:MR_ZX();ROL();MW_ZP();break;
			case 0x37:MR_ZX();RLA();MW_ZP();Cycle+=6;break;
			case 0x38:SEC();break;
			case 0x39:MR_AY();AND();CHECK_EA();break;
			case 0x3B:MR_AY();RLA();MW_EA();Cycle+=7;break;
			case 0x3D:MR_AX();AND();CHECK_EA();break;
			case 0x3E:MR_AX();ROL();MW_EA();break;
			case 0x3F:MR_AX();RLA();MW_EA();Cycle+=7;break;
			case 0x40:RTI();break;
			case 0x41:MR_IX();EOR();break;
			case 0x43:MR_IX();SRE();MW_EA();Cycle+=8;break;
			case 0x45:MR_ZP();EOR();break;
			case 0x46:MR_ZP();LSR();MW_ZP();break;
			case 0x47:MR_ZP();SRE();MW_ZP();Cycle+=5;break;
			case 0x48:PUSH(CPU.A);break;
			case 0x49:MR_IM();EOR();break;
			case 0x4A:LSR_A();break;
			case 0x4B:MR_IM();ASR();break;
			case 0x4C:JMP();break;
			case 0x4D:MR_AB();EOR();break;
			case 0x4E:MR_AB();LSR();MW_EA();break;
			case 0x4F:MR_AB();SRE();MW_EA();Cycle+=6;break;
			case 0x50:MR_IM();BVC();break;
			case 0x51:MR_IY();EOR();CHECK_EA();break;
			case 0x53:MR_IY();SRE();MW_EA();Cycle+=8;break;
			case 0x55:MR_ZX();EOR();break;
			case 0x56:MR_ZX();LSR();MW_ZP();break;
			case 0x57:MR_ZX();SRE();MW_ZP();Cycle+=6;break;
			case 0x58:CLI();break;
			case 0x59:MR_AY();EOR();CHECK_EA();break;
			case 0x5B:MR_AY();SRE();MW_EA();Cycle+=7;break;
			case 0x5D:MR_AX();EOR();CHECK_EA();break;
			case 0x5E:MR_AX();LSR();MW_EA();break;
			case 0x5F:MR_AX();SRE();MW_EA();Cycle+=7;break;
			case 0x60:RTS();break;
			case 0x61:MR_IX();ADC();break;
			case 0x63:MR_IX();RRA();MW_EA();Cycle+=8;break;
			case 0x65:MR_ZP();ADC();break;
			case 0x66:MR_ZP();ROR();MW_ZP();break;
			case 0x67:MR_ZP();RRA();MW_ZP();Cycle+=5;break;
			case 0x68:CPU.A=POP();SET_ZN_FLAG(CPU.A);break;
			case 0x69:MR_IM();ADC();break;
			case 0x6A:ROR_A();break;
			case 0x6B:MR_IM();ARR();break;
			case 0x6C:JMP_ID();break;
			case 0x6D:MR_AB();ADC();break;
			case 0x6E:MR_AB();ROR();MW_EA();break;
			case 0x6F:MR_AB();RRA();MW_EA();Cycle+=6;break;
			case 0x70:MR_IM();BVS();break;
			case 0x71:MR_IY();ADC();CHECK_EA();break;
			case 0x73:MR_IY();RRA();MW_EA();Cycle+=8;break;
			case 0x75:MR_ZX();ADC();break;
			case 0x76:MR_ZX();ROR();MW_ZP();break;
			case 0x77:MR_ZX();RRA();MW_ZP();Cycle+=6;break;
			case 0x78:SEI();break;
			case 0x79:MR_AY();ADC();CHECK_EA();break;
			case 0x7B:MR_AY();RRA();MW_EA();Cycle+=7;break;
			case 0x7D:MR_AX();ADC();CHECK_EA();break;
			case 0x7E:MR_AX();ROR();MW_EA();break;
			case 0x7F:MR_AX();RRA();MW_EA();Cycle+=7;break;
			case 0x81:EA_IX();STA();MW_EA();break;
			case 0x83:MR_IX();SAX();MW_EA();Cycle+=6;break;
			case 0x84:EA_ZP();STY();MW_ZP();break;
			case 0x85:EA_ZP();STA();MW_ZP();break;
			case 0x86:EA_ZP();STX();MW_ZP();break;
			case 0x87:MR_ZP();SAX();MW_ZP();Cycle+=3;break;
			case 0x88:DEY();break;
			case 0x8A:TXA();break;
			case 0x8B:MR_IM();ANE();break;
			case 0x8C:EA_AB();STY();MW_EA();break;
			case 0x8D:EA_AB();STA();MW_EA();break;
			case 0x8E:EA_AB();STX();MW_EA();break;
			case 0x8F:MR_AB();SAX();MW_EA();Cycle+=4;break;
			case 0x90:MR_IM();BCC();break;
			case 0x91:EA_IY();STA();MW_EA();break;
			case 0x93:MR_IY();SHA();MW_EA();Cycle+=6;break;
			case 0x94:EA_ZX();STY();MW_ZP();break;
			case 0x95:	EA_ZX();STA();MW_ZP();break;
			case 0x96:EA_ZY();STX();MW_ZP();break;
			case 0x97:MR_ZY();SAX();MW_ZP();Cycle+=4;break;
			case 0x98:TYA();break;
			case 0x99:EA_AY();STA();MW_EA();break;
			case 0x9A:TXS();break;
			case 0x9B:MR_AY();SHS();MW_EA();Cycle+=5;break;
			case 0x9C:MR_AX();SHY();MW_EA();Cycle+=5;break;
			case 0x9D:EA_AX();STA();MW_EA();break;
			case 0x9E:MR_AY();SHX();MW_EA();Cycle+=5;break;
			case 0x9F:MR_AY();SHA();MW_EA();Cycle+=5;break;
			case 0xA0:MR_IM();LDY();break;
			case 0xA1:MR_IX();LDA();break;
			case 0xA2:MR_IM();LDX();break;
			case 0xA3:MR_IX();LAX();Cycle+=6;break;
			case 0xA4:MR_ZP();LDY();break;
			case 0xA5:MR_ZP();LDA();break;
			case 0xA6:MR_ZP();LDX();break;
			case 0xA7:MR_ZP();LAX();Cycle+=3;break;
			case 0xA8:TAY();break;
			case 0xA9:MR_IM();LDA();break;
			case 0xAA:TAX();break;
			case 0xAB:MR_IM();LXA();Cycle+=2;break;
			case 0xAC:MR_AB();LDY();break;
			case 0xAD:MR_AB();LDA();break;
			case 0xAE:MR_AB();LDX();break;
			case 0xAF:MR_AB();LAX();Cycle+=4;break;
			case 0xB0:MR_IM();BCS();break;
			case 0xB1:MR_IY();LDA();CHECK_EA();break;
			case 0xB3:MR_IY();LAX();CHECK_EA();Cycle+=5;break;
			case 0xB4:MR_ZX();LDY();break;
			case 0xB5:MR_ZX();LDA();break;
			case 0xB6:MR_ZY();LDX();break;
			case 0xB7:MR_ZY();LAX();Cycle+=4;break;
			case 0xB8:CLV();break;
			case 0xB9:MR_AY();LDA();CHECK_EA();break;
			case 0xBA:TSX();break;
			case 0xBB:MR_AY();LAS();CHECK_EA();Cycle+=4;break;
			case 0xBC:MR_AX();LDY();CHECK_EA();break;
			case 0xBD:MR_AX();LDA();CHECK_EA();break;
			case 0xBE:MR_AY();LDX();CHECK_EA();break;
			case 0xBF:MR_AY();LAX();CHECK_EA();Cycle+=4;break;
			case 0xC0:MR_IM();CPY();break;
			case 0xC1:MR_IX();CMP_();break;
			case 0xC3:MR_IX();DCP();MW_EA();break;
			case 0xC4:MR_ZP();CPY();break;
			case 0xC5:MR_ZP();CMP_();break;
			case 0xC6:MR_ZP();DEC();MW_ZP();break;
			case 0xC7:MR_ZP();DCP();MW_ZP();break;
			case 0xC8:INY();break;
			case 0xC9:MR_IM();CMP_();break;
			case 0xCA:DEX();break;
			case 0xCB:MR_IM();SBX();Cycle+=2;break;
			case 0xCC:MR_AB();CPY();break;
			case 0xCD:MR_AB();CMP_();break;
			case 0xCE:MR_AB();DEC();MW_EA();break;
			case 0xCF:MR_AB();DCP();MW_EA();break;
			case 0xD0:MR_IM();BNE();break;
			case 0xD1:MR_IY();CMP_();CHECK_EA();break;
			case 0xD3:MR_IY();DCP();MW_EA();break;
			case 0xD5:MR_ZX();CMP_();break;
			case 0xD6:MR_ZX();DEC();MW_ZP();break;
			case 0xD7:MR_ZX();DCP();MW_ZP();break;
			case 0xD8:CLD();break;
			case 0xD9:MR_AY();CMP_();CHECK_EA();break;
			case 0xDB:MR_AY();DCP();MW_EA();break;
			case 0xDD:MR_AX();CMP_();CHECK_EA();break;
			case 0xDE:MR_AX();DEC();MW_EA();break;
			case 0xDF:MR_AX();DCP();MW_EA();break;
			case 0xE0:MR_IM();CPX();break;
			case 0xE1:MR_IX();SBC();break;
			case 0xE4:MR_ZP();CPX();break;
			case 0xE3:MR_IX();ISB();MW_EA();Cycle+=5;break;
			case 0xE5:MR_ZP();SBC();break;
			case 0xE6:MR_ZP();INC();MW_ZP();break;
			case 0xE7:MR_ZP();ISB();MW_ZP();break;
			case 0xE8:INX();break;
			case 0xE9:MR_IM();SBC();break;
			case 0xEA:break;
			case 0xEB:MR_IM();SBC();Cycle+=2;break;
			case 0xEC:MR_AB();CPX();break;
			case 0xED:MR_AB();SBC();break;
			case 0xEE:MR_AB();INC();MW_EA();break;
			case 0xEF:MR_AB();ISB();MW_EA();Cycle+=5;
			case 0xF0:MR_IM();BEQ();break;
			case 0xF1:MR_IY();SBC();CHECK_EA();break;
			case 0xF3:MR_IY();ISB();MW_EA();Cycle+=5;break;
			case 0xF5:MR_ZX();SBC();break;
			case 0xF6:MR_ZX();INC();MW_ZP();break;
			case 0xF7:MR_ZX();ISB();MW_ZP();Cycle+=5;break;
			case 0xF8:SED();break;
			case 0xF9:MR_AY();SBC();CHECK_EA();break;
			case 0xFB:MR_AY();ISB();MW_EA();Cycle+=5;break;
			case 0xFD:MR_AX();SBC();CHECK_EA();break;
			case 0xFE:MR_AX();INC();MW_EA();break;
			case 0xFF:MR_AX();ISB();MW_EA();Cycle+=5;break;
			case 0x1A:		
			case 0x3A:	
			case 0x5A:		
			case 0x7A:		
			case 0xDA:		
			case 0xFA:Cycle+=2;break;
			case 0x80:		
			case 0x82:		
			case 0x89:	
			case 0xC2:	
			case 0xE2:Cycle+=2;CPU.PC++;break;
			case 0x04:	
			case 0x44:	
			case 0x64:Cycle+=3;CPU.PC++;break;
			case 0x14:		
			case 0x34:	
			case 0x54:	
			case 0x74:	
			case 0xD4:	
			case 0xF4:Cycle+=4;CPU.PC++;break;
			case 0x0C:	
			case 0x1C:	
			case 0x3C:	
			case 0x5C:	
			case 0x7C:	
			case 0xDC:	
			case 0xFC:Cycle+=2;CPU.PC+=2;break;
			default:break;
		}
		Cycle+=gCycle[OpCode];
		if(CPU.INT_pending&NMI_FLAG)
		{
			CPU.INT_pending&=~NMI_FLAG;
			PUSH(CPU.PC>>8);
			PUSH(CPU.PC&0xFF);
			CLR_FLAG(B_FLAG);
			PUSH(CPU.P);
			SET_FLAG(I_FLAG);
			CPU.PC=CPU_ReadW(NMI_VECTOR);
			Cycle+=7;
		}
	}
	return Cycle;
}
