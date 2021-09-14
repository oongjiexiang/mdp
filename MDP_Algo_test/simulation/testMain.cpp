#include "component.h"
#include "config.h"
#include "action.h"
#include <iostream>

using namespace std;

int main(){
    vector<Obstacle> obstacles;
    // obstacles.push_back(Obstacle(1, 2, 2, 0));
    // obstacles.push_back(Obstacle(2, 2, 11, 90));
    obstacles.push_back(Obstacle(6, 5, 1, 270));
    obstacles.push_back(Obstacle(3, 10, 9, -100));
    obstacles.push_back(Obstacle(5, 13, 14, 90));
    Map map(obstacles);
    map.printMap();
    // map.findVertexByCoor(15.0, 10)->printVertex();
    // map.findVertexByGrid(10, 3)->printVertex();
    // cout << map.isValidCoor(0, 0) << endl;
    // cout << map.isValidGrid(1, 300000) << endl;

    //************** test action.cpp****************
    // Vertex* initPosition = new Vertex(9, 7);
    // State* initState = new State(initPosition, false, 90, nullptr, "");
    // initState->printState();
    // ActionForward* af = new ActionForward(10);
    // ActionReverse* ar = new ActionReverse(20);
    // ActionDetect* ad = new ActionDetect(5);
    // ActionTurn* at = new ActionTurn(90);
    // cout << af->canTakeAction(initState, map) << endl;
    // cout << ar->canTakeAction(initState, map) << endl;
    // cout << ad->canTakeAction(initState, map) << endl;
    // cout << at->canTakeAction(initState, map) << endl;
    
}