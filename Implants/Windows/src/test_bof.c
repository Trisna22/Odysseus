#include <windows.h>
#include "bofdefs.h"
#include "beacon.h"


int go(void) {
    
    USER32$MessageBoxA(NULL, "Messagebox BOF", "Odysseus C2 Test", MB_ICONWARNING | MB_OK);

    BPrintf(CALLBACK_OUTPUT, "Test beacon printf!\n");

    return 22;
}
