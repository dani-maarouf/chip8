#include <stdio.h>
#include <stdlib.h>
#include <SDL/SDL.h>
#include <time.h>

#include "chip8.h"

#define FPS 120

int processNextOpcode(struct chip8System *);
void runLoop(struct chip8System);

int main (int argc, char ** argv) {

    if (argc != 2) {
        perror("Please call program with 1 argument pointing to location of ROM\n");
        return 1;
    }

    struct chip8System chip8 = chip8Init(argv[1], 0x200, 0x50);

    srand(time(NULL));
    runLoop(chip8);

    return 0;
}


int processNextOpcode(struct chip8System * chip8) {

    int opcodeDigits[4] = {chip8->RAM[chip8->PC] / 16, chip8->RAM[chip8->PC] % 16,
                                chip8->RAM[chip8->PC + 1] / 16, chip8->RAM[chip8->PC + 1] % 16};
    bool draw = false;

    switch (opcodeDigits[0]) {
        case 0: {

            if (opcodeDigits[1]) {
               printf("Illegal opcode 0NNN\n");
            } else {

                if (opcodeDigits[2] == 0xE && opcodeDigits[3] == 0x0) {

                    for (int x = 0; x < 32; x++) {
                        for (int y = 0; y < 64; y++) {
                            chip8->display[x][y] = false;
                        }
                    }
                    chip8->PC += 2;


                } else if (opcodeDigits[2] == 0xE && opcodeDigits[3] == 0xE) {
                   
                   chip8->SP -= 1;
                   chip8->PC = chip8->stack[chip8->SP];
                   chip8->PC += 2;
                    
                } else {
                    
                    printf("Unrecognized opcode %x%x%x%x\n", opcodeDigits[0], 
                        opcodeDigits[1], opcodeDigits[2], opcodeDigits[3]);
                    return 0;
                    break;

                }

            }
            break;
        }
        case 1:
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

            chip8->V[opcodeDigits[1]] += opcodeDigits[2] * 0x10 + opcodeDigits[3];
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
                    if (result >= 0x100) {
                        chip8->V[0xF] = 1;
                        chip8->V[opcodeDigits[1]] = (result - 256);
                    } else {
                        chip8->V[0xF] = 0;
                        chip8->V[opcodeDigits[1]] = result;
                    }
                    chip8->PC += 2;
                    break;
                }
                case 5: {

                    int result;
                    result = (int) chip8->V[opcodeDigits[1]] - (int) chip8->V[opcodeDigits[2]];

                    if (result >= 0) {
                        chip8->V[0xF] = 1;
                        chip8->V[opcodeDigits[1]] = result;
                    } else {
                        chip8->V[0xF] = 0;
                        chip8->V[opcodeDigits[1]] = 256 + result;
                    }
                    chip8->PC += 2;
                    break;
                }
                case 6:
                    
                    chip8->V[0xF] = chip8->V[opcodeDigits[1]] & 1;
                    chip8->V[opcodeDigits[1]] = chip8->V[opcodeDigits[1]] >> 1;
                    chip8->PC += 2;
                    break;

                case 7: {

                    int result = (int) chip8->V[opcodeDigits[2]] - (int) chip8->V[opcodeDigits[1]];

                    if (result >= 0) {
                        chip8->V[0xF] = 0;
                        chip8->V[opcodeDigits[1]] = result;
                    } else {
                        chip8->V[0xF] = 1;
                        chip8->V[opcodeDigits[1]] = result + 256;
                    }
                    chip8->PC += 2;
                    
                    break;
                }
                case 0xE:

                    chip8->V[0xF] = chip8->V[opcodeDigits[1]] / 128;
                    chip8->V[opcodeDigits[1]] *= 2;
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

                        if (chip8->display[yPos + i][xPos + j] == 1) {
                            chip8->V[0xF] = 1;
                        }

                        chip8->display[yPos + i][xPos + j] ^= 1;

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
                    }
                    break;

                case 1:
                    if (opcodeDigits[3] == 5) {
                        chip8->DT = chip8->V[opcodeDigits[1]];
                    } else if (opcodeDigits[3] == 8) {
                        chip8->ST = chip8->V[opcodeDigits[1]];
                    } else if (opcodeDigits[3] == 0xE) {
                        chip8->I += chip8->V[opcodeDigits[1]];
                    }
                    chip8->PC += 2;
                    break;

                case 2:
                    
                    chip8->I = 0x50 + (chip8->V[opcodeDigits[1]] * 5);
                    chip8->PC += 2;
                    break;

                case 3:
 
                    chip8->RAM[chip8->I] = chip8->V[opcodeDigits[1]] / 100;
                    chip8->RAM[chip8->I + 1] = (chip8->V[opcodeDigits[1]] / 10) % 10;
                    chip8->RAM[chip8->I + 2] = chip8->V[opcodeDigits[1]] % 10;

                    chip8->PC += 2;
                    break;
                case 5:

                    for (int x = 0; x <= opcodeDigits[1]; x++) {
                        chip8->RAM[chip8->I + x] = chip8->V[x];
                    }
                    chip8->PC += 2;
                    break;

                case 6:
                    for (int x = 0; x <= opcodeDigits[1]; x++) {
                        chip8->V[x] = chip8->RAM[chip8->I + x];
                    }
                    chip8->PC += 2;
                    break;
            }

            break;

        default:
            printf("ERROR: Unrecognized opcode %x%x%x%x\n", opcodeDigits[0], 
                    opcodeDigits[1], opcodeDigits[2], opcodeDigits[3]);
            break;
    }

    if (chip8->DT != 0) {
        chip8->DT -= 1;
    }
    if (chip8->ST != 0) {
        chip8->ST -= 1;
    }
    //printf("Executed opcode %x%x%x%x, PC:%x, I:%x, SP:%x\n", opcodeDigits[0], opcodeDigits[1], opcodeDigits[2], opcodeDigits[3], chip8->PC, chip8->I, chip8->SP);
    //getchar();

    if (draw) {
        return 2;
    } else {
        return 1;
    }
}

void runLoop(struct chip8System chip8) {

    SDL_Init(SDL_INIT_EVERYTHING);
    SDL_Surface * chip8Monitor;
    chip8Monitor=SDL_SetVideoMode(640, 320, 8, SDL_HWSURFACE);

    bool running = true;
    int startTime;

    SDL_Rect pixels[32][64];
    for (int y = 0; y < 32; y++) {
        for (int x = 0; x < 64; x++) {
            pixels[y][x].x = x * 10;
            pixels[y][x].y = y * 10;
            pixels[y][x].w = 10;
            pixels[y][x].h = 10;
        }
    }

    uint32_t colour[2] = {SDL_MapRGB(chip8Monitor->format, 0x0, 0x0, 0x0), 
        SDL_MapRGB(chip8Monitor->format, 0xFF, 0xFF, 0xFF)};


    while (running) {

        startTime = SDL_GetTicks();
        SDL_Event event;

        while (SDL_PollEvent(&event)) {
            switch(event.type) {
                
                case SDL_QUIT: {
                    running = false;
                    break;
                }

                case SDL_KEYDOWN: {
                    switch(event.key.keysym.sym) {

                        case SDLK_1:
                            chip8.key[0x1] = 1;
                            break;
                        case SDLK_2:
                            chip8.key[0x2] = 1;
                            break;
                        case SDLK_3:
                            chip8.key[0x3] = 1;
                            break;
                        case SDLK_4:
                            chip8.key[0xC] = 1;
                            break;

                        case SDLK_q:
                            chip8.key[0x4] = 1;
                            break;
                        case SDLK_w:
                            chip8.key[0x5] = 1;
                            break;
                        case SDLK_e:
                            chip8.key[0x6] = 1;
                            break;
                        case SDLK_r:
                            chip8.key[0xD] = 1;
                            break;

                        case SDLK_a:
                            chip8.key[0x7] = 1;
                            break;
                        case SDLK_s:
                            chip8.key[0x8] = 1;
                            break;
                        case SDLK_d:
                            chip8.key[0x9] = 1;
                            break;
                        case SDLK_f:
                            chip8.key[0xE] = 1;
                            break;

                        case SDLK_z:
                            chip8.key[0xA] = 1;
                            break;
                        case SDLK_x:
                            chip8.key[0x0] = 1;
                            break;
                        case SDLK_c:
                            chip8.key[0xB] = 1;
                            break;
                        case SDLK_v:
                            chip8.key[0xF] = 1;
                            break;

                        default:
                            break;
                    }
                    break;
                 }

                case SDL_KEYUP: {
                 switch(event.key.keysym.sym) {

                     case SDLK_1:
                         chip8.key[0x1] = 0;
                         break;
                     case SDLK_2:
                         chip8.key[0x2] = 0;
                         break;
                     case SDLK_3:
                         chip8.key[0x3] = 0;
                         break;
                     case SDLK_4:
                         chip8.key[0xC] = 0;
                         break;

                     case SDLK_q:
                         chip8.key[0x4] = 0;
                         break;
                     case SDLK_w:
                         chip8.key[0x5] = 0;
                         break;
                     case SDLK_e:
                         chip8.key[0x6] = 0;
                         break;
                     case SDLK_r:
                         chip8.key[0xD] = 0;
                         break;

                     case SDLK_a:
                         chip8.key[0x7] = 0;
                         break;
                     case SDLK_s:
                         chip8.key[0x8] = 0;
                         break;
                     case SDLK_d:
                         chip8.key[0x9] = 0;
                         break;
                     case SDLK_f:
                         chip8.key[0xE] = 0;
                         break;

                     case SDLK_z:
                         chip8.key[0xA] = 0;
                         break;
                     case SDLK_x:
                         chip8.key[0x0] = 0;
                         break;
                     case SDLK_c:
                         chip8.key[0xB] = 0;
                         break;
                     case SDLK_v:
                         chip8.key[0xF] = 0;
                         break;

                     default:
                         break;
                 }
                 break;
              }


            }
        }
    
        int opcodeResult;
        
        do {
            opcodeResult = processNextOpcode(&chip8);
            if (opcodeResult == 0) {
                running = false;
                break;
            }

        } while (opcodeResult != 2);
       

        //fix this trash
        SDL_FillRect(chip8Monitor, &chip8Monitor->clip_rect, colour[0]);
        for (int x = 0; x < 32; x++) {
            for (int y = 0; y < 64; y++) {
                SDL_FillRect(chip8Monitor, &(pixels[x][y]), colour[chip8.display[x][y]]);
            }
        }
        SDL_Flip(chip8Monitor);

        int frameTime = SDL_GetTicks() - startTime;

        //wait for next frame
        if (frameTime < 1000/FPS) {
            SDL_Delay(1000/FPS - frameTime);
        }

    }

    SDL_Quit();
    return;
}


