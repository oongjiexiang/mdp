#include "config.h"
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

class aStar{
    Map* grid;
    bool isUnBlocked(const Vertex& vertex){ // check whether cell is not blocked
        // Returns true if the cell is not blocked else false
        int row = vertex.row;
        int column = vertex.column;
        Vertex v = grid->findVertexByGrid(row, column);
        printf("(%d, %d): %d %d\n", v.row, v.column, v.is_border, v.is_obstacle);
        return (!v.is_border && !v.is_obstacle);
    }

    // A Utility Function to check whether destination cell has
    // been reached or not
    bool isDestination(const Vertex& position, const Vertex& dest){
        return (position.row == dest.row && position.column == dest.column);
    }

    // A Utility Function to calculate the 'h' heuristics.
    double calculateHValue(const Vertex& src, const Vertex& dest){  // tochange
        // h is estimated with the two points distance formula
        return sqrt(pow((src.row - dest.row), 2.0) + pow((src.column - dest.column), 2.0));
    }

    // Encapsulate g cost calculation
    double calculateGValue(Vertex& cur){    // to change
        return cur.g_cost + 1.0; 
    }
    // A Utility Function to trace the path from the source to destination
    void tracePath(const vector<vector<Vertex>>& cellDetails, Vertex dest){
        stack<Vertex> Path;
        int row = dest.row;
        int col = dest.column;
        
        Vertex* next_node = &dest;// = cellDetails[row][col].prev_vertex;
        // trace from destination back to source
        do {
            Path.push(*next_node);
            next_node = cellDetails[row][col].prev_vertex;
            row = next_node->row;
            col = next_node->column;
        } while (cellDetails[row][col].prev_vertex != next_node);   // at source, prev_vertex/parent = next_node
        cout << "not this anymore "; cellDetails[row][col].prev_vertex->printVertex(); cout << endl;
        Path.emplace(*next_node);
        printf("\nThe Path is ");
        while (!Path.empty()) {
            Vertex p = Path.top();
            Path.pop();
            printf("-> (%d,%d) ", p.row, p.column);
        }
    }
    public:
        aStar(){
            grid = new Map();
        }
        aStar(vector<vector<int>> fullMap){
            grid = new Map();
            grid->setMap(fullMap);
        }
        void search(Vertex& src, Vertex& dest){
            // If the source is out of range
            if (!grid->validVertex(src)) {
                printf("Source is invalid\n");
                return;
            }
        
            // If the destination is out of range
            if (!grid->validVertex(dest)) {
                printf("Destination is invalid\n");
                return;
            }
        
            // Either the source or the destination is blocked
            if (!isUnBlocked(src) || !isUnBlocked(dest)) {
                printf("Source or the destination is blocked\n");
                return;
            }
        
            // If the destination cell is the same as source cell
            if (isDestination(src, dest)) {
                printf("We are already at the destination\n");
                return;
            }
        
            // Create a closed list and initialise it to false which
            // means that no cell has been included yet This closed
            // list is implemented as a boolean 2D vector
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
        
            /*
            Create an open list having information as-
            <f, <i, j>>
            where f = g + h,
            and i, j are the row and column index of that cell
            Note that 0 <= i <= ROW_COUNT-1 & 0 <= j <= COLUMN_COUNT-1
            This open list is implenented as a set of tuple.*/
            priority_queue<Tuple, vector<Tuple>, greater<Tuple> > openList;
        
            // Put the starting cell on the open list and set its
            // 'f' as 0
            openList.emplace(0.0, i, j);
        
            // We set this boolean value as false as initially
            // the destination is not reached.
            while (!openList.empty()) {
            
                const Tuple& p = openList.top();
                // Add this vertex to the closed list
                i = get<1>(p); // column element of tuple
                j = get<2>(p); // third element of tuple

                cout << "popping out (" << i << ", " << j << ")" << endl; // test
        
                // Remove this vertex from the open list
                openList.pop();
                closedList[i][j] = true;
                cout << "start looking at neighbours" << endl;
                for (int add_x = -1; add_x <= 1; add_x++) {
                    for (int add_y = -1; add_y <= 1; add_y++) {
                        if(!grid->vertexInRange(i + add_x, j + add_y)) continue;
                        Vertex neighbour = cellDetails[i + add_x][j + add_y];
                        cout << "i + add_x is " << i + add_x << endl;
                        // Only process this cell if this is a valid one
                        if (grid->validVertex(neighbour)) {
                            neighbour.printVertex();    // test

                            // If the destination cell is the same as the current successor
                            if (isDestination(neighbour, dest)) { // Set the Parent of the destination cell
                                cellDetails[neighbour.row][neighbour.column].prev_vertex = &cellDetails[i][j];
                                printf("The destination cell is found\n");
                                tracePath(cellDetails, dest);
                                return;
                            }
                            // If the successor is already on the closed list or if it is blocked, then ignore it.  Else do the following
                            else if (!closedList[neighbour.row][neighbour.column] && isUnBlocked(neighbour)) {
                                double gNew, hNew, fNew;
                                gNew = calculateGValue(cellDetails[i][j]);
                                hNew = calculateHValue(neighbour, dest);
                                fNew = gNew + hNew;
        
                                // If it isn’t on the open list, add
                                // it to the open list. Make the
                                // current square the prev_vertex of this
                                // square. Record the f, g, and h
                                // costs of the square cell
                                //             OR
                                // If it is on the open list
                                // already, check to see if this
                                // path to that square is better,
                                // using 'f' cost as the measure.
                                // Vertex neighbour = cellDetails[neighbour.row][neighbour.column];
                                cout << "neighbour info "; neighbour.printVertex(); cout << endl;
                                // cout << "neighbour_a info"; neighbour_a.printVertex(); cout << endl;
                                
                                if ((neighbour.g_cost == START_COST && neighbour.h_cost == START_COST) || neighbour.g_cost + neighbour.h_cost > fNew){
                                    cout << "queuing (" << neighbour.row << ", " << neighbour.column << ") with f=" << fNew << endl; // test
                                    openList.emplace(fNew, neighbour.row, neighbour.column);
        
                                    // Update the details of this cell
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
};

int main()
{
    /* Description of the Grid-
    1--> The cell is not blocked
    0--> The cell is blocked */
    vector<vector<int>> grid{
        { { { 1, 0, 1, 1, 1, 1, 0, 1, 1, 1 } },
          { { 1, 1, 1, 0, 1, 1, 1, 0, 1, 1 } },
          { { 1, 1, 1, 0, 1, 1, 0, 1, 0, 1 } },
          { { 0, 0, 1, 0, 1, 0, 0, 0, 0, 1 } },
          { { 1, 1, 1, 0, 1, 1, 1, 0, 1, 0 } },
          { { 1, 0, 1, 1, 1, 1, 0, 1, 0, 0 } },
          { { 1, 0, 0, 0, 0, 1, 0, 0, 0, 1 } },
          { { 1, 0, 1, 1, 1, 1, 0, 1, 1, 1 } },
          { { 1, 1, 1, 0, 0, 0, 1, 0, 0, 1 } } }
    };
 
    // Source is the left-most bottom-most corner
    Vertex src(8, 9);
 
    // Destination is the left-most top-most corner
    Vertex dest(0, 7);
    aStar astar(grid);
    astar.search(src, dest);
 
    return 0;
}