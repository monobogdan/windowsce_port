#include <stdlib.h>
#include <stdio.h>

#include "Waves'NES.h"
#include "CPU.h"
#include "PPU.h"
#include "PPUINC.h"
#include "APU.h"
#include "Mapper.h"

uint16 * w_getScreenBuffer(void);


int PROM_8K_SIZE;
int PROM_16K_SIZE;
int PROM_32K_SIZE;
BYTE* CPU_MEM_BANK[8];//每个元素指向8K数据

int VROM_1K_SIZE;
int VROM_2K_SIZE;
int VROM_4K_SIZE;
int VROM_8K_SIZE;
BYTE* PPU_MEM_BANK[12];//每个元素指向1K数据


BYTE* CRAM;
BYTE* VRAM;

BYTE RomHeader[16];


BYTE* RAM;
BYTE* SRAM;

extern BYTE PRGBlk[8*PRG_BLOCK_SIZE];
extern BYTE PatTab[1*PAT_BLOCK_SIZE];

BYTE* PRGBlock;
BYTE* PatternTable;	

BYTE NameTable[0x800];		//命名表
BYTE BGPal[0x20];			//背景调色板
BYTE SPPal[0x20];			//精灵调色板
BYTE SPRAM[256];			//精灵RAM，256字节，可存放64个精灵

BYTE LineBit[256];
SPRITE* Sprite;

extern u8 func_ptr;
extern const uint16 NES_Palette[];
extern const BYTE gCycle[];
int32 timerf;
BYTE* NES_buf;

BYTE*RomMalloc(int32 size)
{
     BYTE* buf;
     buf=malloc(size);
     memset(buf,0,size);
return buf;
}
void RomMallocAll()
{
     PRGBlock=PRGBlk;
     Sprite=(SPRITE*)SPRAM;
     PatternTable=PatTab;
     memset(PatternTable,0,sizeof(PatTab)); 
     memset(PRGBlock,0,sizeof(PRGBlk));
     RevertByte=RomMalloc(256);
     CRAM=RomMalloc(8*1024);
     VRAM=RomMalloc(2*1024);
     NES_buf=RomMalloc(512);
     RAM=RomMalloc(0x800);
     SRAM=RomMalloc(0x800);
}

void RUN_WaveNES()
{   
	RomMallocAll();
	NES_Init();
	LoadRomFile("\\Storage Card\\rom.nes");
	NES_Start();
	NES_FrameExec();
}

void NES_Init()
{	
	int32 i,j;
	BYTE c,mask;
	for(i=0;i<256;i++)
	{
		c=0;
		mask=0x80;
		for(j=0;j<8;j++)if(i&(1<<j))c|=(mask>>j);
		RevertByte[i]=c;
	}

	LCD = malloc(240 * 320 * sizeof(uint16));
}

void NES_Start()
{
	memset(NameTable, 0, sizeof(NameTable));
	CPU_Reset();
	PPU_Reset();
}

void NesTimer(int32 data)
{
int i;

for(i=0;i<240;i++)
		{
			PPU_ScanLine(LineBit,i);
            
			CPU_Exec(106);
			if(PPU.m_REG[1]&0x18)
			{
				m_CurLineOft=m_ScreenOffset;
				PPU.m_CurByteIndex=PPU.m_ByteIndex;
			}
           
		}

PPU.m_REG[2]|=PPU_VBLANK_FLAG;
		if(PPU.m_REG[0]&PPU_VBLANK_BIT)CPU_NMI();
		for(i=242;i<262;i++)
		{
			CPU_ExecOnBaseCycle(1024);
			CPU_ExecOnBaseCycle(338);
		}
		PPU.m_REG[2]&=~PPU_VBLANK_FLAG;
		PPU.m_REG[2]&=~PPU_SPHIT_FLAG;
		Get_key();

}
void NES_FrameExec() 
{

}

void LoadRomFile(char*NES_ROM)
{
	int32 MapperNo=0;
	int32 i=0,base=0;
    FILE* fnes;
    
	memset(CPU_MEM_BANK,0,sizeof(CPU_MEM_BANK));
	memset(PPU_MEM_BANK,0,sizeof(PPU_MEM_BANK));
	memset(CRAM,0,sizeof(CRAM));
	memset(VRAM,0,sizeof(VRAM));
	base=0;
	fnes=fopen(NES_ROM,"rb");
	fread(NES_buf, 512, 1, fnes);
	for(i=0;i<16;i++)
	{	
		RomHeader[i]=NES_buf[base];
		base++;
	}
	if(RomHeader[6]&0x04)base=512;
	if(RomHeader[4]>0)
	{		
		for(i=0;i<RomHeader[4]*PRG_BLOCK_SIZE;i++)
		{	
			if(base%512==0)fread(NES_buf,512, 1, fnes);
			PRGBlock[i]=NES_buf[base%512];
			base++;
		}
	}
	if(RomHeader[5]>0)
	{
		for(i=0;i<RomHeader[5]*PAT_BLOCK_SIZE;i++)
		{	
			if(base%512==0)fread(NES_buf, 512, 1, fnes);
			PatternTable[i]=NES_buf[base%512];
			base++;
		}
	}
	PROM_8K_SIZE=RomHeader[4]*2;
	PROM_16K_SIZE=RomHeader[4];
	PROM_32K_SIZE=RomHeader[4]/2;
	VROM_1K_SIZE=RomHeader[5]*8;
	VROM_2K_SIZE=RomHeader[5]*4;
	VROM_4K_SIZE=RomHeader[5]*2;
	VROM_8K_SIZE=RomHeader[5];
	if(VROM_8K_SIZE)SetVROM_8K_Bank(0);
	else SetCRAM_8K_Bank(0);
	if(RomHeader[6]&0x2){CPU_MEM_BANK[3]=SRAM;}

	if(RomHeader[6]&0x01){SetNameTable_Bank(0,1,0,1);}//垂直镜像	
	else {SetNameTable_Bank(0,0,1,1);}//水平镜像
	
	MapperNo=RomHeader[6]>>4;
	CreateMapper(MapperNo);
	NES_Mapper.Reset();
	fclose(fnes);
} 

void SetPROM_8K_Bank(BYTE page,int bank)
{
	bank%=PROM_8K_SIZE;
	CPU_MEM_BANK[page]=PRGBlock+0x2000*bank;
}

void SetPROM_16K_Bank(BYTE page,int bank)
{
	SetPROM_8K_Bank(page+0,bank*2+0);
	SetPROM_8K_Bank(page+1,bank*2+1);
}

void SetPROM_32K_Bank1(int bank)
{
	SetPROM_8K_Bank(4,bank*4+0);
	SetPROM_8K_Bank(5,bank*4+1);
	SetPROM_8K_Bank(6,bank*4+2);
	SetPROM_8K_Bank(7,bank*4+3);
}

void SetPROM_32K_Bank2(int bank0,int bank1,int bank2,int bank3)
{
	SetPROM_8K_Bank(4,bank0);
	SetPROM_8K_Bank(5,bank1);
	SetPROM_8K_Bank(6,bank2);
	SetPROM_8K_Bank(7,bank3);
}

void SetVROM_1K_Bank(BYTE page,int bank)
{
	bank%=VROM_1K_SIZE;
	PPU_MEM_BANK[page]=PatternTable+0x0400*bank;
}

void SetVROM_2K_Bank(BYTE page,int bank)
{
	SetVROM_1K_Bank(page+0,bank*2+0);
	SetVROM_1K_Bank(page+1,bank*2+1);
}

void SetVROM_4K_Bank(BYTE page,int bank)
{
	SetVROM_1K_Bank(page+0,bank*4+0);
	SetVROM_1K_Bank(page+1,bank*4+1);
	SetVROM_1K_Bank(page+2,bank*4+2);
	SetVROM_1K_Bank(page+3,bank*4+3);
}

void SetVROM_8K_Bank(int bank)
{
	int i;
	for(i=0;i<8;i++)
	{
	SetVROM_1K_Bank(i,bank*8+i);
	}
}

void SetCRAM_1K_Bank(BYTE page,int bank)
{
	bank&=0x1F;
	PPU_MEM_BANK[page]=CRAM+0x0400*bank;
}

void SetCRAM_2K_Bank(BYTE page,int bank)
{
	SetCRAM_1K_Bank(page+0,bank*2+0);
	SetCRAM_1K_Bank(page+1,bank*2+1);
}

void SetCRAM_4K_Bank(BYTE page,int bank)
{
	SetCRAM_1K_Bank(page+0,bank*4+0);
	SetCRAM_1K_Bank(page+1,bank*4+1);
	SetCRAM_1K_Bank(page+2,bank*4+2);
	SetCRAM_1K_Bank(page+3,bank*4+3);
}

void SetCRAM_8K_Bank(int bank)
{
	int i;
	for(i=0;i<8;i++)
	{
		SetCRAM_1K_Bank(i, bank*8+i);
	}
}

void SetVRAM_1K_Bank(BYTE page,int bank)
{
	bank &=3;
	PPU_MEM_BANK[page]=VRAM+0x0400*bank;
}

void SetNameTable_Bank(int bank0,int bank1,int bank2,int bank3)
{
	SetVRAM_1K_Bank( 8,bank0);
	SetVRAM_1K_Bank( 9,bank1);
	SetVRAM_1K_Bank(10,bank2);
	SetVRAM_1K_Bank(11,bank3);
}

BYTE NES_ReadReg(WORD addr)
{
	switch(addr&0xFF)
	{
		case 0x00: case 0x01: case 0x02: case 0x03:
		case 0x04: case 0x05: case 0x06: case 0x07:
		case 0x08: case 0x09: case 0x0A: case 0x0B:
		case 0x0C: case 0x0D: case 0x0E: case 0x0F:
		case 0x10: case 0x11: case 0x12: case 0x13:
			return APU_Read(addr);
		case 0x15:return APU_Read(addr);
		case 0x14:return 0x14;
		case 0x16:return (JOYPAD_GetValue(0)|0x40);
		case 0x17:return JOYPAD_GetValue(1)|APU_Read(addr);
		default:return 0x00;
	}
}

BYTE NES_Read(WORD addr)
{
	switch(addr>>13)
	{
		case 0x00:return RAM[addr&0x07FF];
		case 0x01:return PPU_ReadFromPort(addr&0xE007);
		case 0x02:
			if(addr<0x4100)return NES_ReadReg(addr);
			else return NES_Mapper.ReadLow(addr);
		case 0x03:return NES_Mapper.ReadLow(addr);
		case 0x04:
		case 0x05:
		case 0x06:	
		case 0x07:return CPU_MEM_BANK[addr>>13][addr&0x1FFF];
	}
	return 0x00;
}

void NES_WriteReg(WORD addr, BYTE val)
{		
	switch(addr&0xFF)
	{
		case 0x14 :
			memcpy(Sprite,CPU_GetRAM(((WORD)val)<<8),256);
			CPU.m_DMACycle+=514;
			break;
		case 0x16 :
			JOYPAD_InputBursh(val);
			break;
	}
}

void NES_Write(WORD addr, BYTE val)
{
	switch(addr>>13)
	{
		case 0x00:
			RAM[addr&0x07FF]=val;
			break;
		case 0x01:
			PPU_WriteToPort(addr&0xE007,val);
			break;
		case 0x02:	
			if(addr<0x4100)NES_WriteReg(addr,val);
			else NES_Mapper.WriteLow(addr,val);
			break;
		case 0x03:NES_Mapper.WriteLow(addr,val);break;
		case 0x04:
		case 0x05:
		case 0x06:
		case 0x07:	
			NES_Mapper.Write(addr, val);
			break;
	}
}

BYTE * NES_GetRAM(WORD addr)
{
	switch ( addr>>13 )
	{
		case 0x00:return &RAM[addr&0x07FF];
		case 0x04:
		case 0x05:
		case 0x06:
		case 0x07:return &CPU_MEM_BANK[addr>>13][addr&0x1FFF];
	}
	return 0;
}


