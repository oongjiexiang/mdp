#ifndef ACTION_FORMULATOR_H
#define ACTION_FORMULATOR_H

#include "action.h"

class ActionFormulator{
    public:
        virtual vector<Action*> convertToActualActions(vector<State*> states) = 0;
};

class FormulatorSmooth : public ActionFormulator{
    public:
        vector<Action*> convertToActualActions(vector<State*> states);
};

#endif