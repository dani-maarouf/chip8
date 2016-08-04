#include <stdio.h>
#include <stdlib.h>

#include "chip8.h"

const int MAX_INT = 0xFFFFFFFF;

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

int processNextOpcode(struct chip8System * chip8, int incrementI, int debug) {

    uint16_t opcode = (chip8->RAM[chip8->PC] << 8) | chip8->RAM[chip8->PC + 1];
    int nybble1 = ((opcode & 0xF000) >> 12);
    int nybble2 = ((opcode & 0x0F00) >> 8);
    int nybble3 = ((opcode & 0x00F0) >> 4);
    int nybble4 = ((opcode & 0x000F));
    bool draw = false;

    if (debug) {
        printf("0x%x\t%x%x%x%x, I:%x, SP:%x ", chip8->PC, nybble1, nybble2, 
                       nybble3, nybble4, chip8->I, chip8->SP);

        //for (int x = 0; x < 16; x++) {
        //    printf("V%x: %2x ", x, chip8->V[x]);
        //}
        printf("\n");
    }


    switch (nybble1) {

        case 0: {

            switch((opcode & 0x0FFF)) {

                case 0x0E0:     //CLS
                for (int i = 0; i < C8_WIDTH * C8_HEIGHT; i++) {
                    chip8->display[i] = 0;
                }
                chip8->PC += 2;
                draw = true;
                break;

                case 0x0EE:     //RET
                chip8->SP -= 1;
                chip8->PC = chip8->stack[chip8->SP];
                chip8->PC += 2;
                break;

                default:
                printf("Illegal opcode 0x%x\n", opcode);
                exit(1);
            }


            break;
        }
        case 1:     //JP addr
            if ((opcode & 0x0FFF) == chip8->PC) {
                //endless loop
                return 0;
            }
            chip8->PC = (opcode & 0x0FFF);
            break;
            
        case 2:     //CALL addr
            chip8->stack[chip8->SP] = chip8->PC;
            chip8->SP += 1;
            chip8->PC = (opcode & 0x0FFF);
            break;

        case 3:     //SE Vx, byte
            if (chip8->V[nybble2] == (opcode & 0xFF)) {
                chip8->PC += 4;
            } else {
                chip8->PC += 2;
            }
            break;

        case 4:     //SNE Vx, byte
            if (chip8->V[nybble2] != (opcode & 0xFF)) {
                chip8->PC += 4;
            } else {
                chip8->PC += 2;
            }
            break;

        case 5:     //SE Vx, Vy
            if (chip8->V[nybble2] == chip8->V[nybble3]) {
                chip8->PC += 4;
            } else {
                chip8->PC += 2;
            }
            break;

        case 6:     //LD Vx, byte
            chip8->V[nybble2] = ((opcode & 0xFF));
            chip8->PC += 2;
            break;

        case 7:     //ADD Vx, byte
            chip8->V[nybble2] = ((opcode & 0xFF) + chip8->V[nybble2]) & 0xFF;
            chip8->PC += 2;
            break;

        case 8: {

            switch(nybble4) {

                case 0: //LD Vx, Vy
                    chip8->V[nybble2] = chip8->V[nybble3];
                    chip8->PC += 2;
                    break;
                case 1: //OR Vx, Vy
                    chip8->V[nybble2] |= chip8->V[nybble3];
                    chip8->PC += 2;
                    break;

                case 2: //AND Vx, Vy
                    chip8->V[nybble2] &= chip8->V[nybble3];
                    chip8->PC += 2;
                    break;

                case 3: //XOR Vx, Vy
                    chip8->V[nybble2] ^= chip8->V[nybble3];
                    chip8->PC += 2;
                    break;

                case 4: {   //ADD Vx, Vy

                    unsigned int result = (int) chip8->V[nybble2] + (int) chip8->V[nybble3];

                    if (result > 256) {
                        chip8->V[0xF] = 1;
                    } else {
                        chip8->V[0xF] = 0;
                    }
                    chip8->V[nybble2] = (chip8->V[nybble2] + chip8->V[nybble3]) & 0xFF;
                    chip8->PC += 2;
                    break;
                }
                case 5: {   //SUB Vx, Vy

                    int result;
                    result = (int) chip8->V[nybble2] - (int) chip8->V[nybble3];

                    if (result >= 0) {
                        chip8->V[0xF] = 1;
                        chip8->V[nybble2] = (chip8->V[nybble2] - chip8->V[nybble3]) & 0xFF;
                    } else {
                        chip8->V[0xF] = 0;
                        chip8->V[nybble2] = (chip8->V[nybble2] - chip8->V[nybble3]) & 0xFF;
                    }
                    chip8->PC += 2;
                    break;
                }
                case 6: //SHR Vx {, Vy}
                    
                    chip8->V[0xF] = (chip8->V[nybble2] & 0x01) ? 1 : 0;
                    chip8->V[nybble2] = chip8->V[nybble2] >> 1;
                    chip8->PC += 2;
                    break;

                case 7: {   //SUBN Vx, Vy

                    int result = (int) chip8->V[nybble3] - (int) chip8->V[nybble2];

                    if (result >= 0) {
                        chip8->V[0xF] = 1;
                        chip8->V[nybble2] = (chip8->V[nybble3] - chip8->V[nybble2]) & 0xFF;
                    } else {
                        chip8->V[0xF] = 0;
                        chip8->V[nybble2] = (chip8->V[nybble3] - chip8->V[nybble2]) & 0xFF;
                    }
                    chip8->PC += 2;
                    
                    break;

                }
                case 0xE:   //SHL Vx {, Vy}
                    chip8->V[0xF] = (chip8->V[nybble2] & 0x80) ? 1 : 0;
                    chip8->V[nybble2] = chip8->V[nybble2] << 1;
                    chip8->PC += 2;
                    break;

                default:
                printf("Illegal opcode 0x%x\n", opcode);
                exit(1);
            }

            break;
        }

        case 9: //SNE Vx, Vy
            if (chip8->V[nybble2] != chip8->V[nybble3]) {
                chip8->PC += 4;
            } else {
                chip8->PC += 2;
            }
            break;

        case 0xA:   //LD I, addr
            chip8->I = (opcode & 0x0FFF);
            chip8->PC += 2;
            break;

        case 0xB:   //JP V0, addr
            chip8->PC = ((opcode & 0x0FFF) + chip8->V[0]);
            break;

        case 0xC: { //RND Vx, byte
            uint8_t randomNum = rand() % 256;
            chip8->V[nybble2] = ((opcode & 0xFF) & randomNum);
            chip8->PC += 2;
            break;
        }
        case 0xD:   //DRW Vx, Vy, nibble

            chip8->V[0xF] = 0;

            for (int i = 0; i < nybble4; i++) {

                unsigned int spriteRow = chip8->RAM[chip8->I + i];

                for (int j = 0; j < 8; j++) {
                    if ((spriteRow & (0x80 >> j)) != 0) {
                        
                        int yPos = chip8->V[nybble3];
                        int xPos = chip8->V[nybble2];
                        int pixelLocation = ((yPos + i) * C8_WIDTH + xPos + j);

                        if (pixelLocation < C8_WIDTH * C8_HEIGHT) {

                            if (chip8->display[pixelLocation] == 0) {
                                chip8->display[pixelLocation] = MAX_INT;
                            } else {
                                chip8->display[pixelLocation] = 0;
                                chip8->V[0xF] = 1;
                            }

                        }
                    }
                }
            }
            draw = true;
            chip8->PC += 2;
            break;

        case 0xE:

            if (nybble3 == 0x9) {   //SKP Vx
                if (chip8->key[chip8->V[nybble2]]) {
                    chip8->PC += 4;
                } else {
                    chip8->PC += 2;
                }
            } else if (nybble3 == 0xA) {    //SKNP Vx
                if (!chip8->key[chip8->V[nybble2]]) {
                    chip8->PC += 4;
                } else {
                    chip8->PC += 2;
                }
            }
            break;

        case 0XF: {


            switch((opcode & 0xFF)) {

                case 0x07:  //LD Vx, DT
                chip8->V[nybble2] = chip8->DT;
                chip8->PC += 2;
                break;

                case 0x0A:  //LD Vx, K
                for (int x = 0; x < 0x10; x++) {
                    if (chip8->key[x]) {
                        chip8->V[nybble2] = x;
                        chip8->PC += 2;
                        break;
                    }
                }
                return 3;

                case 0x15:  //LD DT, Vx
                chip8->DT = chip8->V[nybble2];
                chip8->PC += 2;
                break;

                case 0x18:  //LD ST, Vx
                chip8->ST = chip8->V[nybble2];
                chip8->PC += 2;
                break;

                case 0x1E:  //ADD I, Vx
                chip8->I = (chip8->V[nybble2] + chip8->I) & 0xFFFF;
                chip8->PC += 2;
                break;

                case 0x29:  //LD F, Vx
                chip8->I = 0x50 + (chip8->V[nybble2] * 5);
                chip8->PC += 2;
                break;

                case 0x33:  //LD B, Vx
                chip8->RAM[chip8->I] = chip8->V[nybble2] / 100;
                chip8->RAM[chip8->I + 1] = ((chip8->V[nybble2]) % 100) / 10;
                chip8->RAM[chip8->I + 2] = chip8->V[nybble2] % 10;
                chip8->PC += 2;
                break;

                case 0x55:  //LD [I], Vx
                for (int x = 0; x <= nybble2; x++) {
                    chip8->RAM[chip8->I + x] = chip8->V[x];
                }

                if (incrementI == 1) {
                    chip8->I += nybble2;
                } else if (incrementI == 2) {
                    chip8->I += nybble2 + 1;
                }

                chip8->PC += 2;
                break;

                case 0x65:  //LD Vx, [I]
                for (int x = 0; x <= nybble2; x++) {
                    chip8->V[x] = chip8->RAM[chip8->I + x];
                }
                if (incrementI == 1) {
                    chip8->I += nybble2;
                } else if (incrementI == 2) {
                    chip8->I += nybble2 + 1;
                }
                chip8->PC += 2;
                break;

                default:
                printf("Illegal opcode 0x%x\n", opcode);
                exit(1);
            }

            break;
        }

        default:
            printf("Illegal opcode 0x%x\n", opcode);
            exit(1);
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
