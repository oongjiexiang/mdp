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
#include <NetworkClass.h>

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
    }while(checker!=0);

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
bool Network::messageSender(string message){
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
        checkMsg = message.substr(i,2);
        if(checkMsg.compare("\n")==0 || i > 4096){
            return true;
        }
        readMsg = message.substr(i,1);
        receiverNumberString = message.substr(i+1,1);
        //3 cases for which subteam to send the message to 1 AND, 2 STM, 3 RPI
        if(receiverNumberString.compare("1")==0){
            receiverNumber=1;
        }
        if(receiverNumberString.compare("2")==0){
            receiverNumber=2;
        }
        if(receiverNumberString.compare("3")==0){
            receiverNumber=3;
        }
        //parameter 1 = send to STM
        encodedMsg = encodeMessage(receiverNumber,readMsg);
        if(encodedMsg.compare("ERROR")){
            return false;
        }
        sendMessage(encodedMsg);
        reply = decodeMessage();
        while(reply.compare("r")!=0){
            printf("Waiting for ready, current msg: %s\n",reply.c_str());
        }
        i++;
    }
    return true;
}

//call this once you are done sending and receiving messages from the server
void Network::endConnection(){
//cleanup and close all
closesocket(ConnectSocket);
WSACleanup();

}

