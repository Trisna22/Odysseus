
#include <iostream>
#include <stdio.h>
#include <vector>
#include <string.h>
#include <pthread.h>
#include <unistd.h>

#ifndef JOBHUNTER_H
#define JOBHUNTER_H

class Job {
private:
    int id;
public:

    Job(int value) : id(value) {}

    int doCalcs(int val) {
        return val * 2;
    }

    void scan() {

        printf("Job with ID %d scanning now...\n", this->id);
        
        for (int i = 0; i < 10; i++) {
            
            printf("Job %d still scanning for %d seconds...\n", this->id, 10 -i);
            sleep(1);
        }

    }

    int getJobID() {
        return this->id;
    }

};

class ConnectionController {
private:
    int ID = 666;
    pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
public:
    ConnectionController(int value) : ID(value) {}

    void sendToServer(int result) {

        pthread_mutex_lock(&mutex);


        printf("CC with ID %d sending result %d \n", ID, result);

        pthread_mutex_unlock(&mutex);
        
    }
};

struct JobInfo {
    int arg;
    ConnectionController** cc;
};


/**
 * Controls the threads.
 * 
 * Takes the CC as argument.
*/
class ThreadManager {
private:
    ConnectionController* cc;
    int arg;
    int ID;
    pthread_t threadID;

public:
    ThreadManager(ConnectionController* value, int TODO, int id) : cc(value), arg(TODO), ID(id) {}

    void startThread() {

        pthread_create(&threadID, NULL, jobThread, this);
    }

    int killThread() {
        return pthread_cancel(threadID);
    }


private:
    static void* jobThread(void* params) {

        ThreadManager* threadManager = (ThreadManager*)params;

        Job *job = new Job(threadManager->ID); // Can be our object loader.

        /** 
         * Execute our job
        **/
        job->scan();

        /**
         * Put return value.
        **/
        printf("CC in thread %d is %p\n", threadManager->ID, threadManager->cc);
        threadManager->cc->sendToServer(job->doCalcs(threadManager->arg));

        return NULL;
    }
};

class JobHunter {
private:
    std::vector<ThreadManager*> runningJobs;
    std::vector<int*> finishedJobs;

public:
    typedef void*(*job_func)(void*);

    JobHunter() {

    }

    ~JobHunter() {
        
    }

    bool startNewJob(ConnectionController* cc, int arg, int thr) {
        
        printf("Value CC: %p\n", cc);

        ThreadManager* tm = new ThreadManager (cc, arg, thr);

        tm->startThread();
        this->runningJobs.push_back(tm);

        return true;
    }

    bool killJob(int id) {
        
        int retVal = this->runningJobs.at(id)->killThread();
        if (retVal != 0) {
            printf("Failed to kill, errno: %d\n", retVal);
        }

        return retVal == 0;
    }

    void getAllJobs() {

        printf("$ Active jobs:\n");
        if (runningJobs.size() == 0) {
            printf("$ No active jobs...\n");
            return;
        }

        for (int i = 0; i < runningJobs.size(); i++) {
            printf("  Job [%d] \n", i);
        }
    }

    void checkFinishedJobs() {

        // for (int i = 0; i < runningJobs.size(); i++) {
            
        //     JobInfo jobInfo  = runningJobs.at(i);
        //     if (*jobInfo.returnValue > 0) {
        //         printf("Job with ID %d is finished with value %d!\n", jobInfo.tid, *jobInfo.returnValue);
        //     }
        // }
    }

};

#endif // !~ JOBHUNTER_H