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
    return (x_center < AREA_LENGTH + (distFromBorderAllowed - 0.5)*UNIT_LENGTH && y_center < AREA_LENGTH + (distFromBorderAllowed - 0.5)*UNIT_LENGTH 
    && x_center >= -(distFromBorderAllowed - 0.5)*UNIT_LENGTH && y_center >= -(distFromBorderAllowed - 0.5)*UNIT_LENGTH);
}

bool Map::isValidGrid(int xGrid, int yGrid){    // 7 Oct 2021: to disable robot center to be on the edge during search, will return false
    return (yGrid > -distFromBorderAllowed && xGrid > -distFromBorderAllowed && 
        yGrid < Y_GRID_COUNT + distFromBorderAllowed - 1 && 
        xGrid < X_GRID_COUNT + distFromBorderAllowed - 1);
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
            if(grids[j][i]->is_obstacle) cout << "O";
            else if(grids[j][i]->is_border) cout << "B";
            else cout << " ";
        }
        cout << endl;
    }
    cout << "---------------End----------------" << endl << endl;
}
