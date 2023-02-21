
all: xylab2

xylab2: xylab2.cpp
	g++ xylab2.cpp libggfonts.a fonts.h -Wall -oxylab2 -lX11 -lGL -lGLU -lm


clean:
	rm -f xylab2

