#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/utsname.h>
#include <time.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/socket.h>
#include <netinet/in.h> 
#include <arpa/inet.h>
#include <netdb.h>
#include <fcntl.h>

#include "../OutputFormatter.h" // Required!

#define SOCKET_ERROR                -1
typedef void(*output_func)(int, const char* fmt, ...); // For printing output on C2 server.

// Scan types.
#define SCAN_PORTSCAN_CONNECT       0x00
#define SCAN_PORTSCAN_SYN           0x01
#define SCAN_PING                   0x02
#define MAX_THREADS                 30
#define MAX_TIMEOUT                 0x03

// Target information.
#define TARGET_HOST                 "192.168.2.254"
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
sem_t semaphore;
pthread_mutex_t mutex;

// Function parameters for the thread functions.
struct FunctionParams {
    int threadId;
    const char* host;
    int port;
};

typedef void* (*FunctionPointer)(void*);

void* scanSYN(void* params) {

    return 0;
}

void* scanCONNECT(void* input) {

    FunctionParams* params = (FunctionParams*)input;

    /**
     * Connect to the target.
     */

    char* scanResult;
    int currentFlags;
    servent *service;

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == SOCKET_ERROR) {
        printf("[#%d!] Failed to create a socket! Error code: %d\n", params->threadId, errno);
        goto end_scan;
    }

    sockaddr_in remoteAddr;
    memset(&remoteAddr, 0, sizeof(remoteAddr));
    remoteAddr.sin_addr.s_addr = inet_addr(params->host);
    remoteAddr.sin_port = htons(params->port);
    remoteAddr.sin_family = AF_INET;

    // Get current flags.
    currentFlags = fcntl(sock, F_GETFL, 0);
    if (currentFlags == SOCKET_ERROR) {
        printf("[#%d!] Failed to get current flags of socket! Error code: %d\n", params->threadId, errno);
        goto end_scan;
    }

    // Set current flag and non-blocking mode.
    if (fcntl(sock, F_SETFL, currentFlags | O_NONBLOCK) == SOCKET_ERROR) {
        printf("[#%d!] Failed to set socket flag! Error code: %d\n", params->threadId, errno);
        goto end_scan;
    }

    // Connect to target.
    int ret;
    if ((ret = connect(sock, (sockaddr*)&remoteAddr, sizeof(remoteAddr))) == SOCKET_ERROR && errno != EINPROGRESS) {

        // Connect error.
        printf("[#%d!] Failed to connect to target! Error code: %d\n", params->threadId, errno);
        
        goto end_scan;
    }

    if (errno == EINPROGRESS && ret == SOCKET_ERROR) {

        fd_set fds;
        FD_ZERO(&fds);
        FD_SET(sock, &fds);

        // 5 second connection timeout.
        timeval tv;
        tv.tv_sec = MAX_TIMEOUT;
        tv.tv_usec = 0;

        // Wait for timeout.
        ret = select(sock + 1, NULL, &fds, NULL, &tv);
        if (ret == SOCKET_ERROR) {

            // Select failed.
            printf("[#%d!] Failed to select() on socket! Error code: %d\n", params->threadId, errno);
            goto end_scan;
        }
        else if (ret == 0) {

            // Connection timeout, port closed.
            goto end_scan;

        } else {
            
            // Check first if any other socket error.
            int valopt;
            socklen_t lon = sizeof(int);
            if (getsockopt(sock, SOL_SOCKET, SO_ERROR, (void*)(&valopt), &lon) == -1) {
                
                // Failed to check socket option.
                printf("[#%d!] Failed to check for socket option! Error code: %d\n", params->threadId, errno);
                goto end_scan;
            }

            // If there is any error.
            if (valopt) {
                goto end_scan;
            }
        }
    }

    /**
     * Port alive, get information and set results.
    **/

    // Port information.
    service = getservbyport(htons(params->port), "tcp");
    
    // When outputting to result lock mutex.
    pthread_mutex_lock(&mutex);

    results[params->threadId] = new char[30];
    snprintf(results[params->threadId], 30, "%d\t%s\topen\n", 
        params->port, service == NULL ? "unknown" :  service->s_name); 

    // And unlock
    pthread_mutex_unlock(&mutex);

end_scan:
    sem_post(&semaphore); // Release semaphore lock.
    return 0;
}

void* scanPING(void* params) {


    return 0;
}

/**
 *  Master threads port scan.
**/
void startScanPorts() {
    
    FunctionPointer functionPointer;
    
    switch (TYPE_SCAN) {

        case SCAN_PORTSCAN_CONNECT: {

            functionPointer = &scanCONNECT;
            resultSize = sizePortScan; // Don't forget to put the potential size of output.

            threads = new pthread_t[sizePortScan];
            results = (char**)calloc(sizePortScan, sizeof(char*));
            for (int i = 0; i < sizePortScan; i++) {

                sem_wait(&semaphore); // Wait for slot to open.

                FunctionParams *params = new FunctionParams();

                params->threadId = i;
                params->host = TARGET_HOST;
                params->port = portScan[i];

                if (pthread_create(&threads[i], NULL, functionPointer, params) != 0) {
                    printf("Failed to start portscan on port %d\n", portScan[i]);
                    sem_post(&semaphore); // Release on error.
                }
            }
        
            break;
        }

            case SCAN_PORTSCAN_SYN: {

                functionPointer = &scanSYN;
                resultSize = sizePortScan; // Don't forget to put the potential size of output.

                threads = new pthread_t[sizePortScan];
                results = (char**)calloc(sizePortScan, sizeof(char*));
                for (int i = 0; i < sizePortScan; i++) {

                    sem_wait(&semaphore); // Wait for slot to open.

                    FunctionParams *params = new FunctionParams();

                    params->threadId = i;
                    params->host = TARGET_HOST;
                    params->port = portScan[i];

                    if (pthread_create(&threads[i], NULL, functionPointer, params) != 0) {
                        printf("Failed to start portscan on port %d\n", portScan[i]);
                        sem_post(&semaphore); // Release on error.
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

    // Initialize mutex lock and semaphore.
    pthread_mutex_init(&mutex, NULL);
    sem_init(&semaphore, NULL, MAX_THREADS);

    startScanPorts(); // Start scanning.

    // Set results to output.
    for (int i = 0; i < resultSize; i++) {
        if (results[i] != NULL) {
            printf("%s", results[i]);
        }
    }

    // Cleanup.
    sem_destroy(&semaphore);

    return 0;
}
#else
int payload_init(int id, output_func output) {

    output(id, "Starting portscan on %s with range %s\n", TARGET_HOST, TARGET_PORTS);

    procesPortsToScan(); // Process port input.

    // Initialize mutex lock and semaphore.
    pthread_mutex_init(&mutex, NULL);
    sem_init(&semaphore, NULL, MAX_THREADS);

    startScanPorts(); // Start scanning.

    // Set results to output.
    for (int i = 0; i < resultSize; i++) {
        if (results[i] != NULL) {
            output(id, "%s\n", results[i]);
        }
    }

    // Cleanup.
    sem_destroy(&semaphore);

    return 0;
}
#endif 

