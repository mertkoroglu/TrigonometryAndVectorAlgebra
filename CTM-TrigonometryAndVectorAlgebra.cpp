/*********
   
PROBLEMS:
1. For some reason I can't rotate the chasing object.
----------
ADDITIONAL FEATURES:
1. If you press F1 sun will disapear and moon will take its place.
2. moon moves in the opposite direction of sun.
3. if you press F1 also backgound will change accordingly. For example, sky will go dark, building and tree's color will change.
4. top left it displays which mode are you on (DAY/NIGHT).
5. Clouds move linearly in x axis.
*********/

#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdarg.h>
#include "vec.h"

#define WINDOW_WIDTH  800
#define WINDOW_HEIGHT 500

#define TIMER_PERIOD  16 // Period for the timer.
#define TIMER_ON       1 // 0:disable timer, 1:enable timer
#define ENEMY_SPEED 4
#define D2R 0.0174532

/* Global Variables for Template File */
bool up = false, down = false, right = false, left = false, day = true, mode1 = false;
int  winWidth, winHeight; // current Window width and height
float xi, yi;
int move1 = 0, move2 = 0;

typedef struct {
    float r, g, b;
} color_t;

typedef struct {
    vec_t   pos;
    color_t color;
    vec_t   vel;
} light_t;

typedef struct {
    vec_t pos;
    vec_t N;
} vertex_t;

typedef struct {
    vec_t p;
    vec_t v;
    float angle;
} enemy_t;

light_t sun = { { -250, -100 }, { 1, 0.88, 0 }, {4,  1} };
light_t moon = { {-250, -100}, {1, 1, 1}, {4, 1} };
enemy_t Q;

color_t mulColor(float k, color_t c) {
    color_t tmp = { k * c.r, k * c.g, k * c.b };
    return tmp;
}

color_t addColor(color_t c1, color_t c2) {
    color_t tmp = { c1.r + c2.r, c1.g + c2.g, c1.b + c2.b };
    return tmp;
}

// To add distance into calculation
// when distance is 0  => its impact is 1.0
// when distance is 350 => impact is 0.0
// Linear impact of distance on light calculation.
double distanceImpact(double d) {
    return (-1.0 / 350.0) * d + 1.0;
}

color_t calculateColor(light_t source, vertex_t v) {
    vec_t L = subV(source.pos, v.pos);
    vec_t uL = unitV(L);
    float factor = dotP(uL, v.N) * distanceImpact(magV(L));
    return mulColor(factor, source.color);
}


//
// to draw circle, center at (x,y)
// radius r
//
void circle(int x, int y, int r)
{
#define PI 3.1415
    float angle;
    glBegin(GL_POLYGON);
    for (int i = 0; i < 100; i++)
    {
        angle = 2 * PI * i / 100;
        glVertex2f(x + r * cos(angle), y + r * sin(angle));
    }
    glEnd();
}

void circle_wire(int x, int y, int r)
{
#define PI 3.1415
    float angle;

    glBegin(GL_LINE_LOOP);
    for (int i = 0; i < 100; i++)
    {
        angle = 2 * PI * i / 100;
        glVertex2f(x + r * cos(angle), y + r * sin(angle));
    }
    glEnd();
}

void print(int x, int y, const char* string, void* font)
{
    int len, i;

    glRasterPos2f(x, y);
    len = (int)strlen(string);
    for (i = 0; i < len; i++)
    {
        glutBitmapCharacter(font, string[i]);
    }
}

// display text with variables.
void vprint(int x, int y, void* font, const char* string, ...)
{
    va_list ap;
    va_start(ap, string);
    char str[1024];
    vsprintf_s(str, string, ap);
    va_end(ap);

    int len, i;
    glRasterPos2f(x, y);
    len = (int)strlen(str);
    for (i = 0; i < len; i++)
    {
        glutBitmapCharacter(font, str[i]);
    }
}

void vprint2(int x, int y, float size, const char* string, ...) {
    va_list ap;
    va_start(ap, string);
    char str[1024];
    vsprintf_s(str, string, ap);
    va_end(ap);
    glPushMatrix();
    glTranslatef(x, y, 0);
    glScalef(size, size, 1);

    int len, i;
    len = (int)strlen(str);
    for (i = 0; i < len; i++)
    {
        glutStrokeCharacter(GLUT_STROKE_ROMAN, str[i]);
    }
    glPopMatrix();
}


void TieFighter() {
    double angle = 20 * D2R;
    glColor3ub(57, 240, 153);
    circle_wire(Q.p.x, Q.p.y, 10);


    glColor3ub(128, 128, 128);
 
    glRectf(Q.p.x - 20, Q.p.y - 15, Q.p.x - 16, Q.p.y + 15);
    glRectf(Q.p.x + 20, Q.p.y - 15, Q.p.x + 16, Q.p.y + 15);
    glRectf(Q.p.x, Q.p.y - 2, Q.p.x - 20, Q.p.y + 2);
    glRectf(Q.p.x, Q.p.y - 2, Q.p.x + 20, Q.p.y + 2);
   
    glColor3ub(0, 0, 0);
    circle(Q.p.x, Q.p.y, 9);
  
    

}

void SUN() {

    glColor3f(sun.color.r, sun.color.g, sun.color.b);
    circle(sun.pos.x, sun.pos.y, 30);

   glBegin(GL_TRIANGLES);
    glVertex2f(sun.pos.x + 25, sun.pos.y + 15);
    glVertex2f(sun.pos.x + 50, sun.pos.y);
    glVertex2f(sun.pos.x + 25, sun.pos.y - 15);

    glVertex2f(sun.pos.x - 25, sun.pos.y + 15);
    glVertex2f(sun.pos.x - 50, sun.pos.y);
    glVertex2f(sun.pos.x - 25, sun.pos.y - 15);

    glVertex2f(sun.pos.x - 15, sun.pos.y + 25);
    glVertex2f(sun.pos.x, sun.pos.y + 50);
    glVertex2f(sun.pos.x + 15, sun.pos.y + 25);

    glVertex2f(sun.pos.x - 15, sun.pos.y - 25);
    glVertex2f(sun.pos.x, sun.pos.y - 50);
    glVertex2f(sun.pos.x + 15, sun.pos.y - 25);
    glEnd();

    glColor3f(0.38, 0.68, 1);
    circle_wire(sun.pos.x, sun.pos.y, 30);

}

void MOON() {

    glColor3f(moon.color.r, moon.color.g, moon.color.b);
    circle(moon.pos.x, moon.pos.y, 30);
    glColor3f(0, 0.047, 0.29);
    circle(moon.pos.x + 15, moon.pos.y, 30);

}

void Background() {
    glColor3ub(255, 255, 255);  //clouds
    
    circle(winWidth / 2 - 610 + move1, winHeight / 2 - 150, 29);
    circle(winWidth / 2 - 590 + move1, winHeight / 2 - 180, 30);
    circle(winWidth / 2 - 590 + move1, winHeight / 2 - 170, 29);
    circle(winWidth / 2 - 580 + move1, winHeight / 2 - 160, 30);
    circle(winWidth / 2 - 620 + move1, winHeight / 2 - 180, 34);

    circle(winWidth / 2 - 310 + move2, winHeight / 2 - 190, 29);
    circle(winWidth / 2 - 290 + move2, winHeight / 2 - 230, 30);
    circle(winWidth / 2 - 290 + move2, winHeight / 2 - 220, 29);
    circle(winWidth / 2 - 280 + move2, winHeight / 2 - 210, 30);
    circle(winWidth / 2 - 320 + move2, winHeight / 2 - 230, 34);

    if(day == true)
    vprint(-winWidth / 2 + 50, winHeight / 2 - 50, GLUT_BITMAP_HELVETICA_18, "DAY");
    else
    vprint(-winWidth / 2 + 50, winHeight / 2 - 50, GLUT_BITMAP_HELVETICA_18, "NIGHT");

    if (day == true)
        glColor3ub(158, 74, 0); //tree body
    else
        glColor3ub(48, 23, 0);
    glRectf(-winWidth / 2 + 130, -winHeight / 2 + 110, -winWidth / 2 + 90, -winHeight / 2 + 20);

    if (day == true)
        glColor3ub(23, 117, 11);  //tree leafs
    else
        glColor3ub(14, 26, 8);
    circle(-winWidth / 2 + 120, -winHeight / 2 + 100, 29);
    circle(-winWidth / 2 + 100, -winHeight / 2 + 140, 30);
    circle(-winWidth / 2 + 100, -winHeight / 2 + 130, 29);
    circle(-winWidth / 2 + 90, -winHeight / 2 + 120, 30);
    circle(-winWidth / 2 + 130, -winHeight / 2 + 140, 34);

    if (day == true)
        glColor3ub(255, 255, 255);
    else
        glColor3ub(105, 106, 107);
    glRectf(-winWidth / 2 + 200, -winHeight / 2 + 250, -winWidth / 2 + 310, -winHeight / 2 + 50);


    int y1 = 230, y2 = 210;
    if (day == true)
        glColor3ub(31, 83, 156);
    else
        glColor3ub(255, 238, 0);
    for (int i = 0; i < 6; i++) {
        glRectf(-winWidth / 2 + 210, -winHeight / 2 + y1, -winWidth / 2 + 245, -winHeight / 2 + y2);
        glRectf(-winWidth / 2 + 265, -winHeight / 2 + y1, -winWidth / 2 + 300, -winHeight / 2 + y2);
        y1 = y1 - 30;
        y2 = y2 - 30;

    }

    int x1 = 330, y3 = 290;
    
}

void display() {

    if (day == true)
        glClearColor(0.38, 0.68, 1, 0);
    else
        glClearColor(0, 0.047, 0.29 , 0);

    glClear(GL_COLOR_BUFFER_BIT);
   
    // sun
    if (day != true) {
        MOON();
    }
    // moon
    else
    {
        SUN();
    }

    Background();

   double y = -250;

    for (int x = -400; x <= 400; x++) {
        vertex_t P = { { x, y }, { 0, 1 } };
        color_t res = { 0, 0, 0 };
        
        if(day == true)
            res = addColor(res, calculateColor(sun, P));
        else
            res = addColor(res, calculateColor(moon, P));

        glBegin(GL_LINES);
        glColor3f(0.07, 0.08, 0.05);
        glVertex2f(x, -250);
        glColor3f(res.r, res.g, res.b);
        glVertex2f(x, y);
        glEnd();
        y = y + 0.3;
    }
    
    if (mode1 != false)
        TieFighter();
   
    glutSwapBuffers();

}

//
// key function for ASCII charachters like ESC, a,b,c..,A,B,..Z
//
void onKeyDown(unsigned char key, int x, int y)
{
    // exit when ESC is pressed.
    if (key == 27)
        exit(0);

    // to refresh the window it calls display() function
    glutPostRedisplay();
}

void onKeyUp(unsigned char key, int x, int y)
{
    // exit when ESC is pressed.
    if (key == 27)
        exit(0);

    // to refresh the window it calls display() function
    glutPostRedisplay();
}
//
// Special Key like GLUT_KEY_F1, F2, F3,...
// Arrow Keys, GLUT_KEY_UP, GLUT_KEY_DOWN, GLUT_KEY_RIGHT, GLUT_KEY_RIGHT
//
void onSpecialKeyDown(int key, int x, int y)
{
    // Write your codes here.
    switch (key) {
    case GLUT_KEY_UP: up = true; break;
    case GLUT_KEY_DOWN: down = true; break;
    case GLUT_KEY_LEFT: left = true; break;
    case GLUT_KEY_RIGHT: right = true; break;
    case GLUT_KEY_F1: day = !day;
    }
    // to refresh the window it calls display() function
    glutPostRedisplay();
}

//
// Special Key like GLUT_KEY_F1, F2, F3,...
// Arrow Keys, GLUT_KEY_UP, GLUT_KEY_DOWN, GLUT_KEY_RIGHT, GLUT_KEY_RIGHT
//
void onSpecialKeyUp(int key, int x, int y)
{
    // Write your codes here.
    switch (key) {
    case GLUT_KEY_UP: up = false; break;
    case GLUT_KEY_DOWN: down = false; break;
    case GLUT_KEY_LEFT: left = false; break;
    case GLUT_KEY_RIGHT: right = false; break;
    }
    // to refresh the window it calls display() function
    glutPostRedisplay();
}

//
// When a click occurs in the window,
// It provides which button
// buttons : GLUT_LEFT_BUTTON , GLUT_RIGHT_BUTTON
// states  : GLUT_UP , GLUT_DOWN
// x, y is the coordinate of the point that mouse clicked.
//
void onClick(int button, int stat, int x, int y)
{
    // Write your codes here.

    if (button == GLUT_LEFT_BUTTON && stat == GLUT_UP)
    {    
            Q.p.x = x - winWidth/2;
            Q.p.y = winHeight/2 - y;
            mode1 = true;
    }

    // to refresh the window it calls display() function
    glutPostRedisplay();
}

//
// This function is called when the window size changes.
// w : is the new width of the window in pixels.
// h : is the new height of the window in pixels.
//
void onResize(int w, int h)
{
    winWidth = w;
    winHeight = h;
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-w / 2, w / 2, -h / 2, h / 2, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    display(); // refresh window.
}

void onMoveDown(int x, int y) {
    // Write your codes here.



    // to refresh the window it calls display() function   
    glutPostRedisplay();
}

void onMove(int x, int y) {
    // Write your codes here.



    // to refresh the window it calls display() function
    glutPostRedisplay();
}


#if TIMER_ON == 1
void onTimer(int v) {

    glutTimerFunc(TIMER_PERIOD, onTimer, 0);
    // Write your codes here.

    //cloud movement
    if (move1 < 650)
        move1++;
    else
        move1 = -250;

    if (move2 < 350)
        move2++;
    else
        move2 = -550;

    if (day == true) {
        sun.pos = addV(sun.pos, sun.vel);
        

        if (sun.pos.x > 650) {
            sun.pos.x = -650;
            sun.pos.y = -100;
        }

        //sun intersection
        if (Q.p.x < sun.pos.x + 15 && Q.p.x > sun.pos.x - 15 && Q.p.y < sun.pos.y + 15 && Q.p.y > sun.pos.y - 15)
            mode1 = false;

      
        Q.v = mulV(ENEMY_SPEED, unitV(subV(sun.pos, Q.p)));
        Q.p = addV(Q.p, Q.v); // chase
  

    }
    else
    {
        moon.pos = subV(moon.pos, moon.vel);
 

        if (moon.pos.x < -400) {
            moon.pos.x = 400;
            moon.pos.y = 200;
        }


        // moon intersection
        if (Q.p.x < moon.pos.x + 15 && Q.p.x > moon.pos.x - 15 && Q.p.y < moon.pos.y + 15 && Q.p.y > moon.pos.y - 15)
            mode1 = false;

       

        Q.v = mulV(ENEMY_SPEED, unitV(subV(moon.pos, Q.p)));
        Q.p = addV(Q.p, Q.v); // chase
       
        
        
    }


    // to refresh the window it calls display() function
    glutPostRedisplay(); // display()

}
#endif

void Init() {

    // Smoothing shapes
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

}

void main(int argc, char* argv[]) {

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    glutCreateWindow("HW-4: 'Chasing Sun and Moon' by Mert Köroðlu");

    glutDisplayFunc(display);
    glutReshapeFunc(onResize);

    //
    // keyboard registration
    //
    glutKeyboardFunc(onKeyDown);
    glutSpecialFunc(onSpecialKeyDown);

    glutKeyboardUpFunc(onKeyUp);
    glutSpecialUpFunc(onSpecialKeyUp);

    //
    // mouse registration
    //
    glutMouseFunc(onClick);
    glutMotionFunc(onMoveDown);
    glutPassiveMotionFunc(onMove);

#if  TIMER_ON == 1
    // timer event
    glutTimerFunc(TIMER_PERIOD, onTimer, 0);
#endif

    Init();

    glutMainLoop();
}
