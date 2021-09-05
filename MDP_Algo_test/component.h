#ifndef UTILITY_H
#define UTILITY_H

#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <float.h>

//---- constants (cm) -------
// JX: Zhi Yu when experimenting just modify the values here

extern double UNIT_LENGTH;
extern double ROBOT_LENGTH;
extern int ROW_COUNT;
extern int COLUMN_COUNT;
extern int TOTAL_GRID_COUNT;
extern double OBSTACLE_SIZE;  //total length of an obstacle, the obstacle is a square
extern double OBSTACLE_BOUNDARY;  // + to each side of the obstacle

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


class Goal{
    public:
        double x_right;
        double x_left;
        double y_high;
        double y_low;
        double theta;
};

//used to track the robot's simulated location
class Robot{
    public:
        double x_right;
        double x_left;
        double y_high;
        double y_low;
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
