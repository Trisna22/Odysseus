
#include "stdafx.h"

#include "Crypto.h"
#include "Executor.h"

#ifndef OBJECT_LOADER_H
#define OBJECT_LOADER_H

#define ENTRY_POINT "payload_init\0"

#if __GNUC__
#if __x86_64__ || __ppc64__
    #define ELF_R_SYM(i)			((i) >> 32)
    #define ELF_R_TYPE(i)			((i) & 0xffffffff)
    #define ELF_R_INFO(sym,type)    ((((Elf64_Xword) (sym)) << 32) + (type))
    
    // All x64 typedefs.
    typedef Elf64_Ehdr Elf_Ehdr;
    typedef Elf64_Phdr Elf_Phdr;
    typedef Elf64_Shdr Elf_Shdr;
    typedef Elf64_Sym Elf_Sym;
    typedef Elf64_Rela Elf_Rela;
    typedef Elf64_Rel Elf_Rel;
#else
    #define ELF_R_SYM(val)		    ((val) >> 8)
    #define ELF_R_TYPE(val)		    ((val) & 0xff)
    #define ELF_R_INFO(sym, type)	(((sym) << 8) + ((type) & 0xff))

    // All x32 typedefs.
    typedef Elf32_Ehdr Elf_Ehdr;
    typedef Elf32_Phdr Elf_Phdr;
    typedef Elf32_Shdr Elf_Shdr;
    typedef Elf32_Sym Elf_Sym;
    typedef Elf32_Rela Elf_Rela;
    typedef Elf32_Rel Elf_Rel;
#endif
#endif

#if defined(__amd64__) || defined(__x86_64__)
    #define THUNK_TRAMPOLINE "\x48\xb8\xEE\xEE\xEE\xEE\xEE\xEE\xEE\xEE\xff\xe0"
    #define THUNK_TRAMPOLINE_SIZE 12
    #define THUNKOFFSET 2
#endif

#if defined(__i386__)
    #define THUNK_TRAMPOLINE "\x68\x00\x00\x00\x00\x58\xff\xe0";
    #define THUNK_TRAMPOLINE_SIZE = 8;
    #define THUNKOFFSET 1
#endif    

#define MEMFD_CREATE 319
inline int anonFile(const char* name, unsigned int flags) {
    return syscall(MEMFD_CREATE, name, flags);
}

class ObjectLoader {
private:
    int objectFd = 0;        // The memfd_create() object file descriptor.
    FILE* encObjFILE = NULL; // The encrypted object file descriptor.
    size_t objectSize = 0;
    unsigned char* objData;

    // Object function to execute.
    typedef int (*functionPointer)();
    functionPointer entryPoint = NULL;

    // Program and section header defines.
    Elf_Ehdr* elfHeader;
    Elf_Phdr* progHeader;
    Elf_Shdr* sectHeader;

    size_t progHeaderCount;
    size_t sectHeaderCount;

    unsigned char** sectionMappings;
    int* sectionMappingsProtections;

    char* stringTable;
    char* sectionsStringTable;
    Elf_Sym* symbolTable;

    // Demangles function name to check if it is our entry point.
    bool demangleCheck(char* foo) {

        // Check if we see mangled encoding.
        if (!(foo[0] == '_' && foo[1] == 'Z' && foo[strlen(foo) -1] == 'v')) {
            return false;
        }

        int lenName = atoi(foo + 2); // Get length of function name.
        if (lenName != strlen(ENTRY_POINT)) {
            return false;
        }

        // Copy substring of mangled function.
        char funcName[lenName];
        memcpy(funcName, foo +4, lenName);
        funcName[lenName] = '\0';

        return (strncmp(ENTRY_POINT, funcName, lenName) == 0);
    }

    bool handleRelocations() {

        unsigned char* tempOffsetTable = (unsigned char*)mmap(NULL, 255* THUNK_TRAMPOLINE_SIZE, PROT_READ | PROT_WRITE, MAP_ANON | MAP_PRIVATE, -1, 0);
        int tempOffsetCounter = 0x5000;

        // Loop trough all sections and copy the progbits to virtual space.
        for (int i = 0; i < sectHeaderCount; i++) {

            int SECTION_PROTS = PROT_READ | PROT_WRITE;

            // Check memory permissions.
            if (sectHeader[i].sh_flags & 01) {
                SECTION_PROTS = PROT_READ | PROT_WRITE;
            }
            if (sectHeader[i].sh_flags & 04) {
                SECTION_PROTS = PROT_READ | PROT_EXEC;
            }

            // Check if section contains program bits.
            if (sectHeader[i].sh_size > 0 && sectHeader[i].sh_type == SHT_PROGBITS) {

                // Allocate space for our code bits.
                sectionMappings[i] = (unsigned char*)mmap(tempOffsetTable + tempOffsetCounter, sectHeader[i].sh_size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANON, -1, 0);
                tempOffsetCounter += 0x5000;
                if (sectionMappings[i] == NULL || sectionMappings[i] == MAP_FAILED) {
                    printf("Failed to allocate memory for section number %d\n", i);
                    return false;
                }

                // Copy over the progbits to our virtual memory.
                memcpy(sectionMappings[i], objData + sectHeader[i].sh_offset, sectHeader[i].sh_size);
            } else {
                sectionMappings[i] = NULL;
                // printf("No PROGBITS, so skipping this section...\n\n");
            }

            sectionMappingsProtections[i] = SECTION_PROTS;

            switch(sectHeader[i].sh_type) {
                case SHT_SYMTAB: {
                    symbolTable = (Elf_Sym*)(objData + sectHeader[i].sh_offset);
                    stringTable = (char*)(objData + sectHeader[sectHeader[i].sh_link].sh_offset);
                    break;
                }
                case SHT_STRTAB: {
                    sectionsStringTable = (char*)(objData + sectHeader[i].sh_offset);
                    break;
                }

                default: {
                    break;
                }
            }
        }

        int offsetCounterThunk = 0;
        bool foundEntry = false;

        for (int i = 0; i < sectHeaderCount; i++) {

            char* sectName = sectionsStringTable + sectHeader[i].sh_name;

            Elf_Rela* rel = (Elf_Rela*)(objData + sectHeader[i].sh_offset); 

            
            if (sectHeader[i].sh_type == SHT_RELA) {
                
                for (int j = 0; j < sectHeader[i].sh_size / sizeof(Elf_Rela); j++) {

                    char* relocStr = stringTable + symbolTable[ELF_R_SYM(rel[j].r_info)].st_name;
                    char workingTrampoline[THUNK_TRAMPOLINE_SIZE];
                    memcpy(workingTrampoline, THUNK_TRAMPOLINE, THUNK_TRAMPOLINE_SIZE);

                    // For symbols outside the object file, potentially external objects.
                    if (symbolTable[ELF_R_SYM(rel[j].r_info)].st_shndx == 0) {

                        // Retrieve the symbol from external object.
                        void* symAddr = dlsym(RTLD_DEFAULT, relocStr);
                        memcpy(workingTrampoline + THUNKOFFSET, &symAddr, sizeof(void*));

                        memcpy(tempOffsetTable + (offsetCounterThunk * THUNK_TRAMPOLINE_SIZE), workingTrampoline, THUNK_TRAMPOLINE_SIZE);

                        int32_t relativeOffsetFunc = 0;

                        #if defined(__amd64__) || defined(__x86_64__)

                            relativeOffsetFunc = (tempOffsetTable + 
                                (offsetCounterThunk * THUNK_TRAMPOLINE_SIZE)) - 
                                (sectionMappings[sectHeader[i].sh_info] + rel[j].r_offset) + 
                                rel[j].r_addend;

                                
                            memcpy(sectionMappings[sectHeader[i].sh_info] + rel[j].r_offset, &relativeOffsetFunc, 4);

                        #elif defined(__i386__)

                            memcpy(&relativeOffsetFunc, sectionMappings[sectHeader[i].sh_info] + rel[j].r_offset, 4);
                            relativeOffsetFunc += (tempOffsetTable + (offsetCounterThunk * THUNK_TRAMPOLINE_SIZE)) -
                                (sectionMappings[sectHeader[i].sh_info] + rel[j].r_offset);
                            memcpy(sectionMappings[sectHeader[i].sh_info + rel[j].r_offset, &relativeOffsetFunc, 4]);

                        #endif

                        offsetCounterThunk += 1;
                    }
                    else if (sectHeader[i].sh_flags == 0x40) {
                        
                        #if defined(__amd64__) || defined(__x86_64__)
                            int32_t relativeOffset = (sectionMappings[symbolTable[ELF_R_SYM(rel[j].r_info)].st_shndx]) -
                                (sectionMappings[sectHeader[i].sh_info] + rel[j].r_offset) + 
                                    rel[j].r_addend + symbolTable[ELF_R_SYM(rel[j].r_info)].st_value;

                        #elif defined(__i386__)

                            int32_t relativeOffset = 0;
                            if (ELF_R_TYPE(rel[j].r_info) == R_386_32) {

                                memcpy(&relativeOffset, sectionMappings[sectHeader[i].sh_info] + rel[j].r_offset, 4);
                                relativeOffset += elfinfo.symbolTable[ELF_R_SYM(rel[c2].r_info)].st_value;
                                relativeOffset += (int32_t)(elfinfo.sectionMappings[elfinfo.symbolTable[ELF_R_SYM(rel[c2].r_info)].st_shndx]);
                
                            }
                            else if (ELF_R_TYPE(rel[c2].r_info) == R_386_PC32) {
                                
                                memcpy(&relativeOffset, sectionMappings[sectHeader[i].sh_info] + rel[j].r_offset, 4);
                                relativeOffset = (sectionMappings[symbolTable[ELF_R_SYM(rel[j].r_info)].st_shnx]) - 
                                    (sectionMappings[sectHeader[i].sh_info] + rel[j].r_offset) + relativeOffset + symbolTable[ELF_R_SYM(rel[j].r_info)].st_value;
                            }   
                        #endif 
    
                        memcpy(sectionMappings[sectHeader[i].sh_info] + rel[j].r_offset, &relativeOffset, 4);
                    }
                }
            }

            // Handle the symbols here and get the entry point and all that.
            if (sectHeader[i].sh_type == SHT_SYMTAB) {
                for (int j = 0; j < sectHeader[i].sh_size / sizeof(Elf_Sym); j++) {

                    Elf_Sym* syms = (Elf_Sym*)(objData + sectHeader[i].sh_offset);
                    if (demangleCheck(stringTable + syms[j].st_name)) {
                        foundEntry = true;
                        entryPoint = (functionPointer)sectionMappings[syms[j].st_shndx] + syms[j].st_value;
                        break;
                    }
                }
            }
        }


        if (!foundEntry) {
            printf("Failed to find entry point!\n");
            return false;
        }

        // Add EXEC permissions to thunk table.
        if (mprotect(tempOffsetTable, 255* THUNK_TRAMPOLINE_SIZE, PROT_READ | PROT_EXEC) != 0) {
            printf("Failed to mprotect the thunk table!\n");
            return false;
        }

        // Set the original EXEC permissions back to the sections.
        for (int i = 0; i < sectHeaderCount; i++) {
            if (sectionMappings[i] != NULL) {
                if (mprotect(sectionMappings[i], sectHeader[i].sh_size, sectionMappingsProtections[i]) != 0) {
                    printf("Failed to set permission of section %d! Error code: %d\n", i, errno);
                    return false;
                }
            }
        }

        return true;
    }

public:
    ObjectLoader() {

    }

    ~ObjectLoader() {

        // Cleaning up the sections data.
        for (int i = 0; i < this->sectHeaderCount; i++) {

            if (sectionMappings[i] != NULL) {
                
                // Set all permissions to write before unmapping memory.
                mprotect(sectionMappings[i], sectHeader[i].sh_size, PROT_READ | PROT_WRITE);

                // Zero out memory.
                memset(sectionMappings[i], '\0', sectHeader[i].sh_size);
                                
                // Cleaning up per section.
                if (munmap(sectionMappings[i], sectHeader[i].sh_size) != 0) {
                    printf("Failed to unmap section header %d!\n");
                }
            }
        }

        // Freeing the section mapping variables.
        if (sectionMappings) {
            free(sectionMappings);
            sectionMappings = 0;
        }
        if (sectionMappingsProtections) {
            free(sectionMappingsProtections);
            sectionMappingsProtections = 0;
        }
        
        // Cleanup file object
        if (objData != NULL || objData != MAP_FAILED) {
            munmap(objData, this->objectSize);
        }

        // Delete the data from memory descriptors.
        ftruncate(this->objectFd, objectSize);
        write(this->objectFd, 0, objectSize);

        // Close memory descriptors.
        close(this->objectFd);
        fclose(this->encObjFILE);
    }

    // Allocates file descriptors.
    bool prepareObject(size_t objectSize) {

        if (objectSize <= 0) return false;

        this->objectFd = anonFile(".", 1);
        this->objectSize = objectSize;

        // Create the memory file.
        this->encObjFILE = fmemopen(NULL, objectSize, "wb+");
        if (this->encObjFILE == NULL) {
            printf("Failed to open memory file for encrypted object data!\n");
            return false;
        }

        // Truncate memory descriptors.
        fseek(this->encObjFILE, 0, SEEK_SET);
        ftruncate(this->objectFd, objectSize);
        return true;
    }

    // Wrapper for writing object data directly to memory file.
    int writeObject(unsigned char c) {
        return fputc(c, this->encObjFILE);
    }

    // Parses the object data.
    bool parseObject()
    {  
        rewind(this->encObjFILE); // Set cursor to start.

        // Decrypt the object data.
        if (!Crypto::decryptObject(this->encObjFILE, objectFd, this->objectSize)) {
            printf("Failed to decrypt given object!\n");
            return false;
        }

        lseek(objectFd, 0, SEEK_SET); // Rewind before reading.

        // Read the entire object.
        this->objData = (unsigned char*)mmap(NULL, this->objectSize, 
            PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANON, NULL, NULL); // Protect memory for other processes.
        if (this->objData == MAP_FAILED) {
            printf("Failed to allocate memory for objData! Error code: %d\n\n", errno);
            return false;
        }

        read(this->objectFd, this->objData, this->objectSize);
        this->elfHeader = (Elf_Ehdr*)this->objData;

        // Check if ELF relocation object file.
        if (this->elfHeader->e_type != ET_REL) {
            printf("Given file is not an ELF relocation object!\n");
            return false;
        }

        // Setting up program and section headers.
        this->progHeader = (Elf_Phdr*)(objData + elfHeader->e_phoff);
        this->sectHeader = (Elf_Shdr*)(objData + elfHeader->e_shoff);
        this->progHeaderCount = elfHeader->e_phnum;
        this->sectHeaderCount = elfHeader->e_shnum;

        // Setting up mappings for sections.
        sectionMappings = (unsigned char**)calloc(sectHeaderCount * sizeof(char*), 1);
        sectionMappingsProtections = (int*)calloc(sectHeaderCount* sizeof(int), 1);
        if (sectionMappings == NULL || sectionMappingsProtections == NULL) {
            printf("Failed to allocate memory for section mapping! Error code: %d\n", errno);
            return false;
        }
        
        return handleRelocations();
    }   

    int executeObject() {
        
        return entryPoint();
        // TODO
        // return executeFork(entryPoint);
    }
};

#endif // !~ OBJECT_LOADER_H