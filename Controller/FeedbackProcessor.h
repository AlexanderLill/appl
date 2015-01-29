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

public:
    FeedbackProcessor(SharedPointer<MOMDP> problem);

    vector<RewardChange> getRewardChangesForFeedback(Feedback feedback);
    void applyRewardChanges(vector<RewardChange> rewardChanges);
    void applyRewardChange(RewardChange rewardChange);
    bool hasRewardChanges();
    vector<RewardChange> getHistoryOfRewardChanges();
    vector<RewardChange> getRewardChangesInUse();
};

#endif
