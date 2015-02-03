/**
   @brief An implementation of a controller for appl
   @author Le Trong Dao
   @date 2012-02-03
**/

#ifndef Controller_H
#define Controller_H

#include <vector>
#include "MOMDP.h"

// Some macros to adapt the Interface
#define ActionDefine int const
#define ObsDefine int const
#define Belief SharedPointer<BeliefWithState>
#define Problem SharedPointer<MOMDP>
#define Policy SharedPointer<AlphaVectorPolicy>

#include "ControllerInterface.h"

namespace momdp
{
    class AlphaVectorPolicy;
    class SolverParams;

    class Controller: public ControllerInterface
    {
      private:
        Problem problem;
        Policy policy;
        SolverParams* solverParams;
        // starting value of X
        Belief currBelSt;
        // If this is the first action => ignore the obs and don't
        // update the belief
        bool firstAction;
        int lastAction;

      public:
        Controller(Problem problem,
                   Policy policy,
                   SolverParams * solverParams,
                   int initialBeliefStvalX);
        ActionDefine nextAction(ObsDefine obs, int nextStateX);
        Belief currBelief() const;
    };
}

#endif
