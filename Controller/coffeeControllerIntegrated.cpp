/**
  @brief Controller with reinforcement learning for coffee example
  @author Alexander Lill
  @date 2014-12-19
**/

// Usage: "./coffeeControllerIntegrated coffeeproblem.pomdpx"

#include "coffeeControllerIntegrated.h"

using namespace std;
using namespace momdp;

OutputParams::OutputParams(void) {
	timeoutSeconds = -1;
	interval = -1;
}

int main(int argc, char **argv) {

    // SOLVER
    SolverParams* p = &GlobalResource::getInstance()->solverParams;

    bool parseCorrect = SolverParams::parseCommandLineOption(argc, argv, *p);
    if(!parseCorrect) {
        cout << "Wrong command line options!" << endl; //usage(p->cmdName);
        exit(EXIT_FAILURE);
    }

    OutputParams op;
    if(GlobalResource::getInstance()->benchmarkMode) {
        if(GlobalResource::getInstance()->simNum == 0|| GlobalResource::getInstance()->simLen == 0) {
            cout << "Benchmark Length and/or Number not set, please set them using option --simLen and --simNum" << endl;
            exit(-1);
        }
    }

    GlobalResource::getInstance()->init();
    string baseName = GlobalResource::getInstance()->parseBaseNameWithoutPath(p->problemName);
    GlobalResource::getInstance()->setBaseName(baseName);

    printf("\nLoading the model ...\n  ");

    GlobalResource::getInstance()->PBSolverPrePOMDPLoad();
    SharedPointer<MOMDP> problem (NULL);
    if(p->hardcodedProblem.length() ==0 ) {
        problem = ParserSelector::loadProblem(p->problemName, *p);
    } else {
        cout << "Unknown hard coded problem type : " << p->hardcodedProblem << endl;
        exit(0);
    }

    double pomdpLoadTime = GlobalResource::getInstance()->PBSolverPostPOMDPLoad();
    printf("  loading time : %.2fs \n", pomdpLoadTime);
    GlobalResource::getInstance()->problem = problem;

    SARSOP* sarsopSolver = NULL;
    BackupAlphaPlaneMOMDP* lbBackup = new BackupAlphaPlaneMOMDP();
    BackupBeliefValuePairMOMDP* ubBackup = new BackupBeliefValuePairMOMDP();

    sarsopSolver = new SARSOP(problem, p);

    lbBackup->problem = problem;
    sarsopSolver->lowerBoundBackup = lbBackup;

    ((BackupAlphaPlaneMOMDP* )(sarsopSolver->lowerBoundBackup))->solver = sarsopSolver;

    ubBackup->problem = problem;
    sarsopSolver->upperBoundBackup = ubBackup;

    //solve the problem
    sarsopSolver->solve(problem);

    //load calculated policy
    SharedPointer<AlphaVectorPolicy> policy = new AlphaVectorPolicy(problem);
    policy->load(sarsopSolver->getPolicy());

    if (p->useLookahead) {
        cout<<"   action selection : one-step look ahead\n";
    }

    Controller control(problem, policy, p, -1);

    cout<<"\nInitialized the controller\n";
    cout<<"\nEnter number of the observation or any character to exit."<<endl<<endl;

    cout << "Initial belief: " << (*(control.currBelief())->bvec).ToString() << endl << endl;

    int num, nitems, firstAction, action;

    Improvements improvements;

    // Give first observation as dummy observation - to begin the process
    // Signature: nextAction(ObsDefine currObservation, int nextStateX)
    control.nextAction(0, 0);

    while (true) {

        cout << "### grabMilk=0 grabCoffee=1 grabCup=2 putbackMilk=3 putbackCoff=4 putbackCup=5 done=6 negative=7 ### ";

        nitems = scanf("%d", &num);
        if (nitems == EOF) {
            cout <<"\n\nExiting...\n";
            break;
        } else if (nitems == 0) {
            cout <<"\n\nExiting...\n";
            break;
        } else {
            cout << endl << "OBSERVATION  : " << lookupObservation(num) << endl;

            // Handle negative feedback
            if (num == 7) {

                cout << "Last action  : " << lookupAction(action) << endl;
                cout << "Beliefs      : " << (*(control.currBelief())->bvec).ToString() << endl;

                UserFeedback uf = UserFeedback(problem, control.currBelief(), action, true, 0.01);
                improvements.addUserFeedback(uf);
                improvements.applyToProblem(problem);

                cout << "Created and applied user feedback: " << uf.toString() << endl;

                // Recalculate the policy and load it
                sarsopSolver->solve(problem);
                policy->load(sarsopSolver->getPolicy());

            } else {
                // Normal observation ... continue as usual
                cout << "BELIEF BEFORE: " << (*(control.currBelief())->bvec).ToString() << endl;
                action = control.nextAction(num, 0);
                cout << "CHOSEN ACTION: " << lookupAction(action) << endl << endl;
            }
        }
    }

    // Save the improvements to the filesystem
    if (improvements.size() != 0) {

        // Print all improvements
        cout << "All improvements:" << endl;
        cout << improvements.toString() << endl;

        // Write all improvements to the pomdpx file
        writeImprovementsToFile(p->problemName, improvements);
    }

    return 0;
}

void writeImprovementsToFile(string filename, Improvements improvements) {

    map<string, RewardChange> rewardChanges = improvements.getImprovements();

    TiXmlDocument doc(filename.c_str());

    TiXmlHandle hDoc(&doc); // the handler
    bool loadOkay = doc.LoadFile();

    if (!loadOkay) {
        cerr << "ERROR\n  Could not load pomdpX file" << endl ;
        cerr << "  Line"<< doc.ErrorRow() << ":" << doc.ErrorDesc() << endl;
        cerr << "Check pomdpX file with pomdpX's XML schema using a XML validator." << endl;
        exit(1);
    }

    TiXmlElement* pRewardFunction = hDoc.FirstChild("pomdpx").FirstChild("RewardFunction").ToElement();

    if(pRewardFunction == NULL) {
        cerr << "ERROR\n  Cannot find RewardFunction tag"<< endl;
        exit(XML_INPUT_ERROR);
    }

    TiXmlElement* entries = pRewardFunction->FirstChild("Func")->FirstChild("Parameter")->ToElement();

    TiXmlNode* child;
    ostringstream ss;

    map<string,RewardChange>::iterator it;
    for(it = rewardChanges.begin(); it != rewardChanges.end(); ++it) {
        cout << it->first << " - " << it->second.getNewReward() << endl;

        child = 0;
        while( child = entries->IterateChildren(child) ) {
            //key = child->FirstChild("Instance")->ToElement()->GetText();
            //value = child->FirstChild("ValueTable")->ToElement()->GetText();

            if (child->FirstChild("Instance")->ToElement()->GetText() == it->first) {
                ss.str("");
                ss.clear();
                ss << it->second.getNewReward();
                child->FirstChild("ValueTable")->ToElement()->FirstChild()->SetValue(ss.str().c_str());
            }
        }
    }

    doc.SaveFile(filename.append("new.pomdpx").c_str());
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
