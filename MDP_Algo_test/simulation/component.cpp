#include "component.h"
#include <iostream>
using namespace std;


// Vertex
Vertex::Vertex(){   // to indicate invalid vertex
    row = column = -1;
}
Vertex::Vertex(int row, int column): row(row), column(column){
    x_right = ((column+1)*UNIT_LENGTH); // no -1. If there is, then when a coordinate point lies between 0.9 and 1.0, which vertex does it belong to?
    x_left = column*UNIT_LENGTH;
    y_high = ((row+1)*UNIT_LENGTH);   // no -1. If there is, then when a coordinate point lies between 0.9 and 1.0, which vertex does it belong to?
    y_low = row*UNIT_LENGTH;
    is_obstacle = false;
    is_border = false;
    visited = false;
}
bool Vertex::valid(Vertex target){
    if(target.row == -1) return false;
    return true;
}
void Vertex::printVertex(){
    printf("Vertex (%d, %d) with (%.0f, %.0f) to (%.0f, %.0f):\n", row, column, x_left, y_low, x_right, y_high);
    printf("\t%s %s %s\n", is_border? "border": "!border", is_obstacle? "obstacle": "!obstacle", visited? "visited": "!visited");
    // printf("Vertex (%d, %d) - %d %d\n", row, column, is_border, is_obstacle);    // uncomment if see vital (RELEVANT) debug info
}

// Obstacles
Obstacle::Obstacle(int id, int row, int column, double face_direction):   // should we not feed row and column, since we can mathematically calculate it?
    id(id), row(row), column(column), face_direction(face_direction){
        is_seen = false;
}

void Obstacle::printObstacle(){
    printf("Obstacle: %d: (%d, %d), %s\n", id, row, column, is_seen? "seen": "not seen");
}

// Robot
Robot::Robot(int row, int column, double face_direction): row(row), column(column), face_direction(face_direction){
    x_center = (column+0.5)*UNIT_LENGTH;
    y_center = (row+0.5)*UNIT_LENGTH;
    x_right = x_center + ROBOT_OCCUPY_LENGTH/2;
    x_left = x_center - ROBOT_OCCUPY_LENGTH/2;
    y_high = y_center + ROBOT_OCCUPY_LENGTH/2;
    y_low = y_center - ROBOT_OCCUPY_LENGTH/2;
}
void Robot::printRobot(){
    printf("Robot: (%d, %d), (%.1f, %.1f) facing %.1f | bottom left (%.1f, %.1f) to top right (%.1f, %.1f) facing %.0f\n", row, column, x_center, y_center, face_direction,\
    x_left, y_low, x_right, y_high, face_direction);
}
