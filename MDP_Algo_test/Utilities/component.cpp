#include "component.h"
#include <iostream>
using namespace std;

Vertex::Vertex(int xGrid, int yGrid): xGrid(xGrid), yGrid(yGrid){
    x_right = ((xGrid+1)*UNIT_LENGTH); // no -1. If there is, then when a coordinate point lies between 0.9 and 1.0, which vertex does it belong to?
    x_left = xGrid*UNIT_LENGTH;
    y_high = ((yGrid+1)*UNIT_LENGTH);   // no -1. If there is, then when a coordinate point lies between 0.9 and 1.0, which vertex does it belong to?
    y_low = yGrid*UNIT_LENGTH;
    is_obstacle = false;
    is_border = false;
    safe = false;
}
void Vertex::printVertex(){
    printf("Vertex (%d, %d) with (%.0f, %.0f) to (%.0f, %.0f):\n", xGrid, yGrid, x_left, y_low, x_right, y_high);
    printf("\t%s %s %s\n", is_border? "border": "!border", is_obstacle? "obstacle": "!obstacle", safe? "safe": "!safe");
    // printf("Vertex (%d, %d) - %d %d\n", yGrid, xGrid, is_border, is_obstacle);    // uncomment if see vital (RELEVANT) debug info
}

// Obstacles
Obstacle::Obstacle(int id, int xGrid, int yGrid, int face_direction):   // should we not feed yGrid and xGrid, since we can mathematically calculate it?
    id(id), xGrid(xGrid), yGrid(yGrid), face_direction(face_direction){
        is_seen = false;
}

void Obstacle::printObstacle(){
    printf("Obstacle: %d: (%d, %d) facing %d, %s\n", id, xGrid, yGrid, face_direction, is_seen? "seen": "not seen");
}

// Robot
Robot::Robot(int xGrid, int yGrid, float face_direction): xGrid(xGrid), yGrid(yGrid), face_direction(face_direction){
    x_center = (xGrid+0.5)*UNIT_LENGTH;
    y_center = (yGrid+0.5)*UNIT_LENGTH;
    x_right = x_center + ROBOT_OCCUPY_LENGTH/2;
    x_left = x_center - ROBOT_OCCUPY_LENGTH/2;
    y_high = y_center + ROBOT_OCCUPY_LENGTH/2;
    y_low = y_center - ROBOT_OCCUPY_LENGTH/2;
}
void Robot::printRobot(){
    printf("Robot: (%d, %d), (%.1f, %.1f) facing %.1f | bottom left (%.1f, %.1f) to top right (%.1f, %.1f) facing %.0f\n", xGrid, yGrid, x_center, y_center, face_direction,\
    x_left, y_low, x_right, y_high, face_direction);
}
