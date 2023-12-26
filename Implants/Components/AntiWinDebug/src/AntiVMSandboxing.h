
#include "stdafx.h"

// https://www.cyberbit.com/endpoint-security/anti-vm-and-anti-sandbox-explained/

// Resoures:
// https://github.com/LordNoteworthy/al-khaser
// Potential git sources:
// https://github.com/topics/anti-sandbox
// https://dmcxblue.gitbook.io/red-team-notes-2-0/red-team-techniques/defense-evasion/t1497-virtualization-sandbox-evasion

// https://www.deepinstinct.com/blog/malware-evasion-techniques-part-2-anti-vm-blog

// https://trustedsec.com/blog/enumerating-anti-sandboxing-techniques

/**
 * Check if system has more than 2 processors. 
 */
BOOL checkCPULimited() {
    
    SYSTEM_INFO sysInfo;
    GetSystemInfo(&sysInfo);
    if (sysInfo.dwNumberOfProcessors < 2) {
        return TRUE;
    }

    return FALSE;
} 

/**
 * Check if system has more than 2 Gb of RAM memory.
 */
BOOL checkRAMLimited() {
    MEMORYSTATUSEX memoryStatus;
    memoryStatus.dwLength = sizeof(memoryStatus);

    GlobalMemoryStatusEx(&memoryStatus);
    DWORD RAMMB = memoryStatus.ullTotalPhys / 1024 / 1024;
    if (RAMMB < 2048) {
        return TRUE;
    }

    return FALSE;
}

/**
 * Check if user contains temporary files.
 */
BOOL checkTempFiles() {
    LPSTR pathTemp = (LPSTR)malloc(MAX_PATH);
    GetEnvironmentVariableA("TEMP", pathTemp, MAX_PATH);

    int fileCount = 0;

    WIN32_FIND_DATAA findData;
    HANDLE hFind = FindFirstFileA(pathTemp, &findData);
    if (hFind != INVALID_HANDLE_VALUE) {

        do {
            fileCount ++;
        } while(FindNextFileA(hFind, &findData));
        FindClose(hFind);
    }

    return !(fileCount < 3); // Total temp files is less than 3.
}
