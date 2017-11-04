#include "clkUtilities.h"
#include "schedulerUtilities.h"
#include "RRScheduler.h"
#include "HPFScheduler.h"
#include "SRTNScheduler.h"
#include <iostream>
#include <string>
#include <iomanip>
#include "defs.h"
#include <fstream>
#include <math.h>

using namespace std;

int main(int argc, char* argv[])
{
	ofstream file;
    bool EndScheduler = false;
    remove( "scheduler.log" );
    int CPUUtilizationClocks = 0, ProcessesCount;
    double TotalWaiting = 0, TotalWTA = 0;
    vector<double> WTAs;
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
    bool ProcessState=false;
    while (1)
    {
        cout<<"Scheduler Clock "<<getClk()<<endl;
        vector<struct processData> PD;  //Vector of arrived processes

        int end = getData(Clock,PD);  //Get processes from message queue
        if (end == -1)  //If end process is received
        {
            EndScheduler = true;
            ProcessesCount = PD[PD.size()-1].ID;
        }
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
            cout<<"No processes in the queue nothing to be run\n";
            kill(getppid(),SIGIO);
            cout<<"Scheduler: Waiting because no processes are available\n";
            int Temp = getClk();
            pause();
            CPUUtilizationClocks += getClk() - Temp;

            //cout<<"Schduler: done waiting because either a process has arrived or something went wrong, if no process arrived check the signals"<<endl;
            continue;
        }

        Clock = getClk();    //clock at which process starts running

        if(Process.Pid == -1)   //Process status = started (if first time to run)
        {
            status = "started";
            Process.LastRunTime = Process.PD.ArrivalTime;
        }
        else {
            status = "resumed";
        }

        Process.WaitingTime += Clock - Process.LastRunTime;

       // if(ProcessState!=LASTPROCESS)
        //{
            //cout<<"clock ()() "<<Clock<<endl;

        scheduler->logProcessData(Clock,status,Process);
        //}

        ProcessState=scheduler->runProcess(Process);

        PD.clear();
       //----------------experimental
        end = getData(Clock,PD);  //Get processes from message queue

        if (end == -1)  //If end process is received
        {
            EndScheduler = true;
            ProcessesCount = PD[PD.size()-1].ID;
        }
        scheduler->pushDataToQueue(PD);  //Push received processes in the priority queue
        //----------------experimental



//cout<<"after run process"<<endl;
        int Stop = getClk();    //clock at which process finishes/stops running
        Process.LastRunTime = Stop;
        Process.RemainingTime -= (Stop - Clock);  //subtract running time from the process remaining time
        //cout<<" process remaining time "<<Process.RemainingTime<<endl;
        if (ProcessState)     //process finished
        {
            status = "finished";

            scheduler->logProcessData(Stop,status,Process);

            int TA = Stop - Process.PD.ArrivalTime;    //Turn around
            double WTA = (double)TA / Process.PD.RunningTime;   //Weighted turn around

            TotalWaiting += Process.WaitingTime;
            TotalWTA += WTA;
            WTAs.push_back(WTA);

            cout << " TA " << TA << " WTA " << setprecision(2) << fixed << WTA << endl;

            file.open("scheduler.log",fstream::out | fstream::app);
            file << " TA " << TA << " WTA " << setprecision(2) << fixed << WTA << endl;
            file.close();
        }
        else
        {
                scheduler->logProcessData(Stop,"stopped",Process);
                scheduler->returnProcessToQueue(Process);

        }


    }

    //Calculating scheduler.perf variables
    double StdWTA = 0, AvgWTA = (double)TotalWTA/ProcessesCount;

    for(int i = 0; i < WTAs.size(); i++)
        StdWTA += (AvgWTA - WTAs[i]) * (AvgWTA - WTAs[i]);
    StdWTA /= ProcessesCount;
    StdWTA = sqrt(StdWTA);

    file.open("scheduler.perf",fstream::out);

    file <<"CPU Utilization = " << ( (getClk() - (double)CPUUtilizationClocks) / (getClk()-1) ) * 100 << "%\n";
    file <<"Avg WTA = " << AvgWTA << endl;
    file <<"Avg Waiting = " << (double)TotalWaiting/ProcessesCount << endl;
    file <<"Std WTA = " << StdWTA << endl;

    file.close();

    cout << "Scheduler exiting...\n";

    return 0;
}