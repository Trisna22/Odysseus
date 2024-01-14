
#include <elf.h>

#ifndef ELFLOADER_H
#define ELFLOADER_H

// Parses the object data.
int parseObject(unsigned char* objData, size_t objSize)
{
    // Parse ELF header.
    Elf64_Ehdr* elfHeader = (Elf64_Ehdr*)objData;
    
}

#endif // !~ ELFLOADER_H