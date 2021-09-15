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
        double face_direction;
        double gCost, hCost;
        string broughtByAction;
        State* prevState;
        State(Vertex* position, int obstacleSeen, double face_direction, State* prevState, string broughtByAction);
        
        // debug
        void printState();
};

class Action{
    public:
        virtual State* takeAction(State* initState, Map& maps) = 0;
        virtual bool canTakeAction(State* initState, Map& maps) = 0;    // 14/9/2021 JX: simple checks only. eg if move forward by 3 squares, check only the goal square. In actual fact, should check all 3 
        // only consider one because currently each step in A* involves moving one square
        virtual int getCost() = 0;
        virtual string generateActionString() = 0;

        // debug
        virtual void printAction() = 0;
};

class ActionForward : public Action{
    float travelDist;   // in cm
    int cost = 1;
    public:
        ActionForward();
        ActionForward(float travelDist);
        State* takeAction(State* initState, Map& maps);
        bool canTakeAction(State* initState, Map& maps);
        int getCost();
        string generateActionString();

        // debug
        void printAction();
};

class ActionReverse : public Action {
    float travelDist;
    int cost = 1;
    public:
        ActionReverse();
        ActionReverse(float travelDist);
        State* takeAction(State* initState, Map& maps);
        bool canTakeAction(State* initState, Map& maps);
        int getCost();
        string generateActionString();

        // debug
        void printAction();
};

class ActionTurn : public Action{
    float turnAngle;
    int cost = 4;  // need to use mathematical calculation
    public:
        ActionTurn(float turnAngle);
        State* takeAction(State* initState, Map& maps);
        bool canTakeAction(State* initState, Map& maps);

        int getCost();
        string generateActionString();

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
        bool canTakeAction(State* initState, Map& maps);

        void setObstacleId(int obstacleId);
        int getCost();
        string generateActionString();

        // debug
        void printAction();
};

#endif