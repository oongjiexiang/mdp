#include "navigate.h"
#include "moveAround.h"
#include <iostream>

bool stmDetectsObstacleInFront();
float stmCalibrate();
void rpiCaptureImage();

void Navigate::run(){
    Map grid;
    Vertex* curPosition = new Vertex(ROBOT_INIT_X_GRID, ROBOT_INIT_Y_GRID);
    State* curState = new State(curPosition, 0, ROBOT_INIT_FACEDIRECTION, nullptr);

    // Initialise possible actions
    Action* forward = new ActionStraight(1);
    Action* reverse = new ActionStraight(-1);

    // start detecting
    while(!stmDetectsObstacleInFront()){
        Vertex* curPosition = curState->position;
        cout << "debug: print map" << endl;
        // curPosition->printVertex();
        grid.printMap();
        int newYGrid = curPosition->yGrid + (int)(sin(M_PI/180*curState->face_direction));
        int newXGrid = curPosition->xGrid + (int)(cos(M_PI/180*curState->face_direction));
        bool reachBorder = false;
        if(!reachBorder && !grid.getVertexArray()[newXGrid][newYGrid]->safe){   // if that grid is still not visited
            curState = forward->takeAction(curState, grid);
        }
    }

    // once the obstacle is in front of the robot:
    cout << "Facing obstacle. Passing control to RPi and STM to navigate around" << endl;
    stmCalibrate(); // how to get distance from STM?
    rpiCaptureImage();

    // Create Obstacle
    vector<Obstacle> obstacles;
    // assume that robot will be 20cm away, hence +2
    Obstacle* onlyObs = new Obstacle(1, curState->position->xGrid + 2, curState->position->yGrid + 2, (curState->face_direction + 180)%360);
    obstacles.push_back(*onlyObs);

    // Generate Path
    MoveAround ma(obstacles);
    vector<State*> surroundStates = ma.generatePath(curState, *onlyObs);

    // debug
    for(int i = 0; i < surroundStates.size(); i++){
        surroundStates[i]->printState();
    }
    // debug end
}

bool stmDetectsObstacleInFront(){
    string test_ObstacleDetect;
    cout << "is obstacle detected?" << endl;
    cin >> test_ObstacleDetect;
    if(test_ObstacleDetect == "0") return false;
    return true;
}

float stmCalibrate(){   // calls STM to calibrate. STM returns the distance
    // JX to ZY: API to call STM. Please overwrite this with your function, and replace your function name in this file (Find -> Replace All)
    float distance;
    cout << "distance detected between obstacle and STM" << endl;
    cin >> distance;
    return distance;
}


void rpiCaptureImage(){
    cout << "Rpi to capture image" << endl;
}

