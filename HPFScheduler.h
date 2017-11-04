#pragma once
#include "scheduler.h"

struct comparePriority{
    bool operator()(const struct PCB & p1,const struct PCB & p2) {
        return (p1.PD.Priority > p2.PD.Priority);
    }
};

class HPFScheduler : public  Scheduler
{
private: 
	priority_queue<struct PCB, vector<struct PCB>, comparePriority> PQueue;
public:
	//Constructor
	HPFScheduler(){
		signal(SIGCONT,contSig);
		signal(SIGCHLD,SIG_IGN);
		signal(SIGILL,processExitSig);
	};

	//Handler for the exit signal of the process
	static void processExitSig(int Sig)
	{
		int c = getClk(FALLING);	//wait for the next positive edge
		while(c == getClk(FALLING));
	};
	
	//Handler of the continue signal from the process generator
	static void contSig(int Sig)
	{
		//Wake up...
	};	

	//Push received processes in the priority queue
	virtual void pushDataToQueue(const vector<struct processData> & PD)
	{
		for(int i = 0; i < PD.size();i++)
		{
			struct PCB TempPCB;
			TempPCB.PD = PD[i];
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


    	//Returns process to the end of the queue to wait for its turn to run again
	virtual void returnProcessToQueue(const struct PCB & Process)
	{
		PQueue.push(Process);
	};

	//Fork process to run
	//returns true if process finished, otherwise false
	virtual bool runProcess( struct PCB & ProcessData)
	{
        int Pid = fork();

        if(Pid == 0)
        {
            //child .. we create process here
            execl("process.out",to_string(ProcessData.RemainingTime).c_str(),(char  *) NULL);
        }

        cout << "Waiting process to exit\n";
		pause();	//sleep until exit of process

		return true;	//process has finished processing
	};
};
