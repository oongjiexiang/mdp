#include <stdio.h>
#include <math.h>

// Constant variables
double UNIT_LENGTH = 10;     // cm
double ROBOT_LENGTH = 30;

// Data structures
typedef struct _obstacle{
    int id;
    float x_coor;
    float y_coor;
} Obstacle;

// Function prototypes
Obstacle* populate_obstacles();
Obstacle* shortest_path(Obstacle *obstacles, int n);
Obstacle* output_results(Obstacle *obstacles);
Obstacle* heuristic_greedy(Obstacle *obstacles, int n);
Obstacle* heuristic_greedy_enhanced(Obstacle *obstacles, int n);
Obstacle* heuristic_exhaustive(Obstacle *obstacles, int n);
Obstacle* heuristic_euclidean(Obstacle *obstacles, int n);
double calc_euclidean(Obstacle o1, Obstacle o2);
int* permutation(int* order, int l, int r);
void swap(int *a, int *b);

int main(){
    // initialise
    int n_obs = 0;
    Obstacle start = { 0, ROBOT_LENGTH/2, ROBOT_LENGTH/2};

    // start exploration procedure
    Obstacle *obstacles = populate_obstacles(&n_obs);
    Obstacle *results = shortest_path(obstacles, n_obs);
    output_result(results);
    return 0;
}

Obstacle* populate_obstacles(int *n_obs){
    // malloc
}
Obstacle* shortest_path(Obstacle *obstacles, int n){
    
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
    float* dist = (float*)malloc(sizeof(float)*n_locations*n_locations);
    for(i = 0; i < n_locations; i++){
        for(j = 0; j < n_locations; j++){
            dist[obstacles[i].id*n_locations + obstacles[j].id] = calc_euclidean(obstacles[i], obstacles[j]);
        }
    }

    // calculate shortest cycle
    int* path = malloc(sizeof(int));     // stores the final path


    free(dist);
}

// Done
double calc_euclidean(Obstacle o1, Obstacle o2){
    return pow(pow(o1.x_coor - o2.x_coor, 2) + pow(o1.y_coor - o2.y_coor, 2), 0.5);
}

int* permutation(int* order, int l, int r){
    int i;
    if(l == r){
        return order;
    }
    for(i = l; i <= r; i++){
        swap(order + i, order + l);
        permutation(order, l+1, r);
        swap(order + i, order + l);
    }
}

void swap(int *a, int *b){
    int *temp;
    temp = b;
    *a = *b;
    *b = *temp;
}