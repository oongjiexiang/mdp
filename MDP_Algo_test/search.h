#ifndef SEARCH_H
#define SEARCH_H

#include "tripPlan.h"
#include "component.h"
#include "config.h"

class AStarSearch{
    public:
        //add obstacle avoidance and stop search once the goal has been found
        // --- not done ---
        Vertex* a_star_search(vector<vector<Vertex>> vertex_array, Vertex* src_vertex, Vertex* target_vertex, int TOTAL_GRID_COUNT);
        
        // --- done ---
        //prints the path from a star search backwards, to implement forward, save the id into an array starting from the last element or use a stack
        void print_path(Vertex* src_vertex, Vertex* target_vertex);
        //updates the h_cost for every vertex using Euclidean distance from previous vertex
        double calculate_g_cost(Vertex* prev_vertex, Vertex* current_vertex);
        //updates the h_cost for every vertex using Euclidean distance from target
        void update_h_cost(vector<vector<Vertex>> vertex_array, Vertex* src_vertex, Vertex* target_vertex, int TOTAL_GRID_COUNT);
};

#endif