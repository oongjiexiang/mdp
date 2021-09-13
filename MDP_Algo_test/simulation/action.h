#ifndef ACTION_H
#define ACTION_H

#include "component.h"
#include "config.h"

using namespace std;

// to illustrate each step. Will add more attributes when necessary
class Action{
    public:
        virtual State takeAction(State& initState, Map& maps) = 0;
        virtual bool canTakeAction(State& initState, Map& maps) = 0;
};

class ActionForward : public Action{
    float travelDist;
    public:
        ActionForward(float travelDist);
        State takeAction(State* initState, Map& maps);
        bool canTakeAction(State* initState, Map& maps);
};

class ActionReverse : public Action {
    float travelDist;
    public:
        ActionReverse(float traverseDist);
        State takeAction(State* initState, Map& maps);
        bool canTakeAction(State* initState, Map& maps);
};

class ActionTurn : public Action{
    float turnAngle;
    public:
        ActionTurn(float turnAngle);
        State takeAction(State* initState, Map& maps);
        bool canTakeAction(State* initState, Map& maps);
};

class ActionDetect : public Action{
    bool imageDetected;
    int obstacleId;
    public:
        ActionDetect();
        State takeAction(State* initState, Map& maps);
        bool canTakeAction(State* initState, Map& maps);
};

#endif