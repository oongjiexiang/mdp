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
#include <vector>
#include <chrono>
#include <thread>
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

//encodes the message, target device 1 for android, 2 for stm, 3 for rpi, assumes the unformatted message is correct
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
    do{
        //clear buffer before receive
        memset(receiveBuffer,'\n',sizeof(char)*bufferLength);
        //receive message from server
        checker = recv(ConnectSocket, receiveBuffer,bufferLength,0);

        printf("%c",receiveBuffer[0]);
        if(checker>0){ //bytes are being received
            printf("Bytes received: %d\n", checker);
            printf("Message received: %s\n",decodeMessage().c_str());
        }
        else if (checker == 0){ //no more bytes received
            //printf("Connection closed\n");
        }
        else{ //something went wrong{
            printf("receive failed: %d\n", WSAGetLastError());
            break;
        }
    }while(checker<=0);
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

//
////send the steps from a* search to android and stm
////OLD, no need to use
//bool Network::sendPath(vector<State*>& vectorOfStates, int noOfStates, vector<Action*>& vectorOfAction){
//    int currentStateIndex, facingDirection0, facingDirection1;
//    float x0, x1, y0, y1;
//    string currentExpectedMsg = "";
//    string expectedMsgFromSTM0 = "R";//a for ready, change this if there are changes from STM side
//    string expectedMsgFromSTM1 = "r";
//    int expectedMessage = 0;
//    string andMsg = "";
//    string stmMsg = "";
//    string replyMessage = "";
//    //noOfStates-1 as last state+1 is OOB
//    for(currentStateIndex = 0; currentStateIndex < (noOfStates-1); currentStateIndex++){
//        x0 = vectorOfStates[currentStateIndex]->position->x_left;
//        x1 = vectorOfStates[currentStateIndex+1]->position->x_left;
//        y0 = vectorOfStates[currentStateIndex]->position->y_low;
//        y1 = vectorOfStates[currentStateIndex+1]->position->y_low;
//        facingDirection0 = vectorOfStates[currentStateIndex]->face_direction;
//        facingDirection1 = vectorOfStates[currentStateIndex+1]->face_direction;
//        //create the messages
//        printf("\nx0 %f, y0 %f, facingDirection0 %d | x1 %f, y1 %f, facingDirection1 %d\n",x0,y0,facingDirection0,x1,y1,facingDirection1);
//        stmMsg = calculateAction(x0,x1,y0,y1,facingDirection0,facingDirection1);
//        andMsg = generateAndroidMessage(x1,y1,facingDirection1,checkMsgSent(stmMsg));
//        //encode the messages
//        stmMsg = encodeMessage(2,stmMsg);
//        andMsg = encodeMessage(1,andMsg);
//        sendMessage(andMsg);
//        sendMessage(stmMsg);
//
//        //wait for reply message from STM side
//        while(true){
//            replyMessage="";
//            if(expectedMessage==0){
//                currentExpectedMsg=expectedMsgFromSTM0;
//                expectedMessage=1;
//            }
//            else{
//                currentExpectedMsg=expectedMsgFromSTM0;
//                expectedMessage=0;
//            }
//            receiveMessage();
//            replyMessage = decodeMessage();
//            printf("reply message %s\ncurrent expectedMsg %s", replyMessage.c_str(),currentExpectedMsg.c_str());
//            if(replyMessage.substr(0,1)==(currentExpectedMsg)){
//                break;
//            }
//        }
//        replyMessage="";
//    }
//    return true;
//}

bool Network::sendCombinedActionPath(vector<State*>& vectorOfStates, int noOfStates, vector<Action*>& vectorOfAction){
    int androidMessageIndex, facingDirection0, facingDirection1;
    float x0, x1, y0, y1;
    string currentExpectedMsg = "";
    string expectedMsgFromSTM0 = "R";//a for ready, change this if there are changes from STM side
    string expectedMsgFromSTM1 = "r";
    int expectedMessage = 0;
    string andMsg = "";
    string stmMsg = "";
    string encodedStmMsg = "";
    string currentStateAction = "";
    string nextStateAction = "";
    string replyMessage = "";
    androidMessageIndex =0;
    int actionVectorSize = static_cast<int>(vectorOfAction.size());
    printf("number of actions:%d",actionVectorSize);
    for(int i=0; i<actionVectorSize;i++){
    //send stm message first
        printf("\n\nnew action");
        stmMsg = calculateActionNew(vectorOfAction[i]);
        encodedStmMsg = encodeMessage(2,stmMsg);
        sendMessage(encodedStmMsg);
        //prepare android message
        //check if we are sending  combined forward/backward for stm commands
        do{
            if(androidMessageIndex+1>=noOfStates){
            break;
            }
            x0 = vectorOfStates[androidMessageIndex]->position->x_left;
            x1 = vectorOfStates[androidMessageIndex+1]->position->x_left;
            y0 = vectorOfStates[androidMessageIndex]->position->y_low;
            y1 = vectorOfStates[androidMessageIndex+1]->position->y_low;
            facingDirection0 = vectorOfStates[androidMessageIndex]->face_direction;
            facingDirection1 = vectorOfStates[androidMessageIndex+1]->face_direction;
            currentStateAction = calculateAction(x0,x1,y0,y1,facingDirection0,facingDirection1);
            printf("x0 %f, y0 %f, facingDirection0 %d | x1 %f, y1 %f, facingDirection1 %d\n",x0,y0,facingDirection0,x1,y1,facingDirection1);
            andMsg = generateAndroidMessage(x1,y1,facingDirection1,checkMsgSent(stmMsg));
            andMsg = encodeMessage(1,andMsg);
            //wait for a short time before sending another message to android to prevent crashing
            this_thread::sleep_for(500ms);
            sendMessage(andMsg);

            if(androidMessageIndex+2>=noOfStates){
                androidMessageIndex++;
                continue;
            }
            //calculate the next action STM will take
            x0 = vectorOfStates[androidMessageIndex+1]->position->x_left;
            x1 = vectorOfStates[androidMessageIndex+2]->position->x_left;
            y0 = vectorOfStates[androidMessageIndex+1]->position->y_low;
            y1 = vectorOfStates[androidMessageIndex+2]->position->y_low;
            facingDirection0 = vectorOfStates[androidMessageIndex+1]->face_direction;
            facingDirection1 = vectorOfStates[androidMessageIndex+2]->face_direction;
            nextStateAction = calculateAction(x0,x1,y0,y1,facingDirection0,facingDirection1);
            androidMessageIndex++;
            printf("current action state %s | next state action %s\n", currentStateAction.c_str(),nextStateAction.c_str());
        }while(currentStateAction==nextStateAction && (nextStateAction=="b" || nextStateAction=="f") && androidMessageIndex<noOfStates);
        printf("finish sending to android\n");
        while(true){
            replyMessage="";
            if(expectedMessage==0){
                currentExpectedMsg=expectedMsgFromSTM0;
                expectedMessage=1;
            }
            else{
                currentExpectedMsg=expectedMsgFromSTM0;
                expectedMessage=0;
            }
            receiveMessage();
            replyMessage = decodeMessage();
            printf("reply message %s\ncurrent expectedMsg %s", replyMessage.c_str(),currentExpectedMsg.c_str());
            if(replyMessage.substr(0,1)==(currentExpectedMsg)){
                break;
            }
        }
        replyMessage="";
    }
    return true;
}

//return the command that was sent to stm
int Network::checkMsgSent(string stmMsg){
        if(stmMsg.substr(0,1).compare("b")==0 || stmMsg.substr(0,1).compare("p")==0 ){ //forward
            return 0;
        }
        if(stmMsg.substr(0,1).compare("f")==0 || stmMsg.substr(0,1).compare("s")==0){ //reverse
            return 1;
        }

        if(stmMsg.substr(0,1).compare("j")==0){ //forward right
            return 2;
        }

        if(stmMsg.substr(0,1).compare("i")==0){ //forward left
            return 3;
        }

        if(stmMsg.substr(0,1).compare("A")==0){ //reverse right
            return 4;
        }
        if(stmMsg.substr(0,1).compare("B")==0){ //reverse left
            return 5;
        }
        if(stmMsg.substr(0,1).compare("C")==0){//turn on the spot right
           return 6;
           }
        if(stmMsg.substr(0,1).compare("E")==0){//turn on the spot left
           return 7;
           }
        printf("Error when checking stmMsg %s",stmMsg.c_str());
        return -1;
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
int Network::sendObstacleIdToAndroid(int obstacleId){
    string sObstacleId = to_string(obstacleId);
    string message="ALG,"+sObstacleId;
    message = encodeMessage(1,message);
    sendMessage(message);
    return 1;
}

//end of search, tell android to stop
int Network::sendEndToAndroid(){
    string message="";
    message = encodeMessage(1,"END");
    sendMessage(message);
    return 1;
}


//end of search, tell android to stop
int Network::sendCalibrateToSTM(){
    string message="";
    string replyMessage = "";
    message = encodeMessage(2,"n");
    sendMessage(message);
    while(true){
        replyMessage="";
        receiveMessage();
        replyMessage = decodeMessage();
        printf("reply message %s", replyMessage.c_str());
        if(replyMessage.substr(0,1)==("R")){
            break;
        }
    }
    return 1;
}

//wait and read the message from android to generate the obstacles
int Network::readAndGenerateObstacles(vector<Obstacle>& obstacles){
    vector<int> xVector;
    vector<int> yVector;
    vector<int> fVector;
    int noOfObstacles = 0;
    string msg = "";
    //test message, remove later
//    msg = "5.5,9.5,S;7.5,14.5,W;12.5,9,E;15.5,15.5,S;15.5,4.5,W;\n";
//    convertAndroidMessage(msg,xVector,yVector,fVector);
    while(true){
        receiveMessage();
        msg = decodeMessage();
        //convert android message and create obstacles
        convertAndroidMessage(msg,xVector,yVector,fVector);
        //xVector will only have a size greater than 0 when a message from android has been converted
        //if the message only contains 'R' from STM, keep reading, if message from android read, break
        if(xVector.size()>0){
             break;
        }
    }

    //generate the obstacles and store them into the obstacle array
    noOfObstacles = static_cast<int>(xVector.size());
    //printf("%d number of obstacles",noOfObstacles);
    //test to print the first set of obstacle
    for(int i=0; i < noOfObstacles; i++){
        //printf("\n%d,%d,%d,%d\n",i,xVector[i],yVector[i],fVector[i]);
        obstacles.push_back(Obstacle(i+1,xVector[i],yVector[i],fVector[i]));
    }
    return 1;
}

//convert the message telling us the locations of the obstacles android sent
//message in the format of x,y,F\nx,y,F\n
int Network::convertAndroidMessage(string message, vector<int>& xVector, vector<int>& yVector, vector<int>& facingDirection){
    string currentValue = "";
    string currentMessage = "";
    int newIntValue;
    int msgLen = message.length();
    int positionCounter = 0; // 0 = x, 1 = y

    //loop through the message
    for(int i = 0; i < msgLen; i++){
        currentValue = message.substr(i,1);
        //skip R and r from stm
        if( currentValue.compare("\n")==0){
            return 0;
        }
        if(currentValue.compare("R")==0 || currentValue.compare("r")==0){
            continue;
        }
        //if newline character, end of facing direction, start new loop
        if(currentValue.compare(";")==0){ //handle r case
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
            if(currentMessage.length()>0){
            newIntValue = stoi(currentMessage);
            facingDirection.push_back(newIntValue);
            positionCounter = 0;
            currentMessage = "";
            }
        }
        else if(currentValue.compare(",")==0){
            //values of x
            if(positionCounter==0){
                newIntValue = stof(currentMessage)-0.5;
                xVector.push_back(newIntValue);
            }
            //values of y
            if(positionCounter==1){
                newIntValue = stof(currentMessage)-0.5;
                yVector.push_back(newIntValue);
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
string Network::generateAndroidMessage(float x, float y, int facingDirection, int stmMsgNumber){
    string retMessage ="";
    string message[8] = {"f","r","fr","fl","rr","rl","sr","sl"}; //make sure this is in the same order as calculate action
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
    retMessage = retMessage + "," + message[stmMsgNumber];
    return retMessage;
}

//given two states' x, y and facing direction, return the action to be sent to STM
string Network::calculateAction(float x0, float x1, float y0, float y1, int facingDirection0, int facingDirection1){
    //define the actions according to the command list
    string forward10 = "b";
    string reverse10 = "f";
    string turnRight = "j";
    string turnLeft = "i";
    string reverseRight = "A";
    string reverseLeft = "B";
    string onTheSpotTurnRight = "C";
    string onTheSpotTurnLeft = "E";
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
    //if facing opposite directions, should not happen
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

        if(x==0 && y==0){
            return onTheSpotTurnLeft;
        }
    }
    //if facing direction differs by + 90, turn left
    if((facingDirection0+90)%360 == facingDirection1){
        if(facingDirection0 == 0){ //face east
            //check if y changes
            if(y < 0){
                return reverseRight;
            }
            if(y > 0){
                return turnLeft;
            }
            if(x > 0){
                return turnLeft;
            }
            if(x < 0){
                return reverseRight;
            }
        }
        if(facingDirection0 == 90){
            //check if y changes
            if(y < 0){
                return reverseRight;
            }
            if(y > 0){
                return turnLeft;
            }
            if(x > 0){
                return reverseRight;
            }
            if(x < 0){
                return turnLeft;
            }
        }
        if(facingDirection0 == 180){
            //check if y changes
            if(y < 0){
                return turnLeft;
            }
            if(y > 0){
                return reverseRight;
            }
            if(x > 0){
                return reverseRight;
            }
            if(x < 0){
                return turnLeft;
            }
        }
        if(facingDirection0 == 270){
            //check if y changes
            if(y < 0){
                return turnLeft;
            }
            if(y > 0){
                return reverseRight;
            }
            if(x > 0){
                return reverseRight;
            }
            if(x < 0){
                return turnLeft;
            }
        }
        if(x==0 && y==0){
            return onTheSpotTurnLeft;
        }
    }
    //if facing direction differs by -270, turn right
    if((facingDirection0+270)%360 == facingDirection1){
        if(facingDirection0 == 0){
            //check if y changes
            if(y < 0){
                return turnRight;
            }
            if(y > 0){
                return reverseLeft;
            }
            if(x > 0){
                return turnRight;
            }
            if(x < 0){
                return reverseLeft;
            }
        }
        if(facingDirection0 == 90){
            //check if y changes
            if(y < 0){
                return reverseLeft;
            }
            if(y > 0){
                return turnRight;
            }
            if(x > 0){
                return turnRight;
            }
            if(x < 0){
                return reverseLeft;
            }
        }
        if(facingDirection0 == 180){
            //check if y changes
            if(y < 0){
                return reverseLeft;
            }
            if(y > 0){
                return turnRight;
            }
            if(x > 0){
                return reverseLeft;
            }
            if(x < 0){
                return turnRight;
            }
        }
        if(facingDirection0 == 270){
            //check if y changes
            if(y < 0){
                return turnRight;
            }
            if(y > 0){
                return reverseLeft;
            }
            if(x > 0){
                return reverseLeft;
            }
            if(x < 0){
                return turnRight;
            }
        }
        if(x==0 && y==0){
            return onTheSpotTurnRight;
        }
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

//calculate the msg to send to stm based on the action vector provided
string Network::calculateActionNew(Action* actionVector){
    string forward10 = "p";
    string reverse10 = "f";
    string turnRight = "j";
    string turnLeft = "i";
    string reverseRight = "A";
    string reverseLeft = "B";
    string returnMsg ="";
    string turnOnSpotRight = "C";
    string turnOnSpotLeft = "E";
    char buffer [100];
    ActionStraight* aPtr = dynamic_cast<ActionStraight*>(actionVector);
        if(aPtr!=nullptr){
            if(aPtr->getTravelDistGrid()>0){ //check if forward
                if(aPtr->getTravelDistGrid()>=10)
                    sprintf(buffer,"p%d",aPtr->getTravelDistGrid()*10);
                else{
                    sprintf(buffer,"p0%d",aPtr->getTravelDistGrid()*10);
                }
                returnMsg=buffer;
            }
            else if(aPtr->getTravelDistGrid()<0){ //check if backward
                if(aPtr->getTravelDistGrid()>=10)
                    sprintf(buffer,"s%d",abs(aPtr->getTravelDistGrid()*10));
                else{
                    sprintf(buffer,"s0%d",abs(aPtr->getTravelDistGrid()*10));
                }
                returnMsg=buffer;

                //returnMsg = "f"; //currently no instructions for reverseXXX, replace this once we have the function
            }
        }
    ActionTurn2By4* atPtr = dynamic_cast<ActionTurn2By4*>(actionVector);
        if(atPtr!=nullptr){
            if(atPtr->getTurnAngle()>0){
                returnMsg = turnLeft;
            }
            else{
                returnMsg = turnRight;
            }
        }
    ActionReverseTurn2By4* arPtr = dynamic_cast<ActionReverseTurn2By4*>(actionVector);
        if(arPtr!=nullptr){
            if(arPtr->getTurnAngle()>0){
                returnMsg = reverseLeft;
            }
            else{
                returnMsg = reverseRight;
            }
        }
    ActionTurnOnSpot* atosPtr = dynamic_cast<ActionTurnOnSpot*>(actionVector);
        if(atosPtr!=nullptr){
            if(atosPtr->getTurnAngle()>0){
                returnMsg = turnOnSpotLeft;
            }
            else{
                returnMsg = turnOnSpotRight;
            }
        }

    return returnMsg;
}
