CC=gcc
CFLAGS=-Iinclude -std=c99 -Wall

chip8: src/main.c src/readFile.c
	$(CC) $(CFLAGS) src/main.c src/readFile.c -o bin/chip8
