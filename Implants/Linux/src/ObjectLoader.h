
#include "stdafx.h"

#include "Crypto.h"

#ifndef OBJECT_LOADER_H
#define OBJECT_LOADER_H

#define COMPILEDMACHINEARCH EM_X86_64
#define TRAMPOLINE_SIZE 255 * 12
#define THUNK_TRAMPOLINE "\x48\xb8\xEE\xEE\xEE\xEE\xEE\xEE\xEE\xEE\xff\xe0"
#define THUNK_TRAMPOLINE_SIZE 12
#define THUNKOFFSET 2

#define MEMFD_CREATE 319
inline int anonFile(const char* name, unsigned int flags) {
    return syscall(MEMFD_CREATE, name, flags);
}

class ObjectLoader {
private:
    int objectFd = 0;        // The memfd_create() object file descriptor.
    FILE* encObjFILE = NULL; // The encrypted object file descriptor.
    size_t objectSize = 0;

    // Object function to execute.
    typedef int (*functionPointer)();

    // Program and section header defines.
    Elf64_Ehdr* elfHeader;
    Elf64_Phdr* progHeader;
    Elf64_Shdr* sectHeader;

    size_t progHeaderCount;
    size_t sectHeaderCount;

    unsigned char** sectionMappings;
    int* sectionMappingsProtections;

    char* stringTable;
    char* sectionsStringTable;
    Elf64_Sym* symbolTable;

public:
    ObjectLoader() {

    }

    ~ObjectLoader() {

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

        // Load the decryped object data to memory.
        
        return true;
    }   
};

#endif // !~ OBJECT_LOADER_H