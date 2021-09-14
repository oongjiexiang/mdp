#ifndef PATHPLANNING_H
#define PATHPLANNING_H

#include "tripPlan.h"
#include "component.h"
#include "config.h"

// main class for week 8 and simulation for ShortestPath path
class ShortestPath{
    aStar astar;
    vector<vector<SearchResult>> shortestDist;
    vector<Obstacle&> obstacles;
    ShortestPath(vector<Obstacle&> obstacles);

    // Master Control and Solver for the entire Week 8 problem
    vector<State> hamiltonianPath();

    // calculates each combination of A* between two obstacles and with the initial point
    void permutation(vector<double>& fullPathDistance, vector<vector<State>>& fullPathSolution);
};
#endif