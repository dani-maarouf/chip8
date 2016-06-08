#ifndef __DMAAROUF_CHIP8__
#define __DMAAROUF_CHIP8__

#include <stdint.h>
#include <stdbool.h>

static const int C8_WIDTH = 64;
static const int C8_HEIGHT = 32; 

struct chip8System {

    uint8_t RAM[0x1000];
    uint8_t V[16];          //registers

    uint16_t stack[16];

    uint32_t * display;    

    bool key[16];           //controller

    uint16_t I;

    uint16_t DT;            //delay timer
    uint16_t ST;            //sound timer

    uint16_t PC;            //program counter
    uint8_t SP;             //stasck pointer

};

/* chip8Init
 * Initializes struct values, reads user specified file into memory and reads sprites into memory
 */
struct chip8System * chip8Init(char *, int, int);
void decrementC8Counters(struct chip8System *);
int processNextOpcode(struct chip8System *);

#endif
/* defined __DMAAROUF_CHIP8__ */
