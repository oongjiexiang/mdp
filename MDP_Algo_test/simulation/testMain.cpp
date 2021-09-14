#include "component.h"
#include <iostream>

using namespace std;

int main(){
    Vertex v(1, 34);
    Obstacle o(2, 1, 3, -90);
    Robot r(1, 1, 100);
    v.printVertex();
    o.printObstacle();
    r.printRobot();
}