#include <iostream>
#include <string>
#include "NetworkClass.h"
#include "action.h"
#include "pathPlanning.h"


#include <chrono>
#include <thread>

using namespace std;

int main()
{
    int checker=0;
    string retMessage="";
    Network n;
    checker = n.initializeConnection();
    if(checker == 1){
        printf("error during initialization");
        return 1;
    }
    vector<Obstacle> obstacles;

    n.readAndGenerateObstacles(obstacles);
    printf("obstacles created\n");
    for(int i =0; i<5;i++){
      obstacles[i].printObstacle();
    }
    int stateSize;
    ShortestPath sp(obstacles);
    //generate paths
    vector<ActionListPerObstacle> result = sp.hamiltonianPath();
    int resultSize = static_cast<int>(result.size());
    cout << resultSize << endl;
    vector<State*> states;

    //iterate through the paths to the obstacles
    for(int i = 0; i < resultSize; i++){
        cout << "Obstacle-------------------------" << endl;
        result[i].first.printObstacle();
        states = result[i].second;
        stateSize = static_cast<int>(states.size());
        printf("states size %d",stateSize);
        n.sendPath(states,stateSize);
        n.sendObstacleIdToAndroid(obstacles[i].id);
        n.sendTakePhoto();
    }
    n.sendEndToAndroid();
    n.endConnection();
    return 0;
}

