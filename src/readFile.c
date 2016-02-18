#include <stdio.h>
#include <stdlib.h>
#include "readFile.h"

ROM readFile(const char * fileLocation) {

    ROM newROM;
    newROM.opCodes = NULL;
    newROM.numOpcodes = 0;

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
        newROM.numOpcodes++;
    }

    fclose(romFile);
    romFile = fopen(fileLocation, "rb");
    if (romFile == NULL) {
        perror("readFile() -> Error: Could not open ROM file\n");
        return newROM;
    }

    newROM.opCodes = malloc(sizeof(byte) * newROM.numOpcodes);
    if (newROM.opCodes == NULL) {
        perror("readFile() -> Error: Malloc could not allocate memory\n");
        return newROM;
    }

    for (int x = 0; x < newROM.numOpcodes; x++) {
        newROM.opCodes[x] = fgetc(romFile);
    }
    fclose(romFile);
    return newROM;
}
