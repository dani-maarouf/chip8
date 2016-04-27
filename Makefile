CC=gcc
CFLAGS=-Iinclude -std=c99 -Wall

chip8: src/main.c src/chip8.c
	$(CC) $(CFLAGS) src/main.c src/chip8.c -o bin/chip8 -lSDL
