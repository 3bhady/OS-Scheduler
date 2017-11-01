#include <iostream>
#include <string>
#include <sstream>
#include "clkUtilities.h"

using namespace std;
int x;
int stringToInt(string str);//TODO: include scheduler utilities to use function string to int
void handlerKILLED(int);
void handlerStop(int);
void handlerCont(int);
int main(int agrc, char* argv[])
{
     signal(SIGCONT,handlerCont);
    signal(SIGSTOP,handlerStop);
    signal(SIGKILL,handlerKILLED);
    signal(SIGINT,handlerKILLED);

    cout << "Process: Entered process\n";
    initClk();
     x= getClk();
    int NewClock;
  	int remainingtime = stringToInt(argv[0]);   //remaining time passed as a parameter

    while(true)    //loop until process finishes executing
    {
    //cout<<" before waiting \n";

        usleep(100);
        while(x == getClk());
        //cout<<" x = "<<x<<endl;
        cout<<"Process: my id is:"<<getpid()<<" call me"<<endl;
        x=getClk();
        remainingtime--; //run=2  clock=3  -> clock=4 re=1 ,clock=5 re=0
        cout<<"Process: proc remain time "<<remainingtime<<endl;
        cout<<"Process clock is "<<getClk()<<endl;

        if(remainingtime<=0)break;

    }

    cout << "Process: Leaving process   "<<endl;
    destroyClk(false);


    return 53;
}

int stringToInt(string str)
{
    int x;
    stringstream ss;
    ss << str;
    ss >> x;
    return x;
}
void handlerKILLED(int sig )
{

}
void handlerCont(int sig){
x=getClk();
    cout<<"Process: handler continue\n";
}
void handlerStop(int sig )
{
    cout<<"Process: Stopped \n";
    pause();
}
