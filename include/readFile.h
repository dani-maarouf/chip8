#ifndef __DMAAROUF_READFILE__
#define __DMAAROUF_READFILE__

typedef unsigned char byte;

typedef struct ROM {
    int numOpcodes;
    byte * opCodes;
} ROM;

ROM readFile(const char *);

#endif
/* defined __DMAAROUF_READFILE__ */