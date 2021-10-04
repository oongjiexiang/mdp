#ifndef CONFIG_H
#define CONFIG_H

#include <vector>
#include "component.h"

using namespace std;

class Map{
    private:
        vector<vector<Vertex*>> grids;
        vector<Obstacle> obstacles;
    public:
        Map();

        // in case the robot must go outside the border
        Map(vector<Obstacle> obstacles, int distFromBorderAllowed);
        
        // get the map from Qt UI simulator
        Map(vector<Obstacle> obstacles);

        //search for a vertex given the x and y coordinates and returns a pointer to the vertex
        Vertex* findVertexByCoor(float x_center, float y_center);

        // search for a vertex given the yGrid and xGridumn
        Vertex* findVertexByGrid(int xGrid, int yGrid);

        void addObstacle(Obstacle* o);

        bool isValidCoor(float x_center, float y_center);

        bool isValidGrid(int xGrid, int yGrid);

        bool isAvailableGrid(int xGrid, int yGrid);
        // get vertex array
        vector<vector<Vertex*>>& getVertexArray();

        vector<Obstacle>& getObstacles();

        // debug
        void printMap();
};

#endif
