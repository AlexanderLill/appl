#include "Improvements.h"

void Improvements::addRewardChange(RewardChange rewardChange) {
    improvements[rewardChange.getInstanceString()] = rewardChange;
}

Improvements::Improvements() {
    cout << "Improvements()" << endl;
}

void Improvements::addUserFeedback(UserFeedback userFeedback) {
    vector<RewardChange> rewardChanges = userFeedback.getImprovements();

    vector<RewardChange>::iterator  vi;
    for (vi = rewardChanges.begin(); vi != rewardChanges.end(); ++vi) {
            addRewardChange(*vi);
    }
}

map<string, RewardChange> Improvements::getImprovements() {
    return improvements;
}

void Improvements::applyToProblem(SharedPointer<MOMDP> problem) {
    map<string, RewardChange>::iterator  vi;

    for (vi = improvements.begin(); vi != improvements.end(); ++vi) {
        vi->second.applyToProblem(problem);
    }
}

int Improvements::size() {
    return improvements.size();
}

string Improvements::toString() {
    ostringstream ss;
    map<string, RewardChange>::iterator  vi;
    for (vi = improvements.begin(); vi != improvements.end(); ++vi) {
            ss << vi->second.toString() << endl;
    }
    return ss.str();
}