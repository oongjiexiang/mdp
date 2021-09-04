#ifndef UTILITY_H
#define UTILITY_H

#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <float.h>

//---- constants (cm) -------
// JX: Zhi Yu when experimenting just modify the values here
double UNIT_LENGTH = 10;
double ROBOT_LENGTH = 30;
int ROW_COUNT = 200/UNIT_LENGTH;
int COLUMN_COUNT = 200/UNIT_LENGTH;
int TOTAL_GRID_COUNT = ROW_COUNT*COLUMN_COUNT;
double OBSTACLE_SIZE = 10;  //total length of an obstacle, the obstacle is a square
double OBSTACLE_BOUNDARY = 15;  // + to each side of the obstacle

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
class Vertex{
    public: 
        int id;
        int g_cost; //distance from source to current vertex
        int h_cost;
        double x_right;
        double x_left;
        double y_high;
        double y_low;
        int row;
        int column;
        bool is_obstacle;
        bool is_border;
        bool visited;
        Vertex* prev_vertex; //reset each time
        void printVertex();
};


//used to read the obstacles
class Obstacle{
    public:
        int id;
        double x_coor;
        double y_coor;
        bool is_seen;
        Obstacle(int id, double x_coor, double y_coor);
};

//used to track the robot's simulated location
class Robot{
    public:
        double x_right;
        double x_left;
        double y_high;
        double y_low;
        double x_center;
        double y_center;
        double theta;
};

//leave as int for now, once the sort function works, change int id to struct Vertex* vertex -> hcost+gcost
class QueueNode{
    public:
        Vertex* q_vertex;
        QueueNode* next;
};

//used in the search
class SortedQueue{
    public:
        QueueNode* head;
        QueueNode* tail;
        int current_size;
        SortedQueue();
        //----Queue functions----
        //add new node to queue
        void enqueue(Vertex* vertex);
        //remove a node from the head of queue and return the vertex pointer of the node that was removed
        Vertex* dequeue();
        //sorts the queue according in ascending order of g_cost
        void sort_queue();
};

#endif