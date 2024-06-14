#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/utsname.h>
#include <time.h>
#include <stdlib.h>


#include "../OutputFormatter.h" // Required!

typedef void(*output_func)(int, const char* fmt, ...); // For printing output on C2 server.
#define TARGET_HOST "127.0.0.1"
#define TARGET_PORTS "0-100"
#define TYPE_SCAN 

/**
* Network mapping BOF for the Odysseus C2 framework.
*
* - SYN scan
* - CONNECT scan
* - ping scan
*/
int sizePortScan;
int* portScan;

void scanSYN(int port) {

}

void scanCONNECT(int port) {

    
}

bool scanPING(char* host) {

}

/**
 *  Master threads port scan.
**/
void startScanPorts() {
    
}

/**
 *  String magic   
**/

int getCharPosition(char c) {

    int pos = -1;
    for (int i = 0; i < strlen(TARGET_PORTS); i++) {
        if (TARGET_PORTS[i] == c) {
            return i;
        }
    }

    return pos;
}

void procesPortsToScan() {

    int lenStr = strlen(TARGET_PORTS);

    // Check if ports are a range 0-XXX
    int pos = -1;
    if ((pos = getCharPosition('-')) != -1) {

        // Get first number.
        char first[pos-1];
        strncpy(first, TARGET_PORTS, pos);
         
        // Get last number.
        char second[lenStr - pos -1];
        strncpy(second, TARGET_PORTS + pos +1, lenStr - pos);

        // Now set the numbers.
        int firstPort = atoi(first);
        int secondPort = atoi(second);

        sizePortScan = secondPort - firstPort;
        portScan = new int[sizePortScan];
        for (int i = 1; i < secondPort +1; i++) {
            portScan[i -1] = i;
        }
        return;
    }

    // Check if ports are given seperatly ,,,,

    // Check if ports are top-ports 10 -> [use first 10 of top ports]
}


#ifdef DEBUG_BOF
int main(int argc, char* argv[]) {

    printf("[!] Starting portscan on %s with range %s\n", TARGET_HOST, TARGET_PORTS);

    procesPortsToScan();


    return 0;
}
#else
int payload_init(int id, output_func output) {

    output(id, "Starting portscan on %s with range %s\n", TARGET_HOST, TARGET_PORTS);

    procesPortsToScan();
}
#endif 

