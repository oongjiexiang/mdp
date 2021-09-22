#include "../include/actionFormulator.h"
#include <cstdlib>

vector<Action*> FormulatorSmooth::convertToActualActions(vector<State*> states){
    vector<Action*> actions;
    if(states.size() <= 1) return actions;

    Vertex* initPosition;
    Vertex* nextPosition;
    for(int i = 1; i < states.size(); i++){
        initPosition = states[i-1]->position;
        nextPosition = states[i]->position;
        if(states[i-1]->face_direction == 0){
            if(abs(initPosition->xGrid - nextPosition->xGrid) == 1){
                actions.push_back(new ActionStraight(nextPosition->xGrid - initPosition->xGrid));
            }
            else if(initPosition->yGrid != nextPosition->yGrid){
                int sign = (nextPosition->yGrid - initPosition->yGrid)/abs(nextPosition->yGrid - initPosition->yGrid);
                actions.push_back(new ActionTurn(90*sign));
            }
        }
        else if(states[i-1]->face_direction == 90){
            if(abs(initPosition->yGrid - nextPosition->yGrid) == 1){
                actions.push_back(new ActionStraight(nextPosition->yGrid - initPosition->yGrid));
            }
            else if(initPosition->xGrid != nextPosition->xGrid){
                int sign = (initPosition->xGrid - nextPosition->xGrid)/abs(initPosition->xGrid - nextPosition->xGrid);
                actions.push_back(new ActionTurn(90*sign));
            }
        }
        else if(states[i-1]->face_direction == 180){
            if(abs(initPosition->xGrid - nextPosition->xGrid) == 1){
                actions.push_back(new ActionStraight(initPosition->xGrid - nextPosition->xGrid));
            }
            else if(initPosition->yGrid != nextPosition->yGrid){
                int sign = (initPosition->yGrid - nextPosition->yGrid)/abs(initPosition->yGrid - nextPosition->yGrid);
                actions.push_back(new ActionTurn(90*sign));
            }
        }
        else if(states[i-1]->face_direction == 270){
            if(abs(initPosition->yGrid - nextPosition->yGrid) == 1){
                actions.push_back(new ActionStraight(initPosition->yGrid - nextPosition->yGrid));
            }
            else if(initPosition->xGrid != nextPosition->xGrid){
                int sign = (nextPosition->xGrid - initPosition->xGrid)/abs(nextPosition->xGrid - initPosition->xGrid);
                actions.push_back(new ActionTurn(90*sign));
            }
        }
        if(states[i-1]->obstacleSeen == 0 && states[i]->obstacleSeen != 0){
            actions.push_back(new ActionDetect(states[i]->obstacleSeen));
        }
    }

    for(int i = 0; i < actions.size(); i++){
        actions[i]->printAction();
    }
    return actions;
}