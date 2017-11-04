#include "clkUtilities.h"
#include "queueUtilities.h"
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include "defs.h"
using namespace std;


int stringToInt(string str);
void clearResources(int);
void readProcesses(vector<processData> &Process);
void quantumWake(int sig);
void newProcessWake(int sig);

int QuantumStart = -1;
bool WakeOnNewProcess = false;

int main() {

    string ScAlgo;
    int Quantum;
    int pid;
    int SchedulerID;
    int ClockID;
    vector<processData> Process;	//List of processes to run

    signal(SIGINT,clearResources);  //clear resources on sudden exit
    signal(SIGALRM,quantumWake);	//wake scheduler when quantum finishes
    signal(SIGIO,newProcessWake);	//wake scheduler on arrival of new process

    initQueue(true);

    readProcesses(Process);     //read processes data from file

    cout << "Start\n";
    cout << "Enter scheduling algorithm\n"
            "1-HPF\n"
            "2-SRTN\n"
            "3-RR\n";
    cin >> ScAlgo;

    if(ScAlgo == RR)
    {
        cout << "Enter Quantum value\n";
        cin >> Quantum;
        if(Quantum < 1) 
        {
            cout << "Enter a valid quantum value\n";
            exit(0);
        }
    }

    pid = fork();
    if(pid == 0)
    {
        //child .. we create Clock process here
        execl("clock.out","",(char  *) NULL);
    }

    ClockID = pid;

    pid = fork();
    if(pid == 0)
    {
        //child .. we create scheduler process here
        execl("sch.out",ScAlgo.c_str(),(char  *) NULL);
    }

    //Storing Scheduler ID for signals communication
    SchedulerID=pid;

    //Use this function AFTER creating clock process to initialize clock, and initialize MsgQueue
    initClk();

    int x;
    while(1){

        x = getClk(FALLING);    //to get time

        if( (x-QuantumStart) == Quantum && QuantumStart != -1)
        {
            kill(SchedulerID,SIGCONT);  //wake scheduler as new processes are added to the queue
            QuantumStart = -1;
        }

        //No processes in the file, send end of transmission message
        if(Process.size() == 0)
        {
            lastSend();
            break;
        }

        bool WakeScheduler = false;

        for(int i = 0; i < Process.size(); i++)
        {
            if(x == Process[i].ArrivalTime)
            {
                int result = Sendmsg(Process[i]);   //push process in message queue

                if(ScAlgo == SRTN)
                {
                    WakeScheduler = true;
                	cout << "Another input to scheduler\n";
                }
                if(result == -1) {    //returns -1 on failure;
                    cout << "Failed to push process in message queue\n";
                }

                Process.erase(Process.begin() + i);     //remove process from vector
                i--;
            }
            else break;
        }

        if(Process.size() == 0)
            lastSend();  //no more processes, send end of transmission message


        if(!queueIsEmpty()) 
        {
            if (WakeOnNewProcess) 
            {
                WakeOnNewProcess = false;
                kill(SchedulerID, SIGCONT);  //wake scheduler as new processes are added to the queue
                cout << "Waking up scheduler by request from the shceduler\n";
                cout << "ProcessGenerator clock: " << x << endl;
                WakeScheduler = false;
            }
        }

        if(Process.size() == 0)
        	break;

        //TODO: respond to scheduler timer , wether scheduler needs to  be invoked at certain  times or no
        //schedulerResponse();
    }

    int status;
    waitpid(SchedulerID,&status,WNOHANG);     //wait for scheduler exit
    while(!WIFEXITED(status))
    {
        waitpid(SchedulerID,&status,WNOHANG);     //wait for scheduler exit
        if(WIFEXITED(status))
            break;

        if( ((x-QuantumStart) == Quantum )&& QuantumStart !=-1)
        {
            kill(SchedulerID,SIGCONT);  //wake scheduler as new processes are added to the queue
            cout << "Please wake up this is the morning call\n";
            QuantumStart=-1;
        }
        x = getClk(FALLING);
    }
    cout << "Exit status is: " << status << endl;

    clearResources(0);   //to clear all resources
}

void clearResources(int)
{
    cout << "Resources cleared\n";
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
    ifstream FIn("processes.txt");  //reading processes data from processes.txt file
    string Line;    //line read from file

    while(getline(FIn,Line))
    {
        if(Line[0] != '#')  //not commented line
        {
            vector<string> parameter;   //vector contains the process parameters
            string temp="";     //stores the parameter

            for(int i = 0; i < Line.size(); i++)
            {
                if(Line[i] != '\t' && Line[i] != ' ' && Line[i] != '\n') //not tab or space or end line characters
                {
                    temp += Line[i];
                }
                else {
                    if(temp.size()!=0)
                    parameter.push_back(temp);
                    temp="";
                }
            }

            /*pushing process data into processData struct and pushing the struct
            in the processes vector*/
            if(temp.size()!=0) {
                parameter.push_back(temp);
            }
            if(parameter.size()!=0)
            {
            	struct processData PData;

            	PData.ID=stringToInt(parameter[0]);
            	PData.ArrivalTime=stringToInt(parameter[1]);
            	PData.RunningTime=stringToInt(parameter[2]);
            	PData.Priority=stringToInt(parameter[3]);

	            Process.push_back(PData);
            }

        }
    }
    FIn.close();
}

void quantumWake(int sig){
    cout<<"Call to Quantum Wake in process generator has been made\n";
    QuantumStart = getClk(FALLING);
}

void newProcessWake(int sig){
    cout<<"Call to wake on new process in process generator has been made\n";
    WakeOnNewProcess = true;
}