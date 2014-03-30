//Project Started Jan 8th 12:28am
#include "gba.h"
#include "greenblock.h"
#include "greysquare.h"
#include "darkgreysquare.h"
#include "level1.h"
#include <cstring>

#define REG_KEYS (*(unsigned short*)0x4000130)

void InitializeSprites(void);
void CopyOAM(void);
void MoveSprite(OAMEntry* sp, int x, int y);
void MoveBall(void);
void loadlevel(int level);

OAMEntry sprites[128];


//int startposition[][] {{16*6, 16*2+8}, {16*5, 16*4+8}, {16*9, 16*4+8}}; 


u16 x = 0;
u16 y = 0;
u16 newx = x;
u16 newy = y;
u16 toppad = 8;

u16 uppress;
u16 downpress;
u16 rightpress;
u16 leftpress;

int speed = 1;
int sprite = 40;
int spriteCheck = 40;


int main() {
	u16 loop;
	SetMode(MODE_4 | BG2_ENABLE | OBJ_ENABLE | OBJ_MAP_1D);
	//Sets up player
	memcpy( (u16 *)0x06014000, &greenblockData, sizeof(greenblockData) );

	for(loop = 0; loop < 256; loop++) {
	OBJ_PaletteMem[loop] = greenblockPalette[loop];
	}

	sprites[1].attribute0 = COLOR_256 | SQUARE | y;
	sprites[1].attribute1 = SIZE_16 | x;
	sprites[1].attribute2 = 512;

	//Sets up ground sprite

	memcpy( (u16 *)0x06014200, &greysquare_gfx, sizeof(greysquare_gfx) );

	for(loop = 0; loop < 256; loop++) {
	OBJ_PaletteMem[loop] = greysquare_pal[loop];
	}

	//Sets up Dark Ground Sprite

	memcpy( (u16 *)0x06014400, &darkgreysquare_gfx, sizeof(darkgreysquare_gfx) );

	for(loop = 0; loop < 256; loop++) {
	OBJ_PaletteMem[loop] = darkgreysquare_pal[loop];
	}

	loadlevel(1);
	while(1)
	{
		MoveBall();
		MoveSprite(&sprites[1],x,y);
		WaitForVsync();
		CopyOAM();
		for(loop = 0; loop < 150; loop++) {
		if (level1[loop] == 1 && x == 16*(loop - level1y[loop]*15) && y == toppad+(level1y[loop]*16)) {
	        sprites[spriteCheck].attribute2 = 512 + 32;
		}
		if (level1[loop] == 1) spriteCheck++;
		}
		spriteCheck = 40;
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

void loadlevel(int level) {
	//Sprites 40 - onward for grey blocks
	x = 16*9;
	y = 16*4+8;
	newx = x;
	newy = y;
	toppad = 8;	
	
	u16 loop;

	//Sets up level
	if (level == 1) {
	for(loop = 0; loop < 150; loop++) {
	if (level1[loop] == 1) {
	sprites[sprite].attribute0 = COLOR_256 | SQUARE | (level1y[loop]*16) + toppad;
	sprites[sprite].attribute1 = SIZE_16 | (16*(loop-(15*level1y[loop])));
	sprites[sprite].attribute2 = 512 + 16;
	sprite++;
	}
	}
	}
}
void MoveBall(void) {
   if(!(REG_KEYS & KEY_UP)) {
      if (uppress == 0 && downpress == 0 && rightpress == 0 && leftpress == 0 && x == newx && y == newy && newy-16 >= 0 && level1[(newx/16) + ((y-21)/16)*15] == 1) newy -= 16; uppress = 1;
      }else{ uppress = 0; }
 
   if(!(REG_KEYS & KEY_DOWN)) {
      if (uppress == 0 && downpress == 0 && rightpress == 0 && leftpress == 0 && x == newx && y == newy && newy+16 <= 160 && level1[(newx/16) + ((y+11)/16)*15] == 1) newy += 16; downpress = 1;
      }else{ downpress = 0; }

   if(!(REG_KEYS & KEY_LEFT)) {
      if (uppress == 0 && downpress == 0 && rightpress == 0 && leftpress == 0 && x == newx && y == newy && newx-16 >= 0 && level1[(newx-16)/16 + ((y)/16)*15] == 1) newx -= 16; leftpress = 1;
      }else{ leftpress = 0; }
 
   if(!(REG_KEYS & KEY_RIGHT)) {
      if (uppress == 0 && downpress == 0 && rightpress == 0 && leftpress == 0 && x == newx && y == newy && newx+16 <= 240 && level1[(newx+16)/16 + ((y)/16)*15] == 1) newx += 16; rightpress = 1;
      }else{ rightpress = 0; }

   if(x < newx) x++;
   if(x > newx) x--;
   if(y < newy) y++;
   if(y > newy) y--;
}

