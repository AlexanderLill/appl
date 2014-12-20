#include "UserFeedback.h"

UserFeedback::UserFeedback() {
    cout << "UserFeedback()" << endl;
}

UserFeedback::UserFeedback(SharedPointer<MOMDP> problem, SharedPointer<BeliefWithState> belief, int previousAction, bool negativeFeedback, float impact) {
    cout << "UserFeedback(problem,belief," << previousAction << "," << negativeFeedback << "," << impact << ")" << endl;

    this->problem = problem;
    this->belief = belief;
    this->previousAction = previousAction;

    if (negativeFeedback) {
        this->feedbackSign = -1;
    } else {
        this->feedbackSign = 1;
    }

    this->impact = impact;
}

vector<RewardChange> UserFeedback::getImprovements() {
    if (improvements.size() == 0) {
        calculateImprovements();
    }
    return improvements;
}

string UserFeedback::toString() {
    ostringstream ss;
    ss << "UserFeedback - belief: " << belief->bvec->ToString() << ", previousAction: " << previousAction << ", feedbackSign=" << feedbackSign << ", impact: " << impact;

    vector<RewardChange>::iterator  vi;

    for (vi = improvements.begin(); vi != improvements.end(); ++vi) {
        ss << vi->toString() << endl;
    }

    return ss.str();
}

void UserFeedback::calculateImprovements() {

    vector<SparseVector_Entry>::iterator  vi;
    int state = 0;
    REAL_VALUE probability = 0.0;
    REAL_VALUE oldReward = 0.0;
    REAL_VALUE diff = 0.0;
    REAL_VALUE newReward = 0.0;

    // Iterate over all entries of the current belief
    for (vi = belief->bvec->data.begin(); vi != belief->bvec->data.end(); ++vi) {

        state = vi->index;
        probability = vi->value;
        oldReward = (*(problem->rewards->getMatrix(belief->sval)))(state, previousAction);
        diff = probability * impact * oldReward * feedbackSign;
        newReward = oldReward + diff;

        cout << "state=" << state << ", probability=" << probability << ", action=" << previousAction << ", oldReward=" << oldReward << ", feedbackSign=" << feedbackSign << ", diff=" << diff << ", newReward=" << newReward << endl;

        improvements.push_back(RewardChange(state, previousAction, oldReward, newReward));
    }

}