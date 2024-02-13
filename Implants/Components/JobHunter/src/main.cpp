
#include "JobHunter.h"

/**
 * Want our jobs (payload_init) to execute in a different thread, so that 
 * the main thread keeps job hunting.
 * 
 * Also we want to have a thread manager that controls the threads, can be done on the main thread.
 * 
 * 
*/

int jobThread() {


}

int main(int argc, char* argv[])
{
    JobHunter jobHunter;

    for (;;) {

        printf("Command: ");
        std::string cmd;
        std::cin >> cmd;

        if (cmd.find("start") != std::string::npos) {
            printf("Starting a new job!\n\n");
            
        }
    }


    return 0;
}