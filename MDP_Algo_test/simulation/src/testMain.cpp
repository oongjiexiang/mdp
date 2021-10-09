#include "component.h"
#include "config.h"
#include "action.h"
#include "asearch.h"
#include "pathPlanning.h"
#include "actionFormulator.h"
#include <iostream>

using namespace std;

int main(){
    // 1. Set up environment
    vector<Obstacle> obstacles;
    obstacles.push_back(Obstacle(1, 5, 9, 270));
    obstacles.push_back(Obstacle(2, 7, 14, 180));
    obstacles.push_back(Obstacle(3, 12, 9, 0));
    obstacles.push_back(Obstacle(4, 15, 15, 270));
    obstacles.push_back(Obstacle(5, 15, 4, 180));

    // 2. Find shortest path
    ShortestPath sp(obstacles);
    vector<ActionListPerObstacle> result = sp.hamiltonianPath();

    // 3. Print results
    // do not combine all states and send to FormulatorShorten in one go, as forward and reverse during consecutive obstacle targets will be combined
    ActionFormulator* af = new FormulatorShorten();
    for(int i = 0; i < result.size(); i++){
        Obstacle o = result[i].first;
        af->convertToActualActions(result[i].second);
    }
}