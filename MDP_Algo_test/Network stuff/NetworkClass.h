#ifndef NETWORKCLASS_H
#define NETWORKCLASS_H
// NETWORKCLASS_H
#define WIN32_LEAN_AND_MEAN
//makes it so that the code compiled only works for windows vista and above(required in order to use inet_pton function)
#define NTDDI_VERSION NTDDI_VISTA
#define WINVER _WIN32_WINNT_VISTA

#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <vector>
#include "action.h"

#pragma comment(lib, "Ws2_32.lib")

class Network{
    public:
    std::string serverIP; //ip address of the server
    int portNumber; //listening port on the server
    int checker;
    std::string message;//u8 encodes the string us utf-8 format
    int bufferLength; //change this later
    Network();
    int initializeConnection();
    std::string encodeMessage(int targetDevice, std::string unformattedMessage);
    std::string decodeMessage();
    int sendMessage(std::string formattedMessage);
    void endConnection();
    bool messageSender(std::string message, int targetNumber);
    std::string sendPath(std::vector<State>& vectorOfStates);
    std::string calculateAction(float x0, float x1, float y0, float y1, int facingDirection0, int facingDirection1);

};

#endif
