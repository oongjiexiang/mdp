#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

//constant variables
double UNIT_LENGTH = 10;
double ROBOT_LENGTH = 30;
//robot takes up 3 cells

//structure of a single vertex
struct Vertex{
    int vertex_number;  //vertex number starts from 0
    double x_start;    //starts from 0
    double x_end;      //ends at vertex length -1
    double y_start;    //starts from 0
    double y_end;      // ends at vertex height -1
    bool is_obstacle;
    bool is_goal;
    bool visited;
    int g_cost;         //cost of distance from the starting vertex costs are multiplied by 10 to make them whole numbers which will be easier to work with
    int h_cost;         //cost of distance from current vertex to the final vertex
};

struct AdjListNode{
    struct Vertex* vertex_addr;
    struct AdjListNode* next;
};

//first vertex of the list
struct AdjList{
    struct AdjListNode *head;
};

//struct of the graph, an array of adjacency list
struct Grid{
    int grid_size;
    struct AdjList* array;
};

struct QNode{
    struct Vertex* vertex;
    struct QNode* next;
};
//structure for q
struct Queue{
    struct QNode* head;
    struct QNode* tail;
};


//function prototypes
//----queue functions----
void initialize_queue(struct Queue* q);
bool enqueue(struct Queue* q, struct Vertex* vertex);
int dequeue(struct Queue* q);
bool move_closest_node_to_front_q(struct Queue* q);
//----graph generation----
struct AdjListNode* new_adj_list_node(struct Vertex* vertex_addr);
struct Grid* create_grid(int grid_size);
void add_edge(struct Grid* grid, int src_vertex, int dest_vertex,struct Vertex* src_vertex_addr, struct Vertex* dest_vertex_addr);
void initialize_vertex(int grid_length, int grid_height, int total_length, int total_height, struct Vertex* vertexArray);
void connect_grid(struct Grid* grid, struct Vertex* vertex_array, int grid_length, int grid_height, int total_length, int total_height);
//----search----
void add_obstacle(struct Vertex* vertex_addr);
bool plan_trip(struct Grid* grid, struct Vertex* vertex_array, struct Vertex* goal_array, struct Vertex* src_vertex_addr,int grid_size,int number_of_goals);
struct Vertex* find_closest_vertex(struct Vertex* vertex_array, int src_vertex, bool* visited, int no_of_vertex);
void distance_to_goals(struct Vertex* vertex_array,double* distance_array,int* vertex_number_array,bool* visited, int src_vertex, int number_of_goals, int no_of_vertex);

int main()
{
    //testing function
    int total_grid_count, grid_length, grid_height, total_length, total_height,goals,i;
    total_grid_count = 25;
    grid_length=10;
    grid_height=10;
    total_length=50;
    total_height=50;
    goals=4;
    //possible to put these into grid struct instead so we do not pass as many variables into the functions

    struct Vertex* vertex_array = (struct Vertex*) malloc(total_grid_count*sizeof(struct Vertex));
    struct Grid* grid = create_grid(total_grid_count);
    struct Vertex* goals_order = (struct Vertex*)malloc(goals*sizeof(struct Vertex));

    initialize_vertex(grid_length,grid_height,total_length,total_height,vertex_array);
    connect_grid(grid,vertex_array,grid_length,grid_height,total_length,total_height);


    vertex_array[12].is_goal=true;
    vertex_array[9].is_goal=true;
    vertex_array[20].is_goal=true;
    vertex_array[24].is_goal=true;

    vertex_array[21].is_obstacle=true;
    vertex_array[16].is_obstacle=true;

    plan_trip(grid,vertex_array,goals_order,&vertex_array[22],total_grid_count,goals);
    printf("Starting location: 22\n");
    printf("Order of goals closest to you are: ");
    for(i=0;i<goals;i++){
        printf("%d gcost: %d, ", goals_order[i].vertex_number,goals_order[i].g_cost);
    }

    return 0;
}

//----queue functions----
void initialize_queue(struct Queue* q){
    q->head=NULL;
    q->tail=NULL;
}
//enqueues a vertex
bool enqueue(struct Queue* q, struct Vertex* vertex){
    //create the new node
    struct QNode* new_node = (struct QNode*)malloc(sizeof(struct QNode));
    if(new_node==NULL)
        return false;

    new_node->vertex = vertex;
    new_node->next = NULL;

    //if q is not empty connect new node to the end of q
    if(q->tail!=NULL){
        q->tail->next = new_node;
    }
    //if q is empty add new node to head of q
    if(q->head == NULL){
        q->head = new_node;
    }
    //update the q tail
    q->tail=new_node;
    return true;
}
//dequeues a node in the q and returns the vertex number
int dequeue(struct Queue* q){
    int vertex_removed;
    //return if head is empty
    if(q->head == NULL){
        printf("Queue is empty!");
        return -1;
    }
    vertex_removed = q->head->vertex->vertex_number;

    q->head=q->head->next;
    //q the q becomes empty, set tail = null
    if(q->head==NULL){
        q->tail=NULL;
    }
    return vertex_removed;
}
//move the highest priority(shortest distance, gcost) node to the front of q (only moves the first node with highest priority) if 2 nodes have equal priority, move only the first one
bool move_closest_node_to_front_q(struct Queue* q){
    if(q->head==NULL){
        return false;
    }
    if(q->head==q->tail){
        return true;
    }
    //pointers to keep track of locations
    struct QNode* prev=q->head;
    struct QNode* curr=q->head;
    struct QNode* smallest =q->head;
    struct QNode* temp = NULL;
    double shortest_distance;
    //assume shortest distance is head for now
    shortest_distance=q->head->vertex->g_cost;

    //if shortest distance is the first node, no need to move
    while(curr->next!=NULL){
        //move curr pointer forward
        curr=curr->next;
        //if current node has a smaller gcost(shorter distance)
        if(curr->vertex->g_cost < shortest_distance){
            shortest_distance=curr->vertex->g_cost;
            //save the node just before the smallest node
            temp = prev;
            smallest=curr;
        }
        //move prev pointer forward
        prev=curr;
    }
    //first element is the smallest and no sorting is needed
    if(temp==NULL){
        return false;
    }
    //after the loop the smallest node and the node before it is saved in smallest and temp respectively
    //if smallest is the head, q in the right order
    if(smallest!=q->head){
    temp->next=smallest->next;
    smallest->next=q->head;
    q->head=smallest;
    }

    return true;
}

//create a new adjacency list node
struct AdjListNode* new_adj_list_node(struct Vertex* vertex){
    struct AdjListNode* newNode = (struct AdjListNode*) malloc(sizeof(struct AdjListNode));
    newNode->vertex_addr = vertex;
    newNode->next = NULL;
    return newNode;
}


//create an area in memory to store the grid/graph of grid_size number of vertices
struct Grid* create_grid(int grid_size){
    int i;
    //memory allocation for the grid/graph
    struct Grid* grid = (struct Grid*) malloc(sizeof(struct Grid));
    if(grid == NULL){
        printf("Error allocating memory");
        return NULL;
    }
    grid->grid_size = grid_size;

    //create an array of adjacency lists headers
    grid->array = (struct AdjList*) malloc(grid_size*sizeof(struct AdjList));
    if(grid->array == NULL){
        printf("Error allocating memory");
        return NULL;
    }

    //set the head to be equal to null for now
    for(i=0;i<grid_size;i++)
        grid->array[i].head=NULL;

    return grid;
}


//pass in a pointer to an array of empty vertices to generate vertices with it's variables populated
void initialize_vertex(int grid_length, int grid_height, int total_length, int total_height, struct Vertex* vertex_array){
    int i,j,vertex_index, no_of_grids_length, no_of_grids_height;
    //find number of grids
    no_of_grids_length = total_length/grid_length;
    no_of_grids_height = total_height/grid_height;


    for(i=0;i<no_of_grids_height;i++){
        for(j=0;j<no_of_grids_length;j++){
                //vertex index starts from 0
            vertex_index = i*no_of_grids_length+j;
            vertex_array[vertex_index].vertex_number = vertex_index;
            vertex_array[vertex_index].x_start =  (double)(j*grid_length);
            vertex_array[vertex_index].x_end =  (double)((j+1)*grid_length-1);
            vertex_array[vertex_index].y_start =  (double)(i*grid_height);
            vertex_array[vertex_index].y_end =  (double)((i+1)*grid_height-1);
            vertex_array[vertex_index].is_obstacle = false;
            vertex_array[vertex_index].is_goal=false;
            vertex_array[vertex_index].visited=false;
            //initialise to 0 until we get the starting locations and target locations
            vertex_array[vertex_index].g_cost = 10000; //very large number
            vertex_array[vertex_index].h_cost = 0;
        }
    }

}


//add a single direct edge from source vertex to destination vertex and store it into the grid
//note that vertex number is +1 of the vertex index, eg. vertex 1 implies vertex[0] in the array
//insert the node to the back of the linked list
//after creating the new node, traverse through linked list  of grid->array[src_vertex].head to the end of the list
//after reaching the end, set node->next of the last node = the new node that we want to insert
void add_edge(struct Grid* grid, int src_vertex, int dest_vertex,struct Vertex* src_vertex_addr, struct Vertex* dest_vertex_addr){
    src_vertex = src_vertex-1;
    dest_vertex = dest_vertex-1;

    //only create a new node if head is not initialised
    if(grid->array[src_vertex].head == NULL){
    grid->array[src_vertex].head = new_adj_list_node(src_vertex_addr);
    }

    //traverse to the end of the list
    struct AdjListNode *traversal_ptr = grid->array[src_vertex].head;
    struct AdjListNode *prev_ptr = traversal_ptr;
    while(traversal_ptr!=NULL){
        prev_ptr = traversal_ptr;
        traversal_ptr = traversal_ptr->next;
    }
    //add new node to the end of the list
    prev_ptr->next = new_adj_list_node(dest_vertex_addr);
    //printf("src vertex: %d connected to: %d\n", src_vertex,dest_vertex);
}

/*adds all necessary edges to the graph
    Vertex and grid here is used interchangeably
    Edges are added to the vertices following these rules(all edges added are undirected, 2 way edges):
    5 cases:
    1. Vertex not located at the border: Connect with right, left, top, bot, top left, top right, bot left, bot right (connect to all neighbors)
    2. Vertex in last column: Connect with left, top, bot, top left, bot left (do not connect with anything from the right)
    3. Vertex in first column: Connect with right, top, bot, top right, bot right (do not connect with anything from the left)
    4. Vertex in top row: Connect with right, left, bot, bot right, bot left (do not connect with anything from the top)
    5. Vertex in the bot row: Connect with right, left, top, top right, top left (do not connect with anything from the bot)
    If a vertex first in more than one case, take the intersect of the 2 cases
    eg. vertex is the first vertex (first column, bot row): right, top, top right
*/
void connect_grid(struct Grid* grid, struct Vertex* vertex_array, int grid_length, int grid_height, int total_length, int total_height){
    int i, j, current_vertex_number, next_vertex_number, prev_vertex_number, no_of_grids_length, no_of_grids_height,
    mod_result_length, height_threshold ,prev_index_number, current_index_number, next_index_number;

    no_of_grids_length = total_length/grid_length;
    no_of_grids_height = total_height/grid_height;
    //printf("grid length, grid height: %d , %d \n",no_of_grids_length, no_of_grids_height);
    height_threshold = (no_of_grids_height-1)*no_of_grids_length; //if the vertex number is greater than height threshold, it is a vertex on the top row

   //row
    for(i=0;i<no_of_grids_height;i++){
        //column
        for(j=0;j<no_of_grids_length;j++){
            //current vertex number starts from 1
            prev_vertex_number = i*no_of_grids_length+(j);
            current_vertex_number = prev_vertex_number+1;
            next_vertex_number = current_vertex_number+1;

            prev_index_number = prev_vertex_number-1;
            current_index_number = current_vertex_number-1;
            next_index_number = next_vertex_number-1;

            mod_result_length = current_vertex_number%no_of_grids_length; //if mod value == 0 it is the last vertex of the row, if value == 1, it is the first vertex of the row

            //if vertex not in the top row && not in the bot row && not the last column && not the first column
            if((current_vertex_number<=height_threshold) && (current_vertex_number>no_of_grids_length) && !(mod_result_length==0) && !(mod_result_length ==1)){
                    //perform connection with all neighbors
                    //adds an edge with the top vertex
                    add_edge(grid,current_vertex_number,current_vertex_number+no_of_grids_length,&vertex_array[current_index_number],&vertex_array[current_index_number+no_of_grids_length]);
                    //adds an edge with the top right vertex
                    add_edge(grid,current_vertex_number,next_vertex_number+no_of_grids_length,&vertex_array[current_index_number],&vertex_array[next_index_number+no_of_grids_length]);
                    //adds and edge with the top left vertex
                    add_edge(grid,current_vertex_number,prev_vertex_number+no_of_grids_length,&vertex_array[current_index_number],&vertex_array[prev_index_number+no_of_grids_length]);
                    //adds an edge with the vertex on the right
                    add_edge(grid,current_vertex_number,next_vertex_number,&vertex_array[current_index_number],&vertex_array[next_index_number]);
                    //adds an edge with the vertex on the left
                    add_edge(grid,current_vertex_number,prev_vertex_number,&vertex_array[current_index_number],&vertex_array[prev_index_number]);
                    //adds an edge with the bot vertex
                    add_edge(grid,current_vertex_number,current_vertex_number-no_of_grids_length,&vertex_array[current_index_number],&vertex_array[current_index_number-no_of_grids_length]);
                    //adds an edge with the bot right vertex
                    add_edge(grid,current_vertex_number,next_vertex_number-no_of_grids_length,&vertex_array[current_index_number],&vertex_array[next_index_number-no_of_grids_length]);
                    //adds and edge with the bot left vertex
                    add_edge(grid,current_vertex_number,prev_vertex_number-no_of_grids_length,&vertex_array[current_index_number],&vertex_array[prev_index_number-no_of_grids_length]);
            }
            //if vertex in the first column but not the top row or bot row
            else if(mod_result_length ==1 && !(current_vertex_number>height_threshold) && !(current_vertex_number<=no_of_grids_length)){
                //connect with top, bot ,top right, bot right, right
                //adds an edge with the top vertex
                add_edge(grid,current_vertex_number,current_vertex_number+no_of_grids_length,&vertex_array[current_index_number],&vertex_array[current_index_number+no_of_grids_length]);
                //adds an edge with the top right vertex
                add_edge(grid,current_vertex_number,next_vertex_number+no_of_grids_length,&vertex_array[current_index_number],&vertex_array[next_index_number+no_of_grids_length]);
                //adds an edge with the vertex on the right
                add_edge(grid,current_vertex_number,next_vertex_number,&vertex_array[current_index_number],&vertex_array[next_index_number]);
                //adds an edge with the bot vertex
                add_edge(grid,current_vertex_number,current_vertex_number-no_of_grids_length,&vertex_array[current_index_number],&vertex_array[current_index_number-no_of_grids_length]);
                //adds an edge with the bot right vertex
                add_edge(grid,current_vertex_number,next_vertex_number-no_of_grids_length,&vertex_array[current_index_number],&vertex_array[next_index_number-no_of_grids_length]);
            }
            //if vertex in the last column but not the top row or bot row
            else if(mod_result_length ==0 && !(current_vertex_number>height_threshold) && !(current_vertex_number<=no_of_grids_length)){
                //connect with top, bot, top left, bot left, left
                //adds an edge with the top vertex
                add_edge(grid,current_vertex_number,current_vertex_number+no_of_grids_length,&vertex_array[current_index_number],&vertex_array[current_index_number+no_of_grids_length]);
                //adds and edge with the top left vertex
                add_edge(grid,current_vertex_number,prev_vertex_number+no_of_grids_length,&vertex_array[current_index_number],&vertex_array[prev_index_number+no_of_grids_length]);
                //adds an edge with the vertex on the left
                add_edge(grid,current_vertex_number,prev_vertex_number,&vertex_array[current_index_number],&vertex_array[prev_index_number]);
                //adds an edge with the bot vertex
                add_edge(grid,current_vertex_number,current_vertex_number-no_of_grids_length,&vertex_array[current_index_number],&vertex_array[current_index_number-no_of_grids_length]);
                //adds and edge with the bot left vertex
                add_edge(grid,current_vertex_number,prev_vertex_number-no_of_grids_length,&vertex_array[current_index_number],&vertex_array[prev_index_number-no_of_grids_length]);
            }
            //if vertex in top row
            else if(current_vertex_number>height_threshold){
                //if vertex in first column
                if(mod_result_length ==1){
                    //connect with bot, bot right, bot ,right
                    //adds an edge with the bot vertex
                    add_edge(grid,current_vertex_number,current_vertex_number-no_of_grids_length,&vertex_array[current_index_number],&vertex_array[current_index_number-no_of_grids_length]);
                    //adds an edge with the bot right vertex
                    add_edge(grid,current_vertex_number,next_vertex_number-no_of_grids_length,&vertex_array[current_index_number],&vertex_array[next_index_number-no_of_grids_length]);
                    //adds an edge with the vertex on the right
                    add_edge(grid,current_vertex_number,next_vertex_number,&vertex_array[current_index_number],&vertex_array[next_index_number]);
                }
                //if vertex in last column
                else if(mod_result_length==0){
                    //connect with bot, bot left, left
                    //adds an edge with the bot vertex
                    add_edge(grid,current_vertex_number,current_vertex_number-no_of_grids_length,&vertex_array[current_index_number],&vertex_array[current_index_number-no_of_grids_length]);
                    //adds and edge with the bot left vertex
                    add_edge(grid,current_vertex_number,prev_vertex_number-no_of_grids_length,&vertex_array[current_index_number],&vertex_array[prev_index_number-no_of_grids_length]);
                    //adds an edge with the vertex on the left
                    add_edge(grid,current_vertex_number,prev_vertex_number,&vertex_array[current_index_number],&vertex_array[prev_index_number]);
                }
                //if vertex in top row but not in the first column or last column
                else{
                    //connect with bot, bot left, bot right, right, left
                    //adds an edge with the bot vertex
                    add_edge(grid,current_vertex_number,current_vertex_number-no_of_grids_length,&vertex_array[current_index_number],&vertex_array[current_index_number-no_of_grids_length]);
                    //adds and edge with the bot left vertex
                    add_edge(grid,current_vertex_number,prev_vertex_number-no_of_grids_length,&vertex_array[current_index_number],&vertex_array[prev_index_number-no_of_grids_length]);
                    //adds an edge with the bot right vertex
                    add_edge(grid,current_vertex_number,next_vertex_number-no_of_grids_length,&vertex_array[current_index_number],&vertex_array[next_index_number-no_of_grids_length]);
                    //adds an edge with the vertex on the right
                    add_edge(grid,current_vertex_number,next_vertex_number,&vertex_array[current_index_number],&vertex_array[next_index_number]);
                    //adds an edge with the vertex on the left
                    add_edge(grid,current_vertex_number,prev_vertex_number,&vertex_array[current_index_number],&vertex_array[prev_index_number]);
                }
            }
            //if vertex in bot row
            else if(current_vertex_number<=no_of_grids_length){
                //if vertex in first column
                if(mod_result_length ==1){
                    //connect with top, top right, right
                    //adds an edge with the top vertex
                    add_edge(grid,current_vertex_number,current_vertex_number+no_of_grids_length,&vertex_array[current_index_number],&vertex_array[current_index_number+no_of_grids_length]);
                    //adds an edge with the top right vertex
                    add_edge(grid,current_vertex_number,next_vertex_number+no_of_grids_length,&vertex_array[current_index_number],&vertex_array[next_index_number+no_of_grids_length]);
                    //adds an edge with the vertex on the right
                    add_edge(grid,current_vertex_number,next_vertex_number,&vertex_array[current_index_number],&vertex_array[next_index_number]);
                }
                //if vertex in last column
                else if(mod_result_length==0){
                    //connect with top, top left, left
                    //adds an edge with the top vertex
                    add_edge(grid,current_vertex_number,current_vertex_number+no_of_grids_length,&vertex_array[current_index_number],&vertex_array[current_index_number+no_of_grids_length]);
                    //adds and edge with the top left vertex
                    add_edge(grid,current_vertex_number,prev_vertex_number+no_of_grids_length,&vertex_array[current_index_number],&vertex_array[prev_index_number+no_of_grids_length]);
                    //adds an edge with the vertex on the left
                    add_edge(grid,current_vertex_number,prev_vertex_number,&vertex_array[current_index_number],&vertex_array[prev_index_number]);
                }
                //if vertex in bot row but not in the first column or last column
                else{
                    //connect with top, top left, top right, right, left
                    //adds an edge with the top vertex
                    add_edge(grid,current_vertex_number,current_vertex_number+no_of_grids_length,&vertex_array[current_index_number],&vertex_array[current_index_number+no_of_grids_length]);
                    //adds and edge with the top left vertex
                    add_edge(grid,current_vertex_number,prev_vertex_number+no_of_grids_length,&vertex_array[current_index_number],&vertex_array[prev_index_number+no_of_grids_length]);
                    //adds an edge with the top right vertex
                    add_edge(grid,current_vertex_number,next_vertex_number+no_of_grids_length,&vertex_array[current_index_number],&vertex_array[next_index_number+no_of_grids_length]);
                    //adds an edge with the vertex on the right
                    add_edge(grid,current_vertex_number,next_vertex_number,&vertex_array[current_index_number],&vertex_array[next_index_number]);
                    //adds an edge with the vertex on the left
                    add_edge(grid,current_vertex_number,prev_vertex_number,&vertex_array[current_index_number],&vertex_array[prev_index_number]);
                }
            }

        }

    }
}


//-------------------------Search algorithm-----------------------------------
//Based on dijkstra's algorithm, find the path to a set of goals based on the distance of the goal from the source
//needs to be changed, some parts are still not correct
//add a distance array and a previous node array
bool plan_trip(struct Grid* grid, struct Vertex* vertex_array, struct Vertex* goal_array, struct Vertex* src_vertex_addr,int grid_size,int number_of_goals){
    //find closest vertex
    //check if it is an obstacle
    //if it is an obstacle save the distance into an array
    //exit once number of goals have been reached
    int i,goals_found,current_vertex_number;
    double x1,x2,y1,y2,temp;
    struct Queue* q = (struct Queue*)malloc(sizeof(struct Queue));
    struct Vertex* current_vertex = (struct Vertex*)malloc(sizeof(struct Vertex));

    struct AdjListNode* list_ptr = (struct AdjListNode*)malloc(sizeof(struct AdjListNode));
    goals_found=0;
    for(i=0;i<grid_size;i++){
        enqueue(q,&vertex_array[i]);
    }
    //set src distance to 0
    src_vertex_addr->g_cost=0;
    x1 = src_vertex_addr->x_start;
    y1 = src_vertex_addr->y_start;
    //move src to the front of the q

    move_closest_node_to_front_q(q);

    //while there are still vertices unvisited
    while(q->head!=NULL){
        current_vertex_number=dequeue(q);
        current_vertex=&vertex_array[current_vertex_number];

        current_vertex->visited=true;
        current_vertex_number = current_vertex->vertex_number;
        if(current_vertex->is_goal==true){
            //add vertex to array
            goal_array[goals_found]=*current_vertex;
            goals_found++;
            if(goals_found==number_of_goals){
            return true;
            }
        }

        list_ptr=grid->array[current_vertex_number].head;

        //look through all the neighbours
        while(list_ptr->next!=NULL){
            //if vertex has not been visited
            if(list_ptr->vertex_addr->visited==false){
                x2 = list_ptr->vertex_addr->x_start;
                y2 = list_ptr->vertex_addr->y_start;
                temp = sqrt((x2-x1)*(x2-x1) + (y2-y1) * (y2-y1));
                //check if new distance is minimum distance
                if(temp<list_ptr->vertex_addr->g_cost){
                    //update new cost
                    list_ptr->vertex_addr->g_cost=temp;
                }
            }
            list_ptr=list_ptr->next;
        }
        //move closest vertex to the front of the q, it will be explored next
        move_closest_node_to_front_q(q);
    }
    return false;

}

/*
//brute force method to find nearest goals
void distance_to_goals(struct Vertex* vertex_array,double* distance_array,int* vertex_number_array,bool* visited, int src_vertex, int number_of_goals, int no_of_vertex){
    //find closest vertex
    //check if it is an obstacle
    //if it is an obstacle save the distance into an array
    //exit once number of goals have been reached
    int i,j,closest_distance,closest_vertex_number;
    closest_vertex_number=-1;
    double x1,x2,y1,y2;
    x1=vertex_array[src_vertex].x_start;
    y1=vertex_array[src_vertex].y_start;
    closest_distance=100000; //just a very large number
    j=0;

    for(i=0;i<no_of_vertex;i++){
        if(vertex_array[i].is_obstacle && i!=src_vertex){
            //calculate distance to source
            x2 = vertex_array[i].x_start;
            y2 = vertex_array[i].y_start;
            distance_array[j]=sqrt((x2-x1)*(x2-x1) + (y2-y1) * (y2-y1));
            vertex_number_array[j]=i;
            j++;
        }
    }

}
*/


