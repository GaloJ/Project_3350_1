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
	GLuint texture;

	
	Global();
};

/*class Texture {
    public:
	    Image *backImage;
	    GLuint backTexture;
	    float xc[2];
	    float yc[2];
};
*/

/*class Image {
    public:
	    int width, height;
	    unsigned char *data;
	    ~Image() { delete [] data; }
	    Image(const char *fname) {
		    if (fname[0] == '\0')
			    return;
		    char name[40];
		    strcpy(name, fname);
		    int slen = strlen(name);
		    name[slen-4] = '\0';
		    char ppmname[80];
		    sprintf(ppmname,"%s.ppm", name);
		    char ts[100];
		    sprintf(ts, "convert %s %s", fname, ppmname);
		    system(ts);
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
			    printf("ERROR opening image: %s\n", ppmname);
			    exit(0);
		    }
		    unlink(ppmname);
	    }
};
Image img[1] = {"background.gif"};
*/


