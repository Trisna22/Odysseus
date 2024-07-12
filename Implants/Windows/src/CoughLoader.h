
#include "stdafx.h"

#ifndef COUGH_LOADER_H
#define COUGH_LOADER_H

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
    #define PREPENDSYMBOLVALUE "__imp__"
#endif

// typedef 


BOOL parseObject(unsigned char* objData, size_t fileSize)
{

    return FALSE;
}



#endif // !~ COUGH_LOADER_H