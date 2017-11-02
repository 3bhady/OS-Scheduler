#pragma once
#include <iostream>
#include <vector>
#include <sstream>
#include "queueUtilities.h"

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
        PrevClock = getClk();
  //  }
    return 0;
}

int stringToInt(string str)
{
    int x;
    stringstream ss;
    ss << str;
    ss >> x;
    return x;
}

