#include <iostream>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <memoryapi.h>
#include <libloaderapi.h>
#include <windows.h>
#include <lm.h>
#include <dsgetdc.h>

#include "beaconBase.h"

#ifndef COUGH_H
#define COUGH_H

#define MACHINETYPE_AMD64 0x8664

// Counting __VA_ARGS__ number of arguments.
#define ELEVEN_ARGUMENTS(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, ...) a11
#define COUNT_ARGUMENTS(...) ELEVEN_ARGUMENTS(dummy, ## __VA_ARGS__, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0)

// Debug printing.
#define info(x, ...) { \
    if (COUNT_ARGUMENTS(__VA_ARGS__) == 0) { \
        printf("[i] %s\n", x); \
    } else { \
        printf("[i] "); \
        printf(x, ##__VA_ARGS__); \
        printf("\n"); \
    } \
}
#define warn(x, ...)  { \
    if (COUNT_ARGUMENTS(__VA_ARGS__) == 0) { \
        printf("[!] %s\n", x); \
    } else { \ 
        printf("[!] "); \
        printf(x, ##__VA_ARGS__); \
        printf("\n"); \
    } \
} 

// Structures for cough loading.

/**
 * COFF file header structure
 */
typedef struct COFF_FILE_HEADER {
    uint16_t machine;
    uint16_t numberOfSections;
    uint32_t timeDateStamp;
    uint32_t pointerToSymbolTable;
    uint32_t numberOfSymbols;
    uint16_t sizeOfOptionalHeader;
    uint16_t characteristics;
} coff_header_t;
// https://learn.microsoft.com/en-us/windows/win32/debug/pe-format#x64-processors 


#pragma pack(push,1)

/**
 * COFF section header structure
 */
typedef struct COFF_SECTION {
    char name[8];
    uint32_t virtualSize;
    uint32_t virtualAddress;
    uint32_t sizeOfRawData;
    uint32_t pointerToRawData;
    uint32_t pointerToRelocations;
    uint32_t pointerToLineNumbers;
    uint16_t numberOfRelocations;
    uint16_t numberOfLinenumbers;
    uint32_t characteristics;
} coff_section_t;
// https://learn.microsoft.com/en-us/windows/win32/debug/pe-format#section-table-section-headers

/**
 * COFF symbol table
 */
typedef struct COFF_SYMBOL_TABLE {
    union {
        char name[8];
        uint32_t value[2];
    } first;
    uint32_t value;
    uint16_t sectionNumber;
    uint16_t type;
    uint8_t storageClass;
    uint8_t numberOfAuxSymbols;

} coff_symbol_t;
// https://learn.microsoft.com/en-us/windows/win32/debug/pe-format#coff-symbol-table

/**
 * COFF relocation table
 */
typedef struct COFF_RELOC_TABLE {
    uint32_t virtualAddress;
    uint32_t symbolTableIndex;
    uint16_t type;
} coff_reloc_t;

#pragma pack(pop)

/* AMD64 Specific types */
#define IMAGE_REL_AMD64_ABSOLUTE            0x0000
#define IMAGE_REL_AMD64_ADDR64              0x0001
#define IMAGE_REL_AMD64_ADDR32              0x0002
#define IMAGE_REL_AMD64_ADDR32NB            0x0003
/* Most common from the looks of it, just 32-bit relative address from the byte following the relocation */
#define IMAGE_REL_AMD64_REL32               0x0004
/* Second most common, 32-bit address without an image base. Not sure what that means... */
#define IMAGE_REL_AMD64_REL32_1             0x0005
#define IMAGE_REL_AMD64_REL32_2             0x0006
#define IMAGE_REL_AMD64_REL32_3             0x0007
#define IMAGE_REL_AMD64_REL32_4             0x0008
#define IMAGE_REL_AMD64_REL32_5             0x0009
#define IMAGE_REL_AMD64_SECTION             0x000A
#define IMAGE_REL_AMD64_SECREL              0x000B
#define IMAGE_REL_AMD64_SECREL7             0x000C
#define IMAGE_REL_AMD64_TOKEN               0x000D
#define IMAGE_REL_AMD64_SREL32              0x000E
#define IMAGE_REL_AMD64_PAIR                0x000F
#define IMAGE_REL_AMD64_SSPAN32             0x0010
// https://learn.microsoft.com/en-us/windows/win32/debug/pe-format#x64-processors

/* i386 Relocation types */
#define IMAGE_REL_I386_ABSOLUTE             0x0000
#define IMAGE_REL_I386_DIR16                0x0001
#define IMAGE_REL_I386_REL16                0x0002
#define IMAGE_REL_I386_DIR32                0x0006
#define IMAGE_REL_I386_DIR32NB              0x0007
#define IMAGE_REL_I386_SEG12                0x0009
#define IMAGE_REL_I386_SECTION              0x000A
#define IMAGE_REL_I386_SECREL               0x000B
#define IMAGE_REL_I386_TOKEN                0x000C
#define IMAGE_REL_I386_SECREL7              0x000D
#define IMAGE_REL_I386_REL32                0x0014
// https://learn.microsoft.com/en-us/windows/win32/debug/pe-format#x64-processors

/* Section Characteristic Flags */
#define IMAGE_SCN_MEM_WRITE                 0x80000000
#define IMAGE_SCN_MEM_READ                  0x40000000
#define IMAGE_SCN_MEM_EXECUTE               0x20000000
#define IMAGE_SCN_ALIGN_16BYTES             0x00500000
#define IMAGE_SCN_MEM_NOT_CACHED            0x04000000
#define IMAGE_SCN_MEM_NOT_PAGED             0x08000000
#define IMAGE_SCN_MEM_SHARED                0x10000000
#define IMAGE_SCN_CNT_CODE                  0x00000020
#define IMAGE_SCN_CNT_UNINITIALIZED_DATA    0x00000080
#define IMAGE_SCN_MEM_DISCARDABLE           0x02000000
// https://learn.microsoft.com/en-us/windows/win32/debug/pe-format#section-flags

// Prepends value for symbols based on OS version 64 bit.
#if defined(__x86_64__) || defined(_WIN64)
    #define PREPENDSYMBOLVALUE "__imp_"
#else
    #define PREPENDSYMBOLVALUE "__imp_"
#endif
/**
 * Data variables we need for parsing object.
*/
COFF_FILE_HEADER* coffHeader;
COFF_SYMBOL_TABLE* coffSymbolTable;
COFF_RELOC_TABLE* coffRelocTable;
char** sectionMapping;
char* functionMapping;
int totalCountRelocations = 0;
int* sectionSize = NULL;

static bool startsWith(const char* str, const char* substr) {
    return strncmp(str, substr, strlen(substr)) == 0;
}

// Determines what function and library a symbol string is from and returns
// the right function pointer for it. Need to implement in the loading of the 
// beacon internal functions. 
void* processSymbol(char* symbolString) {

    char localCopy[1024] = {0};
    strncpy(localCopy, symbolString, sizeof(localCopy) -1);
    printf("Symbol to check: %s\n", symbolString);

    // Check if symbol is from internal functions.
    // if (startsWith(symbolString, PREPENDSYMBOLVALUE "Beacon") ||
    //     startsWith(symbolString, PREPENDSYMBOLVALUE "toWideChar") || 
    //     startsWith(symbolString, PREPENDSYMBOLVALUE "GetProcAddress") || 
    //     startsWith(symbolString, PREPENDSYMBOLVALUE "LoadLibraryA") ||
    //     startsWith(symbolString, PREPENDSYMBOLVALUE "GetModuleHandleA") || 
    //     startsWith(symbolString, PREPENDSYMBOLVALUE "FreeLibrary") ||
    //     startsWith(symbolString, "__C_specific_handler")) {


    if (startsWith(symbolString, PREPENDSYMBOLVALUE "BPrintf")) {
            char* internalFunc = NULL;
            if (strcmp(symbolString, "__C_specific_handler") == 0) {
                internalFunc = symbolString;
                return InternalFunctions[0][1];
            }
            else {
                internalFunc = symbolString + strlen(PREPENDSYMBOLVALUE);
            }

            printf("Internal function symbolname to check: %s\n", internalFunc);

            // Get the internal symbol and set the functionaddress variable.
            void* functionAddress = NULL;
            for (int i = 0; i < 30; i++) {
                if (InternalFunctions[i][0] != NULL) {
                    if (startsWith(internalFunc, (char*)(InternalFunctions[i][0]))) {
                        functionAddress = (void*)InternalFunctions[i][1];
                        return functionAddress; // Return the address of the function.
                    }
                }
            }
    }

    if (strncmp(symbolString, PREPENDSYMBOLVALUE, strlen(PREPENDSYMBOLVALUE)) == 0) {
        char* localLib = localCopy + strlen(PREPENDSYMBOLVALUE);

        localLib = strtok(localLib, "$");   // Get the library name from symbol.
        char* localFunc = strtok(NULL, "$");      // Get the function from within library.
        printf("Searching for function %s within %s\n", localFunc, localLib);

        // Lookup the function inside the library.
        HMODULE hHandle = LoadLibraryA(localLib);
        return (void*)GetProcAddress(hHandle, localFunc);
    }
    
    return NULL;
}

// Handles relocations dynamically.
int handleRelocations(unsigned char* objData) {

    printf("\nHANDLING RELOCATIONS\n\n");
    int functionMappingCount = 0;

    for (int i = 0; i < coffHeader->numberOfSections; i++) {
        COFF_SECTION* coffSection = (coff_section_t*)(objData + sizeof(coff_header_t) + (sizeof(coff_section_t) *i));
        COFF_RELOC_TABLE* coffReloc = (coff_reloc_t*)(objData + coffSection->pointerToRelocations);

        // Handle relocs one by one.
        printf("\n################## Relocating section %s (%d relocations)\n\n", coffSection->name, coffSection->numberOfRelocations);
        for (int j = 0; j < coffSection->numberOfRelocations; j++) {

            // WE could print the symbol name, but idk if we want to.
            // printf("  Symbol name:          %s\n", sectionMapping[coffReloc->symbolTableIndex]);
            printf("------------------------------ Relocation %d\n", j);
            printf("  Virtual address:      0x%X\n", coffReloc->virtualAddress);
            printf("  Symbol table index:   0x%X\n", coffReloc->symbolTableIndex);
            printf("  Type:                 0x%X\n\n", coffReloc->type);
         
            // Check if symbol has valid name or just null.
            if (coffSymbolTable[coffReloc->symbolTableIndex].first.name[0] != 0) {
                
                uint32_t symbol = coffSymbolTable[coffReloc->symbolTableIndex].first.value[1];
                printf("- Symbol pointer: 0x%X\n", symbol);
                printf("- Symbol name:    %s\n", coffSymbolTable[coffReloc->symbolTableIndex].first.name);
                printf("- Section numner: %d\n", coffSymbolTable[coffReloc->symbolTableIndex].sectionNumber);

                // Check if 64 bit binary
                #ifdef _WIN32
                #ifdef _WIN64
                          
                    switch(coffReloc->type) {
                        case IMAGE_REL_AMD64_ADDR64: {
                            printf("- Reloc type:     IMAGE_REL_AMD64_ADDR64\n");
                            
                            uint64_t offsetValue;
                            memcpy(&offsetValue, sectionMapping[i] + coffReloc->virtualAddress, sizeof(uint64_t));
                            printf("READING offset value : 0x%llX\n", offsetValue);
                            offsetValue = (uint64_t)(sectionMapping[
                                coffSymbolTable[coffReloc->symbolTableIndex].sectionNumber -1
                                ] + (uint64_t)offsetValue);
                                  
                            offsetValue += coffSymbolTable[coffReloc->symbolTableIndex].value;
                            printf("Modified offset value: 0x%llX Base Address: %p\n", offsetValue, sectionMapping[coffSymbolTable[coffReloc->symbolTableIndex].sectionNumber - 1]);
                            memcpy(sectionMapping[i] + coffReloc->virtualAddress, &offsetValue, sizeof(uint64_t));
                            break;
                        }
                        case IMAGE_REL_AMD64_ADDR32NB: {
                            printf("- Reloc type:     IMAGE_REL_AMD64_ADDR32NB\n");

                            size_t offsetValue = 0;
                            memcpy(&offsetValue, sectionMapping[i] + coffReloc->virtualAddress, sizeof(int32_t));

                            if (((sectionMapping[coffSymbolTable[coffReloc->symbolTableIndex].sectionNumber -1] + offsetValue) - 
                                (sectionMapping[i] + coffReloc->virtualAddress + 4)) > 0xffffffff) {
                                
                                printf("Relocations > 4 gigs away, failed to relocate, stopping!\n\n");
                                return 0;
                            }

                            offsetValue = (sectionMapping[coffSymbolTable[coffReloc->symbolTableIndex].sectionNumber -1] + offsetValue) -
                                (sectionMapping[i] + coffReloc->virtualAddress + 4);
                            offsetValue += coffSymbolTable[coffReloc->symbolTableIndex].value;
                            memcpy(sectionMapping[i] + coffReloc->virtualAddress, &offsetValue, sizeof(uint32_t));
                            break;
                        }
                        case IMAGE_REL_AMD64_REL32: {
                            printf("- Reloc type:     IMAGE_REL_AMD64_REL32\n");

                            uint64_t offsetValue = 0;
                            memcpy(&offsetValue, sectionMapping[i] + coffReloc->virtualAddress, sizeof(uint32_t));

                            printf("Offset value:   0x%X\n", offsetValue);
                            
                            
                            if ((sectionMapping[coffSymbolTable[coffReloc->symbolTableIndex].sectionNumber -1] - 
                                (sectionMapping[i] + coffReloc->virtualAddress + 4)) > 0xffffffff) {
                                
                                printf("Relocations > 4 gigs away, failed to relocate, stopping!\n\n");
                                return 0;
                            }

                            offsetValue += (sectionMapping[coffSymbolTable[coffReloc->symbolTableIndex].sectionNumber -1] -
                                (sectionMapping[i] + coffReloc->virtualAddress + 4));
                            offsetValue += coffSymbolTable[coffReloc->symbolTableIndex].value;
                            printf("Setting 0x%p to relative addr 0x%X\n", sectionMapping[i] + coffReloc->virtualAddress, offsetValue);
                            memcpy(sectionMapping[i] + coffReloc->virtualAddress, &offsetValue, sizeof(uint32_t));
                            break;
                        }
                        case IMAGE_REL_AMD64_REL32_1: {
                            printf("- Reloc type:     IMAGE_REL_AMD64_REL32_1\n");
                            printf("Cannot perfom this relocation yet!\n");
                            return 0;
                            break;
                        }
                        case IMAGE_REL_AMD64_REL32_2: {
                            printf("- Reloc type:     IMAGE_REL_AMD64_REL32_2\n");
                            printf("Cannot perfom this relocation yet!\n");
                            break;
                        }
                        case IMAGE_REL_AMD64_REL32_3: {
                            printf("- Reloc type:     IMAGE_REL_AMD64_REL32_3\n");
                            printf("Cannot perfom this relocation yet!\n");
                            break;
                        }
                        case IMAGE_REL_AMD64_REL32_4: {
                            printf("- Reloc type:     IMAGE_REL_AMD64_REL32_4\n");
                            printf("Cannot perfom this relocation yet!\n");
                            break;
                        }
                        case IMAGE_REL_AMD64_REL32_5: {
                            printf("- Reloc type:     IMAGE_REL_AMD64_REL32_5\n");
                            printf("Cannot perfom this relocation yet!\n");
                            break;
                        }
                        default: {
                            printf("No code for relocation type %d!\n", coffReloc->type);
                            break;
                        }
                    } 
                #else // 32 bit relocations.
                    switch (coffReloc->type == IMAGE_REL_I386_DIR32) {
                        case IMAGE_REL_I386_DIR32: {
                            break;
                        }
                        case IMAGE_REL_I386_REL32: {
                            
                            break;
                        }
                        default: {
                            printf("No code for relocation type %d!\n", coffReloc->type);
                            break;
                        }
                    }
                #endif
                #endif
            }
            else {
                // Symbol name not present in name, but need to use offset.
                uint32_t symbol = coffSymbolTable[coffReloc->symbolTableIndex].first.value[1];
                printf("- Symbol pointer: 0x%X\n", symbol);
                printf("- Symbol value:   %s\n", coffSymbolTable[coffReloc->symbolTableIndex].first.name);
                printf("- Symbol name:    %s\n", (coffSymbolTable + coffHeader->numberOfSections) + symbol);
                printf("- Section numner: %d\n", coffSymbolTable[coffReloc->symbolTableIndex].sectionNumber);


                printf("PROCES SYMBOOOOOOO:     %s\n", ((char*)(coffSymbolTable + coffHeader->numberOfSymbols)) + symbol);
                void* funcLocation = processSymbol(((char*)(coffSymbolTable + coffHeader->numberOfSymbols)) + symbol);
                if (funcLocation == NULL && coffSymbolTable[coffReloc->symbolTableIndex].sectionNumber == 0) {
                    warn("Failed to resolve symbol!");
                    return 0;
                }

                // Check if 64 bit binary
                #ifdef _WIN32
                #ifdef _WIN64
                    if (coffReloc->type == IMAGE_REL_AMD64_ADDR64) {
                        printf("- Reloc type:     IMAGE_REL_AMD64_ADDR64\n");

                        uint64_t offsetValue;
                        memcpy(&offsetValue, sectionMapping[i] + coffReloc->virtualAddress, sizeof(uint64_t));

                        offsetValue = (uint64_t)(sectionMapping[coffSymbolTable[coffReloc->symbolTableIndex].sectionNumber -1] + (uint64_t)offsetValue);
                        offsetValue += coffSymbolTable[coffReloc->symbolTableIndex].value;

                        memcpy(sectionMapping[i] + coffReloc->virtualAddress, &offsetValue, sizeof(uint64_t));
                    }  
                    else if (coffReloc->type == IMAGE_REL_AMD64_REL32 && funcLocation != NULL) {
                        printf("- Reloc type:     IMAGE_REL_AMD64_REL32 && funcLocation\n");

                        if (((functionMapping + (functionMappingCount * 8)) - (sectionMapping[i] + coffReloc->virtualAddress + 4)) > 0xffffffff) {
                            printf(" Relocations > 4 gigs away, exiting...\n" );
                            return 0;
                        }

                        memcpy(functionMapping + (functionMappingCount * 8), &funcLocation, sizeof(uint64_t));
                        size_t offsetValue = (int32_t)((functionMapping + (functionMappingCount * 8)) - (sectionMapping[i] + coffReloc->virtualAddress + 4));
                        offsetValue += coffSymbolTable[coffReloc->symbolTableIndex].value;

                        memcpy(sectionMapping[i] + coffReloc->virtualAddress, &offsetValue, sizeof(uint32_t));
                        functionMappingCount++;
                        
                    }
                    else if (coffReloc->type == IMAGE_REL_AMD64_REL32) {
                        printf("- Reloc type:     IMAGE_REL_AMD64_REL32\n");

                        size_t offsetValue;
                        memcpy(&offsetValue, sectionMapping[i] + coffReloc->virtualAddress, sizeof(int32_t));
                        if ((sectionMapping[coffSymbolTable[coffReloc->symbolTableIndex].sectionNumber -1] - 
                            (sectionMapping[i] + coffReloc->virtualAddress + 4)) > 0xffffffff) {
                            printf(" Relocations > 4 gigs away, exiting...\n" );
                            return 0;
                        }

                        offsetValue += (sectionMapping[coffSymbolTable[coffReloc->symbolTableIndex].sectionNumber -1] - (sectionMapping[i] + coffReloc->virtualAddress + 4));
                        offsetValue += coffSymbolTable[coffReloc->symbolTableIndex].value;

                        memcpy(sectionMapping[i] + coffReloc->virtualAddress, &offsetValue, sizeof(uint32_t));
                    }
                    else if (coffReloc->type == IMAGE_REL_AMD64_ADDR32NB) {
                        printf("- Reloc type:     IMAGE_REL_AMD64_ADDR32NB\n");

                        size_t offsetValue;
                        memcpy(&offsetValue, sectionMapping[i] + coffReloc->virtualAddress, sizeof(int32_t));
                        if (((char*)(sectionMapping[coffSymbolTable[coffReloc->symbolTableIndex].sectionNumber -1] + offsetValue) - 
                            (char*)(sectionMapping[i] + coffReloc->virtualAddress + 4)) > 0xffffffff) {
                            printf(" Relocations > 4 gigs away, exiting...\n" );
                            return 0;                            
                        }

                        offsetValue = ((char*)(sectionMapping[coffSymbolTable[coffReloc->symbolTableIndex].sectionNumber -1] + offsetValue) - 
                            (char*)(sectionMapping[i] + coffReloc->virtualAddress + 4));
                        offsetValue += coffSymbolTable[coffReloc->symbolTableIndex].value;
                        memcpy(sectionMapping[i] + coffReloc->virtualAddress, &offsetValue, sizeof(uint32_t));
                    }
                    else {
                        printf("No code for relocation type %d!\n", coffReloc->type);
                    }

                // 32 bit binary.
                #else 
                
                    if (coffReloc->type == IMAGE_REL_I386_DIR32 && funcLocation != NULL) {
                        printf("- Reloc type:     IMAGE_REL_I386_DIR32 && funcLocation\n");
                    }
                    else if (coffReloc->type == IMAGE_REL_I386_DIR32) {
                        printf("- Reloc type:     IMAGE_REL_I386_DIR32\n");
                    }
                    else if (coffReloc->type == IMAGE_REL_I386_REL32) {
                        printf("- Reloc type:     IMAGE_REL_I386_REL32\n");
                    }
                    else {
                        printf("No code for relocation type %d!\n", coffReloc->type);
                    }
                #endif
                #endif
            }

            // Iterate to next relocation inside this section.
            coffReloc = (coff_reloc_t*)(((char*)coffReloc) + sizeof(coff_reloc_t));
        }
    }

    for (int i = 0; i < coffHeader->numberOfSections; i++) {
        if (sectionMapping[i] != NULL) {
            printf("Section %d (%d bytes)\n", i, sectionSize[i]);
            for (int j = 0; j < sectionSize[i]; j++) {
                if (j > 0 && j % 16 == 0) {
                    printf("\n%02x ", (uint8_t)(sectionMapping[i][j]));
                } else{
                    printf("%02x ", (uint8_t)(sectionMapping[i][j]));
                }
            }
            printf("\n");
        }
    }

    printf("################ EXECUTING CODE ################\n\n");

    for (int i = 0; i < coffHeader->numberOfSymbols; i++) {

        // Look for entry function. (default: payload)
        if (strcmp(coffSymbolTable[i].first.name, "go") == 0) {
            
            // Add debug trap.
            // sectionMapping[coffSymbolTable[i].sectionNumber-1][coffSymbolTable[i].value] = '\xcc';
    
            int(*foo)(void) = (int(*)())(sectionMapping[coffSymbolTable[i].sectionNumber -1] + coffSymbolTable[i].value);
            printf("Found entry point for payload... %p\n", foo);
            printf("RETURN VALUE: %d\n", foo());
        }
    }

    return 0;
}

// Parses the object code.
int parseObject(unsigned char* objData, size_t fileSize) {

    // Parse COFF header.
    coffHeader = (coff_header_t*)objData;
    info("COFF header");
    printf("  Machine:              0x%x\n", coffHeader->machine);
    printf("  Numer of sections:    %d\n", coffHeader->numberOfSections);
    printf("  TimeDateStamp:        %X\n", coffHeader->timeDateStamp);
    printf("  Number of symbols:    %d\n", coffHeader->numberOfSymbols);
    printf("  Optional header size: %d\n", coffHeader->sizeOfOptionalHeader);
    printf("  Characteristics:      %d\n", coffHeader->characteristics);
    printf("  Pointer symbol table: 0x%x (%d bytes)\n\n", coffHeader->pointerToSymbolTable, coffHeader->pointerToSymbolTable);

    // Create memory for section mapping.
    sectionMapping = (char**)calloc(sizeof(char*) *(coffHeader->numberOfSections +1), 1);
    sectionSize = (int*)calloc(sizeof(int) * (coffHeader->numberOfSections +1), 1);
    if (sectionMapping == NULL) {
        warn("Failed to allocate memory for section mapping! Error code: %d", GetLastError());
        return 0;
    }
 
    // Parse symbol table.
    coffSymbolTable = (coff_symbol_t*)(objData + coffHeader->pointerToSymbolTable);
    
    // Handle allocation and copying of the sections.
    for (int i = 0; i < coffHeader->numberOfSections; i++) {
        COFF_SECTION* coffSection = (coff_section_t*)(objData + sizeof(coff_header_t) + sizeof(coff_section_t) * i);
        // info("COFF section #%d", i);
        // printf("- Name:                %s\n", coffSection->name);
        // printf("- Virtual size:        0x%X\n", coffSection->virtualSize);
        // printf("- Virtual address:     0x%X (%d bytes)\n", coffSection->virtualSize, coffSection->virtualSize);
        // printf("- Raw data size:       0x%X (%d bytes)\n", coffSection->sizeOfRawData, coffSection->sizeOfRawData);
        // printf("- Pointer raw data:    0x%X (%d bytes)\n", coffSection->pointerToRawData, coffSection->pointerToRawData);
        // printf("- Number relocations:  %d\n", coffSection->numberOfRelocations);
        // printf("- Pointer relocations: 0x%X (%d bytes)\n", coffSection->pointerToRelocations, coffSection->pointerToRelocations);
        // printf("- Characteristics:     0x%X\n", coffSection->characteristics);

        totalCountRelocations += coffSection->numberOfRelocations; // Add up to total relocs.

        // Skip allocating memory if there is no data to be copied.
        if (coffSection->sizeOfRawData == 0)  {
            info("Skip allocating memory for section %s\n", coffSection->name);
            continue;
        }

        // Allocate memory for setions.
        sectionMapping[i] = (char*)VirtualAlloc(NULL, coffSection->sizeOfRawData, MEM_COMMIT | MEM_RESERVE | MEM_TOP_DOWN, PAGE_EXECUTE_READWRITE);
        sectionSize[i] = coffSection->sizeOfRawData;
        if (sectionMapping[i] == NULL) {
            warn("Failed to allocate memory for raw data of sections! Error code: %d", GetLastError());
            return 0;
        }

        // Copy section raw data to allocated memory.
        info("Section %s [%d] allocated at %p\n", coffSection->name, i, sectionMapping[i]);
        if (coffSection->pointerToRawData != 0) {
            memcpy(sectionMapping[i], objData + coffSection->pointerToRawData, coffSection->sizeOfRawData);
        } else {
            // If there is no raw data, just set it to zero.
            memset(sectionMapping[i], 0, coffSection->sizeOfRawData);
        }
    }

    info("Object got %d relocations.", totalCountRelocations);

    // Allocating memory for function relocations.
    functionMapping = (char*)VirtualAlloc(NULL, totalCountRelocations * 8, MEM_COMMIT | MEM_RESERVE | MEM_TOP_DOWN, PAGE_EXECUTE_READWRITE);
    if (functionMapping == NULL) {
        warn("Failed to allocate memory for function mapping! Error code: %d\n", GetLastError());
        return 0;
    }

    // Parse and handle relocations.
    handleRelocations(objData);
    return 0;
}

#endif // !~ COUGH_H