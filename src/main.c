#include <stdio.h>
#include <stdlib.h>

#include "readFile.h"

struct memory {



};

int main (int argc, char * argv[]) {

    if (argc != 2) {
        perror("Please call program with 1 argument pointing to location of ROM\n");
        return -1;
    }

    ROM gameROM;
    gameROM = readFile(argv[1]);
    if (gameROM.opCodes == NULL) {
        perror("main() -> Error: Could not read ROM\n");
        return -1;
    }

    for (int x = 0; x < gameROM.numOpcodes; x++) {
        printf("%x ", gameROM.opCodes[x]);
    }
    printf("\n");

    //https://en.wikipedia.org/wiki/CHIP-8
    //http://devernay.free.fr/hacks/chip8/C8TECH10.HTM

    free(gameROM.opCodes);
    printf("Success\n");
    return 0;
}