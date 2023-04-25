//Box.h
#include <iostream>
using namespace std;
#include <stdio.h>

class Box {
    public:
        int t;
        float w,h;
        float pos[2];
        float pos_i[2];
        float vel[2];
	float color[3];
        //unsigned char color[3];
        /*void set_color(unsigned char col[3]){
            memcpy(color,col,sizeof(unsigned char) * 3);
        }*/
        Box();
	Box(int, float, float, float, float , float ,float);
};
