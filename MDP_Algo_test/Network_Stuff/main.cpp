#include <iostream>
#include <string>
#include "NetworkClass.h"
#include "action.h"
#include "pathPlanning.h"

//----extra includes
#include "component.h"
#include "config.h"
#include "asearch.h"
#include <iostream>

using namespace std;

int main()
{
    int checker=0;
    string retMessage="";

    Network n;
    //checker = n.initializeConnection();
    if(checker == 1){
        printf("error during initialization");
        return 1;
    }
    vector<Obstacle> obstacles;
    //message from android to obstacles
    n.readAndGenerateObstacles(obstacles);
    int stateSize;
    ShortestPath sp(obstacles);
    //generate paths
    vector<ActionListPerObstacle> result = sp.hamiltonianPath();
    int resultSize = static_cast<int>(result.size());
    cout << resultSize << endl;
    vector<State*> states;
    for(int i = 0; i < 1; i++){
        cout << "Obstacle-------------------------" << endl;
        result[i].first.printObstacle();
        states = result[i].second;
        stateSize = static_cast<int>(states.size());
        printf("states size %d",stateSize);
        n.sendPath(states,stateSize);
        //states[j]->printState();
        n.sendTakePhoto();
    }
    //n.sendEndToAndroid();
//    n.endConnection();
    return 0;
}


//----------- Old test function (can remove later)-----------------
//    Vertex* initPosition = new Vertex(1, 1);
//    State* initState = new State(initPosition, false, 90, nullptr);
//    aStar* astar = new aStar(obstacles);
//    vector<State> vector1;
//
//    float pathCost = 0;
//
//    bool photoTaken;
//    vector<State*> resultStates;
//    int vectorSize=0;
//    for(int j=0;j<2;j++){
//        astar->search(initState, obstacles[j], &pathCost, &resultStates);
//        printf("obs id: %d",obstacles[0].id);
//        vectorSize = static_cast<int>(resultStates.size());
//        for(int i = 0; i < vectorSize; i++){
//            resultStates[i]->printState();
//            vector1.push_back(*resultStates[i]);
//        }
//        printf("%d\n",vector1[vectorSize-1].face_direction);
//        printf("%d\n",initState->face_direction);
//        photoTaken = n.sendPath(vector1);
//    }

    //hardcoded values of obstacles
    //obstacles.push_back(Obstacle(1, 5, 9, 270));
    //obstacles.push_back(Obstacle(2, 7, 14, 180));
    //obstacles.push_back(Obstacle(3, 12, 9, 0));
    //obstacles.push_back(Obstacle(4, 15, 15, 270));
    //obstacles.push_back(Obstacle(5, 15, 4, 180));
