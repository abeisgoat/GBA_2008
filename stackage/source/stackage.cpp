#include "gba.h"
#include "yellowblock.h"
#include "redblock.h"
#include "greenblock.h"
#include "blueblock.h"
#include "bg.h"
#include <cstring>

#define REG_DISPCNT (*(volatile unsigned short*)0x4000000)
#define REG_KEYS (*(unsigned short*)0x4000130)

void InitializeSprites(void);
void CopyOAM(void);
void MoveSprite(OAMEntry* sp, int x, int y);
void MoveBall(void);

OAMEntry sprites[128];

u16 xball = 0;
u16 xballnew = 0;
u16 yball = 150;
u16 speed = 0;

u16 mode = 0;

u16 sprite = 0;
u16 apress = 0;

int main() {

	u16 loop;

	SetMode(MODE_4 | BG2_ENABLE | OBJ_ENABLE | OBJ_MAP_1D);

	for (loop = 0; loop < 256; loop++) {
		BG_PaletteMem[loop]=bg_pal[loop];
	}

	for (loop = 0; loop < (120*160); loop++) {
		FrontBuffer[loop] = bg_gfx[loop] ;
	}

	for(loop = 0; loop < 256; loop++) {
	OBJ_PaletteMem[loop] = yellowblockPalette[loop];
	}

	memcpy( (u16 *)0x06014000, &yellowblockData, sizeof(yellowblockData) );
	memcpy( (u16 *)0x06014200, &redblockData, sizeof(redblockData) );
	memcpy( (u16 *)0x06014400, &greenblockData, sizeof(greenblockData) );
	memcpy( (u16 *)0x06014600, &blueblockData, sizeof(blueblockData) );

	while(1)
	{
		MoveBall();
		MoveSprite(&sprites[sprite],xball,yball-(16*sprite));
		WaitForVsync();
		CopyOAM();
	}

	return 0;

}

void InitializeSprites(void)
{
	u16 loop;
	for(loop = 0; loop < 128; loop++) {
		sprites[loop].attribute0 = 160;
		sprites[loop].attribute1 = 240;
	}
}

void CopyOAM(void)
{
	u16 loop;
	u16* temp;
	temp = (u16*)sprites;
	for(loop = 0; loop < 128*4; loop++)	{
		OAM_Mem[loop] = temp[loop];
	}
}

void MoveSprite(OAMEntry* sp, int x, int y)
{


	sp->attribute1 = sp->attribute1 & 0xFE00;
	sp->attribute1 = sp->attribute1 | x;

	sp->attribute0 = sp->attribute0 & 0xFF00;
	sp->attribute0 = sp->attribute0 | y;
}

void MoveBall(void) {
   if(speed == 0 && apress == 0) {
	if (sprite > 10) {sprite = 0;}else{sprite++;}
       	sprites[sprite].attribute0 = COLOR_256 | SQUARE | yball-(16*sprite);
	sprites[sprite].attribute1 = SIZE_16 | xball;
	sprites[sprite].attribute2 = 512;

	speed = 1;
	
   }

   if(!(REG_KEYS & KEY_A)) {
      if (apress == 0) speed = 0; apress = 1;
      }else{ apress = 0; }
	
   if (mode == 0 && xballnew < 240-16) {
	xballnew += speed;
   }else if (mode == 0 && xballnew == 240-16) {
	mode = 1;
   }

   if (mode == 1 && xballnew > 0) {
	xballnew -= speed;
   }else if (mode == 1 && xballnew == 0) {
	mode = 0;
   }

   if (xball-16 == xballnew || xball+16 == xballnew) {
	xball = xballnew;
   }
}

