
#include "stdafx.h"

#ifndef HASHLIB_H
#define HASHLIB_H

namespace HashLib {
    // CreateThread() prototype.
    typedef HANDLE (WINAPI*pCreateThread)(
        LPSECURITY_ATTRIBUTES   lpThreadAttributes,
        SIZE_T                  dwStackSize,
        LPTHREAD_START_ROUTINE  lpStartAddress,
        __drv_aliasesMem LPVOID lpParameter,
        DWORD                   dwCreationFlags,
        LPDWORD                 lpThreadId
    ); 
    // VirtualProtect() prototype.
    typedef BOOL (WINAPI*pVirtualProtect)(
        LPVOID lpAddress,
        SIZE_T dwSize,
        DWORD  flNewProtect,
        PDWORD lpflOldProtect
    );
    // VirtualAlloc() prototype.
    typedef LPVOID (WINAPI* pVirtualAlloc)(
        LPVOID lpAddress,
        SIZE_T dwSize,
        DWORD  flAllocationType,
        DWORD  flProtect
    );    

    pVirtualAlloc vAlloc;
}



// Retrieves the hash of the function by name.
DWORD getFunctionHash(char* foo) 
{
    size_t stringLen = strnlen_s(foo, 60);
	DWORD hash = 0x22;

	for (int i = 0; i < stringLen; i++) {
		hash += (hash * 0xa82f3ef + foo[i]) & 0xffffff;
	}

	return hash;
}



#endif // !~ HASHLIB_H