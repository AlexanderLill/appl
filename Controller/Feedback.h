#ifndef USERFEEDBACK_H
#define USERFEEDBACK_H

#include "BeliefWithState.h"
#include "Controller.h"
#include <iostream>
#include <sstream>

using namespace std;

class Feedback {

public:
    enum Type { POSITIVE = 1, NEGATIVE = -1 };

private:
    Belief belief;
    int previousAction;
    Type type;
    float impact;
    int id;

    static int idcounter;

public:
    Feedback(Belief belief, int previousAction, Type type, float impact);

    Belief getBelief();
    int getPreviousAction();
    Type getType();
    float getImpact();
    int getID();

    string toString();
};

#endif
