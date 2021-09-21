#include "../../Simulation/asearch.h"
#include <iostream>

using namespace std;

class MoveAround{
    vector<Action*> actions;
    vector<State*> states;
    int faceDirectionHasImage;
    aStar* astar;
    public:
        MoveAround(vector<Obstacle> obstacles);
        void generatePath(State* robotInitState, Obstacle obstacle);
};