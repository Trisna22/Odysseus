
#include "stdafx.h"

#include "Crypto.h"

#ifndef OBJECT_LOADER_H
#define OBJECT_LOADER_H


#if __GNUC__
#if __x86_64__ || __ppc64__
    typedef Elf64_Ehdr Elf_Ehdr;
    typedef Elf64_Phdr Elf_Phdr;
    typedef Elf64_Shdr Elf_Shdr;
    typedef Elf64_Sym Elf_Sym;
    
#else
    typedef Elf32_Ehdr Elf_Ehdr;
    typedef Elf32_Phdr Elf_Phdr;
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

    bool handleRelocations() {

        printf("Handling section header relocations...(%d)\n", sectHeaderCount);

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

            switch(sectHeader[i].sh_type) {
                case SHT_SYMTAB: {
                    printf("Found symbol and string table in section %d\n", i);
                    symbolTable = (Elf_Sym*)(objData + sectHeader[i].sh_offset);
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

        printf("Relocating now...\n");

        
    }

public:
    ObjectLoader() {

    }

    ~ObjectLoader() {

        if (objData != NULL || objData != MAP_FAILED) {
            munmap(objData, this->objectSize);
            objData = NULL;
        }

        // Cleaning up the sections data.
        for (int i = 0; i < this->sectHeaderCount; i++) {

            if (sectionMappings != NULL && sectionMappings[i] != NULL) {
                
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
        return 0x22;
    }
};

#endif // !~ OBJECT_LOADER_H