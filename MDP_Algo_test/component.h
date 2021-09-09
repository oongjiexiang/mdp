#ifndef COMPONENT_H
#define COMPONENT_H

#include <float.h>

const double START_COST = DBL_MAX;
const double UNIT_LENGTH = 10;
const int ROW_COUNT = 200/UNIT_LENGTH;
const int COLUMN_COUNT = 200/UNIT_LENGTH; 
const double OBSTACLE_LENGTH = 10;
const double BOUNDARY_SIZE = 15;

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
        bool valid(Vertex target);
        void reset();
        void printVertex();
};

// to illustrate each step. Will add more attributes when necessary
class Action{
    int row, column;
    double face_direction;
    public:
        Action(int row, int column, double face_direction);
};

//used to read the obstacles
class Obstacle{
    public:
        int id;
        int row;
        int column;
        double x_center;
        double y_center;
        double face_direction;      // degree of the image's direction
        bool is_seen;
        Obstacle(int id, double x_center, double y_center, int row, int column, double face_direction);
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