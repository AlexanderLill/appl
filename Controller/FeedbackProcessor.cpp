#include "FeedbackProcessor.h"

FeedbackProcessor::FeedbackProcessor(Problem problem)
    : problem(problem)
{
    DEBUG_TRACE( cout << "FeedbackProcessor(problem)" << endl; );
}

vector<RewardChange> FeedbackProcessor::getRewardChangesForFeedback(Feedback feedback)
{
    // Select 1 from currently 2 methods to calculate RewardChanges

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

        // Initialize state and its probability
        int state              = vi->index;
        REAL_VALUE probability = vi->value;

        // Calculate reward change for previousAction
        REAL_VALUE oldReward   = getRewardForStateAndAction(feedback.getBelief(), state, feedback.getPreviousAction());
        REAL_VALUE diff        = probability * feedback.getImpact() * oldReward * feedback.getType();
        REAL_VALUE newReward   = oldReward + diff;

        DEBUG_TRACE( cout << "state=" << state
                          << ", probability=" << probability
                          << ", action=" << feedback.getPreviousAction()
                          << ", oldReward=" << oldReward
                          << ", feedbackSign=" << feedback.getType()
                          << ", diff=" << diff
                          << ", newReward=" << newReward << endl; );

        // Create reward change
        RewardChange rc = RewardChange(state, feedback.getPreviousAction(),
                                       oldReward, newReward,
                                       feedback.getID());

        cout << "Created new RewardChange: " << rc.toString();

        // Put reward change in the list of reward changes
        rewardChanges.push_back(rc);
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

        // Initialize state and its probability
        int state              = vi->index;
        REAL_VALUE probability = vi->value;

        // Get all rewards for this state in format oldReward,newReward
        vector<REAL_VALUE> rewards = getRewardsForState(feedback.getBelief(), state);

        // Loop vars
        float oldRewardSum = 0.0;
        vector<REAL_VALUE>::iterator it;

        // Iterate over rewards to get sum of old rewards
        for (it = rewards.begin(); it != rewards.end(); ++it) {
            oldRewardSum += *it;
        }

        // Calculate new reward for previousAction
        REAL_VALUE oldReward = rewards[feedback.getPreviousAction()];
        REAL_VALUE diff      = oldReward * probability * feedback.getImpact() * feedback.getType();
        REAL_VALUE newReward = oldReward + diff;

        DEBUG_TRACE( cout << "state=" << state
                          << ", probability=" << probability
                          << ", action=" << feedback.getPreviousAction()
                          << ", oldReward=" << oldReward
                          << ", feedbackSign=" << feedback.getType()
                          << ", diff=" << diff
                          << ", newReward=" << newReward << endl; );

        // Calculate sum of new rewards
        float newRewardSum = oldRewardSum + diff;

        // Calculate scaleFactor
        float scaleFactor = oldRewardSum / newRewardSum;

        // Create all the reward changes
        for (it = rewards.begin(); it != rewards.end(); ++it) {

            // Current action
            int action = it - rewards.begin();

            // Old reward
            REAL_VALUE oldActionReward = *it;

            // Calculate new reward
            REAL_VALUE newActionReward;
            if (action == feedback.getPreviousAction()) {
                // If exectued action, scale new reward
                newActionReward = newReward * scaleFactor;
            } else {
                // If one of the other action, scale old reward
                newActionReward = oldActionReward * scaleFactor;
            }

            // Create reward change
            RewardChange rc = RewardChange(state, it - rewards.begin(),
                                           oldActionReward, newActionReward,
                                           feedback.getID());

            // Put reward change in the list of reward changes
            rewardChanges.push_back(rc);
        }
    }

    return rewardChanges;
}

void FeedbackProcessor::applyRewardChanges(vector<RewardChange> rewardChanges)
{
    // Loop var
    vector<RewardChange>::iterator  vi;

    // Iterate over all reward changes
    for (vi = rewardChanges.begin(); vi != rewardChanges.end(); ++vi) {
        applyRewardChange(*vi);
    }
}

void FeedbackProcessor::applyRewardChange(RewardChange rewardChange)
{
    // Change value in reward matrix
    (problem->rewards->getMatrix(0))->changeValue(rewardChange.getState(),
                                                  rewardChange.getAction(),
                                                  rewardChange.getNewReward());

    // Put reward change into map of rewardChangesInUse (maybe overwriting an old one)
    rewardChangesInUse[rewardChange.getActionAndStateString()] = rewardChange;

    // Put reward change in list of all applied reward changes
    rewardChangesHistory.push_back(rewardChange);
}

bool FeedbackProcessor::hasRewardChanges()
{
    // Check if any reward changes have been applied so far

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
    // Result vector
    vector<RewardChange> returnVector;

    // Loop var
    map<string, RewardChange>::iterator  it;

    // Transform the map rewardChangesInUse into a vector
    for(it = rewardChangesInUse.begin(); it != rewardChangesInUse.end(); ++it) {
    	returnVector.push_back(it->second);
    }

    return returnVector;
}

REAL_VALUE FeedbackProcessor::getRewardForStateAndAction(Belief belief,
                                                         int state,
                                                         int action)
{
    // Get reward from reward matrix
    return (*(problem->rewards->getMatrix(belief->sval)))(state, action);
}

vector<REAL_VALUE> FeedbackProcessor::getRewardsForState(Belief belief,
                                                         int state)
{
    // Result vector
    vector<REAL_VALUE> rewards;

    // Get reward for all actions in the current state
    for(int i = 0; i<problem->getNumActions(); ++i) {
        rewards.push_back(getRewardForStateAndAction(belief, state, i));
    }

    return rewards;
}
