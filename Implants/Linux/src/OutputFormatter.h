
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <errno.h>

#include <dlfcn.h>

#ifndef OUTPUT_FORMATTER_H
#define OUTPUT_FORMATTER_H

// volatile struct*
namespace OutputVariables {

    char* OUTPUT_DATA = "xxx"; // The data buffer to store it in.
    int sizeOutputData;
    int outputDataOffset;

    void setOutputData(const char* fmt, ...) {

        printf("[OF setter] setOutputdata data loc: %p\n", OutputVariables::OUTPUT_DATA);

        int length = 0;
        char* tempstr = NULL;
        va_list args;

        // Get argument count.
        printf("va_start");
        va_start(args, fmt);
        length = vsnprintf(NULL, 0, fmt, args) +1;
        va_end(args);
        printf("va_end\n");

        // Resize the OUTPUT_DATA buffer in case there was already something in it.
        tempstr = (char*)realloc((char*)OutputVariables::OUTPUT_DATA, OutputVariables::sizeOutputData + length +1);
        if (tempstr) {
            printf("Failed to reallocate the OUTPUT_DATA buffer! Error code: %d\n", errno);
            return;
        }

        OutputVariables::OUTPUT_DATA = tempstr;
        memset((char*)OUTPUT_DATA + OutputVariables::outputDataOffset, 0, length +1); // Cleanup OUTPUT_DATA.

        // Now copy over the data to the buffer.
        va_start(args, fmt);
        length = vsnprintf(OUTPUT_DATA + OutputVariables::outputDataOffset, length, fmt, args);
        // update size and offset for next time.
        OutputVariables::sizeOutputData += length; 
        OutputVariables::outputDataOffset += length;
        va_end(args);

    }

};

#endif


// #ifndef OUTPUT_FORMATTER_H
// #define OUTPUT_FORMATTER_H

// class OutputFormatter {
// public:
//     OutputFormatter() {

//         OutputVariables::OUTPUT_DATA = (char*)malloc(10);
//         printf("[OF constr] OUTPUT_DATA location: %p\n", OUTPUT_DATA);
//     }

//     static void setOutputData(const char* fmt, ...) {

//         printf("[OF setter] setOutputdata data loc: %p\n", OutputVariables::OUTPUT_DATA);

//         int length = 0;
//         char* tempstr = NULL;
//         va_list args;

//         // Get argument count.
//         printf("va_start");
//         va_start(args, fmt);
//         length = vsnprintf(NULL, 0, fmt, args) +1;
//         va_end(args);
//         printf("va_end\n");

//         // Resize the OUTPUT_DATA buffer in case there was already something in it.
//         tempstr = (char*)realloc((char*)OutputVariables::OUTPUT_DATA, OutputVariables::sizeOutputData + length +1);
//         if (tempstr) {
//             printf("Failed to reallocate the OUTPUT_DATA buffer! Error code: %d\n", errno);
//             return;
//         }

//         OUTPUT_DATA = tempstr;
//         memset((char*)OUTPUT_DATA + outputDataOffset, 0, length +1); // Cleanup OUTPUT_DATA.

//         // Now copy over the data to the buffer.
//         va_start(args, fmt);
//         length = vsnprintf((char*)OUTPUT_DATA + outputDataOffset, length, fmt, args);
//         // update size and offset for next time.
//         sizeOutputData += length; 
//         outputDataOffset += length;
//         va_end(args);

//     }

//     static char* getOutputData(int* outputSize) {

//         printf("[OF getter] OUTPUT DATA: %p\n", OUTPUT_DATA);
        
//         char* outputData = (char*)OUTPUT_DATA;
//         *outputSize = sizeOutputData;

//         // Cleanup.
//         sizeOutputData = 0;
//         outputDataOffset = 0;
//         free((char*)OUTPUT_DATA);
//         return outputData;
//     }


// };



// #endif // !~ OUTPUT_FORMATTER_H

