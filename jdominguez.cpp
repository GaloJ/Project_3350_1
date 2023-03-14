//jorge dominguez
#include <iostream>
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


class Menu_box {
    public:
	float w,h;
	float pos[1];
	float pos_i[1];
	unsigned char color[2];
	void color_set(unsigned char col[2] * 2){
		memcpy(color, col, sizeof(unsigned char) * 3);
	}
	Menu_box(){
	    t=1;
	    w = 20.0f;
	    h = 20.0f;
	    pos[0] = g.xres;
	    pos[1] = g.yres;
	    pos_i[0] = pos[0];
	    pos_i[1] = pos[1];
	}

	Box(int type, float wid, float hgt, float x, float y){
	    t = type;
	    w = wid;
	    h = hgt;
	    pos[0] = x;
	    pos[1] = y;
	    pos_i[0] = x;
	    pos_i[1] = y;	
	}


}
