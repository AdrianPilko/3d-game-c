#include <GL/glut.h>

// simple ocde to draw a rectangle in 2d using opengl

/// this code based on the video by 3DSage and is a test of the open gl install and dev-cpp environment
/// https://www.youtube.com/watch?v=RTk6W67dyR4
/// https://www.youtube.com/@3DSage

void display()
{
	glColor3f(1,0,0);
	glBegin(GL_POLYGON);
	glVertex2f(100,300);
	glVertex2f(100,100);
	glVertex2f(200,100);
	glVertex2f(200,300);
	glEnd();
	
	glFlush();
	glutSwapBuffers();
}

int main(int argc, char * argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowSize(480,640);
	glutCreateWindow("");
	glutDisplayFunc(display);
	gluOrtho2D(0,640,0,640);
	glClearColor(0.5,0.7,0.5,0);
	glutMainLoop();
	return 0;
}
