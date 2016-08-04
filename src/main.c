#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#include "chip8.h"
#include "gameLoop.h"

int main (int argc, char ** argv) {

    char * romLocation;
    int enableI = 0;
    romLocation = argv[1];

    if (argc == 1) {
        printf("\nUsage: %s /PATH/TO/CHIP8/ROM\nFlags:\n-i0\t Default: I = I for FX55 and FX65 \n", argv[0]);
        printf("-i1\t I = I + x for FX55 and FX65\n-i2\t I = I + x + 1 for FX55 and FX65\n");
        return 1;
    } else if (argc == 3) {

        if (strcmp("-i0", argv[1]) == 0) {
            romLocation = argv[2];
            enableI = 0;
        } else if (strcmp("-i0", argv[2]) == 0) {
            romLocation = argv[1];
            enableI = 0;
        } else if (strcmp("-i1", argv[1]) == 0) {
            romLocation = argv[2];
            enableI = 1;
        } else if (strcmp("-i1", argv[2]) == 0) {
            romLocation = argv[1];
            enableI = 1;
        } else if (strcmp("-i2", argv[1]) == 0) {
            romLocation = argv[2];
            enableI = 2;
        } else if (strcmp("-i2", argv[2]) == 0) {
            romLocation = argv[1];
            enableI = 2;
        } else {
            printf("Program arguments not recognized\n");
            return 1;
        }

    } else if (argc != 2) {
        printf("Error: Please call program with 1 argument pointing to location of ROM\n");
        return 1;
    }

    srand(time(NULL));

    struct chip8System * chip8;
    chip8 = chip8Init(romLocation);
    if (chip8 == NULL) {
        printf("Error: Invalid file '%s'\n", romLocation);
        return 1;
    }

    runLoop(*chip8, argv[1], enableI);

    destroyChip8System(chip8);

    return 0;
}

