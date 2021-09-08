#ifndef COMPONENT_H
#define COMPONENT_H

#include <float.h>

const double START_COST = DBL_MAX;
const double UNIT_LENGTH = 10;
const int ROW_COUNT = 200/UNIT_LENGTH;
const int COLUMN_COUNT = 200/UNIT_LENGTH; 

//used for obstacle avoidance
class Vertex{
    public: 
        double g_cost, h_cost;
        int row, column;
        double x_right, x_left, y_high, y_low;
        bool is_obstacle, is_border, visited;
        Vertex* prev_vertex; //reset each time
        Vertex();
        Vertex(int row, int column);
        void reset();
        void printVertex();
};


//used to read the obstacles
class Obstacle{
    public:
        int id;
        double x_center;
        double y_center;
        double face_direction;      // degree of the image's direction
        bool is_seen;
        Obstacle(int id, double x_center, double y_center, double face_direction);
        void printObstacle();
};


//used to track the robot's simulated location
class Robot{
    public:
        double x_right, x_left, y_high, y_low;
        double x_center, y_center;
        double face_direction;  // theta in range (-pi, pi]
        void printRobot();
};

#endif