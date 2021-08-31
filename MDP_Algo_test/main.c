#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <float.h>

//----constants----
//double UNIT_LENGTH = 10;     // cm
//double ROBOT_LENGTH = 30;
//double OBSTACLE_BOUNDARY = 15; //how to do this one?
int unit_length,robot_length,obstacle_boundary,row_count,column_count,total_grid_count;


/*
    theta refers to the direction
    theta: between 0 to 360, or -pi to pi
    direction:  0:   forward
                90:  left
                180: back
                270: right
                359: forward
*/
//----struct----
//used for obstacle avoidance
struct Vertex{
    int id;
    int g_cost; //distance from source to current vertex
    int h_cost;
    double x_right;
    double x_left;
    double y_high;
    double y_low;
    bool is_obstacle;
    bool is_border;
    bool visited;
    struct Vertex* prev_vertex; //reset each time
};
//used to read the obstacles
struct Obstacle{
    int id;
    double x_coor;
    double y_coor;
};
struct Goal{
    double x_right;
    double x_left;
    double y_high;
    double y_low;
    double theta;
};
//used to track the robot's simulated location
struct Robot{
    double x_right;
    double x_left;
    double y_high;
    double y_low;
    double theta;
};
//used in the search
struct Sorted_Queue{
    struct Queue_Node* head;
    struct Queue_Node* tail;
    int current_size;
};
//leave as int for now, once the sort function works, change int id to struct Vertex* vertex -> hcost+gcost
struct Queue_Node{
    struct Vertex* q_vertex;
    struct Queue_Node* next;
};

//trip planning functions
//----done----
double calculate_g_cost(struct Vertex* prev_vertex, struct Vertex* current_vertex);
void update_h_cost(struct Vertex* vertex_array, struct Vertex* src_vertex, struct Vertex* target_vertex, int total_grid_count);//caculates the hcost, gcost+heuristic value, distance from source and estimated distance from target, distance from source
//----not done----
double* trip_planning(struct Vertex* obstacle_array, struct Vertex* vertex_array, struct Robot* robot);//a*search with obstacle avoidance, returns the path and direction(theta)
bool obstacle_avoidance(struct Vertex* obstacle_array, struct Robot* robot);//obstacle avoidance checking and redirection, returns true if obstacle found, false if no obstacle
int check_turn_condition(struct Vertex* start_loc, struct Vertex* end_loc);//calculates which of the 3 scenarios the source and vertex falls under and returns 1, 2, or 3
struct Vertex* a_star_search(struct Vertex* vertex_array, struct Vertex* adj_matrix[total_grid_count][total_grid_count], struct Vertex* src_vertex, struct Vertex* target_vertex, int total_grid_count);
void update_robot(struct Robot* robot);//update the robot's coordinates

//----vertex functions----
//----done----
void initialize_vertex(struct Vertex* vertex_array,int row_count, int column_count,int unit_length,int total_count);//initialize all the vertex variables
void reset_vertex(struct Vertex* vertex_array,int row_count, int column_count,int unit_length,int total_count);//reset the cost, obstacle, border, visited, and prev_vertex for all vertices, use this every time after 1 path has been found
void generate_adj_matrix(struct Vertex* adj_matrix[total_grid_count][total_grid_count],struct Vertex* vertex_array, int row_count, int column_count, int total_count);
//----not done----
void add_obstacle(struct Vertex* vertex_array, struct Obstacle* obstacle_array, int no_of_obstacles, int unit_length,int border_length, int total_count);//add obstacles and borders

//----queue functions----
void create_empty_queue(struct Sorted_Queue* q);
void enqueue(struct Sorted_Queue* q, struct Vertex* vertex);
struct Vertex* dequeue(struct Sorted_Queue* q);
void sort_queue(struct Sorted_Queue* q);

int main()
{
    int i, src,target;
    unit_length=10;//set to 5 so that obstacle boundary can be calculated more easily
    robot_length=30;
    obstacle_boundary=15;// how to check for halfgrid? make each grid 5 wide?
    row_count=5;
    column_count=5;
    total_grid_count = row_count*column_count;
    src=0;
    target=9;
    //----test vertex fucntions----
    struct Vertex* vertex_array= (struct Vertex*)malloc(total_grid_count*sizeof(struct Vertex));
    initialize_vertex(vertex_array,row_count,column_count,unit_length,total_grid_count);
    for(i=0;i<5;i++){
            printf("%d\n",i);
        printf("vertex values: %d, %d, %d, %f, %f, %f, %f\n",vertex_array[i].id,vertex_array[i].g_cost,vertex_array[i].h_cost,vertex_array[i].x_left,vertex_array[i].x_right,vertex_array[i].y_high,vertex_array[i].y_low);
    }

    struct Vertex* adj_matrix[total_grid_count][total_grid_count];

    generate_adj_matrix(adj_matrix,vertex_array,row_count,column_count,total_grid_count);
    update_h_cost(vertex_array,&vertex_array[src],&vertex_array[target],total_grid_count);
    struct Vertex* vptr = a_star_search(vertex_array,adj_matrix,&vertex_array[src],&vertex_array[target],total_grid_count);

    for(i=0;i<25;i++){
        if(vertex_array[i].prev_vertex!=NULL)
        printf("i: %d prev_vertex: %d\n",i,vertex_array[i].prev_vertex->id);
    }

    //----end of test vertex----

    /*
    //---test queue functions---
    struct Vertex qv[5]={
    { 0,10},{ 1,15},{ 2,5},{ 3,25},{ 4,20}
    };
    struct Vertex* qptr=(struct Vertex*)malloc(sizeof(struct Vertex*));
    struct Sorted_Queue* q = (struct Sorted_Queue*)malloc(sizeof(struct Sorted_Queue));
    create_empty_queue(q);
    for(i=0;i<5;i++){
        qptr=&qv[i];
        enqueue(q,qptr);
    }
    struct Queue_Node* curr=q->head;
    for(i=0;i<q->current_size;i++){
        printf("%d ",curr->q_vertex->g_cost);
        curr=curr->next;
    }
    printf("\n");
    sort_queue(q);
    curr=q->head;
    for(i=0;i<q->current_size;i++){
        printf("%d ",curr->q_vertex->g_cost);
        curr=curr->next;
    }
    free(q);
    //----end of test queue----
    */
    free(vertex_array);

    return 0;
}

//----Search functions
struct Vertex* a_star_search(struct Vertex* vertex_array, struct Vertex* adj_matrix[total_grid_count][total_grid_count], struct Vertex* src_vertex, struct Vertex* target_vertex, int total_grid_count){
    int i, temp_g_cost;
    struct Vertex* curr_vertex;
    struct Vertex* print_vertex;
    struct Vertex* neighbour_vertex;
    struct Sorted_Queue* q = (struct Sorted_Queue*)malloc(sizeof(struct Sorted_Queue*));
    create_empty_queue(q);
    src_vertex->g_cost=0;
    enqueue(q,src_vertex);
    curr_vertex=src_vertex;

    while(q->head!=NULL){
        curr_vertex=dequeue(q);
        curr_vertex->visited=true;
        for(i=0;i<total_grid_count;i++){
            if(adj_matrix[curr_vertex->id][i]!=NULL){
                neighbour_vertex=adj_matrix[curr_vertex->id][i];
                if(neighbour_vertex->visited==false){
                    temp_g_cost=calculate_g_cost(curr_vertex,neighbour_vertex);
                    if(temp_g_cost<neighbour_vertex->g_cost){
                        neighbour_vertex->g_cost=temp_g_cost;
                        neighbour_vertex->prev_vertex=curr_vertex;
                        enqueue(q,neighbour_vertex);
                    }
                }
            }
        }
        sort_queue(q);
    }
    //print the path (backwards)
    print_vertex=target_vertex;
    printf("path to reach vertex %d from source %d\n",target_vertex->id, src_vertex->id);
    while(print_vertex->id!=src_vertex->id){
        printf(" vertex %d\n",print_vertex->prev_vertex->id);
        print_vertex=print_vertex->prev_vertex;
    }
    free(q);
    q=NULL;
    return target_vertex;
}
//updates the h_cost for every vertex using Euclidean distance from previous vertex
double calculate_g_cost(struct Vertex* prev_vertex, struct Vertex* current_vertex){
    int x1, x2, y1, y2;
    x1 = prev_vertex->x_left;
    y1 = prev_vertex->y_low;
    x2 = current_vertex->x_left;
    y2 = current_vertex->y_low;
    return sqrt((x2-x1)*(x2-x1) + (y2-y1)*(y2-y1))+prev_vertex->g_cost;
}
//updates the h_cost for every vertex using Euclidean distance from target
void update_h_cost(struct Vertex* vertex_array, struct Vertex* src_vertex, struct Vertex* target_vertex, int total_grid_count){
    int i, x1, x2, y1, y2;
    src_vertex->h_cost=0;
    x2 = target_vertex->x_left;
    y2 = target_vertex->y_low;
    for(i=0;i<total_grid_count;i++){
        x1 = vertex_array[i].x_left;
        y1 = vertex_array[i].y_low;
        vertex_array->h_cost= sqrt((x2-x1)*(x2-x1) + (y2-y1) * (y2-y1));
    }
}

//----Vertex functions----
//initialize all the variables for all vertices of the graph
void initialize_vertex(struct Vertex* vertex_array,int row_count, int column_count,int unit_length,int total_count){
    int i,j,start_cost,current_index;
    start_cost = INT_MAX;
    for(i=0;i<row_count;i++){
        for(j=0;j<column_count;j++){
            current_index = i*row_count+j;
            vertex_array[current_index].id = current_index;
            vertex_array[current_index].g_cost = start_cost;
            vertex_array[current_index].h_cost = start_cost;
            vertex_array[current_index].x_right =j*unit_length;
            vertex_array[current_index].x_left =((j+1)*unit_length)-1;
            vertex_array[current_index].y_high =i*unit_length;
            vertex_array[current_index].y_low =((i+1)*unit_length)-1;
            vertex_array[current_index].is_obstacle = false;
            vertex_array[current_index].is_border = false;
            vertex_array[current_index].visited = false;
            vertex_array[current_index].prev_vertex = NULL;
        }
    }
}
//reset the cost, obstacle, border, visited, and prev_vertex for all vertices, use this every time after 1 path has been found
void reset_vertex(struct Vertex* vertex_array,int row_count, int column_count,int unit_length,int total_count){
    int i,j,start_cost,current_index;
    start_cost = INT_MAX;
    for(i=0;i<row_count;i++){
        for(j=0;j<column_count;j++){
            current_index = i*row_count+j;
            vertex_array[current_index].g_cost = start_cost;
            vertex_array[current_index].h_cost = start_cost;
            vertex_array[current_index].is_obstacle = false;
            vertex_array[current_index].is_border = false;
            vertex_array[current_index].visited = false;
            vertex_array[current_index].prev_vertex = NULL;
        }
    }
}
//generates the grid structure in the adjacency matrix
void generate_adj_matrix(struct Vertex* adj_matrix[total_grid_count][total_grid_count],struct Vertex* vertex_array, int row_count, int column_count, int total_count){
    int i,j, mod_result;
    //          top bot left right topleft topright botleft botright
    //offset x  0   0   -1      1   -1      1       -1      1
    //offset y  c   -c  0       0   c       c       -c      -c

    //set all as NULL for now
    for(i=0;i<total_count;i++){ //row
        for(j=0;j<total_count;j++){ //col
            adj_matrix[i][j]=NULL;
        }
    }
    for(i=0;i<total_count;i++){ //row
            adj_matrix[i][j]=NULL;
            mod_result = (i+1)%column_count;
            //if vertex not in the top row && not in the bot row && not the last column && not the first column
            if(i<(total_count-row_count) && i>row_count && mod_result!=0 && mod_result!=1){
                adj_matrix[i][i+1]=&vertex_array[i+1]; // right
                adj_matrix[i][i-1]=&vertex_array[i-1]; // left
                adj_matrix[i][i+column_count]=&vertex_array[i+column_count]; // top
                adj_matrix[i][i-column_count]=&vertex_array[i-column_count]; // bot
                adj_matrix[i][i+1+column_count]=&vertex_array[i+1+column_count]; // top right
                adj_matrix[i][i+1-column_count]=&vertex_array[i+1-column_count]; // bot right
                adj_matrix[i][i-1+column_count]=&vertex_array[i-1+column_count]; // top left
                adj_matrix[i][i-1-column_count]=&vertex_array[i-1-column_count]; // bot left
            }
            //if vertex in the first column and first row
            else if(mod_result==1 && i<row_count){
                adj_matrix[i][i+1]=&vertex_array[i+1]; //right
                adj_matrix[i][i+column_count]=&vertex_array[i+column_count]; //top
                adj_matrix[i][i+1+column_count]=&vertex_array[i+1+column_count]; //top right
            }
            //if vertex in first column last row
            else if(mod_result==1 && i>=(total_count-row_count)){
                adj_matrix[i][i+1]=&vertex_array[i+1];// right
                adj_matrix[i][i-column_count]=&vertex_array[i-column_count]; // bot
                adj_matrix[i][i+1-column_count]=&vertex_array[i+1-column_count]; // bot right
            }
            //if vertex in last column first row
            else if(mod_result==0 && i<row_count){
                adj_matrix[i][i-1]=&vertex_array[i-1]; // left
                adj_matrix[i][i+column_count]=&vertex_array[i+column_count]; // top
                adj_matrix[i][i-1+column_count]=&vertex_array[i-1+column_count]; // top left
            }
            //if vertex in last column last row
            else if(mod_result==0 && i>=(total_count-row_count)){
                adj_matrix[i][i-1]=&vertex_array[i-1]; // left
                adj_matrix[i][i-column_count]=&vertex_array[i-column_count]; // bot
                adj_matrix[i][i-1-column_count]=&vertex_array[i-1-column_count]; // bot left
            }
            //first column
            else if(mod_result==1){
                adj_matrix[i][i+1]=&vertex_array[i+1]; //right
                adj_matrix[i][i+column_count]=&vertex_array[i+column_count]; // top
                adj_matrix[i][i-column_count]=&vertex_array[i-column_count]; // bot
                adj_matrix[i][i+1+column_count]=&vertex_array[i+1+column_count]; //top right
                adj_matrix[i][i+1-column_count]=&vertex_array[i+1-column_count]; // bot right
            }
            //last column
            else if(mod_result==0){
                adj_matrix[i][i-1]=&vertex_array[i-1]; // left
                adj_matrix[i][i+column_count]=&vertex_array[i+column_count]; // top
                adj_matrix[i][i-column_count]=&vertex_array[i-column_count]; // bot
                adj_matrix[i][i-1+column_count]=&vertex_array[i-1+column_count]; // top left
                adj_matrix[i][i-1-column_count]=&vertex_array[i-1-column_count]; // bot left
            }
            //first row
            else if(i<row_count){
                adj_matrix[i][i+1]=&vertex_array[i+1]; //right
                adj_matrix[i][i-1]=&vertex_array[i-1]; // left
                adj_matrix[i][i+column_count]=&vertex_array[i+column_count]; // top
                adj_matrix[i][i+1+column_count]=&vertex_array[i+1+column_count]; //top right
                adj_matrix[i][i-1+column_count]=&vertex_array[i-1+column_count]; // top left
            }
            //last row
            else if(i>=(total_count-row_count)){
                adj_matrix[i][i+1]=&vertex_array[i+1]; //right
                adj_matrix[i][i-1]=&vertex_array[i-1]; // left
                adj_matrix[i][i-column_count]=&vertex_array[i-column_count]; // bot
                adj_matrix[i][i+1-column_count]=&vertex_array[i+1-column_count]; // bot right
                adj_matrix[i][i-1-column_count]=&vertex_array[i-1-column_count]; // bot left
        }
    }
}
/*
//adds an obstacles and the borders of the obstacles into the grid
//not complete
void add_obstacle(struct Vertex* vertex_array, struct Obstacle* obstacle_array, int no_of_obstacles, int unit_length,int border_length, int total_count){
    int x_pos, y_pos, i, j, k, l;
    //find obstacle location on vertex array
    for(i=0;i<no_of_obstacles;i++){
        x_pos=obstacle_array[i].x_coor;
        y_pos=obstacle_array[i].y_coor;
        for(j=0;j<total_count;j++){
            //if x_pos and y_pos are within the x and y coordinates of a vertex, that vertex is set to an obstacle, assuming each grid is 10 wide
            //set the corresponding vertex is_obstacle to true
            if(x_pos>=vertex_array[j].x_left && x_pos<=vertex_array.x_right && y_pos>=vertex_array[j].y_low && y_pos<=vertex_array[j].y_high){
                vertex_array[j].is_obstacle=true;
                break;
            }
        }
    //set the neighbours to border
    //2 for loops
    //-2 to +2 -2,-1,0,1,2 for row & col
    //

    }

}
*/

//----Queue functions----
 void create_empty_queue(struct Sorted_Queue* q){
    q->head=NULL;
    q->tail=NULL;
    q->current_size=0;
}
//add new node to queue
void enqueue(struct Sorted_Queue* q, struct Vertex* vertex){
    struct Queue_Node* new_node = (struct Queue_Node*)malloc(sizeof(struct Queue_Node));
    if(new_node==NULL){
        printf("not enough memory");
    }
    new_node->q_vertex=vertex;
    new_node->next=NULL;
    //q empty
    if(q->head==NULL){
        q->head = new_node;
    }
    //q not empty
    if(q->tail!=NULL){
        q->tail->next = new_node;
    }
    //update tail and size regardless since queue is FIFO
    q->tail = new_node;
    q->current_size++;
}
//remove a node from the head of queue and return the vertex pointer of the node that was removed
struct Vertex* dequeue(struct Sorted_Queue* q){
    struct Vertex* result = (struct Vertex*)malloc(sizeof(struct Vertex));
    //q is empty
    if(q->head==NULL){
        return NULL;
    }
    result = q->head->q_vertex;
    q->head = q->head->next;
    q->current_size--;
    //if q is now empty
    if(q->head==NULL){
        q->tail = NULL;
    }
    return result;
}
//sorts the queue according in ascending order of g_cost
void sort_queue(struct Sorted_Queue* q){

    int i, j, k, min_index, size, min_cost, curr_cost;
    struct Vertex* min = (struct Vertex*)malloc(sizeof(struct Vertex));
    struct Vertex* temp = (struct Vertex*)malloc(sizeof(struct Vertex));
    struct Queue_Node* curr = (struct Queue_Node*)malloc(sizeof(struct Queue_Node));
    if(q->head!=NULL){
        size=q->current_size;
        //loops through the whole queue
        for(i=0;i<size;i++){
            curr= q->head;
            min = q->head->q_vertex; //just a large number
            min_index =0;
            //find the index and min value for the first size-i elements in the queue
            for(j=0;j<size-i;j++){
                min_cost = min->g_cost+ min->h_cost;
                curr_cost = curr->q_vertex->g_cost + curr->q_vertex->h_cost;
                if(min_cost > curr_cost){
                    min = curr->q_vertex;
                    min_index = j;
                }
                curr=curr->next;
            }
            //dequeue all elements and enqueue them in order, except for the min value
            for(k=0;k<size;k++){
                temp = dequeue(q);
                if(k!=min_index){
                    enqueue(q,temp);
                }
            }
            //enqueue min value at the end
            enqueue(q,min);
        }
    }
}

