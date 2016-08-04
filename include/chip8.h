#ifndef __DMAAROUF_CHIP8__
#define __DMAAROUF_CHIP8__

#include <stdint.h>
#include <stdbool.h>

static const int C8_WIDTH = 64;
static const int C8_HEIGHT = 32; 

struct chip8System {

    uint8_t RAM[0x1000];    //memory
    uint8_t V[16];          //registers
    uint16_t stack[16];     //stack
    uint32_t * display;     //argb pixel buffer
    bool key[16];           //controller keys
    uint16_t I;             //I register
    uint16_t DT;            //delay timer
    uint16_t ST;            //sound timer
    uint16_t PC;            //program counter
    uint8_t SP;             //stack pointer

};

/* chip8Init
 * Initializes struct values, reads user specified file into memory and reads sprites into memory
 */
struct chip8System * chip8Init(char *);
void destroyChip8System(struct chip8System *);
void decrementC8Counters(struct chip8System *);
int processNextOpcode(struct chip8System *, int, int);

#endif
/* defined __DMAAROUF_CHIP8__ */
