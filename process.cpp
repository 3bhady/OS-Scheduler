
#include<iostream>
#include<string>
#include "headers.h"
#include "clkUtilities.h"
using namespace std;

/* Modify this file as needed*/
int remainingtime =0;
int stringToInt(string str);//TODO : include scheduler utilities to use function string to int 

int main(int agrc, char* argv[]) {

    //if you need to use the emulated clock uncomment the following line
    initClk();


    //TODO: it needs to get the remaining time from somewhere
    //remainingtime = ??;
    x=getClk();

  	remainingtime=stringToInt(argv[0]);
    while(remainingtime>0) {
       while(x==getClk());
       x=getClk();
       remainingtime--;
    }
    //if you need to use the emulated clock uncomment the following line
    destroyClk(false);
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