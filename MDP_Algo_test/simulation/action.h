#ifndef ACTION_H
#define ACTION_H

#include "component.h"
#include "config.h"

using namespace std;

// to illustrate each step. Will add more attributes when necessary
class State{
    public:
        Vertex* position;
        int obstacleSeen = 0;
        double face_direction;
        double gCost, hCost;
        State* prevState; // should add an action here
        Action* broughtBy;
        State(Vertex position, int obstacleSeen, double face_direction, State* prevState, Action* broughtBy);
};

class Action{
    public:
        virtual State takeAction(State* initState, Map& maps) = 0;
        virtual bool canTakeAction(State* initState, Map& maps) = 0;
        virtual int getCost() = 0;
};

class ActionForward : public Action{
    float travelDist;
    int cost;
    public:
        ActionForward(float travelDist);
        State takeAction(State* initState, Map& maps);
        bool canTakeAction(State* initState, Map& maps);
        int getCost();
};

class ActionReverse : public Action {
    float travelDist;
    int cost = 1;
    public:
        ActionReverse(float traverseDist);
        State takeAction(State* initState, Map& maps);
        bool canTakeAction(State* initState, Map& maps);
        int getCost();
};

class ActionTurn : public Action{
    float turnAngle;
    int cost = 4;  // need to use mathematical calculation
    public:
        ActionTurn(float turnAngle);
        State takeAction(State* initState, Map& maps);
        bool canTakeAction(State* initState, Map& maps);
        int getCost();
};

class ActionDetect : public Action{
    bool imageDetected;
    int obstacleId;
    int cost = 1;
    public:
        ActionDetect();
        State takeAction(State* initState, Map& maps);
        bool canTakeAction(State* initState, Map& maps);
        int getCost();
};

#endif