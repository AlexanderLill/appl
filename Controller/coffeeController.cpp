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

string lookupAction(int);
string lookupObservation(int);

int main(int argc, char **argv) {

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

    cout << "Initial belief: " << (*(control.currBelief())->bvec).ToString() << endl << endl;

    int num, nitems, firstAction, action;

    // Give first observation as dummy observation - to begin the process
    // Signature: nextAction(ObsDefine currObservation, int nextStateX)
    control.nextAction(0, 0);

    while (true) {

        cout << "--- obs: grabMilk=0, grabCoffee=1, grabCup=2, putbackMilk=3, putbackCoff=4, putbackCup=5, done=6 --- ";
        nitems = scanf("%d", &num);
        if (nitems == EOF) {
            cout <<"\n\nExiting loop...\n";
            break;
        } else if (nitems == 0) {
            cout <<"\n\nExiting loop...\n";
            break;
        } else {
            cout << endl << "OBSERVATION  : " << lookupObservation(num) << endl;
            cout << "BELIEF BEFORE: " << (*(control.currBelief())->bvec).ToString() << endl;
            action = control.nextAction(num, 0);
            cout << "BELIEF AFTER : " << (*(control.currBelief())->bvec).ToString() << endl;
            cout << "CHOSEN ACTION: " << lookupAction(action) << endl << endl;

        }

    }

    return 0;
}

string lookupAction(int action) {

    string returnval;

    /*
    reachCup        #0
    addMilk         #1
    addCoffee       #2
    putbackCup      #3
    putbackMilk     #4
    putbackCoff     #5
    wait            #6
    */

    switch (action) {

        case 0:
           returnval = "reachCup";
           break;
        case 1:
           returnval = "addMilk";
           break;
        case 2:
           returnval = "addCoffee";
           break;
        case 3:
           returnval = "putbackCup";
           break;
        case 4:
           returnval = "putbackMilk";
           break;
        case 5:
           returnval = "putbackCoff";
           break;
        case 6:
           returnval = "wait";
           break;
        default:
           returnval = "Invalid action number!";
    }

    return returnval;
}

string lookupObservation(int observation) {

    string returnval;

    /*
    grabMilk        #0
    grabCoffee      #1
    grabCup         #2
    putbackMilk     #3
    putbackCoff     #4
    putbackCup      #5
    done            #6
    */

    switch (observation) {

        case 0:
           returnval = "grabMilk";
           break;
        case 1:
           returnval = "grabCoffee";
           break;
        case 2:
           returnval = "grabCup";
           break;
        case 3:
           returnval = "putbackMilk";
           break;
        case 4:
           returnval = "putbackCoff";
           break;
        case 5:
           returnval = "putbackCup";
           break;
        case 6:
           returnval = "done";
           break;
        default:
           returnval = "Invalid observation number!";
    }

    return returnval;
}