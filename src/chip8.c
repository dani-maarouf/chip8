#include <stdio.h>
#include <stdlib.h>

#include "chip8.h"

const int MAX_INT = 4294967295;

static int readFile(const char *, uint8_t *, int);
static void readSpritesIntoRAM(uint8_t *, int);

struct chip8System * chip8Init(char * fileLoc) {

    if (fileLoc == NULL) {
        return NULL;
    }

    struct chip8System * chip8Sys;
    chip8Sys = malloc(sizeof(struct chip8System));

    if (chip8Sys == NULL) {
        return NULL;
    }

    for (int x = 0; x < 0x1000; x++) chip8Sys->RAM[x] = 0x0;

    for (int x = 0; x < 16; x++) {
        chip8Sys->V[x] = 0x0;
        chip8Sys->stack[x] = 0x0;
        chip8Sys->key[x] = false;
    }

    chip8Sys->display = malloc(sizeof(uint32_t) * C8_WIDTH * C8_HEIGHT);

    if (chip8Sys->display == NULL) {
        free(chip8Sys);
        return NULL;
    }

    for (int x = 0; x < C8_WIDTH * C8_HEIGHT; x++) {
        chip8Sys->display[x] = 0;
    }

    chip8Sys->I = 0x0;
    chip8Sys->DT = 0x0;
    chip8Sys->ST = 0x0;
    chip8Sys->PC = 0x200;   //this may need to be different for some applications
    chip8Sys->SP = 0x0;

    int readResult;
    readResult = readFile(fileLoc, chip8Sys->RAM, chip8Sys->PC);

    if (!readResult) {
        free(chip8Sys);
        return NULL;
    }

    readSpritesIntoRAM(chip8Sys->RAM, 0x50);

    return chip8Sys;

}

void destroyChip8System(struct chip8System * chip8) {
    free(chip8->display);
    free(chip8);
    return;
}

void decrementC8Counters(struct chip8System * chip8) {

    if (chip8->DT != 0) chip8->DT -= 1;
    if (chip8->ST != 0) chip8->ST -= 1;
    
    return;
}

int processNextOpcode(struct chip8System * chip8, int incrementI) {

    int opcodeDigits[4] = {chip8->RAM[chip8->PC] / 16, chip8->RAM[chip8->PC] % 16,
                                chip8->RAM[chip8->PC + 1] / 16, chip8->RAM[chip8->PC + 1] % 16};
    bool draw = false;

    /*
    printf("Executed opcode %x%x%x%x, PC:%x, I:%x, SP:%x ", opcodeDigits[0], opcodeDigits[1], 
                   opcodeDigits[2], opcodeDigits[3], chip8->PC, chip8->I, chip8->SP);

    for (int x = 0; x < 16; x++) {
        printf("V%x: %2x ", x, chip8->V[x]);
    }
    printf("\n");
    */

    switch (opcodeDigits[0]) {
        case 0: {

            if (opcodeDigits[1] == 0) {
                if (opcodeDigits[2] == 0xE && opcodeDigits[3] == 0x0) {

                    for (int i = 0; i < C8_WIDTH * C8_HEIGHT; i++) {
                        chip8->display[i] = 0;
                    }

                    chip8->PC += 2;
                    draw = true;


                } else if (opcodeDigits[2] == 0xE && opcodeDigits[3] == 0xE) {
                   
                   chip8->SP -= 1;
                   chip8->PC = chip8->stack[chip8->SP];
                   chip8->PC += 2;
                    
                } else {
                    printf("Unrecognized opcode %x%x%x%x\n", opcodeDigits[0], 
                        opcodeDigits[1], opcodeDigits[2], opcodeDigits[3]);
                    exit(1);
                    return 0;
                }

            } else {
                printf("Illegal opcode 0NNN\n");
                exit(1);
                chip8->PC += 2;
            }
            break;
        }
        case 1:
            if (opcodeDigits[1] * 0x100 + opcodeDigits[2] * 0x10 + opcodeDigits[3] == chip8->PC) {
                //endless loop
                return 0;
            }
            chip8->PC = opcodeDigits[1] * 0x100 + opcodeDigits[2] * 0x10 + opcodeDigits[3];
            break;
            
        case 2:

            chip8->stack[chip8->SP] = chip8->PC;
            chip8->SP += 1;
            chip8->PC = opcodeDigits[1] * 0x100 + opcodeDigits[2] * 0x10 + opcodeDigits[3];
            break;

        case 3:

            if (chip8->V[opcodeDigits[1]] == opcodeDigits[2] * 0x10 + opcodeDigits[3]) {
                chip8->PC += 4;
            } else {
                chip8->PC += 2;
            }
            break;

        case 4:

            if (chip8->V[opcodeDigits[1]] != opcodeDigits[2] * 0x10 + opcodeDigits[3]) {
                chip8->PC += 4;
            } else {
                chip8->PC += 2;
            }
            break;

        case 5:

            if (chip8->V[opcodeDigits[1]] == chip8->V[opcodeDigits[2]]) {
                chip8->PC += 4;
            } else {
                chip8->PC += 2;
            }
            break;

        case 6:

            chip8->V[opcodeDigits[1]] = opcodeDigits[2] * 0x10 + opcodeDigits[3];
            chip8->PC += 2;
            break;

        case 7:

            chip8->V[opcodeDigits[1]] = (opcodeDigits[2] * 0x10 + opcodeDigits[3] + chip8->V[opcodeDigits[1]]) & 0xFF;
            chip8->PC += 2;
            break;

        case 8: {

            switch(opcodeDigits[3]) {

                case 0:
                    chip8->V[opcodeDigits[1]] = chip8->V[opcodeDigits[2]];
                    chip8->PC += 2;
                    break;
                case 1:
                    chip8->V[opcodeDigits[1]] |= chip8->V[opcodeDigits[2]];
                    chip8->PC += 2;
                    break;

                case 2:
                    chip8->V[opcodeDigits[1]] &= chip8->V[opcodeDigits[2]];
                    chip8->PC += 2;
                    break;

                case 3:
                    chip8->V[opcodeDigits[1]] ^= chip8->V[opcodeDigits[2]];
                    chip8->PC += 2;
                    break;

                case 4: {

                    unsigned int result;
                    result = (int) chip8->V[opcodeDigits[1]] + (int) chip8->V[opcodeDigits[2]];

                    if (result >= 255) {
                        chip8->V[0xF] = 1;
                        chip8->V[opcodeDigits[1]] = (chip8->V[opcodeDigits[1]] + chip8->V[opcodeDigits[2]]) & 0xFF;
                    } else {
                        chip8->V[0xF] = 0;
                        chip8->V[opcodeDigits[1]] = (chip8->V[opcodeDigits[1]] + chip8->V[opcodeDigits[2]]) & 0xFF;
                    }
                    chip8->PC += 2;
                    break;
                }
                case 5: {

                    int result;
                    result = (int) chip8->V[opcodeDigits[1]] - (int) chip8->V[opcodeDigits[2]];

                    if (result >= 0) {
                        chip8->V[0xF] = 1;
                        chip8->V[opcodeDigits[1]] = (chip8->V[opcodeDigits[1]] - chip8->V[opcodeDigits[2]]) & 0xFF;
                    } else {
                        chip8->V[0xF] = 0;
                        chip8->V[opcodeDigits[1]] = (chip8->V[opcodeDigits[1]] - chip8->V[opcodeDigits[2]]) & 0xFF;
                    }
                    chip8->PC += 2;
                    break;
                }
                case 6:
                    
                    chip8->V[0xF] = (chip8->V[opcodeDigits[2]] & 0x01) ? 1 : 0;
                    chip8->V[opcodeDigits[1]] = chip8->V[opcodeDigits[2]] >> 1;
                    chip8->PC += 2;
                    break;

                case 7: {

                    int result = (int) chip8->V[opcodeDigits[2]] - (int) chip8->V[opcodeDigits[1]];

                    if (result >= 0) {
                        chip8->V[0xF] = 1;
                        chip8->V[opcodeDigits[1]] = (chip8->V[opcodeDigits[2]] - chip8->V[opcodeDigits[1]]) & 0xFF;
                    } else {
                        chip8->V[0xF] = 0;
                        chip8->V[opcodeDigits[1]] = (chip8->V[opcodeDigits[2]] - chip8->V[opcodeDigits[1]]) & 0xFF;
                    }
                    chip8->PC += 2;
                    
                    break;
                }
                case 0xE:

                    chip8->V[0xF] = (chip8->V[opcodeDigits[2]] & 0x80) ? 1 : 0;

                    chip8->V[opcodeDigits[1]] = chip8->V[opcodeDigits[2]] << 1;

                    chip8->PC += 2;
                    break;
            }

            break;
        }

        case 9:

            if (chip8->V[opcodeDigits[1]] != chip8->V[opcodeDigits[2]]) {
                chip8->PC += 4;
            } else {
                chip8->PC += 2;
            }
            break;

        case 0xA:
            chip8->I = (opcodeDigits[1] * 0x100) + (opcodeDigits[2] * 0x10) + opcodeDigits[3];
            chip8->PC += 2;
            break;

        case 0xB:

            chip8->PC = opcodeDigits[1] * 0x100 + opcodeDigits[2] * 0x10 + opcodeDigits[3] + chip8->V[0];
            break;

        case 0xC: {

            uint8_t randomNum;
            randomNum = rand() % 256;

            chip8->V[opcodeDigits[1]] = (opcodeDigits[2] * 0x10 + opcodeDigits[3]) & randomNum;
            chip8->PC += 2;

            break;
        }
        case 0xD:

            for (int i = 0; i < opcodeDigits[3]; i++) {

                unsigned int spriteRow = chip8->RAM[chip8->I + i];

                chip8->V[0xF] = 0;

                for (int j = 0; j < 8; j++) {
                    if ((spriteRow & (0x80 >> j)) != 0) {
                        
                        int yPos = chip8->V[opcodeDigits[2]];
                        int xPos = chip8->V[opcodeDigits[1]];

                        int pixelLocation;
                        pixelLocation = ((yPos + i) * C8_WIDTH + xPos + j);

                        if (pixelLocation < C8_WIDTH * C8_HEIGHT) {

                            if (chip8->display[pixelLocation] != 0) {
                                chip8->V[0xF] = 1;
                            }

                            if (chip8->display[pixelLocation] == 0) {
                                chip8->display[pixelLocation] = MAX_INT;
                            } else {
                                chip8->display[pixelLocation] = 0;
                            }

                        }


                    }
                }
            }
            draw = true;
            chip8->PC += 2;
            break;

        case 0xE:



            if (opcodeDigits[2] == 0x9) {
                
                if (chip8->key[chip8->V[opcodeDigits[1]]]) {
                    chip8->PC += 4;
                } else {
                    chip8->PC += 2;
                }

            } else if (opcodeDigits[2] == 0xA) {

                if (!chip8->key[chip8->V[opcodeDigits[1]]]) {
                    chip8->PC += 4;
                } else {
                    chip8->PC += 2;
                }

            }
            break;

        case 0XF:

            switch(opcodeDigits[2]) {

                case 0:

                    if (opcodeDigits[3] == 7) {
                        chip8->V[opcodeDigits[1]] = chip8->DT;
                        chip8->PC += 2;
                    } else if (opcodeDigits[3] == 0xA) {

                        for (int x = 0; x < 0x10; x++) {
                            if (chip8->key[x]) {
                                chip8->V[opcodeDigits[1]] = x;
                                chip8->PC += 2;
                                break;
                            }
                        }
                        return 3;
                    }
                    break;

                case 1:
                    if (opcodeDigits[3] == 5) {
                        chip8->DT = chip8->V[opcodeDigits[1]];
                    } else if (opcodeDigits[3] == 8) {
                        chip8->ST = chip8->V[opcodeDigits[1]];
                    } else if (opcodeDigits[3] == 0xE) {
                        chip8->I = (chip8->V[opcodeDigits[1]] + chip8->I) & 0xFFFF;
                    }
                    chip8->PC += 2;
                    break;

                case 2:
                    //FX29
                    chip8->I = 0x50 + (chip8->V[opcodeDigits[1]] * 5);
                    chip8->PC += 2;
                    break;

                case 3: {

                    chip8->RAM[chip8->I] = chip8->V[opcodeDigits[1]] / 100;
                    chip8->RAM[chip8->I + 1] = ((chip8->V[opcodeDigits[1]]) % 100) / 10;
                    chip8->RAM[chip8->I + 2] = chip8->V[opcodeDigits[1]] % 10;

                    chip8->PC += 2;
                    break;
                }
                case 5:

                    for (int x = 0; x <= opcodeDigits[1]; x++) {

                        if (incrementI) {
                            chip8->RAM[chip8->I] = chip8->V[x];
                            chip8->I += 1;
                        } else {
                            chip8->RAM[chip8->I + x] = chip8->V[x];
                        }
                        

                    }

                    chip8->PC += 2;
                    break;

                case 6:
                    for (int x = 0; x <= opcodeDigits[1]; x++) {

                        if (incrementI) {
                            chip8->V[x] = chip8->RAM[chip8->I];
                            chip8->I += 1;
                        } else {
                            chip8->V[x] = chip8->RAM[chip8->I + x];
                        }
                        


                    }


                    chip8->PC += 2;
                    break;
            }

            break;

        default:
            printf("Warning: Unrecognized opcode %x%x%x%x\n", opcodeDigits[0], 
                    opcodeDigits[1], opcodeDigits[2], opcodeDigits[3]);
            exit(1);
            break;
    }

    if (draw) {
        return 2;
    } else {
        return 1;
    }
}

static int readFile(const char * fileLocation, uint8_t * RAM, int startLoc) {

    if (fileLocation == NULL) {
        return 0;
    }

    FILE * romFile;
    romFile = fopen(fileLocation, "rb");
    if (romFile == NULL) {
        return 0;
    }

    int readChar;
    int index;

    index = startLoc;

    while ((readChar = fgetc(romFile)) != EOF && index < 0x1000) {
        RAM[index] = readChar;
        index++;
    }

    fclose(romFile);
    return 1;
}

static void readSpritesIntoRAM(uint8_t * RAM, int start) {

    uint8_t sprites[5 * 16] = { 0xF0, 0x90, 0x90, 0x90, 0xF0,
                                0x20, 0x60, 0x20, 0x20, 0x70,
                                0xF0, 0x10, 0xF0, 0x80, 0xF0,
                                0xF0, 0x10, 0xF0, 0x10, 0xF0,
                                0x90, 0x90, 0xF0, 0x10, 0x10,
                                0xF0, 0x80, 0xF0, 0x10, 0xF0,
                                0xF0, 0x80, 0xF0, 0x90, 0xF0,
                                0xF0, 0x10, 0x20, 0x40, 0x40,
                                0xF0, 0x90, 0xF0, 0x90, 0xF0,
                                0xF0, 0x90, 0xF0, 0x10, 0xF0,
                                0xF0, 0x90, 0xF0, 0x90, 0x90,
                                0xE0, 0x90, 0xE0, 0x90, 0xE0,
                                0xF0, 0x80, 0x80, 0x80, 0xF0,
                                0xE0, 0x90, 0x90, 0x90, 0xE0,
                                0xF0, 0x80, 0xF0, 0x80, 0xF0,
                                0xF0, 0x80, 0xF0, 0x80, 0x80};

    for (int x = 0; x < 80; x++) RAM[x + start] = sprites[x];

    return;
}
