#include "RewardChange.h"

RewardChange::RewardChange() : state(0), action(0), oldReward(0.0), newReward(0.0) {
    cout << "RewardChange()" << endl;
}

RewardChange::RewardChange(int state, int action, REAL_VALUE oldReward, REAL_VALUE newReward) {
    cout << "RewardChange(" << state << "," << action << "," << oldReward << "," << newReward << ")" << endl;

    this->state = state;
    this->action = action;
    this->oldReward = oldReward;
    this->newReward = newReward;
}

void RewardChange::applyToProblem(SharedPointer<MOMDP> problem) {
    cout << "applyToProblem()" << endl;

    (problem->rewards->getMatrix(0))->changeValue(state, action, newReward);
}

REAL_VALUE RewardChange::getNewReward() {
    return newReward;
}

string RewardChange::toString() {
    ostringstream ss;
    ss << "RewardChange - state: " << state << ", action: " << action << ", oldReward: " << oldReward << ", newReward: " << newReward;
    return ss.str();
}

string RewardChange::getInstanceString() {
    ostringstream ss;
    ss << "a" << action << " s" << state;
    return ss.str();
}
