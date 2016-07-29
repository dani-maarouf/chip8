#include <stdio.h>
#include <string.h>
#include "chip8.h"

#include <SDL2/SDL.h>


//video
static const int FRAME_TIME = 1000/60;
static const int SCALE_FACTOR = 10;

SDL_Window * window = NULL;
SDL_Renderer * renderer = NULL;
SDL_Texture * texture = NULL;


//audio
const int samplingFrequency = 48000;
const int sampleBytes = sizeof(int16_t) * 2;
const int channels = 2;
const SDL_AudioFormat format = AUDIO_S16LSB;
const int16_t waveAmplitude = 1500;
const int squareHalfPeriod = (48000 / 512) / 2;

uint64_t sampleClock = 0;
SDL_AudioDeviceID sdlAudioDevice = 0;


//local functions
static int processEvents(bool *, SDL_Event *);
static int initSDL(const char *);
static void closeSDL();
static inline void draw(uint32_t * pixels);
static inline void generateAndQueueSquare(int bytesToQueue, uint64_t * clock, int period, int volume);
static inline void topUpQueue();


void runLoop(struct chip8System chip8, const char * fileLoc) {

    if (!initSDL(fileLoc)) {
        fprintf(stderr, "Could not initialize SDL : %s\n", SDL_GetError());
        SDL_Quit();
        return;
    }

    bool audioPaused;
    audioPaused = true;
    
    int startTime;
    startTime = SDL_GetTicks();

    draw(chip8.display);                        //first frame

    SDL_Event event;

    while (true) {

        int opcodeResult;
        int eventResult = processEvents(chip8.key, &event); //process input and events

        do {
            opcodeResult = processNextOpcode(&chip8, false);
            if (SDL_GetTicks() - startTime >= FRAME_TIME) {
                break;
            }
        } while (eventResult != 0 && opcodeResult == 1);

        if (eventResult == 0) {
            //quitd
            break;
        }

        if (opcodeResult == 0) {
            //infinite chip8 loop
            SDL_Delay(1000);
            break;
        }

        topUpQueue();

        if (chip8.ST != 0) {
            if (audioPaused) {
                SDL_PauseAudioDevice(sdlAudioDevice, 0);    //unpause audio
                audioPaused = false;
            }
            SDL_Delay(3);
        } else {
            if (!audioPaused) {
                SDL_PauseAudioDevice(sdlAudioDevice, 1);    //pause audio
                audioPaused = true;
            }
        }

        decrementC8Counters(&chip8);
        draw(chip8.display);    //pixel buffer to screen

        //wait for next frame
        int frameTime = SDL_GetTicks() - startTime;
        if (frameTime < FRAME_TIME) {
            SDL_Delay(FRAME_TIME - frameTime);
        }
        startTime = SDL_GetTicks();        
    }

    SDL_PauseAudioDevice(sdlAudioDevice, 1);    //pause
    SDL_ClearQueuedAudio(sdlAudioDevice);

    closeSDL();
    return;
}

inline static void draw(uint32_t * pixels) {
    SDL_UpdateTexture(texture, NULL, pixels, C8_WIDTH * sizeof(uint32_t));
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, NULL, NULL);
    SDL_RenderPresent(renderer);
}

static inline void topUpQueue() {
    int bytes = (48000 * sampleBytes) - SDL_GetQueuedAudioSize(sdlAudioDevice);
    if (bytes) {
        generateAndQueueSquare(bytes, &sampleClock, squareHalfPeriod, waveAmplitude);
    }
    return;
}

static inline void generateAndQueueSquare(int bytesToQueue, uint64_t * clock, int period, int volume) {

    int16_t * SoundBuffer = (int16_t *) malloc(bytesToQueue);
    
    for (int i = 0; i < (bytesToQueue/sampleBytes); i++, (*clock)++) {
        //alternatives produces high and low signals based on current time and period
        int16_t SampleValue = ((*clock / period) % 2) ? volume : -volume;
        SoundBuffer[i * 2] = SampleValue;
        SoundBuffer[i * 2 + 1] = SampleValue;

    }

    SDL_QueueAudio(sdlAudioDevice, (void *) SoundBuffer, bytesToQueue);

    free(SoundBuffer);

    return;
}

static int initSDL(const char * fileLoc) {

    if (SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO) < 0) {
        printf("Failed to initialize SDL : %s\n", SDL_GetError());
        return 0;
    }

    /* VIDEO */
    char windowTitle[100];
    strcpy(windowTitle, "chip8 emulator : ");
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

    /* AUDIO */
    SDL_AudioSpec want;
    SDL_memset(&want, 0, sizeof(want));

    SDL_AudioSpec have;
    want.freq = samplingFrequency;
    want.format = format;        //32-bit floating point samples in little-endian byte order
    want.channels = channels;
    want.samples = samplingFrequency * sampleBytes / 60;

    sdlAudioDevice = SDL_OpenAudioDevice(NULL, 0, &want, &have, 0);

    if (sdlAudioDevice == 0) {
        printf("Failed to open audio: %s\n", SDL_GetError());
        SDL_DestroyTexture(texture);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 0;
    }

    return 1;

}

static int processEvents(bool * key, SDL_Event * event) {

    SDL_PollEvent(event);

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

    if (sdlAudioDevice != 0) {
        SDL_CloseAudioDevice(sdlAudioDevice);
    }



    SDL_Quit();

    return;
}
