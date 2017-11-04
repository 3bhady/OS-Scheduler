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

    static void  SRTNHandler(int Sig){
        cout<<"signal : "<<Sig<<endl;
        if(Sig==SIGCONT)
        cout<<"in sig cont handler \n";
        if(Sig==SIGILL)
        {
            ChildDead=true;
        }
        else
        if(Sig==SIGCHLD)
        {   int status;
            int p;
            p=waitpid(-1, &status, WNOHANG);
            if(p!=-1)
            {   if (WIFEXITED(status)){


                    if(WEXITSTATUS(status)==53)
                        cout<<"RRScheduler: child handler , child is dead"<<endl;
                    else {
                        if(paused)
                            pause();
                    }

               // pause();
                }
                else if(WIFCONTINUED(status)){

                    cout<<"in SIGCHLD handler \n";
                   if(paused)
                    pause();

                }
                


            }


        }
       // else { cout<<" unkown signal "<<endl;}

        }
	//Constructor
	SRTNScheduler(){
        //TODO : set signal cont handler //
        signal (SIGCONT,SRTNHandler);
        //signal (SIGCHLD,SRTNHandler);
        signal (SIGILL,SRTNHandler);
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
        //cout<<" chosen process remaining time & queue size : "<<Process.PD.RunningTime<<endl<<"size "<<PQueue.size()<<endl;
		Size=PQueue.size();
        return 1;
	};

    //Returns process to the end of the queue to wait for its turn to run again
	virtual void returnProcessToQueue(const struct PCB & Process)
	{
		PQueue.push(Process);
	};

	//handle process stat
	virtual int  runProcess( struct PCB & ProcessData) {
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
            cout<<" running process "<<ProcessData.Pid<<endl;
        }
        ChildDead=false;
        int status;
        //waitpid(Pid,&status,0);     //wait for process exit
        cout<<" srtn sleep "<<endl;
        cout<<" current process remaining time : "<<ProcessData.RemainingTime<<endl;
        paused=true;
       // if( !(NoMoreProcesses&&ProcessData.RemainingTime<=1&&PQueue.size()==0))

        pause();
        paused=false;
       // else cout<<" finished everything in srtn scheduler \n";
        cout<<" return to srtn"<<endl;
        //usleep(50000);
        kill(ProcessData.Pid,SIGSTOP);

        return ChildDead;


    }


};