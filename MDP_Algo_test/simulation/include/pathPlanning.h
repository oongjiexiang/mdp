#ifndef PATHPLANNING_H
#define PATHPLANNING_H

#include "component.h"
#include "config.h"
#include "asearch.h"
#include <utility>

typedef pair<Obstacle, vector<State*>> ActionListPerObstacle;

// main class for week 8 and simulation for ShortestPath path
class ShortestPath{
    aStar* astar;
    vector<ActionListPerObstacle> shortestPathSolution;
    vector<Obstacle> obstacles;
    float calculateDist(int x1, int y1, int dir1, int x2, int y2, int dir2);

    public:
        ShortestPath(vector<Obstacle> obstacles);

        // Master Control and Solver for the entire Week 8 problem
        vector<ActionListPerObstacle> hamiltonianPath();

        void pathForObstacle_6(vector<ActionListPerObstacle>& resultPath);
        void pathForObstacle_78(vector<ActionListPerObstacle>& resultPath);

        // calculates each combination of A* between two obstacles and with the initial point
        // returns false if there is no Hamiltonian path
        bool permutation(vector<vector<double>>& pathDistanceList, vector<vector<ActionListPerObstacle>>& pathSolutionList);
};
#endif