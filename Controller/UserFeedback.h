#ifndef USERFEEDBACK_H
#define USERFEEDBACK_H

#include "RewardChange.h"
#include "MOMDP.h"
#include "BeliefWithState.h"

class UserFeedback {

private:
    SharedPointer<MOMDP> problem;
    SharedPointer<BeliefWithState> belief;
    int previousAction;
    int feedbackSign;
    float impact;
    vector<RewardChange> improvements;
    void calculateImprovements();

public:
    UserFeedback();
    UserFeedback(SharedPointer<MOMDP> problem, SharedPointer<BeliefWithState> belief, int previousAction, bool negativeFeedback, float impact);
    vector<RewardChange> getImprovements();
    string toString();
};

#endif