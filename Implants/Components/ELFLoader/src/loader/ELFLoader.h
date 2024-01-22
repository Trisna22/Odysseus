
#include "stdafx.h"

#ifndef ELFLOADER_H
#define ELFLOADER_H

#define TRAMPOLINE_SIZE 255 * 12
#define THUNK_TRAMPOLINE "\x48\xb8\xEE\xEE\xEE\xEE\xEE\xEE\xEE\xEE\xff\xe0"
#define THUNK_TRAMPOLINE_SIZE 12
#define THUNKOFFSET 1
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
    int tempOffsetCounterElf = 0;

    for (int i = 0; i < sectHeaderCount; i++) {

        printf("Section number %d:\n\n", i);

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
            printf("Skipping this section!\n");
        }


        sectionMappingsProts[i] = SECTION_PROTS;

        printf("\tName is %d\n", sectHeader[i].sh_name);
        printf("\tType is %d\n", sectHeader[i].sh_type);
        printf("\tAddr is 0x%lx\n", sectHeader[i].sh_addr);
        printf("\tOffset is 0x%lx\n", sectHeader[i].sh_offset);
        printf("\tSize is %ld\n", sectHeader[i].sh_size);
        printf("\tLink is %d\n", sectHeader[i].sh_link);
        printf("\tInfo is %d\n", sectHeader[i].sh_info);
        printf("\tAddrAlign is %ld\n", sectHeader[i].sh_addralign);
        printf("\tEntSize is %ld\n", sectHeader[i].sh_entsize);

        // Check remaining sections for tracking the important ones.
        switch (sectHeader[i].sh_type) {

            case SHT_SYMTAB: {
                printf("Found symbol and string table...\n");
                symbolTable = (Elf64_Sym*)(objData + sectHeader[i].sh_offset);
                stringTable = (char*)(objData + sectHeader[sectHeader[i].sh_link].sh_offset);
                printf("  SymbolTable: %p\n", symbolTable);
                printf("  StringTable: %p\n", stringTable);
                break;
            }

            case SHT_STRTAB: {
                sectionStringTable = (char*)(objData + sectHeader[i].sh_offset);
                break;
            }
            default: {
                printf("\t\tCase not handled!\n");
                break;
            }
        }
    }

    printf("\nRelocating now...\n");

    for (int i = 0; i < sectHeaderCount; i++) {
        
        char* sectName = sectionStringTable  + sectHeader[i].sh_name;
        printf("Section %s[%d]:\n", sectName, i);

        Elf64_Rela* rel = (Elf64_Rela*)(objData + sectHeader[i].sh_offset);

        printf("Type: %d\n", sectHeader[i].sh_type);
        // Only check for relocations with addends.
        if (sectHeader[i].sh_type == SHT_REL) {
            
            /// IDK why but code will skip this statement!...
            // Type = 4, needs to be 9.

            
            // Handle all relocations in this section.
            for (int j = 0; j < sectHeader[i].sh_size / sizeof(Elf64_Rel); j++) {

                printf("\n  Relocation %d from section[%d]\n", j, i);

                char* relocStr = stringTable + symbolTable[ELF64_R_SYM(rel[j].r_info)].st_name;
                char workingTrampoline[TRAMPOLINE_SIZE];
                memcpy(workingTrampoline, THUNK_TRAMPOLINE, THUNK_TRAMPOLINE_SIZE);

                printf("  Symbol:    %s\n", relocStr);
                printf("  Type:      0x%lx\n", ELF64_R_TYPE(rel[j].r_info));
                printf("  Offset:    0x%lx\n", rel[j].r_offset);
                printf("  Addend:    0x%lx\n", rel[j].r_addend);

                // For functions that aren't defined in the object file, potentially external object.
                if (symbolTable[ELF64_R_SYM(rel[j].r_info)].st_shndx == 0) {

                    
                    void* symAddr = dlsym(RTLD_DEFAULT, relocStr);
                    if (symAddr == NULL) {
                        printf("Failed to find the relocation function!\n");
                        return false;
                    }

                    printf("    Function addr found:   %p\n", symAddr);
                    memcpy(workingTrampoline + THUNKOFFSET, &symAddr, sizeof(void*));

                    printf("  Temp offset counter: %d\n", tempOffsetCounterElf);
                    memcpy((void*)(tempOffsetCounterElf + (tempOffsetCounterElf * THUNK_TRAMPOLINE_SIZE)), workingTrampoline, THUNK_TRAMPOLINE_SIZE);

                    int32_t relativeOffset = (tempOffsetTable + (tempOffsetCounter * THUNK_TRAMPOLINE_SIZE)) - 
                            (sectionMappings[sectHeader[i].sh_info] + rel[j].r_offset) + rel[j].r_addend;

                    printf("\t\tFirstAddress: %p\n", (sectionMappings[symbolTable[ELF64_R_SYM(rel[j].r_info)].st_shndx]+rel[j].r_addend));
                    printf("\t\tSecondAddress(NoOffset): %p\n", (sectionMappings[sectHeader[i].sh_info]));
                    printf("\t\tSecondAddress: %p\n", (sectionMappings[sectHeader[i].sh_info]+rel[j].r_offset));
                    printf("\t\tRelativeOffset: 0x%x\n", relativeOffset);

                    /* Copy over the relative offset of the value to the section+offset */
                    memcpy(sectionMappings[sectHeader[i].sh_info] + rel[j].r_offset, &relativeOffset, 4);
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

        // Handle the symbols here and get the entry point and all that.
        if (sectHeader[i].sh_type == SHT_SYMTAB) {
            for (int j = 0; j < sectHeader[i].sh_size / sizeof(Elf64_Sym); j++) {

                Elf64_Sym* syms = (Elf64_Sym*)(objData + sectHeader[i].sh_offset);
                if (strcmp("go", stringTable + syms[j].st_name) == 0) {
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
        printf("Section #%d is mapped at %p\n", i, sectionMappings[i]);
        if (sectionMappings[i] != NULL) {

            if (mprotect(sectionMappings[i], sectHeader[i].sh_size, sectionMappingsProts[i]) != 0) {
                printf("Failed to protect the function data!\n");
                return false;
            }
        }

    }

    printf("Trying to run go()\n");
    funcPointer();
    printf("Returned from go()\n");
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