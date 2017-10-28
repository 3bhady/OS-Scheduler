#include <iostream>
#include <string>
#include <sstream>
#include "clkUtilities.h"

using namespace std;

int stringToInt(string str);//TODO: include scheduler utilities to use function string to int
void handler(int);
void handlerStop(int);
int main(int agrc, char* argv[])
{
    signal(SIGCONT,handler);
    signal(SIGSTOP,handlerStop);
    cout << "Entered process\n";
    initClk();

    int x = getClk();

  	int remainingtime = stringToInt(argv[0]);   //remaining time passed as a parameter

    while(true)    //loop until process finishes executing
    {

        while(x == getClk());

        x = getClk();
        remainingtime--; //ruسn=2  clock=3  -> clock=4 re=1 ,clock=5 re=0
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

}
void handlerStop(int sig )
{
    cout<<"process Stopped \n";
    pause();
}
