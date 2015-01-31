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
    void applyRewardChanges(vector<RewardChange> rewardChanges);
    void applyRewardChange(RewardChange rewardChange);

    bool hasRewardChanges();
    vector<RewardChange> getHistoryOfRewardChanges();
    vector<RewardChange> getRewardChangesInUse();

    REAL_VALUE getRewardForStateAndAction(Belief belief, int state, int action);
    vector<REAL_VALUE> getRewardsForState(Belief belief, int state);
};

#endif
