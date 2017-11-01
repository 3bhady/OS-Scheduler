#include "clkUtilities.h"
#include "schedulerUtilities.h"
#include "RRScheduler.h"
#include "HPFScheduler.h"
#include "SRTNScheduler.h"
#include <iostream>
#include <string>
#include <iomanip>
#include "defs.h"

using namespace std;

int main(int argc, char* argv[])
{
    bool EndScheduler = false;

    string SchAlgo = argv[0];  //Scheduler algorithm passed to the process

    Scheduler* scheduler;    //Scheduler object

    //Create scheduler according to scheduler algorithm

    if(SchAlgo == HPF) {
        scheduler = new HPFScheduler();
        cout <<"Created HPFScheduler\n";
    }
    else if(SchAlgo == SRTN){
        scheduler = new SRTNScheduler();
        cout <<"Created SRTNScheduler\n";
    }
    else {
        scheduler = new RRScheduler();
        cout <<"Created RRScheduler\n";
    }


    initQueue(false);   //Subscribe to message queue

    initClk();

    int Clock = -1;     //Initial clock time = -1

    string status;       //Process state

    /*
     HPF -> scheduler  continue when it receives signal from process
     SRTN -> invoked when another process pushed in queue
     RR -> invoked  every quantum
     */
    int ProcessState=-1000;
    while (1)
    {
        cout<<"Scheduler Clock "<<getClk()<<endl;
        vector<struct processData> PD;  //Vector of arrived processes

        int end = getData(Clock,PD);  //Get processes from message queue
        if (end == -1)  //If end process is received
            EndScheduler = true;
        if(PD.size()!=0)
            cout<<"count me"<<endl;
        //this must be before checking for state or otherwise the code will break
        scheduler->pushDataToQueue(PD);  //Push received processes in the priority queue

        struct PCB Process;

        int state = scheduler->getProcess(Process); //Get process from queue

        if(state == -1)     //If no processes in the scheduler queue block until a new process arrives
        {
            if (EndScheduler == true)
                break;  //End scheduler
            cout<<"no processes in the queue nothing to be ran"<<endl;
            kill(getppid(),SIGIO);
            cout<<"Scheduler: waiting because no processes are available"<<endl;
            pause();

            //cout<<"Schduler: done waiting because either a process has arrived or something went wrong, if no process arrived check the signals"<<endl;
            continue;
        }

        Clock = getClk();    //clock at which process starts running

        if(Process.Pid == -1)   //Process status = started (if first time to run) or resumed
        {
            status = "started";
            Process.WaitingTime = Clock - Process.PD.ArrivalTime;
        }
        else if(ProcessState!=LASTPROCESS){
            status = "resumed";
            Process.WaitingTime += Clock - (Process.WaitingTime + Process.PD.RunningTime - Process.RemainingTime);
        }
        if(ProcessState!=LASTPROCESS)
        {
            //cout<<"clock ()() "<<Clock<<endl;
            scheduler->logProcessData(Clock,status,Process);
        }

         ProcessState=scheduler->runProcess(Process);
        PD.clear();
       //----------------experimental
         end = getData(Clock,PD);  //Get processes from message queue

        if (end == -1)  //If end process is received
            EndScheduler = true;
        scheduler->pushDataToQueue(PD);  //Push received processes in the priority queue
        //----------------experimental



//cout<<"after run process"<<endl;
        int Stop = getClk();    //clock at which process finishes/stops running

        Process.RemainingTime -= (Stop - Clock);  //subtract running time from the process remaining time
        //cout<<" process remaining time "<<Process.RemainingTime<<endl;
        if (Process.RemainingTime <= 0)     //process finished
        {
            status = "finished";

            scheduler->logProcessData(Stop,status,Process);

            int TA = Stop - Process.PD.ArrivalTime;    //Turn around
            double WTA = (double)TA / Process.PD.RunningTime;   //Weighted turn around

            cout << " TA " << TA << " WTA " << setprecision(2) << fixed << WTA << endl;
            cout<<" queue size " <<scheduler->Size<<endl;
        }
        else
        {   if(ProcessState!=-10)
            {
            cout<<"down :";
            scheduler->logProcessData(Stop,"stopped",Process);
            }
            if(ProcessState==0)
            scheduler->returnProcessToQueue(Process);

        }
        //  while(Stop==getClk()){};

    }
    cout << "Scheduler exiting...\n";
    return 0;
}