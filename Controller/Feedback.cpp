#include "Feedback.h"

using namespace std;

int Feedback::idcounter = 0;

Feedback::Feedback()
    : belief(NULL),
      previousAction(-1),
      type(NEGATIVE),
      impact(0.01),
      id(idcounter++)
{
    cout << "Feedback() - id:" << id << endl;
}

Feedback::Feedback(SharedPointer<BeliefWithState> belief,
                   int previousAction,
                   Type type,
                   float impact)
    : belief(belief),
      previousAction(previousAction),
      type(type),
      impact(impact),
      id(idcounter++)
{
    cout << "Feedback(" << belief->bvec->ToString() << "," << previousAction << "," << type << "," << impact << "," << id << ")" << endl;
}

SharedPointer<BeliefWithState> Feedback::getBelief()
{
    return belief;
}

int Feedback::getPreviousAction()
{
    return previousAction;
}

Feedback::Type Feedback::getType()
{
    return type;
}

float Feedback::getImpact()
{
    return impact;
}

int Feedback::getID()
{
    return id;
}

string Feedback::toString()
{
    ostringstream ss;
    ss << "Feedback " << id << " - belief: " << belief->bvec->ToString() << ", previousAction: " << previousAction << ", type: " << type << ", impact: " << impact;
    return ss.str();
}
