
#include "JobHunter.h"

/**
 * Want our jobs (payload_init) to execute in a different thread, so that 
 * the main thread keeps job hunting.
 * 
 * Also we want to have a thread manager that controls the threads, can be done on the main thread.
 * It will be able to kill threads or get information about them.
 * 
 */
// WARNING: dup2() fucks it up for the input when using reverse shells.

int main(int argc, char* argv[])
{
    JobHunter jobHunter;
    int workValue = 22;
    ConnectionController *cc = new ConnectionController(666);
    int thr = 0;

    for (int i = 0; i < 1000; i++) {

        jobHunter.getAllJobs();

        printf("\n$ Command: ");
        std::string cmd;
        std::getline(std::cin, cmd);
        
        if (cmd.find("start") != std::string::npos) {
            printf("Starting a new job with id %d with arg %d!\n\n", 0, 22);
            
            if (!jobHunter.startNewJob(cc, workValue, thr++)) {
                printf("Failed to start new job!\n");
            }
        }

        if (cmd.find("kill ") != std::string::npos && cmd.length() == 6) {

            int idtoKill = atoi(cmd.substr(cmd.length() -2).c_str()); // Fuckedup code, but works.
            printf("Killing job %d\n", idtoKill);

            if (!jobHunter.killJob(idtoKill)) {
                printf("Failed to kill job %d!\n", idtoKill);
            }
        }

        workValue++;
    }


    return 0;
}