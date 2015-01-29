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

    int feedbackID;

public:
    RewardChange();
    RewardChange(int state, int action, REAL_VALUE oldReward, REAL_VALUE newReward);
    RewardChange(int state, int action, REAL_VALUE oldReward, REAL_VALUE newReward, int feedbackID);

    int getState();
    int getAction();
    REAL_VALUE getOldReward();
    REAL_VALUE getNewReward();
    int getFeedbackID();

    string toString();
    string getActionAndStateString();
};

#endif
