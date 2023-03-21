//main.cpp
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

#define PI 3.14159265

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
    tp = 0;
    a_1 = a_2 = a_3 = a_4 = a_5 = a_6 = a_7 = 0;
    rep_ctr = 0;
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

/*
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
} x11;*/

class Image {
public:
        int width, height;
        unsigned char *data;
        ~Image() { delete [] data; }
        Image(const char *fname) {
                if (fname[0] == '\0')
                        return;
                char name[40];
                strcpy(name, fname);
                int slen = strlen(name);
                name[slen-4] = '\0';
                char ppmname[80];
                sprintf(ppmname,"%s.ppm", name);
                char ts[100];
                sprintf(ts, "convert %s %s", fname, ppmname);
                system(ts);
                FILE *fpi = fopen(ppmname, "r");
                if (fpi) {
                        char line[200];
                        fgets(line, 200, fpi);
                        fgets(line, 200, fpi);
                        //skip comments and blank lines
                        while (line[0] == '#' || strlen(line) < 2)
                                fgets(line, 200, fpi);
                        sscanf(line, "%i %i", &width, &height);
                        fgets(line, 200, fpi);
                        //get pixel data
                        int n = width * height * 3;                     
                        data = new unsigned char[n];                    
                        for (int i=0; i<n; i++)
                                data[i] = fgetc(fpi);
                        fclose(fpi);
                } else {
                        printf("ERROR opening image: %s\n", ppmname);
                        exit(0);
                }
                unlink(ppmname);
        }
};
Image img[1] = {"background.gif"};

class Texture {
    public:
            Image *backImage;
            GLuint backTexture;
            float xc[2];
            float yc[2];
} tex;

class X11_wrapper {
	private:
		Display *dpy;
		Window win;
		GLXContext glc;
	public:
		~X11_wrapper();
		X11_wrapper() {
			GLint att[] = { GLX_RGBA, GLX_DEPTH_SIZE, 24, GLX_DOUBLEBUFFER, None };
			//GLint att[] = { GLX_RGBA, GLX_DEPTH_SIZE, 24, None };
			setup_screen_res(640, 480);
			dpy = XOpenDisplay(NULL);
			if(dpy == NULL) {
				printf("\n\tcannot connect to X server\n\n");
				exit(EXIT_FAILURE);
			}
			Window root = DefaultRootWindow(dpy);
			XVisualInfo *vi = glXChooseVisual(dpy, 0, att);
			if(vi == NULL) {
				printf("\n\tno appropriate visual found\n\n");
				exit(EXIT_FAILURE);
			}
			Colormap cmap = XCreateColormap(dpy, root, vi->visual, AllocNone);
			XSetWindowAttributes swa;
			swa.colormap = cmap;
			swa.event_mask =
				ExposureMask | KeyPressMask | KeyReleaseMask | PointerMotionMask |
				ButtonPressMask | ButtonReleaseMask |
				StructureNotifyMask | SubstructureNotifyMask;
			win = XCreateWindow(dpy, root, 0, 0, g.xres, g.yres, 0,
					vi->depth, InputOutput, vi->visual,
					CWColormap | CWEventMask, &swa);
			set_title();
			glc = glXCreateContext(dpy, vi, NULL, GL_TRUE);
			glXMakeCurrent(dpy, win, glc);
		}
		void cleanupXWindows() {
			XDestroyWindow(dpy, win);
			XCloseDisplay(dpy);
		}
		void setup_screen_res(const int w, const int h) {
			g.xres = w;
			g.yres = h;
		}
		void reshape_window(int width, int height) {
			//window has been resized.
			setup_screen_res(width, height);
			glViewport(0, 0, (GLint)width, (GLint)height);
			glMatrixMode(GL_PROJECTION); glLoadIdentity();
			glMatrixMode(GL_MODELVIEW); glLoadIdentity();
			glOrtho(0, g.xres, 0, g.yres, -1, 1);
			set_title();
		}
		void set_title() {
			//Set the window title bar.
			XMapWindow(dpy, win);
			XStoreName(dpy, win, "scrolling background (seamless)");
		}
		bool getXPending() {
			return XPending(dpy);
		}
		XEvent getXNextEvent() {
			XEvent e;
			XNextEvent(dpy, &e);
			return e;
		}
		void swapBuffers() {
			glXSwapBuffers(dpy, win);
		}
		void check_resize(XEvent *e) {
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
extern float inv_tan(float, float);


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
	    make_particle(99,4,4,e->xbutton.x , g.yres - e->xbutton.y,0,0);
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
    //new stuff
    g.tex.backImage = &img[0];
    //create opengl texture elements
    glGenTextures(1, &g.tex.backTexture);
    int w = g.tex.backImage->width;
    int h = g.tex.backImage->height;
    glBindTexture(GL_TEXTURE_2D, g.tex.backTexture);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, 3, w, h, 0,
		    GL_RGB, GL_UNSIGNED_BYTE, g.tex.backImage->data);
    g.tex.xc[0] = 0.0;
    g.tex.xc[1] = 0.25;
    g.tex.yc[0] = 0.0;
    g.tex.yc[1] = 1.0;

}

void action(void)
// Function reserved for actions that are persistent, for example making a laser like
// attack where multiple small particles are made in quick succession 
{
    	int static toggle_d = 0;
	int static toggle_att = 0;
	int static toggle_s = 0;
	int static toggle_r = 0;
	int static toggle_w = 0;

	int static max_spd = 5;
	int static deaccel = 0.75;


	if (g.keys[XK_q]){
		max_spd = 15;
	}else{
		max_spd = 5;
	}

        if(g.keys[XK_w] && toggle_w == 0){//-----------w
                if(g.tp == 0){
                        g.tp = 1;
                }else{
                        g.tp = 0;
                }
                toggle_w = 1;
        }else if(!g.keys[XK_w] && toggle_w == 1){
                toggle_w = 0;
        }


	if (g.keys[XK_Up]){
		if(box.vel[1] <= 0)
		    	box.vel[1] = 0;		
		if(box.vel[1] < max_spd)
			box.vel[1] += 1.5;
	}
	if (g.keys[XK_Down]){
		if(box.vel[1] >= 0)
			box.vel[1] = 0;	
		if(box.vel[1] > -max_spd)
			box.vel[1] -= 1.5;
	}
	if (g.keys[XK_Right]){
		if(box.vel[0] <= 0)
			box.vel[0] = 0;
		if(box.vel[0] < max_spd)
            		box.vel[0] += 1.5;
	}
	if (g.keys[XK_Left]){
		if(box.vel[0] >= 0)
			box.vel[0] = 0;
		if(box.vel[0] > -max_spd)
            		box.vel[0] -= 1.5;
	}
	// Big deceleration when no keys pressed
	if(!g.keys[XK_Up] && !g.keys[XK_Down]){
		box.vel[1] = box.vel[1]*deaccel;
	}
	
	if(!g.keys[XK_Right] && !g.keys[XK_Left]){
	box.vel[0] = box.vel[0]*deaccel;
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

        if(g.keys[XK_r] && toggle_r == 0){//-----------att
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

	g.tex.xc[0] += 0.001;
	g.tex.xc[1] += 0.001;

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
		//BM p4 explosion xD
	
		if (g.rep_ctr == 1){ // Repel physics logic
		    float xf = particle[i].pos[0] - box.pos[0];
		    float yf = -(box.pos[1] - particle[i].pos[1]);
		    float mag = hypot(xf,yf);
		    //float theta = atan(yf/xf);
		    float theta = inv_tan(yf,xf);
		    if(yf >= 0 && xf >= 0 && mag < 150){//1st
			particle[i].vel[0] -= (200000/pow(mag,3))*xf*cos(theta);
                    	particle[i].vel[1] -= (200000/pow(mag,3))*yf*sin(theta);
			particle[i].t = 99;
		    }else if(yf <= 0 && xf >= 0 && mag < 150){//4th
			particle[i].vel[0] -= (200000/pow(mag,3))*xf*cos(theta);
                        particle[i].vel[1] += (200000/pow(mag,3))*yf*sin(theta);
			particle[i].t = 99;
		    }else if(yf <= 0 && xf <= 0 && mag < 150){//3rd
			particle[i].vel[0] += (200000/pow(mag,3))*xf*cos(theta);
                        particle[i].vel[1] += (200000/pow(mag,3))*yf*sin(theta);
			particle[i].t = 99;			
		    }else if (yf >= 0 && xf <= 0 && mag < 150){
			particle[i].vel[0] += (200000/pow(mag,3))*xf*cos(theta);
                        particle[i].vel[1] -= (200000/pow(mag,3))*yf*sin(theta);
			particle[i].t = 99;
		    }
		}
	
		
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

        }

    	if (g.tp == 1){
	    if(box.vel[0] > 0){
		box.pos[0] = box.pos[0] + 100;
	    }
	    else if(box.vel[0] < 0){
		box.pos[0] = box.pos[0] - 100;
	    }

	    if(box.vel[1] > 0){
                box.pos[1] = box.pos[1] + 100;
	    }
            else if(box.vel[1] < 0){
                box.pos[1] = box.pos[1] - 100;
	    }

	}

    // Check if box will go out of bounds
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
g.rep_ctr = 0; // so that it only happends once
g.tp = 0; // only teleport once
}

void render()
{
	static int t = 20;
	Rect r1;
	glClear(GL_COLOR_BUFFER_BIT);
	//Draw background
	glBindTexture(GL_TEXTURE_2D, g.tex.backTexture);
	glBegin(GL_QUADS);
	glTexCoord2f(g.tex.xc[0], g.tex.yc[1]); glVertex2i(0,      0);
	glTexCoord2f(g.tex.xc[0], g.tex.yc[0]); glVertex2i(0,      g.yres);
	glTexCoord2f(g.tex.xc[1], g.tex.yc[0]); glVertex2i(g.xres, g.yres);
	glTexCoord2f(g.tex.xc[1], g.tex.yc[1]); glVertex2i(g.xres, 0);
	glEnd();


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

    	if(g.d == 1){	
     	glColor3f(0.0, 1.0, 0.0);
    	glBegin(GL_TRIANGLE_STRIP);
        glVertex2i(0,0);
        glVertex2i(t,t);
        glVertex2i(0,g.yres);
        glVertex2i(t,g.yres-t);
        glVertex2i(g.xres, g.yres);
        glVertex2i(g.xres-t,g.yres-t);
        glVertex2i(g.xres,0);
        glVertex2i(g.xres-t,t);
        glVertex2i(0,0);
        glVertex2i(t,t);
    	glEnd();
	}

	// SCREEN WRITINGS
    screen_write(r1);
    }
