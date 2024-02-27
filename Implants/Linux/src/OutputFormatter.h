
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <errno.h>

#include <dlfcn.h>

#ifndef OUTPUT_FORMATTER_H
#define OUTPUT_FORMATTER_H

class OutputFormatter {
private:

    char* OUTPUT_DATA ; // The data buffer to store output in.
    int sizeOutputData = 0;
    int outputDataOffset = 0;

public:
    OutputFormatter() {
        printf("OUTPUT_DATA location: %p\n", OUTPUT_DATA);
    }

    void setOutputData(const char* fmt, ...) {

        printf("setOutputdata data loc: %p\n", OUTPUT_DATA);

        int length = 0;
        char* tempstr = NULL;
        va_list args;

        // Get argument count.
        va_start(args, fmt);
        length = vsnprintf(NULL, 0, fmt, args) +1;
        va_end(args);

        // Resize the OUTPUT_DATA buffer in case there was already something in it.
        tempstr = (char*)realloc(OUTPUT_DATA, sizeOutputData + length +1);
        if (tempstr) {
            printf("Failed to reallocate the OUTPUT_DATA buffer! Error code: %d\n", errno);
            return;
        }

        OUTPUT_DATA = tempstr;
        memset(OUTPUT_DATA + outputDataOffset, 0, length +1); // Cleanup OUTPUT_DATA.

        // Now copy over the data to the buffer.
        va_start(args, fmt);
        length = vsnprintf(OUTPUT_DATA + outputDataOffset, length, fmt, args);
        // update size and offset for next time.
        sizeOutputData += length; 
        outputDataOffset += length;
        va_end(args);

    }

    char* getOutputData(int* outputSize) {

        printf("OUTPUT DATA: %p\n", OUTPUT_DATA);
        
        char* outputData = OUTPUT_DATA;
        *outputSize = sizeOutputData;

        // Cleanup.
        sizeOutputData = 0;
        outputDataOffset = 0;
        free(OUTPUT_DATA);
        OUTPUT_DATA = NULL;
        return outputData;
    }


};



#endif // !~ OUTPUT_FORMATTER_H

