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
        std::string serverIP;
        int portNumber;
        int checker;
        std::string message;
        int bufferLength;
        Network();
        int initializeConnection();
        std::string encodeMessage(int targetDevice, std::string unformattedMessage);
        std::string decodeMessage();
        int sendMessage(std::string formattedMessage);
        int receiveMessage();
        bool sendPath(std::vector<State*>& vectorOfStates, int noOfStates,std::vector<Action*>& vectorOfAction);
        bool sendCombinedActionPath(std::vector<State*>& vectorOfStates, int noOfStates, std::vector<Action*>& vectorOfAction);
        bool sendTakePhoto();
        int sendObstacleIdToAndroid(int obstacleId);
        int sendEndToAndroid();
        std::string generateAndroidMessage(float x, float y, int facingDirection,int stmMsgNumber);
        std::string calculateAction(float x0, float x1, float y0, float y1, int facingDirection0, int facingDirection1);
        int readAndGenerateObstacles(vector<Obstacle>& obstacles);
        int convertAndroidMessage(std::string message, std::vector<int>& xVector, std::vector<int>& yVector, std::vector<int>& facingDirection);
        std::string calculateActionNew(Action* actionVector);
        int checkMsgSent(std::string stmMsg);
        int sendSingleMessageToSTM(string messageToSend);
        void endConnection();
};

#endif
