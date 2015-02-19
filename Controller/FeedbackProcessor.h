#ifndef FEEDBACKPROCESSOR_H
#define FEEDBACKPROCESSOR_H

#include "Feedback.h"
#include "RewardChange.h"
#include "Controller.h"
#include "MOMDP.h"

class FeedbackProcessor {

private:
    Problem problem;
    map<string, RewardChange> rewardChangesInUse;
    vector<RewardChange> rewardChangesHistory;

    vector<RewardChange> calculateRewardChanges_simple(Feedback feedback);
    vector<RewardChange> calculateRewardChanges_enhanced(Feedback feedback);

public:
    FeedbackProcessor(Problem problem);

    vector<RewardChange> getRewardChangesForFeedback(Feedback feedback);
    void applyRewardChange(RewardChange rewardChange);
    void applyRewardChanges(vector<RewardChange> rewardChanges);

    bool hasRewardChanges();
    vector<RewardChange> getRewardChangesInUse();
    vector<RewardChange> getHistoryOfRewardChanges();

    REAL_VALUE getRewardForStateAndAction(int state, int action);
    vector<REAL_VALUE> getRewardsForState(int state);
};

#endif
