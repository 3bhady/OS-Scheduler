#pragma once
#include "scheduler.h"

struct compareRunningTime{
	bool operator()(const struct PCB & p1,const struct PCB & p2) {
		return (p1.RemainingTime > p2.RemainingTime);
	}
};

class SRTNScheduler : public Scheduler
{
private:
	priority_queue<struct PCB, vector<struct PCB>, compareRunningTime> PQueue;
public:
	//Constructor
	SRTNScheduler(){
        signal (SIGCONT,SRTNHandler);
        signal (SIGILL,SRTNHandler);
    };

    //Handles the continue and process exit signals
    static void  SRTNHandler(int Sig)
    {
        cout << "SRTNScheduler: signal : " << Sig << endl;

        if(Sig == SIGCONT)
            cout << "SRTNScheduler: in sig cont handler\n";

        if(Sig == SIGILL)
        {
            cout << "SRTNScheduler: child handler , child is dead\n";
            ChildDead=true;

            int c = getClk(FALLING);      //wait for next falling edge
            while(c == getClk(FALLING));
        }
    };

	//Push received processes in the priority queue
	virtual void pushDataToQueue(const vector<struct processData> & PD)
	{
        if(PD.size()!=0)
        {
            kill(getppid(),SIGIO); // send a wake me up when something new comes
        }
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

	//Fork process to run
	//returns true if process finished, otherwise false
	virtual bool runProcess( struct PCB & ProcessData)
    {
        if (ProcessData.Pid == -1)//first time we should fork :)
        {
            ProcessData.Pid = fork();
            if(ProcessData.Pid == 0)
            {
                //child .. we create process here
                execl("process.out",to_string(ProcessData.RemainingTime).c_str(),(char  *) NULL);
            }

        } 
        else {  //not first time to run the process
            kill(ProcessData.Pid,SIGCONT);
            cout<<"SRTNScheduler: running process " << ProcessData.Pid << endl;
        }

        ChildDead = false;

        cout << "SRTNScheduler: current process remaining time : " << ProcessData.RemainingTime << endl;
        cout << "SRTNScheduler: SRTN sleep\n";
        pause();
        cout << "SRTNScheduler: return to SRTN\n";
        kill(ProcessData.Pid,SIGSTOP);

        return ChildDead;
    }
};
