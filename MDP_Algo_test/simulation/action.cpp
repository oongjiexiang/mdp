#include "action.h"
#include <cmath>
#include <iostream>

using namespace std;

// const float TURN_RADIUS = 34;   // to update regularly according to STM Team
const float TURN_RADIUS = 20;
const int TURN_RADIUS_GRID = (int)(ceil(TURN_RADIUS/UNIT_LENGTH));
const float MAX_IMAGE_VIEW_DISTANCE = 20;
const int MAX_IMAGE_VIEW_DISTANCE_GRID = (int)(ceil(MAX_IMAGE_VIEW_DISTANCE/UNIT_LENGTH));
const float MAX_IMAGE_VIEW_ANGLE = 180; // currently set as complete opposite. Will change in real application

// -------------Utility Euclidean distance-------------
double euclidean(double x1, double y1, double x2, double y2){
    return pow(pow((x1 - x2), 2.0) + pow((y1 - y2), 2.0), 0.5);
}


// State
State::State(Vertex* position, int obstacleSeen, double face_direction, State* prevState, string broughtByAction):
    position(position), obstacleSeen(obstacleSeen), face_direction(face_direction), prevState(prevState), broughtByAction(broughtByAction){}

// debug State
void State::printState(){
    cout << "--------------State---------------" << endl;
    position->printVertex();
    cout << "obstacleSeen: " << obstacleSeen << endl;
    cout << "faceDirection: " << face_direction << endl;
    cout << "action: " << broughtByAction << endl;
    cout << "--------------End State---------------" << endl;
}

// ------------------------Forward action------------------------
ActionForward::ActionForward(): travelDist(UNIT_LENGTH), cost(1) {}

ActionForward::ActionForward(float travelDist): travelDist(travelDist){
    cost = 1;
}

State* ActionForward::takeAction(State* initState, Map& maps){
    Vertex curPosition = *(initState->position);
    int newObstacleSeen = initState->obstacleSeen;

    int moveGridDistance = (int)(travelDist/UNIT_LENGTH);
    int newRow = curPosition.row + moveGridDistance*sin(M_PI/180*initState->face_direction);
    int newColumn = curPosition.column + moveGridDistance*cos(M_PI/180*initState->face_direction);

    Vertex* newPosition = new Vertex(newRow, newColumn);
    State* endState = new State(newPosition, newObstacleSeen, initState->face_direction, initState, this->generateActionString());

    return endState;
}

bool ActionForward::canTakeAction(State* initState, Map& maps){
    Vertex newPosition = *(initState->position);
    double faceDirection = initState->face_direction;

    int moveGridDistance = (int)travelDist/UNIT_LENGTH;
    int newRow = (int)(newPosition.row + moveGridDistance*sin(M_PI/180*faceDirection));
    int newCol = (int)(newPosition.column + moveGridDistance*cos(M_PI/180*faceDirection));
    if(maps.isValidGrid(newRow, newCol)){
        Vertex* cell = maps.findVertexByGrid(newRow, newCol);
        return !cell->is_obstacle && !cell->is_border;
    }
    return false;
}

int ActionForward::getCost(){
    return cost;
}

string ActionForward::generateActionString(){
    return "FORWARD " + to_string(travelDist);
}

// debug ActionForward
void ActionForward::printAction(){
    cout << "FORWARD: travelDist = " << travelDist << endl;
}


// ---------- Reverse action ------------
ActionReverse::ActionReverse(): travelDist(UNIT_LENGTH), cost(1) {}

ActionReverse::ActionReverse(float travelDist):
    travelDist(travelDist){}

State* ActionReverse::takeAction(State* initState, Map& maps){
    Vertex curPosition = *(initState->position);
    int newObstacleSeen = initState->obstacleSeen;

    int moveGridDistance = (int)(travelDist/UNIT_LENGTH);
    int newRow = curPosition.row - moveGridDistance*sin(M_PI/180*initState->face_direction);
    int newColumn = curPosition.column - moveGridDistance*cos(M_PI/180*initState->face_direction);
    
    Vertex* newPosition = new Vertex(newRow, newColumn);
    State* endState = new State(newPosition, newObstacleSeen, initState->face_direction, initState, this->generateActionString());
    return endState;
}

bool ActionReverse::canTakeAction(State* initState, Map& maps){
    Vertex newPosition = *(initState->position);
    double faceDirection = initState->face_direction;
    
    int moveGridDistance = (int)travelDist/UNIT_LENGTH;
    int newRow = (int)(newPosition.row - moveGridDistance*sin(M_PI/180*faceDirection));
    int newCol = (int)(newPosition.column - moveGridDistance*cos(M_PI/180*faceDirection));
    if(maps.isValidGrid(newRow, newCol)){
        Vertex* cell = maps.findVertexByGrid(newRow, newCol);
        return !cell->is_obstacle && !cell->is_border;
    }
    return false;
}

int ActionReverse::getCost(){
    return cost;
}

string ActionReverse::generateActionString(){
    return "REVERSE " + to_string(travelDist);
}

// debug ActionReverse
void ActionReverse::printAction(){
    cout << "REVERSE: travelDist = " << travelDist << endl;
}


// -------------------Turning Action----------------------
ActionTurn::ActionTurn(float turnAngle):
    turnAngle(turnAngle){}
    
State* ActionTurn::takeAction(State* initState, Map& maps){
    // assume that turnAngle is always 90 or -90
    float newX, newY, centerX, centerY;
    Vertex position = *(initState->position);
    float pivotY = position.row*UNIT_LENGTH + UNIT_LENGTH/2;
    float pivotX = position.column*UNIT_LENGTH + UNIT_LENGTH/2;
    double faceDirection = initState->face_direction;

    // determine turning center (xc, yc)
    switch((int)faceDirection){
        case 0:
            centerX = pivotX - TURN_RADIUS;
            centerY = pivotY;
        break;
        case 90:
            centerX = pivotX;
            centerY = pivotY - TURN_RADIUS;
        break;
        case 180:
            centerX = pivotX + TURN_RADIUS;
            centerY = pivotY;
        break;
        case -90:
            centerX = pivotX;
            centerY = pivotY + TURN_RADIUS;
        break;
        default:
            cout << "faceDirection is " << faceDirection << endl;
    }

    // T(xc, yc)R(turnAngle)T(-xc, -yx) affine transformation
    newX = (pivotX - centerX)*cos(M_PI/180*turnAngle) - (pivotY - centerY)*sin(M_PI/180*turnAngle) + centerX;
    newY = (pivotX - centerX)*sin(M_PI/180*turnAngle) - (pivotY - centerY)*cos(M_PI/180*turnAngle) + centerY;
    
    double newFaceDirection = (faceDirection + turnAngle);
    if(newFaceDirection > 180) newFaceDirection-=360;
    else if(newFaceDirection <= -180) newFaceDirection+=360;

    Vertex* newPosition = maps.findVertexByCoor(newX, newY);

    int newObstacleSeen = initState->obstacleSeen;
    State* endState = new State(newPosition, newObstacleSeen, newFaceDirection, initState, this->generateActionString());
    return endState;
}

bool ActionTurn::canTakeAction(State* initState, Map& maps){
    float newX, newY, centerX, centerY;
    Vertex position = *(initState->position);
    float pivotY = position.row*UNIT_LENGTH + UNIT_LENGTH/2;
    float pivotX = position.column*UNIT_LENGTH + UNIT_LENGTH/2;
    double faceDirection = initState->face_direction;

    // determine turning center (xc, yc)
    switch((int)faceDirection){
        case 0:
            centerX = pivotX - TURN_RADIUS;
            centerY = pivotY;
        break;
        case 90:
            centerX = pivotX;
            centerY = pivotY - TURN_RADIUS;
        break;
        case 180:
            centerX = pivotX + TURN_RADIUS;
            centerY = pivotY;
        break;
        case -90:
            centerX = pivotX;
            centerY = pivotY + TURN_RADIUS;
        break;
        default:
            cout << "faceDirection is " << faceDirection << endl;
    }

    // T(xc, yc)R(turnAngle)T(-xc, -yx) affine transformation
    newX = (pivotX - centerX)*cos(M_PI/180*turnAngle) - (pivotY - centerY)*sin(M_PI/180*turnAngle) + centerX;
    newY = (pivotX - centerX)*sin(M_PI/180*turnAngle) - (pivotY - centerY)*cos(M_PI/180*turnAngle) + centerY;
    
    double newFaceDirection = (faceDirection + turnAngle);
    if(newFaceDirection > 180) newFaceDirection-=360;
    else if(newFaceDirection <= -180) newFaceDirection+=360;

    Vertex newPosition;
    if(maps.isValidCoor(newX, newY)){
        newPosition = *maps.findVertexByCoor(newX, newY);
    }
    else return false;  // if out of map, return false

    // check if there is any obstacle
    for(int i = min(position.row, newPosition.row); i <= max(position.row, newPosition.row); i++){
        for(int j = min(position.column, newPosition.column); j <= max(position.column, newPosition.column); j++){
            if(maps.findVertexByGrid(i, j)->is_obstacle || maps.findVertexByGrid(i, j)->is_border) return false;
        }
    }
    return true;
}

string ActionTurn::generateActionString(){
    return "TURN " + to_string(turnAngle);
}

int ActionTurn::getCost(){
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
    imageDetected = true;       // actual interaction logic with RPi should be here
    vector<Obstacle>& obstacles = maps.getObstacles();
    for(int i = 0; i < obstacles.size(); i++){
        if(obstacles[i].id == obstacleId){
            obstacles[i].is_seen = true;
            State* endState = new State(initState->position, obstacleId, initState->face_direction, initState, this->generateActionString());
            return endState;
        }
    }
}

bool ActionDetect::canTakeAction(State* initState, Map& maps){     // actual interaction logic with RPi should be here
    vector<Obstacle>& obstacles = maps.getObstacles();
    Vertex position = *(initState->position);
    for(int i = 0; i < obstacles.size(); i++){
        Obstacle& o = obstacles[i];
        cout << euclidean(position.column, position.row, o.column, o.row) << " " << abs(initState->face_direction - o.face_direction) << endl;
        if(!o.is_seen && euclidean(position.column, position.row, o.column, o.row) <= MAX_IMAGE_VIEW_DISTANCE_GRID
        && abs(initState->face_direction - o.face_direction) >= MAX_IMAGE_VIEW_ANGLE){ // not really >=
            obstacleId = o.id;
            return true;
        }
    }
    return false;
}

int ActionDetect::getCost(){
    return cost;
}

string ActionDetect::generateActionString(){
    return "DETECT " + to_string(obstacleId);
}

// debug ActionDetect
void ActionDetect::printAction(){
    cout << "DETECT: obstacleId = " << obstacleId << (imageDetected? "detected": "!detected") << endl;
}



