#include "action.h"
#include <cmath>
#include <iostream>

using namespace std;

// const float TURN_RADIUS = 34;   // to update regularly according to STM Team
const float TURN_RADIUS = 20;
const int TURN_RADIUS_GRID = (int)(ceil(TURN_RADIUS/UNIT_LENGTH));
const float MAX_IMAGE_VIEW_DISTANCE = 30;
const int MAX_IMAGE_VIEW_DISTANCE_GRID = (int)(ceil(MAX_IMAGE_VIEW_DISTANCE/UNIT_LENGTH));
const float MAX_IMAGE_VIEW_ANGLE = 180; // currently set as complete opposite. Will change in real application

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
    // 1. update map
    maps.getVertexArray()[newXGrid][newYGrid]->safe = true;

    // 2. generate new state
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
ActionTurn::ActionTurn(int turnAngle){
    // if(turnAngle < 0) this->turnAngle = turnAngle + 360;
    // else this->turnAngle = turnAngle;
    this->turnAngle = turnAngle;
}

State* ActionTurn::takeAction(State* initState, Map& maps){
    // assume that turnAngle is always 90 or -90
    Vertex* newPosition;
    int newXGrid, newYGrid;

    int faceDirection = initState->face_direction;
    int curXGrid = initState->position->xGrid;
    int curYGrid = initState->position->yGrid;
    vector<vector<Vertex*>> grids = maps.getVertexArray();

    // 1. find new state
    switch(faceDirection){
        case 0:
            newXGrid = curXGrid + 1;
            newYGrid = curYGrid + turnAngle/abs(turnAngle);

        break;
        case 90:
            newXGrid = curXGrid - turnAngle/abs(turnAngle);
            newYGrid = curYGrid + 1;
        break;
        case 180:
            newXGrid = curXGrid - 1;
            newYGrid = curYGrid - turnAngle/abs(turnAngle);
        break;
        case 270:
            newXGrid = curXGrid + turnAngle/abs(turnAngle);
            newYGrid = curYGrid - 1;
        break;
        default:
            cout << "faceDirection is not straight, but = " << faceDirection << endl;
            return nullptr;
    }

    // 2. check if new position and surrounding grids are safe
    if(!maps.isAvailableGrid(newXGrid, newYGrid)) return nullptr;   // check if new grid is valid
    newPosition = grids[newXGrid][newYGrid];
    for(int i = min(curXGrid, newXGrid); i <= max(curXGrid, newXGrid); i++){    // check if all other neighbouring grids exist and are available
        for(int j = min(curYGrid, newYGrid); j <= max(curYGrid, newYGrid); j++){
            if(!maps.isAvailableGrid(i, j)) return nullptr;
        }
    }

    // 3. perform action since it can be taken
    // update map
    for(int i = min(curXGrid, newXGrid); i <= max(curXGrid, newXGrid); i++){
        for(int j = min(curYGrid, newYGrid); j <= max(curYGrid, newYGrid); j++){
            maps.getVertexArray()[i][j]->safe = true;
        }
    }

    // introduce new state
    int newFaceDirection = (faceDirection + (int)turnAngle + 360)%360;

    State* endState = new State(newPosition, newFaceDirection, initState);
    return endState;
}

int ActionTurn::getCost(State* initState, Map maps, Obstacle o){
    return cost;
}

//zy added
int ActionTurn::getTurnAngle(){
    return turnAngle;
}

// debug ActionTurn
void ActionTurn::printAction(){
    cout << "TURN: turnAngle = " << turnAngle << endl;
}


// -------------------Turning Action Updated----------------------
ActionTurn2By4::ActionTurn2By4(int turnAngle){
    // if(turnAngle < 0) this->turnAngle = turnAngle + 360;
    // else this->turnAngle = turnAngle;
    this->turnAngle = turnAngle;
}

State* ActionTurn2By4::takeAction(State* initState, Map& maps){
    // assume that turnAngle is always 90 or -90
    Vertex* newPosition;
    int newXGrid, newYGrid;

    int faceDirection = initState->face_direction;
    int curXGrid = initState->position->xGrid;
    int curYGrid = initState->position->yGrid;
    vector<vector<Vertex*>> grids = maps.getVertexArray();

    // 1. find new state
    switch(faceDirection){
        case 0:
            newXGrid = curXGrid + 2;
            newYGrid = curYGrid + 4*turnAngle/abs(turnAngle);

        break;
        case 90:
            newXGrid = curXGrid - 4*turnAngle/abs(turnAngle);
            newYGrid = curYGrid + 2;
        break;
        case 180:
            newXGrid = curXGrid - 2;
            newYGrid = curYGrid - 4*turnAngle/abs(turnAngle);
        break;
        case 270:
            newXGrid = curXGrid + 4*turnAngle/abs(turnAngle);
            newYGrid = curYGrid - 2;
        break;
        default:
            cout << "faceDirection is not straight, but = " << faceDirection << endl;
            return nullptr;
    }

    // 2. check if new position and surrounding grids are safe
    if(!maps.isAvailableGrid(newXGrid, newYGrid)) return nullptr;   // check if new grid is valid
    newPosition = grids[newXGrid][newYGrid];
    for(int i = min(curXGrid, newXGrid); i <= max(curXGrid, newXGrid); i++){    // check if all other neighbouring grids exist and are available
        for(int j = min(curYGrid, newYGrid); j <= max(curYGrid, newYGrid); j++){
            if(!maps.isAvailableGrid(i, j)) return nullptr;
        }
    }

    // 3. perform action since it can be taken
    // update map
    for(int i = min(curXGrid, newXGrid); i <= max(curXGrid, newXGrid); i++){
        for(int j = min(curYGrid, newYGrid); j <= max(curYGrid, newYGrid); j++){
            maps.getVertexArray()[i][j]->safe = true;
        }
    }

    // introduce new state
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
    // if(turnAngle < 0) this->turnAngle = turnAngle + 360;
    // else this->turnAngle = turnAngle;
    this->turnAngle = turnAngle;
}

State* ActionReverseTurn2By4::takeAction(State* initState, Map& maps){
    // assume that turnAngle is always 90 or -90
    Vertex* newPosition;
    int newXGrid, newYGrid;

    int faceDirection = initState->face_direction;
    int curXGrid = initState->position->xGrid;
    int curYGrid = initState->position->yGrid;
    vector<vector<Vertex*>> grids = maps.getVertexArray();

    // 1. find new state
    switch(faceDirection){
        case 0:
            newXGrid = curXGrid - 2;
            newYGrid = curYGrid + 4*turnAngle/abs(turnAngle);

        break;
        case 90:
            newXGrid = curXGrid - 4*turnAngle/abs(turnAngle);
            newYGrid = curYGrid - 2;
        break;
        case 180:
            newXGrid = curXGrid + 2;
            newYGrid = curYGrid - 4*turnAngle/abs(turnAngle);
        break;
        case 270:
            newXGrid = curXGrid + 4*turnAngle/abs(turnAngle);
            newYGrid = curYGrid + 2;
        break;
        default:
            cout << "faceDirection is not straight, but = " << faceDirection << endl;
            return nullptr;
    }

    // 2. check if new position and surrounding grids are safe
    if(!maps.isAvailableGrid(newXGrid, newYGrid)) return nullptr;   // check if new grid is valid
    newPosition = grids[newXGrid][newYGrid];
    for(int i = min(curXGrid, newXGrid); i <= max(curXGrid, newXGrid); i++){    // check if all other neighbouring grids exist and are available
        for(int j = min(curYGrid, newYGrid); j <= max(curYGrid, newYGrid); j++){
            if(!maps.isAvailableGrid(i, j)) return nullptr;
        }
    }

    // 3. perform action since it can be taken
    // update map
    for(int i = min(curXGrid, newXGrid); i <= max(curXGrid, newXGrid); i++){
        for(int j = min(curYGrid, newYGrid); j <= max(curYGrid, newYGrid); j++){
            maps.getVertexArray()[i][j]->safe = true;
        }
    }

    // introduce new state
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


