#include <stdio.h>

#include "chip8.h"

static int readFile(const char *, uint8_t *, int);
static void readSpritesIntoRAM(uint8_t *, int);

struct chip8System chip8Init(char * fileLoc, int startPC, int startSprites) {

    struct chip8System chip8Sys;

    for (int x = 0; x < 0x1000; x++) chip8Sys.RAM[x] = 0x0;

    for (int x = 0; x < 16; x++) {
        chip8Sys.V[x] = 0x0;
        chip8Sys.stack[x] = 0x0;
        chip8Sys.key[x] = false;
    }

    for (int x = 0; x < 32; x++) {
        for (int y = 0; y < 64; y++) {
            chip8Sys.display[x][y] = false;
        }
    }

    chip8Sys.I = 0x0;
    chip8Sys.DT = 0x0;
    chip8Sys.ST = 0x0;
    chip8Sys.PC = startPC;
    chip8Sys.SP = 0x0;

    int readResult;
    readResult = readFile(fileLoc, chip8Sys.RAM, chip8Sys.PC);

    if (!readResult) {

        
    }

    readSpritesIntoRAM(chip8Sys.RAM, startSprites);

    return chip8Sys;

}

static int readFile(const char * fileLocation, uint8_t * RAM, int startLoc) {

    if (fileLocation == NULL) {
        perror("readFile() -> Error: File location string is NULL\n");
        return 0;
    }

    FILE * romFile;
    romFile = fopen(fileLocation, "rb");
    if (romFile == NULL) {
        perror("readFile() -> Error: Could not open ROM file\n");
        return 0;
    }

    int readChar;
    int index;

    index = startLoc;

    while ((readChar = fgetc(romFile)) != EOF && index < 0x1000) {
        RAM[index] = readChar;
        index++;
    }

    return 1;
}

static void readSpritesIntoRAM(uint8_t * RAM, int start) {

    uint8_t sprites[5 * 16] = { 0xF0, 0x90, 0x90, 0x90, 0xF0,
                                0x20, 0x60, 0x20, 0x20, 0x70,
                                0xF0, 0x10, 0xF0, 0x80, 0xF0,
                                0xF0, 0x10, 0xF0, 0x10, 0xF0,
                                0x90, 0x90, 0xF0, 0x10, 0x10,
                                0xF0, 0x80, 0xF0, 0x10, 0xF0,
                                0xF0, 0x10, 0x20, 0x40, 0x40,
                                0xF0, 0x90, 0xF0, 0x90, 0xF0,
                                0xF0, 0x90, 0xF0, 0x10, 0xF0,
                                0xF0, 0x90, 0xF0, 0x90, 0x90,
                                0xE0, 0x90, 0xE0, 0x90, 0xE0,
                                0xF0, 0x80, 0x80, 0x80, 0xF0,
                                0xE0, 0x90, 0x90, 0x90, 0xE0,
                                0xF0, 0x80, 0xF0, 0x80, 0xF0,
                                0xF0, 0x80, 0xF0, 0x80, 0x80};

    for (int x = 0; x < 80; x++) {
        RAM[x + start] = sprites[x];
    }

    return;
}


