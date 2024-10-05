#ifndef _MAPPER_H_
#define _MAPPER_H_
#include "mytype.h"

typedef struct
{
	void	(*Reset)(void);
	void	(*Write)( WORD addr, BYTE data );
	void	(*Read)( WORD addr, BYTE data );
	BYTE	(*ReadLow)( WORD addr );
	void	(*WriteLow)( WORD addr, BYTE data );
	BYTE	(*ExRead)( WORD addr );
	void	(*ExWrite)( WORD addr, BYTE data );
	BYTE	(*ExCmdRead)(void);
} MAPPER;

extern MAPPER NES_Mapper;

void Mapper_Init(void);
BOOL CreateMapper(int no);
extern void Mapper_Reset(void);
void Mapper_Write( WORD addr, BYTE data );
void Mapper_Read( WORD addr, BYTE data );
BYTE Mapper_ReadLow( WORD addr );
void Mapper_WriteLow( WORD addr, BYTE data );
BYTE Mapper_ExRead( WORD addr );
void Mapper_ExWrite( WORD addr, BYTE data );
BYTE Mapper_ExCmdRead(void);

#endif
