#include "mytype.h"

#define PRG_BLOCK_SIZE	0x4000
#define PAT_BLOCK_SIZE	0x2000
BYTE *RevertByte;
BYTE  PRGBlk[8*PRG_BLOCK_SIZE];
BYTE  PatTab[1*PAT_BLOCK_SIZE];


