#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/utsname.h>

#include "../OutputFormatter.h" // Required!

int payload_init(char* data) {

    printf("[BOF] OUTPUT_DATA:  %p\n", data);
    printf("[BOF] data:         %s\n", data);

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

    snprintf(info, infoSize, "%s %s %s", buffer.release, buffer.machine, buffer.version); // From OutputFormatter.h
    printf("[BOF]: %s\n", info);


    // Realloc buffer...
    // char* tempStr = (char*)realloc(data, infoSize) +1;

    snprintf(data, infoSize, "%s %s %s", buffer.release, buffer.machine, buffer.version); // Add output to dataOutput.
    return 0;
}
