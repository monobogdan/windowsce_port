#include "Mapper.h"
#include "Waves'NES.h"


MAPPER NES_Mapper;


void Mapper002_Reset()
{
	SetPROM_32K_Bank2(0, 1, PROM_8K_SIZE - 2, PROM_8K_SIZE - 1);
}

void Mapper002_WriteLow(WORD addr, BYTE data)
{
	Mapper_WriteLow(addr, data);
}

void Mapper002_Write(WORD addr, BYTE data)
{
	SetPROM_16K_Bank(4, data);
}

void Mapper002_Init()
{
	NES_Mapper.Reset = Mapper002_Reset;
	NES_Mapper.Write = Mapper002_Write;
	NES_Mapper.WriteLow = Mapper002_WriteLow;
}

void Mapper000_Reset()
{
	switch (RomHeader[4])
	{
	case 1 :	// 16K only
		SetPROM_16K_Bank(4, 0);
		SetPROM_16K_Bank(6, 0);
		break;
	case 2 :	// 32K
		SetPROM_32K_Bank1(0);
		break;
	}
}

void Mapper000_Init()
{
	NES_Mapper.Reset = Mapper000_Reset;
}

void Mapper_Init(void)
{
	NES_Mapper.Reset         = Mapper_Reset;
	NES_Mapper.Write         = Mapper_Write;
	NES_Mapper.Read          = Mapper_Read;
	NES_Mapper.WriteLow      = Mapper_WriteLow;
	NES_Mapper.ReadLow       = Mapper_ReadLow;	
	NES_Mapper.ExWrite       = Mapper_ExWrite;
	NES_Mapper.ExRead        = Mapper_ExRead;
	NES_Mapper.ExCmdRead     = Mapper_ExCmdRead;
}

BOOL CreateMapper(int no)
{
	Mapper_Init();
	switch (no)
	{
		case 0:Mapper000_Init();break;
		case 2:Mapper002_Init();break;
		default:break;
	}
	return TRUE;
}

void Mapper_Reset(void){}

BYTE Mapper_ReadLow(WORD addr)
{
	if(addr>=0x6000&&addr<=0x7FFF)return CPU_MEM_BANK[addr>>13][addr&0x1FFF];
	return (BYTE)(addr>>8);
}

void Mapper_WriteLow(WORD addr,BYTE data)
{
	if(addr>=0x6000&&addr<=0x7FFF)CPU_MEM_BANK[addr>>13][addr&0x1FFF]=data;
}

void Mapper_Write( WORD addr, BYTE data ){}

void Mapper_Read( WORD addr, BYTE data ){}

BYTE Mapper_ExRead( WORD addr )	
{ 
	return 0x00; 
}

void Mapper_ExWrite( WORD addr, BYTE data ){}

BYTE Mapper_ExCmdRead ()	
{ 
	return 0x00; 
}


