#include "gba.h"
#include "ball.h"
#include "hbar.h"
#include "slope.h"
#include "bg.h"
#include <cstring>


#define REG_DISPCNT (*(volatile unsigned short*)0x4000000)
#define REG_KEYS (*(unsigned short*)0x4000130)

void InitializeSprites(void);
void CopyOAM(void);
void MoveSprite(OAMEntry* sp, int x, int y);
void MoveBall(void);
void HitCheck(void);
void gravity(void);
void updateVars(void);

OAMEntry sprites[128];

int xposition = 10;
int yposition = 102;
u16 xball = xposition;
u16 yball = 10;

u16 xbar = xposition + 30;
u16 ybar = yposition + 34;      

u16 xbar2 = xposition - 10;
u16 ybar2 = yposition + 50;     

u16 xslope = xposition + 14;
u16 yslope = yposition + 42;    

u16 xslope2 = xposition + 22;
u16 yslope2 = yposition +34;     
    

int speed = 1.5;
int hit = 0;
int gspeed = 10;
int ugspeed = 6/3.5;

int main() {

	u16 loop;

	SetMode(MODE_4 | BG2_ENABLE | OBJ_ENABLE | OBJ_MAP_1D);

	for (loop = 0; loop < 256; loop++) {
		BG_PaletteMem[loop]=bgPalette[loop];
	}

	for (loop = 0; loop < (120*160); loop++) {
		FrontBuffer[loop] = bgData[loop] ;
	}

//ball
	for(loop = 0; loop < 256; loop++) {
	OBJ_PaletteMem[loop] = ballPalette[loop];
	}

	memcpy( (u16 *)0x06014000, &ballData, sizeof(ballData) );

	sprites[0].attribute0 = COLOR_256 | SQUARE | yball;
	sprites[0].attribute1 = SIZE_8 | xball;
	sprites[0].attribute2 = 512;

//bar2
	for(loop = 0; loop < 256; loop++) {
	OBJ_PaletteMem[loop] = hbarPalette[loop];
	}

	memcpy( (u16 *)0x06014100, &hbarData, sizeof(hbarData) );

	sprites[1].attribute0 = COLOR_256 | WIDE | ybar;
	sprites[1].attribute1 = SIZE_16 | xbar;
	sprites[1].attribute2 = 512 + 8;

	sprites[2].attribute0 = COLOR_256 | WIDE | ybar2;
	sprites[2].attribute1 = SIZE_16 | xbar2;
	sprites[2].attribute2 = 512 + 8;

//slope
	for(loop = 0; loop < 256; loop++) {
	OBJ_PaletteMem[loop] = slopePalette[loop];
	}

	memcpy( (u16 *)0x06014300, &slopeData, sizeof(slopeData) );

	sprites[3].attribute0 = COLOR_256 | SQUARE | yslope;
	sprites[3].attribute1 = SIZE_8 | xslope;
	sprites[3].attribute2 = 512 + 24;

	sprites[4].attribute0 = COLOR_256 | SQUARE | yslope2;
	sprites[4].attribute1 = SIZE_8 | xslope2;
	sprites[4].attribute2 = 512 + 24;


	while(1)
	{
		MoveBall();
		HitCheck();
		gravity();
		MoveSprite(&sprites[0],xball,yball);
		//MoveSprite(&sprites[1],xbar,ybar);
		//MoveSprite(&sprites[2],xbar2,ybar2);
		//MoveSprite(&sprites[3],xslope,yslope);
		//MoveSprite(&sprites[4],xslope2,yslope2);
		WaitForVsync();
		CopyOAM();
		//updateVars();
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
   //Gltch: Jumps after slope if a press malfuction :: Fixed with slope correction
   if (hit == 1 && !(REG_KEYS & KEY_A)){ gspeed = 12; yball -= 2;}else if (hit == 0 && gspeed > 0){yball -= gspeed/2; gspeed -= 0.01;}
 
   if(!(REG_KEYS & KEY_LEFT)) xball -= speed;
   if(!(REG_KEYS & KEY_RIGHT)) xball += speed;
   if(yball >160) yball = 0;
   if(xball <= 0) xball = 240;  
}

void HitCheck() {
//X hitchecks
//xball == xbar-8 && yball > ybar-8 && yball < ybar+8 || 
   if (xball == xbar2-8 && yball > ybar2-8 && yball < ybar2+8) {
      xball -= speed;
      hit = 1;
   }
   else if (xball == xbar+32 && yball > ybar-8 && yball < ybar+8 || xball == xbar2+32 && yball > ybar2-8 && yball < ybar2+8) {
      xball += speed;
      hit = 1;
   }

//Y outer hitchecks
   else if (yball == ybar-8 && xball > xbar-8 && xball < xbar+32 || yball == ybar2-8 && xball > xbar2-8 && xball < xbar2+32) {
      yball -= ugspeed;
      hit = 1;
   }
   else if (yball == ybar+8 && xball > xbar-8 && xball < xbar+32 || yball == ybar2+8 && xball > xbar2-8 && xball < xbar2+32) {
      yball += ugspeed;
      hit = 1;
   }
   else 
      {hit = 0;}
//Slope 
   //CHANGE YPOSITION TO YBALL

if (xball < xslope+6 && xball > xslope-6 && yball < yslope + 0 && yball > yslope - 8 ) {
 if (yball > yslope + (xslope - xball) - 7)
  {
 if ((xslope - xball) - 7 == -9) 
  {
     yball = yslope + (xslope - xball) - 6;
  }
 else 
  {
     yball = yslope + (xslope - xball) - 7;
  }
hit = 1;
  }}else if (xball < xslope2+6 && xball > xslope2-6 && yball < yslope2 + 0 && yball > yslope2 - 8 ) {
 if (yball > yslope2 + (xslope2 - xball) - 7)
  {
 if ((xslope2 - xball) - 7 == -9) 
  {
    yball = yslope2 + (xslope2 - xball) - 6;
  }
 else 
  {
     yball = yslope2 + (xslope2 - xball) - 7;
  }
hit = 1;
}
}
}



void gravity(void) {

   yball += ugspeed;
}

void updateVars(void) {
 xbar = xposition + 28;
 ybar = yposition + 36;      

 xbar2 = xposition - 10;
 ybar2 = yposition + 50;     

 xslope = xposition + 14;
 yslope = yposition + 42;    

 xslope2 = xposition + 20;
 yslope2 = yposition +36; 
}



