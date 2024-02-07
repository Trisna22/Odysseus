

#include "stdafx.h"
#include "ConnectionController.h"
#include "ObjectLoader.h"

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

bool newObject(ConnectionController* cc) {

    ObjectLoader* loader = new ObjectLoader();
    
    cc->getObject(loader);

    ~loader();
    
}

void loop(ConnectionController* cc) {

}

int main(int argc, char* argv[]) 
{
    const char* slaveId =  SLAVE_ID;

    // Check if slaveId contains Id string.
    if (strlen(SLAVE_ID) == 0) {
        slaveId = "FAKE_SLAVE_ID";
    }

    printf("Slave ID: %s\n", slaveId);
    ConnectionController *cc = new ConnectionController(slaveId);
    prepareConnection(cc);

    loop(cc);
    return 0;
}