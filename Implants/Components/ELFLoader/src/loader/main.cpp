

#include <iostream>
#include <stdio.h>
#include <string.h>

#include "ELFLoader.h"

unsigned char* getObject(char* objectFile, size_t *fileSize)
{

    FILE* f = fopen(objectFile, "rb");
    if (f == NULL) {
        printf("Failed to open the object file!\n");
        return NULL;
    }

    fseek(f, 0, SEEK_END);
    *fileSize = ftell(f);
    rewind(f);

    unsigned char* buffer = (unsigned char*)malloc(*fileSize);
    memset(buffer, '\0', *fileSize);

    if (fread(buffer, *fileSize, 1, f) <= 0) {
        printf("Failed to read the object file!\n");
        return NULL;
    }

    fclose(f);
    return buffer;

}

int main(int argc, char* argv[]) 
{
    if (argc != 2) {

        printf("Usage: %s [bof-file]\n", argv[0]);
        return 1;
    }

    size_t fileSize = 0;
    unsigned char* buffer = getObject(argv[1], &fileSize);
    if (buffer == NULL) {
        return 1;
    }

    return parseObject(buffer, fileSize);
}