#include <SDL/SDL.h>
#include "chip8.h"

#define FPS 60

static int processEvents(struct chip8System *);

SDL_Surface * screen;
SDL_Rect pixels[32][64];
uint32_t colour[2];

int initSDL() {

    if (SDL_Init(SDL_INIT_EVERYTHING) == -1) {
        return 0;
    }

    screen = SDL_SetVideoMode(640, 320, 8, SDL_SWSURFACE);

    if (screen == NULL) {
        return 0;
    }

    SDL_WM_SetCaption("chip8 emulator", NULL);

    for (int y = 0; y < 32; y++) {
        for (int x = 0; x < 64; x++) {
            pixels[y][x].x = x * 10;
            pixels[y][x].y = y * 10;
            pixels[y][x].w = 10;
            pixels[y][x].h = 10;
        }
    }

    colour[0] = SDL_MapRGB(screen->format, 0x0, 0x0, 0x0);
    colour[1] = SDL_MapRGB(screen->format, 0xFF, 0xFF, 0xFF);

    return 1;

}

void draw(struct chip8System chip8) {

    for (int x = 0; x < 32; x++) {
        for (int y = 0; y < 64; y++) {
            if (chip8.display[x][y]) {
                SDL_FillRect(screen, &(pixels[x][y]), colour[1]);
            } else {
                SDL_FillRect(screen, &(pixels[x][y]), colour[0]);
            }

        }
    }
    SDL_Flip(screen);

    return;

}

void runLoop(struct chip8System chip8) {

    if (!initSDL()) {
        SDL_Quit();
        return;
    }

    SDL_FillRect(screen, &screen->clip_rect, colour[0]);

    int startTime;
    startTime = SDL_GetTicks();

    while (true) {

        if (SDL_GetTicks() - startTime >= 1000/FPS) {
            decrementC8Counters(&chip8);
            startTime = SDL_GetTicks();
        }
        
        //process events
        int result;
        result = processEvents(&chip8);
        if (!result) break;
        
        //next instruction
        int opcodeResult;
        opcodeResult = processNextOpcode(&chip8);
        if (opcodeResult == 0) break;
        else if (opcodeResult == 1) continue;

        //draw
        if (opcodeResult == 2 || opcodeResult == 3) {
            if (opcodeResult == 2) {
                draw(chip8);
            }

            int frameTime = SDL_GetTicks() - startTime;
            if (frameTime < 1000/FPS) {
                SDL_Delay(1000/FPS - frameTime);
            }

            decrementC8Counters(&chip8);
            startTime = SDL_GetTicks();
        }
    }


    SDL_FreeSurface(screen);
    SDL_Quit();
    return;
}

static int processEvents(struct chip8System * chip8) {

    SDL_Event event;

    while (SDL_PollEvent(&event)) {
        switch(event.type) {
            case SDL_QUIT: {
                return 0;
            }

            case SDL_KEYDOWN: {
                switch(event.key.keysym.sym) {

                    case SDLK_1:
                        chip8->key[0x1] = true;
                        break;
                    case SDLK_2:
                        chip8->key[0x2] = true;
                        break;
                    case SDLK_3:
                        chip8->key[0x3] = true;
                        break;
                    case SDLK_4:
                        chip8->key[0xC] = true;
                        break;
                    case SDLK_q:
                        chip8->key[0x4] = true;
                        break;
                    case SDLK_w:
                        chip8->key[0x5] = true;
                        break;
                    case SDLK_e:
                        chip8->key[0x6] = true;
                        break;
                    case SDLK_r:
                        chip8->key[0xD] = true;
                        break;
                    case SDLK_a:
                        chip8->key[0x7] = true;
                        break;
                    case SDLK_s:
                        chip8->key[0x8] = true;
                        break;
                    case SDLK_d:
                        chip8->key[0x9] = true;
                        break;
                    case SDLK_f:
                        chip8->key[0xE] = true;
                        break;
                    case SDLK_z:
                        chip8->key[0xA] = true;
                        break;
                    case SDLK_x:
                        chip8->key[0x0] = true;
                        break;
                    case SDLK_c:
                        chip8->key[0xB] = true;
                        break;
                    case SDLK_v:
                        chip8->key[0xF] = true;
                        break;
                    default:
                        break;
                }
                break;
            }

            case SDL_KEYUP: {
               switch(event.key.keysym.sym) {

                    case SDLK_1:
                        chip8->key[0x1] = false;
                        break;
                    case SDLK_2:
                        chip8->key[0x2] = false;
                        break;
                    case SDLK_3:
                        chip8->key[0x3] = false;
                        break;
                    case SDLK_4:
                        chip8->key[0xC] = false;
                        break;
                    case SDLK_q:
                        chip8->key[0x4] = false;
                        break;
                    case SDLK_w:
                        chip8->key[0x5] = false;
                        break;
                    case SDLK_e:
                        chip8->key[0x6] = false;
                        break;
                    case SDLK_r:
                        chip8->key[0xD] = false;
                        break;
                    case SDLK_a:
                        chip8->key[0x7] = false;
                        break;
                    case SDLK_s:
                        chip8->key[0x8] = false;
                        break;
                    case SDLK_d:
                        chip8->key[0x9] = false;
                        break;
                    case SDLK_f:
                        chip8->key[0xE] = false;
                        break;
                    case SDLK_z:
                        chip8->key[0xA] = false;
                        break;
                    case SDLK_x:
                        chip8->key[0x0] = false;
                        break;
                    case SDLK_c:
                        chip8->key[0xB] = false;
                        break;
                    case SDLK_v:
                        chip8->key[0xF] = false;
                        break;
                    default:
                        break;
               }
               break;
           }
        }
    }

    return 1;
}

