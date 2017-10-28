#include <iostream>
#include <string>
#include <sstream>
#include "clkUtilities.h"

using namespace std;
int x;
int stringToInt(string str);//TODO: include scheduler utilities to use function string to int
void handler(int);
void handlerStop(int);
void handlerCont(int);
int main(int agrc, char* argv[])
{
     signal(SIGCONT,handlerCont);
    signal(SIGSTOP,handlerStop);
    cout << "Entered process\n";
    initClk();
     x= getClk();
    int NewClock;
  	int remainingtime = stringToInt(argv[0]);   //remaining time passed as a parameter

    while(true)    //loop until process finishes executing
    {
    cout<<" before waiting \n";


        while(x == getClk());
        cout<<" x = "<<x<<endl;
        x=getClk();
        remainingtime--; //run=2  clock=3  -> clock=4 re=1 ,clock=5 re=0
        cout<<"  proc remain time "<<remainingtime<<endl;
        cout<<" clock is "<<getClk()<<endl;

        if(remainingtime<=0)break;

    }

    destroyClk(false);
    cout << "Leaving process   "<<endl;

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
void handler(int sig )
{
    cout<<"process continue \n";
}
void handlerCont(int sig){
x=getClk();
    cout<<"handler continue in process \n";
}
void handlerStop(int sig )
{
    cout<<"process Stopped \n";
    pause();
}
