all: main 

main: main.cpp gjimenez4.cpp aynostroza.cpp global.h
	g++ main.cpp gjimenez4.cpp aynostroza.cpp libggfonts.a fonts.h -Wall -lX11 -lGL -lGLU -lm -omain

clean:
	rm -f main

