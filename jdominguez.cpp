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
	unsigned char color[2];
	void color_set(unsigned char col[2] * 2){
		memcpy(color, col, sizeof(unsigned char) * 3);
	}
	Menu_box(float wid, float hgt, float x, float y){
	    w = wid;
	    h = hgt;
	    pos[0] = x;
	    pos[1] = y;
	}
}
