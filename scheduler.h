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

    //Returns process to the end of the queue to wait for its turn to run again
    virtual void returnProcessToQueue(const struct PCB & Process) = 0;

    //Fork the process to run
    virtual void runProcess(const struct PCB & ProcessData) = 0;

    //Log process info
    void logProcessData(int Clock, string state, const struct PCB & Process)
    {
        cout << "At time " << Clock << " process " << Process.PD.ID << " " + state +
                " arr " << Process.PD.ArrivalTime << " total " << Process.PD.RunningTime <<
                " remain " << Process.RemainingTime << " wait " << Process.WaitingTime;

        if(state != "finished")     //if state == finished do not end line because other information will be logged
            cout << endl;
    };
};