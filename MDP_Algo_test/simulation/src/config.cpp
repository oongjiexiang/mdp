#include <cmath>
#include <stdexcept>
#include <iostream>
#include "component.h"
#include "config.h"


using namespace std;

Map::Map(int distFromBorderAllowed): distFromBorderAllowed(distFromBorderAllowed){
    grids.resize(X_GRID_COUNT + 2*distFromBorderAllowed, vector<Vertex*>(Y_GRID_COUNT  + 2*distFromBorderAllowed));
    //initialize all the variables for all vertices of the graph
    for(int i=0; i < X_GRID_COUNT + 2*distFromBorderAllowed; i++){
        for(int j=0; j < Y_GRID_COUNT + 2*distFromBorderAllowed; j++){
            grids[i][j] = new Vertex(i - distFromBorderAllowed, j - distFromBorderAllowed);
        }
    }
}

Map::Map(vector<Obstacle> obstacles, int distFromBorderAllowed): Map::Map(distFromBorderAllowed){
    int boundaryGridCount = (int)(ceil(BOUNDARY_SIZE/UNIT_LENGTH));
    for(int i = 0; i < obstacles.size(); i++){
        addObstacle(&obstacles[i]);
    }
}

void Map::addObstacle(Obstacle* o){
    o->xGrid = o->xGrid + distFromBorderAllowed;
    o->yGrid = o->yGrid + distFromBorderAllowed;
    obstacles.push_back(*o);
    int boundaryGridCount = (int)(ceil(BOUNDARY_SIZE/UNIT_LENGTH));
    grids[o->xGrid + distFromBorderAllowed][o->yGrid + distFromBorderAllowed]->is_obstacle = true;
    for(int j = -boundaryGridCount; j <= boundaryGridCount; j++){
        for(int k = -boundaryGridCount; k <= boundaryGridCount; k++){
            try{
                grids.at(o->xGrid + j + distFromBorderAllowed).at(o->yGrid + k + distFromBorderAllowed)->is_border = 
                grids.at(o->xGrid + j + distFromBorderAllowed).at(o->yGrid + k + distFromBorderAllowed)->is_obstacle? false: true;
            }
            catch(out_of_range& e){}
        }
    }
}

Vertex* Map::getVertex(int xGrid, int yGrid){
    return grids[xGrid + distFromBorderAllowed][yGrid + distFromBorderAllowed];
}

/*
// JX comment out (13/9/2021) as I am assuming that obstacles will be placed in square grids precisely. If not, need calculations
//adds an obstacles and the borders of the obstacles into the grid given the array of obstacles
void Map::add_obstacle(vector<Obstacle> obstacleList){
    int i,j;
    float obstacle_x_left, obstacle_x_right, obstacle_y_high, obstacle_y_low, border_x_left, border_x_right, border_y_high, border_y_low;
    float obstacle_length = OBSTACLE_LENGTH/2;      // JX: var here means half of obstacle length to find the left, right, high, low coordinates (for loop below)
    float boundary_size = BOUNDARY_SIZE;

    // 1. register the list of obstacles into this grid map
    obstacles.insert(obstacleList.end(), obstacleList.begin(), obstacleList.end());

    // 2. mark applicable Vertices (ie Grids) as blocked with obstacle (is_obstacle) or a virtual obstacle (is_blocked)
    //loop through the obstacle array
    for(i = 0; i < obstacleList.size(); i++){
        //find the 4 points of the obstacle
        obstacle_x_left = obstacleList[i].x_center - obstacle_length;
        obstacle_x_right = obstacleList[i].x_center + obstacle_length;
        obstacle_y_high = obstacleList[i].y_center + obstacle_length;
        obstacle_y_low = obstacleList[i].y_center - obstacle_length;
        printf("Obstacle : (%.1f, %.1f) to (%.1f, %.1f)",obstacle_x_left,obstacle_y_low,obstacle_x_right,obstacle_y_high);

        //find the 4 points of the border
        border_x_left = obstacleList[i].x_center - boundary_size;
        border_x_right = obstacleList[i].x_center + boundary_size;
        border_y_high = obstacleList[i].y_center + boundary_size;
        border_y_low = obstacleList[i].y_center - boundary_size;

        
        Vertex bottomLeft = findVertexByCoor(obstacle_x_left, obstacle_y_low);
        Vertex topRight = findVertexByCoor(obstacle_x_right, obstacle_y_high);
        for(int i = bottomLeft.yGrid; i <= topRight.yGrid; i++){
            for(int j = bottomLeft.xGrid; j <= topRight.xGrid; j++){
                grids[i][j].is_obstacle = true;
            }
        }
        bottomLeft = findVertexByCoor(border_x_left, border_y_low);
        topRight = findVertexByCoor(border_x_right, border_y_high);
        for(int i = bottomLeft.yGrid; i <= topRight.yGrid; i++){
            for(int j = bottomLeft.xGrid; j <= topRight.xGrid; j++){
                grids[i][j].is_border = grids[i][j].is_obstacle? false: true;
            }
        }

    }
}
*/

//search for a vertex given the x and y coordinates and returns a pointer to the vertex
Vertex* Map::findVertexByCoor(float x_center, float y_center){
    int xGrid = (int)(ceil(x_center/UNIT_LENGTH)) - 1;
    int yGrid = (int)(ceil(y_center/UNIT_LENGTH)) - 1;
    return grids[xGrid + distFromBorderAllowed][yGrid + distFromBorderAllowed];
}

Vertex* Map::findVertexByGrid(int xGrid, int yGrid){
    return grids[xGrid + distFromBorderAllowed][yGrid + distFromBorderAllowed];
}

bool Map::isValidCoor(float x_center, float y_center){
    return (x_center < AREA_LENGTH + distFromBorderAllowed*UNIT_LENGTH && y_center < AREA_LENGTH + distFromBorderAllowed*UNIT_LENGTH 
    && x_center >= -distFromBorderAllowed*UNIT_LENGTH && y_center >= -distFromBorderAllowed*UNIT_LENGTH);
}

bool Map::isValidGrid(int xGrid, int yGrid){
    return (yGrid >= -distFromBorderAllowed && xGrid >= -distFromBorderAllowed && 
        yGrid < Y_GRID_COUNT + distFromBorderAllowed && 
        xGrid < X_GRID_COUNT + distFromBorderAllowed);
}

bool Map::isAvailableGrid(int xGrid, int yGrid){
    if(isValidGrid(xGrid, yGrid)){
        Vertex* v = grids[xGrid + distFromBorderAllowed][yGrid + distFromBorderAllowed];
        return !v->is_border && !v->is_obstacle;
    }
    return false;
}

// get vertex array
vector<vector<Vertex*>>& Map::getVertexArray(){
    return grids;
}

vector<Obstacle>& Map::getObstacles(){
    return obstacles;
}

// debug
void Map::printMap(){
    cout << "---------------Map----------------" << endl;
    for(int i = Y_GRID_COUNT + distFromBorderAllowed - 1; i >= 0; i--){
        for(int j = 0; j < X_GRID_COUNT + distFromBorderAllowed; j++){
            if(grids[j + distFromBorderAllowed][i + distFromBorderAllowed]->is_obstacle) cout << "O";
            else if(grids[j + distFromBorderAllowed][i + distFromBorderAllowed]->is_border) cout << "B";
            else if(grids[j + distFromBorderAllowed][i + distFromBorderAllowed]->safe) cout << "^";
            else cout << " ";
        }
        cout << endl;
    }
    cout << "---------------End----------------" << endl << endl;
}
