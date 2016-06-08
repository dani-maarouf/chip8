#include <SDL2/SDL.h>
#include <string.h>
#include "chip8.h"

#define FPS 60

const int SCALE_FACTOR = 15;

static int processEvents(struct chip8System *);

SDL_Window * window = NULL;
SDL_Renderer * renderer = NULL;
SDL_Texture * texture = NULL;

int initSDL(const char * fileLoc) {

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        return 0;
    }

    char windowTitle[100];
    strcpy(windowTitle, "chip8 emulator : ");
    strcat(windowTitle, fileLoc);


    window = SDL_CreateWindow(windowTitle, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        64 * SCALE_FACTOR, 32 * SCALE_FACTOR, 0);

    if (window == NULL) {
        SDL_Quit();
        return 0;
    }

    renderer = SDL_CreateRenderer(window, -1, 0);

    if (renderer == NULL) {
        SDL_Quit();
        return 0;
    }

    texture = SDL_CreateTexture(renderer,
        SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC, 64, 32);

    if (texture == NULL) {
        SDL_Quit();
        return 0;
    }

    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, 0); 

    return 1;

}

void runLoop(struct chip8System chip8, const char * fileLoc) {

    if (!initSDL(fileLoc)) {
        SDL_Quit();
        return;
    }

    SDL_UpdateTexture(texture, NULL, chip8.display, 64 * sizeof(uint32_t));
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, NULL, NULL);
    SDL_RenderPresent(renderer);

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
                SDL_UpdateTexture(texture, NULL, chip8.display, 64 * sizeof(uint32_t));
                SDL_RenderClear(renderer);
                SDL_RenderCopy(renderer, texture, NULL, NULL);
                SDL_RenderPresent(renderer);
            }

            int frameTime = SDL_GetTicks() - startTime;
            if (frameTime < 1000/FPS) {
                SDL_Delay(1000/FPS - frameTime);
            }

            decrementC8Counters(&chip8);
            startTime = SDL_GetTicks();
        }
    }

    //destroy
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

