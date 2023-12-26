
#include "stdafx.h"
// Resources: https://anti-debug.checkpoint.com/


/**
 * PEB->BeingDebugged check.
 */
int checkPEBDebugFlag() {
    PPEB peb = (PPEB)__readgsqword(0x60);
    return (int)peb->BeingDebugged;
}

/**
 * PEB->dwNtGlobalFlag contains NT_GLOBAL_FLAG_DEBUGGED
 */
#define FLG_HEAP_ENABLE_TAIL_CHECK   0x10
#define FLG_HEAP_ENABLE_FREE_CHECK   0x20
#define FLG_HEAP_VALIDATE_PARAMETERS 0x40
#define NT_GLOBAL_FLAG_DEBUGGED (FLG_HEAP_ENABLE_TAIL_CHECK | \
     FLG_HEAP_ENABLE_FREE_CHECK | FLG_HEAP_VALIDATE_PARAMETERS)
     
int checkNtGlobalFlag() {
    PPEB peb = (PPEB)__readgsqword(0x60);
    DWORD dwNtGlobalFlag = *(PDWORD)((PBYTE)peb + 0xBC);
    return (dwNtGlobalFlag & NT_GLOBAL_FLAG_DEBUGGED);
}

/**
 * Checking for specific bytes (breakpoints) in functions.
 */
BOOL checkBreakpoints(BYTE cByte, PVOID pMemory)
{
    PBYTE pBytes = (PBYTE)pMemory;
    for (int i =0; ; i++) {

        // Break on RET (0xc3), because we don' t know the function' s size.
        if (pBytes[i] == 0xc3) {
            break;
        }

        if (pBytes[i] == cByte) {
            return TRUE;
        }
    }

    return FALSE;
}

/**
 * Checking for function checksums, if changes breakpoint was added. 
 */
// Calculate the size of the function.
size_t getFunctionSize(PVOID func) {
    PBYTE pMem = (PBYTE)func;
    size_t nFuncSize = 0;
    do
    {
        ++nFuncSize;
    } while (*(pMem++) != 0xC3);
    return nFuncSize;
}
// Calculate the hash for the function.
DWORD getFuncHash(PVOID pMemory, size_t memSize) {

    DWORD hash = 0x22;
    PBYTE pInst = (PBYTE)pMemory;

    for (size_t i = 0; i < memSize; i++) {
        hash += (hash * 0x93412 + (pInst[i]) & 0xfffffff);
    }
    return hash;
}
// Thread to keep checking the function integrity.
DWORD origHash;
PVOID functionToCheck;
size_t sizeFoo;
static DWORD WINAPI functionHashLoop(LPVOID _) 
{
    for (;;) {
        if (origHash != getFuncHash(functionToCheck, sizeFoo)) {
            printf("Function integrity failed, function hash changed!\n\n");
            exit(1);
        }
        Sleep(2000);
    }
}
BOOL checkFunctionHash(PVOID pMemory) {

    functionToCheck = pMemory;
    sizeFoo = getFunctionSize(pMemory);
    origHash = getFuncHash(pMemory, sizeFoo);
    
    // Loop to check if the function hash changes.
    CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)functionHashLoop, NULL, NULL, NULL);

    return TRUE;
}