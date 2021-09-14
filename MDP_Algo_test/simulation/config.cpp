#include <cmath>
#include <stdexcept>
#include <iostream>
#include "component.h"
#include "config.h"


using namespace std;

Map::Map(){
    grids.resize(ROW_COUNT, vector<Vertex*>(COLUMN_COUNT));
    //initialize all the variables for all vertices of the graph
    for(int i=0; i < ROW_COUNT; i++){
        for(int j=0; j < COLUMN_COUNT; j++){
            grids[i][j] = new Vertex(i, j);
        }
    }
}

Map::Map(vector<Obstacle> obstacles): Map::Map(){
    this->obstacles = obstacles;
    int boundaryGridCount = (int)(ceil(BOUNDARY_SIZE/UNIT_LENGTH));
    for(int i = 0; i < obstacles.size(); i++){
        Obstacle o = obstacles[i];
        grids[o.row][o.column]->is_obstacle = true;
        for(int j = -boundaryGridCount; j <= boundaryGridCount; j++){
            for(int k = -boundaryGridCount; k <= boundaryGridCount; k++){
                grids[o.row + j][o.column + k]->is_border = grids[o.row + j][o.column + k]->is_obstacle? false: true;
            }
        }
    }
}

/*
// JX comment out (13/9/2021) as I am assuming that obstacles will be placed in square grids precisely. If not, need calculations
//adds an obstacles and the borders of the obstacles into the grid given the array of obstacles
void Map::add_obstacle(vector<Obstacle> obstacleList){
    int i,j;
    double obstacle_x_left, obstacle_x_right, obstacle_y_high, obstacle_y_low, border_x_left, border_x_right, border_y_high, border_y_low;
    double obstacle_length = OBSTACLE_LENGTH/2;      // JX: var here means half of obstacle length to find the left, right, high, low coordinates (for loop below)
    double boundary_size = BOUNDARY_SIZE;

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
        for(int i = bottomLeft.row; i <= topRight.row; i++){
            for(int j = bottomLeft.column; j <= topRight.column; j++){
                grids[i][j].is_obstacle = true;
            }
        }
        bottomLeft = findVertexByCoor(border_x_left, border_y_low);
        topRight = findVertexByCoor(border_x_right, border_y_high);
        for(int i = bottomLeft.row; i <= topRight.row; i++){
            for(int j = bottomLeft.column; j <= topRight.column; j++){
                grids[i][j].is_border = grids[i][j].is_obstacle? false: true;
            }
        }

    }
}
*/

//search for a vertex given the x and y coordinates and returns a pointer to the vertex
Vertex* Map::findVertexByCoor(double x_center, double y_center){
    int row = (int)(ceil(y_center/UNIT_LENGTH)) - 1;
    int col = (int)(ceil(x_center/UNIT_LENGTH)) - 1;
    return grids[row][col];
}

Vertex* Map::findVertexByGrid(int row, int col){
    return grids[row][col];
}

bool Map::isValidCoor(double x_center, double y_center){
    return (x_center < AREA_LENGTH && y_center < AREA_LENGTH 
    && x_center >= 0 && y_center >= 0);
}

bool Map::isValidGrid(int row, int col){
    return (row >= 0 && col >= 0 && row < ROW_COUNT && col < COLUMN_COUNT);
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
    for(int i = grids.size() - 1; i >= 0; i--){
        for(int j = 0; j < grids.size(); j++){
            if(grids[i][j]->is_obstacle) cout << "O";
            else if(grids[i][j]->is_border) cout << "B";
            else cout << " ";
        }
        cout << endl;
    }
    cout << "---------------End----------------" << endl << endl;
}
