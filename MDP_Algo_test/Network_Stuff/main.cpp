#include <iostream>
#include <string>
#include "NetworkClass.h"
#include "action.h"
#include "pathPlanning.h"
#include "actionFormulator.h"

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
    for(int i =0; i<obstacles.size();i++){
      obstacles[i].printObstacle();
    }
    int stateSize;
    ShortestPath sp(obstacles);
    //generate paths
    vector<ActionListPerObstacle> result = sp.hamiltonianPath();
    int resultSize = static_cast<int>(result.size());
    cout << resultSize << endl;
    vector<State*> states;
    vector<Action*> actions;
    // new
    ActionFormulator* af = new FormulatorShorten();

    //iterate through the paths to the obstacles
    for(int i = 0; i < resultSize; i++){
        cout << "Obstacle-------------------------" << endl;
        result[i].first.printObstacle();
        states = result[i].second;
        // new
        actions=af->convertToActualActions(states);
        stateSize = static_cast<int>(states.size());
        printf("\n=========step path=============\n\n");
        //n.sendPath(states,stateSize,actions);
        printf("\n=========2nd path function============\n\n");
        n.sendCombinedActionPath(states,stateSize,actions);
        n.sendObstacleIdToAndroid(result[i].first.id);
        //n.sendCalibrateToSTM();
        n.sendTakePhoto();
    }
    n.sendEndToAndroid();
    //n.endConnection();
    return 0;
}

