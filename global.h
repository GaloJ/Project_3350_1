#include <iostream>
using namespace std;
#include <stdio.h>

const int MAX_PARTICLES = 10000;

class Global {
    public:
	int xres, yres;
	char keys[65536];
	int n;
	int f;
	int w;
	int s;
	int plyr_decel;
	int att_count;
	int difficulty;

	Global()
	{
	    xres = 640;
	    yres = 960;
	    n = 0; // Number of particle 
	    f = 0;
	    w = 0; // Times hit 
	    s = 1; // Time stop
	    plyr_decel = 0.75; // 0 is full stop, 1 is no decel
	    att_count = 0;
	    difficulty = 50;
	    memset(keys, 0, 65536);
	}

};
