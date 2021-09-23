#include <iostream>
#include <string>
#include "NetworkClass.h"
#include "action.h"
#include "pathPlanning.h"

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
    string retMessage="";
    Network n;
    //checker = n.initializeConnection();
    if(checker == 1){
        printf("error during initialization");
        return 1;
    }
//    Vertex v1 = Vertex(0,0);
//    Vertex v2 = Vertex(1,0);
//    Vertex v3 = Vertex(0,1);
//    Vertex v4 = Vertex(1,1);
//
//    State s1 = State(&v1, 0, 0, NULL);
//    State s2 = State(&v2, 0, 0, &s2);
//    State s3 = State(&v3, 0, 180, &s3);
//    State s4 = State(&v4, 0, 90, &s4);
//
//    vector<State> vector1= {};
//    vector1.push_back(s1);
//    vector1.push_back(s2);
//    vector1.push_back(s3);
//    vector1.push_back(s4);
//    printf("%d\n",static_cast<int>(vector1.size()));
    //ZY: set vector1 = vector produced by a*/spiral
    //vector<State> = vector1 = {};

    vector<Obstacle> obstacles;
    obstacles.push_back(Obstacle(1, 4, 9, 270));
    obstacles.push_back(Obstacle(3, 10, 9, 90));
    Vertex* initPosition = new Vertex(4, 2);
    State* initState = new State(initPosition, false, 90, nullptr);
    aStar* astar = new aStar(obstacles);
    
    float pathCost = 0;
    vector<State*> resultStates;
    astar->search(initState, obstacles[0], &pathCost, &resultStates);

    vector<State> vector1;
    for(int i = 0; i < resultStates.size(); i++){
        resultStates[i]->printState();
        vector1.push_back(*resultStates[i]);
    }


    n.sendPath(vector1);
    retMessage = n.decodeMessage();
    printf("%s",retMessage.c_str());

    n.endConnection();
    return 0;
}

//
//    message0 = n.encodeMessage(2, message0);
//    checker = n.sendMessage(message0);
//    printf("Msg 1 sent");
//
//    message1 = n.encodeMessage(2, message1);
//    checker = n.sendMessage(message1);
//
//    message2 = n.encodeMessage(2, message2);
//    checker = n.sendMessage(message2);
//
//    message3 = n.encodeMessage(2, message3);
//    checker = n.sendMessage(message3);
//
//    checker = n.messageSender(longMsg,2);
//
//    if(checker == 1){
//        printf("error during send message");
//        return 1;
//    }
