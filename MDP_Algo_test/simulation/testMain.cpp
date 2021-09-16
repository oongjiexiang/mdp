#include "component.h"
#include "config.h"
#include "action.h"
#include "asearch.h"
#include "pathPlanning.h"
#include <iostream>

using namespace std;

int main(){
    vector<Obstacle> obstacles;
    obstacles.push_back(Obstacle(1, 5, 5, 270));

    //*********************test component.cpp******************
    // Robot r(3, 4, 90);
    // Vertex v(1, 10);
    // Obstacle o(1, 7, 7, 90);
    // r.printRobot();
    // v.printVertex();
    // o.printObstacle();


    //***************** test config.cpp********************
    // obstacles.push_back(Obstacle(2, 3, 3, 0));
    // obstacles.push_back(Obstacle(6, 5, 1, 270));
    obstacles.push_back(Obstacle(3, 10, 9, 90));
    // obstacles.push_back(Obstacle(5, 13, 14, 90));
    // Map map(obstacles);
    // map.printMap();
    // map.findVertexByCoor(15.0, 10)->printVertex();
    // map.findVertexByGrid(10, 3)->printVertex();
    // cout << map.isValidCoor(0, 0) << endl;
    // cout << map.isValidGrid(1, 300000) << endl;

    //************** test action.cpp****************
    // vector<Obstacle> obstacles;
    // obstacles.push_back(Obstacle(1, 7, 7, 0));
    // obstacles.push_back(Obstacle(2, 3, 3, 0));
    // Map map(obstacles);
    // map.printMap();
    // Vertex* initPosition = new Vertex(7, 8);
    // State* initState = new State(initPosition, false, 180, nullptr);
    // initState->printState();
    // ActionStraight* aforward = new ActionStraight(1);
    // ActionStraight* areverse = new ActionStraight(-1);
    // ActionDetect* ad = new ActionDetect(2);
    // ActionTurn* at = new ActionTurn(-90);
    // aforward->takeAction(initState, map)->printState();
    // map.printMap();
    // cout << areverse->takeAction(initState, map) << endl;
    // map.printMap();
    // cout << ad->takeAction(initState, map) << endl;
    // map.printMap();
    // cout << at->takeAction(initState, map) << endl;
    // map.printMap();

    //**************** test asearch.cpp ***************
    // Vertex* initPosition = new Vertex(4, 2);
    // State* initState = new State(initPosition, false, 90, nullptr);
    // aStar* astar = new aStar(obstacles);
    
    // float pathCost = 0;
    // vector<State*> resultStates;
    // astar->search(*initState, obstacles[0], &pathCost, &resultStates);
    
    // cout << "-----------path--------------" << endl;
    // for(int i = 0; i < resultStates.size(); i++){
    //     resultStates[i]->printState();
    // }
    // cout << pathCost << endl;

    //*******************test pathPlanning.cpp ****************
    ShortestPath sp(obstacles);
    vector<ActionListPerObstacle> result = sp.hamiltonianPath();
    cout << result.size() << endl;
    for(int i = 0; i < result.size(); i++){
        cout << "Obstacle-------------------------" << endl;
        result[i].first.printObstacle();
        vector<State*> states = result[i].second;
        for(int j = 0; j < states.size(); j++){
            states[j]->printState();
        }
    }
}