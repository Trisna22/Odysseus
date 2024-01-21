
#include "stdafx.h"

#ifndef ELFLOADER_H
#define ELFLOADER_H

#define TRAMPOLINE_SIZE 255 * 12
#define THUNK_TRAMPOLINE "\x48\xb8\xEE\xEE\xEE\xEE\xEE\xEE\xEE\xEE\xff\xe0"
#define THUNK_TRAMPOLINE_SIZE 12
// Program and section header defines.
Elf64_Ehdr* elfHeader;
Elf64_Phdr* progheader;
Elf64_Shdr* sectHeader;
size_t progHeaderCount;
size_t sectHeaderCount;

// Mapping for section data.
unsigned char** sectionMappings;
int* sectionMappingsProts;
char* stringTable;
char* sectionStringTable;
Elf64_Sym* symbolTable;
int (*funcPointer)(); // The function to execute.

bool handleRelocations(unsigned char* objData) {

    printf("\nHandling section headers:(%d)\n", elfHeader->e_shnum);
    unsigned char* tempOffsetTable = (unsigned char*)mmap(NULL, TRAMPOLINE_SIZE, PROT_READ | PROT_WRITE, MAP_ANON | MAP_PRIVATE, -1, 0); 
    int tempOffsetCounter = 0x5000;
    int tempOffsetCounter2 = 0;

    for (int i = 0; i < sectHeaderCount; i++) {

        int SECTION_PROTS = PROT_READ | PROT_WRITE;

        // printf("\nSection [%d]\n", i);

        // printf("  Type:    0x%x ", sectHeader[i].sh_type);
        // switch(sectHeader[i].sh_type) {
        //     case SHT_NULL:
        //         printf("NULL\n");break;
        //     case SHT_PROGBITS:
        //         printf("PROGBITS\n");break;
        //     case SHT_STRTAB:
        //         printf("STRTAB\n");break;
        //     case SHT_NOBITS:
        //         printf("SHT_NOBITS\n");break;
        //     case SHT_RELA:
        //         printf("RELA\n");break;
        //     case SHT_REL:
        //         printf("RELOC\n");break;
        //     case SHT_NOTE: 
        //         printf("NOTE");break;
        //     case SHT_DYNAMIC:
        //         printf("DYNAMIC\n");break;
        //     case SHT_SYMTAB:
        //         printf("SHT_SYMTAB\n");break;
        //     case SHT_DYNSYM:
        //         printf("SHT_DYNSYM\n");break;
        //     default:
        //         break;
        // }


        // printf("  Flags:   0x%lx\n", sectHeader[i].sh_flags);
        
        // Checking memory permissions.
        if (sectHeader[i].sh_flags & 01) {
            // printf("    PROT_READ | PROT_WRITE\n");
            SECTION_PROTS = PROT_READ | PROT_WRITE;
        }
        if (sectHeader[i].sh_flags & 04) {
            // printf("    PROT_READ | PROT_EXEC\n");
            SECTION_PROTS = PROT_READ | PROT_EXEC;
        }

        // Check if section contains program bits.
        if (sectHeader[i].sh_size > 0 &&
            sectHeader[i].sh_type == SHT_PROGBITS) {

            // Make space/allocate for our code bits.
            sectionMappings[i] = (unsigned char*)mmap(tempOffsetTable + tempOffsetCounter, sectHeader[i].sh_size, PROT_READ | PROT_WRITE, MAP_ANON | MAP_PRIVATE, -1, 0);
            tempOffsetCounter += 0x5000;
            if (sectionMappings[i] == NULL || sectionMappings[i] == (void*)-1) {
                printf("Failed to allocate memory for section number %d\n", i);
                return false;
            }

            // Copy over the progbits to our virtual memory.
            memcpy(sectionMappings[i], objData + sectHeader[i].sh_offset, sectHeader[i].sh_size);
        } else {
            // Don't allocate memory, since not needed.
            sectionMappings[i] = NULL;
        }

        sectionMappingsProts[i] = SECTION_PROTS;

        // Check remaining sections for tracking the important ones.
        switch (sectHeader[i].sh_type) {

            case SHT_SYMTAB: {
                printf("Found symbol and string table...\n");
                symbolTable = (Elf64_Sym*)(objData + sectHeader[i].sh_offset);
                stringTable = (char*)(objData + sectHeader[sectHeader[i].sh_link].sh_offset);
                break;
            }

            case SHT_STRTAB: {
                sectionStringTable = (char*)(objData + sectHeader[i].sh_offset);
                break;
            }
            default: {
                break;
            }
        }
    }

    printf("\nRelocating now...\n");

    for (int i = 0; i < sectHeaderCount; i++) {
        
        char* sectName = sectionStringTable  + sectHeader[i].sh_name;
        printf("Section %s[%d]:\n", sectName, i);

        Elf64_Rela* rel = (Elf64_Rela*)(objData + sectHeader[i].sh_offset);

        // Only check for relocations with addends.
        if (sectHeader[i].sh_type == SHT_RELA) {
            
            // Handle all relocations in this section.
            for (int j = 0; j < sectHeader[i].sh_size / sizeof(Elf64_Rel); j++) {

                char* relocStr = stringTable + symbolTable[ELF64_R_SYM(rel[j].r_info)].st_name;
                char workingTrampoline[TRAMPOLINE_SIZE];
                memcpy(workingTrampoline, THUNK_TRAMPOLINE, THUNK_TRAMPOLINE_SIZE);

                printf("  Symbol:    %s\n", relocStr);
                printf("  Type:      0x%lx\n", ELF64_R_TYPE(rel[j].r_info));

                // For functions that aren't defined in the object file.
                if (symbolTable[ELF64_R_SYM(rel[j].r_info)].st_shndx == 0) {

                    // Fuck this.
                }
                else if (sectHeader[i].sh_flags == 0x40) {

                    // God knows what they mean with this.
                    int32_t relativeOffset = (sectionMappings[symbolTable[ELF64_R_SYM(rel[j].r_info)].st_shndx]) -
                        (sectionMappings[sectHeader[i].sh_info] + rel[j].r_offset) + 
                            rel[j].r_addend + symbolTable[ELF64_R_SYM(rel[j].r_info)].st_value;

                    memcpy(sectionMappings[sectHeader[i].sh_info] + rel[j].r_offset, &relativeOffset, 4);    
                }
            }
        }

        // Don't know why it won't work...

        // Handle the symbols here and get the entry point and all that.
        if (sectHeader[i].sh_type == SHT_SYMTAB) {
            for (int j = 0; j < sectHeader[i].sh_size / sizeof(Elf64_Sym); j++) {

                Elf64_Sym* syms = (Elf64_Sym*)(objData + sectHeader[i].sh_offset);
                if (strcmp("_Z2gov", stringTable + syms[j].st_name) == 0) {
                    printf("FOUND go()!\n");
                    funcPointer = (int(*)())sectionMappings[syms[j].st_shndx] + syms[j].st_value;
                }
            }
        }
    }


    if (mprotect(tempOffsetTable, TRAMPOLINE_SIZE, PROT_READ | PROT_EXEC) != 0) {
        printf("Failed to mprotect the thunk table!\n");
        return false;
    }

    for (int i = 0; i < sectHeaderCount; i++) {
        if (sectionMappings[i] != NULL) {

            if (mprotect(sectionMappings[i], sectHeader[i].sh_size, sectionMappingsProts[i]) != 0) {
                printf("Failed to protect the function data!\n");
                return false;
            }
        }

    }
    funcPointer();
    return true;

}

// Parses the object data.
bool parseObject(unsigned char* objData, size_t objSize)
{
    if (!(objData[0] == '\x7f' && objData[1] == 'E' && 
        objData[2] == 'L' && objData[3] == 'F'))
    {
        printf("Given file is not an ELF file!\n");
        return false;
    }

    // Parse ELF header.
    elfHeader = (Elf64_Ehdr*)objData;
    if (elfHeader->e_type != ET_REL) {
        printf("Object file isn't a relocatable file type!\n");
        return false;
    } 

    printf("ELF header\n");
    printf("  Type:            %d\n", elfHeader->e_type);
    printf("  Machine:         %d\n", elfHeader->e_machine);
    printf("  Entry point:     0x%lx\n", elfHeader->e_entry);
    printf("  Program Header\n");
    printf("    Offset:        0x%lx\n", elfHeader->e_phoff);
    printf("    Size:          %d\n", elfHeader->e_phentsize);
    printf("    Count:         %d\n", elfHeader->e_phnum);
    printf("  Section Headers\n");
    printf("    Offset:        0x%lx\n", elfHeader->e_shoff);
    printf("    Sect. size:    %d\n", elfHeader->e_shentsize);
    printf("    Count:         %d\n", elfHeader->e_shnum);
    printf("    StringTab idx: %d\n", elfHeader->e_shstrndx);

    // Setting up program and section headers.
    progheader = (Elf64_Phdr*)(objData + elfHeader->e_phoff);
    sectHeader = (Elf64_Shdr*)(objData + elfHeader->e_shoff);
    progHeaderCount = elfHeader->e_phnum;
    sectHeaderCount = elfHeader->e_shnum;

    // Setting up mappings for sections.
    sectionMappings = (unsigned char**)calloc(sectHeaderCount * sizeof(char*), 1);
    sectionMappingsProts = (int*)calloc(sectHeaderCount * sizeof(int), 1);
    if (sectionMappingsProts == NULL || sectionMappings == NULL) {

        printf("Failed to allocate memory for section mapping! Error code: %d\n");
        return false;
    }

    // Now handle relocations based on parsed data.
    if (!handleRelocations(objData)) {
        printf("Relocations appear to have failed, so exiting...\n");
        return false;
    }
    
    

    
    return false;
}

#endif // !~ ELFLOADER_H