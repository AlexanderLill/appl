/**
  @brief A simple test for the Controller using the Tiger example
  @author Le Trong Dao
  @date 2012-02-07
**/

// Usage: "./testController --policy-file=out.policy
// ../examples/POMDPX/Tiger.pomdpx"


#include <stdio.h>
#include <stdlib.h>
#include <cfloat>

#ifdef _MSC_VER
#include "getopt.h"
#define NOMINMAX
#include <windows.h>

#else
#include <getopt.h>
#include <sys/time.h>
#endif
#include <signal.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <assert.h>

#include "CPTimer.h"

#include "GlobalResource.h"
//#include "ActionSelector.h"
//#include "PolicyFollower.h"

#include "solverUtils.h"
#include "Parser.h"
#include "POMDP.h"
#include "ParserSelector.h"
#include "MOMDP.h"
#include "SARSOP.h"
#include "BackupAlphaPlaneMOMDP.h"
#include "BackupBeliefValuePairMOMDP.h"

//#include "FSVI.h"
//#include "GES.h"
#include "FullObsUBInitializer.h"
#include "FastInfUBInitializer.h"

#include <string.h>

using namespace std;
using namespace momdp;

#ifdef __cplusplus
extern "C"
#endif
{
	extern unsigned long GlobalMemLimit;
}

struct OutputParams {
	double timeoutSeconds;
	double interval;
	OutputParams(void);
};

OutputParams::OutputParams(void) {
	timeoutSeconds = -1;
	interval = -1;
}

#include "Controller.h"
#include "GlobalResource.h"
//#include "MOMDP.h"
//#include "ParserSelector.h"
#include "AlphaVectorPolicy.h"
#include <iostream>
//using namespace std;

int MDPSolution(SharedPointer<MOMDP> problem, SolverParams* p);
string lookupAction(int);
string lookupObservation(int);

int main(int argc, char **argv) {


    // SOLVER
    SolverParams* p = &GlobalResource::getInstance()->solverParams;

    bool parseCorrect = SolverParams::parseCommandLineOption(argc, argv, *p);
    if(!parseCorrect)
    {
        cout << "Wrong command line options!" << endl; //usage(p->cmdName);
        exit(EXIT_FAILURE);
    }


    OutputParams op;
    if(GlobalResource::getInstance()->benchmarkMode)
    {
        if(GlobalResource::getInstance()->simNum == 0|| GlobalResource::getInstance()->simLen == 0)
        {
            cout << "Benchmark Length and/or Number not set, please set them using option --simLen and --simNum" << endl;
            exit(-1);
        }
    }


    GlobalResource::getInstance()->init();
    string baseName = GlobalResource::getInstance()->parseBaseNameWithoutPath(p->problemName);
    GlobalResource::getInstance()->setBaseName(baseName);

    //*************************
    //TODO: parse the problem
    //	long int clk_tck = sysconf(_SC_CLK_TCK);
    //	struct tms now1, now2;
    //	float utime, stime;

    printf("\nLoading the model ...\n  ");

    //Parser* parser = new Parser();

    GlobalResource::getInstance()->PBSolverPrePOMDPLoad();
    SharedPointer<MOMDP> problem (NULL);
    if(p->hardcodedProblem.length() ==0 )
    {
        problem = ParserSelector::loadProblem(p->problemName, *p);
    }
    else
    {
        cout << "Unknown hard coded problem type : " << p->hardcodedProblem << endl;
        exit(0);
    }

    double pomdpLoadTime = GlobalResource::getInstance()->PBSolverPostPOMDPLoad();
    printf("  loading time : %.2fs \n", pomdpLoadTime);
    GlobalResource::getInstance()->problem = problem;

//    //Getting a MDP solutions
//    if(p->MDPSolution == true)
//    {
//        MDPSolution(problem, p);
//        return 0;
//    }
//
//    if(p->QMDPSolution == true)
//    {
//        QMDPSolution(problem, p);
//        return 0;
//    }
//
//    if(p->FIBSolution == true)
//    {
//        FIBSolution(problem, p);
//        return 0;
//    }

    if(GlobalResource::getInstance()->benchmarkMode)
    {
        srand(GlobalResource::getInstance()->randSeed);
        GlobalResource::getInstance()->expRewardRecord.resize(GlobalResource::getInstance()->simNum);
    }
    //decide which solver to create
    PointBasedAlgorithm* solver;

    switch (p->strategy)
    {
    case S_SARSOP:
        {
            SARSOP* sarsopSolver = NULL;
            BackupAlphaPlaneMOMDP* lbBackup = new BackupAlphaPlaneMOMDP();
            BackupBeliefValuePairMOMDP* ubBackup = new BackupBeliefValuePairMOMDP();

            sarsopSolver = new SARSOP(problem, p);

            lbBackup->problem = problem;
            sarsopSolver->lowerBoundBackup = lbBackup;

            ((BackupAlphaPlaneMOMDP* )(sarsopSolver->lowerBoundBackup))->solver = sarsopSolver;

            ubBackup->problem = problem;
            sarsopSolver->upperBoundBackup = ubBackup;
            solver = sarsopSolver;
        }
        break;

        //case S_FSVI:
        //	solver = new FSVI(problem, p);
        //	break;

        //case S_GES:
        //	if(GlobalResource::getInstance()->migsPathFile != NULL)
        //	{
        //		if(GlobalResource::getInstance()->migsPathFileNum < 0 )
        //		{
        //			GlobalResource::getInstance()->migsPathFileNum = 10;
        //		}
        //		solver = new GES(problem, p, true);
        //	}
        //	else
        //	{
        //		solver = new GES(problem, p);
        //	}
        //	break;

    default:
        assert(0);// should never reach this point
    };

    //solve the problem
    solver->solve(problem);

    //load calculated policy
    SharedPointer<AlphaVectorPolicy> policy = new AlphaVectorPolicy(problem);
    policy->load(solver->getPolicy());

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
            if (num == 7) {
                // Negative feedback - to something with it!
                int state = (*(control.currBelief())->bvec).argmax();

                cout << "Last action:   " << lookupAction(action) << endl;
                cout << "Beliefs:       " << (*(control.currBelief())->bvec).ToString() << endl;
                cout << "Belief (max):  " << state << endl;

                //cout << "Given reward:  " << problem->rewards->matrix[*(control.currBelief())->sval)]
                SparseCol s = problem->rewards->getMatrix(0)->col(action);

                REAL_VALUE reward = 0;

                vector<SparseVector_Entry>::const_iterator  di;
                for (di = s.begin(); di != s.end(); di++) {
                    if (di->index == state) {
                        reward = di->value;
                    }
                }

                cout << "Original reward:  " << reward << endl;

                //SharedPointer<SparseMatrix> sm = (*(problem->rewards->getMatrix(0))(state, action)
                cout << "testo" << (*(problem->rewards->getMatrix(0)))(state, action) << endl;



                //SharedPointer<SparseMatrix> getMatrix

            } else {
                // Normal observation ... continue as usual
                cout << "BELIEF BEFORE: " << (*(control.currBelief())->bvec).ToString() << endl;
                action = control.nextAction(num, 0);
                cout << "CHOSEN ACTION: " << lookupAction(action) << endl << endl;
            }

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
    NEGATIVE        #7
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
        case 7:
           returnval = "NEGATIVE";
           break;
        default:
           returnval = "Invalid observation number!";
    }

    return returnval;
}
