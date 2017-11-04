#pragma once
#include <iostream>
#include <vector>
#include <sstream>
#include "queueUtilities.h"
#include <fstream>
#include <math.h>

using namespace std;

//Process Control Block
struct PCB {
    struct processData PD;
    int RemainingTime;
    int WaitingTime = 0;
    int Pid = -1;
    int LastRunTime;
};

//Gets data from message queue and puts it in PD
//returns -1 if last process is received
int getData(int PrevClock,vector<struct processData>& PD)
{
   // if (PrevClock != getClk())  //Next clock?!
    //{
        struct processData TempData;  //Received data from the process generator

        int BytesNum = Recmsg(TempData);

        if (BytesNum == 0) {
            //   cout << "Nothing read!" << endl;
        }
        else if (BytesNum == -1) {
            cout << "End of processes!" << endl;
            return -1;
        }
        else {
            while (BytesNum != 0)   //Loop to get all arrived processes
            {
                if (BytesNum == -1)
                {
                    cout << "End of processes!" << endl;
                    return -1;
                }
                else {
                    cout << "Process arrived at: " << TempData.ArrivalTime << endl;
                    PD.push_back(TempData);
                }
                BytesNum = Recmsg(TempData);
            }

        }
        PrevClock = getClk(true);
  //  }
    return 0;
}

void printCalculations(int CPUUtilizationClocks,int ProcessesCount,double TotalWaiting,double TotalWTA,const vector<double> & WTAs,ofstream & file)
{
    //Calculating scheduler.perf variables
    double StdWTA = 0, AvgWTA = (double)TotalWTA/ProcessesCount;

    for(int i = 0; i < WTAs.size(); i++)
        StdWTA += (AvgWTA - WTAs[i]) * (AvgWTA - WTAs[i]);
    StdWTA /= ProcessesCount;
    StdWTA = sqrt(StdWTA);

    file.open("scheduler.perf",fstream::out);

    file << "CPU Utilization = " << ( (getClk(FALLING) - (double)CPUUtilizationClocks) / (getClk(FALLING)-1) ) * 100 << "%\n";
    file << "Avg WTA = " << AvgWTA << endl;
    file << "Avg Waiting = " << (double)TotalWaiting/ProcessesCount << endl;
    file << "Std WTA = " << StdWTA << endl;

    file.close();
}
