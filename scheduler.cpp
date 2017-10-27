#include "clkUtilities.h"
#include "schedulerUtilities.h"
#include "RRScheduler.h"
#include "HPFScheduler.h"
#include "SRTNScheduler.h"
#include <iostream>
#include "defs.h"


using namespace std;

int main(int argc, char* argv[])
{
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

    string state;       //Process state

    /*
     HPF -> scheduler  continue when it receives signal from process
     SRTN -> invoked when another process pushed in queue
     RR -> invoked  every quantum
     */

    while (1)
    {
        vector<struct processData> PD;  //Vector of arrived processes

        getData(Clock,PD);  //Get processes from message queue

        scheduler->pushDataToQueue(PD);  //Push received processes in the priority queue

        struct PCB Process;

        int state = scheduler->getProcess(Process); //Get process from queue

        if(state == -1)     //If no processes in the scheduler queue block until a new process arrives
        {
            //TODO: if end of process
            //kill(getppid(),WakeMeUpWhenItsAllOver);
            //pause();
            continue;
        }

        Clock = getClk();    //clock at which process starts running

        Process.WaitingTime += Clock - (Process.WaitingTime + Process.PD.RunningTime - Process.RemainingTime);

        state = "resumed";  //Process status = started (if first time to run) or resumed
        if(Process.Pid == -1)
            state = "started";

        scheduler->logProcessData(Clock,state,Process);

        scheduler->runProcess(Process);

        int Stop = getClk();    //clock at which process finishes/stops running

        Process.RemainingTime -= (Stop - Clock);  //subtract running time from the process remaining time

        if (Process.RemainingTime == 0)     //process finished
        {
            state = "finished";

            scheduler->logProcessData(Stop,state,Process);

            int TA = Stop - Process.PD.ArrivalTime;    //Turn around
            double WTA = (double)TA / Process.PD.RunningTime;   //Weighted turn around

            cout << " TA " + TA + " WTA " + WTA << endl;
        }
        else
        {
            scheduler->logProcessData(Stop,state,Process);

            scheduler->returnProcessToQueue(Process);
        }
    }
    return 0;
}
