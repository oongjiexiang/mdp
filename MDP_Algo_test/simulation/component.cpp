#include "component.h"
#include <iostream>
using namespace std;


// Vertex
Vertex::Vertex(){   // to indicate invalid vertex
    row = column = -1;
}
Vertex::Vertex(int row, int column): row(row), column(column){
    x_right = ((column+1)*UNIT_LENGTH)-1;  // -1 because eg cell[0][0] spans from 0 to 9, not 10
    x_left = column*UNIT_LENGTH;
    y_high = ((row+1)*UNIT_LENGTH)-1;   // -1 for the same reason
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
    printf("Vertex (%d, %d) with (%.1f, %.1f) to (%.1f, %.1f):\n", row, column, x_left, y_low, x_right, y_high);
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
Robot::Robot(int row, int column, double face_direction): row(row), column(column), face_direction(face_direction){}
void Robot::printRobot(){
    printf("Robot: (%.1f, %.1f) facing %.1f | bottom left (%.1f, %.1f) to top right (%.1f, %.1f)\n", x_center, y_center, face_direction,\
    x_left, y_low, x_right, y_high);
}
