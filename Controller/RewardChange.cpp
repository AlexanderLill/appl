#include "RewardChange.h"

RewardChange::RewardChange()
    : state(-1),
      action(-1),
      oldReward(0.0),
      newReward(0.0),
      feedbackID(-1)
{
    DEBUG_TRACE( cout << "RewardChange()" << endl; );
}

RewardChange::RewardChange(int state,
                           int action,
                           REAL_VALUE oldReward,
                           REAL_VALUE newReward)
    : state(state),
      action(action),
      oldReward(oldReward),
      newReward(newReward),
      feedbackID(-1)
{
    DEBUG_TRACE( cout << "RewardChange(" << state << "," << action
                      << "," << oldReward << "," << newReward
                      << "," << feedbackID << ")" << endl; );
}

RewardChange::RewardChange(int state,
                           int action,
                           REAL_VALUE oldReward,
                           REAL_VALUE newReward,
                           int feedbackID)
    : state(state),
      action(action),
      oldReward(oldReward),
      newReward(newReward),
      feedbackID(feedbackID)
{
    DEBUG_TRACE( cout << "RewardChange(" << state << "," << action
                      << "," << oldReward << "," << newReward
                      << "," << feedbackID << ")" << endl; );
}

int RewardChange::getState()
{
    return state;
}

int RewardChange::getAction()
{
    return action;
}

REAL_VALUE RewardChange::getOldReward()
{
    return oldReward;
}

REAL_VALUE RewardChange::getNewReward()
{
    return newReward;
}

int RewardChange::getFeedbackID()
{
    return feedbackID;
}

string RewardChange::getActionAndStateString()
{
    ostringstream ss;
    ss << "a" << action << " " << "s" << state;
    return ss.str();
}

string RewardChange::toString()
{
    ostringstream ss;
    ss << "RewardChange"
       << " - state: " << state
       << " - action: " << action
       << " - oldReward: " << oldReward
       << " - newReward: " << newReward
       << " - feedbackID: " << feedbackID;
    return ss.str();
}
