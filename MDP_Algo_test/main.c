#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

//structure of a single vertex
struct Vertex{
    int vertex_number;  //vertex index+1(vertex number starts from 0 for easier calculation later)
    int x_start_pos;    //starts from 0
    int x_end_pos;      //ends at vertex length -1
    int y_start_pos;    //starts from 0
    int y_end_pos;      // ends at vertex height -1
    bool is_obstacle;
    int g_cost;         //cost of distance from the starting vertex
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
struct AdjListNode* newAdjListNode(struct Vertex* vertex_addr);
struct Grid* createGrid(int grid_size);
void addEdge(struct Grid* grid, int src_vertex, int dest_vertex,struct Vertex* src_vertex_addr, struct Vertex* dest_vertex_addr);
void initializeVertex(int grid_length, int grid_height, int total_length, int total_height, struct Vertex* vertexArray);
void connectGrid(struct Grid* grid, struct Vertex* vertex_array, int grid_length, int grid_height, int total_length, int total_height);

int main()
{
    //-------------testing function to generate a 20 by 20 grid-----------------
    int i;
    //generate vertices
    int total_grid_count = 400;
    struct Vertex* vertex_array = (struct Vertex*) malloc(total_grid_count*sizeof(struct Vertex));

    initializeVertex(10,10,200,200,vertex_array);

    //generate graph
    struct Grid* g = createGrid(400);

    connectGrid(g,vertex_array,10,10,200,200);

    struct AdjListNode *list_ptr = NULL;
    struct Vertex *vertexA =NULL;

    //print the vertices and it's neighbors
    for(i=0;i<400;i++){
    list_ptr = g->array[i].head;
    printf("current vertex: index = %d, vertex number = %d\n", i,list_ptr->vertex_addr->vertex_number);

    vertexA= list_ptr->vertex_addr;
        while(list_ptr != NULL){
            vertexA = list_ptr->vertex_addr;
            printf("connected to %d \n", vertexA->vertex_number);
            list_ptr = list_ptr->next;
        }
    }

    /*
    //-------------testing function to generate a 5 by 5 grid-----------------
    int i;
    //generate vertices
    int total_grid_count = 400;
    struct Vertex* vertex_array = (struct Vertex*) malloc(total_grid_count*sizeof(struct Vertex));

    initializeVertex(10,10,50,50,vertex_array);

    //generate graph
    struct Grid* g = createGrid(25);

    connectGrid(g,vertex_array,10,10,50,50);

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
struct AdjListNode* newAdjListNode(struct Vertex* vertex){
    struct AdjListNode* newNode = (struct AdjListNode*) malloc(sizeof(struct AdjListNode));
    newNode->vertex_addr = vertex;
    newNode->next = NULL;
    return newNode;
}


//create an area in memory to store the grid/graph of grid_size number of vertices
struct Grid* createGrid(int grid_size){
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
void initializeVertex(int grid_length, int grid_height, int total_length, int total_height, struct Vertex* vertex_array){
    int i,j,vertex_index, no_of_grids_length, no_of_grids_height;
    //find number of grids
    no_of_grids_length = total_length/grid_length;
    no_of_grids_height = total_height/grid_height;

    for(i=1;i<=no_of_grids_height;i++){
        for(j=1;j<=no_of_grids_length;j++){
                //vertex index starts from 0
            vertex_index = (i-1)*no_of_grids_length+(j-1);
            vertex_array[vertex_index].vertex_number = vertex_index+1;
            vertex_array[vertex_index].x_start_pos =  vertex_index*grid_length;
            vertex_array[vertex_index].x_end_pos =  vertex_index*grid_length-1;
            vertex_array[vertex_index].y_start_pos =  vertex_index*grid_height;
            vertex_array[vertex_index].y_end_pos =  vertex_index*grid_height-1;
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
void addEdge(struct Grid* grid, int src_vertex, int dest_vertex,struct Vertex* src_vertex_addr, struct Vertex* dest_vertex_addr){
    src_vertex = src_vertex-1;
    dest_vertex = dest_vertex-1;

    //only create a new node if head is not initialised
    if(grid->array[src_vertex].head == NULL){
    grid->array[src_vertex].head = newAdjListNode(src_vertex_addr);
    }

    //traverse to the end of the list
    struct AdjListNode *traversal_ptr = grid->array[src_vertex].head;
    struct AdjListNode *prev_ptr = traversal_ptr;
    while(traversal_ptr!=NULL){
        prev_ptr = traversal_ptr;
        traversal_ptr = traversal_ptr->next;
    }
    //add new node to the end of the list
    prev_ptr->next = newAdjListNode(dest_vertex_addr);
    printf("src vertex: %d connected to: %d\n", src_vertex,dest_vertex);
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
void connectGrid(struct Grid* grid, struct Vertex* vertex_array, int grid_length, int grid_height, int total_length, int total_height){
    int i, j, current_vertex_number, next_vertex_number, prev_vertex_number, no_of_grids_length, no_of_grids_height,
    mod_result_length, height_threshold ,prev_index_number, current_index_number, next_index_number;

    no_of_grids_length = total_length/grid_length;
    no_of_grids_height = total_height/grid_height;
    printf("grid length, grid height: %d , %d \n",no_of_grids_length, no_of_grids_height);
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
                    addEdge(grid,current_vertex_number,current_vertex_number+no_of_grids_length,&vertex_array[current_index_number],&vertex_array[current_index_number+no_of_grids_length]);
                    //adds an edge with the top right vertex
                    addEdge(grid,current_vertex_number,next_vertex_number+no_of_grids_length,&vertex_array[current_index_number],&vertex_array[next_index_number+no_of_grids_length]);
                    //adds and edge with the top left vertex
                    addEdge(grid,current_vertex_number,prev_vertex_number+no_of_grids_length,&vertex_array[current_index_number],&vertex_array[prev_index_number+no_of_grids_length]);
                    //adds an edge with the vertex on the right
                    addEdge(grid,current_vertex_number,next_vertex_number,&vertex_array[current_index_number],&vertex_array[next_index_number]);
                    //adds an edge with the vertex on the left
                    addEdge(grid,current_vertex_number,prev_vertex_number,&vertex_array[current_index_number],&vertex_array[prev_index_number]);
                    //adds an edge with the bot vertex
                    addEdge(grid,current_vertex_number,current_vertex_number-no_of_grids_length,&vertex_array[current_index_number],&vertex_array[current_index_number-no_of_grids_length]);
                    //adds an edge with the bot right vertex
                    addEdge(grid,current_vertex_number,next_vertex_number-no_of_grids_length,&vertex_array[current_index_number],&vertex_array[next_index_number-no_of_grids_length]);
                    //adds and edge with the bot left vertex
                    addEdge(grid,current_vertex_number,prev_vertex_number-no_of_grids_length,&vertex_array[current_index_number],&vertex_array[prev_index_number-no_of_grids_length]);
            }
            //if vertex in the first column but not the top row or bot row
            else if(mod_result_length ==1 && !(current_vertex_number>height_threshold) && !(current_vertex_number<=no_of_grids_length)){
                //connect with top, bot ,top right, bot right, right
                //adds an edge with the top vertex
                addEdge(grid,current_vertex_number,current_vertex_number+no_of_grids_length,&vertex_array[current_index_number],&vertex_array[current_index_number+no_of_grids_length]);
                //adds an edge with the top right vertex
                addEdge(grid,current_vertex_number,next_vertex_number+no_of_grids_length,&vertex_array[current_index_number],&vertex_array[next_index_number+no_of_grids_length]);
                //adds an edge with the vertex on the right
                addEdge(grid,current_vertex_number,next_vertex_number,&vertex_array[current_index_number],&vertex_array[next_index_number]);
                //adds an edge with the bot vertex
                addEdge(grid,current_vertex_number,current_vertex_number-no_of_grids_length,&vertex_array[current_index_number],&vertex_array[current_index_number-no_of_grids_length]);
                //adds an edge with the bot right vertex
                addEdge(grid,current_vertex_number,next_vertex_number-no_of_grids_length,&vertex_array[current_index_number],&vertex_array[next_index_number-no_of_grids_length]);
            }
            //if vertex in the last column but not the top row or bot row
            else if(mod_result_length ==0 && !(current_vertex_number>height_threshold) && !(current_vertex_number<=no_of_grids_length)){
                //connect with top, bot, top left, bot left, left
                //adds an edge with the top vertex
                addEdge(grid,current_vertex_number,current_vertex_number+no_of_grids_length,&vertex_array[current_index_number],&vertex_array[current_index_number+no_of_grids_length]);
                //adds and edge with the top left vertex
                addEdge(grid,current_vertex_number,prev_vertex_number+no_of_grids_length,&vertex_array[current_index_number],&vertex_array[prev_index_number+no_of_grids_length]);
                //adds an edge with the vertex on the left
                addEdge(grid,current_vertex_number,prev_vertex_number,&vertex_array[current_index_number],&vertex_array[prev_index_number]);
                //adds an edge with the bot vertex
                addEdge(grid,current_vertex_number,current_vertex_number-no_of_grids_length,&vertex_array[current_index_number],&vertex_array[current_index_number-no_of_grids_length]);
                //adds and edge with the bot left vertex
                addEdge(grid,current_vertex_number,prev_vertex_number-no_of_grids_length,&vertex_array[current_index_number],&vertex_array[prev_index_number-no_of_grids_length]);
            }
            //if vertex in top row
            else if(current_vertex_number>height_threshold){
                //if vertex in first column
                if(mod_result_length ==1){
                    //connect with bot, bot right, bot ,right
                    //adds an edge with the bot vertex
                    addEdge(grid,current_vertex_number,current_vertex_number-no_of_grids_length,&vertex_array[current_index_number],&vertex_array[current_index_number-no_of_grids_length]);
                    //adds an edge with the bot right vertex
                    addEdge(grid,current_vertex_number,next_vertex_number-no_of_grids_length,&vertex_array[current_index_number],&vertex_array[next_index_number-no_of_grids_length]);
                    //adds an edge with the vertex on the right
                    addEdge(grid,current_vertex_number,next_vertex_number,&vertex_array[current_index_number],&vertex_array[next_index_number]);
                }
                //if vertex in last column
                else if(mod_result_length==0){
                    //connect with bot, bot left, left
                    //adds an edge with the bot vertex
                    addEdge(grid,current_vertex_number,current_vertex_number-no_of_grids_length,&vertex_array[current_index_number],&vertex_array[current_index_number-no_of_grids_length]);
                    //adds and edge with the bot left vertex
                    addEdge(grid,current_vertex_number,prev_vertex_number-no_of_grids_length,&vertex_array[current_index_number],&vertex_array[prev_index_number-no_of_grids_length]);
                    //adds an edge with the vertex on the left
                    addEdge(grid,current_vertex_number,prev_vertex_number,&vertex_array[current_index_number],&vertex_array[prev_index_number]);
                }
                //if vertex in top row but not in the first column or last column
                else{
                    //connect with bot, bot left, bot right, right, left
                    //adds an edge with the bot vertex
                    addEdge(grid,current_vertex_number,current_vertex_number-no_of_grids_length,&vertex_array[current_index_number],&vertex_array[current_index_number-no_of_grids_length]);
                    //adds and edge with the bot left vertex
                    addEdge(grid,current_vertex_number,prev_vertex_number-no_of_grids_length,&vertex_array[current_index_number],&vertex_array[prev_index_number-no_of_grids_length]);
                    //adds an edge with the bot right vertex
                    addEdge(grid,current_vertex_number,next_vertex_number-no_of_grids_length,&vertex_array[current_index_number],&vertex_array[next_index_number-no_of_grids_length]);
                    //adds an edge with the vertex on the right
                    addEdge(grid,current_vertex_number,next_vertex_number,&vertex_array[current_index_number],&vertex_array[next_index_number]);
                    //adds an edge with the vertex on the left
                    addEdge(grid,current_vertex_number,prev_vertex_number,&vertex_array[current_index_number],&vertex_array[prev_index_number]);
                }
            }
            //if vertex in bot row
            else if(current_vertex_number<=no_of_grids_length){
                //if vertex in first column
                if(mod_result_length ==1){
                    //connect with top, top right, right
                    //adds an edge with the top vertex
                    addEdge(grid,current_vertex_number,current_vertex_number+no_of_grids_length,&vertex_array[current_index_number],&vertex_array[current_index_number+no_of_grids_length]);
                    //adds an edge with the top right vertex
                    addEdge(grid,current_vertex_number,next_vertex_number+no_of_grids_length,&vertex_array[current_index_number],&vertex_array[next_index_number+no_of_grids_length]);
                    //adds an edge with the vertex on the right
                    addEdge(grid,current_vertex_number,next_vertex_number,&vertex_array[current_index_number],&vertex_array[next_index_number]);
                }
                //if vertex in last column
                else if(mod_result_length==0){
                    //connect with top, top left, left
                    //adds an edge with the top vertex
                    addEdge(grid,current_vertex_number,current_vertex_number+no_of_grids_length,&vertex_array[current_index_number],&vertex_array[current_index_number+no_of_grids_length]);
                    //adds and edge with the top left vertex
                    addEdge(grid,current_vertex_number,prev_vertex_number+no_of_grids_length,&vertex_array[current_index_number],&vertex_array[prev_index_number+no_of_grids_length]);
                    //adds an edge with the vertex on the left
                    addEdge(grid,current_vertex_number,prev_vertex_number,&vertex_array[current_index_number],&vertex_array[prev_index_number]);
                }
                //if vertex in bot row but not in the first column or last column
                else{
                    //connect with top, top left, top right, right, left
                    //adds an edge with the top vertex
                    addEdge(grid,current_vertex_number,current_vertex_number+no_of_grids_length,&vertex_array[current_index_number],&vertex_array[current_index_number+no_of_grids_length]);
                    //adds and edge with the top left vertex
                    addEdge(grid,current_vertex_number,prev_vertex_number+no_of_grids_length,&vertex_array[current_index_number],&vertex_array[prev_index_number+no_of_grids_length]);
                    //adds an edge with the top right vertex
                    addEdge(grid,current_vertex_number,next_vertex_number+no_of_grids_length,&vertex_array[current_index_number],&vertex_array[next_index_number+no_of_grids_length]);
                    //adds an edge with the vertex on the right
                    addEdge(grid,current_vertex_number,next_vertex_number,&vertex_array[current_index_number],&vertex_array[next_index_number]);
                    //adds an edge with the vertex on the left
                    addEdge(grid,current_vertex_number,prev_vertex_number,&vertex_array[current_index_number],&vertex_array[prev_index_number]);
                }
            }

        }

    }
}


/* to do:

    1. Create linked list, stack and queue functions
    2. Design graph for obstacle and obstacle course
    3. Complete simple Uniform cost search algorithm
    4. Update the UCS to A* search

    Use linked list to create the edges between vertices

    inputs: coordinates of the obstacles(xStartPos, yStartPos)

    Graph design of the obstacle course
    Graph - edges (pointers to other vertices)
            Vertices: a strut
                variables inside the strut:
                int vertexNumber
                int xStartPos  (left of the vertex)
                int xEndPos    (right of the vertex)
                int yStartPos  (top of the vertex)
                int yEndPos   (bottom of the vertex)
                Array of 8 edges (use array to store the neighbours' vertex number)
                gCost   (distance from current node to start node)
                hCost   (estimated distance from current node to end node)
                boolean isObstacle (check if the vertex should be traversed)

    Graph creation method
    variables:
    gridColumnCount //refers to number of columns in the grid
    gridRowCount //refers to number of rows in the grid

    //VERTEX CREATION
    //vertex starts from 1 to make calculations easier when using modulo later
    generateGrid(array of coordinates for obstacles,gridColumnCount,gridRowCount,gridLength,gridHeight){
        totalNumberOfVertices = gridColumnCount * gridRowCount;
        vertices vertexArray[totalNumberOfVertices];
        int i, j, currentVertex;
        for(i=1;i<=gridRowCount;i++){
                for(j=1;j<=gridColumnCount;j++{
                currentVertex = j*i;
                currentIndex = j*i-1;
                vertexArray[currentIndex].vertexNumber =i*j;
                vertexArray[currentIndex].xStartPos = currentIndex * gridLength;
                vertexArray[currentIndex].xEndPos = gridLength + currentIndex * gridLength;
                vertexArray[currentIndex].yStartPos = currentIndex * gridHeight;
                vertexArray[currentIndex].yEndPos = gridHeight + currentIndex * gridHeight;

                //if obstacle coordinates falls within the vertex range set vertex to be an obstacle
                if(obstacleCoordinates >vertexArray[currentIndex].xStartPos && obstacleCoordinates < vertexArray[currentIndex].xEndPos
                   && obstacleCoordinate>vertexArray[currentIndex].yStartPos && obstacleCoordinates < vertexArray[currentIndex].yEndPos){
                vertexArray[currentIndex].isObstacle = true;
                }
                gCost = 0;//to be calculated later when u select start vertex during the search
                hCost = 0;//to be calculated later when u select target vertex during the search
                }
        }
    }

    adjacencyList:
    array of pointers pointing to vertices, makes use of linked lists
      vertex
      pointer to the next linked list element
      weight(fcost)

    //EDGE CREATION
    edge creation algorithm to generate edges between the vertices
    edges create will be 2 way (undirected)
    current vertex+grid length = the vertex located above it
    in order to get generate all edges around 1 vertex:


        vertex+1 (right of current vertex)(NOTE: this line does not apply to vertices on the last column),
        vertex+gridColumnCount (above current vertex)(NOTE: this line does not apply to vertices on the last/topmost row),
        vertex+gridColumnCount+1 (diagonally top right of current vertex)(NOTE: this line does not apply to vertices on the last column),
        vertex+gridColumnCount-1 (diagonally top left of current vertex) (NOTE: this line does not apply to vertices on the first column)

        PSUEDO CODE:
        int i=0;
        for(i=0;i<gridRowCount;i++){
            if(vertexNumber%gridColumnCount==0 && vertexNumber/Height>gridRowCount-1){ //vertex located on the last column and topmost row
                add edge between currentVertex and currentVertex+gridColumnCount //add edge with the vertex above it
            }
        }



    //SEARCH ALGORITHM



    int calculateGCost(startVertex){
        xDistance = abs(vertexArray[currentIndex].xStartPos - vertexArray[targetNode].xStartPos);
        yDistance = abs(vertexArray[currentIndex].yStartPos - vertexArray[targetNode].yStartPos);
        return ;
    }

    int calculateHCost(TargetNode){
        xDistance = abs(vertexArray[currentIndex].xStartPos - vertexArray[targetNode].xStartPos);
        yDistance = abs(vertexArray[currentIndex].yStartPos - vertexArray[targetNode].yStartPos);
        return ;
    }


    Variables
    1. Array to store path taken to reach the goal (node number)
    2.

    Returns a path for order of goal node traversal in the form of a linked list
    simpleSearch(startNodeCoordinates, goalNodeCoordinates, graph){






        }


    }
*/
