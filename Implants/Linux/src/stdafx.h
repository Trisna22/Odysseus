
#include <iostream>
#include <string.h>
#include <dlfcn.h>

// Check if the compiler gives us a implant ID else use the fake one.
#ifndef SLAVE_ID
    #define SLAVE_ID "TEST_SLAVE_ID"
#elif STRLEN(SLAVE_ID)== 0
    #define SLAVE_ID "TEST_SLAVE_ID2"
#endif
