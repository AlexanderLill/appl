#include "Feedback.h"

// Counter for ascending IDs
int Feedback::idcounter = 0;

Feedback::Feedback(Belief belief,
                   int action,
                   Type type,
                   float impact)
    : belief(belief),
      action(action),
      type(type),
      impact(impact),
      id(idcounter++)
{
    DEBUG_TRACE( cout << "Feedback(" << belief->bvec->ToString() << ","
                     << action << "," << type << "," << impact
                     << "," << id << ")" << endl; );
}

Belief Feedback::getBelief()
{
    return belief;
}

int Feedback::getAction()
{
    return action;
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
    ss << "Feedback - ID: " << id
       << " - belief: " << belief->bvec->ToString()
       << " - action: " << action
       << " - type: " << type
       << " - impact: " << impact;
    return ss.str();
}
