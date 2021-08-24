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


//function prototypes
//----graph generation----
struct AdjListNode* new_adj_list_node(struct Vertex* vertex_addr);
struct Grid* create_grid(int grid_size);
void add_edge(struct Grid* grid, int src_vertex, int dest_vertex,struct Vertex* src_vertex_addr, struct Vertex* dest_vertex_addr);
void initialize_vertex(int grid_length, int grid_height, int total_length, int total_height, struct Vertex* vertexArray);
void connect_grid(struct Grid* grid, struct Vertex* vertex_array, int grid_length, int grid_height, int total_length, int total_height);

//----search----
void add_obstacle(struct Vertex* vertex_addr);
void plan_path(struct Grid* grid, struct Vertex* vertex_path_array, int src_vertex_number, int number_of_obstacles, int number_of_vertices);
struct Vertex* find_closest_vertex(struct Vertex* vertex_array, int src_vertex, bool* visited, int no_of_vertex);
void distance_to_goals(struct Vertex* vertex_array,double* distance_array,int* vertex_number_array,bool* visited, int src_vertex, int number_of_goals, int no_of_vertex);

int main()
{
    //-------------testing function to generate a 20 by 20 grid-----------------
    int i,j;
    //generate vertices
    int total_grid_count = 25;
    struct Vertex* vertex_array = (struct Vertex*) malloc(total_grid_count*sizeof(struct Vertex));

    initialize_vertex(10,10,50,50,vertex_array);

    //generate graph
    struct Grid* g = create_grid(25);

    connect_grid(g,vertex_array,10,10,50,50);

    struct AdjListNode *list_ptr = NULL;
    struct Vertex *vertexA =NULL;

    //print the vertices and it's neighbors
    for(i=0;i<25;i++){
    list_ptr = g->array[i].head;
    //printf("current vertex: index = %d, vertex number = %d\n", i,list_ptr->vertex_addr->vertex_number);

    vertexA= list_ptr->vertex_addr;
        while(list_ptr != NULL){
            vertexA = list_ptr->vertex_addr;
            //printf("connected to %d \n", vertexA->vertex_number);
            list_ptr = list_ptr->next;
        }
    }
    bool* visited = (bool*)malloc(total_grid_count*sizeof(bool));
    for(j=0;j<total_grid_count;j++){
        visited[j]=false;
        //printf("vertex x start pos: %f\n",vertex_array[j].x_start);
        //printf("vertex x end pos: %f\n",vertex_array[j].x_end);
        //printf("vertex y start pos: %f\n",vertex_array[j].y_start);
        //printf("vertex y end pos: %f\n",vertex_array[j].y_end);
    }
    find_closest_vertex(vertex_array,5,visited,25);
    double* distance_array=(double*)malloc(total_grid_count*sizeof(double));
    int* vertex_number_array = (int*)malloc(total_grid_count*sizeof(int));

    vertex_array[4].is_obstacle=true;
    vertex_array[10].is_obstacle=true;
    vertex_array[7].is_obstacle=true;

    distance_to_goals(vertex_array,distance_array,vertex_number_array,visited,0,3,25);
    for(i=0;i<3;i++){

    printf("distance of vertex %d: %f\n", vertex_number_array[i],distance_array[i]);
    }
    /*
    //-------------testing function to generate a 5 by 5 grid-----------------
    int i;
    //generate vertices
    int total_grid_count = 400;
    struct Vertex* vertex_array = (struct Vertex*) malloc(total_grid_count*sizeof(struct Vertex));

    initialize_vertex(10,10,50,50,vertex_array);

    //generate graph
    struct Grid* g = create_grid(25);

    coonnect_grid(g,vertex_array,10,10,50,50);

    struct AdjListNode *list_ptr = NULL;
    struct Vertex *vertexA =NULL;

    for(i=0;i<25;i++){
    list_ptr = g->array[i].head;
    printf("current vertex: index = %d, vertex number = %d\n", i,list_ptr->vertex_addr->vertex_number);

    vertexA= list_ptr->vertex_addr;
        while(list_ptr != NULL){
            vertexA = list_ptr->vertex_addr;
            printf("connected to %d \n", vertexA->vertex_number);
            list_ptr = list_ptr->next;
        }
    }
    */
    return 0;
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

    for(i=1;i<=no_of_grids_height;i++){
        for(j=1;j<=no_of_grids_length;j++){
                //vertex index starts from 0
            vertex_index = (i-1)*no_of_grids_length+(j-1);
            vertex_array[vertex_index].vertex_number = vertex_index;
            vertex_array[vertex_index].x_start =  (double)((j-1)*grid_length);
            vertex_array[vertex_index].x_end =  (double)(j*grid_length-1);
            vertex_array[vertex_index].y_start =  (double)((i-1)*grid_height);
            vertex_array[vertex_index].y_end =  (double)(i*grid_height-1);
            vertex_array[vertex_index].is_obstacle = false;
            //initialise to 0 until we get the starting locations and target locations
            vertex_array[vertex_index].g_cost = 0;
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
/*
// pass in the grid, array to store the path, distance to reach the path, source vertex, obstacle/goal count, total number of vertices
void plan_path(struct Grid* grid, struct Vertex* vertex_path_array, int src_vertex_number, int number_of_obstacles, int number_of_vertices){
    //all vertices have already been initialized to -1
    int goals_found, i;
    //create an array to track the vertices that have been visited and initialize to false
    bool* visited_array = (bool*) malloc(number_of_vertices*sizeof(bool));
    //create an array to store shortest distance of each vertex from the source initialize to -1 to represent infinity
    int* distance_array = (int*) malloc(number_of_vertices*sizeof(int));
    for(i=0;i<number_of_vertices;i++){
        visited_array[i]=false;
        distance_array[i]=-1;
    }
    //create an array to store which node is the previous node of the current node, eg. vertex 2's previous node is vertex 1
    struct Vertex* previous_vertex = (struct Vertex*)malloc(number_of_vertices*sizeof(struct Vertex));

    //distance for source vertex to itself
    distance_array[src_vertex_number]=0;

    //create a loop to check all neighbors of the src vertex

    free(visited_array);
    free(distance_array);
}
*/

//finds the closest vertex using the formula: sqrt((x2-x1)*(x2-x1) + (y2-y1) * (y2-y1))
//can be used to update the gcost
struct Vertex* find_closest_vertex(struct Vertex* vertex_array, int src_vertex, bool* visited, int no_of_vertex){
    //assuming locataion of the pos is at the bottom left corner
    int i,closest_distance,closest_vertex_number;
    closest_vertex_number=-1;
    double x1,x2,y1,y2,temp;
    x1=vertex_array[src_vertex].x_start;
    y1=vertex_array[src_vertex].y_start;
    closest_distance=100000; //just a very large number

    for(i=0;i<no_of_vertex;i++){
        if(visited[i]==false && i!=src_vertex){
            //calculatae distance to source
            x2 = vertex_array[i].x_start;
            y2 = vertex_array[i].y_start;
            temp = sqrt((x2-x1)*(x2-x1) + (y2-y1) * (y2-y1));
            //update g cost
            //vertex_array[i].g_cost=temp;
            printf("closest distance %d: %f\n",i,temp);
            if(temp<closest_distance){
                closest_distance=temp;
                closest_vertex_number=i;
            }
        }
    }
    return &vertex_array[closest_vertex_number];
};

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
