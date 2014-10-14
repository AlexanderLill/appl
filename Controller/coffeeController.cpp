/**
  @brief A simple test for the Controller using the Tiger example
  @author Le Trong Dao
  @date 2012-02-07
**/

// Usage: "./testController --policy-file=out.policy
// ../examples/POMDPX/Tiger.pomdpx"

#include "Controller.h"
#include "GlobalResource.h"
#include "MOMDP.h"
#include "ParserSelector.h"
#include "AlphaVectorPolicy.h"
#include <iostream>
using namespace std;

int main(int argc, char **argv)
{
    SolverParams* p = &GlobalResource::getInstance()->solverParams;
    bool parseCorrect = SolverParams::parseCommandLineOption(argc, argv, *p);

    if (p->policyFile == "") {
        cout<<"Invalid params\n";
        return 0;
    }

    cout<<"\nLoading the model ...\n   ";
    SharedPointer<MOMDP> problem = ParserSelector::loadProblem(p->problemName, *p);

    SharedPointer<AlphaVectorPolicy> policy = new AlphaVectorPolicy(problem);

    cout<<"\nLoading the policy ... input file : "<<p->policyFile<<"\n";
    bool policyRead = policy->readFromFile(p->policyFile);

    if (p->useLookahead) {
        cout<<"   action selection : one-step look ahead\n";
    }

    Controller control(problem, policy, p, -1);

    cout<<"\nInitialized the controller\n";



    cout<<"\nEnter number of the observation or any character to exit."<<endl<<endl;

    // nextAction(ObsDefine currObservation, int nextStateX)

    cout<<"Belief: " << (*(control.currBelief())->bvec).ToString()<<endl<<endl;


    int num, nitems, firstAction, action;

    while (true) {

        cout << "Observation: ";
        nitems = scanf("%d", &num);
        if (nitems == EOF) {
            cout<<"\n\nExiting loop...\n";
            break;
        } else if (nitems == 0) {
            cout<<"\n\nExiting loop...\n";
            break;
        } else {
            cout<<"Belief before: " << (*(control.currBelief())->bvec).ToString()<<endl;
            action = control.nextAction(num, 0);
            cout<<"Observation: "<<num<<" => Next Action: "<<action<<endl;
            cout<<"Belief after: " << (*(control.currBelief())->bvec).ToString()<<endl<<endl;

        }

    }

    return 0;
}
