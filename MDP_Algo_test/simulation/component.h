#ifndef COMPONENT_H
#define COMPONENT_H

#include <float.h>
#include <vector>

using namespace std;

const double AREA_LENGTH = 200;
const double START_COST = DBL_MAX;
const double UNIT_LENGTH = 10;
const int ROW_COUNT = AREA_LENGTH/UNIT_LENGTH;
const int COLUMN_COUNT = AREA_LENGTH/UNIT_LENGTH; 
const double OBSTACLE_LENGTH = 10;
const double BOUNDARY_SIZE = 10;    // originally 15. Temporarily changed to 15 for Qt testing

//used for obstacle avoidance
class Vertex{
    public: 
        int row, column;
        double x_right, x_left, y_high, y_low;
        bool is_obstacle, is_border, visited;
        
        Vertex();
        Vertex(int row, int column);
        bool valid(Vertex target);
        void printVertex();
};
class State{
    public:
        Vertex* position;
        vector<Obstacle> obstaclesSeen;
        double face_direction;
        double gCost, hCost;
        State* prevState; // should add an action here
        State(Vertex position, vector<Obstacle> obstaclesSeen, double face_direction, State* prevState);
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
        Obstacle(int id, int row, int column, double face_direction);
        void printObstacle();
};


//used to track the robot's simulated location
class Robot{
    public:
        double x_right, x_left, y_high, y_low;
        double x_center, y_center;
        int row, column;
        double face_direction;  // theta in range (-pi, pi]
        Robot(int row, int column, double face_direction);
        void printRobot();
};

#endif
