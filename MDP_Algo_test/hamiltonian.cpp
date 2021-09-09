#include "config.h"
#include "component.h"
#include "asearch.h"
#include "hamiltonian.h"
#include <vector>
#include <utility>



// main class for week 8 and simulation for Hamiltonian path
class Hamiltonian{
    aStar astar;
    vector<vector<SearchResult>> shortestDist;
    vector<Obstacle> obstacles;
    Hamiltonian(vector<vector<int>> fullMap, vector<Obstacle> obstacles): obstacles(obstacles){
        astar = aStar(fullMap, obstacles);
        int nObstacles = obstacles.size();
        shortestDist.resize(nObstacles + 1, vector<SearchResult>(nObstacles + 1));
    }
    void shortestPath(){
        for(int i = 0; i < shortestDist.size(); i++){
            for(int j = i + 1; j <= shortestDist.size(); j++){
                obstacles[i].row;
                astar.search(, , shortestDist[i][j]);   // instead of saying from Obstacle A to B
                // I currently find the obstacles' positions and return the vertices
            }
        }
    }
    Obstacle* heuristic_euclidean(Obstacle *obstacles, int n){
        int i, j;

        // calculate distance adjacency matrix
        int n_locations = n+1;
        dist = (double*)malloc(sizeof(double)*n_locations*n_locations);
        for(i = 0; i < n_locations; i++){
            dist[obstacles[i].id*n_locations + start.id] = calc_euclidean(obstacles[i], start);
            dist[start.id*n_locations + obstacles[i].id] = dist[obstacles[i].id*n_locations + start.id];
            for(j = 0; j < n_locations; j++){
                dist[obstacles[i].id*n_locations + obstacles[j].id] = calc_euclidean(obstacles[i], obstacles[j]);
                dist[obstacles[j].id*n_locations + obstacles[i].id] = dist[obstacles[i].id*n_locations + obstacles[j].id];
            }
        }
        // calculate shortest cycle
        int* min_path = malloc(sizeof(int)*n);
        double min_dist = FLT_MAX;
        int* order = malloc(sizeof(int)*n);
        for(i = 0; i < n; i++){
            order[i] = i+1;
        }
        permutation(order, 1, n, &min_dist, min_path);

        free(dist);
        free(order);

        int index;
        for(index = 0; index < n - 1; index++){
            for(j = index; j < n; j++){
                if((obstacles+j)->id == min_path[index]){
                    swap_obs(obstacles + index, obstacles + j);
                    break;
                }
            }
        }
        return obstacles;
    }

    // Done
    void permutation(int* path_order, int l, int r, double* min_dist, int* min_path){
        int i;
        double distance_buffer;
        if(l == r){
            distance_buffer = calc_path(path_order, r, dist);
            if(distance_buffer < *min_dist){
                for(i = 0; i < r; i++){
                    min_path[i] = path_order[i];
                }
                *min_dist = distance_buffer;
            }
            return;
        }
        for(i = l; i <= r; i++){
            swap(path_order + i - 1, path_order + l - 1);
            permutation(path_order, l+1, r, min_dist, min_path);
            swap(path_order + i - 1, path_order + l - 1);
        }
    }

    // Assumption: 5-8 targets, never less than 3
    double calc_path(int* path, int n_location, double* dist){
        int i;
        double path_length = 0;
        for(i = 0; i < n_location - 1; i++){
            path_length+=dist[path[i]*n_location + path[i+1]];
        }

        // add the distance from START point to first target, and from last target back to START
        path_length = path_length + dist[path[n_location-1]*n_location + 0] + dist[0*n_location + path[0]];
        return path_length;
    }

    void swap(int *a, int *b){
        int temp;
        temp = *a;
        *a = *b;
        *b = temp;
    }

    void swap_obs(Obstacle *a, Obstacle *b){
        Obstacle temp = *a;
        *a = *b;
        *b = temp;
    }
};