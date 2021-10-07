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


aStar::aStar(vector<Obstacle> obstacles, int maxDistFromBorder): maxDistFromBorder(maxDistFromBorder){
    grid = new Map(obstacles, maxDistFromBorder);
}

void aStar::generatePossibleActions(Obstacle obstacle){
    ActionStraight* forward = new ActionStraight(1);
    ActionStraight* reverse = new ActionStraight(-1);
    ActionTurnOnSpot* leftOnSpot = new ActionTurnOnSpot(90);
    ActionTurnOnSpot* rightOnSpot = new ActionTurnOnSpot(-90);
    ActionTurn2By4* left = new ActionTurn2By4(90);
    ActionTurn2By4* right = new ActionTurn2By4(-90);
    ActionReverseTurn2By4* reverseLeft = new ActionReverseTurn2By4(90);
    ActionReverseTurn2By4* reverseRight = new ActionReverseTurn2By4(-90);

    possibleActions.clear();
    possibleActions.push_back(forward);
    possibleActions.push_back(reverse);
    possibleActions.push_back(leftOnSpot);
    possibleActions.push_back(rightOnSpot);
    possibleActions.push_back(left);
    possibleActions.push_back(right);
    //possibleActions.push_back(reverseLeft);
    //possibleActions.push_back(reverseRight);
}

State* aStar::generateGoalState(Obstacle obstacle){
    int goalXGrid = obstacle.xGrid + ((int)cos(M_PI/180*obstacle.face_direction))*ROBOT_VIEWING_GRID_LENGTH;
    int goalYGrid = obstacle.yGrid + ((int)sin(M_PI/180*obstacle.face_direction))*ROBOT_VIEWING_GRID_LENGTH;
    if(!grid->isValidGrid(goalXGrid, goalYGrid)){
        cout << "grid at " << goalXGrid << ", " << goalYGrid << " is not empty. Check with the position near to obstacle is available" << endl;
        goalXGrid = obstacle.xGrid + ((int)cos(M_PI/180*obstacle.face_direction))*(ROBOT_VIEWING_GRID_LENGTH - 1);
        goalYGrid = obstacle.yGrid + ((int)sin(M_PI/180*obstacle.face_direction))*(ROBOT_VIEWING_GRID_LENGTH - 1);
        if(!grid->isValidGrid(goalXGrid, goalYGrid)){
            cout << "Position near obstacle is also not available. Last check: 40cm from obstacle" << endl;
            goalXGrid = obstacle.xGrid + ((int)cos(M_PI/180*obstacle.face_direction))*(ROBOT_VIEWING_GRID_LENGTH + 1);
            goalYGrid = obstacle.yGrid + ((int)sin(M_PI/180*obstacle.face_direction))*(ROBOT_VIEWING_GRID_LENGTH + 1);
            if(!grid->isValidGrid(goalXGrid, goalYGrid)){
                cout << "fail to find goal state. Hamiltonian path cannot be found" << endl;
                throw(nullptr);
            }
        }
    }
    // create goal state
    Vertex* goalPosition = grid->findVertexByGrid(goalXGrid, goalYGrid);
    int goalFaceDirection = (obstacle.face_direction + 180)%360;
    State* goalState = new State(goalPosition, goalFaceDirection, nullptr);
    return goalState;
}

bool aStar::isDestination(const State& curState, const State& goalState){
    return (curState.position->xGrid == goalState.position->xGrid &&
        curState.position->yGrid == goalState.position->yGrid &&
        curState.face_direction == goalState.face_direction);
}

// A Utility Function to calculate the 'h' heuristics.
float aStar::calculateHValue(State& curState, State& goalState){  // tochange
    float hCost = 0;
    hCost = hCost + abs(goalState.position->xGrid - curState.position->xGrid) + abs(goalState.position->yGrid - curState.position->yGrid);
    if((goalState.face_direction == 0 && curState.face_direction == 0 && goalState.position->xGrid + 1 == curState.position->xGrid && goalState.position->yGrid == curState.position->yGrid)
        || (goalState.face_direction == 90 && curState.face_direction == 90 && goalState.position->yGrid + 1 == curState.position->yGrid && goalState.position->xGrid == curState.position->xGrid)
        || (goalState.face_direction == 180 && curState.face_direction == 180 && goalState.position->xGrid - 1 == curState.position->xGrid && goalState.position->yGrid == curState.position->yGrid)
        || (goalState.face_direction == 270 && curState.face_direction == 270 && goalState.position->yGrid - 1 == curState.position->yGrid && goalState.position->xGrid == curState.position->xGrid)
    ){
        hCost+=20;  // penalise the algo when it is nearly touching the obstacle
    }
    return hCost;
}

// Encapsulate g cost calculation
float aStar::calculateGValue(State& curState, Action* action, Map& localMap, Obstacle& obstacle){
    return curState.gCost + action->getCost(&curState, localMap, obstacle);
}
// A Utility Function to trace the path from the source to destination
float aStar::tracePath(State* goalState, vector<State*>* states){
    stack<State*> Path;
    State* next_node = goalState;

    // trace from destination back to source
    do {
        Path.push(next_node);
        next_node = next_node->prevState;
    } while (next_node != nullptr);   // at source, prev_vertex/parent = next_node

    // Package into <distance: float, actions: vector<Action>> and modify the memory
    states->clear();
    while(!Path.empty()){
        State* p = Path.top();
        states->push_back(p);

        Path.pop();
    }
    return states->back()->gCost;
}




void aStar::changeObstacleFace(Obstacle obstacle, int newFaceDirection){
    vector<Obstacle>& obstacles = grid->getObstacles();
    for(int i = 0; i < obstacles.size(); i++){
        if(obstacles[i].id == obstacle.id){
            obstacles[i].face_direction = newFaceDirection;
            return;
        }
    }
}

State* aStar::search(State* initState, Obstacle& dest, float* pathCost, vector<State*>* states){
    // Find the goal state
    State* goalState = generateGoalState(dest);

    // 1. Check if this search definitely fails
    // Either the source or the destination is blocked
    if (!grid->isAvailableGrid(initState->position->xGrid, initState->position->yGrid)){
        printf("Source is blocked\n");
        return nullptr;
    }

    // If the destination cell is the same as source cell
    if (isDestination(*initState, *goalState)) {
        printf("We already detected %d at (%d, %d)\n", dest.id, dest.xGrid, dest.yGrid);
        return nullptr;
    }
    // 2. Initialise useful variables
    // Create a closed list and initialise it to false ie no cell is visited yet
    generatePossibleActions(dest);  // a. all 5 actions
    Map localMap = *grid;   // b. local map so that the original map is not modified after every search

    bool closedList[X_GRID_COUNT + 2*maxDistFromBorder][Y_GRID_COUNT + 2*maxDistFromBorder][4];    // c. check if the state is visited (5 actions)
    memset(closedList, false, sizeof(closedList));

    State* cellDetails[X_GRID_COUNT + 2*maxDistFromBorder][Y_GRID_COUNT + 2*maxDistFromBorder][4];  // d. get the latest state detail at that position and face direction
    memset(cellDetails, false, sizeof(cellDetails));

    priority_queue<Tuple, vector<Tuple>, greater<Tuple> > openList; // e. Priority Queue <f-cost, state>

    // for indexing purposes. For cellDetails, 0 degree to 0, 90 corresponds to 1, 180 to 2, -90 to 3
    int faceDirection = (int)(initState->face_direction)/90;

    // 3. Set up initial state
    Vertex* position = initState->position;
    initState->gCost = initState->hCost = 0.0;
    initState->prevState = nullptr;
    cellDetails[position->xGrid + maxDistFromBorder][position->yGrid + maxDistFromBorder][faceDirection] = initState;  // mark as visited
    openList.emplace(Tuple(0.0, initState));

    // 4. A* algorithm begins
    while (!openList.empty()) {
        const Tuple& p = openList.top();
        State* source = get<1>(p);

        // Remove this vertex from the open list
        openList.pop();
        closedList[source->position->xGrid + maxDistFromBorder][source->position->yGrid + maxDistFromBorder][source->face_direction/90] = true;

        // start finding next states/ neighbours
        for(int i = 0; i < possibleActions.size(); i++){
            State* neighbourState = possibleActions[i]->takeAction(source, localMap);

            // if action fails to be taken, skip
            if(neighbourState == nullptr) continue;

            // if action sees the image, proceed to trace path
            if(isDestination(*neighbourState, *goalState)){
                neighbourState->gCost = calculateGValue(*source, possibleActions[i], localMap, dest);
                *pathCost = tracePath(neighbourState, states);
                return neighbourState;
            }
            Vertex* neighbourPosition = neighbourState->position;
            int neighbourFaceDirection = (int)(neighbourState->face_direction)/90;

            bool newlySeenState = false;
            if(cellDetails[neighbourPosition->xGrid + maxDistFromBorder][neighbourPosition->yGrid + maxDistFromBorder][neighbourFaceDirection] != nullptr){
                neighbourState = cellDetails[neighbourPosition->xGrid + maxDistFromBorder][neighbourPosition->yGrid + maxDistFromBorder][neighbourFaceDirection];
            }
            else{
                newlySeenState = true;
                cellDetails[neighbourPosition->xGrid + maxDistFromBorder][neighbourPosition->yGrid + maxDistFromBorder][neighbourFaceDirection] = neighbourState;
            }

            // If the successor is not yet in the closed list
            if(!closedList[neighbourPosition->xGrid + maxDistFromBorder][neighbourPosition->yGrid + maxDistFromBorder][neighbourFaceDirection]){
                float gNew, hNew, fNew;
                gNew = calculateGValue(*source, possibleActions[i], localMap, dest);
                hNew = calculateHValue(*neighbourState, *goalState);
                fNew = gNew + hNew;

                if(newlySeenState || neighbourState->gCost + neighbourState->hCost > fNew){
                    openList.push(Tuple(fNew, neighbourState));
                    neighbourState->gCost = gNew;
                    neighbourState->hCost = hNew;
                    neighbourState->prevState = source;    // need to link to previous cellDetail?
                }
            }
        }
    }
    // When the destination cell is not found
    printf("Failed to find the Destination Cell\n");
    return nullptr;
}

