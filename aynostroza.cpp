//Arjun Ynostroza
//aynostroza.cpp
//3350 project

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
#include "image.h"

extern Global g;
extern Box box;
extern Box particle[MAX_PARTICLES];

extern void make_particle(int,int,int,float,float,float,float);

Image::~Image()
{
    delete [] data;
}

Image::Image(const char *fname)
{
    if (fname[0] == '\0')
	return;
    ppmFlag = 0;
    strcpy(name, fname);
    slen = strlen(name);
    if (strncmp(name+(slen-4), ".ppm", 4) == 0)
	ppmFlag = 1;
    if (ppmFlag) 
    {
	strcpy(ppmname, name);
    } else {
	name[slen-4] = '\0';
	sprintf(ppmname,"%s.ppm", name);
	char ts[100];
	sprintf(ts, "convert %s %s", fname, ppmname);
	system(ts);
    }
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
	printf("ERROR opening image: %s\n",ppmname);
	exit(0);
    }
    if (!ppmFlag)
	unlink(ppmname);
}

void background_debug(void)
{
    g.debug ^= 1;
}

void change_background()
{
    if (g.s == 1)
    {
	static int current_background = 0;
	const char *backgrounds[] = {"background1.png", "background2.png", "background3.png"};
	int num_backgrounds = sizeof(backgrounds) / sizeof(backgrounds[0]);

	current_background = (current_background + 1) % num_backgrounds;
	Image new_background(backgrounds[current_background]);

	glGenTextures(1, &g.texture);
	glBindTexture(GL_TEXTURE_2D, g.texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, new_background.width, new_background.height, 0,
		GL_RGB, GL_UNSIGNED_BYTE, new_background.data);
	glBindTexture(GL_TEXTURE_2D, 0); 
    }
}

void spawn_powerup(Box &powerup) 
{
    if (g.s == 1)
    {
	srand(time(NULL));

	// Random position within the screen
	float x = static_cast<float>(rand() % g.xres);
	float y = static_cast<float>(rand() % g.yres);

	// Power up size
	float width = 20.0f;
	float height = 20.0f;

	powerup = Box(2, width, height, x, y, 0.0f, 0.0f);
	powerup.color[0] = 0.0f; 
	powerup.color[1] = 1.0f; 
	powerup.color[2] = 0.0f;
    }
}

void lose_animation() 
{
    if (g.w < 10) 
    {
	return;
    }

    float velocity = 5.0f;
    float color_change_speed = 0.01f;

    box.color[0] += color_change_speed;
    if (box.color[0] > 1.0f) 
    {
	box.color[0] = 1.0f;
	box.color[1] += color_change_speed;
	if (box.color[1] > 1.0f) 
	{
	    box.color[1] = 1.0f;
	    box.color[2] += color_change_speed;
	    if (box.color[2] > 1.0f) 
	    {
		box.color[2] = 1.0f;
	    }
	}
    }
    // Drift downward
    box.pos[1] -= velocity;
    make_particle(4,4,4,g.xres*0.5,g.yres*0.8,10,20);
    make_particle(4,4,4,g.xres*0.5,g.yres*0.8,-10,20);
}
