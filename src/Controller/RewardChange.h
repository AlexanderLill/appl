#ifndef REWARDCHANGE_H
#define REWARDCHANGE_H

#include "Const.h"
#include <iostream>
#include <sstream>

using namespace std;

class RewardChange {

private:
    int state;
    int action;
    REAL_VALUE oldReward;
    REAL_VALUE newReward;
    int feedbackID;

public:
    RewardChange();
    RewardChange(int state, int action,
                 REAL_VALUE oldReward, REAL_VALUE newReward);
    RewardChange(int state, int action,
                 REAL_VALUE oldReward, REAL_VALUE newReward, int feedbackID);

    int getState();
    int getAction();
    REAL_VALUE getOldReward();
    REAL_VALUE getNewReward();
    int getFeedbackID();
    string getActionAndStateString();

    string toString();
};

#endif
