// from starter code released on github by 3dsage - and them modified by 
// Adrian Pilkington 2025 (using same MIT license)
// this is myu attempts to create a 3d game engine using 3d sages starter code
// and the video https://www.youtube.com/watch?v=huMO4VQEwPc&t=56s
// please support 3d sage and his youtube channel as it is an excellent collention
// of videos and work!!!

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

const int numSect = 4;
const int numWalls = 16;

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
} walls_t;
walls_t W[30];

typedef struct sector_s
{
	int ws,we; // wall number atsrt (ws) end (we)
	int z1,z2; // height of bottom and top
	//int x,y; // centre position for sector
	int d; // add y distance too sort draw ordering		
	int c1,c2;  // bottom and top colour
	int surf[SW]; // hols points for surfaces
	int surface;
} sector_t;
sector_t S[30];

// hard code these until we have a level / game editor
int sectorConfig[] = 
// wall start, wall end, z1 height, z2 height, bottom colour, top colour
{
	0,	4, 	0,	40,	2,	3,
	4,	8, 	0,	40,	4,	5,
	8,	12,	0,	40,	6,	7,
	12,	16,	0,	40,	0,	1
};
int wallConfig[] = 
// x1, y1,x2,y2,colour
{
 	0,0,32,0,0,
 	32,0,32,32,1,
 	32,32,0,32,0,
 	0,32,0,0,1,
 	
 	64,0,96,0,2,
 	96,0,96,32,3,
 	96,32,64,32,2,
 	64,32,64,0,3,
 	
 	64,64,96,64,4,
 	96,64,96,96,5,
 	96,96,64,96,4,
 	64,96,64,64,5,
 	
 	0,64,32,64,6,
 	32,64,32,96,7,
 	32,96,0,96,6,
 	0,96,0,64,7,
};

int distance(int x1, int y1, int x2, int y2)
{
	return (int)sqrt((x2-x1)*(x2-x1) + (y2-y1)*(y2-y1));
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

void drawWall(int x1, int x2, int b1, int b2, int t1, int t2,int colour,int s)
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
	if (x1 < 1) {x1 = 1;}
	if (x2 < 1) {x2 = 1;}
	if (x1 > SW-1) {x1=SW-1;}
	if (x2 > SW-1) {x2=SW-1;}
	
	
	for (x = x1; x < x2; x++)
	{
		int y1 = dyb * (x-xs+0.5) / dx+b1; // y bottom point
		int y2 = dyt * (x-xs+0.5) / dx+t1; // y top point

		if (y1 < 1) {y1 = 1;}
		if (y2 < 1) {y2 = 1;}
		if (y1 > SH-1) {y1=SH-1;}
		if (y2 > SH-1) {y2 =SH-1;}
		// check surfaces
		if (S[s].surface==1) {S[s].surf[x]=y1;continue;} // save bottom points
		if (S[s].surface==2) {S[s].surf[x]=y2;continue;} // save top points
		if (S[s].surface==-1)		
		{
			for (y=S[s].surf[x];y <y1;y++) 
			{
				pixel(x,y,S[s].c1); // draw bottom
			}			
		}
		if (S[s].surface==-2)
		{
			for (y=y2; y < S[s].surf[x];y++) 
			{
				pixel(x,y,S[s].c2); // draw top
			}					
		}		
		for (y = y1; y < y2; y++)
		{
			pixel(x,y,colour);
		}
	}
}


void draw3D()
{	
	int s, w;  // loop counts for walls and sectors	
	int wall_x[4];
	int wall_y[4];
	int wall_z[4];
	float COS=M.cosine[P.angle];
	float SIN=M.sine[P.angle];
	
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
		}
		else if (P.z > S[s].z2) 
		{
			S[s].surface = 2;	// top surface is visible
		}
		else
		{
			//neither surface visible
			S[s].surface = 0;
		}
		
		int frontBackLoop = 0;
		for (frontBackLoop = 0; frontBackLoop < 2; frontBackLoop++)
		{
		
			for (w=S[s].ws;w <S[s].we;w++)
			{
			
				int x1 = W[w].x1 - P.x;	int y1 = W[w].y1 - P.y;
				int x2 = W[w].x2 - P.x;	int y2 = W[w].y2 - P.y;
				if (frontBackLoop == 0)
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
				wall_z[2] = wall_z[0] + S[s].z2;
				wall_z[3] = wall_z[1] + S[s].z2;
				
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
						 W[w].colour,s);
			}
			S[s].d /= (S[s].we-S[s].ws);
			S[s].surface *= -1;
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
 if(key=='w'==1){ K.w =1;} 
 if(key=='s'==1){ K.s =1;} 
 if(key=='a'==1){ K.a =1;} 
 if(key=='d'==1){ K.d =1;} 
 if(key=='m'==1){ K.m =1;} 
 if(key==','==1){ K.sr=1;} 
 if(key=='.'==1){ K.sl=1;} 
}
void KeysUp(unsigned char key,int x,int y)
{ 
 if(key=='w'==1){ K.w =0;}
 if(key=='s'==1){ K.s =0;}
 if(key=='a'==1){ K.a =0;}
 if(key=='d'==1){ K.d =0;}
 if(key=='m'==1){ K.m =0;}
 if(key==','==1){ K.sr=0;} 
 if(key=='.'==1){ K.sl=0;}
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
	// initialise the player attributes
	P.x =70;
	P.y =-110;
	P.z = 20;
	P.angle = 0;
	P.look = 0;
	int s,w,v1=0,v2=0;
	for (s=0; s < numSect;s++)
	{
		S[s].ws=sectorConfig[v1+0];
		S[s].we=sectorConfig[v1+1];
		S[s].z1=sectorConfig[v1+2];
		S[s].z2=sectorConfig[v1+3] - sectorConfig[v1+2];
		S[s].c1 = sectorConfig[v1+4];
		S[s].c2 = sectorConfig[v1+5];	
		v1+=6;
		for (w =S[s].ws; w < S[s].we;w++)
		{
			W[w].x1 =wallConfig[v2+0];
			W[w].y1 =wallConfig[v2+1];
			W[w].x2 =wallConfig[v2+2];
			W[w].y2 =wallConfig[v2+3];
			W[w].colour =wallConfig[v2+4];
			v2+=5;
		}		
	}
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

