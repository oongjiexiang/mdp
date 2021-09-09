#include <cmath>
#include <stdexcept>
#include <iostream>
#include "component.h"
#include "config.h"


using namespace std;

Map::Map(){
    grids.resize(ROW_COUNT, vector<Vertex>(COLUMN_COUNT));
    //initialize all the variables for all vertices of the graph
    for(int i=0; i < ROW_COUNT; i++){
        for(int j=0; j < COLUMN_COUNT; j++){
            grids[i][j] = Vertex(i, j); // Q: without `new`, will it be destroyed?
        }
    }
}

// experimental feature: set map from 2D matrix for GeeksforGeeks A* Search. to delete
void Map::setMap(vector<vector<int>> fullMap){
    for(int i = 0; i < fullMap.size(); i++){
        for(int j = 0; j < fullMap[i].size(); j++){
            if(fullMap[i][j] == 0){
                grids[i][j].is_obstacle = true;
            }
        }
    }
}

//reset the cost, obstacle, border, visited, and prev_vertex for all vertices, use this every time after 1 path has been found
void Map::resetMap(){
    for(int i=0; i < ROW_COUNT;i++){
        for(int j = 0; j < COLUMN_COUNT;j++){
            grids[i][j].reset();
        }
    }
}

//adds an obstacles and the borders of the obstacles into the grid given the array of obstacles
void Map::add_obstacle(vector<Obstacle> obstacleList){
    int i,j;
    double obstacle_x_left, obstacle_x_right, obstacle_y_high, obstacle_y_low, border_x_left, border_x_right, border_y_high, border_y_low;
    double obstacle_length=obstacle_length/2;      // JX: var here means half of obstacle length to find the left, right, high, low coordinates (for loop below)
    double boundary_size= boundary_size;

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

//search for a vertex given the x and y coordinates and returns a pointer to the vertex
Vertex Map::findVertexByCoor(double x_center, double y_center){
    int row = ceil(y_center/UNIT_LENGTH);
    int col = ceil(x_center/UNIT_LENGTH);
    try{
        return grids.at(row).at(col);
    }
    catch(const out_of_range &oor){
        printf("vertex could not be found.\n");
        Vertex unavailable;
        return unavailable;
    }
}

Vertex Map::findVertexByGrid(int row, int col){
    try{
        return grids.at(row).at(col);
    }
    catch(const out_of_range &oor){
        printf("vertex could not be found.\n");
        Vertex unavailable;
        return unavailable;
    }
}

bool Map::validVertex(Vertex target){
    return (target.row >= 0 && target.column >= 0
        && target.row < ROW_COUNT && target.column < COLUMN_COUNT
        && ROW_COUNT > 0 && COLUMN_COUNT > 0);
}

bool Map::vertexInRange(int row, int column){
    return (row >= 0 && column >= 0
        && row < ROW_COUNT && column < COLUMN_COUNT
        && ROW_COUNT > 0 && COLUMN_COUNT > 0);
}
// get vertex array
vector<vector<Vertex>>* Map::getVertexArray(){
    return &grids;
}
