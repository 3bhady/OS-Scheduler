#include <iostream>
#include <string>
#include <sstream>
#include "clkUtilities.h"

using namespace std;

int stringToInt(string str);//TODO : include scheduler utilities to use function string to int

int main(int agrc, char* argv[])
{
    cout << "Entered process\n";
    initClk();

    int x = getClk();

  	int remainingtime = stringToInt(argv[0]);   //remaining time passed as a parameter

    while(remainingtime > 0)    //loop until process finishes executing
    {
        while(x == getClk());

        x = getClk();

        remainingtime--;
    }

    destroyClk(false);
    cout << "Leaving process...\n";
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