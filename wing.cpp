#define _USE_MATH_DEFINES
#include <iostream>
#include <fstream>
#include <math.h>
#include <ctime>
#include <time.h>
#include <stdio.h>      /* printf, scanf, puts, NULL */
#include <stdlib.h>     /* srand, rand */
#include <time.h> 
#include <string>
#include <GL/glew.h>
#include <GL/glut.h>

#include <helper_math.h> //float3, float4

#include "Types.h"
#include "ParticleSystem.h"
#include "HelperFunctions.h"

const unsigned int window_width = 768;
const unsigned int window_height = 768;

const unsigned int mesh_width = 40;
const unsigned int mesh_height = 40;
const unsigned int size = mesh_height * mesh_width;


bool ppause = false;
bool showWing = false;
bool showBox = true;
bool showPhontain = true;
bool showForce = true;
bool showGravityForce = false;
bool showCoordinates = false;
bool saveResults = false;

float box = 5.f;
const float3 f = make_float3(10.f, 0.f, 0.f);
const float dt = 0.001;
unsigned long long frame = 0;
std::ofstream myfile;

const float3 phontain = make_float3(-box / 2.f, 0.f, 0.f);
Wing wing;
ParticleSystemPhountain<Wing>* pSystem = new ParticleSystemPhountain<Wing>(size, box, wing, phontain);

int	nFPS = 0, nFrames = 0;
unsigned int dwLastFPS = 0;

void fps()
{
    if (clock() - dwLastFPS >= 1000) // after 1 s
    {
        dwLastFPS = clock();
        nFPS = nFrames;
        nFrames = 0;
        char szTitle[256] = { 0 };
        sprintf(szTitle, "Symulacja skrzydla: %i FPS, liczba czastek: %i", nFPS, size);
        glutSetWindowTitle(szTitle);
    }
    nFrames++;
}

void keyboard(unsigned char key, int /*x*/, int /*y*/)
{
    switch (key) {
    case(27) :
        exit(0);
        break;
    case('b') :
        showBox = !showBox;
        break;
    case('c') :
        showCoordinates = !showCoordinates;
        break; 
    case('f') :
        showForce = !showForce;
        break;
    case('g') :
        showGravityForce = !showGravityForce;
        break;
    case('p') :
        ppause = !ppause;
        break;
    case('w') :
        showWing = !showWing;
        break;
    case('r') :
        pSystem->reset();
        frame = 0;
        break;
    }
}

float calculateEnergy()
{
    float energy = 0;
    for (int i = 0; i < size; ++i) 
    {
        const float3 vel = pSystem->getVel(i);
        energy += ((vel.x * vel.x + vel.y * vel.y + vel.z * vel.z) / 2.f);
    }
    return energy;
}

void move()
{
    for (unsigned i = 0; i < size; i++)
        pSystem->boxCollision(i);

    for (unsigned i = 0; i < size; i++)
    //for (unsigned i = 0; i < size - 1; i++)
        pSystem->particleCollision(i);

    for (unsigned i = 0; i < size; i++)
        pSystem->wingCollision(i);

    for (unsigned i = 0; i < size; i++)
        pSystem->prepareMove(i, f, dt);
    
    pSystem->move();
}

void singleStep()
{
    if (!ppause)
    {
        move();
        
        if (saveResults)
        {
            myfile << frame << " " << pSystem->getEnergy() << " " << wing.getForce() << " " << nFPS << std::endl;
            std::cout << frame << " " << pSystem->getEnergy() << " " << wing.getForce() << " " << nFPS << std::endl;
        }
       
        frame++;
    }
}

void display(void)
{
    fps();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    singleStep();

    if (showBox)
    {
        glColor3f(1.0, 1.0, 1.0);
        glutWireCube(box);
    }
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    myCamera();

    glTranslatef(0.0, 0.0, translate_z);

    glPointSize(1.f);

    glBegin(GL_POINTS);
    for (int i = 0; i < size; i++)
    {
        const float4 color = pSystem->getColor(i);
        glColor4f(color.x, color.y, color.z, color.w);

        const float3 pos = pSystem->getPos(i);
        glVertex3f(pos.x, pos.y, pos.z);
    }
    glEnd();

    if (showPhontain)
    {
        glPointSize(10.f);
        glBegin(GL_POINTS);
        glColor4f(1.f, 1.f, 0.f, 0.f);
        glVertex3f(phontain.x, phontain.y, phontain.z);
        glEnd();
    }

    if (showWing)
    {
        glColor4f(0.9f, 0.9f, 0.9f, 1.0);

        glDisable(GL_BLEND);
        glEnable(GL_LIGHTING);
        glEnable(GL_LIGHT0);
        glEnable(GL_COLOR_MATERIAL);
        glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

        GLUquadric * o = gluNewQuadric();
        gluQuadricNormals(o, GLU_SMOOTH);

        const float3 wingPos = wing.getPos();
        const float wingRadius = wing.getRadius();

        glPushMatrix();
        glTranslatef(wingPos.x, wingPos.y, -box / 2);
        gluCylinder(o, wingRadius, wingRadius, box, 20, 2);	// o, r_top, r_bot, wys, ile katow, ?
        glPopMatrix();
        gluDeleteQuadric(o);

        glBegin(GL_TRIANGLE_FAN);
        for (float kat = 0.0f; kat < (2.0f*3.14); kat += (3.14 / 32.0f))
        {
            float x = wingRadius * sin(kat);
            float y = wingRadius * cos(kat);
            glVertex3f(x + wingPos.x, y + wingPos.y, -box / 2);
        }
        glEnd();

        glBegin(GL_TRIANGLE_FAN);
        for (float kat = 0.0f; kat < (2.0f*3.14); kat += (3.14 / 32.0f))
        {
            float x = wingRadius * sin(kat);
            float y = wingRadius * cos(kat);
            glVertex3f(x + wingPos.x, y + wingPos.y, box / 2);
        }
        glEnd();

        glDisable(GL_LIGHTING);  
    }

    if (showCoordinates)
    {
        glColor3f(1.0, .0, .0);
        glBegin(GL_LINES);
        glVertex3f(.0f, .0f, .0f);  //x
        glVertex3f(box / 2 + 20.0f, .0f, .0f);

        glVertex3f(box / 2 + 20.0f, .0f, .0f);	//strzalka gora
        glVertex3f(box / 2 + 15.0f, 2.0f, .0f);

        glVertex3f(box / 2 + 20.0f, .0f, .0f);	//strzalka dol
        glVertex3f(box / 2 + 15.0f, -2.0f, .0f);

        glVertex3f(.0f, .0f, .0f);  //y
        glVertex3f(.0f, box / 2 + 20.0f, .0f);

        glVertex3f(.0f, box / 2 + 20.0f, .0f);	//strzalka prawo
        glVertex3f(2.0f, box / 2 + 15.0f, .0f);

        glVertex3f(.0f, box / 2 + 20.0f, .0f);	//strzalka lewo
        glVertex3f(-2.0f, box / 2 + 15.0f, .0f);

        glVertex3f(.0f, .0f, .0f);  //z
        glVertex3f(.0f, .0f, box / 2 + 20.0f);

        glVertex3f(.0f, .0f, box / 2 + 20.0f);	//strzalka lewo
        glVertex3f(-2.0f, .0f, box / 2 + 15.0f);

        glVertex3f(.0f, .0f, box / 2 + 20.0f);	//strzalka prawo
        glVertex3f(2.0f, .0f, box / 2 + 15.0f);
        glEnd();
    }

    if (showForce)
    {
        GLUquadric * oo = gluNewQuadric();
        gluQuadricNormals(oo, GLU_SMOOTH);

        glColor3f(1.0, 1.0, .0);

        glPushMatrix();

        glRotatef(270, 1.f, 0.f, 0.f);
        glTranslatef(wing.getPos().x, 0, 0);
        gluCylinder(oo, 0.05f, 0.05f, wing.getForce() * 0.05f, 20, 2);	// o, r_top, r_bot, wys, ile katow, ?

        glPopMatrix();

        gluDeleteQuadric(oo);

        GLUquadric * b = gluNewQuadric();
        gluQuadricNormals(b, GLU_SMOOTH);
    }

    if (showGravityForce)
    {
        GLUquadric * oo = gluNewQuadric();
        gluQuadricNormals(oo, GLU_SMOOTH);

        glColor3f(.0, 1.0, 1.0);

        glPushMatrix();

        glRotatef(90, 1.f, 0.f, 0.f);
        glTranslatef(wing.getPos().x, 0, 0);
        gluCylinder(oo, 0.05f, 0.05f, 9.81f * 0.05f, 20, 2);	// o, r_top, r_bot, wys, ile katow, ?

        glPopMatrix();

        gluDeleteQuadric(oo);

        GLUquadric * b = gluNewQuadric();
        gluQuadricNormals(b, GLU_SMOOTH);
    }

    glutSwapBuffers();
    glutPostRedisplay();
    

    wing.resetForce();
}

void initGL(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
    glutInitWindowSize(window_width, window_height);
    glutCreateWindow("Million particles");
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutMotionFunc(motion);

    glClearColor(0.0, 0.0, 0.0, 1.0);
    glDisable(GL_DEPTH_TEST);

    glViewport(0, 0, window_width, window_height);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(90.0, (GLfloat)window_width / (GLfloat)window_height, 0.1, box + 100.0);
    //glTranslatef(0, 0, - (box * 0.5f));
}

void manual()
{
    std::cout << "ppause = P" << std::endl;
    std::cout << "showWing = w" << std::endl;
    std::cout << "showBox = b" << std::endl;
    std::cout << "showForce = f" << std::endl;
    std::cout << "showGravityForce = g" << std::endl;
    std::cout << "showCoordinates = c" << std::endl;

    std::cout << std::endl << "Press p to start!" << std::endl;
}

int main(int argc, char** argv)
{
    manual();

    pSystem->initializeParticle();
    //pSystem->printSystem("initialize");

    if (saveResults)
        myfile.open("./results.txt");
    
    initGL(argc, argv);

    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutMouseFunc(mouse);
    glutMotionFunc(motion);

    glutMainLoop();

    if (saveResults)
        myfile.close();

    return 0;
}
