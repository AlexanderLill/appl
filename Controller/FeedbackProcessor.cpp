#include "FeedbackProcessor.h"

FeedbackProcessor::FeedbackProcessor(SharedPointer<MOMDP> problem)
    : problem(problem)
{
    cout << "FeedbackProcessor(problem)" << endl;
}

vector<RewardChange> FeedbackProcessor::getRewardChangesForFeedback(Feedback feedback)
{
    //return calculateRewardChanges_simple(feedback);
    return calculateRewardChanges_enhanced(feedback);
}

vector<RewardChange> FeedbackProcessor::calculateRewardChanges_simple(Feedback feedback)
{
    // Result vector
    vector<RewardChange> rewardChanges;

    // Iterate over all states in our belief
    vector<SparseVector_Entry>::iterator  vi;
    for (vi = feedback.getBelief()->bvec->data.begin(); vi != feedback.getBelief()->bvec->data.end(); ++vi) {

        int state              = vi->index;
        REAL_VALUE probability = vi->value;

        // Calculate reward change for previousAction
        REAL_VALUE oldReward   = (*(problem->rewards->getMatrix(feedback.getBelief()->sval)))(state, feedback.getPreviousAction());
        REAL_VALUE diff        = probability * feedback.getImpact() * oldReward * feedback.getType();
        REAL_VALUE newReward   = oldReward + diff;

        cout << "state=" << state << ", probability=" << probability
             << ", action=" << feedback.getPreviousAction() << ", oldReward=" << oldReward
             << ", feedbackSign=" << feedback.getType() << ", diff=" << diff
             << ", newReward=" << newReward << endl;

        rewardChanges.push_back(RewardChange(state,
                                             feedback.getPreviousAction(),
                                             oldReward,
                                             newReward,
                                             feedback.getID()));
    }

    return rewardChanges;
}

vector<RewardChange> FeedbackProcessor::calculateRewardChanges_enhanced(Feedback feedback)
{
    // Result vector
    vector<RewardChange> rewardChanges;

    // Iterate over all states in our belief
    vector<SparseVector_Entry>::iterator  vi;
    for (vi = feedback.getBelief()->bvec->data.begin(); vi != feedback.getBelief()->bvec->data.end(); ++vi) {

        int state              = vi->index;
        REAL_VALUE probability = vi->value;

        // Get all rewards for this state in format oldReward,newReward
        vector<REAL_VALUE> rewards = getRewardsForState(feedback.getBelief(), state);

        // iterate over rewards to get sum of old rewards
        float oldRewardSum = 0.0;
        vector<REAL_VALUE>::iterator it;

        for (it = rewards.begin(); it != rewards.end(); ++it) {
            oldRewardSum += *it;
        }

        // Calculate reward change for previousAction
        REAL_VALUE oldReward = rewards[feedback.getPreviousAction()];
        REAL_VALUE diff      = oldReward * probability * feedback.getImpact() * feedback.getType();
        REAL_VALUE newReward = oldReward + diff;

        cout << "state=" << state << ", probability=" << probability
             << ", action=" << feedback.getPreviousAction() << ", oldReward=" << oldReward
             << ", feedbackSign=" << feedback.getType() << ", diff=" << diff
             << ", newReward=" << newReward << endl;

        rewards[feedback.getPreviousAction()] = newReward;

        // calculate sum of new rewards
        float newRewardSum = oldRewardSum + diff;

        // calculate scaleFactor
        float scaleFactor = oldRewardSum / newRewardSum;

        // Create all the rewardChanges and add to rewardChanges
        for (it = rewards.begin(); it != rewards.end(); ++it) {
            REAL_VALUE newActionReward = *it * scaleFactor;
            rewardChanges.push_back(RewardChange(state, it - rewards.begin(), *it, newActionReward, feedback.getID()));
        }
    }

    return rewardChanges;
}

void FeedbackProcessor::applyRewardChanges(vector<RewardChange> rewardChanges)
{
    vector<RewardChange>::iterator  vi;

    for (vi = rewardChanges.begin(); vi != rewardChanges.end(); ++vi) {
        applyRewardChange(*vi);
    }
}

void FeedbackProcessor::applyRewardChange(RewardChange rewardChange)
{
    (problem->rewards->getMatrix(0))->changeValue(rewardChange.getState(),
                                                  rewardChange.getAction(),
                                                  rewardChange.getNewReward());

    rewardChangesInUse[rewardChange.getActionAndStateString()] = rewardChange;
    rewardChangesHistory.push_back(rewardChange);
}

bool FeedbackProcessor::hasRewardChanges()
{
    if (rewardChangesHistory.size() == 0) {
        return false;
    } else {
        return true;
    }
}

vector<RewardChange> FeedbackProcessor::getHistoryOfRewardChanges()
{
    return rewardChangesHistory;
}

vector<RewardChange> FeedbackProcessor::getRewardChangesInUse()
{
    vector<RewardChange> returnVector;
    map<string, RewardChange>::iterator  it;

    for(it = rewardChangesInUse.begin(); it != rewardChangesInUse.end(); ++it) {
    	returnVector.push_back(it->second);
    }

    return returnVector;
}

vector<REAL_VALUE> FeedbackProcessor::getRewardsForState(SharedPointer<BeliefWithState> belief, int state)
{
    vector<REAL_VALUE> rewards;

    for(int i = 0; i<problem->getNumActions(); ++i) {
        rewards.push_back((*(problem->rewards->getMatrix(belief->sval)))(state, i));
    }

    return rewards;
}
