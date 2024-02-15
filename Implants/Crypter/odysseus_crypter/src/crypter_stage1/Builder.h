#include "stdafx.h"

#ifndef Builder_H
#define Builder_H

class Builder
{
private:
    FILE* targetFile = NULL;
public:
    Builder(char* fileName);

    /**
     * @brief Stub writing.
     * 
     *  locateCode();
     *  locateStub();
     *  writeToStub();
     * 
     *  First open the program target file.
     * 
     *  Then locate the stub to write to in the stub program.
     * 
     *  Then write the program to the stub.
     */

    void* locateStub();
    bool writeToStub();
};

#endif // !~ Builder_H

Builder::Builder(char* fileName) {

    targetFile = fopen(fileName, "rb");
    if (targetFile == NULL)
    {
        printf("[%d] Failed to open target file!\n", errno);
        return;
    }
} 

// Locate the place where to write the code in.
void * Builder::locateStub() {
    
    if (targetFile == NULL) {
        printf("[%d] No target file specified!\n");
        return NULL;
    }
}

// Write the code from target file to the stub.
bool Builder::writeToStub() {
    return false;
}