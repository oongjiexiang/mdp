#ifndef UTILITY_H
#define UTILITY_H

#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <float.h>

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
        bool is_obstacle;
        bool is_border;
        bool visited;
        Vertex* prev_vertex; //reset each time
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