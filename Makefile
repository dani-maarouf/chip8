# x86_64-w64-mingw32-gcc -Iinclude -std=c99 src/main.c src/chip8.c -lmingw32 -lSDLmain -lSDL

CC=gcc
CFLAGS=-Iinclude -std=c99 -Wall

chip8: src/main.c src/chip8.c src/gameLoop.c
	$(CC) $(CFLAGS) src/main.c src/chip8.c src/gameLoop.c -Ofast -o bin/chip8 -lSDL
