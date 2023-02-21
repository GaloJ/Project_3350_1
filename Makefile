all: xylab2 aynostroza

xylab2: xylab2.cpp
	g++ xylab2.cpp libggfonts.a fonts.h -Wall -oxylab2 -lX11 -lGL -lGLU -lm

aynostroza: aynostroza.cpp
	g++ aynostroza.cpp -Wall -oaynostroza

clean:
	rm -f xylab2 aynostroza

