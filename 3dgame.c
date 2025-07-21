// from starter code released on github by 3dsage - and then modified by 
// Adrian Pilkington 2025 (using same MIT license)

// this is my attempts to create a 3d game engine using 3d sages starter code
// and the video https://www.youtube.com/watch?v=huMO4VQEwPc&t=56s
// please support 3dSage and his youtube channel as it is an excellent collention
// of videos and work!!!

// so far I've been able to follow the first tutorial video and update the code 
// I didn't just copy it but followed the tutorial and wrote it. some bits I 
// don't fully understand yet

//------------------------------------------------------------------------------
//--------------------------Code By: 3DSage-------------------------------------
//----------------Video tutorial on YouTube-3DSage------------------------------
//------------------------------------------------------------------------------
//https://github.com/3DSage/OpenGL-Starter_v1/blob/main/LICENSE.md
//MIT License
// Copyright (c) 2022 3DSage

// Permission is hereby granted, free of charge, to any person obtaining a copy 
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights 
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell 
// copies of the Software, and to permit persons to whom the Software is 
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in 
// all copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL 
// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, 
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE 
// SOFTWARE.

#include <math.h>
#include <stdio.h>
#include <GL/glut.h> 

#define res        1                        //0=160x120 1=360x240 4=640x480
#define SW         160*res                  //screen width
#define SH         120*res                  //screen height
#define SW2        (SW/2)                   //half of screen width
#define SH2        (SH/2)                   //half of screen height
#define pixelScale 4/res                    //OpenGL pixel scale
#define GLSW       (SW*pixelScale)          //OpenGL window width
#define GLSH       (SH*pixelScale)          //OpenGL window height

#define DEGREES_IN_360 360

//textures
#include "textures/T_NUMBERS.h"
#include "textures/T_VIEW2D.h"
#include "textures/T_00.h"
#include "textures/T_01.h"
#include "textures/T_02.h"
#include "textures/T_03.h"
#include "textures/T_04.h"
#include "textures/T_05.h"
#include "textures/T_06.h"
#include "textures/T_07.h"
#include "textures/T_08.h"
#include "textures/T_09.h"
#include "textures/T_10.h"
#include "textures/T_11.h"
#include "textures/T_12.h"
#include "textures/T_13.h"
#include "textures/T_14.h"
#include "textures/T_15.h"
#include "textures/T_16.h"
#include "textures/T_17.h"
#include "textures/T_18.h"
#include "textures/T_19.h"
int numText=19;                          //number of textures
int numSect= 0;                          //number of sectors
int numWall= 0;                          //number of walls

//------------------------------------------------------------------------------
typedef struct 
{
 int fr1,fr2;           //frame 1 frame 2, to create constant frame rate
}time; time T;

typedef struct 
{
 int w,s,a,d;           //move up, down, left, right
 int sl,sr;             //strafe left, right 
 int m;                 //move up, down, look up, down
}keys; keys K;

typedef struct mathPrecalcd
{
	float sine[DEGREES_IN_360];
	float cosine[DEGREES_IN_360];
} mathPrecalcd; 

mathPrecalcd M;

typedef struct playerAttributes_s
{
	int x,y,z; // coordinates
	int angle;   // angle
	int look;	 // up down 	
} playerAttributes_t; playerAttributes_t P;

// wall structure and storage

typedef struct walls_s
{
	int x1,y1; //bottom line point 1
	int x2,y2; // bottom line point 2
	int colour;
	int wt,u,v; /// wall texture		
	int shade;
} walls_t;
walls_t W[256];

typedef struct sector_s
{
	int ws,we; // wall number atsrt (ws) end (we)
	int z1,z2; // height of bottom and top
	//int x,y; // centre position for sector
	int d; // add y distance too sort draw ordering		
	int c1,c2;  // bottom and top colour
	int st,ss;   // surface texture ans scale
	int surf[SW]; // hols points for surfaces
	int surface;
} sector_t;
sector_t S[128];

typedef struct 
{
 int w,h;                             //texture width/height
 const unsigned char *name;           //texture name
}TexureMaps; TexureMaps Textures[64]; //increase for more textures

int distance(int x1, int y1, int x2, int y2)
{
	return (int)sqrt((x2-x1)*(x2-x1) + (y2-y1)*(y2-y1));
}



void load()
{
	FILE *fp = fopen("level.h","r");
	if(fp == NULL){ printf("Error opening level.h"); return;}
	int s,w;
	
	fscanf(fp,"%i",&numSect);   //number of sectors 
	for(s=0;s<numSect;s++)      //load all sectors
	{
		fscanf(fp,"%i",&S[s].ws);  
		fscanf(fp,"%i",&S[s].we); 
		fscanf(fp,"%i",&S[s].z1);  
		fscanf(fp,"%i",&S[s].z2); 
		fscanf(fp,"%i",&S[s].st); 
		fscanf(fp,"%i",&S[s].ss);  
	}
	fscanf(fp,"%i",&numWall);   //number of walls 
	for(s=0;s<numWall;s++)      //load all walls
	{
		fscanf(fp,"%i",&W[s].x1);  
		fscanf(fp,"%i",&W[s].y1); 
		fscanf(fp,"%i",&W[s].x2);  
		fscanf(fp,"%i",&W[s].y2); 
		fscanf(fp,"%i",&W[s].wt);
		fscanf(fp,"%i",&W[s].u); 
		fscanf(fp,"%i",&W[s].v);  
		fscanf(fp,"%i",&W[s].shade);  
	}
	fscanf(fp,"%i %i %i %i %i",&P.x,&P.y,&P.z, &P.angle,&P.look); //player position, angle, look direction 
	fclose(fp); 
}
	
//------------------------------------------------------------------------------

void pixel(int x,int y, int c)                  //draw a pixel at x/y with rgb
{int rgb[3];
 if(c==0){ rgb[0]=255; rgb[1]=255; rgb[2]=  0;} //Yellow	
 if(c==1){ rgb[0]=160; rgb[1]=160; rgb[2]=  0;} //Yellow darker	
 if(c==2){ rgb[0]=  0; rgb[1]=255; rgb[2]=  0;} //Green	
 if(c==3){ rgb[0]=  0; rgb[1]=160; rgb[2]=  0;} //Green darker	
 if(c==4){ rgb[0]=  0; rgb[1]=255; rgb[2]=255;} //Cyan	
 if(c==5){ rgb[0]=  0; rgb[1]=160; rgb[2]=160;} //Cyan darker
 if(c==6){ rgb[0]=160; rgb[1]=100; rgb[2]=  0;} //brown	
 if(c==7){ rgb[0]=110; rgb[1]= 50; rgb[2]=  0;} //brown darker
 if(c==8){ rgb[0]=  0; rgb[1]= 60; rgb[2]=130;} //background 
 glColor3ub(rgb[0],rgb[1],rgb[2]); 
 glBegin(GL_POINTS);
 glVertex2i(x*pixelScale+2,y*pixelScale+2);
 glEnd();
}

void movePlayer()
{
 //move up, down, left, right
 if(K.a ==1 && K.m==0){ P.angle -= 4; if(P.angle<0) { P.angle += 360; }}  
 if(K.d ==1 && K.m==0){ P.angle += 4; if(P.angle>359) { P.angle -= 360; }}  
 int dx=M.sine[P.angle] * 10.0;
 int dy=M.cosine[P.angle] * 10.0;
 if(K.w ==1 && K.m==0){ P.x+= dx; P.y+= dy; }
 if(K.s ==1 && K.m==0){ P.x-= dx; P.y-= dy; }
 //strafe left, right
 if(K.sr==1){ P.x+= dy; P.y -= dx;}
 if(K.sl==1){ P.x-= dy; P.y += dx;}
 //move up, down, look up, look down
 if(K.a==1 && K.m==1){ P.look -= 1; }
 if(K.d==1 && K.m==1){ P.look += 1; }
 if(K.w==1 && K.m==1){ P.z -= 4;}
 if(K.s==1 && K.m==1){ P.z += 4;}
}

void clearBackground() 
{int x,y;
 for(y=0;y<SH;y++)
 { 
  for(x=0;x<SW;x++){ pixel(x,y,8);} //clear background color
 }	
}

void clipBehindPlayer(int *x1, int *y1, int * z1,
					int x2, int y2, int z2)
{
	float da = *y1;
	float db = y2;
	float d=da-db; if (d==0) {d = 1;}
	float s = da / (da-db);
	*x1 = *x1 + s*(x2-(*x1));
	*y1 = *y1 + s*(y2-(*y1)); if (*y1==0){*y1 = 1;}
	*z1 = *z1 + s*(z2-(*z1));
}

void drawWall(int x1, int x2, int b1, int b2, int t1, int t2,int s, int w, int frontBack)
{
	int x,y;
	int dyb = b2 - b1;
	int dyt = t2 - t1;
	int dx = x2 - x1;
	if (dx == 0) 
	{
		dx = 1;
	}
	int xs = x1;
	// clip x and y so we don't draw behind or under
	if (x1 < 0) {x1 = 0;}
	if (x2 < 0) {x2 = 0;}
	if (x1 > SW) {x1=SW;}
	if (x2 > SW) {x2=SW;}
	
	
	for (x = x1; x < x2; x++)
	{
		int y1 = dyb * (x-xs+0.5) / dx+b1; // y bottom point
		int y2 = dyt * (x-xs+0.5) / dx+t1; // y top point

		if (y1 < 0) {y1 = 0;}
		if (y2 < 0) {y2 = 0;}
		if (y1 > SH) {y1=SH;}
		if (y2 > SH) {y2 =SH;}
		
		// draw front facing walls
		if (frontBack == 0)
		{
			if (S[s].surface == 1) {S[s].surf[x]=y1;} // bottom surface save top row
			if (S[s].surface == 2) {S[s].surf[x]=y2;} // top surface save row				
			for (y = y1; y < y2; y++) {	pixel(x,y,0);}			
		}
		// draw back walls
		if (frontBack == 1)
		{
			if (S[s].surface == 1) {y2=S[s].surf[x];} // bottom surface top row			
			if (S[s].surface == 2) {y1 =S[s].surf[x];} // bottom surface top row
			for (y = y1; y < y2; y++){pixel(x,y,2);}			
		}				
	}
}


void draw3D()
{	
	int x, s, w;  // loop counts for walls and sectors	
	int wall_x[4];
	int wall_y[4];
	int wall_z[4];
	float COS=M.cosine[P.angle];
	float SIN=M.sine[P.angle];
	int cycles;
	
	// sort the walls in order for drawing using bubble sort
	for (s = 0; s < numSect-1; s++)
	{
		for (w = 0; w < numSect-s-1;w++) 
		{
			if (S[w].d < S[w+1].d)
			{
				sector_t swapTemp=S[w];
				S[w]=S[w+1];
				S[w+1] = swapTemp;
			}
		}
	}
	// create the points for wall based on players x and the angle
	// its all based onthe "map"/wolrd moving not the player hence the 
	
	
	// draw sectors
	for (s = 0; s < numSect; s++)
	{		
		S[s].d = 0;		
		if (P.z < S[s].z1) 			
		{
			S[s].surface = 1;   // bottom surface should be drawn
			cycles = 2;
			for (x=0;x<SW;x++) 
			{
				S[s].surf[x]=SH;
			}
		}
		else if (P.z > S[s].z2) 
		{
			S[s].surface = 2;	// top surface is visible
			cycles = 2;
			for (x=0;x<SW;x++) 
			{
				S[s].surf[x]=0;
			}			
		}
		else
		{
			//neither surface visible
			S[s].surface = 0;
			cycles = 1;
		}
		
		int frontBackLoop = 0;
		for (frontBackLoop = 0; frontBackLoop < cycles; frontBackLoop++)
		{
		
			for (w=S[s].ws;w <S[s].we;w++)
			{
			
				int x1 = W[w].x1 - P.x;	int y1 = W[w].y1 - P.y;
				int x2 = W[w].x2 - P.x;	int y2 = W[w].y2 - P.y;
				if (frontBackLoop == 1)
				{				
					int swapTemp = x1; x1=x2; x2=swapTemp; 
					swapTemp=y1;y1=y2;y2=swapTemp;
				}
				// world x position
				wall_x[0] = x1 * COS - y1 * SIN;
				wall_x[1] = x2 * COS - y2 * SIN;
				wall_x[2] = wall_x[0];
				wall_x[3] = wall_x[1];
				// world y position
				wall_y[0] = y1 * COS + x1 * SIN;
				wall_y[1] = y2 * COS + x2 * SIN;
				wall_y[2] = wall_y[0];
				wall_y[3] = wall_y[1];	
				
				S[s].d+=distance(0,0,(wall_x[0]+wall_x[1])/2, (wall_y[0]+wall_y[1])/2);
				
				// world z height
				wall_z[0] = S[s].z1 - P.z + ((P.look* wall_y[0])/32.0);
				wall_z[1] = S[s].z1 - P.z + ((P.look* wall_y[1])/32.0);
				wall_z[2] = S[s].z2 - P.z + ((P.look* wall_y[0])/32.0);
				wall_z[3] = S[s].z2 - P.z + ((P.look* wall_y[1])/32.0);
				
				if (wall_z[0] < 1 && wall_y[1] < 1) 
				{
					//wall behind, don't draw!!
					continue; // don't return due to drawing all the other sectos ands walls - but move on to next
				}	
				if (wall_y[0] < 1)
				{
					clipBehindPlayer(&wall_x[0],&wall_y[0],&wall_z[0],wall_x[1],wall_y[1],wall_z[1]); // bottom line
					clipBehindPlayer(&wall_x[2],&wall_y[2],&wall_z[2],wall_x[3],wall_y[3],wall_z[3]); // top line
				}
			
				if (wall_y[1] < 1)
				{
					clipBehindPlayer(&wall_x[1],&wall_y[1],&wall_z[1],wall_x[0],wall_y[0],wall_z[0]); // bottom line
					clipBehindPlayer(&wall_x[3],&wall_y[3],&wall_z[3],wall_x[2],wall_y[2],wall_z[2]); // top line
				}	
				// screen x and y position
				// include offset from origin to center of screen so perspective is fixed 
				// there not bottom left
				wall_x[0] = wall_x[0] * 200 / wall_y[0] + SW2;	wall_y[0] = wall_z[0] * 200 / wall_y[0] + SH2;
				wall_x[1] = wall_x[1] * 200 / wall_y[1] + SW2;	wall_y[1] = wall_z[1] * 200 / wall_y[1] + SH2;	
				wall_x[2] = wall_x[2] * 200 / wall_y[2] + SW2;	wall_y[2] = wall_z[2] * 200 / wall_y[2] + SH2;
				wall_x[3] = wall_x[3] * 200 / wall_y[3] + SW2;	wall_y[3] = wall_z[3] * 200 / wall_y[3] + SH2;	
					
				//if (wall_x[0] > 0 && wall_x[0] < SW && wall_y[0]> 0 && wall_y[0] < SH) { pixel(wall_x[0], wall_y[0],0); }
				//if (wall_x[1] > 0 && wall_x[1] < SW && wall_y[1]> 0 && wall_y[1] < SH) { pixel(wall_x[1], wall_y[1],0);	}
				drawWall(wall_x[0], wall_x[1], wall_y[0], 
				         wall_y[1], wall_y[2],wall_y[3],
						 s, w, frontBackLoop);
			}
			S[s].d /= (S[s].we-S[s].ws);			
		}
	}
}

void display() 
{int x,y;
 if(T.fr1-T.fr2>=50)                        //only draw 20 frames/second
 { 
  clearBackground();
  movePlayer();
  draw3D(); 

  T.fr2=T.fr1;   
  glutSwapBuffers(); 
  glutReshapeWindow(GLSW,GLSH);             //prevent window scaling
 }

 T.fr1=glutGet(GLUT_ELAPSED_TIME);          //1000 Milliseconds per second
 glutPostRedisplay();
} 

void KeysDown(unsigned char key,int x,int y)   
{ 
 if(key=='w'){ K.w =1;} 
 if(key=='s'){ K.s =1;} 
 if(key=='a'){ K.a =1;} 
 if(key=='d'){ K.d =1;} 
 if(key=='m'){ K.m =1;} 
 if(key==','){ K.sr=1;} 
 if(key=='.'){ K.sl=1;} 
 if(key==13){ load();} 
}
void KeysUp(unsigned char key,int x,int y)
{ 
 if(key=='w'){ K.w =0;}
 if(key=='s'){ K.s =0;}
 if(key=='a'){ K.a =0;}
 if(key=='d'){ K.d =0;}
 if(key=='m'){ K.m =0;}
 if(key==','){ K.sr=0;} 
 if(key=='.'){ K.sl=0;} 
}

void init()
{       
	// pre-calculate the sine and cosine and store in the sineCos struct
	int degreeIndex;
	for (degreeIndex = 0; degreeIndex < 360; degreeIndex++)
	{
		M.sine[degreeIndex]	= sin(degreeIndex / 180.0f * M_PI);
		M.cosine[degreeIndex] = cos(degreeIndex / 180.0f * M_PI);
	}
	load();
}

int main(int argc, char* argv[])
{
 glutInit(&argc, argv);
 glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
 glutInitWindowPosition(GLSW/2,GLSH/2);
 glutInitWindowSize(GLSW,GLSH);
 glutCreateWindow(""); 
 glPointSize(pixelScale);                        //pixel size
 gluOrtho2D(0,GLSW,0,GLSH);                      //origin bottom left
 init();
 glutDisplayFunc(display);
 glutKeyboardFunc(KeysDown);
 glutKeyboardUpFunc(KeysUp);
 glutMainLoop();
 return 0;
} 

