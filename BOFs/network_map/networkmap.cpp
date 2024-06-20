#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/utsname.h>
#include <time.h>
#include <stdlib.h>
#include <pthread.h>

#include "../OutputFormatter.h" // Required!

typedef void(*output_func)(int, const char* fmt, ...); // For printing output on C2 server.

// Scan types.
#define SCAN_PORTSCAN_CONNECT       0x00
#define SCAN_PORTSCAN_SYN           0x01
#define SCAN_PING                   0x02

// Target information.
#define TARGET_HOST                 "127.0.0.1"
#define TARGET_PORTS                "0-1000"
#define TYPE_SCAN                   0x00

/**
* Network mapping BOF for the Odysseus C2 framework.
*
* - SYN scan
* - CONNECT scan
* - ping scan
*/
int sizePortScan;
int* portScan;

int resultSize = 0;
char** results;
pthread_t* threads;
pthread_mutex_t mutex;

// Function parameters for the thread functions.
struct FunctionParams {
    int threadId;
    const char* host;
    int port;
};

typedef void* (*FunctionPointer)(void*);

void* scanSYN(void* params) {

}

void* scanCONNECT(void* input) {

    FunctionParams* params = (FunctionParams*)input;

    // When outputting to result lock mutex.
    pthread_mutex_lock(&mutex);
    
    results[params->threadId] = new char[sizeof(params->port)];
    sprintf(results[params->threadId], "Portscan %d\n", params->port); 

    // And unlock
    pthread_mutex_unlock(&mutex);
}

void* scanPING(void* params) {

}

/**
 *  Master threads port scan.
**/
void startScanPorts() {
    
    FunctionPointer functionPointer;
    
    switch (TYPE_SCAN) {

        case SCAN_PORTSCAN_CONNECT: {

            threads = new pthread_t[sizePortScan];
            for (int i = 0; i < sizePortScan; i++) {

                functionPointer = &scanCONNECT;

                FunctionParams params;

                params.threadId = i;
                params.host = TARGET_HOST;
                params.port = portScan[i];

                if (pthread_create(&threads[i], NULL, functionPointer, &params) != 0) {
                    printf("Failed to start portscan on port %d\n", portScan[i]);
                }
            }

            break;
        }
    }

    // Wait for all the threads to finish.
    for (int i = 0; i < sizePortScan; i++) {

        if (threads[i] != 0) {
            pthread_join(threads[i], nullptr);
        }
    }
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
    startScanPorts();

    for (int i = 0; i < resultSize; i++) {
        printf("%s\n", results[i]);
    }

    return 0;
}
#else
int payload_init(int id, output_func output) {

    output(id, "Starting portscan on %s with range %s\n", TARGET_HOST, TARGET_PORTS);

    procesPortsToScan();
    startScanPorts();

    for (int i = 0; i < resultSize; i++) {
        output(id, "%s\n", results[i]);
    }

    return 0;
}
#endif 

