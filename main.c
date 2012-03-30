#ifdef __APPLE__
	#include <OpenGL/gl.h>
	#include <OpenGL/glu.h>
	#include <GLUT/glut.h>
#else
	#include <GL/glut.h>
	#include <GL/gl.h>
	#include <GL/glu.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define ESCAPE 27
#define UP    119
#define DOWN  115
#define LEFT  97
#define RIGHT 100

int window; 
float xrot, yrot, zrot;

GLfloat LightAmbient[]=  { 0.2f, 0.2f, 0.2f, 1.0f };
GLfloat LightDiffuse[]=  { 0.5f, 0.5f, 0.5f, 1.0f };
GLfloat LightPosition[]= { 2.0f, 2.0f, 5.0f, 1.0f };
GLfloat LightColor[]= { 0.5f, 0.5f, 0.5f };

GLfloat distance;
GLfloat pitch;

typedef struct planet {
	char name[50];
	long diameter_km;
	double x_au, y_au;
	float color[3];
	float speed; // KM/Sec
	float avg_dist; // AU
} planet;

planet planets[] = {
	{
		.name = "Sun",
		.diameter_km = 1377648,
		.x_au = 0.0,
		.y_au = 0.0,
		.color = { 0.8f, 0.8f, 0.0f },
		.speed = 0.0f,
		.avg_dist = 0.0f,
	},
	{
		.name = "Mercury",
		.diameter_km = 4880,
		.x_au = -0.387096774,
		.y_au = 0.096774194,
		.color = { 0.6f, 0.6f, 0.6f },
		.speed = 47.8725f,
		.avg_dist = 0.387f,
	},
	{
		.name = "Venus",
		.diameter_km = 12104,
		.x_au = -0.774193548,
		.y_au = -0.096774194,
		.color = { 0.5f, 0.1f, 0.1f },
		.speed = 35.0214f,
		.avg_dist = 0.722f,
	},
	{
		.name = "Earth",
		.diameter_km = 12756,
		.x_au = -0.387096774,
		.y_au = -0.935483871,
		.color = { 0.0f, 0.5f, 0.8f },
		.speed = 29.7859f,
		.avg_dist = 1.0f,
	},
	{
		.name = "Mars",
		.diameter_km = 6788,
		.x_au = -0.580645161,
		.y_au = -1.580645161,
		.color = { 0.4f, 0.0f, 0.0f },
		.speed = 24.1309f,
		.avg_dist = 1.52f,
	},
	{
		.name = "Vesta",
		.diameter_km = 530,
		.x_au = -0.483870968,
		.y_au = 2.419354839,
		.color = { 0.6f, 0.6f, 0.6f },
		.speed = 0.0f,
		.avg_dist = 0.0f,
	},
	{
		.name = "Jupiter",
		.diameter_km = 142740,
		.x_au = -3.677419355,
		.y_au = 3.516129032,
		.color = { 0.5f, 0.5f, 0.5f },
		.speed = 13.0697f,
		.avg_dist = 5.20f,
	},
	{
		.name = "Saturn",
		.diameter_km = 120034,
		.x_au = 4.0,
		.y_au = -8.838709677,
		.color = { 0.4f, 0.4f, 0.0f },
		.speed = 9.6724f,
		.avg_dist = 9.58f,
	},
};

void InitGL(int Width, int Height)
{
    glClearColor(0.13f, 0.13f, 0.13f, 0.0f);
    glClearDepth(1.0);
    glDepthFunc(GL_LESS);
    glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
    glShadeModel(GL_SMOOTH);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE);
	glLightfv(GL_LIGHT1, GL_AMBIENT, LightAmbient);
	glLightfv(GL_LIGHT1, GL_POSITION,LightPosition);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, LightDiffuse);
	glLightfv(GL_LIGHT1, GL_SPECULAR, LightColor);
	glEnable(GL_LIGHT1);
	glEnable(GL_LIGHTING);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0f,(GLfloat)Width/(GLfloat)Height,0.1f,100.0f);
    glMatrixMode(GL_MODELVIEW);
}

void ReSizeGLScene(int Width, int Height)
{
    if (Height==0)
	Height=1;
    glViewport(0, 0, Width, Height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    gluPerspective(45.0f,(GLfloat)Width/(GLfloat)Height,0.1f,1000.0f);
    glMatrixMode(GL_MODELVIEW);
}

void DrawPlanet(planet *planet) {
	float speed;
	float draw_size;

	speed = (float)planet->speed*yrot;
	if (planet->avg_dist > 0.0f) {
		speed /= planet->avg_dist;
	}
	draw_size = (double)planet->diameter_km / 149597870.0;

	float draw_x = planet->x_au;
	float draw_y = planet->y_au;

	draw_size *= 10000.0f;
	if (planet == &planets[0]) {
		draw_size /= 10.0f;
	}

	glMaterialfv(GL_FRONT, GL_AMBIENT, planet->color);
	glMaterialfv(GL_FRONT, GL_SPECULAR, planet->color);

	glRotatef(speed, 0.0f, 0.0f, 0.001f);
	//glTranslatef(draw_x, draw_y, 0.0f);
	glutSolidSphere(draw_size, 80, 80);
	//glTranslatef(-draw_x, -draw_y, 0.0f);
	glRotatef(speed, 0.0f, 0.0f, -0.001f);

}

void DrawGLScene()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glLoadIdentity();

	// View
	glTranslatef(0.0f,0.0f,-5.0f+distance);
	glRotatef(-65.0f+pitch, 1.0f, 0.0f, 0.0f);

	// Scene
	DrawPlanet(&planets[3]);

    yrot+=0.001f; // TODO: Computer speed dependent

    glutSwapBuffers();
}

void keyPressed(unsigned char key, int x, int y) 
{
    usleep(100);

    if (key == ESCAPE) { 
		glutDestroyWindow(window); 
		exit(0);                   
    }
    if (key == UP) { 
		distance -= 0.1f;
	}
    if (key == DOWN) { 
		distance += 0.1f;
	}
    if (key == LEFT) { 
		pitch -= 1.0f;
	}
    if (key == RIGHT) { 
		pitch += 1.0f;
	}
}

int main(int argc, char **argv) 
{  
    glutInit(&argc, argv);  
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_ALPHA | GLUT_DEPTH);  
    glutInitWindowSize(1920, 1200);  
    glutInitWindowPosition(0, 0);  
    window = glutCreateWindow("Planets");  
    glutDisplayFunc(&DrawGLScene);  
    glutFullScreen();
    glutIdleFunc(&DrawGLScene);
    glutReshapeFunc(&ReSizeGLScene);
    glutKeyboardFunc(&keyPressed);
    InitGL(1920, 1200);
    glutMainLoop();  
    return 1;
}

