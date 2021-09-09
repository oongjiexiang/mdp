#include "config.h"
#include "component.h"
#include "asearch.h"
#include "hamiltonian.h"
#include <vector>
#include <utility>
#include <algorithm>



Hamiltonian::Hamiltonian(vector<vector<int>> fullMap, vector<Obstacle> obstacles): obstacles(obstacles){
    astar = aStar(fullMap, obstacles);
    int nObstacles = obstacles.size();
    shortestDist.resize(nObstacles + 1, vector<SearchResult>(nObstacles + 1));
}

// perform A* search between all obstacles and initial point and keep them for permutation later
void Hamiltonian::shortestPathMatrix(){
    // instead of saying from Obstacle A to B, we find from Vertex A to Obstacle B
    // hence, entire path involves: Vertex start to Obstacle A; Obstacle A's VERTEX to Obstacle B,...
    for(int i = 0; i < shortestDist.size(); i++){
        // A* for path from robot's initial point to its FIRST obstacle
        Vertex robotInitPosition(0, 0);
        Obstacle& firstObstacle = obstacles[i];
        astar.search(robotInitPosition, firstObstacle, shortestDist[0][i+1]);

        // A* for path between every pair of obstacles
        for(int j = i + 1; j <= shortestDist.size(); j++){
            Vertex source(obstacles[i].row, obstacles[i].column);
            Obstacle& dest = obstacles[j];
            astar.search(source, dest, shortestDist[i+1][j+1]);   
            
        }
    }
}

void Hamiltonian::hamiltonianPath(){
    vector<pathDist> paths;
    permutation(paths);
    vector<SearchResult> fullPath;
    double minTotalDist = DBL_MAX;
    for(int i = 0; i < paths.size(); i++){
        if(minTotalDist > paths[i].first){
            minTotalDist = paths[i].first;
                // now paths[i].second.j represents obstacle id -- the sequence of obstacles to visit
            vector<int> obsSeqToVisit = paths[i].second;
            for(int j = 0; j < obsSeqToVisit.size() - 1; j++){   
                fullPath.insert(fullPath.end(), shortestDist.begin(), shortestDist.end());
            }
            return;
        }
    }
}

void Hamiltonian::permutation(vector<pathDist>& paths){
    vector<int> goal_ids;
    for(int i = 0; i <= obstacles.size(); i++){
        goal_ids.push_back(i);
    }
    sort(goal_ids.begin(), goal_ids.end());
    do{
        double totalDist = 0;
        for(int i = 0; i < goal_ids.size() - 1; i++){
            totalDist+=shortestDist[goal_ids[i]][goal_ids[i+1]].first;
            paths.push_back(pathDist(totalDist, goal_ids));
        }
    // + 1 because first position is always initial point with id = 0
    }while(next_permutation(goal_ids.begin() + 1, goal_ids.end())); 
}