#define WIN32_LEAN_AND_MEAN
//makes it so that the code compiled only works for windows vista and above(required in order to use inet_pton function)
#define NTDDI_VERSION NTDDI_VISTA
#define WINVER _WIN32_WINNT_VISTA
#define _WIN32_WINNT _WIN32_WINNT_VISTA

#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include "NetworkClass.h"
#include <vector>
#include "action.h"

#pragma comment(lib, "Ws2_32.lib")

using namespace std;
char receiveBuffer[4096];

//create wsaData and socket
WSADATA wsaData;
SOCKET ConnectSocket = INVALID_SOCKET;
sockaddr_in hint;

//constructor to set the server ip address and port number
Network::Network(){
    Network::serverIP = "192.168.25.1"; //ip given by RPi
    Network::portNumber = 5180;
    Network::checker =0;
    Network::message = "test message";
    Network::bufferLength = 4096; //change this later
    //create wsadata
}

//call this to initialize the network
int Network::initializeConnection(){
    //initialize WSAData as version 2.2
    checker = WSAStartup(MAKEWORD(2,2),&wsaData);
    if(checker!=0){
        printf("WSAStartup failed: %d\n", checker);
        return 1;
    }

    //initialize values for hint data
    hint.sin_family = AF_INET; //can be either ipv4 or ipv6
    hint.sin_port = htons(portNumber); //converts port_number
    inet_pton(AF_INET,serverIP.c_str(),&hint.sin_addr); //converts the string ip address to code

    //connect socket using tcp
    ConnectSocket = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
    if (ConnectSocket == INVALID_SOCKET) {
        printf("Error at socket(): %d\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }
    //connect to server
    checker = connect(ConnectSocket,(sockaddr*)&hint,sizeof(hint));
    if(checker == SOCKET_ERROR){
        printf("failed to connect to IP address %d\n",WSAGetLastError());
        closesocket(ConnectSocket);
        WSACleanup();
        return 1;
    }
    return 0;
}

//target device 1 for android, 2 for stm, assumes the unformatted message is correct
string Network::encodeMessage(int targetDevice, string unformattedMessage){
    string formattedMessage;
    if(targetDevice==1){
        formattedMessage = "AND|"+unformattedMessage+"\n";
    }
    else if(targetDevice==2){
        formattedMessage = "STM|"+unformattedMessage+"\n";
    }
    else if(targetDevice==3){
        formattedMessage = "RPI|"+unformattedMessage+"\n";
    }
    else{
        printf("Error, target device index does not exist!");
        formattedMessage="ERROR";
    }
    return formattedMessage;
}

//sends only 1 message to the server and waits for a reply
//sends message to the server and receives data that is input into the receiveBuffer
int Network::sendMessage(string formattedMessage){
    //set current message to formatted message
    message = formattedMessage;
    string retMessage = "";
    if(formattedMessage.compare("ERROR")==0){
        printf("Error, message is not properly formatted!");
        closesocket(ConnectSocket);
        WSACleanup();
        return 1;
    }
    //send the message to the server
    checker = send(ConnectSocket, message.c_str(), message.length(),0);
    if (checker == SOCKET_ERROR) {
        printf("send failed: %d\n", WSAGetLastError());
        closesocket(ConnectSocket);
        WSACleanup();
        return 1;
    }
    //get return messages from server

    do{
        checker = recv(ConnectSocket, receiveBuffer,bufferLength,0);
        if(checker>0) //bytes are being received
            printf("Bytes received: %d\n", checker);
        else if (checker == 0){ //no more bytes received
            printf("Connection closed\n");
        }
        else{ //something went wrong{
            printf("receive failed: %d\n", WSAGetLastError());
            break;
        }
    }while(checker<=0);

    return 0;
}

//decode the message received from the server
string Network::decodeMessage(){
    int i=0;
    char temp='a';
    string retMessage="";
    while(temp!='\n' || i < 4096){
        temp = receiveBuffer[i];
        retMessage = retMessage+temp;
        i++;
    }
    return retMessage;
}

//sends multiple messages, message should be in the format of single char commands with no spacing in between them and ending in '\n'
//eg. "aaab\n"
//if true, all actions from the message are completed, if false, there was an error, resend
bool Network::messageSender(string message, int targetNumber){
    //read the list of commands
    //send 1 command at a time to RPi
    //wait for ready msg from STM/Rpi
    int i=0;
    string readMsg = "";
    string receiverNumberString = "0";
    int receiverNumber = 0;
    string checkMsg = "";
    string encodedMsg = "";
    string reply = "";
    while(true){
        checkMsg = message.substr(i,1);
        if(checkMsg.compare("\n")==0 || i > 4096){
            return true;
        }
        readMsg = message.substr(i,1);
        //parameter 1 = send to STM
        encodedMsg = encodeMessage(receiverNumber,readMsg);
        if(encodedMsg.compare("ERROR")){
            return false;
        }
        sendMessage(encodedMsg);
        reply = decodeMessage();
        while(reply.compare("R")!=0){
            printf("Waiting for ready, current msg: %s\n",reply.c_str());
        }
        i++;
    }
    return true;
}


string Network::sendPath(vector<State>& vectorOfStates){
    int currentStateIndex,vectorSize, facingDirection0, facingDirection1;
    float x0, x1, y0, y1;
    string andMsg = "";
    string stmMsg = "";
    string rpiMsg = ""; //fixed as only 1 msg to detect
    string retMsg = "";
    vectorSize = static_cast<int>(vectorOfStates.size());
    for(currentStateIndex = 0; currentStateIndex<vectorSize; currentStateIndex++){
        if(currentStateIndex!=vectorSize-1){
            x0 = vectorOfStates[currentStateIndex].position->x_left;
            x1 = vectorOfStates[currentStateIndex+1].position->x_left;
            y0 = vectorOfStates[currentStateIndex].position->y_low;
            y1 = vectorOfStates[currentStateIndex+1].position->y_low;
            facingDirection0 = vectorOfStates[currentStateIndex].face_direction;
            facingDirection1 = vectorOfStates[currentStateIndex+1].face_direction;
            stmMsg = calculateAction(x0,x1,y0,y1,facingDirection0,facingDirection1);
            andMsg = ""; // convert x0,x1,y0,y1,facingdirection0, facingdireion1 to string and put in this format (x0,y0,fd0),(x1,y1,fd1)
            printf("%s\n",stmMsg.c_str());
            //messageSender(andMsg,1);
            //messageSender(stmMsg,2);
        }
        else{
            //retMsg = messageSender(rpiMsg,3);
        }

    }
    return retMsg;


}


//given two states' x, y and facing direction, return the action to be sent to STM
string Network::calculateAction(float x0, float x1, float y0, float y1, int facingDirection0, int facingDirection1){
    //define the actions according to the command list
    string forward10 = "b";
    string reverse10 = "f";
    string turnRight = "i";
    string turnLeft = "j";
    int x,y;
    x = x1-x0;
    y = y1-y0;
    //forward or reverse
    if(facingDirection0 == facingDirection1){
        if(facingDirection0 == 0){ //face east
            //check if y changes
            if(y < 0){ //turn right when facing east
                return turnRight;
            }
            if(y > 0){ //turn left when facing east
                return turnLeft;
            }
            if(x > 0){//forwarwd motion
                return forward10;
            }
            if(x < 0){//reverse motion
                return reverse10;
            }
        }
        if(facingDirection0 == 180){ //face south
            //check if y changes
            if(y < 0){ //turn left when facing east
                return turnLeft;
            }
            if(y > 0){ //turn right when facing east
                return turnRight;
            }
            if(x > 0){//x1 and x0 should not be equal in this case, if a movement action is required
                return reverse10;
            }
            if(x < 0){
                return forward10;
            }
        }
        if(facingDirection0 == 90){ //face north
            //check if x changes first
            if(x < 0){ //turn left when facing east
                return turnLeft;
            }
            if(x > 0){ //turn right when facing east
                return turnRight;
            }
            if(y > 0){//y1 and y0 should not be equal in this case, if a movement action is required
                return forward10;
            }
            if(y < 0){
                return reverse10;
            }
        }
        if(facingDirection0 == 270){
            //check if x changes first
            if(x<0){ //turn right when facing east
                return turnRight;
            }
            if(x>0){ //turn left when facing east
                return turnLeft;
            }
            if(y > 0){//y1 and y0 should not be equal in this case, if a movement action is required
                return reverse10;
            }
            if(y < 0){
                return forward10;
            }
        }
    }
    //if facing opposite directions
    //for now assume that it only requires a turning motion
    if((facingDirection0+180)%360 == facingDirection1){
        if(facingDirection0 == 0){ //face east
            //check if y changes
            if(y < 0){ //turn right when facing east
                return turnRight+turnRight;
            }
            if(y > 0){ //turn left when facing east
                return turnLeft+turnLeft;
            }
            if(x!=0){ //assume to turn right by default
                return turnRight+turnRight;
            }
        }
        if(facingDirection0 == 180){ //face south
            //check if y changes
            if(y < 0){ //turn left when facing east
                return turnLeft+turnLeft;
            }
            if(y > 0){ //turn right when facing east
                return turnRight+turnRight;
            }
            if(x!=0){ //assume to turn right by default
                return turnRight+turnRight;
            }
        }
        if(facingDirection0 == 90){ //face north
            //check if x changes first
            if(x < 0){ //turn left when facing east
                return turnLeft+turnLeft;
            }
            if(x > 0){ //turn right when facing east
                return turnRight+turnRight;
            }
            if(y!=0){ //assume to turn right by default
                return turnRight+turnRight;
            }
        }
        if(facingDirection0 == 270){
            //check if x changes first
            if(x<0){ //turn right when facing east
                return turnRight+turnRight;
            }
            if(x>0){ //turn left when facing east
                return turnLeft+turnLeft;
            }
            if(x!=0){ //assume to turn right by default
                return turnRight+turnRight;
            }
        }
    }
    //if facing direction differs by + 90, turn left
    if((facingDirection0+90)%360 == facingDirection1){
        return turnLeft;
    }
    //if facing direction differs by -90, turn right
    if((facingDirection0-90)%360 == facingDirection1){
        return turnRight;
    }
    printf("Error");
    return"";
}


//call this once you are done sending and receiving messages from the server
void Network::endConnection(){
//cleanup and close all
closesocket(ConnectSocket);
WSACleanup();

}

