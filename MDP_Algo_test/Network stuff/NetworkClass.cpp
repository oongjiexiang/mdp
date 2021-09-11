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
/*
string serverIP = "192.168.1.8"; //ip address of the server
int portNumber= 17; //listening port on the server
int checker;
string message = "test message";//u8 encodes the string us utf-8 format
*/
char receiveBuffer[4096];

//create wsaData and socket
WSADATA wsaData;
SOCKET ConnectSocket = INVALID_SOCKET;
sockaddr_in hint;

//constructor to set the server ip address and port number
Network::Network(string ipAddr, int portNo){
    Network::serverIP = ipAddr;
    Network::portNumber = portNo;
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
    else{
        printf("Error, target device index does not exist!");
        formattedMessage="ERROR";
    }
    return formattedMessage;
}

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

string Network::decodeMessage(){
    int i=0;
    char temp='a';
    string retMessage="";
    while(temp!='\n'){
        temp = receiveBuffer[i];
        retMessage = retMessage+temp;
        i++;
    }
    return retMessage;
}

//call this once you are done sending and receiving messages from the server
void Network::endConnection(){
//cleanup and close all
closesocket(ConnectSocket);
WSACleanup();

}

