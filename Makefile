CFLAGS=-Iinclude -std=c99 -Wall -Ofast
SRC_FILES = src/main.c src/chip8.c src/gameLoop.c
HEADER_FILES = include/chip8.h include/gameLoop.h

all: linux

linux: $(SRC_FILES) $(HEADER_FILES)
	gcc $(CFLAGS) $(SRC_FILES) -o chip8 -lSDL2 -lalut -lopenal

mac: 
	gcc -Iinclude -I/usr/local/include -std=c99 -Wall -Ofast src/main.c src/chip8.c src/gameLoop.c -o chip8 -L/usr/local/lib -lSDL2 -lalut -lopenal

windows:
	x86_64-w64-mingw32-gcc $(CFLAGS) $(SRC_FILES) -o chip8.exe -lmingw32 -lSDL2main -lSDL2 -lalut -lopenal
