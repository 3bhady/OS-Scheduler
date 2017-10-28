#pragma once
#include "scheduler.h"

class RRScheduler: public Scheduler
{
private: 
	queue<struct PCB> Queue;
public:
	//Constructor
	RRScheduler(){};

	//Push received processes in the priority queue
	virtual void pushDataToQueue(const vector<struct processData> & PD)
	{
		for(int i = 0; i < PD.size();i++)
		{
			struct PCB TempPCB;
			TempPCB.PD = PD[i];
			TempPCB.RemainingTime = TempPCB.PD.RunningTime;
			Queue.push(TempPCB);
		}
	};

	//Get first process in the queue
	virtual int getProcess(struct PCB & Process)
	{
		if(Queue.size() == 0)
			return -1;
		Process = Queue.front();
		Queue.pop();   //Pop process from queue
		return 1;
	};

    //Returns process to the end of the queue to wait for its turn to run again
	virtual void returnProcessToQueue(const struct PCB & Process)
	{
		Queue.push(Process);
	};

	//Handle process stat
	virtual int runProcess( struct PCB & ProcessData)
	{
		if(ProcessData.Pid == -1)
		{

		}
		//cout<<"process ran by HPFS\n";
		//exit(1);
	};
};