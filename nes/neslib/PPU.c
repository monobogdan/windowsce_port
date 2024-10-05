
#include "Waves'NES.h"
#include "CPU.h"
#include "PPU.h"


PPU6528 PPU;
SCREENOFFSET m_ScreenOffset;
SCREENOFFSET m_CurLineOft;
ADDRESS      m_Address;
 uint16 NES_Palette[]=
{
	0x73AE,0x20D1,0x0015,0x4013,0x880E,0xA802,0xA000,0x7840,
	0x4160,0x0220,0x0280,0x01E2,0x19EB,0x0000,0x0000,0x0000,
	0xBDF7,0x039D,0x21DD,0x801E,0xB817,0xE00B,0xD940,0xCA61,
	0x8B80,0x04A0,0x0540,0x0487,0x0411,0x0000,0x0000,0x0000,
	0xF79E,0x3DFF,0x5CBF,0xA45F,0xF3DF,0xFBB6,0xFBAC,0xFCC7,
	0xF5E7,0x8682,0x4EE9,0x5FD3,0x075B,0x0000,0x0000,0x0000,
	0xF79E,0xAF3F,0xC6BF,0xD65F,0xFE3F,0xFE3B,0xFDF6,0xFED5,
	0xFF34,0xE7F4,0xAF97,0xB7F9,0x9FFE,0x0000,0x0000,0x0000
};

void PPU_Reset()
{
	// Quick endianess flip
	int i;

	for(i = 0; i < 64; i++)
	{
		uint16* val = &NES_Palette[i];
		char* ptr = (char*)val;
		unsigned char b1 = val[0], b2 = val[1];

		ptr[0] = b2;
		ptr[1] = b1;
	}

	memset(PPU.m_REG,0,sizeof(PPU.m_REG));
	memset(SPRAM,0,sizeof(SPRAM));
	PPU.m_ByteIndex=0;
}

BYTE PPU_ReadFromPort(WORD addr)
{
	BYTE value=0x00;
	WORD address;

	switch (addr)
	{
		case 0x2002 :
			value=PPU.m_REG[2];
			ADDRESS_Reset(&m_Address);//m_Address.Reset();
			SCREENOFFSET_Reset(&m_ScreenOffset);
			PPU.m_REG[2]&=~PPU_VBLANK_FLAG;
			break;
		case 0x2004 :
			value=SPRAM[PPU.m_REG[3]++];
			break;
		case 0x2007 :
			value=PPU.m_Reg2007Temp;
			address=ADDRESS_GetAddress(&m_Address);//m_Address.GetAddress();
			PPU.m_Reg2007Temp=PPU_Read(address);
			if(address>=0x3F00&&address<0x4000)
			{
				if(address&0x0010)value=SPPal[address&0xF];
				else value=BGPal[address&0xF];
			}
			ADDRESS_Step(&m_Address,PPU.m_REG[0]&PPU_INC32_BIT ? 32:1);
	}
	return value;
}

void PPU_WriteToPort(WORD addr, BYTE val)
{
	switch (addr)
	{
	case 0x2000 :
		//√¸√˚±Ìµÿ÷∑
		//(b:0000110000000000) = (d:00000011<<10)	(10-11)	
		PPU.m_ByteIndex = (PPU.m_ByteIndex & 0xF3FF) | (((WORD)val & 0x03) << 10);
		if((val & 0x80) && !(PPU.m_REG[0] & 0x80) && (PPU.m_REG[2] & 0x80))
			CPU_NMI();
		PPU.m_REG[0] = val;
		break;
	case 0x2001 :
		PPU.m_REG[1] = val;
		break;
	case 0x2003 :
		PPU.m_REG[3] = val;
		break;
	case 0x2004 :
		SPRAM[PPU.m_REG[3]++] = val;
		break;
	case 0x2005 :
		if (SCREENOFFSET_AtX(&m_ScreenOffset))
			PPU.m_ByteIndex = (PPU.m_ByteIndex & 0xFFE0) | (val >> 3);
		else
			PPU.m_ByteIndex = (PPU.m_ByteIndex & 0xFC1F) | ((val & 0xF8) << 2);
		SCREENOFFSET_SetValue(&m_ScreenOffset,val);
		break;
	case 0x2006 :
		if (!ADDRESS_AtLow(&m_Address))//if (!m_Address.AtLow())
		{
			PPU.m_ByteIndex = (PPU.m_ByteIndex & 0xF3FF) | ((val & 0x0C) << 8);
		}
		ADDRESS_SetAddress(&m_Address,val);//m_Address.SetAddress(val);
		break;
	case 0x2007 :
		PPU_Write(ADDRESS_GetAddress(&m_Address), val);//PPU_Write(m_Address.GetAddress(), val);
		ADDRESS_Step(&m_Address,PPU.m_REG[0] & PPU_INC32_BIT ? 32 : 1);//m_Address.Step(PPU.m_REG[0] & PPU_INC32_BIT ? 32 : 1);
		break;
	}
}

BYTE PPU_Read(WORD addr)
{
	if(addr<0x3000)return PPU_MEM_BANK[addr>>10][addr&0x3FF];
	else if(addr<0x3F00)return PPU_Read(addr-0x1000);
	else if(addr<0x4000)
	{
		if(addr&0x0010)return SPPal[addr&0xF];
		else return BGPal[addr&0xF];
	}
	else return PPU_Read(addr&0x3FFF);
}

void PPU_Write(WORD addr,BYTE val)
{
	if(addr<0x3000)PPU_MEM_BANK[addr>>10][addr&0x3FF]=val;
	else if(addr<0x3F00)PPU_Write(addr-0x1000,val);
	else if(addr<0x4000)
	{
		if(addr&0x0010)SPPal[addr&0xF]=val;
		else BGPal[addr&0xF]=val;
		if(!(addr&0x000F))
		{
			BGPal[0x0]=SPPal[0x10]=val;
			BGPal[0x04]=BGPal[0x08]=BGPal[0x0C]=BGPal[0x00];
			SPPal[0x14]=SPPal[0x18]=SPPal[0x1C]=SPPal[0x10];
		}
	}
	else PPU_Write(addr&0x3FFF,val);
}

void PPU_ScanlineStart()
{
	if (PPU.m_REG[1]&(PPU_SHOWBG|PPU_SHOWSPR))
	{
		m_CurLineOft=m_ScreenOffset;
		PPU.m_CurByteIndex=PPU.m_ByteIndex;
	}
}

extern uint16 *LCD;
void PPU_ScanLine(BYTE* pBit,int LineNo)
{
	int32 i,j=0;
	PPU.m_REG[2]&=~PPU_SPMAX_FLAG;
	PPU.m_CurLineSprite=0;
	PPU.m_CurOffsetY8=(m_CurLineOft.y+LineNo)&0x7;
	for(i=0;i<256;i++)pBit[i]=BGPal[0];	//‰÷»æµ◊±≥æ∞
	if (PPU.m_REG[1]&PPU_SHOWBG)PPU_ScanBG(pBit,LineNo);//…®√Ë±≥æ∞“≥
	if (PPU.m_REG[1]&PPU_SHOWSPR)
	{
		PPU_ScanSprite(pBit,LineNo,TRUE);//…®√Ë∫ÛÃ®æ´¡È
		PPU_ScanSprite(pBit,LineNo,FALSE);	//…®√Ë«∞Ã®æ´¡È
		PPU_ScanHitPoint(LineNo);			//…®√ËHitµ„
	}
	for(i=8;i<248;i++)
	{
	  j++;
	  if(j>=240)
	  {j=0;}

	  LCD[j+((240)*(LineNo))]=(uint16)NES_Palette[pBit[i]];
	}
}

//…®√ËHitµ„
void PPU_ScanHitPoint(BYTE LineNo)
{
	u8 i;
	int sp_h;
	int dy;
	WORD spraddr;
	BYTE LowByte;
	BYTE HighByte;

	if(PPU.m_REG[2]&PPU_SPHIT_FLAG)return;
	sp_h=(PPU.m_REG[0]&PPU_SP16_BIT)?15:7;//Sprite size 
	dy=(int)LineNo-((int)Sprite[0].Y+1);
	if(dy<0||dy>sp_h)return;
	if(Sprite[0].Attribute&SP_VREVERT)dy=sp_h-dy;	
	if(!(PPU.m_REG[0]&PPU_SP16_BIT))// 8x8 Sprite
	{		
		spraddr=(((WORD)PPU.m_REG[0]&PPU_SPTBL_BIT)<<9)+((WORD)Sprite[0].Index<<4);
		if(!(Sprite[0].Attribute&SP_VREVERT))spraddr+=dy;
		else spraddr+=7-dy;
	}
	else // 8x16 Sprite
	{		
		spraddr=(((WORD)Sprite[0].Index&1)<<12)+(((WORD)Sprite[0].Index&0xFE)<<4);
		if(!(Sprite[0].Attribute&SP_VREVERT))spraddr+=((dy&8)<<1)+(dy&7);
		else spraddr+=((~dy&8)<<1)+(7-(dy&7));
	}
	LowByte=PPU_MEM_BANK[spraddr>>10][spraddr&0x3FF];
	HighByte=PPU_MEM_BANK[spraddr>>10][(spraddr&0x3FF)+8];
	if(Sprite[0].Attribute&SP_HREVERT)
	{
		LowByte =RevertByte[LowByte];
		HighByte=RevertByte[HighByte];
	}
	for(i=0;i<8;i++)
	{
		if(LowByte&(1<<(7-i))||HighByte&(1<<(7-i)))
		{
			if(PPU_GetScreenBGColor(Sprite[0].X+i,LineNo)&0x03)
			{
				PPU.m_REG[2]|=PPU_SPHIT_FLAG;
				break;
			}
		}
	}
}

void PPU_ScanSprite(BYTE* pBit, BYTE LineNo, BOOL bBackLevel)
{
	u8 i,j;
	int dy;
	WORD spraddr;
	BYTE LowByte;
	BYTE HighByte;
	BYTE Color;
	BYTE HighColor;
	int sp_h=(PPU.m_REG[0]&PPU_SP16_BIT)?15:7;		//æ´¡È≥ﬂ¥Á£¨7ªÚ15
	for(i=0;i<64;i++)
	{
		if(Sprite[i].Attribute&SP_LEVEL&&!bBackLevel)continue;
		if(!(Sprite[i].Attribute & SP_LEVEL)&& bBackLevel)continue;
		dy=(int)LineNo-((int)Sprite[i].Y+1);
		if(dy!=(dy&sp_h))continue;
		PPU.m_CurLineSprite++;
		if(PPU.m_CurLineSprite>=8)PPU.m_REG[2]|=PPU_SPMAX_FLAG;
		if(!(PPU.m_REG[0]&PPU_SP16_BIT))// 8x8 Sprite
		{			
			spraddr=(((WORD)PPU.m_REG[0]&PPU_SPTBL_BIT)<<9)+((WORD)Sprite[i].Index<<4);
			if(!(Sprite[i].Attribute & SP_VREVERT))spraddr+=dy;
			else spraddr+=7-dy;
		}
		else // 8x16 Sprite
		{			
			spraddr=(((WORD)Sprite[i].Index&1)<<12)+(((WORD)Sprite[i].Index&0xFE)<<4);
			if(!(Sprite[i].Attribute&SP_VREVERT))spraddr+=((dy&8)<<1)+(dy&7);				
			else spraddr+=((~dy&8)<<1)+(7-(dy&7));
		}
		LowByte=PPU_MEM_BANK[spraddr>>10][spraddr&0x3FF];
		HighByte=PPU_MEM_BANK[spraddr>>10][(spraddr&0x3FF)+8];
		if (Sprite[i].Attribute & SP_HREVERT)
		{
			LowByte =RevertByte[LowByte];
			HighByte=RevertByte[HighByte];
		}
		HighColor=(Sprite[i].Attribute&SP_HIGHCOLOR)<<2;
		for(j=0;j<8;j++)
		{
			Color=HighColor|((HighByte>>(7-j)<<1)&0x02)|((LowByte>>(7-j))&0x01);
			if (Color&0x03&& Sprite[i].X+j<= 255)pBit[Sprite[i].X+j]=SPPal[Color];
		}
	}
}

void PPU_ScanBG(BYTE* pBit, BYTE LineNo)
{
	u8 j;
	int i;
	BYTE YOft;
	BYTE * pBGPattern;
	WORD AttrIndex;
	int LoopByte;
	int AttrBitOft;
	BYTE ByteLow;
	BYTE ByteHigh;
	BYTE ByteAttr;
	BYTE AttrBit;
	int xoft;
	BYTE ColorIndex;
	BYTE NTIndex=(PPU.m_CurByteIndex>>10)&0x3d;//NameTable index
	WORD ByteIndex=((m_CurLineOft.y+LineNo)%240>>3<<5)+(m_CurLineOft.x>>3);
	if(LineNo+m_CurLineOft.y>=240)NTIndex^=0x2;
	YOft=(LineNo+m_CurLineOft.y)&0x7;
	pBGPattern=PPU_MEM_BANK[(PPU.m_REG[0]&0x10)>>2];
	AttrIndex=(((ByteIndex>>7)&0x7)<<3)+((ByteIndex>>2)&0x7)+0x3C0;
	LoopByte=m_CurLineOft.x&0x7?33:32;
	for(i=0;i<LoopByte;i++)
	{
		if(i&&!(ByteIndex&0x1F))
		{
			AttrIndex&=0xFFF8;
			ByteIndex-=32;
			NTIndex^=1;
		}
		else if(i&&!(ByteIndex&0x3))AttrIndex++;
		AttrBitOft=(ByteIndex>>5)&0x2;
		ByteLow=pBGPattern[(PPU_MEM_BANK[NTIndex+8][ByteIndex]<<4)+YOft];
		ByteHigh=pBGPattern[(PPU_MEM_BANK[NTIndex+8][ByteIndex]<<4)+YOft+8];
		if(ByteLow|ByteHigh)
		{
			AttrBitOft |= (ByteIndex & 0x2) >> 1;
			ByteAttr = PPU_MEM_BANK[NTIndex + 8][AttrIndex];
			AttrBit  = (BYTE)(((ByteAttr >> (AttrBitOft <<1)) & 0x3) << 2);
			for (j = 0; j < 8; j++)
			{
				ColorIndex = ((ByteHigh >> (7 - j)) & 0x1) << 1;
				ColorIndex |= ((ByteLow >> (7 - j)) & 0x1);
				if (ColorIndex)
				{
					ColorIndex |= AttrBit;
					xoft = (i <<3) + j - (m_CurLineOft.x & 0x7);
					if (xoft >= 0 && xoft <= 255)
						pBit[(i <<3) + j - (m_CurLineOft.x & 0x7)] = BGPal[ColorIndex];
				}
			}
		}
		ByteIndex++;
	}
}

BYTE PPU_GetScreenBGColor(int x, int y)
{
	BYTE ByteBit;
	int CurNameTable = (PPU.m_CurByteIndex >> 10) & 0x3;
	int OffsetX = (m_CurLineOft.x + x) & 0xFF;
	int TotalY = m_CurLineOft.y + y;

	WORD NameByte = PPU_MEM_BANK[CurNameTable + 8][(TotalY >> 3 << 5)  + (OffsetX >> 3)];
	BYTE * pBGPattern = PPU_MEM_BANK[(PPU.m_REG[0] & 0x10) >> 2];
	BYTE LowByte  = pBGPattern[(NameByte << 4) + (TotalY & 0x7)];
	BYTE HighByte = pBGPattern[(NameByte << 4) + 8 + (TotalY & 0x7)];
	if (!(LowByte | HighByte))return 0;
	ByteBit = 7 - (OffsetX & 0x7);
	return GetBit(LowByte, ByteBit) | (GetBit(HighByte, ByteBit) << 1);
}

void PPU_VBlankStart()
{
	PPU.m_REG[2] |= PPU_VBLANK_FLAG;
	if (PPU.m_REG[0] & PPU_VBLANK_BIT)CPU_NMI();
}

void PPU_VBlankEnd()
{
	PPU.m_REG[2] &= ~PPU_VBLANK_FLAG;
	PPU.m_REG[2] &= ~PPU_SPHIT_FLAG;
}

