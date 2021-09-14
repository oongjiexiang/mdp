#include "config.h"
#include "component.h"
#include "asearch.h"
#include "pathPlanning.h"
#include <vector>
#include <utility>
#include <algorithm>



ShortestPath::ShortestPath(vector<Obstacle&> obstacles): obstacles(obstacles){
    astar = aStar(obstacles);
    int nObstacles = obstacles.size();
    shortestDist.resize(nObstacles + 1, vector<SearchResult>(nObstacles + 1));
}

vector<State> ShortestPath::hamiltonianPath(){
    vector<double> completePathDistance;
    vector<vector<State>> completePathSolution;
    permutation(completePathDistance, completePathSolution);
    int minPathIndex = 0;
    double minPathDist = DBL_MAX;
    for(int i = 0; i < completePathDistance.size(); i++){
        if(minPathDist < completePathDistance[i]){
            minPathDist = completePathDistance[i];
            minPathIndex = i;
        }
    }
    return completePathSolution[minPathIndex];
}

void ShortestPath::permutation(vector<double>& completePathDistance, vector<vector<State>>& completePathSolution){
    vector<int> goal_ids;
    for(int i = 1; i <= obstacles.size(); i++){
        goal_ids.push_back(i);
    }
    sort(goal_ids.begin(), goal_ids.end());
    do{
        double totalDist = 0;
        vector<State> fullPathSolution;
        Vertex robotInitPosition(ROBOT_INIT_ROW, ROBOT_INIT_COL); 
        State* initState = new State(robotInitPosition, false, ROBOT_INIT_FACEDIRECTION, nullptr, nullptr);
        vector<SearchResult> pathSolution(obstacles.size());
        for(int i = 0; i < goal_ids.size(); i++){
            initState = astar.search(*initState, obstacles[i], pathSolution[i]);
        }
        // combine path solution from one obstacle to another into a single path solution
        for(int i = 0; i < pathSolution.size(); i++){
            totalDist+=pathSolution[i].first;
            fullPathSolution.insert(fullPathSolution.end(), pathSolution.begin(), pathSolution.end());
        }
        completePathDistance.push_back(totalDist);
        completePathSolution.push_back(fullPathSolution);
    // + 1 because first position is always initial point with id = 0
    }while(next_permutation(goal_ids.begin() + 1, goal_ids.end())); 
}