
#include "stdafx.h"

#define KEY_SIZE 10
const char ALPHABET[] = {'a','b','c','d','e','f','g','h',
                        'i','j','k','l','m','n','o','p','q',
                        'r','s','t','u','v','w','x','y','z'};

unsigned char* readTarget(char* target, int fileSize) {
    
    FILE* f = fopen(target, "rb");
    if (f == NULL) {
        printf("Failed to open target file! Code: %d\n\n", errno);
        return NULL;
    }
    
    unsigned char* targetBytes = new unsigned char[fileSize];
    fread(targetBytes, fileSize, 1, f);
    fclose(f);

    return targetBytes;
}

// Creates random string of 10 characters.
unsigned char* generateString() {
    unsigned char* randomStr = new unsigned char[KEY_SIZE];

    for (int i = 0; i < 10; i++) {
        randomStr[i] = ALPHABET[rand() % 26];
    }

    randomStr[KEY_SIZE] ='\0';
    return randomStr;
}

// Encrypt bytes with key string.
unsigned char* encryptBytes(unsigned char* bytes, int size, unsigned char* key) {

    unsigned char* encrypted = new unsigned char[size];
    for (int i = 0; i < size; i++) {
        encrypted[i] = bytes[i] + key[i % KEY_SIZE];
    }

    return encrypted;
}

int main(int argc, char* argv[]) {
        
    if (argc != 2) {
        printf("Usage: %s [payload]\n", argv[0]);
        return 1;
    }

    struct stat info;
    stat(argv[1], &info);
    int fileSize = info.st_size;

    unsigned char* targetBytes = readTarget(argv[1], fileSize);
    if (targetBytes == NULL) {
        return 1;
    }

    // Generate random key.
    srand((unsigned)time(NULL) * getpid());     
    unsigned char* randomStr = generateString();
    printf("\n# Encrypting target file %s ...\n", argv[0]);
    printf("Key: %s\n", randomStr);

    // Encrypt target bytes with key.
    unsigned char* encrypted = encryptBytes(targetBytes, fileSize, randomStr);

    printf("Key written in './config/key.dat'\n");
    printf("Bin written in './config/encryptedPayload.bin'\n");

    FILE* out = fopen("./config/encryptedPayload.bin", "wb");
    fwrite(encrypted, fileSize, 1, out);
    fclose(out);

    FILE* key = fopen("./config/key.dat", "wb");
    fwrite(randomStr, KEY_SIZE, 1, key);
    fclose(key);

    // Done, let make compile the binary.
    printf("\n");
    return 0;
}