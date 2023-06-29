#include "stdafx.h"

void usage(char* progname) {
    printf("Usage: %s [options]\n", progname);
}

void banner() {
    
    printf("\n   .   .      |    |  . | .    .  .    \n");
    printf("  . .  .   . )_)  )_)  )_)     .  .     \n");
    printf("   .  .     )___))___))___)\\  . . .    \n");
    printf(" .  .  .   )____)____)_____)\\   . .    \n");
    printf("         _____|____|____|____\\\\___     \n");
    printf("---------\\  , , ,  , , ,   /---------   \n");
    printf("  ^^^^^ ^^^^^^^^^^^^^^^^^^^^^            \n");
    printf("    ^^^^      ^^^^     ^^^    ^^\n");
    printf(" ^^      ^^^^      ^^^           ^^^\n");
    printf("\n ^^^    Odysseus client CLI.   ^^^^\n\n");
}

int main(int argc, char* argv[])
{
    if (argc < 2) {
        usage(argv[0]);
        return 1;
    }
    
    banner();

    return 0;
}