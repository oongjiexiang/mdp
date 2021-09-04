#ifndef CONFIG_H
#define CONFIG_H

#include <vector>
#include "tripPlan.h"
#include "component.h"

class Map{
    public:
        Map();
        //initialize all the variables for all vertices of the graph
        void initialize_vertex(int ROW_COUNT, int COLUMN_COUNT,int UNIT_LENGTH,int total_count);

        //reset the cost, obstacle, border, visited, and prev_vertex for all vertices, use this every time after 1 path has been found
        void reset_vertex(int ROW_COUNT, int COLUMN_COUNT,int UNIT_LENGTH,int total_count);

        //adds an obstacles and the borders of the obstacles into the grid given the array of obstacles
        void add_obstacle(vector<Obstacle> obstacle_array, double obstacle_length,double boundary_size, int total_count);

        //search for a vertex given the x and y coordinates and returns a pointer to the vertex
        Vertex find_vertex(double x_coor, double y_coor, int total_count);

        // // get vertex array
        vector<vector<Vertex>> getVertexArray();
};

#endif