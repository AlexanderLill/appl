#ifndef USERFEEDBACK_H
#define USERFEEDBACK_H

#include "BeliefWithState.h"
#include <iostream>
#include <sstream>

class Feedback {

private:
    SharedPointer<BeliefWithState> belief;
    int previousAction;
    short feedbackSign;
    float impact;

public:
    enum Type { POSITIVE = 1, NEGATIVE = -1 };

    Feedback();
    Feedback(SharedPointer<BeliefWithState> belief, int previousAction, Type type, float impact);

    string toString();
};

#endif
