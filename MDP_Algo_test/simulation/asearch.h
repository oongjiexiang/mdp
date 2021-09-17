#ifndef ASEARCH_H
#define ASEARCH_H

#include "config.h"
#include "math.h"
#include "action.h"
#include <vector>
#include <iostream>
#include <tuple>

// Creating a shortcut for tuple<int, Vertex> type
typedef tuple<float, State*> Tuple;
typedef pair<float, vector<State*>> SearchResult;

class aStar{
    Map* grid; 
    vector<Action*> possibleActions;

    bool isDestination(const State& curState, const Obstacle& obstacle);
    
    // A Utility Function to calculate the 'h' heuristics.
    float calculateHValue(State& curState, Obstacle& obstacle);
    
    // Encapsulate g cost calculation
    float calculateGValue(State& curState, Action* action, Map& localMap, Obstacle& obstacle);
    
    // A Utility Function to trace the path from the source to destination
    float tracePath(State* goalState, vector<State*>* states);

    // We can include more actions available for the robot. Currently there are 5
    void generatePossibleActions(Obstacle obstacle);
    
    public:
        aStar();
        aStar(vector<Obstacle> obstacles);
        State* search(State* initState, Obstacle& dest, float* pathCost, vector<State*>* states);
};

#endif

