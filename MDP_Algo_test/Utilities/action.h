#ifndef ACTION_H
#define ACTION_H

#include "component.h"
#include "config.h"
#include <iostream>

using namespace std;

// to illustrate each step. Will add more attributes when necessary
class State{
    public:
        Vertex* position;
        int obstacleSeen = 0;
        int face_direction;
        float gCost, hCost;
        State* prevState;
        State(Vertex* position, int obstacleSeen, int face_direction, State* prevState);

        // debug
        void printState();
};

class Action{
    public:
        virtual State* takeAction(State* initState, Map& maps) = 0;
        // only consider one because currently each step in A* involves moving one square
        virtual int getCost(State* initState, Map maps, Obstacle o) = 0;
        //virtual int getTravel();
        // debug
        virtual void printAction() = 0;
};

class ActionStraight : public Action{
    float travelDistGrid;   // in cm
    int cost = 1;
    public:
        ActionStraight();
        ActionStraight(float travelDistGrid);
        State* takeAction(State* initState, Map& maps);

        int getCost(State* initState, Map maps, Obstacle o);
        //int getTravel();
        // debug
        void printAction();
};

class ActionTurn : public Action{
    int turnAngle;
    int cost = 4;  // need to use mathematical calculation
    public:
        ActionTurn(int turnAngle);
        State* takeAction(State* initState, Map& maps);

        int getCost(State* initState, Map maps, Obstacle o);
        //int getTravel();
        // debug
        void printAction();
};

class ActionDetect : public Action{
    bool imageDetected;
    int obstacleId;
    int cost = 1;
    public:
        ActionDetect();
        ActionDetect(int obsId);
        State* takeAction(State* initState, Map& maps);

        void setObstacleId(int obstacleId);
        int getCost(State* initState, Map maps, Obstacle o);

        // debug
        void printAction();
};

#endif