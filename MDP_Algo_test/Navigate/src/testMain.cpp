#include "../../Simulation/component.h"
#include "../../Simulation/config.h"
#include "../../Simulation/action.h"
#include "../../Simulation/asearch.h"
#include "../../Simulation/pathPlanning.h"
#include "../include/moveAround.h"
#include "../include/navigate.h"
#include <iostream>

using namespace std;

int main(){
    // vector<Obstacle> obstacles;
    // Obstacle obs1(1, 10, 9, 270);
    // obstacles.push_back(obs1);
    // obstacles.push_back(Obstacle(3, 10, 9, 90));

    // // ******************test MoveAround.cpp**********************
    // MoveAround ma(obstacles);
    // Vertex* initPosition = new Vertex(10, 7);
    // State* initState = new State(initPosition, false, 90, nullptr);
    // ma.generatePath(initState, obs1);

    // *********************test spiral.cpp*********************
    Navigate algo;
    algo.run();
}