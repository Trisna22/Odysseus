
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

namespace OutputVariables {

    // For memory safe operations on the output.
    pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

    int countOutputs = 0;
    char**outputDataList;
    int* outputSizeList;
    int* outputOffsetList;

    int getNewOutputId() {

        pthread_mutex_lock(&mutex);

        // Reallocating list everytime we have a new output.
        char**tempArray = (char**)reallocarray(outputDataList, countOutputs+1, sizeof(char*));
        int* tempSize = (int*)reallocarray(outputSizeList, countOutputs+1, sizeof(int));
        int*tempOffset = (int*)reallocarray(outputOffsetList, countOutputs+1, sizeof(int));

        outputDataList = tempArray;
        outputSizeList = tempSize;
        outputOffsetList = tempOffset;

        // Inital values.
        outputDataList[countOutputs] = (char*)malloc(10);
        outputSizeList[countOutputs] = 0;
        outputOffsetList[countOutputs] = 0;

        pthread_mutex_unlock(&mutex);
        return countOutputs++; // Give the ID of the last item (which is new).
    }

    void setOutputData(int id, const char* fmt, ...) {
        pthread_mutex_lock(&mutex);

        // char* OUTPUT_DATA = outputDataList[id]; // The data buffer to store it in.
        int sizeOutputData = outputSizeList[id];
        int outputDataOffset = outputOffsetList[id];

        // Get argument count.
        va_list args;
        va_start(args, fmt);
        int length = vsnprintf(NULL, 0, fmt, args) +1;
        va_end(args);

        // Resize the OUTPUT_DATA buffer in case there was already something in it.
        char* tempstr = (char*)realloc(outputDataList[id], sizeOutputData + length +1);
        if (tempstr == NULL) {
            printf("Failed to reallocate the OUTPUT_DATA buffer! Error code: %d\n", errno);
            return;
        }

        outputDataList[id] = tempstr;
        memset(outputDataList[id] + outputDataOffset, 0, length +1); // Cleanup resized char array.

        // Now copy over the data to the buffer.
        va_start(args, fmt);
        length = vsnprintf(outputDataList[id] + outputDataOffset, length, fmt, args);

        // Update size and offset for next time.
        OutputVariables::outputSizeList[id] += length; 
        OutputVariables::outputOffsetList[id] += length;
        va_end(args);

        pthread_mutex_unlock(&mutex);
    }

    // Retrieve the output buffer from the programs.
    char* getOutputData(int id, int* outputSize) {

        pthread_mutex_lock(&mutex);

        *outputSize = outputSizeList[id];

        // Copy over the bytes, not using the same memory location
        // because we are freeing that pointer.
        char* tempStr = new char[outputSizeList[id] +1]; 
        strncpy(tempStr, outputDataList[id], outputSizeList[id]);
        tempStr[outputSizeList[id]] = '\0';

        // Cleanup.
        if (outputDataList[id]) {
            free(outputDataList[id]);
            outputDataList[id] = NULL;
        }
        
        outputSizeList[id]= 0;
        outputOffsetList[id] = 0;

        pthread_mutex_unlock(&mutex);

        return tempStr;
    }
};

#endif
