
all: chessprog

chessprog: chessprog.o brains.o minimax.o graphics.o
	gcc -o chessprog chessprog.o brains.o minimax.o graphics.o -std=c99 -pedantic-errors `sdl-config --libs` -g -lm -L/usr/local/lib/SDL_image-1.2.12/lib -lSDL_image

chessprog.o: chessprog.c
	gcc -std=c99 -pedantic-errors -c -Wall -g chessprog.c -lm `sdl-config --cflags` -I/usr/local/lib/SDL_image-1.2.12/include/SDL

brains.o: brains.c
	gcc -std=c99 -pedantic-errors -c -Wall -g brains.c -lm `sdl-config --cflags` -I/usr/local/lib/SDL_image-1.2.12/include/SDL

minimax.o: minimax.c
	gcc -std=c99 -pedantic-errors -c -Wall -g minimax.c -lm `sdl-config --cflags` -I/usr/local/lib/SDL_image-1.2.12/include/SDL

graphics.o: graphics.c
	gcc -std=c99 -pedantic-errors -c -Wall -g graphics.c -lm `sdl-config --cflags` -I/usr/local/lib/SDL_image-1.2.12/include/SDL

clean:
	-rm *.o chessprog