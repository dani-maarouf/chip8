#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#include "chip8.h"
#include "gameLoop.h"

int main (int argc, char ** argv) {

    char * romLocation;
    int enableI = true;
    romLocation = argv[1];

    if (argc == 1) {
        printf("\nUsage: %s /PATH/TO/CHIP8/ROM\nFlags:\t-i\t Disable FX55 and FX65 I increment for game compatibility \n", argv[0]);
        return 1;
    } else if (argc == 3) {
        if (strcmp("-i", argv[1]) == 0) {
            romLocation = argv[2];
            enableI = false;
        } else if (strcmp("-i", argv[2]) == 0) {
            romLocation = argv[1];
            enableI = false;
        } else {
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

