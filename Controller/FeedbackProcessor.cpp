#include "FeedbackProcessor.h"

FeedbackProcessor::FeedbackProcessor(SharedPointer<MOMDP> problem)
    : problem(problem)
{
    cout << "FeedbackProcessor(problem)" << endl;
}

vector<RewardChange> FeedbackProcessor::getRewardChangesForFeedback(Feedback feedback)
{
    vector<RewardChange> rewardChanges;

    SharedPointer<BeliefWithState> belief = feedback.getBelief();
    float impact = feedback.getImpact();
    short feedbackSign = feedback.getType();
    int previousAction = feedback.getPreviousAction();
    int feedbackID = feedback.getID();

    vector<SparseVector_Entry>::iterator  vi;
    int state = 0;
    REAL_VALUE probability = 0.0;
    REAL_VALUE oldReward = 0.0;
    REAL_VALUE diff = 0.0;
    REAL_VALUE newReward = 0.0;

    for (vi = belief->bvec->data.begin(); vi != belief->bvec->data.end(); ++vi) {

        state = vi->index;
        probability = vi->value;
        oldReward = (*(problem->rewards->getMatrix(belief->sval)))(state, previousAction);
        diff = probability * impact * oldReward * feedbackSign;
        newReward = oldReward + diff;

        cout << "state=" << state << ", probability=" << probability << ", action=" << previousAction << ", oldReward=" << oldReward << ", feedbackSign=" << feedbackSign << ", diff=" << diff << ", newReward=" << newReward << endl;

        rewardChanges.push_back(RewardChange(state, previousAction, oldReward, newReward, feedbackID));
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

bool FeedbackProcessor::hasRewardChanges() {
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
