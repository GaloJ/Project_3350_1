#include <iostream>
using namespace std;
#include <stdio.h>

const int MAX_PARTICLES = 10000;

class Global {
    public:
	int xres, yres;
	char keys[65536];
	int n;
	int w;
	int s;
	int plyr_decel;
        int curr_att;
	int att_count;
	int difficulty;

	Global()
	{
	    xres = 640;
	    yres = 960;
	    n = 0; // Number of particle 
	    w = 0; // Times hit 
	    s = 1; // Time stop
	    plyr_decel = 0.75; // 0 is full stop, 1 is no decel
	    curr_att = 10; //Number of attacks left per attack cycle
	    att_count = 0;
	    difficulty = 50;
	    memset(keys, 0, 65536);
	}

};
