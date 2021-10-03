#ifndef COMPONENT_H
#define COMPONENT_H

#include <float.h>
#include <vector>

using namespace std;

// robot
const int ROBOT_INIT_Y_GRID = 1;
const int ROBOT_INIT_X_GRID = 1;
const int ROBOT_INIT_FACEDIRECTION = 90;
const float ROBOT_OCCUPY_LENGTH = 22;

// map
const float AREA_LENGTH = 200;
const float START_COST = FLT_MAX;
const float UNIT_LENGTH = 10;
const int Y_GRID_COUNT = AREA_LENGTH/UNIT_LENGTH;
const int X_GRID_COUNT = AREA_LENGTH/UNIT_LENGTH;
const float OBSTACLE_LENGTH = 10;
const float BOUNDARY_SIZE = 10;    // originally 15. Temporarily changed to 15 for Qt testing

//used for obstacle avoidance
class Vertex{
    public:
        int xGrid, yGrid;
        float x_right, x_left, y_high, y_low;
        bool is_obstacle, is_border, safe;

        Vertex(int xGrid, int yGrid);
        void printVertex();
};

//used to read the obstacles
class Obstacle{
    public:
        int id;
        int xGrid;
        int yGrid;
        float x_center;
        float y_center;
        int face_direction;      // degree of the image's direction. Int as it is used in simulation only
        bool is_seen;   // 16/9 no use currently
        Obstacle(int id, int xGrid, int yGrid, int face_direction);
        void printObstacle();
};


//used to track the robot's simulated location
class Robot{
    public:
        float x_right, x_left, y_high, y_low;
        float x_center, y_center;
        int xGrid, yGrid;
        float face_direction;  // theta in range (-pi, pi]
        Robot(int xGrid, int yGrid, float face_direction);
        void printRobot();
};

#endif