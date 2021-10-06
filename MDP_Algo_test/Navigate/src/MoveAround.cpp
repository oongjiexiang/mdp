#include "moveAround.h"
#include "action.h"
#include <iostream>

using namespace std;

MoveAround::MoveAround(vector<Obstacle> obstacles){
    astar = new aStar(obstacles,0);
}

vector<State*> MoveAround::generatePath(State* robotInitState, Obstacle obstacle){
    Obstacle obs = obstacle;
    float distance; // Placeholder to keep distance
    vector<State*> subgoalStates;
    State* newInitState = robotInitState;

    for(int i = 0; i < 4; i++){

        astar->changeObstacleFace(obs, (obs.face_direction + 90)%360);  // make robot turn anticlockwise
        obs.face_direction = (obs.face_direction + 90)%360;
        newInitState = new State(newInitState->position, newInitState->face_direction, newInitState);
        newInitState = astar->search(newInitState, obs, &distance, &subgoalStates);
        states.insert(states.end(), subgoalStates.begin(), subgoalStates.end());
    }
    // for(int i = 0; i < states.size(); i++){
    //     states[i]->printState();
    // }
    return states;
}
