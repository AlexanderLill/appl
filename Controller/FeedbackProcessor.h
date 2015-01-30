#ifndef FEEDBACKPROCESSOR_H
#define FEEDBACKPROCESSOR_H

#include "Feedback.h"
#include "RewardChange.h"
#include "MOMDP.h"

class FeedbackProcessor {

private:
    SharedPointer<MOMDP> problem;
    map<string, RewardChange> rewardChangesInUse;
    vector<RewardChange> rewardChangesHistory;

    vector<RewardChange> calculateRewardChanges_simple(Feedback feedback);
    vector<RewardChange> calculateRewardChanges_enhanced(Feedback feedback);

public:
    FeedbackProcessor(SharedPointer<MOMDP> problem);

    vector<RewardChange> getRewardChangesForFeedback(Feedback feedback);
    void applyRewardChanges(vector<RewardChange> rewardChanges);
    void applyRewardChange(RewardChange rewardChange);

    bool hasRewardChanges();
    vector<RewardChange> getHistoryOfRewardChanges();
    vector<RewardChange> getRewardChangesInUse();

    REAL_VALUE getRewardForStateAndAction(SharedPointer<BeliefWithState> belief,
                                          int state, int action);
    vector<REAL_VALUE> getRewardsForState(SharedPointer<BeliefWithState> belief,
                                          int state);
};

#endif
