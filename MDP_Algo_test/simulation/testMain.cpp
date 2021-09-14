#include "component.h"
#include "config.h"
#include <iostream>

using namespace std;

int main(){
    vector<Obstacle> obstacles;
    obstacles.push_back(Obstacle(1, 2, 4, 0));
    // obstacles.push_back(Obstacle(2, 2, 11, 90));
    // obstacles.push_back(Obstacle(6, 5, 1, 270));
    // obstacles.push_back(Obstacle(3, 10, 9, -100));
    // obstacles.push_back(Obstacle(6, 13, 14, 90));
    Map map(obstacles);
    map.printMap();
    map.findVertexByCoor(15.0, 10)->printVertex();
    map.findVertexByGrid(10, 3)->printVertex();
    cout << map.isValidCoor(0, 0) << endl;
    cout << map.isValidGrid(1, 300000) << endl;
}