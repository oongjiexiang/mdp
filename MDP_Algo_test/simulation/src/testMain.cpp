#include "component.h"
#include "config.h"
#include "action.h"
#include "asearch.h"
#include "pathPlanning.h"
#include "actionFormulator.h"
#include <iostream>

using namespace std;

int main(){
    // vector<Obstacle> obstacles;
    // obstacles.push_back(Obstacle(1, 2, 9, 180));

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
    // obstacles.push_back(Obstacle(3, 6, 9, 0));
    // obstacles.push_back(Obstacle(5, 13, 14, 90));
    // Map map(obstacles, FROM_BORDER_GRID_LENGTH);
    // // Map map(obstacles, 0);
    // map.printMap();
    // map.findVertexByCoor(-10, 20)->printVertex();
    // map.findVertexByGrid(-1, -3)->printVertex();
    // cout << map.isValidCoor(-50, -30) << endl;
    // cout << map.isValidGrid(-5, -3) << endl;

    //************** test action.cpp****************
    // vector<Obstacle> obstacles;
    // obstacles.push_back(Obstacle(1, 7, 7, 0));
    // obstacles.push_back(Obstacle(2, 3, 3, 0));
    // Map map(obstacles, FROM_BORDER_GRID_LENGTH);
    // map.printMap();
    // Vertex* initPosition = new Vertex(10 + FROM_BORDER_GRID_LENGTH, 7 + FROM_BORDER_GRID_LENGTH);
    // State* initState = new State(initPosition, 180, nullptr);
    // initState->printState();
    // ActionStraight* aforward = new ActionStraight(1);
    // ActionStraight* areverse = new ActionStraight(-1);
    // ActionTurn2By4* at = new ActionTurn2By4(-90);
    // ActionReverseTurn2By4* art = new ActionReverseTurn2By4(-90);
    // cout << "forward direction" << endl;
    // aforward->takeAction(initState, map)->printState();
    // map.printMap();
    // cout << "reverse" << endl;
    // areverse->takeAction(initState, map);
    // map.printMap();
    // cout << "turn right" << endl;
    // at->takeAction(initState, map);
    // map.printMap();
    // cout << "reverse turn right" << endl;
    // art->takeAction(initState, map);
    // map.printMap();

    //**************** test asearch.cpp ***************
    vector<Obstacle> obstacles;
    obstacles.push_back(Obstacle(1, 8, 19, 90));
    Vertex* initPosition = new Vertex(ROBOT_INIT_X_GRID, ROBOT_INIT_Y_GRID);
    State* initState = new State(initPosition, 90, nullptr);
    aStar* astar = new aStar(obstacles, FROM_BORDER_GRID_LENGTH);
    
    float pathCost = 0;
    vector<State*> resultStates;
    astar->search(initState, obstacles[0], &pathCost, &resultStates);
    
    cout << "-----------path--------------" << endl;
    for(int i = 0; i < resultStates.size(); i++){
        resultStates[i]->printState();
    }
    cout << pathCost << endl;

    //*******************test pathPlanning.cpp ****************
    // vector<Obstacle> obstacles;
    // obstacles.push_back(Obstacle(1, 2, 9, 180));
    // ShortestPath sp(obstacles);
    // vector<ActionListPerObstacle> result = sp.hamiltonianPath();
    // cout << result.size() << endl;
    // for(int i = 0; i < result.size(); i++){
    //     cout << "Obstacle-------------------------" << endl;
    //     result[i].first.printObstacle();
    //     vector<State*> states = result[i].second;
    //     for(int j = 0; j < states.size(); j++){
    //         states[j]->printState();
    //     }
    // }

    //***********************test ActionFormulator.cpp ************************
    // vector<Obstacle> obstacles;
    // obstacles.push_back(Obstacle(1, 5, 9, 270));
    // obstacles.push_back(Obstacle(2, 7, 14, 180));
    // obstacles.push_back(Obstacle(3, 12, 9, 0));
    // obstacles.push_back(Obstacle(4, 15, 15, 270));
    // obstacles.push_back(Obstacle(5, 15, 4, 180));

    // ShortestPath sp(obstacles);
    // vector<ActionListPerObstacle> result = sp.hamiltonianPath();

    // // print results
    // ActionFormulator* af = new FormulatorShorten();
    // vector<State*> states;
    // for(int i = 0; i < result.size(); i++){
    //     Obstacle o = result[i].first;
    //     states.insert(states.end(), result[i].second.begin(), result[i].second.end());
    // }
    // af->convertToActualActions(states);
    // cout << result.size() << endl;

    //**************************test pathPlanning.cpp again***********************
    // vector<Obstacle> obstacles;
    // obstacles.push_back(Obstacle(1, 3, 9, 180));

    // ShortestPath sp(obstacles);
    // vector<ActionListPerObstacle> result = sp.hamiltonianPath();

    // for(int i = 0; i < result.size(); i++){
    //     Obstacle o = result[i].first;
    //     for(int j = 0; j < result[i].second.size(); j++){
    //         result[i].second[j]->printState();
    //     }
    
    // print results
    // ActionFormulator* af = new FormulatorShorten();
    // vector<State*> states;
    // }
    // af->convertToActualActions(states);
}