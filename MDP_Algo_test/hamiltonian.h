#ifndef HAMILTONIAN_H
#define HAMILTONIAN_H

#include "tripPlan.h"
#include "component.h"
#include "config.h"

typedef pair<double, vector<int>> pathDist;
// main class for week 8 and simulation for Hamiltonian path
class Hamiltonian{
    aStar astar;
    vector<vector<SearchResult>> shortestDist;
    vector<Obstacle> obstacles;
    Hamiltonian(vector<vector<int>> fullMap, vector<Obstacle> obstacles);

    // perform A* search between all obstacles and initial point and keep them for permutation later
    void shortestPathMatrix();

    // Master Control and Solver for the entire Week 8 problem
    void hamiltonianPath();

    // calculates each combination of A* between two obstacles and with the initial point
    void permutation(vector<pathDist>& paths);
};
#endif