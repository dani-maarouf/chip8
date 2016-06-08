#include <SDL2/SDL.h>
#include <string.h>
#include "chip8.h"

static const int FRAME_TIME = 1000/60;
static const int SCALE_FACTOR = 10;

static int processEvents(bool *, SDL_Event *);
static int initSDL(const char *);
static void closeSDL();
static void draw(uint32_t * pixels);

SDL_Window * window = NULL;
SDL_Renderer * renderer = NULL;
SDL_Texture * texture = NULL;


void runLoop(struct chip8System chip8, const char * fileLoc) {

    if (!initSDL(fileLoc)) {
        fprintf(stderr, "Could not initialize SDL : %s\n", SDL_GetError());
        SDL_Quit();
        return;
    }

    int startTime;
    startTime = SDL_GetTicks();

    draw(chip8.display);

    while (true) {

        //decrement counters at appropriate FPS
        if (SDL_GetTicks() - startTime >= FRAME_TIME) {
            decrementC8Counters(&chip8);
            startTime = SDL_GetTicks();
        }
        
        SDL_Event event;

        //process events
        int result;
        result = processEvents(chip8.key, &event);  
        if (!result) break;
        
        //next instruction
        int opcodeResult;
        opcodeResult = processNextOpcode(&chip8);
        if (opcodeResult == 0) {
            SDL_Delay(1000);
            break;
        }
        else if (opcodeResult == 1) continue;

        //draw and wait until end of frame
        if (opcodeResult == 2 || opcodeResult == 3) {
            draw(chip8.display);
            int frameTime = SDL_GetTicks() - startTime;
            if (frameTime < FRAME_TIME) {
                SDL_Delay(FRAME_TIME - frameTime);
            }
            startTime = SDL_GetTicks();
            decrementC8Counters(&chip8);
        } 
    }

    closeSDL();
    return;
}

inline static void draw(uint32_t * pixels) {
    SDL_UpdateTexture(texture, NULL, pixels, C8_WIDTH * sizeof(uint32_t));
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, NULL, NULL);
    SDL_RenderPresent(renderer);
}

static int initSDL(const char * fileLoc) {

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        return 0;
    }

    char windowTitle[100];
    strcpy(windowTitle, "chip8 : ");
    strcat(windowTitle, fileLoc);

    window = SDL_CreateWindow(windowTitle, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        C8_WIDTH * SCALE_FACTOR, C8_HEIGHT * SCALE_FACTOR, 0);

    if (window == NULL) {
        SDL_Quit();
        return 0;
    }

    renderer = SDL_CreateRenderer(window, -1, 0);

    if (renderer == NULL) {
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 0;
    }

    texture = SDL_CreateTexture(renderer,
        SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC, C8_WIDTH, C8_HEIGHT);

    if (texture == NULL) {
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 0;
    }

    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, 0); 

    return 1;

}

static int processEvents(bool * key, SDL_Event * event) {

    while (SDL_PollEvent(event)) {
        switch(event->type) {
            case SDL_QUIT: {
                return 0;
            }

            case SDL_KEYDOWN: {
                switch(event->key.keysym.sym) {

                    case SDLK_1:
                    key[0x1] = true;
                    break;
                    case SDLK_2:
                    key[0x2] = true;
                    break;
                    case SDLK_3:
                    key[0x3] = true;
                    break;
                    case SDLK_4:
                    key[0xC] = true;
                    break;
                    case SDLK_q:
                    key[0x4] = true;
                    break;
                    case SDLK_w:
                    key[0x5] = true;
                    break;
                    case SDLK_e:
                    key[0x6] = true;
                    break;
                    case SDLK_r:
                    key[0xD] = true;
                    break;
                    case SDLK_a:
                    key[0x7] = true;
                    break;
                    case SDLK_s:
                    key[0x8] = true;
                    break;
                    case SDLK_d:
                    key[0x9] = true;
                    break;
                    case SDLK_f:
                    key[0xE] = true;
                    break;
                    case SDLK_z:
                    key[0xA] = true;
                    break;
                    case SDLK_x:
                    key[0x0] = true;
                    break;
                    case SDLK_c:
                    key[0xB] = true;
                    break;
                    case SDLK_v:
                    key[0xF] = true;
                    break;
                    default:
                    break;
                }
                break;
            }

            case SDL_KEYUP: {
             switch(event->key.keysym.sym) {

                case SDLK_1:
                key[0x1] = false;
                break;
                case SDLK_2:
                key[0x2] = false;
                break;
                case SDLK_3:
                key[0x3] = false;
                break;
                case SDLK_4:
                key[0xC] = false;
                break;
                case SDLK_q:
                key[0x4] = false;
                break;
                case SDLK_w:
                key[0x5] = false;
                break;
                case SDLK_e:
                key[0x6] = false;
                break;
                case SDLK_r:
                key[0xD] = false;
                break;
                case SDLK_a:
                key[0x7] = false;
                break;
                case SDLK_s:
                key[0x8] = false;
                break;
                case SDLK_d:
                key[0x9] = false;
                break;
                case SDLK_f:
                key[0xE] = false;
                break;
                case SDLK_z:
                key[0xA] = false;
                break;
                case SDLK_x:
                key[0x0] = false;
                break;
                case SDLK_c:
                key[0xB] = false;
                break;
                case SDLK_v:
                key[0xF] = false;
                break;
                default:
                break;
            }
            break;
        }
        default:
        return 1;
    }

}

return 1;
}

static void closeSDL() {

    if (texture != NULL) {
        SDL_DestroyTexture(texture);
    }
    if (renderer != NULL) {
        SDL_DestroyRenderer(renderer);
    }
    if (window != NULL) {
        SDL_DestroyWindow(window);
    }

    SDL_Quit();

    return;
}
