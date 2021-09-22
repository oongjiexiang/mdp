#include "../include/checklistNavigate.h"

int TURN_RADIUS_FORWARD_GRID = 1;
int TURN_RADIUS_HORIZONTAL_GRID = 3;

int stmStatus();
float calibrate();

void ChecklistNavigate::run(){
    Map grid;
    Vertex* curPosition = new Vertex(ROBOT_INIT_X_GRID, ROBOT_INIT_Y_GRID);
    State* curState = new State(curPosition, 0, ROBOT_INIT_FACEDIRECTION, nullptr);

    // Initialise possible actions
    Action* forward = new ActionStraight(10);
    Action* reverse = new ActionStraight(-10);
    Action* turnLeft = new ActionTurn(90);
    Action* turnRight = new ActionTurn(-90);

    // start detecting
    while(stmStatus() != 6){        // Iteratively take two steps
        int status = stmStatus();   // Step 1: sense status

        switch(status){             // Step 2: take an action
            case 1: {// no obstacle detected, so sweep spirally
            // once the status != 1, this case should never be used anymore, because the robot should have detected, and must keep adjusting
            // 22/9/2021: JX considers to make this as the first while loop instead. But let's first try this
                Vertex* curPosition = curState->position;
                int newYGrid = curPosition->yGrid + (int)(sin(M_PI/180*curState->face_direction));
                int newXGrid = curPosition->xGrid + (int)(cos(M_PI/180*curState->face_direction));
                bool reachBorder = false;
                switch(curState->face_direction){
                    case 0:
                        reachBorder = (newXGrid >= X_GRID_COUNT - TURN_RADIUS_FORWARD_GRID);
                    break;
                    case 90:
                        reachBorder = (newYGrid >= Y_GRID_COUNT - TURN_RADIUS_FORWARD_GRID);
                    break;
                    case 180:
                        reachBorder = (newXGrid < TURN_RADIUS_FORWARD_GRID);
                    break;
                    case 270:
                        reachBorder = (newYGrid < TURN_RADIUS_FORWARD_GRID);
                    break;
                    default:
                        cout << "curState.face_direction in checklistNavigate.run() is wrong" << endl;
                        exit(0);
                }
                // if still can move forward
                if(!reachBorder && !grid.getVertexArray()[newXGrid][newYGrid]->safe){   // if that grid is still not visited
                    curState = forward->takeAction(curState, grid);
                }
                // if not, turn right (inwards)
                else{
                    curState = turnRight->takeAction(curState, grid);
                }

            break;}
            case 2: // should turn left for right ir to detect
                curState = forward->takeAction(curState, grid);         // Assumption: can detect obstacle at 20-30cm. Hence forward 10cm then turn
                curState = turnLeft->takeAction(curState, grid);
            break;
            case 3: // should turn right for left ir to detect
                curState = forward->takeAction(curState, grid);         // Assumption: can detect obstacle at 20-30cm. Hence forward 10cm then turn
                curState = turnRight->takeAction(curState, grid);
            break;
            case 4: // should turn left to get closer
                curState = turnLeft->takeAction(curState, grid);
            break;
            case 5: // should turn right to get closer
                curState = turnRight->takeAction(curState, grid);
            break;
            default:
                cout << "STM sends different status. Check with STM Team" << endl;
                exit(0);
        }
    }


    // once the obstacle is in front of the robot:
    if(stmStatus() == 6){
        // 1. Update: get distance from STM
        float distFromObstacle = calibrate();

        // 2. Locate: find the obstacle's location
        float xCoor = (curPosition->xGrid + 0.5)*UNIT_LENGTH;
        float yCoor = (curPosition->yGrid + 0.5)*UNIT_LENGTH;
        xCoor+=distFromObstacle*(int)(sin(M_PI/180*curState->face_direction));
        yCoor+=distFromObstacle*(int)(cos(M_PI/180*curState->face_direction));
        Vertex* obsPosition = grid.findVertexByCoor(xCoor, yCoor);

        // find the obstacle's direction
        int obsFaceDirection = (curState->face_direction + 180)%360; // opposite face

        // 3. Create Obstacle
        vector<Obstacle> obstacles;
        Obstacle onlyObs(1, obsPosition->xGrid, obsPosition->yGrid, obsFaceDirection);
        obstacles.push_back(onlyObs);

        // 4. Generate Path
        MoveAround ma(obstacles);
        ma.generatePath(curState, onlyObs); // JX to ZY: how to execute the path?
    }
}

int stmStatus(){    // JX to ZY: I just need a function to return [1,6]. This function's implementation is just for me to compile. 
    //Please overwrite it with yours and replace this function's name
    if("STM signals no obstacle detected"){
        return 1;
    }
    if("STM signals robot should turn left so that right ir sensor detects obstacle too"){
        return 2;
    }
    if("STM signals should turn right"){
        return 3;
    }
    if("robot should turn left so that right ir sensor gets CLOSER to obstacle too"){
        return 4;
    }
    if("robot should turn right so that left ir sensor gets CLOSER to obstacle too"){
        return 5;
    }
    if("robot should stop and do image recognition"){
        return 6;
    }
}

float calibrate(){   // calls STM to calibrate. STM returns the distance
    // JX to ZY: API to call STM. Please overwrite this with your function, and replace your function name in this file (Find -> Replace All)
}


