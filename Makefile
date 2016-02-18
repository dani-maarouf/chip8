CC=gcc
CFLAGS=-Iinclude -std=c99 -Wall

chip8: src/main.c
	$(CC) $(CFLAGS) src/main.c -o bin/chip8
