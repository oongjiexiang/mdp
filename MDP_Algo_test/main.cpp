#include <stdio.h>
#include <iostream>
#include <vector>
#include <stdexcept>
#include "config.h"
#include "tripPlan.h"

using namespace std;
//----constants----
//double UNIT_LENGTH = 10;     // cm
//double ROBOT_LENGTH = 30;
//double OBSTACLE_BOUNDARY = 15; //how to do this one?
int row_count,column_count,total_grid_count;
double unit_length, obstacle_size,robot_length,obstacle_boundary;

/*
    theta refers to the direction
    theta: between 0 to 360, or -pi to pi
    direction:  0:   forward
                90:  left
                180: back
                270: right
                359: forward
*/

//trip planning functions
//----done----
double calculate_g_cost(Vertex* prev_vertex, Vertex* current_vertex);
void update_h_cost(vector<vector<Vertex>> vertex_array, Vertex* src_vertex, Vertex* target_vertex, int total_grid_count);//caculates the hcost, gcost+heuristic value, distance from source and estimated distance from target, distance from source
void print_path(Vertex* src_vertex, Vertex* target_vertex);

//----not done----
Vertex* a_star_search(vector<vector<Vertex>> vertex_array, vector<vector<Vertex>> adj_matrix, Vertex* src_vertex, Vertex* target_vertex, int total_grid_count);
double* trip_planning(vector<Vertex> obstacle_array, vector<vector<Vertex>> vertex_array, Robot* robot);//a*search with obstacle avoidance, returns the path and direction(theta)
bool obstacle_avoidance(vector<Vertex> obstacle_array, Robot* robot);//obstacle avoidance checking and redirection, returns true if obstacle found, false if no obstacle
int check_turn_condition(Vertex* start_loc, Vertex* end_loc);//calculates which of the 3 scenarios the source and vertex falls under and returns 1, 2, or 3
void update_robot(Robot* robot);//update the robot's coordinates
//----vertex functions----
void initialize_vertex(vector<vector<Vertex>> vertex_array,int row_count, int column_count,int unit_length,int total_count);//initialize all the vertex variables
void reset_vertex(vector<vector<Vertex>> vertex_array,int row_count, int column_count,int unit_length,int total_count);//reset the cost, obstacle, border, visited, and prev_vertex for all vertices, use this every time after 1 path has been found
void generate_adj_matrix(vector<vector<Vertex>> adj_matrix, vector<vector<Vertex>> vertex_array, int row_count, int column_count, int total_count);
Vertex find_vertex(vector<vector<Vertex>> vertex_array, double x_coor, double y_coor, int total_count);//assumes the x and y coordinates given refer to the middle of the object
void add_obstacle(vector<vector<Vertex>> vertex_array, vector<Obstacle> obstacle_array, double obstacle_length,double boundary_size, int total_count);//add obstacles and borders

//----test functions----
void test_queue();

int main()
{
    unit_length=10;//set to 5 so that obstacle boundary can be calculated more easily
    robot_length=30;
    obstacle_boundary=15;// + to each side of the obstacle
    obstacle_size=10;//total length of an obstacle, the obstacle is a square
    row_count=20;
    column_count=20;
    total_grid_count = row_count*column_count;
    // src=0;
    // target=13;
    //----test vertex fucntions----
    vector<vector<Vertex>> vertex_array(row_count, vector<Vertex>(column_count));
    initialize_vertex(vertex_array,row_count,column_count,unit_length,total_grid_count);
    for(int i = 0; i < row_count; i++){
        for(int j = 0; j < column_count; j++){
            cout << i*row_count + column_count << endl;
            printf("vertex values: %d, %d, %d, %f, %f, %f, %f\n",vertex_array[i][j].id,vertex_array[i][j].g_cost,vertex_array[i][j].h_cost,vertex_array[i][j].x_left,vertex_array[i][j].x_right,vertex_array[i][j].y_high,vertex_array[i][j].y_low);
        }
    }
    vector<vector<Vertex>> adj_matrix(total_grid_count, vector<Vertex>(total_grid_count));
    vector<Obstacle> obstacle_array = {Obstacle(0,50 ,10)};
    add_obstacle(vertex_array, obstacle_array, obstacle_size,obstacle_boundary,total_grid_count);

    // printf("index %d, obstacle = %d\n",vertex_array[25].id,vertex_array[25].is_obstacle);
    generate_adj_matrix(adj_matrix,vertex_array,row_count,column_count,total_grid_count);

    // test vertices. I didn't use src and target
    Vertex source = vertex_array[5][5];
    Vertex target = vertex_array[20][10];

    update_h_cost(vertex_array, &source, &target, total_grid_count);
    a_star_search(vertex_array,adj_matrix, &source, &target,total_grid_count);

    //----end of test vertex----

    //test_queue();
    return 0;
}

//----Search functions
//add obstacle avoidance and stop search once the goal has been found
Vertex* a_star_search(vector<Vertex> vertex_array, vector<vector<Vertex>> adj_matrix, Vertex* src_vertex, Vertex* target_vertex, int total_grid_count){
    int i, temp_g_cost;
    Vertex* curr_vertex;
    Vertex* neighbour_vertex;
    SortedQueue q;
    src_vertex->g_cost=0;
    q.enqueue(src_vertex);
    curr_vertex=src_vertex;
    //while q to explore is not empty
    while(q.head!=NULL){
        //visit head of sorted q
        curr_vertex = q.dequeue();
        curr_vertex->visited=true;
        if(curr_vertex->id == target_vertex->id){
            print_path(src_vertex,target_vertex);
            return target_vertex;
        }
        //for each neighbour in the adjacency matrix
        for(i=0;i<total_grid_count;i++){
            if(adj_matrix[curr_vertex->id][i] != NULL){
                neighbour_vertex = &adj_matrix[curr_vertex->id][i];
                if(neighbour_vertex->visited == false && neighbour_vertex->is_border==false && neighbour_vertex->is_obstacle==false ){
                    //compare g costs of neighbour, if new g_cost is lower, replace the old g_cost and prev_vertex
                    temp_g_cost=calculate_g_cost(curr_vertex,neighbour_vertex);
                    if(temp_g_cost < neighbour_vertex->g_cost){
                        neighbour_vertex->g_cost = temp_g_cost;
                        neighbour_vertex->prev_vertex = curr_vertex;
                        //add neighbour to exploration queue
                        q.enqueue(neighbour_vertex);
                    }
                }
            }
        }
        //move the lowest f_cost vertex to front of q
        q.sort_queue();
    }
    print_path(src_vertex,target_vertex);
    return target_vertex;
}
//prints the path from a star search backwards, to implement forward, save the id into an array starting from the last element or use a stack
void print_path(Vertex* src_vertex, Vertex* target_vertex){
    Vertex* print_vertex;
    print_vertex = target_vertex;
    printf("path to reach vertex %d from source %d\n",target_vertex->id, src_vertex->id);
    while(print_vertex->id != src_vertex->id){
        printf("Vertex: %d\n",print_vertex->prev_vertex->id);
        printf("x_left: %.2f, y_low: %.2f\n",print_vertex->prev_vertex->x_left,print_vertex->prev_vertex->y_low);
        print_vertex=print_vertex->prev_vertex;
    }
}
//updates the h_cost for every vertex using Euclidean distance from previous vertex
double calculate_g_cost(Vertex* prev_vertex, Vertex* current_vertex){
    int x1, x2, y1, y2;
    double x1 = prev_vertex->x_left;
    double y1 = prev_vertex->y_low;
    double x2 = current_vertex->x_left;
    double y2 = current_vertex->y_low;
    return sqrt((x2-x1)*(x2-x1) + (y2-y1)*(y2-y1))+prev_vertex->g_cost;
}
//updates the h_cost for every vertex using Euclidean distance from target
void update_h_cost(vector<vector<Vertex>> vertex_array, Vertex* src_vertex, Vertex* target_vertex, int total_grid_count){
    src_vertex->h_cost=0;
    double x2 = target_vertex->x_left;
    double y2 = target_vertex->y_low;
    for(int i = 0;i < row_count; i++){
        for(int j = 0; j < column_count; j++){
            double x1 = vertex_array[i][j].x_left;
            double y1 = vertex_array[i][j].y_low;
            vertex_array[i][j].h_cost= sqrt((x2-x1)*(x2-x1) + (y2-y1) * (y2-y1));
        }
    }
}
//----Vertex functions----
//initialize all the variables for all vertices of the graph
void initialize_vertex(vector<vector<Vertex>> vertex_array,int row_count, int column_count,int unit_length,int total_count){
    int start_cost = INT_MAX;
    for(int i=0; i < row_count; i++){
        for(int j=0; j < column_count; j++){
            vertex_array[i][j].id = i*column_count + j;
            vertex_array[i][j].g_cost = start_cost;
            vertex_array[i][j].h_cost = start_cost;
            vertex_array[i][j].x_right =((j+1)*unit_length)-1;  // JX: why -1?
            vertex_array[i][j].x_left =j*unit_length;
            vertex_array[i][j].y_high =((i+1)*unit_length)-1;   // JX: why -1?
            vertex_array[i][j].y_low =i*unit_length;
            vertex_array[i][j].is_obstacle = false;
            vertex_array[i][j].is_border = false;
            vertex_array[i][j].visited = false;
            vertex_array[i][j].prev_vertex = NULL;
        }
    }
}
//reset the cost, obstacle, border, visited, and prev_vertex for all vertices, use this every time after 1 path has been found
void reset_vertex(vector<vector<Vertex>> vertex_array,int row_count, int column_count,int unit_length,int total_count){
    int i,j,start_cost,current_index;
    int start_cost = INT_MAX;
    for(int i=0; i < row_count;i++){
        for(int j = 0; j < column_count;j++){
            vertex_array[i][j].g_cost = start_cost;
            vertex_array[i][j].h_cost = start_cost;
            vertex_array[i][j].is_obstacle = false;
            vertex_array[i][j].is_border = false;
            vertex_array[i][j].visited = false;
            vertex_array[i][j].prev_vertex = NULL;
        }
    }
}
//generates the grid structure in the adjacency matrix
void generate_adj_matrix(vector<vector<Vertex>> adj_matrix, vector<Vertex> vertex_array, int row_count, int column_count, int total_count){
    int i,j, mod_result;
    //          top bot left right topleft topright botleft botright
    //offset x  0   0   -1      1   -1      1       -1      1
    //offset y  c   -c  0       0   c       c       -c      -c

    //set all as NULL for now
    for(i=0;i<total_count;i++){ //row
        for(j=0;j<total_count;j++){ //col
            adj_matrix[i][j] = NULL;
        }
    }
    for(i=0;i<total_count;i++){ //row
            adj_matrix[i][j] = NULL;
            mod_result = (i+1)%column_count;
            //if vertex not in the top row && not in the bot row && not the last column && not the first column
            if(i<(total_count-row_count) && i>row_count && mod_result!=0 && mod_result!=1){  // JX: why mod_result!=1 means 'not in first column'?
                adj_matrix[i][i+1] = vertex_array[i+1]; // right     // JX: why use '&'?
                adj_matrix[i][i-1] = vertex_array[i-1]; // left
                adj_matrix[i][i+column_count] = vertex_array[i+column_count]; // top
                adj_matrix[i][i-column_count] = vertex_array[i-column_count]; // bot
                adj_matrix[i][i+1+column_count] = vertex_array[i+1+column_count]; // top right
                adj_matrix[i][i+1-column_count] = vertex_array[i+1-column_count]; // bot right
                adj_matrix[i][i-1+column_count] = vertex_array[i-1+column_count]; // top left
                adj_matrix[i][i-1-column_count] = vertex_array[i-1-column_count]; // bot left
            }
            //if vertex in the first column and first row
            else if(mod_result==1 && i<row_count){
                adj_matrix[i][i+1] = vertex_array[i+1]; //right
                adj_matrix[i][i+column_count] = vertex_array[i+column_count]; //top
                adj_matrix[i][i+1+column_count] = vertex_array[i+1+column_count]; //top right
            }
            //if vertex in first column last row
            else if(mod_result==1 && i>=(total_count-row_count)){
                adj_matrix[i][i+1] = vertex_array[i+1];// right
                adj_matrix[i][i-column_count] = vertex_array[i-column_count]; // bot
                adj_matrix[i][i+1-column_count] = vertex_array[i+1-column_count]; // bot right
            }
            //if vertex in last column first row
            else if(mod_result==0 && i<row_count){
                adj_matrix[i][i-1] = vertex_array[i-1]; // left
                adj_matrix[i][i+column_count] = vertex_array[i+column_count]; // top
                adj_matrix[i][i-1+column_count]=&vertex_array[i-1+column_count]; // top left
            }
            //if vertex in last column last row
            else if(mod_result==0 && i>=(total_count-row_count)){
                adj_matrix[i][i-1] = vertex_array[i-1]; // left
                adj_matrix[i][i-column_count] = vertex_array[i-column_count]; // bot
                adj_matrix[i][i-1-column_count] = vertex_array[i-1-column_count]; // bot left
            }
            //first column
            else if(mod_result==1){
                adj_matrix[i][i+1] = vertex_array[i+1]; //right
                adj_matrix[i][i+column_count] = vertex_array[i+column_count]; // top
                adj_matrix[i][i-column_count] = vertex_array[i-column_count]; // bot
                adj_matrix[i][i+1+column_count] = vertex_array[i+1+column_count]; //top right
                adj_matrix[i][i+1-column_count] = vertex_array[i+1-column_count]; // bot right
            }
            //last column
            else if(mod_result==0){
                adj_matrix[i][i-1] = vertex_array[i-1]; // left
                adj_matrix[i][i+column_count] = vertex_array[i+column_count]; // top
                adj_matrix[i][i-column_count] = vertex_array[i-column_count]; // bot
                adj_matrix[i][i-1+column_count] = vertex_array[i-1+column_count]; // top left
                adj_matrix[i][i-1-column_count] = vertex_array[i-1-column_count]; // bot left
            }
            //first row
            else if(i<row_count){
                adj_matrix[i][i+1] = vertex_array[i+1]; //right
                adj_matrix[i][i-1] = vertex_array[i-1]; // left
                adj_matrix[i][i+column_count] = vertex_array[i+column_count]; // top
                adj_matrix[i][i+1+column_count] = vertex_array[i+1+column_count]; //top right
                adj_matrix[i][i-1+column_count] = vertex_array[i-1+column_count]; // top left
            }
            //last row
            else if(i>=(total_count-row_count)){
                adj_matrix[i][i+1] = vertex_array[i+1]; //right
                adj_matrix[i][i-1] = vertex_array[i-1]; // left
                adj_matrix[i][i-column_count] = vertex_array[i-column_count]; // bot
                adj_matrix[i][i+1-column_count] = vertex_array[i+1-column_count]; // bot right
                adj_matrix[i][i-1-column_count] = vertex_array[i-1-column_count]; // bot left
        }
    }
}

//adds an obstacles and the borders of the obstacles into the grid given the array of obstacles
void add_obstacle(vector<vector<Vertex>> vertex_array, vector<Obstacle> obstacle_array, double obstacle_length,double boundary_size, int total_count){
    int i,j;
    double obstacle_x_left, obstacle_x_right, obstacle_y_high, obstacle_y_low, border_x_left, border_x_right, border_y_high, border_y_low;
    obstacle_length=obstacle_length/2;      // JX: var here means half of obstacle length to find the left, right, high, low coordinates (for loop below)
    boundary_size= boundary_size*2;         // JX: why *2 but not /2? 

    //loop through the obstacle array
    for(i = 0; i < obstacle_array.size(); i++){
        //find the 4 points of the obstacle
        obstacle_x_left = obstacle_array[i].x_coor - obstacle_length;
        obstacle_x_right = obstacle_array[i].x_coor + obstacle_length;
        obstacle_y_high = obstacle_array[i].y_coor + obstacle_length;
        obstacle_y_low = obstacle_array[i].y_coor - obstacle_length;
        printf("obs x and y : %.1f, %.1f, %.1f, %.1f",obstacle_x_left,obstacle_x_right,obstacle_y_high,obstacle_y_low);

        //find the 4 points of the border
        border_x_left = obstacle_array[i].x_coor - boundary_size;
        border_x_right = obstacle_array[i].x_coor + boundary_size;
        border_y_high = obstacle_array[i].y_coor + boundary_size;
        border_y_low = obstacle_array[i].y_coor - boundary_size;

        //check every vertex
        for(i = 0; i < row_count; i++){
            for(j = 0; j < column_count; j++){
                // check if a vertex overlaps with an obstacle, if it does set it as an obstacle
                // x1-------x2 x1<=x4, x2>=x3
                // x3-------x4
                if(vertex_array[i][j].x_left <= obstacle_x_right && vertex_array[i][j].x_right >=obstacle_x_left && vertex_array[i][j].y_low <= obstacle_y_high && vertex_array[i][j].y_high >=obstacle_y_low){
                    vertex_array[i][j].is_obstacle = true;
                    //printf("obstacle: %d\n",j);
                }
                //if a vertex falls within the x and y coordinates of the border and the vertex is not an obstacle, set it as the border
                // JX: so if a point is nicely on the obstacle, then it is_obstacle and it is_border?
                if(vertex_array[i][j].x_left <= border_x_right && vertex_array[i][j].x_right >=border_x_left && vertex_array[i][j].y_low <= border_y_high && vertex_array[i][j].y_high >=border_y_low && vertex_array[i][j].is_obstacle==false){
                    vertex_array[i][j].is_border=true;
                    //printf("border: %d\n",j);
                }
            }
        }
    }
}

//search for a vertex given the x and y coordinates and returns a pointer to the vertex
Vertex find_vertex(vector<vector<Vertex>> vertex_array, double x_coor, double y_coor, int total_count){
    // JX: Zhi Yu please check. This uses mathematical calculation instead to use O(1) instead of O(n^2). Note that I didn't modify this function param
    //     to get the unit_length and column_count. I'm just using the global variables
    int row = ceil(y_coor/ unit_length);
    int col = ceil(x_coor/unit_length);
    try{
        return vertex_array.at(row).at(col);
    }
    catch(const out_of_range &oor){
        printf("vertex could not be found.\n");
    }
};
//----test functions----
void test_queue(){
    int i;
    vector<Vertex> qv = {
    { 0,10},{ 1,15},{ 2,5},{ 3,25},{ 4,20}
    };
    Vertex* qptr;
    SortedQueue q;
    for(i=0;i<5;i++){
        q.enqueue(&qv[i]);
    }
    QueueNode* curr = q.head;
    for(i = 0; i < q.current_size;i++){
        printf("%d ",curr->q_vertex->g_cost);
        curr=curr->next;
    }
    printf("\n");
    q.sort_queue();
    curr=q.head;
    for(i = 0; i < q.current_size; i++){
        printf("%d ",curr->q_vertex->g_cost);
        curr=curr->next;
    }
}
