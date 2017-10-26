#pragma once
#include "scheduler.h"

struct compareRunningTime{
	bool operator()(const struct PCB & p1,const struct PCB & p2) {
		return (p1.RemainingTime < p2.RemainingTime);
	}
};

class SRTNScheduler : public Scheduler
{
private: 
	priority_queue<struct PCB, vector<struct PCB>, compareRunningTime> PQueue;
public:
	//constructor
	SRTNScheduler(){};

	//push received processes in the priority queue
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

	//get process with shortest remaining time
	virtual int getProcess(struct PCB & Process)
	{
		if(PQueue.size() == 0)
			return -1;
		Process = PQueue.top();
		PQueue.pop();   //pop process from queue
		return 1;
	};

	//Push process to the end of the queue
	virtual void returnProcessToQueue(const struct PCB & Process)
	{
		PQueue.push(Process);
	};

	//handle process stat
	virtual void  runProcess(const struct PCB & ProcessData){

	};
};