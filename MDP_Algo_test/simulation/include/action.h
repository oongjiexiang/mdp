#ifndef ACTION_H
#define ACTION_H

#include "component.h"
#include "config.h"
#include <iostream>

using namespace std;

const int TURN_FORWARD_LENGTH = 2;
const int TURN_SIDE_LENGTH = 4;

// to illustrate each step. Will add more attributes when necessary
class State{
    public:
        Vertex* position;
        int face_direction;
        float gCost, hCost;
        State* prevState;
        State(Vertex* position, int face_direction, State* prevState);

        // debug
        void printState();
};

class Action{
    public:
        virtual State* takeAction(State* initState, Map& maps) = 0;
        // only consider one because currently each step in A* involves moving one square
        virtual int getCost(State* initState, Map maps, Obstacle o) = 0;

        // debug
        virtual void printAction() = 0;
};

class ActionStraight : public Action{
    int cost = 1;
    public:

        float travelDistGrid;   // in cm
        ActionStraight();
        ActionStraight(float travelDistGrid);
        State* takeAction(State* initState, Map& maps);

        int getCost(State* initState, Map maps, Obstacle o);
        int getTravelDistGrid();

        // debug
        void printAction();
};

class ActionTurn : public Action{
    int cost = 4;  // need to use mathematical calculation
    int turnAngle;
    public:
        ActionTurn(int turnAngle);
        State* takeAction(State* initState, Map& maps);
        int getTurnAngle();
        int getCost(State* initState, Map maps, Obstacle o);

        // debug
        void printAction();
};

class ActionTurn2By4 : public Action{
    int cost = 4;  // need to use mathematical calculation
    int turnAngle;
    public:
        ActionTurn2By4(int turnAngle);
        State* takeAction(State* initState, Map& maps);
        int getTurnAngle();
        int getCost(State* initState, Map maps, Obstacle o);

        // debug
        void printAction();
};

class ActionReverseTurn2By4 : public Action{
    int turnAngle;
    int cost = 4;  // need to use mathematical calculation
    public:
        ActionReverseTurn2By4(int turnAngle);
        State* takeAction(State* initState, Map& maps);
        int getTurnAngle();
        int getCost(State* initState, Map maps, Obstacle o);

        // debug
        void printAction();
};

#endif
