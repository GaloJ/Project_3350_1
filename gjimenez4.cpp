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
extern void make_particle_2(int,int,int,float,float,float,float,float,float);

int random(int a){
	return (rand()%a) + 1;
}

void expl_360(int num_p, int t, int w, int h, float x, float y, int v_t)
{
    num_p = 360/num_p;
    for(int i = 0; i < 360 ; i = i + num_p) {
	make_particle(t,w,h,x,y, v_t * sin(i*PI/180), v_t * cos(i*PI/180));
    }
}

void spiral_360(int num_p, int t, int w, int h, float x, float y, int v_t)
{
	make_particle(t,w,h,x,y, v_t * sin(num_p*PI/180), v_t * cos(num_p*PI/180));
}

void attacks(void)
{
static int cd_2 = 0;
static int cd_3 = 0;
static int cd_4 = 0;
static int cd_5 = 0;
static int cd_6 = 0;

static int spiral = 0;
if(g.s == 1){

if(g.att == 1){
    if (g.att_count%50 > 48){//This means the AI enters an attack pattern
			     //Explosion with type 1 - 4 
	expl_360(random(32),99,2,2,g.xres*0.5, g.yres*0.8,2 + random(3));
    }

    if (g.att_count%250 == 0){
		g.a_1 = 30;
    }
    if(g.att_count%450 == 0){
        g.a_2 = 10;
    }
    if(g.att_count%1500 == 0){
        g.a_3 = 67;
    }
    if(g.att_count%780 == 0){
		g.a_4 = 10;
    }
    if(g.att_count%1150 == 0){
		g.a_5 = 100;
    }
    if(g.att_count%960 == 0){
		g.a_6 = 40;
    }
    if(g.att_count%600 == 0){
		g.a_7 = 200;
    }
    g.att_count ++;
}

if(g.a_1 > 1){ // Double Helix attack
    make_particle(4,4,4,g.xres*0.5,g.yres*0.8,10,20);
    make_particle(4,4,4,g.xres*0.5,g.yres*0.8,-10,20);
    g.a_1--;
}

if(g.a_2 > 1){ // random y axis non homing
    if(cd_2 == 0){
    expl_360(32,7,4,4,random(g.xres - 200) + 100, g.yres*0.8,5);
    g.a_2--;
    cd_2 = 20;
    }
    cd_2--;
}

if(g.a_3 > 1){ // Saturn attack
    if(cd_3 == 0){
    make_particle_2(5,2,2,g.xres*0.5 + 70,g.yres*0.8,g.xres*0.5,g.yres*0.8,3,3);
    make_particle_2(5,2,2,g.xres*0.5 + 60,g.yres*0.8,g.xres*0.5,g.yres*0.8,2.75,2.75);
    make_particle_2(5,2,2,g.xres*0.5 + 50,g.yres*0.8,g.xres*0.5,g.yres*0.8,2.5,2.5);
    make_particle_2(5,2,2,g.xres*0.5 + 40,g.yres*0.8,g.xres*0.5,g.yres*0.8,2.25,2.25);
        
    make_particle_2(5,2,2,g.xres*0.5 + 70,g.yres*0.8,g.xres*0.5,g.yres*0.8,-3,3);
    make_particle_2(5,2,2,g.xres*0.5 + 60,g.yres*0.8,g.xres*0.5,g.yres*0.8,-2.75,2.75);
    make_particle_2(5,2,2,g.xres*0.5 + 50,g.yres*0.8,g.xres*0.5,g.yres*0.8,-2.5,2.5);
    make_particle_2(5,2,2,g.xres*0.5 + 40,g.yres*0.8,g.xres*0.5,g.yres*0.8,-2.25,2.25);

    g.a_3--;
    cd_3 = 3;
    }
    cd_3--;
}

if(g.a_4 > 1){ // Expanding explosion 
    if(cd_4 == 0){
    expl_360(16,2,2,2,random(g.xres - 200) + 100, g.yres*0.8,5);
    g.a_4--;
    cd_4 = 10;
    }
    cd_4--;
}

if(g.a_5 > 1){ // Homing explosion 
    if(cd_5 == 0){
    make_particle(3,4,4,g.xres*0.5,g.yres*0.8,0,0);
    g.a_5--;
    cd_5 = 1;
    }
    cd_5--;
}

if(g.a_6 > 1){ // Homing explosion 
    if(cd_6 == 0){
    expl_360(64,8,2,2,g.xres*0.5, g.yres*0.8,10);
    g.a_6--;
    cd_6 = 1;
    }
    cd_6--;
}

if(g.a_7 > 1){ // Homing explosion   
    spiral_360(spiral,99,5,5,g.xres*0.5, g.yres*0.8,5);
    spiral += 6;
    g.a_7--;
}

}

}

void screen_write(Rect r1)
{

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

    r1.bot = g.yres - g.yres*0.2;
    r1.left = g.xres - g.xres*0.2;
    r1.center = 0;
    if (g.d == 1){
	ggprint8b(&r1,16,0x00ff0000, "DEBUG ON");
	r1.bot -= 20;
	ggprint8b(&r1,16,0x00ff0000, "1 - 7 for attacks");
	r1.bot -= 20;
	ggprint8b(&r1,16,0x00ff0000, "s to stop time");
	r1.bot -= 20;
	ggprint8b(&r1,16,0x00ff0000, "placeholder");
    }


}
