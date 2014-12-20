#ifndef REWARDCHANGE_H
#define REWARDCHANGE_H

#include "MOMDP.h"
#include "Const.h"

class RewardChange {

private:
    int state;
    int action;
    REAL_VALUE oldReward;
    REAL_VALUE newReward;

public:
    RewardChange();
    RewardChange(int state, int action, REAL_VALUE oldReward, REAL_VALUE newReward);
    void applyToProblem(SharedPointer<MOMDP> problem);
    REAL_VALUE getNewReward();
    string toString();
    string getInstanceString();
};

#endif