// Galo Jimenez

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

#define PI 3.14159265

extern Global g;

extern void make_particle(int,int,int,float,float,float,float);

int random(int a){
	return (rand()%a) + 1;
}

void expl_360(int num_p, int t, int w, int h, float x, float y, int v_t)
{
    /*360 degree attack, 
      num_p is the number of particles created
      t is the type of particle (phsyics)
      w,h are width and height respetively
      x,y are x and y coordinates respetively
      v_t velocity of the outwards particles*/
    num_p = 360/num_p;
    for(int i = 0; i < 360 ; i = i + num_p) {
	make_particle(t,w,h,x,y, v_t * sin(i*PI/180), v_t * cos(i*PI/180));
    }
}

void helix(int num_p, int t, int w, int h, float x, float y, float v_x, float v_y)
{
    //helix logic inside of the physics function
    for(int i = 0; i < num_p ; i++){
	make_particle(t,w,h,x,y,v_x,v_y);
    }
}

void attacks(void)
{
    if (g.att_count%50 > 48){//This means the AI enters an attack pattern
			     //Explosion with type 1 - 4 
	expl_360(random(32),1,2,2,g.xres*0.5, g.yres*0.8,2 + random(3));
    }

    if (g.att_count%10 > 5){
        make_particle(3,2,2,g.xres*0.4 + random(100),g.yres*0.8,
	       	random(6) - 3,random(6) - 9);
    }


    g.att_count ++;


}

void screen_write(Rect r1){

    const char* words[5] = {"" , "", "", "" , ""};
    char int_str[9], int_str_2[9], int_str_3[9];

    //Create a char string, and give it the int value to char of a global
    sprintf(int_str, "%d" , g.n);
    words[0] = int_str;

    sprintf(int_str_2, "%d" , g.w);
    words[1] = int_str_2;

    sprintf(int_str_3, "%d" , g.att_count);
    words[2] = int_str_3;

    r1.bot = g.yres - g.yres*0.1;
    r1.left = g.xres - g.xres*0.1;
    r1.center = 0;
    ggprint8b(&r1,16,0x00ff0000, words[0]);

    r1.bot = g.yres - g.yres*0.125;
    r1.left = g.xres - g.xres*0.1;
    r1.center = 0;
    ggprint8b(&r1,16,0x00ff0000, words[1]);

    r1.bot = g.yres - g.yres*0.15;
    r1.left = g.xres - g.xres*0.1;
    r1.center = 0;
    ggprint8b(&r1,16,0x00ff0000, words[2]);

    r1.bot = g.yres - g.yres*0.15;
    r1.left = g.xres - g.xres*0.3;
    r1.center = 0;
    if (g.s == 1){
	ggprint8b(&r1,16,0x00ff0000, "Time Not Stopped");
    }else{
	ggprint8b(&r1,16,0x00ff0000, "Time Stopped");
    }


}
