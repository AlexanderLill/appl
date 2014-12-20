#ifndef IMPROVEMENTS_H
#define IMPROVEMENTS_H

#include "RewardChange.h"
#include "UserFeedback.h"
#include "MOMDP.h"

class Improvements {

private:
    map<string, RewardChange> improvements;
    void addRewardChange(RewardChange rewardChange);

public:
    Improvements();
    void addUserFeedback(UserFeedback userFeedback);
    map<string, RewardChange> getImprovements();
    void applyToProblem(SharedPointer<MOMDP> problem);
    int size();
    string toString();
};

#endif