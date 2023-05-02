//image.h
#include <iostream>
using namespace std;

class Image {
        public:
                int width, height;
                unsigned char *data;
                int ppmFlag;
                char name[40];
                int slen;
                char ppmname[80];
		~Image();
                Image(const char *fname);
};

