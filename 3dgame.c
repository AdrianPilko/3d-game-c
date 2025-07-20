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

typedef struct playerAttributes
{
	int x,y,z; // coordinates
	int angle;   // angle
	int look;	 // up down 	
} playerAttributes; playerAttributes P;
	
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

void drawWall(int x1, int x2, int b1, int b2, int t1, int t2)
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
	
	for (x = x1; x < x2; x++)
	{
		int y1 = dyb * (x-xs+0.5) / dx+b1; // y bottom point
		int y2 = dyt * (x-xs+0.5) / dx+t1; // y top point
		for (y = y1; y < y2; y++)
		{
			pixel(x,y,0);
		}
	}
}


void draw3D()
{
	int wall_x[4];
	int wall_y[4];
	int wall_z[4];
	float COS=M.cosine[P.angle];
	float SIN=M.sine[P.angle];
	// create the points for wall based on players x and the angle
	// its all based onthe "map"/wolrd moving not the player hence the 
	// 40 - P.x etc
	int x1 = 40 - P.x;	
	int y1 = 10 - P.y;
	int x2 = 40 - P.x;	
	int y2 = 290 - P.y;	
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
	// world z height
	wall_z[0] = 0 - P.z + ((P.look* wall_y[0])/32.0);
	wall_z[1] = 0 - P.z + ((P.look* wall_y[1])/32.0);
	wall_z[2] = wall_z[0] + 40;
	wall_z[3] = wall_z[1] + 40;
	// screen x and y position
	// include offset from origin to center of screen so perspective is fixed 
	// there not bottom left
	wall_x[0] = wall_x[0] * 200 / wall_y[0] + SW2;	wall_y[0] = wall_z[0] * 200 / wall_y[0] + SH2;
	wall_x[1] = wall_x[1] * 200 / wall_y[1] + SW2;	wall_y[1] = wall_z[1] * 200 / wall_y[1] + SH2;	
	wall_x[2] = wall_x[2] * 200 / wall_y[2] + SW2;	wall_y[2] = wall_z[2] * 200 / wall_y[2] + SH2;
	wall_x[3] = wall_x[3] * 200 / wall_y[3] + SW2;	wall_y[3] = wall_z[3] * 200 / wall_y[3] + SH2;	
		
	//if (wall_x[0] > 0 && wall_x[0] < SW && wall_y[0]> 0 && wall_y[0] < SH) { pixel(wall_x[0], wall_y[0],0); }
	//if (wall_x[1] > 0 && wall_x[1] < SW && wall_y[1]> 0 && wall_y[1] < SH) { pixel(wall_x[1], wall_y[1],0);	}
	drawWall(wall_x[0], wall_x[1], wall_y[0], wall_y[1], wall_y[2],wall_y[3]);
		
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
	P.x =60;
	P.y =-120;
	P.z = 30;
	P.angle = 0;
	P.look = 0;
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

