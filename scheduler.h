#pragma once
#include <queue>
#include "schedulerUtilities.h"

struct processData;

class Scheduler
{
public:

    //Push received processes in the queue
    virtual void pushDataToQueue(const vector<struct processData> & PD) = 0;

    //Get first process in the queue
    virtual int getProcess(struct PCB & Process) = 0;

    //Push process to the end of the queue
    virtual void returnProcessToQueue(const struct PCB & Process) = 0;

    virtual void runProcess(const struct PCB & ProcessData) = 0;
};