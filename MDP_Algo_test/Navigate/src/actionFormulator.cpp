#include "actionFormulator.h"
#include <cstdlib>
#include <iostream>

using namespace std;

vector<Action*> FormulatorSmooth::convertToActualActions(vector<State*> states){
    vector<Action*> actions;
    if(states.size() <= 1) return actions;

    Vertex* initPosition;
    Vertex* nextPosition;
    for(int i = 1; i < states.size(); i++){
        initPosition = states[i-1]->position;
        nextPosition = states[i]->position;
        cout << "\niteration " << i << endl;
        initPosition->printVertex();
        nextPosition->printVertex();
        if(states[i-1]->face_direction == 0){
            if(abs(initPosition->xGrid - nextPosition->xGrid) == 1){
                actions.push_back(new ActionStraight(nextPosition->xGrid - initPosition->xGrid));
                printf("actions push %s \n", typeid(actions[0]).name());
            }
            else if(initPosition->xGrid == nextPosition->xGrid - TURN_FORWARD_LENGTH){
                int sign = (nextPosition->yGrid - initPosition->yGrid)/abs(nextPosition->yGrid - initPosition->yGrid);
                actions.push_back(new ActionTurn2By4(90*sign));
            }
            else if(initPosition->xGrid == nextPosition->xGrid + TURN_FORWARD_LENGTH){
                int sign = (nextPosition->yGrid - initPosition->yGrid)/abs(nextPosition->yGrid - initPosition->yGrid);
                actions.push_back(new ActionReverseTurn2By4(90*sign));
            }
        }
        else if(states[i-1]->face_direction == 90){
            if(abs(initPosition->yGrid - nextPosition->yGrid) == 1){
                actions.push_back(new ActionStraight(nextPosition->yGrid - initPosition->yGrid));
            }
            else if(initPosition->yGrid == nextPosition->yGrid - TURN_FORWARD_LENGTH){
                int sign = (initPosition->xGrid - nextPosition->xGrid)/abs(initPosition->xGrid - nextPosition->xGrid);
                actions.push_back(new ActionTurn2By4(90*sign));
            }
            else if(initPosition->yGrid == nextPosition->yGrid + TURN_FORWARD_LENGTH){
                int sign = (initPosition->xGrid - nextPosition->xGrid)/abs(initPosition->xGrid - nextPosition->xGrid);
                actions.push_back(new ActionReverseTurn2By4(90*sign));
            }
        }
        else if(states[i-1]->face_direction == 180){
            if(abs(initPosition->xGrid - nextPosition->xGrid) == 1){
                actions.push_back(new ActionStraight(initPosition->xGrid - nextPosition->xGrid));
            }
            else if(initPosition->xGrid == nextPosition->xGrid - TURN_FORWARD_LENGTH){
                int sign = (initPosition->yGrid - nextPosition->yGrid)/abs(initPosition->yGrid - nextPosition->yGrid);
                actions.push_back(new ActionReverseTurn2By4(90*sign));
            }
            else if(initPosition->xGrid == nextPosition->xGrid + TURN_FORWARD_LENGTH){
                int sign = (initPosition->yGrid - nextPosition->yGrid)/abs(initPosition->yGrid - nextPosition->yGrid);
                actions.push_back(new ActionTurn2By4(90*sign));
            }
        }
        else if(states[i-1]->face_direction == 270){
            if(abs(initPosition->yGrid - nextPosition->yGrid) == 1){
                actions.push_back(new ActionStraight(initPosition->yGrid - nextPosition->yGrid));
            }
            else if(initPosition->yGrid == nextPosition->yGrid + TURN_FORWARD_LENGTH){
                int sign = (nextPosition->xGrid - initPosition->xGrid)/abs(nextPosition->xGrid - initPosition->xGrid);
                actions.push_back(new ActionTurn2By4(90*sign));
            }
            else if(initPosition->yGrid == nextPosition->yGrid - TURN_FORWARD_LENGTH){
                int sign = (nextPosition->xGrid - initPosition->xGrid)/abs(nextPosition->xGrid - initPosition->xGrid);
                actions.push_back(new ActionReverseTurn2By4(90*sign));
            }
        }
    }

//     for(int i = 0; i < actions.size(); i++){
//         actions[i]->printAction();
//     }
    return actions;
}

vector<Action*> FormulatorShorten::convertToActualActions(vector<State*> states){
    vector<Action*> actionsRaw = FormulatorSmooth::convertToActualActions(states);
    vector<Action*> actions;
    int accumulatedStraightDistance = 0;

    ActionStraight* as;
    for(int i = 0; i < actionsRaw.size(); i++){
        as = dynamic_cast<ActionStraight*>(actionsRaw[i]);
        if(as == nullptr){
            if(accumulatedStraightDistance != 0){
                as = new ActionStraight(accumulatedStraightDistance);
                actions.push_back(as);
                accumulatedStraightDistance = 0;
            }
            actions.push_back(actionsRaw[i]);
        }
        else{
            accumulatedStraightDistance+=as->getTravelDistGrid();
            //zy added to account for the last state
            if(i+1==actionsRaw.size()){
                if(accumulatedStraightDistance!=0){
                    as = new ActionStraight(accumulatedStraightDistance);
                    actions.push_back(as);
                    accumulatedStraightDistance = 0;
                }
                else{
                actions.push_back(actionsRaw[i]);
                }
            }
        }
    }
    for(int i = 0; i < actions.size(); i++){
        actions[i]->printAction();
    }
    return actions;
}
