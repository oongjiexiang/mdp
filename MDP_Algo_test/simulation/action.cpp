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
State::State(Vertex* position, int obstacleSeen, int face_direction, State* prevState):
    position(position), obstacleSeen(obstacleSeen), face_direction(face_direction), prevState(prevState){}

// debug State
void State::printState(){
    cout << "--------------State---------------\n\t";
    position->printVertex();
    cout << "\tobstacleSeen: " << obstacleSeen << endl;
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
    int newObstacleSeen = initState->obstacleSeen;

    int moveGridDistance = (int)(travelDistGrid);
    int newYGrid = curPosition.yGrid + moveGridDistance*sin(M_PI/180*initState->face_direction);
    int newXGrid = curPosition.xGrid + moveGridDistance*cos(M_PI/180*initState->face_direction);

    // check if action can be taken
    if(maps.isValidGrid(newXGrid, newYGrid)){
        Vertex* cell = maps.findVertexByGrid(newXGrid, newYGrid);
        if(cell->is_obstacle || cell->is_border) return nullptr;
    }

    // Action can be taken!
    // 1. update map
    maps.getVertexArray()[newXGrid][newYGrid]->safe = true;

    // 2. generate new state
    Vertex* newPosition = new Vertex(newXGrid, newYGrid);
    State* endState = new State(newPosition, newObstacleSeen, initState->face_direction, initState);

    return endState;
}

int ActionStraight::getCost(State* initState, Map maps, Obstacle o){
    return cost;
}

// debug ActionStraight
void ActionStraight::printAction(){
    if(travelDistGrid > 0) cout << "FORWARD: travelDistGrid = " << travelDistGrid << endl;
    else cout << "REVERSE: travelDistGrid = " << travelDistGrid << endl;
}

// -------------------Turning Action----------------------
ActionTurn::ActionTurn(int turnAngle):
    turnAngle(turnAngle){}
    
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
            cout << "turnAngle is not straight, but = " << turnAngle << endl;
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
    int newFaceDirection = (faceDirection + (int)turnAngle)%360;
    int newObstacleSeen = initState->obstacleSeen;
    
    State* endState = new State(newPosition, newObstacleSeen, newFaceDirection, initState);
    return endState;
}

int ActionTurn::getCost(State* initState, Map maps, Obstacle o){
    return cost;
}

// debug ActionTurn
void ActionTurn::printAction(){
    cout << "TURN: turnAngle = " << turnAngle << endl;
}


// -----------------Detect Action------------------
ActionDetect::ActionDetect(){
    imageDetected = false;
    obstacleId = 0;
}

ActionDetect::ActionDetect(int obsId){
    imageDetected = false;
    obstacleId = obsId;
}

State* ActionDetect::takeAction(State* initState, Map& maps){
    vector<Obstacle>& obstacles = maps.getObstacles();
    Vertex position = *(initState->position);
    for(int i = 0; i < obstacles.size(); i++){
        Obstacle& o = obstacles[i];
        int faceDirection = initState->face_direction;
        bool correctOrientation = false;

        switch(faceDirection){
            case 0:
                correctOrientation = (o.face_direction == 180 && o.xGrid > position.xGrid);
            break;
            case 90:
                correctOrientation = (o.face_direction == 270 && o.yGrid > position.yGrid);
            break;
            case 180:
                correctOrientation = (o.face_direction == 0 && o.xGrid < position.xGrid);
            break;
            case 270:
                correctOrientation = (o.face_direction == 90 && o.yGrid < position.yGrid);
            break;
            default:
                correctOrientation = false;
        }
        if(correctOrientation && euclidean(position.xGrid, position.yGrid, o.xGrid, o.yGrid) <= MAX_IMAGE_VIEW_DISTANCE_GRID){ 
            obstacleId = o.id;
            imageDetected = true;
            State* endState = new State(initState->position, obstacleId, initState->face_direction, initState);
            return endState;
        }
    }
    return nullptr;
}

void ActionDetect::setObstacleId(int obstacleId){
    this->obstacleId = obstacleId;
}

int ActionDetect::getCost(State* initState, Map maps, Obstacle o){
    return cost;
}

// debug ActionDetect
void ActionDetect::printAction(){
    cout << "DETECT: obstacleId = " << obstacleId << (imageDetected? "detected": "!detected") << endl;
}



