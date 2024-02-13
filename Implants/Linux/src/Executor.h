
#include "stdafx.h"
#include "ServerResponse.h"

#ifndef Executor_H
#define Executor_H

typedef int (*payload)();

/**
 * @brief 
 * The function that executes the newly generated payload in a new thread.
 * And sends the response with use of volatile data and mutex locks.
 */
int executeThread(payload func) {

    
    // thrd_t thread;
    // thrd_create(&thread, (thrd_start_t)func, (void*)NULL);

}

/**
 * @brief 
 * The function that executes the newly generated payload in a forked 
 * process and uses a pipe to send the response to the parent.  
 */
int executeFork(payload func) {

    int pipeFD[2];
    if (pipe(pipeFD) == -1) {
        printf("Failed to create a new pipe! Code: %d\n\n", errno);
        return RESPONSE_ERROR;
    }

    pid_t process = fork();
    if (process == -1) {
        printf("Failed to fork this process! Code: %d\n\n", errno);
        return RESPONSE_ERROR;
    }

    // pipeFD[0] read data (parent)
    // pipeFD[1] write data (child)

    if (process > 0) { // Parent
        close(pipeFD[1]); // Close unused pipe. (write)
        
        // Wait and read the output of the payload.
        wait(NULL);
        int code;
        read(pipeFD[0], &code, sizeof(int));

        close(pipeFD[0]);
        return code;
    }
    else { // Child
        close(pipeFD[0]); // Close unused pipe. (read)

        // Executing payload and write output to other process.
        int code = func();
        write(pipeFD[1], &code, sizeof(int));

        close(pipeFD[1]);
        exit(EXIT_SUCCESS);
        return 0;
    }
}

#endif // !~ Executor_H 