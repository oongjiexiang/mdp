#include "config.h"
#include "component.h"
#include "action.h"
#include "asearch.h"
#include "pathPlanning.h"
#include <vector>
#include <utility>
#include <algorithm>



ShortestPath::ShortestPath(vector<Obstacle> obstacles): obstacles(obstacles){
    astar = aStar(obstacles);
    int nObstacles = obstacles.size();
    // shortestPathSolution.resize(nObstacles);
}

vector<ActionListPerObstacle> ShortestPath::hamiltonianPath(){
    vector<vector<double>> pathDistanceList;
    vector<vector<ActionListPerObstacle>> pathSolutionList;
    permutation(pathDistanceList, pathSolutionList);

    int minPathIndex = 0;
    double minPathDist = DBL_MAX;
    for(int i = 0; i < pathDistanceList.size(); i++){
        double pathDist = 0;
        for(int j = 0; j < pathDistanceList[i].size(); j++){
            pathDist+=pathDistanceList[i][j];
            if(minPathDist > pathDist){
                minPathDist = pathDist;
                minPathIndex = i;
            }
        }
    }
    return pathSolutionList[minPathIndex];
}

void ShortestPath::permutation(vector<vector<double>>& pathDistanceList, vector<vector<ActionListPerObstacle>>& pathSolutionList){
    vector<int> goal_ids;
    for(int i = 0; i < obstacles.size(); i++){
        goal_ids.push_back(i);
    }
    sort(goal_ids.begin(), goal_ids.end());

    // for each permutation
    do{
        vector<ActionListPerObstacle> onePermuteSolution;
        vector<double> onePermuteSubDistance;
        SearchResult buffer;

        // initial state
        Vertex robotInitPosition(ROBOT_INIT_X_GRID, ROBOT_INIT_Y_GRID); 
        State* initState = new State(&robotInitPosition, 0, ROBOT_INIT_FACEDIRECTION, nullptr);

        // compute the Hamiltonian path
        for(int i = 0; i < goal_ids.size(); i++){
            initState = astar.search(*initState, obstacles[goal_ids[i]], &buffer.first, &buffer.second);
            onePermuteSolution.push_back(make_pair(obstacles[goal_ids[i]], buffer.second));
            onePermuteSubDistance.push_back(buffer.first);
        }
        // combine path solution from one obstacle to another into a single path solution
        for(int i = 0; i < onePermuteSolution.size(); i++){
            pathDistanceList.push_back(onePermuteSubDistance);
            pathSolutionList.push_back(onePermuteSolution);
        }
    // + 1 because first position is always initial point with id = 0
    }while(next_permutation(goal_ids.begin(), goal_ids.end())); 
}