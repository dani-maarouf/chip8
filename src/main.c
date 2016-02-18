#include <stdio.h>
#include <stdlib.h>

typedef struct ROM {
    int size;
    int * intArray;
} ROM;

ROM readFile(const char * fileLocation) {

    ROM newROM;
    newROM.intArray = NULL;
    newROM.size = 0;

    if (fileLocation == NULL) {
        perror("readFile() -> Error: File location string is NULL\n");
        return newROM;
    }

    FILE * romFile;
    romFile = fopen(fileLocation, "rb");
    if (romFile == NULL) {
        perror("readFile() -> Error: Could not open ROM file\n");
        return newROM;
    }

    int readChar;
    while ((readChar = fgetc(romFile)) != EOF) {
        newROM.size++;
    }

    fclose(romFile);
    romFile = fopen(fileLocation, "rb");
    if (romFile == NULL) {
        perror("readFile() -> Error: Could not open ROM file\n");
        return newROM;
    }

    newROM.intArray = malloc(sizeof(int) * newROM.size);

    for (int x = 0; x < newROM.size; x++) {
        newROM.intArray[x] = fgetc(romFile);
    }

    return newROM;
}

int main (int argc, char * argv[]) {

    if (argc != 2) {
        perror("Please call program with 1 argument pointing to location of ROM\n");
        return -1;
    }

    ROM game;
    game = readFile(argv[1]);
    if (game.intArray == NULL) {
        perror("main() -> Error: Could not read ROM\n");
        return -1;
    }

    for (int x = 0; x < game.size; x++) {
        printf("%x ", game.intArray[x]);
    }


    printf("\nSuccess\n");
    return 0;
}