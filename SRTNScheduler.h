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
    static void  SRTNHandler(int Sig){}
	//Constructor
	SRTNScheduler(){
        //TODO : set signal cont handler //
        signal (SIGCONT,SRTNHandler);
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

	//Get process with shortest remaining time
	virtual int getProcess(struct PCB & Process)
	{
		if(PQueue.size() == 0)
			return -1;
		Process = PQueue.top();
		PQueue.pop();   //pop process from queue
		return 1;
	};

    //Returns process to the end of the queue to wait for its turn to run again
	virtual void returnProcessToQueue(const struct PCB & Process)
	{
		PQueue.push(Process);
	};

	//handle process stat
	virtual void  runProcess( struct PCB & ProcessData) {
        if (ProcessData.Pid == -1)//first time we should fork :)
        {
            ProcessData.Pid = fork();
            if(ProcessData.Pid == 0)
            {
                //child .. we create process here
                execl("process.out",to_string(ProcessData.RemainingTime).c_str(),(char  *) NULL);
            }

        } else {//not first time to run the process

        kill(ProcessData.Pid,SIGCONT);
        }
        int status;
        //waitpid(Pid,&status,0);     //wait for process exit
        cout<<" srtn sleep "<<endl;
        pause();
        cout<<" return to srtn"<<endl;
        kill(ProcessData.Pid,SIGSTOP);

    }


};