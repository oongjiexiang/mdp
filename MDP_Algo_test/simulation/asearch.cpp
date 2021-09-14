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



// Creating a shortcut for tuple<int, Vertex> type
typedef tuple<double, State*> Tuple;

bool aStar::isDestination(const State& curState, const Obstacle& obstacle){
    return (curState.obstacleSeen == obstacle.id && obstacle.is_seen);
}

// A Utility Function to calculate the 'h' heuristics.
double aStar::calculateHValue(const State& curState, const Obstacle& obstacle){  // tochange
    double hCost = 0;
    Vertex finalState = grid->findVertexByGrid(obstacle.row, obstacle.column);
    hCost = hCost + abs(finalState.row - curState.position->row) + abs(finalState.column - curState.position->column);
    return hCost;
}

// Encapsulate g cost calculation
double aStar::calculateGValue(State& curState, Action* action){ 
    return curState.gCost + action->getCost(); 
}
// A Utility Function to trace the path from the source to destination
void aStar::tracePath(State goalState, SearchResult& searchResult){
    stack<State> Path;
    int row = goalState.position->row;
    int col = goalState.position->column;
    State* next_node = &goalState;
    vector<Action*> actions;

    // trace from destination back to source
    do {
        Path.push(*next_node);
        next_node = next_node->prevState;
        row = next_node->position->row;
        col = next_node->position->column;
    } while (next_node != nullptr);   // at source, prev_vertex/parent = next_node
    Path.emplace(*next_node);

    // Package into <distance: double, actions: vector<Action>> and modify the memory
    vector<State> states;
    while(!Path.empty()){
        State p = Path.top();
        states.push_back(p);
        Path.pop();
    }
    searchResult = SearchResult(states.back().gCost, states);

    // printf("\nThe Path is ");    // for printing the full path
    // while (!Path.empty()) {
    //     Vertex p = Path.top();
    //     Path.pop();
    //     printf("-> (%d,%d) ", p.row, p.column);
    // }
}

aStar::aStar(){
    grid = new Map();
    ActionForward* forward = new ActionForward(UNIT_LENGTH/UNIT_LENGTH);
    ActionReverse* reverse = new ActionReverse(UNIT_LENGTH/UNIT_LENGTH);
    ActionDetect* detect = new ActionDetect();
    ActionTurn* left = new ActionTurn(90);
    ActionTurn* right = new ActionTurn(-90);

    possibleActions.push_back(forward);
    possibleActions.push_back(reverse);
    possibleActions.push_back(detect);
    possibleActions.push_back(left);
    possibleActions.push_back(right);

}
aStar::aStar(vector<Obstacle&> obstacles){
    grid = new Map(obstacles);

    // test
    cout << "test in aStar constructor after feeding in obstacles input vector" << endl;
    vector<vector<Vertex&>> vertexArray = grid->getVertexArray();
    for(int i = 0; i < vertexArray.size(); i++){
        for(int j = 0; j < vertexArray[i].size(); j++){
            cout << (vertexArray[i][j].is_obstacle | vertexArray[i][j].is_border) << " ";
        }
        cout << endl;
    }
    cout << endl;
}
State* aStar::search(State& initState, Obstacle& dest, SearchResult& searchResult){

    // Either the source or the destination is blocked
    if (!initState.position->is_border && !initState.position->is_obstacle) {
        printf("Source is blocked\n");
        return;
    }

    // If the destination cell is the same as source cell
    if (isDestination(initState, dest)) {
        printf("We are already detected %d at (%d, %d)\n", dest.id, dest.row, dest.column);
        return;
    }

    // Create a closed list and initialise it to false ie no cell is visited yet
    bool closedList[ROW_COUNT][COLUMN_COUNT][4];    // 5 because we have 5 actions
    memset(closedList, false, sizeof(closedList));

    // Create a copy of grid and perform A* on it. If we keep resetting, it may cost O(n^2) time for each reset
    // If we just create a copy, we occupy twice the memory of grid, but it is a local variable
    // vector<vector<Vertex&>> cellDetails = grid->getVertexArray();
    State* cellDetails[ROW_COUNT][COLUMN_COUNT][4];
    memset(cellDetails, false, sizeof(cellDetails));

    int faceDirection = (int)(initState.face_direction);
    if(faceDirection < 0) faceDirection+=360;
    Vertex* position = initState.position;
    State* curCell = cellDetails[position->row][position->column][faceDirection];
    curCell->gCost = 0.0;
    curCell->hCost = 0.0;
    curCell->broughtBy = nullptr;
    curCell->prevState = nullptr;

    // Create an open list <f=g+h, <row, column>>
    priority_queue<Tuple, vector<Tuple>, greater<Tuple> > openList;
    
    // starting point
    openList.emplace(Tuple(0.0, &initState));
    // mark as visited
    
    closedList[initState.position->row][initState.position->column][faceDirection%90];

    while (!openList.empty()) { // A* search begins
    
        const Tuple& p = openList.top();
        State* source = get<1>(p);

        // Remove this vertex from the open list
        openList.pop();

        // start finding next states/ neighbours
        for(int i = 0; i < possibleActions.size(); i++){
            if(possibleActions[i]->canTakeAction(source, *grid)){
                State newState = possibleActions[i]->takeAction(source, *grid);
                if(isDestination(newState, dest)){
                    cout << "The goal is found\n" << endl;
                    tracePath(newState, searchResult);
                    return &newState;
                }
                int faceDirection = (int)(source->face_direction);
                if(faceDirection < 0) faceDirection+=360;
                Vertex* position = newState.position;
                State* curCell = cellDetails[position->row][position->column][faceDirection];

                // If the successor is not yet in the closed list
                if(closedList[source->position->row][source->position->column][faceDirection%90]){
                    double gNew, hNew, fNew;
                    gNew = calculateGValue(*source, possibleActions[i]);
                    hNew = calculateHValue(newState, dest);
                    fNew = gNew + hNew;
                    if(curCell->gCost + curCell->hCost > fNew){
                        curCell->gCost = gNew;
                        curCell->hCost = hNew;
                        curCell->prevState = source;    // need to link to previous cellDetail?
                    }
                }
            }
        }
    }

    // When the destination cell is not found
    printf("Failed to find the Destination Cell\n");
}

