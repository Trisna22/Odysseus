
#include <stdio.h>
#include <string.h>

#include <windows.h>

#pragma comment(lib, "user32.lib")

DWORD getFuncHash(char* foo) {
	size_t stringLen = strnlen_s(foo, 60);
	DWORD hash = 0x35;

	for (int i = 0; i < stringLen; i++) {
		hash += (hash * 0xab10f29f + foo[i]) & 0xffffff;
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

        /// We wrote the code wrong somehwere.....

        if (functionNameHash == hash) {
            // If the has is found.
            DWORD functionAddressRVA = addrFunctionsRVA[addrOrdinalsRVA[i]];
            PWORD functionAddr = (PWORD)((DWORD_PTR)libraryBase + functionAddressRVA);
            printf("\nFound function %s (0x%x): %p\n", functionName, functionNameHash, functionAddr);
            return functionAddr;
        }
    } 

    printf("Failed to find function with hash!\n");
    return NULL;
}

typedef int (WINAPI* msgbox)(HWND,LPCSTR,LPCSTR,UINT);

int main(int argc, char* argv[]) {
    printf("MessageBoxA hash: 0x%x\n", getFuncHash("MessageBoxA"));
    
    PWORD functionAddr = getFooAddressesByHash((char*)"user32", 0x4df9741);
    if (functionAddr == NULL) {
        return 1;
    }

    msgbox foo = (msgbox)functionAddr;
    
    foo(NULL, "Win32 API hasher", "Executing MessageBoxA via hashed function!", MB_ICONASTERISK | MB_OK);
    return 0;
}

