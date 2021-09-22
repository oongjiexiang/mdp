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
    string message0="b\n";
    string message1="i\n";
    string message2="j\n";
    string message3="f\n";
    string longMsg = "bijf\n";
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

    message0 = n.encodeMessage(2, message0);
    checker = n.sendMessage(message0);
    printf("Msg 1 sent");

    message1 = n.encodeMessage(2, message1);
    checker = n.sendMessage(message1);

    message2 = n.encodeMessage(2, message2);
    checker = n.sendMessage(message2);

    message3 = n.encodeMessage(2, message3);
    checker = n.sendMessage(message3);

    checker = n.messageSender(longMsg,2);

    if(checker == 1){
        printf("error during send message");
        return 1;
    }
    retMessage = n.decodeMessage();
    printf("%s",retMessage.c_str());

    n.endConnection();
    return 0;
}

