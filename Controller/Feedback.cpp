#include "Feedback.h"

using namespace std;

Feedback::Feedback()
    : belief(NULL),
      previousAction(-1),
      feedbackSign(NEGATIVE),
      impact(0.01)
{
    cout << "Feedback()" << endl;
}

Feedback::Feedback(SharedPointer<BeliefWithState> belief,
                   int previousAction,
                   Type type,
                   float impact)
    : belief(belief),
      previousAction(previousAction),
      feedbackSign(type),
      impact(impact)
{
    cout << "Feedback(" << belief->bvec->ToString() << "," << previousAction << "," << feedbackSign << "," << impact << ")" << endl;
}

string Feedback::toString() {
    ostringstream ss;
    ss << "Feedback - belief: " << belief->bvec->ToString() << ", previousAction: " << previousAction << ", feedbackSign=" << feedbackSign << ", impact: " << impact;
    return ss.str();
}
