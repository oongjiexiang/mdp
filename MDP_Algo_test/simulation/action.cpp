#include "action.h"
#include <cmath>


const float TURN_RADIUS = 34;
const int TURN_RADIUS_GRID = (int)(ceil(TURN_RADIUS/UNIT_LENGTH));
const float MAX_IMAGE_VIEW_DISTANCE = 20;
const int MAX_IMAGE_VIEW_DISTANCE_GRID = (int)(ceil(MAX_IMAGE_VIEW_DISTANCE/UNIT_LENGTH));
const float MAX_IMAGE_VIEW_ANGLE = 180; // currently set as complete opposite. Will change in real application

// State
State::State(Vertex position, int obstacleSeen, double face_direction, State* prevState, Action* broughtBy):
    position(&position), obstacleSeen(obstacleSeen), face_direction(face_direction), prevState(prevState), broughtBy(broughtBy){}

// Forward action
ActionForward::ActionForward(float travelDist): 
    travelDist(travelDist){}

State ActionForward::takeAction(State* initState, Map& maps){
    Vertex newPosition = *(initState->position);
    int newObstacleSeen = initState->obstacleSeen;

    int moveGridDistance = (int)travelDist/UNIT_LENGTH;
    newPosition.row+=moveGridDistance*sin(initState->face_direction);
    newPosition.column+=moveGridDistance*cos(initState->face_direction);
    State endState(newPosition, newObstacleSeen, initState->face_direction, initState, this);
    return endState;
}
bool ActionForward::canTakeAction(State* initState, Map& maps){
    Vertex newPosition = *(initState->position);
    double faceDirection = initState->face_direction;

    int moveGridDistance = (int)travelDist/UNIT_LENGTH;
    int newRow = (int)(newPosition.row + moveGridDistance*sin(faceDirection));
    int newCol = (int)(newPosition.column + moveGridDistance*cos(faceDirection));
    if(maps.isValidGrid(newRow, newCol)){
        Vertex cell = maps.findVertexByGrid(newRow, newCol);
        return !cell.is_obstacle && !cell.is_border;
    }
    return false;
}
int ActionForward::getCost(){
    return cost;
}

// Reverse action
ActionReverse::ActionReverse(float traverseDist):
    travelDist(travelDist){}

State ActionReverse::takeAction(State* initState, Map& maps){
    Vertex newPosition = *(initState->position);
    int newObstacleSeen = initState->obstacleSeen;

    int moveGridDistance = (int)travelDist/UNIT_LENGTH;
    newPosition.row-=moveGridDistance*sin(initState->face_direction);
    newPosition.column-=moveGridDistance*cos(initState->face_direction);
    State endState(newPosition, newObstacleSeen, initState->face_direction, initState, this);
    return endState;
}
bool ActionReverse::canTakeAction(State* initState, Map& maps){
    Vertex newPosition = *(initState->position);
    double faceDirection = initState->face_direction;
    
    int moveGridDistance = (int)travelDist/UNIT_LENGTH;
    int newRow = (int)(newPosition.row - moveGridDistance*sin(faceDirection));
    int newCol = (int)(newPosition.column - moveGridDistance*cos(faceDirection));
    if(maps.isValidGrid(newRow, newCol)){
        Vertex cell = maps.findVertexByGrid(newRow, newCol);
        return !cell.is_obstacle && !cell.is_border;
    }
    return false;
}
int ActionReverse::getCost(){
    return cost;
}

// Turning Action
ActionTurn::ActionTurn(float turnAngle):
    turnAngle(turnAngle){}
    
State ActionTurn::takeAction(State* initState, Map& maps){
    // assume that turnAngle is always 90 or -90
    Vertex position = *(initState->position);
    float pivotX = position.row*UNIT_LENGTH + BOUNDARY_SIZE/2;
    float pivotY = position.column*UNIT_LENGTH + BOUNDARY_SIZE/2;
    float newX, newY;
    double faceDirection = initState->face_direction;

    if(turnAngle < 0){
        newX = pivotX - TURN_RADIUS*cos(faceDirection);
        newY = pivotY - TURN_RADIUS*sin(faceDirection);
    }
    else{
        newX = pivotX + TURN_RADIUS*cos(faceDirection);
        newY = pivotY + TURN_RADIUS*sin(faceDirection);
    }
    
    double newFaceDirection = (faceDirection + turnAngle);
    if(newFaceDirection > 180) newFaceDirection-=360;

    Vertex newPosition = maps.findVertexByCoor(newX, newY);

    int newObstacleSeen = initState->obstacleSeen;
    State endState(newPosition, newObstacleSeen, newFaceDirection, initState, this);
    return endState;
}
bool ActionTurn::canTakeAction(State* initState, Map& maps){
    Vertex position = *(initState->position);
    float pivotX = position.row*UNIT_LENGTH + BOUNDARY_SIZE/2;
    float pivotY = position.column*UNIT_LENGTH + BOUNDARY_SIZE/2;
    float newX, newY;
    double faceDirection = initState->face_direction;

    if(turnAngle < 0){
        newX = pivotX - TURN_RADIUS*cos(faceDirection);
        newY = pivotY - TURN_RADIUS*sin(faceDirection);
    }
    else{
        newX = pivotX + TURN_RADIUS*cos(faceDirection);
        newY = pivotY + TURN_RADIUS*sin(faceDirection);
    }
    
    double newFaceDirection = (faceDirection + turnAngle);
    if(newFaceDirection > 180) newFaceDirection-=360;

    Vertex newPosition;
    if(maps.isValidCoor(newX, newY)){
        newPosition = maps.findVertexByCoor(newX, newY);
    }
    else return false;  // if out of map, return false

    // check if there is any obstacle
    for(int i = min(position.row, newPosition.row); i <= max(position.row, newPosition.row); i++){
        for(int j = min(position.column, newPosition.column); j <= max(position.column, newPosition.column); j++){
            if(maps.findVertexByGrid(i, j).is_obstacle || maps.findVertexByGrid(i, j).is_border) return false;
        }
    }
    return true;
}
int ActionDetect::getCost(){
    return cost;
}

// Detect Action
ActionDetect::ActionDetect(){
    imageDetected = false;
    obstacleId = 0;
}
State ActionDetect::takeAction(State* initState, Map& maps){
    imageDetected = true;       // actual interaction logic with RPi should be here
    vector<Obstacle>& obstacles = maps.getObstacles();
    for(int i = 0; i < obstacles.size(); i++){
        if(obstacles[i].id == obstacleId){
            obstacles[i].is_seen = true;
            break;
        }
    }
}
bool ActionDetect::canTakeAction(State* initState, Map& maps){     // actual interaction logic with RPi should be here
    vector<Obstacle>& obstacles = maps.getObstacles();
    Vertex position = *(initState->position);
    for(int i = 0; i < obstacles.size(); i++){
        Obstacle& o = obstacles[i];
        if(euclidean(position.row, position.column, o.row, o.column) <= MAX_IMAGE_VIEW_DISTANCE_GRID
        && abs(initState->face_direction - o.face_direction) >= MAX_IMAGE_VIEW_ANGLE
        && !o.is_seen){ // not really >=
            obstacleId = o.id;
            return true;
        }
    }
    return false;
}
int ActionDetect::getCost(){
    return cost;
}


// Utility Euclidean distance
double euclidean(double x1, double y1, double x2, double y2){
    return pow(pow((x1 - x2), 2.0) + pow((y1 - y2), 2.0), 0.5);
}
