#include <iostream>
using namespace std;
#include <stdio.h>

const int MAX_PARTICLES = 10000;

class Global {
    public:
	int xres, yres;
	char keys[65536];
	int att; //Attack toogle
	int n; // Particle number
	int w; // times hit or lives
	int s; // time stop
	int d;// debug mode
	int done; // check if game is done
	int plyr_decel;
    int curr_att;
	int att_count;
	int difficulty;
	int a_1,a_2,a_3,a_4, a_5, a_6, a_7;

	Global()
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
	    plyr_decel = 0.75; // 0 is full stop, 1 is no decel
	    curr_att = 10; //Number of attacks left per attack cycle
	    att_count = 1; // Must be 1 or all attacks proc at the start
	    difficulty = 50;
	    memset(keys, 0, 65536);
	}

};
