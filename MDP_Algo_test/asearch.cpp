#include "config.h"
#include "asearch.h"
#include "math.h"
#include <vector>
#include <cstring>
#include <iostream>
#include <queue>
#include <stack>
#include <tuple>
using namespace std;

// Creating a shortcut for tuple<int, Vertex> type
typedef tuple<double, int, int> Tuple;

bool aStar::isUnBlocked(const Vertex& vertex){ // check whether cell is not blocked
    // Returns true if the cell is not blocked else false
    int row = vertex.row;
    int column = vertex.column;
    Vertex v = grid->findVertexByGrid(row, column);
    return (!v.is_border && !v.is_obstacle);
}

bool aStar::isDestination(const Vertex& position, const Vertex& dest){
    return (position.row == dest.row && position.column == dest.column);
}

// A Utility Function to calculate the 'h' heuristics.
double aStar::calculateHValue(const Vertex& src, const Vertex& dest){  // tochange
    // h is estimated with the two points distance formula
    return sqrt(pow((src.row - dest.row), 2.0) + pow((src.column - dest.column), 2.0));
}

// Encapsulate g cost calculation
double aStar::calculateGValue(Vertex& cur){    // to change
    return cur.g_cost + 1.0; 
}
// A Utility Function to trace the path from the source to destination
void aStar::tracePath(const vector<vector<Vertex>>& cellDetails, Vertex dest, SearchResult& searchResult){
    stack<Vertex> Path;
    int row = dest.row;
    int col = dest.column;
    Vertex* next_node = &dest;
    vector<Action> actions;

    // trace from destination back to source
    do {
        Path.push(*next_node);
        next_node = cellDetails[row][col].prev_vertex;
        row = next_node->row;
        col = next_node->column;
    } while (cellDetails[row][col].prev_vertex != next_node);   // at source, prev_vertex/parent = next_node
    Path.emplace(*next_node);

    // Package into <distance: double, actions: vector<Action>> and modify the memory
    while(!Path.empty()){
        Vertex p = Path.top();
        Path.pop();
        Action a(p.row, p.column, 0);   // need to change 3rd param -- face_direction when permutation is done
        actions.push_back(a);
    }
    searchResult = SearchResult(next_node->g_cost, actions);

    // printf("\nThe Path is ");    // for printing the full path
    // while (!Path.empty()) {
    //     Vertex p = Path.top();
    //     Path.pop();
    //     printf("-> (%d,%d) ", p.row, p.column);
    // }
}

aStar::aStar(){
    grid = new Map();
}
aStar::aStar(vector<vector<int>> fullMap, vector<Obstacle> obstacles){
    grid = new Map();
    grid->setMap(fullMap);
    grid->add_obstacle(obstacles);
}
void aStar::search(Vertex& src, Obstacle& dest, SearchResult& searchResult){
    // If the source is out of range
    if (!grid->validVertex(src)) {
        printf("Source is invalid\n");
        return;
    }

    // If the destination is out of range
    Vertex destVertex(dest.row, dest.column);
    if (!grid->validVertex(destVertex)) {
        printf("Destination is invalid\n");
        return;
    }

    // Either the source or the destination is blocked
    if (!isUnBlocked(src) || !isUnBlocked(destVertex)) {
        printf("Source or the destination is blocked\n");
        return;
    }

    // If the destination cell is the same as source cell
    if (isDestination(src, destVertex)) {
        printf("We are already at the destination\n");
        return;
    }

    // Create a closed list and initialise it to false ie no cell is visited yet
    bool closedList[ROW_COUNT][COLUMN_COUNT];
    memset(closedList, false, sizeof(closedList));

    // Create a copy of grid and perform A* on it. If we keep resetting, it may cost O(n^2) time for each reset
    // If we just create a copy, we occupy twice the memory of grid, but it is a local variable
    vector<vector<Vertex>> cellDetails = *grid->getVertexArray();

    int i = src.row, j = src.column;
    // cellDetails[i][j].f = 0.0;
    cellDetails[i][j].g_cost = 0.0;
    cellDetails[i][j].h_cost = 0.0;
    cellDetails[i][j].prev_vertex = &cellDetails[i][j];

    // Create an open list <f=g+h, <row, column>>
    priority_queue<Tuple, vector<Tuple>, greater<Tuple> > openList;

    openList.emplace(0.0, i, j);    // starting point

    while (!openList.empty()) { // A* search begins
    
        const Tuple& p = openList.top();
        i = get<1>(p); // row element of tuple
        j = get<2>(p); // column element of tuple

        // Remove this vertex from the open list
        openList.pop();
        closedList[i][j] = true;
        for (int add_x = -1; add_x <= 1; add_x++) {
            for (int add_y = -1; add_y <= 1; add_y++) {
                if(!grid->vertexInRange(i + add_x, j + add_y)) continue;
                Vertex neighbour = cellDetails[i + add_x][j + add_y];
                // Only process this cell if this is a valid one
                if (grid->validVertex(neighbour)) {

                    // If the destination cell is the same as the current successor
                    if (isDestination(neighbour, destVertex)) { // Set the Parent of the destination cell
                        cellDetails[neighbour.row][neighbour.column].prev_vertex = &cellDetails[i][j];
                        printf("The destination cell is found\n");
                        tracePath(cellDetails, destVertex, searchResult);
                        return;
                    }
                    // If the successor is already on the closed list or if it is blocked, then ignore it
                    else if (!closedList[neighbour.row][neighbour.column] && isUnBlocked(neighbour)) {
                        double gNew, hNew, fNew;
                        gNew = calculateGValue(cellDetails[i][j]);
                        hNew = calculateHValue(neighbour, destVertex);
                        fNew = gNew + hNew;

                        // if it is not in open list and this path has shorter f
                        if ((neighbour.g_cost == START_COST && neighbour.h_cost == START_COST) || neighbour.g_cost + neighbour.h_cost > fNew){
                            openList.emplace(fNew, neighbour.row, neighbour.column);
                            cellDetails[neighbour.row][neighbour.column].g_cost = gNew;
                            cellDetails[neighbour.row][neighbour.column].h_cost = hNew;
                            cellDetails[neighbour.row][neighbour.column].prev_vertex = &cellDetails[i][j];
                        }
                    }
                }
            }
        }
    }

    // When the destination cell is not found
    printf("Failed to find the Destination Cell\n");
}

