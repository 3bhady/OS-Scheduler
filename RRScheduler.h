#pragma once
#include "scheduler.h"

class RRScheduler: public Scheduler
{
private:

	queue<struct PCB> Queue;
	static bool ReceivedContUselessSig;
public:
	//Constructor
	RRScheduler(){
		signal (SIGCONT,RRHandler);
		signal (SIGCHLD,SIG_IGN);
		signal (SIGILL,RRHandler);
	};

	//Handles the continue and process exit signals
	static void  RRHandler(int Sig)
	{
		cout << "RRScheduler: signal : " << Sig << endl;

		if (Sig == SIGILL)
		{
			cout << "RRScheduler: child handler , child is dead\n";
			ChildDead = true;

			int c = getClk(FALLING);		//wait for next falling edge
			while(c == getClk(FALLING));
		}

		if(Sig == SIGCONT)
			cout << "RRScheduler: in sig cont handler\n";
	}

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

	//Fork process to run
	//returns true if process finished, otherwise false
	virtual bool runProcess( struct PCB & ProcessData)
	{
		ChildDead = false;

        if (ProcessData.Pid == -1)	//first time we should fork :)
        {
            ProcessData.Pid = fork();
            if(ProcessData.Pid == 0)
            {
            	//child .. we create process here
                execl("process.out",to_string(ProcessData.RemainingTime).c_str(),(char  *) NULL);
            }
        }
        else {	//not first time to run the process
            kill(ProcessData.Pid,SIGCONT);
            cout<<"RRScheduler: running process " << ProcessData.Pid << endl;
        }

		kill(getppid(),SIGALRM);	//send alarm to the process manager for scheduled wake up
        cout << "RRScheduler: current process remaining time : " << ProcessData.RemainingTime << endl;
        cout << "RRScheduler: RR sleep\n";
        pause();
        cout << "RRScheduler: return to RR\n";

        kill(ProcessData.Pid,SIGSTOP);

        return ChildDead;
    }
};
