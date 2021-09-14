#ifndef ASEARCH_H
#define ASEARCH_H

#include "config.h"
#include "math.h"
#include "action.h"
#include <vector>
#include <iostream>
#include <tuple>

// Creating a shortcut for tuple<int, Vertex> type
typedef tuple<double, State> Tuple;
typedef pair<double, vector<State>> SearchResult;

const int ROBOT_INIT_ROW = 1;
const int ROBOT_INIT_COL = 1;
const double ROBOT_INIT_FACEDIRECTION = 90;

class aStar{
    Map* grid; 
    vector<Action*> possibleActions;

    bool isDestination(const State& curState, const Obstacle& obstacle);
    
    // A Utility Function to calculate the 'h' heuristics.
    double calculateHValue(const State& curState, const Obstacle& obstacle);
    
    // Encapsulate g cost calculation
    double calculateGValue(State& curState, Action* action);
    
    // A Utility Function to trace the path from the source to destination
    void tracePath(State goalState, SearchResult& searchResult);
    
    public:
        aStar();
        aStar(vector<Obstacle&> obstacles);
        State* search(State& initState, Obstacle& dest, SearchResult& searchResult);
};

#endif

