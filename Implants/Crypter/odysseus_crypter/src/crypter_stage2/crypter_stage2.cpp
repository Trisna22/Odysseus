#include <iostream>
#include <string.h>
#include <stdio.h>

#include <unistd.h>
#include <sys/mman.h>
#include <elf.h>

/**
 * @brief Reads all section headers into a array.
 */
Elf64_Shdr* readAllSections(FILE* target, Elf64_Ehdr hdr) {
    
    Elf64_Shdr* allSections = new Elf64_Shdr[hdr.e_shnum];
    
    fseek(target, hdr.e_shoff, SEEK_SET);

    for (int i = 0; i < hdr.e_shnum; i++) {
        fread(&allSections[i], hdr.e_shentsize, 1, target);
    }

    return allSections;
}

/**
 * @brief Encrypts the hidden sections. 
 */
unsigned char* encryptBytes(unsigned char* bytes, int size) {
    
    unsigned char* encrypted = new unsigned char[size];
    for (int i = 0; i < size; i++) {
        encrypted[i] = bytes[i] +13;
    }
    return encrypted;
}

bool encryptSection(FILE* target, Elf64_Shdr secretSection) {

    // First locate the (encrypted) function bytes
    fseek(target, secretSection.sh_offset, SEEK_SET);
    unsigned char functionBytes[secretSection.sh_size];
    fread(functionBytes, secretSection.sh_size, 1, target);

    // Encrypt the section bytes.
    unsigned char* encrypted = encryptBytes(functionBytes, secretSection.sh_size);
    
    // Now write the encrypted bytes to file.
    fseek(target, secretSection.sh_offset, SEEK_SET);
    fwrite(encrypted, secretSection.sh_size, 1, target);

    return true;
}

/**
 * @brief Puts the sections string table into char* array.
 */
char* sectionStringTable(FILE* target, Elf64_Shdr section) {

    char* table = new char[section.sh_size];
    fseek(target, section.sh_offset, SEEK_SET);
    fread(table, section.sh_size, 1, target);
    return table;
}

bool encryptSection(char* progname) {

    Elf64_Ehdr elfHdr;
    Elf64_Shdr* allSections;

    FILE* target = fopen(progname, "r+");
    fread(&elfHdr, sizeof(Elf64_Ehdr), 1, target);

    // Get all sections and their names.
    allSections = readAllSections(target, elfHdr);
    char* stringTable = sectionStringTable(target, allSections[elfHdr.e_shstrndx]);

    // Search for our section.
    for (int i = 0; i < elfHdr.e_shnum; i++) {

        if (memcmp(stringTable + allSections[i].sh_name, ".secure\0", 8) == 0) {
            
            if (!encryptSection(target, allSections[i])) {
                printf("Failed to pack section!\n");
                return false;
            }
            printf("Succesfully encryped .secure section!\n");
        }     

        if (memcmp(stringTable + allSections[i].sh_name, ".secret\0", 8) == 0) {
            
            if (!encryptSection(target, allSections[i])) {
                printf("Failed to pack section!\n");
                return false;
            }
            printf("Succesfully encryped .secret section!\n");
        }      
    }

    fclose(target);
    return true;
}

int main(int argc, char* argv[]) 
{
    if (argc != 2) {

        printf("Usage: %s [malware]\n");
        return 1;
    }

    return !encryptSection(argv[1]);
}