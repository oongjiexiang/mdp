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

        // get the map from Qt UI simulator
        Map(vector<Obstacle> obstacles);

        //search for a vertex given the x and y coordinates and returns a pointer to the vertex
        Vertex* findVertexByCoor(double x_center, double y_center);

        // search for a vertex given the row and column
        Vertex* findVertexByGrid(int row, int column);

        bool isValidCoor(double x_center, double y_center);

        bool isValidGrid(int row, int col);

        // get vertex array
        vector<vector<Vertex*>>& getVertexArray();

        vector<Obstacle>& getObstacles();

        // debug
        void printMap();
};

#endif
