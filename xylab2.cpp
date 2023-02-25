//modified by: Galo Jimenez
//date: February 1st
//
//author: Gordon Griesel
//date: Spring 2022
//purpose: get openGL working on your personal computer
//
#include <iostream>
using namespace std;
#include <stdio.h>
#include <unistd.h>
#include <cstdlib>
#include <ctime>
#include <cstring>
#include <cmath>
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <GL/glx.h>
#include "fonts.h"

#define PI 3.14159265


//some structures

class Global {
    public:
	int xres, yres;
	int n;
	int f;
	int w;
	int s;
	float GRAVITY, GRAVITY_temp;
	Global();
} g;

const int MAX_PARTICLES = 10000;

class Box {
    public:
	int t;
	float w,h;
	float pos[2]; 
	float vel[2];	
	unsigned char color[3];
	void set_color(unsigned char col[3]){
	    memcpy(color,col,sizeof(unsigned char) * 3);
	}
	Box(){
	    t = 1;
	    w = 15.0f;
	    h = 15.0f;
	    pos[0]=g.xres*0.5;
	    pos[1]=g.yres*0.5;
	    vel[0] = 0.0;
	    vel[1] = 0.0;
	}

	Box(int type, float wid, float hgt, int x, int y, float v0,float v1)
	{
	    t = type;
	    w = wid;
	    h = hgt;
	    pos[0]= x;
	    pos[1]= y;
	    vel[0] = v0;
	    vel[1] = v1;

	}
}
box, particle[MAX_PARTICLES];


class X11_wrapper {
    private:
	Display *dpy;
	Window win;
	GLXContext glc;
    public:
	~X11_wrapper();
	X11_wrapper();
	void set_title();
	bool getXPending();
	XEvent getXNextEvent();
	void swapBuffers();
	void reshape_window(int width, int height);
	void check_resize(XEvent *e);
	void check_mouse(XEvent *e);
	int check_keys(XEvent *e);
} x11;

//Function prototypes
void init_opengl(void);
void physics(void);
void render(void);
void action(void);

//=====================================
// MAIN FUNCTION IS HERE
//=====================================
int main()
{
    init_opengl();
    //Main loop
    int done = 0;
    while (!done) {
	//Process external events.
	while (x11.getXPending()) {
	    XEvent e = x11.getXNextEvent();
	    x11.check_resize(&e);
	    x11.check_mouse(&e);
	    done = x11.check_keys(&e);
	}
	action();
	physics();
	render();
	x11.swapBuffers();
	usleep(20000);
    }
    return 0;
}

Global::Global()
{
    xres = 640;
    yres = 960;
    n = 0;
    f = -1;
    w = 0;
    s = 1;
}

X11_wrapper::~X11_wrapper()
{
    XDestroyWindow(dpy, win);
    XCloseDisplay(dpy);
}

X11_wrapper::X11_wrapper()
{
    GLint att[] = { GLX_RGBA, GLX_DEPTH_SIZE, 24, GLX_DOUBLEBUFFER, None };
    int w = g.xres, h = g.yres;
    dpy = XOpenDisplay(NULL);
    if (dpy == NULL) {
	cout << "\n\tcannot connect to X server\n" << endl;
	exit(EXIT_FAILURE);
    }
    Window root = DefaultRootWindow(dpy);
    XVisualInfo *vi = glXChooseVisual(dpy, 0, att);
    if (vi == NULL) {
	cout << "\n\tno appropriate visual found\n" << endl;
	exit(EXIT_FAILURE);
    } 
    Colormap cmap = XCreateColormap(dpy, root, vi->visual, AllocNone);
    XSetWindowAttributes swa;
    swa.colormap = cmap;
    swa.event_mask =
	ExposureMask | KeyPressMask | KeyReleaseMask |
	ButtonPress | ButtonReleaseMask |
	PointerMotionMask |
	StructureNotifyMask | SubstructureNotifyMask;
    win = XCreateWindow(dpy, root, 0, 0, w, h, 0, vi->depth,
	    InputOutput, vi->visual, CWColormap | CWEventMask, &swa);
    set_title();
    glc = glXCreateContext(dpy, vi, NULL, GL_TRUE);
    glXMakeCurrent(dpy, win, glc);
}

void X11_wrapper::set_title()
{
    //Set the window title bar.
    XMapWindow(dpy, win);
    XStoreName(dpy, win, "3350 Lab1");
}

bool X11_wrapper::getXPending()
{
    //See if there are pending events.
    return XPending(dpy);
}

XEvent X11_wrapper::getXNextEvent()
{
    //Get a pending event.
    XEvent e;
    XNextEvent(dpy, &e);
    return e;
}

void X11_wrapper::swapBuffers()
{
    glXSwapBuffers(dpy, win);
}

void X11_wrapper::reshape_window(int width, int height)
{
    //window has been resized.
    g.xres = width;
    g.yres = height;
    //
    glViewport(0, 0, (GLint)width, (GLint)height);
    glMatrixMode(GL_PROJECTION); glLoadIdentity();
    glMatrixMode(GL_MODELVIEW); glLoadIdentity();
    glOrtho(0, g.xres, 0, g.yres, -1, 1);
}

void X11_wrapper::check_resize(XEvent *e)
{
    //The ConfigureNotify is sent by the
    //server if the window is resized.
    if (e->type != ConfigureNotify)
	return;
    XConfigureEvent xce = e->xconfigure;
    if (xce.width != g.xres || xce.height != g.yres) {
	//Window size did change.
	reshape_window(xce.width, xce.height);
    }
}
//-----------------------------------------------------------------------------
void make_particle(float x, float y,float x_v, float y_v){
    if(g.n < MAX_PARTICLES){
	particle[g.n].t = 2; //particle type 3 is for vortex
	particle[g.n].w = 4;
	particle[g.n].h = 4;
	particle[g.n].pos[0] = x;
	particle[g.n].pos[1] = y;
	particle[g.n].vel[0] = x_v;	
	particle[g.n].vel[1] = y_v;			
	++g.n;
    }
}

void make_vortex(float x, float y, float x_v, float y_v){
        if (g.n < MAX_PARTICLES){
	particle[g.n].t = 3; //particle type 3 is for vortex
	particle[g.n].w = 2;
        particle[g.n].h = 2;
        particle[g.n].pos[0] = x;
        particle[g.n].pos[1] = y;
        particle[g.n].vel[0] = x_v;
        particle[g.n].vel[1] = y_v;
	++g.n;
    }
}

void explode(double x, double y){
/*Cirlce spawn, maybe make this into a function later since we will be using it a lot
/----------------------------------------------------
/ Using integers will generate problemsin the future for the particles, we will have to use floats most 
/ Likely for higher precision
*/
    for(int i=0; i < 360 ; i = i + 36){
	make_particle(x,y, 20 * sin(i*PI/180), 20 * cos(i*PI/180));
    }
}

void X11_wrapper::check_mouse(XEvent *e)
{
    static int savex = 0;
    static int savey = 0;

    //Weed out non-mouse events
    if (e->type != ButtonRelease &&
	    e->type != ButtonPress &&
	    e->type != MotionNotify) {
	//This is not a mouse event that we care about.
	return;
    }
    //
    if (e->type == ButtonRelease) {
	return;
    }
    if (e->type == ButtonPress) {
	if (e->xbutton.button==1) {
	    make_particle(e->xbutton.x , g.yres - e->xbutton.y,0,0);
	}
	if(e->xbutton.button==3){
	    explode(e->xbutton.x , g.yres - e->xbutton.y);
	}	
	return ;

	if (e->xbutton.button==2) {
	    //Right button was pressed.
	    return;
	}
    }
    if (e->type == MotionNotify) {
	//The mouse moved!
	if (savex != e->xbutton.x || savey != e->xbutton.y) {
	    savex = e->xbutton.x;
	    savey = e->xbutton.y;
	    //Code placed here will execute whenever the mouse moves.
	    // make_particle(e->xbutton.x,g.yres - e->xbutton.y);

	}
    }
}

int X11_wrapper::check_keys(XEvent *e)
{
    if (e->type != KeyPress && e->type != KeyRelease)
	return 0;
    int key = XLookupKeysym(&e->xkey, 0);
    if (e->type == KeyPress) {
	switch (key) {
	    case XK_Up:
		if(box.vel[1] < 5)
			box.vel[1] += 1;
		if(box.vel[1] < 0)
                        box.vel[1] -= box.vel[1]*0.75;
		break;
	    case XK_Down:
		if(box.vel[1] > -5)
			box.vel[1] -= 1;
		if(box.vel[1] > 0)
			box.vel[1] -= box.vel[1]*0.75;
		break;
	    case XK_Right:
		if(box.vel[0] < 5)
			box.vel[0] += 1;
		if(box.vel[0] < 0)
                        box.vel[0] -= box.vel[0]*0.75;
		break;
	    case XK_Left:
		if(box.vel[0] > -5)
			box.vel[0] -= 1;
		if(box.vel[0] > 0)
                        box.vel[0] -= box.vel[0]*0.75;
		break;
	    case XK_1:
		//Key 1 was pressed
		break;
	    case XK_m:
		box.vel[0] = 0;
		box.vel[1] = 0;
		break;	
	    case XK_f:
		g.f = -g.f;
		break;
	    case XK_v:
		make_vortex(200,200,0,0);
		break;
	    case XK_w:
		g.w = 0;
		break;
	    case XK_s:
		g.s = -g.s;
		break;		
	    case XK_Escape:
		//Escape key was pressed
		return 1;
	}
    }
    
    return 0;
}

void init_opengl(void)
{
    //OpenGL initialization
    glViewport(0, 0, g.xres, g.yres);
    //Initialize matrices
    glMatrixMode(GL_PROJECTION); glLoadIdentity();
    glMatrixMode(GL_MODELVIEW); glLoadIdentity();
    //Set 2D mode (no perspective)
    glOrtho(0, g.xres, 0, g.yres, -1, 1);
    //Set the screen background color
    glClearColor(0.1, 0.1, 0.1, 1.0);
    //Set Box Color
	unsigned char c[3] = {100,200,100};
	box.set_color(c);
	glEnable(GL_TEXTURE_2D);
	initialize_fonts();
}

void action(void)
{

}


void physics()
{
if(g.s == 1){

    // original particle physics ----------------------------------------------------
    	for(int i=0; i < g.n ; i++){
	if(particle[i].t == 2){ // Particle type changes physics
	    particle[i].vel[0] -= 0.1*particle[i].vel[0];
	    particle[i].vel[1] -= 0.1*particle[i].vel[1];
	}else if(particle[i].t == 3){
	    particle[i].vel[0] += (particle[i].pos[0]-box.pos[0])*0.001;
	    particle[i].vel[1] += (particle[i].pos[1]-box.pos[1])*0.001;
	}

	// this is the bread and butter of the phsyics, should always be running for
	// all particles, maybe make a function with it
	particle[i].pos[0] -= particle[i].vel[0];
	particle[i].pos[1] -= particle[i].vel[1];

	// check if particle went off screen and has to be done to every pattern
	if(particle[i].pos[1] < 0.0 || particle[i].pos[1] > g.yres ||
	       	particle[i].pos[0] < 0.0 || particle[i].pos[0] > g.xres){
	    particle[i] = particle[--g.n];
	}
	
	// orignal particle physics ends -----------------------------------------------
            
                if(particle[i].pos[1] < box.pos[1] + box.h &&
                        particle[i].pos[0] > box.pos[0] - box.w &&
                        particle[i].pos[0] < box.pos[0] + box.w &&
                        particle[i].pos[1] > box.pos[1] - box.h)
                {
			particle[i] = particle[--g.n];
		}

        }
	
	box.pos[0] += box.vel[0];
	box.pos[1] += box.vel[1];

    }

}


void render()
{

    // Font init goes here? Maybe

    Rect r1;
    glClear(GL_COLOR_BUFFER_BIT);

   const char* words[5] = {"Request" , "Design", "Testing", "Effort" , "Implementation"};
   char int_str[9], int_str_2[9];	
   sprintf(int_str, "%d" , g.n);
   words[0] = int_str;
   sprintf(int_str_2, "%d" , g.w);
   words[1] = int_str_2;
  
     // Draw box
	glPushMatrix();
	glColor3ubv(box.color);
	glTranslatef(box.pos[0], box.pos[1], 0.0f);
	glBegin(GL_QUADS);
	glVertex2f(-box.w, -box.h);
	glVertex2f(-box.w,  box.h);
	glVertex2f( box.w,  box.h);
	glVertex2f( box.w, -box.h);	
	glEnd();
	glPopMatrix();

    //Draw particle
    for (int i = 0; i<g.n ;i++)
    {
	//particle color
	glPushMatrix(); 
	glColor3ub(150, 160, 220);
	glTranslatef(particle[i].pos[0], particle[i].pos[1], 0.0f);
	glBegin(GL_QUADS);
	glVertex2f(-particle[i].w, -particle[i].h);
	glVertex2f(-particle[i].w,  particle[i].h);
	glVertex2f( particle[i].w,  particle[i].h);
	glVertex2f( particle[i].w, -particle[i].h);
	glEnd();
	glPopMatrix();
    }

    // Draw instructions, should have made it with a loop but I'm lazy? I think xD
    r1.bot = g.yres - g.yres*0.1;
    r1.left = g.xres - g.xres*0.1;
    r1.center = 0;
    ggprint8b(&r1,16,0x00ff0000, words[0]);

	r1.bot = g.yres - g.yres*0.125;
    r1.left = g.xres - g.xres*0.1;
    r1.center = 0;
    ggprint8b(&r1,16,0x00ff0000, words[1]);

    r1.bot = g.yres - g.yres*0.15;
    r1.left = g.xres - g.xres*0.3;
    r1.center = 0;
    if (g.s == 1){
	ggprint8b(&r1,16,0x00ff0000, "Time Not Stopped");
   }else{
   	ggprint8b(&r1,16,0x00ff0000, "Time Stopped");
   } 
    }






