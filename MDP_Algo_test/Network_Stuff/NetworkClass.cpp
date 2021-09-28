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
#include <chrono>
#include <thread>
#include <vector>
#include "action.h"
#include "NetworkClass.h"

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
    Network::bufferLength = 4096;
    memset(receiveBuffer,'\n',sizeof(char)*bufferLength); //initialize buffer with '\n' characters
}

//call this to initialize the network, make sure to call this before any other function
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

//target device 1 for android, 2 for stm, 3 for rpi, assumes the unformatted message is correct
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

//send 1 message to the server
int Network::sendMessage(string message){
    //set current message to formatted message
    if(message.compare("ERROR")==0){
        printf("Error, message is not properly formatted!");
        closesocket(ConnectSocket);
        WSACleanup();
        return 1;
    }
    //send the message to the server
    printf("Sending %s",message.c_str());
    checker = send(ConnectSocket, message.c_str(), message.length(),0);
    if (checker == SOCKET_ERROR) {
        printf("send failed: %d\n", WSAGetLastError());
        closesocket(ConnectSocket);
        WSACleanup();
        return 1;
    }
    printf("Message sent.\n");
    return 0;
}

//receives message from the server
int Network::receiveMessage(){
    //clear buffer before receive
    memset(receiveBuffer,'\n',sizeof(char)*bufferLength);
    do{
        //receive message from server
        checker = recv(ConnectSocket, receiveBuffer,bufferLength,0);
        if(checker>0){ //bytes are being received
            printf("Bytes received: %d\n", checker);
            printf("%s\n",decodeMessage().c_str());
        }
        else if (checker == 0){ //no more bytes received
            //printf("Connection closed\n");
        }
        else{ //something went wrong{
            printf("receive failed: %d\n", WSAGetLastError());
            break;
        }
    }while(checker<=0);
    // joel testing
//    do{
//        checker = recv(ConnectSocket, receiveBuffer,bufferLength,0);
//        if(checker>0){ //bytes are being received
//            printf("Bytes received: %d\n", checker);
//            printf("%s\n",decodeMessage().c_str());
//        }
//        else if (checker == 0){ //no more bytes received
//            //printf("Connection closed\n");
//        }
//        else{ //something went wrong{
//            printf("receive failed: %d\n", WSAGetLastError());
//            break;
//        }
//    }while(checker<=0);
    return 0;
}

//check the buffer and decode the message received from the server
string Network::decodeMessage(){
    int i=0;
    char temp=receiveBuffer[0];
    string retMessage="";
    while(temp!='\n' && i < 4096){
        temp = receiveBuffer[i];
        retMessage = retMessage+temp;
        i++;
    }
    return retMessage;
}

//send the steps from a* search to android and stm
bool Network::sendPath(vector<State*>& vectorOfStates, int noOfStates){
    int currentStateIndex, facingDirection0, facingDirection1;
    float x0, x1, y0, y1;
    string expectedMsgFromSTM = "R";//a for ready, change this if there are changes from STM side
    string andMsg = "";
    string stmMsg = "";
    string replyMessage = "";
    //maybe can make it noOfStates -2 since the last state is invalid and the second last state is a detect state
    for(currentStateIndex = 0; currentStateIndex < (noOfStates-1); currentStateIndex++){
        x0 = vectorOfStates[currentStateIndex]->position->x_left;
        x1 = vectorOfStates[currentStateIndex+1]->position->x_left;
        y0 = vectorOfStates[currentStateIndex]->position->y_low;
        y1 = vectorOfStates[currentStateIndex+1]->position->y_low;
        facingDirection0 = vectorOfStates[currentStateIndex]->face_direction;
        facingDirection1 = vectorOfStates[currentStateIndex+1]->face_direction;
        //create the messages
        printf("\nx0 %f, y0 %f, facingDirection0 %d | x1 %f, y1 %f, facingDirection1 %d\n",x0,y0,facingDirection0,x1,y1,facingDirection1);
        stmMsg = calculateAction(x0,x1,y0,y1,facingDirection0,facingDirection1);
        andMsg = generateAndroidMessage(x1,y1,facingDirection1);
        //encode the messages
        stmMsg = encodeMessage(2,stmMsg);
        andMsg = encodeMessage(1,andMsg);
        sendMessage(andMsg);
        //send the messages to android then STM
        sendMessage(stmMsg);
        //wait for reply message from STM side
        while(true){
            receiveMessage();
            replyMessage = decodeMessage();
            printf("reply message %s", replyMessage.c_str());
            if(replyMessage.substr(0,1)==(expectedMsgFromSTM)){
                break;
            }
            this_thread::sleep_for(100ms);
        }
        replyMessage="";
        //wait for 500ms before sending running the next loop to prevent spamming
        //this_thread::sleep_for(5000ms);
    }
    return true;
}

//call this at the beginning to establish connection with the server, so they can send replies
string Network::sendReadyToRpi(){
    string message ="";
    //message = encodeMessage(3,"t");
    //sendMessage(message);
    receiveMessage();
    message = decodeMessage();
    return message;
}

//send a message to RPI to take photo
bool Network::sendTakePhoto(){
    string msg ="";
    printf("\nRequesting RPI to take photo.\n");
    msg = encodeMessage(3,"t");
    sendMessage(msg);
    receiveMessage();
    msg = decodeMessage();
    if(msg.compare("y")==0){
        return true;
    }
    else{
        return false;
    }
}

//end of search, tell android to stop
int Network::sendEndToAndroid(){
    string message="";
    message = encodeMessage(1,"END");
    sendMessage(message);
    return 1;
}

//wait and read the message from android to generate obstacles
int Network::readAndGenerateObstacles(vector<Obstacle>& obstacles){
    vector<float> xVector;
    vector<float> yVector;
    vector<int> fVector;
    int noOfObstacles = 0;
    string msg = "";
    string messageFromAndroid = "";
    //only android sends messages longer than 1 character, check that message received is from android
//    testing buffer
//    receiveBuffer[1]='a';
//    receiveBuffer[0]='b';
//    receiveBuffer[2]='a';
    //test message, remove later
    messageFromAndroid = "5,9,S\n7,14,W\n12,9,E\n15,15,S\n15,4,W\n";
    while(true){
        receiveMessage();
        msg = decodeMessage();
        if(msg.length()>2){
            messageFromAndroid = msg;
            break;
        }
    }

    //convert android message and create obstacles
    convertAndroidMessage(messageFromAndroid,xVector,yVector,fVector);
    //generate the obstacles and store them into the obstacle array
    noOfObstacles = static_cast<int>(xVector.size());
    //test to print the first set of obstacle
    printf("%d,%f,%f,%d",noOfObstacles,xVector[0],yVector[0],fVector[0]);
    for(int i=0; i < noOfObstacles; i++){
        obstacles.push_back(Obstacle(i+1,xVector[i],yVector[i],fVector[i]));
    }
    return 1;
}

//convert the message android sent
//message in the format of x,y,F\nx,y,F\n
int Network::convertAndroidMessage(string message, vector<float>& xVector, vector<float>& yVector, vector<int>& facingDirection){
    string currentValue = "";
    string currentMessage = "";
    float newFloatValue;
    int newIntValue;
    int msgLen = message.length();
    int positionCounter = 0; // 0 = x, 1 = y

    //loop through the message
    for(int i = 0; i < msgLen; i++){
        currentValue = message.substr(i,1);
        //skip R from stm
        if(currentValue.compare("R")==0){
            continue;
        }
        //if newline character, end of facing direction, start new loop
        if(currentValue.compare("\n")==0){
            if(currentMessage=="N"){
                currentMessage = "90";
            }
            if(currentMessage=="S"){
                currentMessage = "270";
            }
            if(currentMessage=="E"){
                currentMessage = "0";
            }
            if(currentMessage=="W"){
                currentMessage = "180";
            }
            newIntValue = stoi(currentMessage);
            facingDirection.push_back(newIntValue);
            positionCounter = 0;
            currentMessage = "";
        }
        else if(currentValue.compare(",")==0){
            //values of x
            if(positionCounter==0){
                newFloatValue = stof(currentMessage);
                xVector.push_back(newFloatValue);
            }
            //values of y
            if(positionCounter==1){
                newFloatValue = stof(currentMessage);
                yVector.push_back(newFloatValue);
            }
        //increment position counter each time a "," is encountered and empty the currentMessage
        positionCounter++;
        currentMessage = "";
        }
        //only append the character if it is NOT a "," or "\n"
        else{
            //append the new character that was read to the end of the current message
            currentMessage = currentMessage + currentValue;
        }
    }
    return 0;
}


//given x,y,facing direction, generate the message to android
string Network::generateAndroidMessage(float x, float y, int facingDirection){
    string retMessage ="";
    //facing east
    if(facingDirection==0){
        int xInt = static_cast<int>(x);
        int yInt = static_cast<int>(y);
        retMessage = to_string(xInt) + "," + to_string(yInt) + ",E";
    }
    //facing north
    if(facingDirection==90){
        int xInt = static_cast<int>(x);
        int yInt = static_cast<int>(y);
        retMessage = to_string(xInt) + "," + to_string(yInt) + ",N";
    }
    //facing west
    if(facingDirection==180){
        int xInt = static_cast<int>(x);
        int yInt = static_cast<int>(y);
        retMessage = to_string(xInt) + "," + to_string(yInt) + ",W";
    }
    //facing south
    if(facingDirection==270){
        int xInt = static_cast<int>(x);
        int yInt = static_cast<int>(y);
        retMessage = to_string(xInt) + "," + to_string(yInt) + ",S";
    }
    return retMessage;
}

//given two states' x, y and facing direction, return the action to be sent to STM
string Network::calculateAction(float x0, float x1, float y0, float y1, int facingDirection0, int facingDirection1){
    //define the actions according to the command list
    string forward10 = "b";
    string reverse10 = "f";
    string turnRight = "j";
    string turnLeft = "i";
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
            if(x < 0){ //turn right when facing east
                return turnRight;
            }
            if(x > 0){ //turn left when facing east
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
                return turnRight;
            }
            if(y > 0){ //turn left when facing east
                return turnLeft;
            }
            if(x!=0){ //assume to turn right by default
                return turnRight;
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
            if(x!=0){ //assume to turn right by default
                return turnRight;
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
            if(y!=0){ //assume to turn right by default
                return turnRight;
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
            if(y!=0){ //assume to turn right by default
                return turnRight;
            }
        }
    }
    //if facing direction differs by + 90, turn left
    if((facingDirection0+90)%360 == facingDirection1){
        return turnLeft;
    }
    //if facing direction differs by -270, turn right
    if((facingDirection0+270)%360 == facingDirection1){
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

