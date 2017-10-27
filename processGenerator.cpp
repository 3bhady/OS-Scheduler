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

int main() {

     // variables
    string ScAlgo;
    int Quantum;
    int pid;
    int SchedulerID;
    int ClockID;
    vector<processData> Process;

    signal(SIGINT,clearResources);  //clear resources on sudden exit
    void  schedulerResponse();
    initQueue(true);

    readProcesses(Process);     //read processes data from file


    //TODO: 
    //Ask the user about the chosen scheduling Algorithm and its parameters if exists.
    
    cout<<"Enter scheduling algorithm \n";
   // cin>>ScAlgo;

    ScAlgo = HPF;

    if(ScAlgo == HPF){


    }
    else if(ScAlgo == SRTN){
        //SRTN
        cout<<"SRTN ALGO choosed in processGenerator \n";
    }
    else if(ScAlgo == RR){
        //RR
        cout<<"Enter Quantum value \n";
        cin>>Quantum;
    }


    //Initiate and create Scheduler and Clock processes.
    pid=fork();
    if(pid==0)
    {
        //child .. we create Clock process here
        execl("clock.out","",(char  *) NULL);
    }

    ClockID = pid;

    pid=fork();
    if(pid==0)
    {
        //child .. we create scheduler process here
        execl("sch.out",ScAlgo.c_str(),(char  *) NULL);
    }

    //storing Scheduler ID for other purposes including a massive destruction to this damn project
    SchedulerID=pid;

    //use this function AFTER creating clock process to initialize clock, and initialize MsgQueue
    initClk();
   // clearResources(0);   //to clear all resources
    //TODO:  Generation Main Loop
    //Send & Notify the information to  the scheduler at the appropriate time
    //(only when a process arrives) so that it will be put it in its turn.

    while(1){

        int x = getClk();    //to get time
       // printf("current time is %d\n",x);

        //no processes in the file, send end of transmission message
        if(Process.size() == 0)
        {
            lastSend();
            break;
        }

        bool WakeScheduler =false;

        for(int i = 0; i < Process.size(); i++)
        {
            if(x == Process[i].ArrivalTime)
            {
                int result = Sendmsg(Process[i]);   //push process in message queue

                if(ScAlgo == SRTN)
                {

                    WakeScheduler = true;
                cout<<" another input to scheduler"<<endl;
                }
                if(result == -1) {    //returns -1 on failure;
                    printf("Failed to push process in message queue!\n");
                }

                Process.erase(Process.begin() + i);     //remove process from vector
                i--;
            }
            else break;
        }

        if(Process.size() == 0)
        {
            lastSend();  //no more processes, send end of transmission message
            //   kill(SchedulerID,SIGCONT);  //wake scheduler to receive end process
          //  cout<<" to send last process"<<endl;

        }
         if(WakeScheduler){

            kill(SchedulerID,SIGCONT);  //wake scheduler as new processes added to the queue
             cout<<"waking up scheduler"<<endl;
            WakeScheduler=false;
        }
        if(Process.size()==0)break;

        //TODO: respond to scheduler timer , wether scheduler needs to  be invoked at certain  times or no
        schedulerResponse();
    }

    int status;
    waitpid(SchedulerID,&status,0);     //wait for scheduler exit
    cout<<"Exit status is: "<<status<<endl;

    clearResources(0);   //to clear all resources
}

void clearResources(int)
{
    cout<<"Resources cleared\n";
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
            for(int i=0; i<Line.size(); i++)
            {
                if(Line[i] != '\t')     //not tab character
                {
                    temp+= Line[i];
                }
                else {
                    parameter.push_back(temp);
                    temp="";
                }
            }

            /*pushing process data into processData struct and pushing the struct
            in the processes vector*/
            parameter.push_back(temp);

            struct processData PData;

            PData.ID=stringToInt(parameter[0]);
            PData.ArrivalTime=stringToInt(parameter[1]);
            PData.RunningTime=stringToInt(parameter[2]);
            PData.Priority=stringToInt(parameter[3]);


            Process.push_back(PData);
        }
    }
    FIn.close();
}



void schedulerResponse()
{

}