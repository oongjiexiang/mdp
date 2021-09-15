#ifndef ASEARCH_H
#define ASEARCH_H

#include "config.h"
#include "math.h"
#include "action.h"
#include <vector>
#include <iostream>
#include <tuple>

// Creating a shortcut for tuple<int, Vertex> type
typedef tuple<double, State*> Tuple;
typedef pair<double, vector<State>> SearchResult;

class aStar{
    Map* grid; 
    vector<Action*> possibleActions;

    bool isDestination(const State& curState, const Obstacle& obstacle);
    
    // A Utility Function to calculate the 'h' heuristics.
    double calculateHValue(State& curState, Obstacle& obstacle);
    
    // Encapsulate g cost calculation
    double calculateGValue(State& curState, Action* action);
    
    // A Utility Function to trace the path from the source to destination
    double tracePath(State* goalState, vector<State*>* states);
    
    public:
        aStar();
        aStar(vector<Obstacle> obstacles);
        void generatePossibleActions(Obstacle obstacle);
        State* search(State& initState, Obstacle& dest, double* pathCost, vector<State*>* states);
};

#endif

