#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "chip8.h"
#include "gameLoop.h"

int main (int argc, char ** argv) {

    if (argc != 2) {
        perror("Error: Please call program with 1 argument pointing to location of ROM\n");
        return 1;
    }

    srand(time(NULL));

    struct chip8System * chip8;
    chip8 = chip8Init(argv[1], 0x200, 0x50);
    if (chip8 == NULL) {
        perror("Error: Could not open file. Exiting\n");
        return 1;
    }

    runLoop(*chip8, argv[1]);
    free(chip8->display);
    free(chip8);

    return 0;
}

