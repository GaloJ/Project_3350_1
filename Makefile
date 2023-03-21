all: xylab2 

xylab2: xylab2.cpp gjimenez4.cpp global.h
	g++ xylab2.cpp gjimenez4.cpp aynostroza.cpp libggfonts.a fonts.h -Wall -lX11 -lGL -lGLU -lm -oxylab2

clean:
	rm -f xylab2 

