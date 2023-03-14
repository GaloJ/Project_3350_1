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
#include "global.h"
#include "box.h"

Global::Global()
{
    xres = 640;
    yres = 960;
    att = 0;
    d = 0;
    s = 1;
    n = 0;
    w = 0;
    done = 0;
    a_1 = a_2 = a_3 = a_4 = a_5 = a_6 = a_7 = 0;
    rep_ctr = 0;
    plyr_decel = 0.75; // 0 is full stop, 1 is no decel
    curr_att = 10; //Number of attacks left per attack cycle
    att_count = 1; // Must be 1 or all attacks proc at the start
    difficulty = 50;
    memset(keys, 0, 65536);
}Global g;

Box::Box(){
    t = 1;
    w = 10.0f;
    h = 10.0f;
    pos[0] = g.xres*0.5;
    pos[1] = g.yres*0.25;
    pos_i[0] = pos[0];
    pos_i[1] = pos[1];
    vel[0] = 0.0;
    vel[1] = 0.0;
}

Box::Box(int type, float wid, float hgt, float x, float y, float v0,float v1)
{
    t = type;
    w = wid;
    h = hgt;
    pos[0]= x;
    pos[1]= y;
    pos_i[0] = x;
    pos_i[1] = y;
    vel[0] = v0;
    vel[1] = v1;

}

Box box;
Box particle[MAX_PARTICLES];


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
void attacks(void);

//Extern Galo prototypes
extern void expl_360(int,int,int,int,float,float,int);
extern void spiral_360(int,int,int,int,float,float,int);
extern void attacks(void);
extern void physics(void);
extern void screen_write(Rect);
extern int random(int);

//=====================================
// MAIN FUNCTION IS HERE
//=====================================
int main()
{
    init_opengl();
    //Main loop
    while (!g.done) {
	//Process external events.
	while (x11.getXPending()) {
	    XEvent e = x11.getXNextEvent();
	    x11.check_resize(&e);
	    x11.check_mouse(&e);
		x11.check_keys(&e);
	}
	attacks();
	action();
	physics();
	render();
	x11.swapBuffers();
	usleep(20000);
    }
    return 0;
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
    XStoreName(dpy, win, "The Last Hope");
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


void make_particle(int type, int wid, int hei, float x, float y,float x_v, float y_v){
    if(g.n < MAX_PARTICLES){
		particle[g.n].t = type; //particle type 3 is for vortex
		particle[g.n].w = wid;
		particle[g.n].h = hei;
		particle[g.n].pos[0] = x;
		particle[g.n].pos[1] = y;
		particle[g.n].pos_i[0] = x;
		particle[g.n].pos_i[1] = y;
		particle[g.n].vel[0] = x_v;	
		particle[g.n].vel[1] = y_v;			
		++g.n;
    }
}

void make_particle_2(int type, int wid, int hei, float x, float y,float xi, float yi,float x_v, float y_v){
    if(g.n < MAX_PARTICLES){
		particle[g.n].t = type; //particle type 3 is for vortex
		particle[g.n].w = wid;
		particle[g.n].h = hei;
		particle[g.n].pos[0] = x;
		particle[g.n].pos[1] = y;
		particle[g.n].pos_i[0] = xi;
		particle[g.n].pos_i[1] = yi;
		particle[g.n].vel[0] = x_v;	
		particle[g.n].vel[1] = y_v;			
		++g.n;
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
	    make_particle(2,4,4,e->xbutton.x , g.yres - e->xbutton.y,0,0);
	}
	if(e->xbutton.button==3){
	    expl_360(32,99,4,4,e->xbutton.x , g.yres - e->xbutton.y,5);
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
	if (e->type != KeyRelease && e->type != KeyPress) {
		//not a keyboard event
		return 0;
	}
	int key = (XLookupKeysym(&e->xkey, 0) & 0x0000ffff);
	//Log("key: %i\n", key);
	if (e->type == KeyRelease) {
		g.keys[key] = 0;
		return 0;
	}

	if (e->type == KeyPress) {
		//std::cout << "press" << std::endl;
		g.keys[key]=1;
			return 0;
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
// Function reserved for actions that are persistent, for example making a laser like
// attack where multiple small particles are made in quick succession 
{
	int static toggle_d = 0;
	int static toggle_att = 0;
	int static toggle_s = 0;
        int static toggle_r = 0;
	

	if (g.keys[XK_Up]){
		if(box.vel[1] <= 0)
			box.vel[1] = 0;		
		if(box.vel[1] < 5)
			box.vel[1] += 0.5;
	}
	if (g.keys[XK_Down]){
		if(box.vel[1] >= 0)
			box.vel[1] = 0;	
		if(box.vel[1] > -5)
			box.vel[1] -= 0.5;
	}
	if (g.keys[XK_Right]){
		if(box.vel[0] < 5)
			box.vel[0] += 1.5;
		if(box.vel[0] < 0)
            box.vel[0] -= box.vel[0]*0.75;
	}
	if (g.keys[XK_Left]){
		if(box.vel[0] > -5)
			box.vel[0] -= 1.5;
		if(box.vel[0] > 0)
            box.vel[0] -= box.vel[0]*0.75;
	}
	// Big deceleration when no keys pressed
	if(!g.keys[XK_Up] && !g.keys[XK_Down]){
	box.vel[1] = box.vel[1]*g.plyr_decel;
	}
	if(!g.keys[XK_Right] && !g.keys[XK_Left]){
	box.vel[0] = box.vel[0]*g.plyr_decel;
	}

	if(g.keys[XK_d] && toggle_d == 0){//------------d
		if(g.d == 0){
			g.d = 1;
		}else{
			g.d = 0;
		}
		toggle_d = 1;
	}else if(!g.keys[XK_d] && toggle_d == 1){
		toggle_d = 0;
	}

	if(g.keys[XK_a] && toggle_att == 0){//-----------att
		if(g.att == 0){
			g.att = 1;
		}else{
			g.att = 0;
		}
		toggle_att = 1;
	}else if(!g.keys[XK_a] && toggle_att == 1){
		toggle_att = 0;
	}

        if(g.keys[XK_r] && toggle_r == 0){//-----------------r
                if(g.rep_ctr == 0){
		   	g.rep_ctr = 1;
                }else{
			g.rep_ctr = 0;
		}
                toggle_r = 1;
        }else if(!g.keys[XK_r] && toggle_r == 1){
                toggle_r = 0;
        }



	if(g.d ==1){
		if(g.keys[XK_1]){
			g.a_1 = 30;
		}

		if(g.keys[XK_2]){
			g.a_2 = 10;
		}

		if(g.keys[XK_3]){
			g.a_3 = 67;
		}

		if(g.keys[XK_4]){
			g.a_4 = 10;
		}

		if(g.keys[XK_5]){
			g.a_5 = 100;
		}

		if(g.keys[XK_6]){
			g.a_6 = 40;
		}

		if(g.keys[XK_7]){
			g.a_7 = 200;
		}

		if(g.keys[XK_s] && toggle_s == 0){
			if(g.s == 0){
				g.s = 1;
			}else{
				g.s = 0;
			}
			toggle_s = 1;
		}else if(!g.keys[XK_s] && toggle_s == 1){
			toggle_s = 0;
	}
	}

	if(g.keys[XK_Escape]){
		g.done = 1;
	}

}

void physics(){
if(g.s == 1){
    for(int i=0; i < g.n ; i++){

        // check if particle went off screen and has to be done to every pattern
        if(particle[i].pos[1] < 0.0 || particle[i].pos[1] > g.yres ||
                particle[i].pos[0] < 0.0 || particle[i].pos[0] > g.xres){
            particle[i] = particle[--g.n];
        }
		if(particle[i].t == 1){
			particle[i].vel[1] += random(3)*0.1;
			particle[i].vel[0] += (random(5) - 3)*0.05;
		}else if(particle[i].t == 2){ // Decelerate particle -> Type 3 at 0 velocity
            particle[i].vel[0] -= 0.1*particle[i].vel[0];
            particle[i].vel[1] -= 0.1*particle[i].vel[1];
            if(abs(particle[i].vel[0]) < 0.01 && abs(particle[i].vel[1]) < 0.01)
                particle[i].t = 3;
        }else if(particle[i].t == 3){ //Homing type particle
            if (particle[i].pos[1] > box.pos[1]){
            particle[i].vel[0] += (particle[i].pos[0]-box.pos[0])*0.001;
            particle[i].vel[1] += (particle[i].pos[1]-box.pos[1])*0.001;
            }
        }else if(particle[i].t == 4){ // Sin wave going downwards behaviour
        	particle[i].vel[0] += (particle[i].pos[0]-particle[i].pos_i[0])*0.05;
        }else if(particle[i].t == 5){ // oscilate point
        	particle[i].vel[0] += (particle[i].pos[0]-particle[i].pos_i[0])*0.001;
        	particle[i].vel[1] += (particle[i].pos[1]-particle[i].pos_i[1])*0.001;
			if(g.a_3 == 1){
				particle[i].t = 6;
			}
		}else if(particle[i].t == 6){ // Decelerate particle -> Type 3 at 0 velocity
            particle[i].vel[0] += 0.01*particle[i].vel[0];
            particle[i].vel[1] += 0.01*particle[i].vel[1];
		}else if(particle[i].t == 7){ // Decelerate particle -> Type 3 at 0 velocity
            particle[i].vel[0] -= 0.1*particle[i].vel[0];
            particle[i].vel[1] -= 0.1*particle[i].vel[1];
            if(abs(particle[i].vel[0]) < 0.01 && abs(particle[i].vel[1]) < 0.01)
                particle[i].t = 1;
        }else if(particle[i].t == 8){ // Decelerate particle -> Type 9 at 0 velocity
            particle[i].vel[0] -= 0.0125*particle[i].vel[0];
            particle[i].vel[1] -= 0.0125*particle[i].vel[1];
            if(abs(particle[i].vel[0]) < 2 && abs(particle[i].vel[1]) < 2)
                particle[i].t = 9;
        }else if(particle[i].t == 9){ // flip velocity vector 
            particle[i].vel[0] = -particle[i].vel[0];
            particle[i].vel[1] = -particle[i].vel[1];
			particle[i].t = 10;
		}else if(particle[i].t == 10){ // Decelerate particle -> Type 9 at 0 velocity
            particle[i].vel[0] += 0.025*particle[i].vel[0];
            particle[i].vel[1] += 0.025*particle[i].vel[1];
        }

	if(g.rep_ctr == 1){
	particle[i].vel[0] += (particle[i].pos[0]+box.pos[0])*0.01;
        particle[i].vel[1] += (particle[i].pos[1]+box.pos[1])*0.01;
	}
        g.rep_ctr = 0;
		//BM p4 explosion xD

        // this is the bread and butter of the phsyics, 
        // should always be running for
        // all particles, maybe make a function with it
        particle[i].pos[0] -= particle[i].vel[0];
        particle[i].pos[1] -= particle[i].vel[1];

        if(particle[i].pos[1] - particle[i].h < box.pos[1] + box.h &&
                particle[i].pos[0] + particle[i].w > box.pos[0] - box.w &&
                particle[i].pos[0] - particle[i].w< box.pos[0] + box.w &&
                particle[i].pos[1] + particle[i].h > box.pos[1] - box.h)
        {
            particle[i] = particle[--g.n];
            g.w ++;
        }

        }// Check if box will go out of bounds
        if(box.pos[1] > g.yres - box.h){
			box.vel[1] = 0;
			box.pos[1] = g.yres - box.h;
		}else if(box.pos[1] < box.h){
			box.vel[1] = 0;
			box.pos[1] = box.h;
        }else if(box.pos[0] > g.xres - box.w ){
			box.vel[0] = 0;
			box.pos[0] = g.xres - box.w;
        }else if(box.pos[0] < box.w){
			box.vel[0] = 0;
			box.pos[0] = box.w;
        }else{ // If Box is not out of bounds it works properly
            box.pos[0] += box.vel[0];
            box.pos[1] += box.vel[1];
        }
}

}

void render()
{
	Rect r1;
	glClear(GL_COLOR_BUFFER_BIT);

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

	// SCREEN WRITINGS
    screen_write(r1);
    }
