
#include <iostream>
#include <stdio.h>
#include <vector>
#include <string.h>


#ifndef JOBHUNTER_H
#define JOBHUNTER_H

class JobHunter {
private:
    std::vector<int> runningJobs;

public:
    typedef int(*job)();

    JobHunter() {

    }

    bool startNewJob() {
        
    }

    ~JobHunter() {
        
    }
};

#endif // !~ JOBHUNTER_H