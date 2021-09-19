#include <iostream>
#include <string>
#include "NetworkClass.h"

using namespace std;

int main()
{
    //string ipAddr = "192.168.25.1";
    //int port =5180;
    int checker=0;
    //message stored as a string of commands with no characters in between
    string message="fhggh\n";
    //other messages to try
    //"jjjj" turn in a circle
    // "bijbii"
    // "fhffifffib"
    string retMessage="";
    Network n;
    checker = n.initializeConnection();
    if(checker == 1){
        printf("error during initialization");
        return 1;
    }
    message = n.encodeMessage(1, message);


    checker = n.sendMessage(message);
    if(checker == 1){
        printf("error during send message");
        return 1;
    }
    retMessage = n.decodeMessage();
    printf("%s",retMessage.c_str());

    n.endConnection();
    return 0;
}

