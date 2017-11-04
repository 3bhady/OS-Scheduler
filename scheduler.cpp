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
    int CPUUtilizationClocks = 0, ProcessesCount;
    double TotalWaiting = 0, TotalWTA = 0;
    vector<double> WTAs;	//contains WTA of all processes
    string SchAlgo = argv[0];  //Scheduler algorithm passed to the process

	remove("scheduler.log");	//delete log file to create new one

    Scheduler* scheduler;    //to create Scheduler object

    //Create scheduler object according to scheduler algorithm
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

    /*
     HPF -> scheduler invoked when it receives exit signal from process
     SRTN -> invoked when another process pushed in queue
     RR -> invoked  every quantum
     */


    int Clock = -1;     //Initial clock time = -1
    string status;       //Process state
    bool ProcessState = false;

    while (1)
    {
        cout << "Scheduler Clock " << getClk(FALLING) << endl;

        vector<struct processData> PD;  //Vector of arrived processes

        int end = getData(Clock,PD);  //Get processes from message queue
        if (end == -1)  //If end process is received
        {
            EndScheduler = true;
            ProcessesCount = PD[PD.size()-1].ID;
        }

        if(PD.size() != 0)
            cout << "count me" << endl;

        //this must be before checking for state or otherwise the code will break
        scheduler->pushDataToQueue(PD);  //Push received processes in the priority queue

        struct PCB Process;

        int state = scheduler->getProcess(Process); //Get process from queue

        if(state == -1)     //If no processes in the scheduler queue block until a new process arrives
        {
            if (EndScheduler == true)
                break;  //End scheduler

 			cout << "Scheduler: Waiting because no processes are available\n";
            kill(getppid(),SIGIO);	//Send signal to the process generator to wake scheduler
           
            int Temp = getClk(FALLING);
            pause();

            CPUUtilizationClocks += getClk(FALLING) - Temp;

            continue;
        }

        Clock = getClk(FALLING);    //clock at which process starts running

        if(Process.Pid == -1)   //Process status = started (if first time to run)
        {
            status = "started";
            Process.LastRunTime = Process.PD.ArrivalTime;
        }
        else {
            status = "resumed";
        }

        Process.WaitingTime += Clock - Process.LastRunTime;

        scheduler->logProcessData(Clock,status,Process);

        ProcessState = scheduler->runProcess(Process);

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


        int Stop = getClk(FALLING);    //clock at which process finishes/stops running

        Process.LastRunTime = Stop;
        Process.RemainingTime -= (Stop - Clock);  //subtract running time from the process remaining time
        
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

    file <<"CPU Utilization = " << ( (getClk(FALLING) - (double)CPUUtilizationClocks) / (getClk(FALLING)-1) ) * 100 << "%\n";
    file <<"Avg WTA = " << AvgWTA << endl;
    file <<"Avg Waiting = " << (double)TotalWaiting/ProcessesCount << endl;
    file <<"Std WTA = " << StdWTA << endl;

    file.close();

    cout << "Scheduler exiting...\n";

    return 0;
}