#include <iostream>
#include<stdio.h>
using namespace std;

class Menu_box {
    public:
        float w,h;
        float pos[1];
        unsigned char color[2];
        void color_set(unsigned char col[2]){
                memcpy(color, col, sizeof(unsigned char) * 3);
        }
};
