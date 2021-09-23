#ifndef MOVE_AROUND_H
#define MOVE_AROUND_H

#include "asearch.h"
#include <iostream>

using namespace std;

class MoveAround{
    vector<Action*> actions;
    vector<State*> states;
    int faceDirectionHasImage;
    aStar* astar;
    public:
        MoveAround(vector<Obstacle> obstacles);
        vector<State*> generatePath(State* robotInitState, Obstacle obstacle);
};

#endif