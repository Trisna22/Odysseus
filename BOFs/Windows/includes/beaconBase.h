
#include <string.h>
#include <stdarg.h>
#include <windows.h>

#ifndef BEACON_BASE_H
#define BEACON_BASE_H

void BPrintf(const char* str, ...);

unsigned char* InternalFunctions[2][2] = {
    {(unsigned char*)"__C_specific_handler", NULL},
	{(unsigned char*)"BPrintf", (unsigned char*)BPrintf}
};

void BPrintf(const char* str, ...) {

	printf("PRINTF FROM BeaconBase.h: %s\n", str);
}

#endif // !~ BEACON_BASE_H