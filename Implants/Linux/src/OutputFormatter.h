
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <errno.h>
#include <pthread.h>

#include <dlfcn.h>

#ifndef OUTPUT_FORMATTER_H
#define OUTPUT_FORMATTER_H

// volatile struct*
namespace OutputVariables {

    char* OUTPUT_DATA; // The data buffer to store it in.
    int sizeOutputData;
    int outputDataOffset;

    // For memory safe operations on the output.
    pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

    void setOutputData(const char* fmt, ...) {

        pthread_mutex_lock(&mutex);

        // Get argument count.
        va_list args;
        va_start(args, fmt);
        int length = vsnprintf(NULL, 0, fmt, args) +1;
        va_end(args);

        // Resize the OUTPUT_DATA buffer in case there was already something in it.
        char* tempstr = (char*)realloc(OutputVariables::OUTPUT_DATA, OutputVariables::sizeOutputData + length +1);
        if (tempstr == NULL) {
            printf("Failed to reallocate the OUTPUT_DATA buffer! Error code: %d\n", errno);
            return;
        }

        OutputVariables::OUTPUT_DATA = tempstr;
        memset(OutputVariables::OUTPUT_DATA + OutputVariables::outputDataOffset, 0, length +1); // Cleanup OUTPUT_DATA.

        // Now copy over the data to the buffer.
        va_start(args, fmt);
        length = vsnprintf(OutputVariables::OUTPUT_DATA + OutputVariables::outputDataOffset, length, fmt, args);

        // Update size and offset for next time.
        OutputVariables::sizeOutputData += length; 
        OutputVariables::outputDataOffset += length;
        va_end(args);

        pthread_mutex_unlock(&mutex);
    }

    // Retrieve the output buffer from the programs.
    char* getOutputData(int* outputSize) {

        pthread_mutex_lock(&mutex);

        *outputSize = OutputVariables::sizeOutputData;

        // Copy over the bytes, not using the same memory location
        // because we are freeing that pointer.
        char* tempStr = new char[OutputVariables::sizeOutputData +1]; 
        strncpy(tempStr, OutputVariables::OUTPUT_DATA, OutputVariables::sizeOutputData);
        tempStr[OutputVariables::sizeOutputData] = '\0';

        // Cleanup.
        if (OUTPUT_DATA) {
            free(OUTPUT_DATA);
            OUTPUT_DATA = NULL;
        }

        sizeOutputData= 0;
        outputDataOffset = 0;


        pthread_mutex_unlock(&mutex);

        return tempStr;
    }
};

#endif
