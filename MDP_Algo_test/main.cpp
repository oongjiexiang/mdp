#include "component.h"
#include "config.h"
#include "asearch.h"
#include <iostream>

using namespace std;

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

    // Obstacles
    Obstacle o1(1, 5, 3, 0);
    vector<Obstacle> obstacles;
    obstacles.push_back(o1);
    aStar astar(grid, obstacles);
    // astar.search(src, dest);
 
    return 0;
}
