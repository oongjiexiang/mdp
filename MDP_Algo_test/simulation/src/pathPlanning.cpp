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
    if(!permutation(pathDistanceList, pathSolutionList)) return vector<ActionListPerObstacle>();    // if no Hamiltonian path, return an empty vector
    
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
    cout << "min path = " << minPathDist << endl;
    for(int i = 0; i < pathSolutionList[minPathIndex].size(); i++){
        ActionListPerObstacle alpo = pathSolutionList[minPathIndex][i];
        Obstacle o = alpo.first;
        vector<State*> states = alpo.second;
        cout << "going to obstacle *****************" << endl;
        o.printObstacle();
        for(int j = 0; j < states.size(); j++){
            states[j]->printState();
        }
        cout << "obstacle is visited*****************" << endl;
        cout << endl;
    }
    return pathSolutionList[minPathIndex];
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
        cout << float(iteration_debug++)/totalPermutation << "%" << endl;
        // initial state
        Vertex* robotInitPosition = new Vertex(ROBOT_INIT_X_GRID, ROBOT_INIT_Y_GRID); 
        State* initState = new State(robotInitPosition, ROBOT_INIT_FACEDIRECTION, nullptr);

        // compute the Hamiltonian path
        for(int i = 0; i < goal_ids.size(); i++){
            initState = astar.search(initState, obstacles[goal_ids[i]], &buffer.first, &buffer.second);
            onePermuteSolution.push_back(make_pair(obstacles[goal_ids[i]], buffer.second));
            onePermuteSubDistance.push_back(buffer.first);
            if(initState == nullptr){   // if no complete Hamiltonian path
                return false;
            }
        }
        
        pathDistanceList.push_back(onePermuteSubDistance);
        pathSolutionList.push_back(onePermuteSolution);
        // }
    // + 1 because first position is always initial point with id = 0
    }while(next_permutation(goal_ids.begin(), goal_ids.end())); 
    return true;
}