#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// Data structures
typedef struct _obstacle{
    int id;
    double x_coor;
    double y_coor;
} Obstacle;

// Constant variables
double UNIT_LENGTH = 10;     // cm
double ROBOT_LENGTH = 30;
Obstacle start;
double* dist;


// Function prototypes
Obstacle* populate_obstacles();
Obstacle* shortest_path(Obstacle *obstacles, int n);
Obstacle* output_results(Obstacle *obstacles);
Obstacle* heuristic_greedy(Obstacle *obstacles, int n);
Obstacle* heuristic_greedy_enhanced(Obstacle *obstacles, int n);
Obstacle* heuristic_exhaustive(Obstacle *obstacles, int n);
Obstacle* heuristic_euclidean(Obstacle *obstacles, int n);
double calc_path(int* path, int n, double* dist);
double calc_euclidean(Obstacle o1, Obstacle o2);
void output_result(Obstacle* results, int n_obs);
void permutation(int* path_order, int l, int r, double* min_dist, int* min_path);
void swap(int *a, int *b);
void swap_obs(Obstacle *a, Obstacle *b);

// Input: to delete when STM defines IO input
Obstacle input[7] = {
    {1, 40, 30},
    {2, 45, 100},
    {3, 90, 90},
    {4, 180, 160},
    {5, 130, 70},
    {6, 115, 40},
    {7, 130, 30}
};

int main(){
    // initialise
    int n_obs = 0;
    Obstacle *results;
    start.id = 0; start.x_coor = ROBOT_LENGTH/2; start.y_coor = ROBOT_LENGTH/2;

    // start exploration procedure
    Obstacle *obstacles = populate_obstacles(&n_obs);
    results = shortest_path(obstacles, n_obs);
    output_result(results, n_obs);
    return 0;
}

// Done. Pls Change when STM defines IO standard to Algo team
void output_result(Obstacle* results, int n_obs){
    int i;
    for(i = 0; i < n_obs; i++){
        printf("%d ",results[i]);
    }
    printf("\n");
}

// Done. Pls Change when STM defines IO standard to Algo team
Obstacle* populate_obstacles(int *n_obs){    
    *n_obs = 7;
    return input;
}

// Done. Pls change when all heuristic functions are done
Obstacle* shortest_path(Obstacle *obstacles, int n){
    return heuristic_euclidean(obstacles, n);
}
Obstacle* heuristic_greedy(Obstacle *obstacles, int n){

}
Obstacle* heuristic_greedy_enhanced(Obstacle *obstacles, int n){

}
Obstacle* heuristic_exhaustive(Obstacle *obstacles, int n){

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
    //delete_s
    for(i = 0; i < n_locations; i++){
        for(j = 0; j < n_locations; j++){
            printf("%d|%.0f\t", i*n_locations + j, dist[i*n_locations + j]);
        }
        printf("\n");
    }
    //delete_e
    // calculate shortest cycle
    int* min_path = malloc(sizeof(int)*n);
    double min_dist = -1;
    int* order = malloc(sizeof(int)*n);
    for(i = 0; i < n; i++){
        order[i] = i+1;
    }
    permutation(order, 1, n, &min_dist, min_path);

    printf("Freed\n");
    free(dist);
    free(order);

    // Printing here for debugging purposes. Delete once function works
    printf("In heuristic_euclidean():\n");
    for(i = 0; i < n; i++){
        printf("%d ", min_path[i]);
    }
    putchar('\n');

    // returning Obstacle objects
    Obstacle* tmp = obstacles;
    int index = 0;
    while(tmp != NULL){
        for(j = index; j < n; j++){
            if(tmp->id == min_path[j]){
                swap_obs(tmp, obstacles + j);
                tmp+=1;
                index+=1;
                break;
            }
        }
    }
    return obstacles;
}

// Done
double calc_euclidean(Obstacle o1, Obstacle o2){
    return pow(pow(o1.x_coor - o2.x_coor, 2) + pow(o1.y_coor - o2.y_coor, 2), 0.5);
}

// Done
void permutation(int* path_order, int l, int r, double* min_dist, int* min_path){
    int i;
    double distance_buffer;
    if(l == r){
        for(i = 0; i < r; i++) printf("%d ", path_order[i]);        // delete
        if(calc_path(path_order, r+1, dist) < *min_dist){
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
        printf("%d-%d: %d\n", path[i], path[i+1], path[i]*n_location + path[i+1]);
        printf("%.2f\n", dist[path[i]*n_location + path[i+1]]);
        path_length+=dist[path[i]*n_location + path[i+1]];
    }

    // add the distance from START point to first target, and from last target back to START
    path_length = path_length + dist[path[n_location-1]*n_location + 0] + dist[0*n_location + path[0]];
    return path_length;
}

// Done
void swap(int *a, int *b){
    int *temp;
    temp = b;
    *a = *b;
    *b = *temp;
}

void swap_obs(Obstacle *a, Obstacle *b){
    Obstacle *temp;
    temp = b;
    *a = *b;
    *b = *temp;
}