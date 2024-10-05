#ifndef _CPUINC_H_
#define _CPUINC_H_

// STACK操作(入栈操作和出栈操作)
#define	PUSH(V)		{ RAM[(CPU.S--) + 0x100]=(V); }
#define	POP()		RAM[(++CPU.S) + 0x100]

#define	SET_ZN_FLAG(A)	{CPU.P&=~(Z_FLAG|N_FLAG);CPU.P|=ZN_Table[(BYTE)(A)];}

#define	SET_FLAG(V)	{ CPU.P |=  (V); }	//设置标志位
#define	CLR_FLAG(V)	{ CPU.P &= ~(V); }	//清除标志位

#define	TST_FLAG(F,V)	{ CPU.P &= ~(V); if((F)) CPU.P |= (V); }
#define	CHK_FLAG(V)		(CPU.P&(V))

#define	MW_ZP()		ZPWR(EA,DT)
#define	MW_EA()		CPU_Write(EA,DT)

//零页寻址
#define	ZPRD(A)		(RAM[(BYTE)(A)])
#define	ZPRDW(A)	((WORD)RAM[(BYTE)(A)]+((WORD)RAM[(BYTE)((A)+1)]<<8))
#define	ZPWR(A,V)	{ RAM[(BYTE)(A)]=(V); }
#define	ZPWRW(A,V)	{ *((LPWORD)&RAM[(BYTE)(A)])=(WORD)(V); }

//立即数寻址
#define	MR_IM()	{		\
	DT = CPU_Read( CPU.PC++ );	\
}
//零页寻址
#define	MR_ZP()	{		\
	EA = CPU_Read( CPU.PC++ );	\
	DT = ZPRD( EA );	\
}
//零页X变址寻址
#define	MR_ZX()	{		\
	DT = CPU_Read( CPU.PC++ );	\
	EA = (BYTE)(DT + CPU.X);	\
	DT = ZPRD( EA );	\
}
//零页Y变址寻址
#define	MR_ZY()	{		\
	DT = CPU_Read( CPU.PC++ );	\
	EA = (BYTE)(DT + CPU.Y);	\
	DT = ZPRD( EA );	\
}
//绝对寻址
#define	MR_AB()	{		\
	EA = CPU_ReadW( CPU.PC );	\
	CPU.PC += 2;		\
	DT = CPU_Read( EA );	\
}
//绝对X变址寻址
#define	MR_AX()	{		\
	ET = CPU_ReadW( CPU.PC );	\
	CPU.PC += 2;		\
	EA = ET + (WORD)CPU.X;	\
	DT = CPU_Read( EA );	\
}
//绝对Y变址寻址
#define	MR_AY()	{		\
	ET = CPU_ReadW( CPU.PC );	\
	CPU.PC += 2;		\
	EA = ET + (WORD)CPU.Y;	\
	DT = CPU_Read( EA );	\
}
//先零页X变址后间址寻址
#define	MR_IX()	{		\
	DT = CPU_Read( CPU.PC++ );	\
	EA = ZPRDW( DT + CPU.X );	\
	DT = CPU_Read( EA );	\
}
//先零页间址后Y变址寻址
#define	MR_IY()	{		\
	DT = CPU_Read( CPU.PC++ );	\
	ET = ZPRDW( DT );	\
	EA = ET + (WORD)CPU.Y;	\
	DT = CPU_Read( EA );	\
}

// EFFECTIVE ADDRESS
#define	EA_ZP()	{		\
	EA = CPU_Read( CPU.PC++ );	\
}
#define	EA_ZX()	{		\
	DT = CPU_Read( CPU.PC++ );	\
	EA = (BYTE)(DT + CPU.X);	\
}
#define	EA_ZY()	{		\
	DT = CPU_Read( CPU.PC++ );	\
	EA = (BYTE)(DT + CPU.Y);	\
}
#define	EA_AB()	{		\
	EA = CPU_ReadW( CPU.PC );	\
	CPU.PC += 2;		\
}
#define	EA_AX()	{		\
	ET = CPU_ReadW( CPU.PC );	\
	CPU.PC += 2;		\
	EA = ET + CPU.X;		\
}
#define	EA_AY()	{		\
	ET = CPU_ReadW( CPU.PC );	\
	CPU.PC += 2;		\
	EA = ET + CPU.Y;		\
}
#define	EA_IX()	{		\
	DT = CPU_Read( CPU.PC++ );	\
	EA = ZPRDW( DT + CPU.X );	\
}
#define	EA_IY()	{		\
	DT = CPU_Read( CPU.PC++ );	\
	ET = ZPRDW( DT );	\
	EA = ET + (WORD)CPU.Y;	\
}

/* ADC (NV----ZC) */
#define	ADC() {							\
	WT = CPU.A+DT+(CPU.P&C_FLAG);				\
	TST_FLAG( WT > 0xFF, C_FLAG );				\
	TST_FLAG( ((~(CPU.A^DT))&(CPU.A^WT)&0x80), V_FLAG );	\
	CPU.A = (BYTE)WT;						\
	SET_ZN_FLAG(CPU.A);					\
}

/* SBC (NV----ZC) */
#define	SBC() {						\
	WT = CPU.A-DT-(~CPU.P&C_FLAG);			\
	TST_FLAG( ((CPU.A^DT) & (CPU.A^WT)&0x80), V_FLAG );	\
	TST_FLAG( WT < 0x100, C_FLAG );			\
	CPU.A = (BYTE)WT;					\
	SET_ZN_FLAG(CPU.A);				\
}

/* INC (N-----Z-) */
#define	INC() {			\
	DT++;			\
	SET_ZN_FLAG(DT);	\
}
/* INX (N-----Z-) */
#define	INX() {			\
	CPU.X++;			\
	SET_ZN_FLAG(CPU.X);	\
}
/* INY (N-----Z-) */
#define	INY() {			\
	CPU.Y++;			\
	SET_ZN_FLAG(CPU.Y);	\
}

/* DEC (N-----Z-) */
#define	DEC() {			\
	DT--;			\
	SET_ZN_FLAG(DT);	\
}
/* DEX (N-----Z-) */
#define	DEX() {			\
	CPU.X--;			\
	SET_ZN_FLAG(CPU.X);	\
}
/* DEY (N-----Z-) */
#define	DEY() {			\
	CPU.Y--;			\
	SET_ZN_FLAG(CPU.Y);	\
}

/* AND (N-----Z-) */
#define	AND() {			\
	CPU.A &= DT;		\
	SET_ZN_FLAG(CPU.A);	\
}

/* ORA (N-----Z-) */
#define	ORA() {			\
	CPU.A |= DT;		\
	SET_ZN_FLAG(CPU.A);	\
}

/* EOR (N-----Z-) */
#define	EOR() {			\
	CPU.A ^= DT;		\
	SET_ZN_FLAG(CPU.A);	\
}

/* ASL_A (N-----ZC) */
#define	ASL_A() {			\
	TST_FLAG( CPU.A&0x80, C_FLAG );	\
	CPU.A <<= 1;			\
	SET_ZN_FLAG(CPU.A);		\
}

/* ASL (N-----ZC) */
#define	ASL() {				\
	TST_FLAG( DT&0x80, C_FLAG );	\
	DT <<= 1;			\
	SET_ZN_FLAG(DT);		\
}

/* LSR_A (N-----ZC) */
#define	LSR_A() {			\
	TST_FLAG( CPU.A&0x01, C_FLAG );	\
	CPU.A >>= 1;			\
	SET_ZN_FLAG(CPU.A);		\
}
/* LSR (N-----ZC) */
#define	LSR() {				\
	TST_FLAG( DT&0x01, C_FLAG );	\
	DT >>= 1;			\
	SET_ZN_FLAG(DT);		\
}

/* ROL_A (N-----ZC) */
#define	ROL_A() {				\
	if( CPU.P & C_FLAG ) {			\
	TST_FLAG(CPU.A&0x80,C_FLAG);	\
	CPU.A = (CPU.A<<1)|0x01;		\
	} else {				\
	TST_FLAG(CPU.A&0x80,C_FLAG);	\
	CPU.A <<= 1;			\
	}					\
	SET_ZN_FLAG(CPU.A);			\
}
/* ROL (N-----ZC) */
#define	ROL() {					\
	if( CPU.P & C_FLAG ) {			\
	TST_FLAG(DT&0x80,C_FLAG);	\
	DT = (DT<<1)|0x01;		\
	} else {				\
	TST_FLAG(DT&0x80,C_FLAG);	\
	DT <<= 1;			\
	}					\
	SET_ZN_FLAG(DT);			\
}

/* ROR_A (N-----ZC) */
#define	ROR_A() {				\
	if( CPU.P & C_FLAG ) {			\
	TST_FLAG(CPU.A&0x01,C_FLAG);	\
	CPU.A = (CPU.A>>1)|0x80;		\
	} else {				\
	TST_FLAG(CPU.A&0x01,C_FLAG);	\
	CPU.A >>= 1;			\
	}					\
	SET_ZN_FLAG(CPU.A);			\
}
/* ROR (N-----ZC) */
#define	ROR() {					\
	if( CPU.P & C_FLAG ) {			\
	TST_FLAG(DT&0x01,C_FLAG);	\
	DT = (DT>>1)|0x80;		\
	} else {				\
	TST_FLAG(DT&0x01,C_FLAG);	\
	DT >>= 1;			\
	}					\
	SET_ZN_FLAG(DT);			\
}

/* BIT (NV----Z-) */
#define	BIT() {					\
	TST_FLAG( (DT&CPU.A)==0, Z_FLAG );	\
	TST_FLAG( DT&0x80, N_FLAG );		\
	TST_FLAG( DT&0x40, V_FLAG );		\
}

/* LDA (N-----Z-) */
#define	LDA()	{ CPU.A = DT; SET_ZN_FLAG(CPU.A); }
/* LDX (N-----Z-) */
#define	LDX()	{ CPU.X = DT; SET_ZN_FLAG(CPU.X); }
/* LDY (N-----Z-) */
#define	LDY()	{ CPU.Y = DT; SET_ZN_FLAG(CPU.Y); }

/* STA (--------) */
#define	STA()	{ DT = CPU.A; }
/* STX (--------) */
#define	STX()	{ DT = CPU.X; }
/* STY (--------) */
#define	STY()	{ DT = CPU.Y; }

/* TAX (N-----Z-) */
#define	TAX()	{ CPU.X = CPU.A; SET_ZN_FLAG(CPU.X); }
/* TXA (N-----Z-) */
#define	TXA()	{ CPU.A = CPU.X; SET_ZN_FLAG(CPU.A); }
/* TAY (N-----Z-) */
#define	TAY()	{ CPU.Y = CPU.A; SET_ZN_FLAG(CPU.Y); }
/* TYA (N-----Z-) */
#define	TYA()	{ CPU.A = CPU.Y; SET_ZN_FLAG(CPU.A); }
/* TSX (N-----Z-) */
#define	TSX()	{ CPU.X = CPU.S; SET_ZN_FLAG(CPU.X); }
/* TXS (--------) */
#define	TXS()	{ CPU.S = CPU.X; }

/* CMP (N-----ZC) */
#define	CMP_() {				\
	WT = (WORD)CPU.A - (WORD)DT;		\
	TST_FLAG( (WT&0x8000)==0, C_FLAG );	\
	SET_ZN_FLAG( (BYTE)WT );		\
}
/* CPX (N-----ZC) */
#define	CPX() {					\
	WT = (WORD)CPU.X - (WORD)DT;		\
	TST_FLAG( (WT&0x8000)==0, C_FLAG );	\
	SET_ZN_FLAG( (BYTE)WT );		\
}
/* CPY (N-----ZC) */
#define	CPY() {					\
	WT = (WORD)CPU.Y - (WORD)DT;		\
	TST_FLAG( (WT&0x8000)==0, C_FLAG );	\
	SET_ZN_FLAG( (BYTE)WT );		\
}

#define	JMP() {			\
	CPU.PC = CPU_ReadW( CPU.PC );	\
}
#define	JSR() {			\
	EA = CPU_ReadW( CPU.PC );	\
	CPU.PC++;			\
	PUSH( CPU.PC>>8 );	\
	PUSH( CPU.PC&0xFF );	\
	CPU.PC = EA;		\
}
#define	RTS() {			\
	CPU.PC  = POP();		\
	CPU.PC |= POP()*0x0100;	\
	CPU.PC++;			\
}
#define	RTI() {			\
	CPU.P   = POP() | R_FLAG;	\
	CPU.PC  = POP();		\
	CPU.PC |= POP()*0x0100;	\
}
#define	_NMI() {			\
	PUSH( PC>>8 );		\
	PUSH( PC&0xFF );		\
	CLR_FLAG( B_FLAG );		\
	PUSH( P );			\
	SET_FLAG( I_FLAG );		\
	PC = Cpu_RD6502W(NMI_VECTOR);	\
	exec_cycles += 7;		\
}
#define	_IRQ() {			\
	PUSH( CPU.PC>>8 );		\
	PUSH( CPU.PC&0xFF );		\
	CLR_FLAG( B_FLAG );		\
	PUSH( CPU.P );			\
	SET_FLAG( I_FLAG );		\
	CPU.PC = Cpu_RD6502W(IRQ_VECTOR);	\
	exec_cycles += 7;		\
}
#define	BRK() {				\
	CPU.PC++;				\
	PUSH( CPU.PC>>8 );		\
	PUSH( CPU.PC&0xFF );		\
	SET_FLAG( B_FLAG );		\
	PUSH( CPU.P );			\
	SET_FLAG( I_FLAG );		\
	CPU.PC = CPU_Read(IRQ_VECTOR);	\
}

#define	CHECK_EA()	{ if( (ET&0xFF00) != (EA&0xFF00) ) Cycle++; }

#define	REL_JUMP() {		\
	ET = CPU.PC;		\
	EA = CPU.PC + (signed char)DT;	\
	CPU.PC = EA;		\
	Cycle++;		\
	CHECK_EA();		\
}

#define	JMP_ID() {				\
	WT = CPU_ReadW(CPU.PC);			\
	EA = CPU_Read(WT);			\
	WT = (WT&0xFF00)|((WT+1)&0x00FF);	\
	CPU.PC = EA+CPU_Read(WT)*0x100;		\
}

#define	BCC()	{ if( !(CPU.P & C_FLAG) ) REL_JUMP(); }
#define	BCS()	{ if(  (CPU.P & C_FLAG) ) REL_JUMP(); }
#define	BNE()	{ if( !(CPU.P & Z_FLAG) ) REL_JUMP(); }
#define	BEQ()	{ if(  (CPU.P & Z_FLAG) ) REL_JUMP(); }
#define	BPL()	{ if( !(CPU.P & N_FLAG) ) REL_JUMP(); }
#define	BMI()	{ if(  (CPU.P & N_FLAG) ) REL_JUMP(); }
#define	BVC()	{ if( !(CPU.P & V_FLAG) ) REL_JUMP(); }
#define	BVS()	{ if(  (CPU.P & V_FLAG) ) REL_JUMP(); }

#define	CLC()	{ CPU.P &= ~C_FLAG; }
#define	CLD()	{ CPU.P &= ~D_FLAG; }
#define	CLI()	{ CPU.P &= ~I_FLAG; }
#define	CLV()	{ CPU.P &= ~V_FLAG; }
#define	SEC()	{ CPU.P |= C_FLAG; }
#define	SED()	{ CPU.P |= D_FLAG; }
#define	SEI()	{ CPU.P |= I_FLAG; }

#define	ANC()	{			\
	CPU.A &= DT;			\
	SET_ZN_FLAG( CPU.A );		\
	TST_FLAG( CPU.P&N_FLAG, C_FLAG );	\
}

#define	ANE()	{			\
	CPU.A = (CPU.A|0xEE)&CPU.X&DT;	\
	SET_ZN_FLAG( CPU.A );		\
}

#define	ARR()	{				\
	DT &= CPU.A;				\
	CPU.A = (DT>>1)|((CPU.P&C_FLAG)<<7);	\
	SET_ZN_FLAG( CPU.A );			\
	TST_FLAG( CPU.A&0x40, C_FLAG );		\
	TST_FLAG( (CPU.A>>6)^(CPU.A>>5), V_FLAG );	\
}

#define	ASR()	{			\
	DT &= CPU.A;			\
	TST_FLAG( DT&0x01, C_FLAG );	\
	CPU.A = DT>>1;			\
	SET_ZN_FLAG( CPU.A );		\
}

#define	DCP()	{			\
	DT--;				\
	CMP_();				\
}

#define	ISB()	{			\
	DT++;				\
	SBC();				\
}

#define	LAS()	{			\
	CPU.A = CPU.X = CPU.S = (CPU.S & DT);	\
	SET_ZN_FLAG( CPU.A );		\
}

#define	LAX()	{			\
	CPU.A = DT;			\
	CPU.X = CPU.A;			\
	SET_ZN_FLAG( CPU.A );		\
}

#define	LXA()	{			\
	CPU.A = CPU.X = ((CPU.A|0xEE)&DT);	\
	SET_ZN_FLAG( CPU.A );		\
}

#define	RLA()	{				\
	if( CPU.P & C_FLAG ) {			\
	TST_FLAG( DT&0x80, C_FLAG );	\
	DT = (DT<<1)|1;			\
	} else {				\
	TST_FLAG( DT&0x80, C_FLAG );	\
	DT <<= 1;			\
	}					\
	CPU.A &= DT;				\
	SET_ZN_FLAG( CPU.A );			\
}

#define	RRA()	{				\
	if( CPU.P & C_FLAG ) {			\
	TST_FLAG( DT&0x01, C_FLAG );	\
	DT = (DT>>1)|0x80;		\
	} else {				\
	TST_FLAG( DT&0x01, C_FLAG );	\
	DT >>= 1;			\
	}					\
	ADC();					\
}

#define	SAX()	{			\
	DT = CPU.A & CPU.X;			\
}

#define	SBX()	{			\
	WT = (CPU.A&CPU.X)-DT;		\
	TST_FLAG( WT < 0x100, C_FLAG );	\
	CPU.X = WT&0xFF;			\
	SET_ZN_FLAG( CPU.X );		\
}

#define	SHA()	{				\
	DT = CPU.A & CPU.X & (BYTE)((EA>>8)+1);	\
}

#define	SHS()	{			\
	CPU.S = CPU.A & CPU.X;		\
	DT = CPU.S & (BYTE)((EA>>8)+1);	\
}

#define	SHX()	{			\
	DT = CPU.X & (BYTE)((EA>>8)+1);	\
}

#define	SHY()	{			\
	DT = CPU.Y & (BYTE)((EA>>8)+1);	\
}

#define	SLO()	{			\
	TST_FLAG( DT&0x80, C_FLAG );	\
	DT <<= 1;			\
	CPU.A |= DT;			\
	SET_ZN_FLAG( CPU.A );		\
}

#define	SRE()	{			\
	TST_FLAG( DT&0x01, C_FLAG );	\
	DT >>= 1;			\
	CPU.A ^= DT;			\
	SET_ZN_FLAG( CPU.A );		\
}

#endif
