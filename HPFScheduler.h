#pragma once
#include "scheduler.h"

struct comparePriority{
    bool operator()(const struct PCB & p1,const struct PCB & p2) {
        return (p1.PD.Priority < p2.PD.Priority);
    }
};

class HPFScheduler : public  Scheduler
{
private: 
	priority_queue<struct PCB, vector<struct PCB>, comparePriority> PQueue;
public:
	//Constructor
	HPFScheduler(){};

	//Push received processes in the priority queue
	virtual void pushDataToQueue(const vector<struct processData> & PD)
	{

		for(int i = 0; i < PD.size();i++)
		{
			struct PCB TempPCB;
			TempPCB.PD = PD[i];
			TempPCB.Pid = -1;
			TempPCB.RemainingTime = TempPCB.PD.RunningTime;
			PQueue.push(TempPCB);
		}
	};

	//Get highest priority process
	virtual int getProcess(struct PCB & Process)
	{
		if(PQueue.size() == 0)
			return -1;
        Process = PQueue.top();
		PQueue.pop();   //Pop process from queue
		return 1;
	};


	//Push process to the end of the queue
	virtual void returnProcessToQueue(const struct PCB & Process)
	{
		PQueue.push(Process);
	};

	//Handle process stat
	virtual void runProcess(const struct PCB & ProcessData)
	{
		if(ProcessData.Pid == -1)
		{

		}
		//cout<<"process ran by HPFS\n";
		//exit(1);
	};
};