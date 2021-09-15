#include "config.h"
#include "asearch.h"
#include "math.h"
#include <vector>
#include <cstring>
#include <stdio.h>
#include <iostream>
#include <queue>
#include <stack>
#include <tuple>
using namespace std;


bool aStar::isDestination(const State& curState, const Obstacle& obstacle){
    return (curState.obstacleSeen == obstacle.id);
}

// A Utility Function to calculate the 'h' heuristics.
double aStar::calculateHValue(State& curState, Obstacle& obstacle){  // tochange

    double hCost = 0;
    Vertex* finalState = grid->findVertexByGrid(obstacle.row, obstacle.column);
    hCost = hCost + abs(finalState->row - curState.position->row) + abs(finalState->column - curState.position->column);
    return hCost;
}

// Encapsulate g cost calculation
double aStar::calculateGValue(State& curState, Action* action){
    return curState.gCost + action->getCost(); 
}
// A Utility Function to trace the path from the source to destination
double aStar::tracePath(State* goalState, vector<State*>* states){ 
    stack<State*> Path;
    State* next_node = goalState;

    // trace from destination back to source
    do {
        Path.push(next_node);
        next_node = next_node->prevState;
    } while (next_node != nullptr);   // at source, prev_vertex/parent = next_node

    // Package into <distance: double, actions: vector<Action>> and modify the memory
    states->clear();
    while(!Path.empty()){
        State* p = Path.top();
        states->push_back(p);
        Path.pop();
    }
    return states->back()->gCost;
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

aStar::aStar(vector<Obstacle> obstacles){
    grid = new Map(obstacles);
}

void aStar::generatePossibleActions(Obstacle obstacle){
    ActionForward* forward = new ActionForward(UNIT_LENGTH);
    ActionReverse* reverse = new ActionReverse(UNIT_LENGTH);
    ActionDetect* detect = new ActionDetect(obstacle.id);
    ActionTurn* left = new ActionTurn(90);
    ActionTurn* right = new ActionTurn(-90);

    possibleActions.push_back(forward);
    possibleActions.push_back(reverse);
    possibleActions.push_back(detect);
    possibleActions.push_back(left);
    possibleActions.push_back(right);
}

State* aStar::search(State& initState, Obstacle& dest, double* pathCost, vector<State*>* states){
    generatePossibleActions(dest);

    // Either the source or the destination is blocked
    if (initState.position->is_border || initState.position->is_obstacle) {
        printf("Source is blocked\n");
        return nullptr;
    }

    // If the destination cell is the same as source cell
    if (isDestination(initState, dest)) {
        printf("We already detected %d at (%d, %d)\n", dest.id, dest.row, dest.column);
        initState.printState();
        return nullptr;
    }

    // Create a closed list and initialise it to false ie no cell is visited yet
    bool closedList[ROW_COUNT][COLUMN_COUNT][5];    // 5 because we have 5 actions
    memset(closedList, false, sizeof(closedList));

    // Create a copy of grid and perform A* on it. If we keep resetting, it may cost O(n^2) time for each reset
    // If we just create a copy, we occupy twice the memory of grid, but it is a local variable
    State* cellDetails[ROW_COUNT][COLUMN_COUNT][5];
    memset(cellDetails, false, sizeof(cellDetails));

    int faceDirection = (int)(initState.face_direction);
    if(faceDirection < 0) faceDirection+=360;
    faceDirection/=90;  // this faceDirection is for indexing purposes. For cellDetails, 0 degree to 0, 90 corresponds to 1, 180 to 2, -90 to 3

    Vertex* position = initState.position;
    State* curCell = &initState;
    cellDetails[position->row][position->column][faceDirection] = curCell;
    curCell->gCost = 0.0;
    curCell->hCost = 0.0;
    curCell->broughtByAction = "";
    curCell->prevState = nullptr;

    // Create an open list <f=g+h, <row, column>>
    priority_queue<Tuple, vector<Tuple>, greater<Tuple> > openList;
    
    // starting point
    openList.emplace(Tuple(0.0, &initState));
    cellDetails[position->row][position->column][(int)(curCell->face_direction)] = curCell;
    // mark as visited

    while (!openList.empty()) { // A* search begins
    
        const Tuple& p = openList.top();
        State* source = get<1>(p);

        // Remove this vertex from the open list
        openList.pop();
        closedList[source->position->row][source->position->column][(int)(source->face_direction)/90] = true;

        // start finding next states/ neighbours
        for(int i = 0; i < possibleActions.size(); i++){

            if(possibleActions[i]->canTakeAction(source, *grid)){
                State* newState = possibleActions[i]->takeAction(source, *grid);
                if(isDestination(*newState, dest)){
                    newState->gCost = calculateGValue(*source, possibleActions[i]);
                    *pathCost = tracePath(newState, states);
                    return newState;
                }
                int neighbourFaceDirection = (int)(source->face_direction);
                if(neighbourFaceDirection < 0) neighbourFaceDirection+=360;
                Vertex* neighbourPosition = newState->position;
                State* neighbourCell = newState;

                bool newlySeenState = false;
                if(cellDetails[neighbourPosition->row][neighbourPosition->column][neighbourFaceDirection] != nullptr){
                    neighbourCell = cellDetails[neighbourPosition->row][neighbourPosition->column][neighbourFaceDirection];
                }
                else{
                    newlySeenState = true;
                    cellDetails[neighbourPosition->row][neighbourPosition->column][neighbourFaceDirection] = neighbourCell;
                }

                // If the successor is not yet in the closed list
                if(!closedList[neighbourPosition->row][neighbourPosition->column][neighbourFaceDirection/90]){
                    double gNew, hNew, fNew;
                    gNew = calculateGValue(*source, possibleActions[i]);
                    hNew = calculateHValue(*neighbourCell, dest);
                    fNew = gNew + hNew;
                    
                    if(newlySeenState || neighbourCell->gCost + neighbourCell->hCost > fNew){
                        openList.push(Tuple(fNew, newState));
                        neighbourCell->gCost = gNew;
                        neighbourCell->hCost = hNew;
                        neighbourCell->prevState = source;    // need to link to previous cellDetail?
                    }
                }
            }
        }
    }

    // When the destination cell is not found
    printf("Failed to find the Destination Cell\n");
    return nullptr;
}

