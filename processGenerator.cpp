#include "clkUtilities.h"
#include "queueUtilities.h"
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>

using namespace std;

int stringToInt(string str);
void clearResources(int);
void readProcesses(vector<processData> &Process);

int main() {

     // variables
    int ScAlgo;
    int pid;
    int SchedulerID;
    int ClockID;
    vector<processData> Process;

    initQueue(true);


    readProcesses(Process);

    //TODO: 
    //Ask the user about the chosen scheduling Algorithm and its parameters if exists.
    
    cout<<"Enter scheduling algorithm \n";
    //cin>>ScAlgo;

    ScAlgo=0;

    // 2-Initiate and create Scheduler and Clock processes.
    pid=fork();
    if(pid==0)
    {
        //child .. we create Clock process here
        execl("clock.out","",(char  *) NULL);
    }

    ClockID=pid;

    pid=fork();
    if(pid==0)
    {
        //child .. we create scheduler process here
        execl("sch.out","4",(char  *) NULL);
    }

    //Storing Scheduler ID for other purposes including a massive destruction to this damn project
    ShedulerID=pid;

    // 3-use this function AFTER creating clock process to initialize clock, and initialize MsgQueue
    initClk();


    //Reading processes data from processes.txt file
    ifstream FIn("processes.txt");

    while(getline(FIn,Line))
    {
        if(Line[0] != '#')  //Not commented line
        {
            vector<string> parameter;   //Vector contains the process parameters
            string temp="";     //Stores the parameter
            for(int i=0; i<Line.size(); i++)
            {
                if(Line[i] != '\t')     //Not tab character
                {
                    temp+= Line[i];
                }
                else {
                    parameter.push_back(temp);
                    temp="";
                }
            }
            //Pushing process data into processData stuct and pushing the struct in the processes vector
            parameter.push_back(temp);

            processData PData;

            PData.id=stringToInt(parameter[0]);
            PData.arrivaltime=stringToInt(parameter[1]);
            PData.runningtime=stringToInt(parameter[2]);
            PData.priority=stringToInt(parameter[3]);

            Process.push_back(PData);
        }
    }

    FIn.close();

    //TODO:  Generation Main Loop
    //5-Send & Notify the information to  the scheduler at the appropriate time 
    //(only when a process arrives) so that it will be put it in its turn.

    while(1){
        //To get time use the following function
        int x = getClk();
        printf("current time is %d\n",x);

        //No processes in the file, send end of transmission message
        if(Process.size() == 0)
        {
            lastSend();
            while(1);
            //break;
        }

        for(int i = 0; i < Process.size(); i++)
        {
            if(x == Process[i].arrivaltime)
            {
                int result = Sendmsg(Process[i]);   //Push process in message queue

                if(result == -1) {    //Returns -1 on failure;
                    printf("Failed to push process in message queue!\n");
                }

                Process.erase(Process.begin() + i);     //Remove process from vector
                i--;

                if(Process.size() == 0)
                {
                    lastSend();  //no more processes, send end of transmission message
                    while(1);
                    break;
                }
            }
            else{
                break;
            }
        }

    }

    //To clear all resources
    ClearResources(0);
    //======================================
    
}

void clearResources(int)
{
    msgctl(qid, IPC_RMID, (struct msqid_ds*)0);
    destroyClk(true);
    exit(0);
}

int stringToInt(string str)
{
    int x;
    stringstream ss;
    ss << str;
    ss >> x;
    return x;
}

void readProcesses(vector<processData> &Process)
{
    ifstream FIn("processes.txt");  //Reading processes data from processes.txt file
    string Line;    //Line read from file

    while(getline(FIn,Line))
    {
        if(Line[0] != '#')  //Not commented line
        {
            vector<string> parameter;   //Vector contains the process parameters
            string temp="";     //Stores the parameter
            for(int i=0; i<Line.size(); i++)
            {
                if(Line[i] != '\t')     //Not tab character
                {
                    temp+= Line[i];
                }
                else {
                    parameter.push_back(temp);
                    temp="";
                }
            }
            //Pushing process data into processData stuct and pushing the struct in the processes vector
            parameter.push_back(temp);

            processData PData;

            PData.id=stringToInt(parameter[0]);
            PData.arrivaltime=stringToInt(parameter[1]);
            PData.runningtime=stringToInt(parameter[2]);
            PData.priority=stringToInt(parameter[3]);

            Process.push_back(PData);
        }
    }

    FIn.close();
}




