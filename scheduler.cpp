#include "clkUtilities.h"
#include "queueUtilities.h"
#include <iostream>
using namespace std;

int main(int argc, char* argv[]) {
    char SchAlgo;
    struct processData pD;      //Received data from the process generator
    initQueue(false);
    initClk();

    cout << argc << endl;
    string str = argv[0];
    cout << str << endl;

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
            //printf("current received data %d\n",pD.id);

            // cout<<SchAlgo<<endl;
            //===================================
            //Preimplemented Functions examples
            //cout<<" this is scheduler \n";
            /////Toget time use the following function


            //printf("current time is %d\n", x);
            x = getClk();
        }
    }
    return 0;
}
