#include "config.h"
#include "component.h"
#include "action.h"
#include "asearch.h"
#include "pathPlanning.h"
#include <vector>
#include <utility>
#include <algorithm>
#include <cmath>
#include <iostream>

using namespace std;



ShortestPath::ShortestPath(vector<Obstacle> obstacles): obstacles(obstacles){
    astar = new aStar(obstacles, 0);
    int nObstacles = obstacles.size();
    // shortestPathSolution.resize(nObstacles);
}

vector<ActionListPerObstacle> ShortestPath::hamiltonianPath(){
    vector<ActionListPerObstacle> resultPath;
    if(obstacles.size() <= 6) pathForObstacle_6(resultPath);
    else pathForObstacle_78(resultPath);
    return resultPath;
}

void ShortestPath::pathForObstacle_6(vector<ActionListPerObstacle>& resultPath){
    cout << "<= 6 Obstacles: Use Exhaustive A* Search" << endl;
    vector<vector<double>> pathDistanceList;
    vector<vector<ActionListPerObstacle>> pathSolutionList;
    try{
        permutation(pathDistanceList, pathSolutionList);
    }
    catch(...){
        try{
            cout << "Plan B: Allowing robot to move outside the border" << endl;
            astar = new aStar(obstacles, FROM_BORDER_GRID_LENGTH);
            permutation(pathDistanceList, pathSolutionList);
        }
        catch(...){
            cout << "Exhausted all plans. No Hamiltonian Path" << endl;
            resultPath = vector<ActionListPerObstacle>();    // if no Hamiltonian path, return an empty vector
            return;
        }
    }

    int minPathIndex = 0;
    double minPathDist = DBL_MAX;
    for(int i = 0; i < pathDistanceList.size(); i++){
        double pathDist = 0;
        for(int j = 0; j < pathDistanceList[i].size(); j++){
            pathDist+=pathDistanceList[i][j];
        }
        if(minPathDist > pathDist){
            minPathDist = pathDist;
            minPathIndex = i;
        }
    }
    cout << "Path found!" << endl;
    cout << "min path = " << minPathDist << endl;
    resultPath = pathSolutionList[minPathIndex];
}

void ShortestPath::pathForObstacle_78(vector<ActionListPerObstacle>& resultPath){
    cout << ">= 7 obstacles: Use Nearest Neighhbour heuristic" << endl;
    // initial state
    Vertex* robotInitPosition = new Vertex(ROBOT_INIT_X_GRID, ROBOT_INIT_Y_GRID);
    State* initState = new State(robotInitPosition, ROBOT_INIT_FACEDIRECTION, nullptr);

    int nObs = obstacles.size();
    vector<vector<float>> distanceMatrix(nObs + 1, vector<float>(nObs + 1));

    // 1. Calculate distance matrix
    // from robot to obstacle_i
    for(int i = 0; i < nObs; i++){
        distanceMatrix[0][i+1] = distanceMatrix[i+1][0] = calculateDist(robotInitPosition->xGrid, robotInitPosition->yGrid, initState->face_direction, obstacles[i].xGrid, obstacles[i].yGrid, obstacles[i].face_direction);
    }
    // from obstacle_i to obstacle_j
    for(int i = 0; i < nObs - 1; i++){
        for(int j = i+1; j < nObs; j++){
            distanceMatrix[i+1][j+1] = distanceMatrix[j+1][i+1] = calculateDist(obstacles[j].xGrid, obstacles[j].yGrid, obstacles[j].face_direction, obstacles[i].xGrid, obstacles[i].yGrid, obstacles[i].face_direction);
        }
    }

    // 2. Nearest Neighbour algorithm
    vector<bool> visited(nObs + 1);
    vector<int> goal_ids;
    float shortestDist = FLT_MAX;
    int shortestIndex = 0;
    int prevObstacleId;
    visited[0] = true;

    // find the first obstacle target
    for(int i = 1; i < nObs + 1; i++){
        if(shortestDist > distanceMatrix[0][i]){
            shortestIndex = i;
            shortestDist = distanceMatrix[0][i];
        }
    }
    visited[shortestIndex] = true;
    goal_ids.push_back(shortestIndex);
    prevObstacleId = shortestIndex;

    // find subsequent obstacle targets
    while(goal_ids.size() != nObs){
        shortestDist = FLT_MAX;
        shortestIndex = 0;
        for(int i = 1; i < nObs + 1; i++){
            if(!visited[i] && shortestDist > distanceMatrix[i][prevObstacleId]){
                shortestDist = distanceMatrix[i][prevObstacleId];
                shortestIndex = i;
            }
        }
        visited[shortestIndex] = true;
        goal_ids.push_back(shortestIndex);
        prevObstacleId = shortestIndex;
    }
    // normalise all goal_id in [1,n] to [0,n-1]
    for(int i = 0; i < goal_ids.size(); i++) goal_ids[i]-=1;

    // compute the Hamiltonian path
    vector<ActionListPerObstacle> result;
    try{
        SearchResult buffer;
        for(int i = 0; i < goal_ids.size(); i++){
            initState = astar->search(initState, obstacles[goal_ids[i]], &buffer.first, &buffer.second);
            result.push_back(make_pair(obstacles[goal_ids[i]], buffer.second));
            if(initState == nullptr){   // if no complete Hamiltonian path
                throw(initState);
            }
        }
    }
    catch(...){
        cout << "Plan B: Allowing robot to move outside the border" << endl;
        // reinitialise the robot's initial state as we are recalculating, not continuing the previous try block
        Vertex* robotInitPosition = new Vertex(ROBOT_INIT_X_GRID, ROBOT_INIT_Y_GRID);
        State* initState = new State(robotInitPosition, ROBOT_INIT_FACEDIRECTION, nullptr);

        astar = new aStar(obstacles, FROM_BORDER_GRID_LENGTH);
        result.clear();
        SearchResult buffer;
        for(int i = 0; i < goal_ids.size(); i++){
            initState = astar->search(initState, obstacles[goal_ids[i]], &buffer.first, &buffer.second);
            result.push_back(make_pair(obstacles[goal_ids[i]], buffer.second));
            if(initState == nullptr){   // if no complete Hamiltonian path
                cout << "Exhausted all plans. No Hamiltonian path" << endl;
                resultPath = vector<ActionListPerObstacle>();
                return;
            }
        }
    }
    cout << "Path found!" << endl;
    resultPath = result;
}

bool ShortestPath::permutation(vector<vector<double>>& pathDistanceList, vector<vector<ActionListPerObstacle>>& pathSolutionList){
    vector<int> goal_ids;
    for(int i = 0; i < obstacles.size(); i++){
        goal_ids.push_back(i);
    }
    sort(goal_ids.begin(), goal_ids.end());

    int iteration_debug = 0;
    int totalPermutation = 1;
    for(int i = 0; i < goal_ids.size(); i++) totalPermutation*=(i+1);
    // for each permutation
    do{
        vector<ActionListPerObstacle> onePermuteSolution;
        vector<double> onePermuteSubDistance;
        SearchResult buffer;
        cout << float(iteration_debug++)/totalPermutation*100 << "%" << endl;

        // initial state
        Vertex* robotInitPosition = new Vertex(ROBOT_INIT_X_GRID, ROBOT_INIT_Y_GRID);
        State* initState = new State(robotInitPosition, ROBOT_INIT_FACEDIRECTION, nullptr);

        // compute the Hamiltonian path
        for(int i = 0; i < goal_ids.size(); i++){
            initState = astar->search(initState, obstacles[goal_ids[i]], &buffer.first, &buffer.second);
            onePermuteSolution.push_back(make_pair(obstacles[goal_ids[i]], buffer.second));
            onePermuteSubDistance.push_back(buffer.first);
            if(initState == nullptr){   // if no complete Hamiltonian path
                throw(initState);
            }
        }

        pathDistanceList.push_back(onePermuteSubDistance);
        pathSolutionList.push_back(onePermuteSolution);
    // + 1 because first position is always initial point with id = 0
    }while(next_permutation(goal_ids.begin(), goal_ids.end()));
    return true;
}

float ShortestPath::calculateDist(int x1, int y1, int dir1, int x2, int y2, int dir2){
    return pow(pow(x2-x1, 2) + pow(y2-y1, 2), 0.5);
}