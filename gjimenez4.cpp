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
#include "box.h"

#define PI 3.14159265

extern Global g;
extern Box box;
extern Box particle[MAX_PARTICLES];

extern void make_particle(int,int,int,float,float,float,float);
extern void make_particle_2(int,int,int,float,float,float,float,float,float);

int random(int a)
{
    return (rand()%a) + 1;
}

//Quadrants 1 3 4 2
float inv_tan(float a, float b)
{
    if (a > 0 && b > 0) {
        return atan(a/b);
    } else if (a < 0 && b > 0) {
        return (2*PI) + atan(a/b);    
    } else if (a < 0 && b < 0) {
        return (atan(a/b) + PI);
    } else {
        return (PI + atan(a/b));
    }
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

    //Time is running
    if (g.s == 1) {

        //Random attack are set on 
        if (g.att == 1) {
            if (g.att_count%50 > 48) { 
                expl_360(32 + random(32),99,2,2,g.xres*0.5
                        , g.yres*0.8,2 + random(3));
            }

            if (g.att_count%511 == 0) {
                g.a_1 = 30;
		g.dash ++;
            }
            if(g.att_count%917 == 0) {
                g.a_2 = 10;
            }
            if(g.att_count%2978 == 0) {
                g.a_3 = 67;
		g.shield ++;
            }
            if(g.att_count%1527 == 0) {
                g.a_4 = 10;
		g.repel ++;
            }
            if(g.att_count%2356 == 0) {
                g.a_5 = 100;
		g.teleport ++;
            }
            if(g.att_count%2074 == 0) {
                g.a_6 = 40;
            }
            if(g.att_count%1175 == 0) {
                g.a_7 = 200;
            }
            g.att_count ++;
        }

        //Double Hellix attack
        if (g.a_1 > 1) {
            make_particle(4,4,4,g.xres*0.5,g.yres*0.8,10,20);
            make_particle(4,4,4,g.xres*0.5,g.yres*0.8,-10,20);
            g.a_1--;
        }

        //Non homing attack
        if (g.a_2 > 1) {
            if(cd_2 == 0){
                expl_360(32,7,4,4,random(g.xres - 200) + 100, g.yres*0.8,5);
                g.a_2--;
                cd_2 = 20;
            }
            cd_2--;
        }

        // Saturn Attack
        if (g.a_3 > 1) {
            if(cd_3 == 0){
                make_particle_2(5,2,2,g.xres*0.5 + 70,g.yres*
                        0.8,g.xres*0.5,g.yres*0.8,3,3);
                make_particle_2(5,2,2,g.xres*0.5 + 60,g.yres*
                        0.8,g.xres*0.5,g.yres*0.8,2.75,2.75);
                make_particle_2(5,2,2,g.xres*0.5 + 50,g.yres*
                        0.8,g.xres*0.5,g.yres*0.8,2.5,2.5);
                make_particle_2(5,2,2,g.xres*0.5 + 40,g.yres*
                        0.8,g.xres*0.5,g.yres*0.8,2.25,2.25);
                make_particle_2(5,2,2,g.xres*0.5 + 70,g.yres*
                        0.8,g.xres*0.5,g.yres*0.8,-3,3);
                make_particle_2(5,2,2,g.xres*0.5 + 60,g.yres*
                        0.8,g.xres*0.5,g.yres*0.8,-2.75,2.75);
                make_particle_2(5,2,2,g.xres*0.5 + 50,g.yres*
                        0.8,g.xres*0.5,g.yres*0.8,-2.5,2.5);
                make_particle_2(5,2,2,g.xres*0.5 + 40,g.yres*
                        0.8,g.xres*0.5,g.yres*0.8,-2.25,2.25);
                g.a_3--;
                cd_3 = 3;
            }
            cd_3--;
        }

        // Expaning explosion 
        if (g.a_4 > 1) { 
            if(cd_4 == 0){
                expl_360(16,2,2,2,random(g.xres - 200) + 100, g.yres*0.8,5);
                g.a_4--;
                cd_4 = 10;
            }
            cd_4--;
        }

        // Homing explosion
        if (g.a_5 > 1) { 
            if (cd_5 == 0) {
                make_particle(3,4,4,g.xres*0.5,g.yres*0.8,0,0);
                g.a_5--;
                cd_5 = 1;
            }
            cd_5--;
        }

        //Homing explosion
        if (g.a_6 > 1) { 
            if (cd_6 == 0) {
                expl_360(64,8,2,2,g.xres*0.5, g.yres*0.8,10);
                g.a_6--;
                cd_6 = 1;
            }
            cd_6--;
        }

        //Homing explosion
        if (g.a_7 > 1) {   
            spiral_360(spiral,99,5,5,g.xres*0.5, g.yres*0.8,5);
            spiral += 6;
            g.a_7--;
        }

    }

}

void screen_write(Rect r1)
{
    char words[5][100] = {"curr att:" , "times hit:", "Score:", "" , ""};
    char onscreen[6][25] = {"Lives:","Score:","Dash:"
	,"Teleport:","Repel:","Shield:"};
    char cstr [20][9];	

    //Create a char string, and give it the int value to char of a global
    sprintf(cstr[0], "%d" , g.n);
    strcat(words[0], cstr[0]);

    sprintf(cstr[1], "%d" , g.w);
    strcat(words[1], cstr[1]);

    sprintf(cstr[2], "%d" , g.att_count);
    strcat(words[2], cstr[2]);

    sprintf(cstr[3], "%d" , (10 - g.w));
    strcat(onscreen[0], cstr[3]);

    sprintf(cstr[4], "%d" , g.att_count);
    strcat(onscreen[1], cstr[4]);

    sprintf(cstr[5], "%d" , g.dash);
    strcat(onscreen[2], cstr[5]);

    sprintf(cstr[6], "%d" , g.teleport);
    strcat(onscreen[3], cstr[6]);

    sprintf(cstr[7], "%d" , g.repel);
    strcat(onscreen[4], cstr[7]);

    sprintf(cstr[8], "%d" , g.shield);
    strcat(onscreen[5], cstr[8]);

    r1.bot = g.yres - g.yres*0.05;
    r1.left = 0.1*g.xres;
    r1.center = 0;

    ggprint8b(&r1,16,0x00ff0000, onscreen[0]);
    ggprint8b(&r1,16,0x00ff0000, onscreen[1]);
    ggprint8b(&r1,16,0x00ff0000, onscreen[2]);
    ggprint8b(&r1,16,0x00ff0000, onscreen[3]);
    ggprint8b(&r1,16,0x00ff0000, onscreen[4]);
    ggprint8b(&r1,16,0x00ff0000, onscreen[5]);

    if (g.s != 1) {
	r1.bot = g.yres/2;
	r1.left = g.xres/2;
	r1.center = -10;
	if (g.w >= 10)
	    ggprint8b(&r1,16,0x00ff0000,"Score: %d",g.att_count);
	else
	    ggprint8b(&r1,16,0x00ff0000, "PAUSED");
    }

    r1.bot = g.yres - g.yres*0.05;
    r1.left = g.xres - g.xres*0.2;
    r1.center = 0;
    if (g.d == 1) {
	ggprint8b(&r1,16,0x00ff0000, words[0]);
	ggprint8b(&r1,16,0x00ff0000, words[1]);
	ggprint8b(&r1,16,0x00ff0000, words[2]);
	ggprint8b(&r1,16,0x00ff0000, "DEBUG ON");
	ggprint8b(&r1,16,0x00ff0000, "1 - 7 for attacks");
	ggprint8b(&r1,16,0x00ff0000, "s to stop time");
	ggprint8b(&r1,16,0x00ff0000, "placeholder");
    }
}
