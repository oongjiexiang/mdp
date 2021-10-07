#include "action.h"
#include <cmath>
#include <iostream>

using namespace std;

const float TURN_RADIUS = 20;
const int TURN_RADIUS_GRID = (int)(ceil(TURN_RADIUS/UNIT_LENGTH));
const float MAX_IMAGE_VIEW_DISTANCE = 30;
const int MAX_IMAGE_VIEW_DISTANCE_GRID = (int)(ceil(MAX_IMAGE_VIEW_DISTANCE/UNIT_LENGTH));
const float TURN_ON_SPOT_RADIUS = 20;
const int TURN_ON_SPOT_RADIUS_GRID = (int)(ceil(TURN_ON_SPOT_RADIUS/UNIT_LENGTH));

// -------------Utility Euclidean distance-------------
float euclidean(float x1, float y1, float x2, float y2){
    return pow(pow((x1 - x2), 2.0) + pow((y1 - y2), 2.0), 0.5);
}


// State
State::State(Vertex* position, int face_direction, State* prevState):
    position(position), face_direction(face_direction), prevState(prevState){}

// debug State
void State::printState(){
    cout << "--------------State---------------\n\t";
    position->printVertex();
    cout << "\tfaceDirection: " << face_direction << endl;
    cout << "--------------End State---------------" << endl;
}

// ------------------------Forward action------------------------
ActionStraight::ActionStraight(): travelDistGrid(1), cost(1) {}

ActionStraight::ActionStraight(float travelDistGrid): travelDistGrid(travelDistGrid){
    cost = 1;
}

State* ActionStraight::takeAction(State* initState, Map& maps){
    Vertex curPosition = *(initState->position);

    int moveGridDistance = (int)(travelDistGrid);
    int newYGrid = curPosition.yGrid + moveGridDistance*(int)(sin(M_PI/180*initState->face_direction));
    int newXGrid = curPosition.xGrid + moveGridDistance*(int)(cos(M_PI/180*initState->face_direction));

    // check if action can be taken
    if(!maps.isAvailableGrid(newXGrid, newYGrid)) return nullptr;

    // Action can be taken!
    // 1. generate new state
    Vertex* newPosition = new Vertex(newXGrid, newYGrid);
    State* endState = new State(newPosition, initState->face_direction, initState);

    return endState;
}

int ActionStraight::getCost(State* initState, Map maps, Obstacle o){
    return cost;
}

int ActionStraight::getTravelDistGrid(){
    return travelDistGrid;
}

// debug ActionStraight
void ActionStraight::printAction(){
    if(travelDistGrid > 0) cout << "FORWARD: travelDistGrid = " << travelDistGrid << endl;
    else cout << "REVERSE: travelDistGrid = " << travelDistGrid << endl;
}

// -------------------Turning Action----------------------
ActionTurnOnSpot::ActionTurnOnSpot(int turnAngle){
    this->turnAngle = turnAngle;
}

State* ActionTurnOnSpot::takeAction(State* initState, Map& maps){
    // assume that turnAngle is always 90 or -90
    Vertex* newPosition;

    int faceDirection = initState->face_direction;
    int curXGrid = initState->position->xGrid;
    int curYGrid = initState->position->yGrid;
    int furthestXGrid = curXGrid;
    int furthestYGrid = curYGrid;

    // 2. check if surrounding grids are safe
    switch(faceDirection){
        case 0:
            furthestXGrid+=TURN_ON_SPOT_RADIUS_GRID;
            furthestYGrid+=TURN_ON_SPOT_RADIUS_GRID*turnAngle/abs(turnAngle);
        break;
        case 90:
            furthestXGrid-=TURN_ON_SPOT_RADIUS_GRID*turnAngle/abs(turnAngle);
            furthestYGrid+=TURN_ON_SPOT_RADIUS_GRID;
        break;
        case 180:
            furthestXGrid-=TURN_ON_SPOT_RADIUS_GRID;
            furthestYGrid-=TURN_ON_SPOT_RADIUS_GRID*turnAngle/abs(turnAngle);
        break;
        case 270:
            furthestXGrid+=TURN_ON_SPOT_RADIUS_GRID*turnAngle/abs(turnAngle);
            furthestYGrid-=TURN_ON_SPOT_RADIUS_GRID;
        break;
        default:
            cout << "faceDirection in ActionOnSpot = " << faceDirection << endl;
    }

    if(!maps.isAvailableGrid(furthestXGrid, furthestYGrid)) return nullptr;   // check if new grid is valid
    newPosition = maps.findVertexByGrid(curXGrid, curYGrid);
    for(int i = min(curXGrid, furthestXGrid); i <= max(curXGrid, furthestXGrid); i++){    // check if all other neighbouring grids exist and are available
        for(int j = min(curYGrid, furthestYGrid); j <= max(curYGrid, furthestYGrid); j++){
            if(!maps.isAvailableGrid(i, j)) return nullptr;
        }
    }

    // 3. introduce new state
    int newFaceDirection = (faceDirection + (int)turnAngle + 360)%360;

    State* endState = new State(newPosition, newFaceDirection, initState);
    return endState;
}

int ActionTurnOnSpot::getCost(State* initState, Map maps, Obstacle o){
    return cost;
}

//zy added
int ActionTurnOnSpot::getTurnAngle(){
    return turnAngle;
}

// debug ActionTurn
void ActionTurnOnSpot::printAction(){
    cout << "TURN ON SPOT: turnAngle = " << turnAngle << endl;
}


// -------------------Turning Action Updated----------------------
ActionTurn2By4::ActionTurn2By4(int turnAngle){
    this->turnAngle = turnAngle;
}

State* ActionTurn2By4::takeAction(State* initState, Map& maps){
    // assume that turnAngle is always 90 or -90
    Vertex* newPosition;
    int newXGrid, newYGrid;

    int faceDirection = initState->face_direction;
    int curXGrid = initState->position->xGrid;
    int curYGrid = initState->position->yGrid;

    // 1. find new state
    switch(faceDirection){
        case 0:
            newXGrid = curXGrid + TURN_FORWARD_LENGTH;
            newYGrid = curYGrid + TURN_SIDE_LENGTH*turnAngle/abs(turnAngle);

        break;
        case 90:
            newXGrid = curXGrid - TURN_SIDE_LENGTH*turnAngle/abs(turnAngle);
            newYGrid = curYGrid + TURN_FORWARD_LENGTH;
        break;
        case 180:
            newXGrid = curXGrid - TURN_FORWARD_LENGTH;
            newYGrid = curYGrid - TURN_SIDE_LENGTH*turnAngle/abs(turnAngle);
        break;
        case 270:
            newXGrid = curXGrid + TURN_SIDE_LENGTH*turnAngle/abs(turnAngle);
            newYGrid = curYGrid - TURN_FORWARD_LENGTH;
        break;
        default:
            cout << "faceDirection is not straight, but = " << faceDirection << endl;
            return nullptr;
    }

    // 2. check if new position and surrounding grids are safe
    if(!maps.isAvailableGrid(newXGrid, newYGrid)) return nullptr;   // check if new grid is valid
    newPosition = maps.findVertexByGrid(newXGrid, newYGrid);
    for(int i = min(curXGrid, newXGrid); i <= max(curXGrid, newXGrid); i++){    // check if all other neighbouring grids exist and are available
        for(int j = min(curYGrid, newYGrid); j <= max(curYGrid, newYGrid); j++){
            if(!maps.isAvailableGrid(i, j)) return nullptr;
        }
    }

    // 3. introduce new state
    int newFaceDirection = (faceDirection + (int)turnAngle + 360)%360;

    State* endState = new State(newPosition, newFaceDirection, initState);
    return endState;
}

int ActionTurn2By4::getCost(State* initState, Map maps, Obstacle o){
    return cost;
}

//zy added
int ActionTurn2By4::getTurnAngle(){
    return turnAngle;
}

// debug ActionTurn
void ActionTurn2By4::printAction(){
    cout << "TURN: turnAngle = " << turnAngle << endl;
}


// ------------------------Action Reverse------------------------
ActionReverseTurn2By4::ActionReverseTurn2By4(int turnAngle){
    this->turnAngle = turnAngle;
}

State* ActionReverseTurn2By4::takeAction(State* initState, Map& maps){
    // assume that turnAngle is always 90 or -90
    Vertex* newPosition;
    int newXGrid, newYGrid;

    int faceDirection = initState->face_direction;
    int curXGrid = initState->position->xGrid;
    int curYGrid = initState->position->yGrid;

    // 1. find new state
    switch(faceDirection){
        case 0:
            newXGrid = curXGrid - TURN_SIDE_LENGTH;
            newYGrid = curYGrid + TURN_FORWARD_LENGTH*turnAngle/abs(turnAngle);

        break;
        case 90:
            newXGrid = curXGrid - TURN_FORWARD_LENGTH*turnAngle/abs(turnAngle);
            newYGrid = curYGrid - TURN_SIDE_LENGTH;
        break;
        case 180:
            newXGrid = curXGrid + TURN_SIDE_LENGTH;
            newYGrid = curYGrid - TURN_FORWARD_LENGTH*turnAngle/abs(turnAngle);
        break;
        case 270:
            newXGrid = curXGrid + TURN_FORWARD_LENGTH*turnAngle/abs(turnAngle);
            newYGrid = curYGrid + TURN_SIDE_LENGTH;
        break;
        default:
            cout << "faceDirection is not straight, but = " << faceDirection << endl;
            return nullptr;
    }

    // 2. check if new position and surrounding grids are safe
    if(!maps.isAvailableGrid(newXGrid, newYGrid)) return nullptr;   // check if new grid is valid
    newPosition = maps.findVertexByGrid(newXGrid, newYGrid);
    for(int i = min(curXGrid, newXGrid); i <= max(curXGrid, newXGrid); i++){    // check if all other neighbouring grids exist and are available
        for(int j = min(curYGrid, newYGrid); j <= max(curYGrid, newYGrid); j++){
            if(!maps.isAvailableGrid(i, j)) return nullptr;
        }
    }

    // 3. introduce new state
    int newFaceDirection = (faceDirection - (int)turnAngle + 360)%360;

    State* endState = new State(newPosition, newFaceDirection, initState);
    return endState;
}

int ActionReverseTurn2By4::getCost(State* initState, Map maps, Obstacle o){
    return cost;
}

int ActionReverseTurn2By4::getTurnAngle(){
    return turnAngle;
}

// debug ActionTurn
void ActionReverseTurn2By4::printAction(){
    cout << "REVERSE TURN: turnAngle = " << turnAngle << endl;
}


