#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/utsname.h>

#include "../OutputFormatter.h" // Required!

int payload_init(OutputFormatter* of) {

    // Get username.
    char* username = getlogin();

    // Get computer information.
    struct utsname buffer;
    if (uname(&buffer) < 0) {
        return 1;
    }

    // Put all together.
    int infoSize = 65*3;
    char info[infoSize];

    printf("[BOF] OutputFormatter: %p\n", of);
    of->setOutputData("This should print in the main program...\n");
    // of->setOutputData("%s %s %s", buffer.release, buffer.machine, buffer.version); // From OutputFormatter.h
    return 0;
}
