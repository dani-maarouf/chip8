#ifndef __DMAAROUF_CHIP8__
#define __DMAAROUF_CHIP8__

#include <stdint.h>
#include <stdbool.h>

struct chip8System {

    uint8_t RAM[0x1000];
    uint8_t V[16]; //registers
    uint16_t stack[16];
    bool display[32][64];    
    bool key[16];

    uint16_t I;

    uint16_t DT; //delay timer
    uint16_t ST; //sound timer

    uint16_t PC;
    uint8_t SP;

};

/* chip8Init
 * Initializes struct values, reads user specified file into memory and reads sprites into memory
 */
struct chip8System chip8Init(char *, int, int);

#endif
/* defined __DMAAROUF_CHIP8__ */
