//Global.h
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
	int tp; // Teleport 
	int d;// debug mode
	int done; // check if game is done
	int curr_att;
	int att_count;
	int difficulty;
	int a_1,a_2,a_3,a_4, a_5, a_6, a_7;
	int rep_ctr; // repel power up counter
	int debug;
	GLuint texture;

	
	Global();
};


