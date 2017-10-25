#include "clkUtilities.h"
#include "queueUtilities.h"
#include <iostream>
using namespace std;

int main(int argc, char* argv[]) {

    char SchAlgo;
    struct processData pD;      //Received data from the process generator

    initQueue(false);

    initClk();

    //TODO: implement the scheduler :)
    int x = -1;
    while (1) {
        if (x != getClk())
        {
            int BytesNum = Recmsg(pD);
            if (BytesNum == 0)
                cout << "Nothing read!" << endl;
            else if (BytesNum == -1)
                cout << "End of processes!" << endl;
            else {
                while (BytesNum != 0)
                {
                    if (BytesNum == -1)
                        cout << "End of processes!" << endl;
                    else cout << pD.arrivaltime << endl;
                    BytesNum = Recmsg(pD);
                }
            }
            x = getClk();
        }
    }
    return 0;
}
