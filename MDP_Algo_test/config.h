#ifndef CONFIG_H
#define CONFIG_H

#include <vector>
#include "component.h"

using namespace std;

class Map{
    public:
        vector<vector<Vertex>> grids;
        vector<Obstacle> obstacles;
        Map();
        
        //reset the cost, obstacle, border, visited, and prev_vertex for all vertices, use this every time after 1 path has been found
        void resetMap();

        //adds an obstacles and the borders of the obstacles into the grid given the array of obstacles
        void add_obstacle(vector<Obstacle> obstacleList, double obstacle_length,double boundary_size, int total_count);

        //search for a vertex given the x and y coordinates and returns a pointer to the vertex
        Vertex findVertex(double x_center, double y_center);

        // get vertex array
        vector<vector<Vertex>>* getVertexArray();
};

#endif