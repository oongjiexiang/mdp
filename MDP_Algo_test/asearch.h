#ifndef ASEARCH_H
#define ASEARCH_H

#include "config.h"
#include "math.h"
#include <vector>
#include <iostream>
#include <tuple>

// Creating a shortcut for tuple<int, Vertex> type
typedef tuple<double, int, int> Tuple;
typedef pair<double, vector<Action>> SearchResult;

class aStar{
    Map* grid;

    // check whether cell is not blocked
    bool isUnBlocked(const Vertex& vertex); 
    
    bool isDestination(const Vertex& position, const Vertex& dest);
    
    // A Utility Function to calculate the 'h' heuristics.
    double calculateHValue(const Vertex& src, const Vertex& dest);  // tochange
    
    // Encapsulate g cost calculation
    double calculateGValue(Vertex& cur);
    
    // A Utility Function to trace the path from the source to destination
    void tracePath(const vector<vector<Vertex>>& cellDetails, Vertex dest, SearchResult& searchResult);
    
    public:
        aStar();
        aStar(vector<vector<int>> fullMap, vector<Obstacle> obstacles);
        void search(Vertex& src, Vertex& dest, SearchResult& searchResult);
};

#endif

