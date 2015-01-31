#include "Feedback.h"

// Counter for ascending IDs
int Feedback::idcounter = 0;

Feedback::Feedback(Belief belief,
                   int previousAction,
                   Type type,
                   float impact)
    : belief(belief),
      previousAction(previousAction),
      type(type),
      impact(impact),
      id(idcounter++)
{
    DEBUG_TRACE( cout << "Feedback(" << belief->bvec->ToString() << ","
                     << previousAction << "," << type << "," << impact
                     << "," << id << ")" << endl; );
}

Belief Feedback::getBelief()
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
    ss << "Feedback " << id
       << " - belief: " << belief->bvec->ToString()
       << ", previousAction: " << previousAction
       << ", type: " << type
       << ", impact: " << impact;
    return ss.str();
}
