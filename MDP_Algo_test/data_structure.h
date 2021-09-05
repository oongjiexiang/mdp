#ifndef DATA_STRUCTURE_H
#define DATA_STRUCTURE_H

//----struct----
//used for obstacle avoidance
typedef struct _vertex{
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
    struct _vertex* prev_vertex; //reset each time
} Vertex;


//used to read the obstacles
typedef struct _obstacle{
    int id;
    double x_coor;
    double y_coor;
} Obstacle;


typedef struct _goal{
    double x_right;
    double x_left;
    double y_high;
    double y_low;
    double theta;
} Goal;

//used to track the robot's simulated location
typedef struct _robot{
    double x_right;
    double x_left;
    double y_high;
    double y_low;
    double theta;
} Robot;

//leave as int for now, once the sort function works, change int id to struct Vertex* vertex -> hcost+gcost
typedef struct _queue_Node{
    Vertex* q_vertex;
    struct _queue_Node* next;
} Queue_Node;

//used in the search
typedef struct _sorted_Queue{
    Queue_Node* head;
    Queue_Node* tail;
    int current_size;
} Sorted_Queue;

#endif
