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
}
void Vertex::printVertex(){
    printf("Vertex (%d, %d) with (%.0f, %.0f) to (%.0f, %.0f):\n", xGrid, yGrid, x_left, y_low, x_right, y_high);
    printf("\t%s %s\n", is_border? "border": "!border", is_obstacle? "obstacle": "!obstacle");
}

// Obstacles
Obstacle::Obstacle(int id, int xGrid, int yGrid, int face_direction):   // should we not feed yGrid and xGrid, since we can mathematically calculate it?
    id(id), xGrid(xGrid), yGrid(yGrid), face_direction(face_direction){
        is_seen = false;
}

void Obstacle::printObstacle(){
    printf("Obstacle: %d: (%d, %d) facing %d, %s\n", id, xGrid, yGrid, face_direction, is_seen? "seen": "not seen");
}

