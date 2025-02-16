#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/utsname.h>
#include <time.h>

#include "../OutputFormatter.h" // Required!

typedef void(*output_func)(int, const char* fmt, ...); // For printing output on C2 server.
#define TARGET_HOST "127.0.0.1"

/**
* Network mapping BOF for the Odysseus C2 framework.
*
* - SYN scan
* - CONNECT scan
* - ping scan
*/
void scanSYN(int port) {

}

void scanCONNECT(int port) {

}

bool scanPING(char* host) {

}


#ifdef DEBUG_BOF
int main(int argc, char* argv[]) {
    printf("[!] Starting network map BOF...\n");

    return 0;
}
#else
int payload_init(int id, output_func output) {

}
#endif 

