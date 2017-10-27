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
};
//TODO : make getData return  -1 if it's the last process in the world
void getData(int PrevClock,vector<struct processData>& PD)
{
    if (PrevClock != getClk())  //Next clock?!
    {
        struct processData TempData;  //Received data from the process generator

        int BytesNum = Recmsg(TempData);

        if (BytesNum == 0) {
            //   cout << "Nothing read!" << endl;
        }
        else if (BytesNum == -1)
            cout << "End of processes!" << endl;
        else {
            while (BytesNum != 0)   //Loop to get all arrived processes
            {
                if (BytesNum == -1)
                    cout << "End of processes!" << endl;
                else
                {
                    cout << "process arrived at: "<<TempData.ArrivalTime << endl;
                    PD.push_back(TempData);
                }
                BytesNum = Recmsg(TempData);
            }

        }
        PrevClock = getClk();
    }
}

int stringToInt(string str)
{
    int x;
    stringstream ss;
    ss << str;
    ss >> x;
    return x;
}

