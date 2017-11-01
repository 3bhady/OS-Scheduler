#pragma once
#include "scheduler.h"

class RRScheduler: public Scheduler
{
private:

	queue<struct PCB> Queue;

	static bool ReceivedContUselessSig;
public:

	static void IgnoreSIGCHLD(int Sig)
	{
		signal(SIGCHLD,RRHandler);

         int status;
            int p;
            p=waitpid(-1, &status, 0);
            if(p!=-1)
            {   if(WIFCONTINUED(status)){

                    cout<<"RRScheduler: in SIGCHLD handler \n";
//                    if(paused)
                    pause();

                }

            }

	}
	static void  RRHandler(int Sig){
		cout<<"RRScheduler: signal : "<<Sig<<endl;
		if(Sig==SIGCONT)
		{
			cout<<"RRScheduler: in sig cont handler \n";
		}
		if(Sig==SIGCHLD)
		{   int status;
			int p;
			p=waitpid(-1, &status, WNOHANG);
			if(p!=-1)
			{


                if (WIFEXITED(status)){
                    cout<<" exit code for child :"<<WEXITSTATUS(status)<<endl;
                  if(WEXITSTATUS(status)==53) {
                      cout << "RRScheduler: child handler , child is dead" << endl;
                      ChildDead=true;
                  }
                    else {
                      if(paused)
                          pause();
                  }





					// pause();
				}
				else if(WIFCONTINUED(status)){

					cout<<"RRScheduler: in SIGCHLD handler \n";
                    if(paused)
                        pause();

				}
				else if(WIFSIGNALED(status)) {
					cout<<"-------------RRScheduler:signal stopped child: "<<WTERMSIG(status)<<endl;
					if(WCOREDUMP(status))
					{
						cout<<"++++++++++++++RRScheduler: core dump from child\n";
					}
				}
				else if(WIFSTOPPED(status))
				{
					cout<<"///////////RRScheduler: some fucker stopped me"<< WSTOPSIG(status)<<endl;
				}
				else{
					cout<<"***************RRScheduler: unknown signal : "<<(status<<8)<<" \n";
				}




			}


		}
		// else { cout<<" unkown signal "<<endl;}

	}
	//Constructor
	RRScheduler(){
		//TODO : set signal cont handler //
		signal (SIGCONT,RRHandler);
		signal (SIGCHLD,RRHandler);
	};


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

    virtual int  runProcess( struct PCB & ProcessData) {

        if (ProcessData.Pid == -1)//first time we should fork :)
        {
            ProcessData.Pid = fork();
            if(ProcessData.Pid == 0)
            {
                execl("process.out",to_string(ProcessData.RemainingTime).c_str(),(char  *) NULL);
            }

        } else {//not first time to run the process

			//signal(SIGCHLD,IgnoreSIGCHLD);
            kill(ProcessData.Pid,SIGCONT);


            cout<<"RRScheduler: running process "<<ProcessData.Pid<<endl;
        }
        int status;
        ChildDead=false;

        cout<<"RRScheduler: current process remaining time : "<<ProcessData.RemainingTime<<endl;

			//--------------send alarm to the process manager for scheduled wake up
            kill(getppid(),SIGALRM);

            cout<<"RRScheduler: RR sleep "<<endl;
		//if(ReceivedContUselessSig) {

        //signal(SIGCHLD,IgnoreSIGCHLD);
        int to_return;
        if(Lprocess!=ProcessData.Pid)
        {
            to_return=ChildDead;
        }else {
            to_return=-10;
        }


                    paused=true;
					pause();
                    paused=false;


		//	ReceivedContUselessSig=false;
	//	}
			//--------------
        cout<<"RRScheduler: return to rr"<<endl;


        kill(ProcessData.Pid,SIGSTOP);
       /* if(Lprocess!=ProcessData.Pid)
        {
            //not the last chosen process
            Lprocess=ProcessData.Pid;
            return 0;


        }*/
        return to_return;


    }
};
