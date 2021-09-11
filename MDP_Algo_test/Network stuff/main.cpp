#include <iostream>
#include <string>
#include "NetworkClass.h"

using namespace std;

int main()
{
    string ipAddr = "192.168.25.1";
    int port =5180;
    int checker=0;
    string message="abc";
    string retMessage="";
    Network n(ipAddr,port);
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
