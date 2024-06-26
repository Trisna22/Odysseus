
#include "stdafx.h"

#include "ConnectionController.h"

#ifndef JOB_HUNTER_H
#define JOB_HUNTER_H

class JobWorker {
private:
    ConnectionController* cc;
    pthread_t thread;
    bool finished = false;

    // The worker that gets the object and executes it.
    static void* jobThread(void* params) {

        JobWorker* worker = (JobWorker*)params;
        worker->JOB_ID = worker->cc->getJobId();

        printf("JOB [%s]: Downloading and parsing object...\n", worker->JOB_ID.c_str());

        // Download the object.
        ObjectLoader* loader = new ObjectLoader();
        if (!worker->cc->getObject(loader)) {
            printf("Failed to retrieve object!\n");
            return NULL;
        }

        // Parse the object.
        if (!loader->parseObject()) {
            printf("Failed to parse object!\n");
            return NULL;
        }

        printf("JOB [%s]: Executing object...\n===>\n", worker->JOB_ID.c_str());

        // Execute the object.
        int outputId = OutputVariables::getNewOutputId();
        int retVal = loader->executeObject(outputId);
        int code = worker->cc->finishJob(worker->JOB_ID, retVal, outputId);

        printf("JOB [%s]: Finished executing object...\n<===\n", worker->JOB_ID.c_str());

        // Cleanup object loader.
        loader->~ObjectLoader();

        worker->finishThread(); // Let thread manager know we are finished.
        return NULL;
    }

public:
    string JOB_ID;
    JobWorker(ConnectionController* connectionController, string jobId) : cc(connectionController), JOB_ID(jobId) {}

    int startWorker() {
        return pthread_create(&thread, NULL, jobThread, this);
    }

    int killThread() {
        return pthread_cancel(thread);
    }

    void finishThread() {
        this->finished = true;
    }

    bool isFinished() {
        return this->finished;
    }
};

class JobHunter {
private:
    std::vector<JobWorker*> threads;
public:
    typedef void*(*JOB)(void*);

    JobHunter() {

    }

    // Starts new ObjectLoader job.
    bool startNewJob(ConnectionController* cc) {

        JobWorker* worker = new JobWorker(cc, cc->getJobId());
        int code = worker->startWorker();
        if (code != 0) {
            printf("Failed to start worker for new object! Errror code: %d\n", errno);
        }

        this->threads.push_back(worker);
        return code == 0;
    }

    // Kills a worker/thread with running object.
    bool killJob(string jobId) {

        // Loop trough all jobs and check if we need to kill.
        for (int i = 0; i < this->threads.size(); i++) {

            if (this->threads.at(i)->JOB_ID.compare(jobId) == 0) {
                
                if (this->threads.at(i)->killThread() != 0) {
                    printf("Failed to kill worker %d! Error code: %d\n", i, errno);
                    return false;
                }

                this->threads.erase(this->threads.begin() + i); // Delete thread from list.
                printf("\n<===\nJOB [%s]: Killed...\n", jobId.c_str());
            }
        }

        // Even if not found, just return true.
        return true;
    }

    // Delete the jobs that are already finished.
    void checkAllJobs() {

        // Get all jobs and send the server.
        vector<string> jobList;

        if (this->threads.size() == 0) return;

        // Loop trough all saved threads to check if finished.
        for (int i = 0; i < this->threads.size(); i++) {

            if (!this->threads.at(i)->isFinished()) {

                // Add the JOB_ID to the list.
                jobList.push_back(
                    this->threads.at(i)->JOB_ID
                );
            } else {
                // Or else if it is finished, delete.
                this->threads.erase(this->threads.begin() + i);
            }
        }
    }


};

#endif //!~ JOB_HUNTER_H