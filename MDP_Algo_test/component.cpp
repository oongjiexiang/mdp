#include "component.h"

Vertex::Vertex(){   // to indicate invalid vertex
    row = column = -1;
}
Vertex::Vertex(int row, int column): row(row), column(column){
    g_cost = h_cost = START_COST;
    x_right = ((column+1)*UNIT_LENGTH)-1;  // -1 because eg cell[0][0] spans from 0 to 9, not 10
    x_left = column*UNIT_LENGTH;
    y_high = ((row+1)*UNIT_LENGTH)-1;   // -1 for the same reason
    y_low = row*UNIT_LENGTH;
    is_obstacle = false;
    is_border = false;
    visited = false;
    prev_vertex = NULL;
}
void Vertex::reset(){
    g_cost = h_cost = START_COST;
    is_obstacle = is_border = visited = false;
    prev_vertex = NULL;
}
void Vertex::printVertex(){
    printf("Vertex (%d, %d) with (%.1f, %.1f) to (%.1f, %.1f):\n", row, column, x_left, y_low, x_right, y_high);
    printf("\tg = %.1f, h = %.1f. %s %s %s\n", g_cost, h_cost, is_border? "border": "!border", is_obstacle? "obstacle": "!obstacle", visited? "visited": "!visited");
}

Obstacle::Obstacle(int id, double x_center, double y_center, double face_direction): id(id), x_center(x_center), y_center(y_center), face_direction(face_direction){}

void Obstacle::printObstacle(){
    printf("Obstacle: %d: (%.1f, %.1f), %s\n", id, x_center, y_center, is_seen? "seen": "not seen");
}

void Robot::printRobot(){
    printf("Robot: (%.1f, %.1f) facing %.1f | bottom left (%.1f, %.1f) to top right (%.1f, %.1f)\n", x_center, y_center, face_direction,\
    x_left, y_low, x_right, y_high);
}

