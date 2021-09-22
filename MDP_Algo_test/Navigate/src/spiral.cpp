#include "../include/spiral.h"
#include <iostream>

int TURN_RADIUS_FORWARD_GRID = 1;
int TURN_RADIUS_HORIZONTAL_GRID = 3;

bool stmDetectsObstacleInFront();
float stmCalibrate();
void rpiCaptureImage();
void navigateBetweenSTMAndRPi();

void Spiral::run(){
    Map grid;
    Vertex* curPosition = new Vertex(ROBOT_INIT_X_GRID, ROBOT_INIT_Y_GRID);
    State* curState = new State(curPosition, 0, ROBOT_INIT_FACEDIRECTION, nullptr);

    // Initialise possible actions
    Action* forward = new ActionStraight(1);
    Action* reverse = new ActionStraight(-1);
    Action* turnLeft = new ActionTurn(90);
    Action* turnRight = new ActionTurn(-90);

    // Set robot's initial location as safe so that the first round of spiral grows inwards
    grid.getVertexArray()[ROBOT_INIT_X_GRID][ROBOT_INIT_Y_GRID - 1]->safe = true;
    grid.getVertexArray()[ROBOT_INIT_X_GRID][ROBOT_INIT_Y_GRID]->safe = true;

    // start detecting
    while(!stmDetectsObstacleInFront()){
        Vertex* curPosition = curState->position;
        cout << "debug: print map" << endl;
        grid.printMap();
        int newYGrid = curPosition->yGrid + (int)(sin(M_PI/180*curState->face_direction));
        int newXGrid = curPosition->xGrid + (int)(cos(M_PI/180*curState->face_direction));
        int yGrid2Step = curPosition->yGrid + 2*(int)(sin(M_PI/180*curState->face_direction));
        int xGrid2Step = curPosition->xGrid + 2*(int)(cos(M_PI/180*curState->face_direction));
        bool reachBorder = false;
        switch(curState->face_direction){
            case 0:
                reachBorder = (newXGrid >= X_GRID_COUNT - TURN_RADIUS_FORWARD_GRID - 1);
            break;
            case 90:
                reachBorder = (newYGrid >= Y_GRID_COUNT - TURN_RADIUS_FORWARD_GRID - 1);
            break;
            case 180:
                reachBorder = (newXGrid < TURN_RADIUS_FORWARD_GRID + 1);
            break;
            case 270:
                reachBorder = (newYGrid < TURN_RADIUS_FORWARD_GRID + 1);
            break;
            default:
                cout << "curState.face_direction in Spiral.run() is wrong" << endl;
                exit(0);
        }
        // if still can move forward
        // cout << "safe?" << endl;
        // cout << grid.getVertexArray()[newXGrid][newYGrid]->safe << endl;
        // cout << reachBorder << endl;
        if(!reachBorder && !grid.getVertexArray()[newXGrid][newYGrid]->safe && !grid.getVertexArray()[xGrid2Step][yGrid2Step]->safe){   // if that grid is still not visited
            curState = forward->takeAction(curState, grid);
        }
        // if not, turn right (inwards)
        else{
            curState = turnRight->takeAction(curState, grid);
        }
    }

    // once the obstacle is in front of the robot:
    cout << "Facing obstacle. Passing control to RPi and STM to navigate around" << endl;
    stmCalibrate();
    rpiCaptureImage();
    navigateBetweenSTMAndRPi();
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

void navigateBetweenSTMAndRPi(){
    cout << "sending control to RPi and STM to run navigating routine" << endl;
}

void rpiCaptureImage(){
    cout << "Rpi to capture image" << endl;
}

