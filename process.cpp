#include <iostream>
#include <string>
#include <sstream>
#include "clkUtilities.h"

using namespace std;

int x;
int stringToInt(string str);//TODO: include scheduler utilities to use function string to int
void handlerStop(int);
void handlerCont(int);

int main(int agrc, char* argv[])
{
    signal(SIGCONT,handlerCont);
    signal(SIGSTOP,handlerStop);

    cout << "Process: Entered process\n";

    initClk();
    x = getClk(RISING);

  	int remainingtime = stringToInt(argv[0]);   //remaining time passed as a parameter

    while(true)    //loop until process finishes executing
    {
        if(remainingtime <= 0)
        	break;

        x = getClk(RISING);

        remainingtime--; //run=2  clock=3  -> clock=4 re=1 ,clock=5 re=0
        cout << "Process: my id = " << getpid() << " call me\n";
        cout << "Process: remaining time = " << remainingtime << endl;
        cout << "Process: clock = " << getClk(RISING) << endl;

        while(x == getClk(RISING));		//Wait until next rising edge


    }

    kill(getppid(),SIGILL);		//Wake up scheduler because process has finished

    cout << "Process: Leaving process...\n";

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

void handlerCont(int sig)
{
    x = getClk(RISING);		//Update process clock 
    cout << "Process: handler continue\n";
}

void handlerStop(int sig)
{
    cout << "Process: Stopped\n";
    pause();
}