#ifndef USERFEEDBACK_H
#define USERFEEDBACK_H

#include "BeliefWithState.h"
#include <iostream>
#include <sstream>

class Feedback {

public:
    enum Type { POSITIVE = 1, NEGATIVE = -1 };

private:
    SharedPointer<BeliefWithState> belief;
    int previousAction;
    Type type;
    float impact;
    int id;

    static int idcounter;

public:
    Feedback();
    Feedback(SharedPointer<BeliefWithState> belief, int previousAction, Type type, float impact);

    SharedPointer<BeliefWithState> getBelief();
    int getPreviousAction();
    Type getType();
    float getImpact();
    int getID();

    string toString();
};

#endif
