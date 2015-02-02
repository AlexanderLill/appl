/**
  @brief Controller with reinforcement learning for coffee example
  @author Alexander Lill
  @date 2014-12-19
**/

// Usage: "./coffeeController coffeeproblem.pomdpx"

#include "coffeeController.h"

using namespace std;
using namespace momdp;

OutputParams::OutputParams(void) {
	timeoutSeconds = -1;
	interval = -1;
}

int main(int argc, char **argv) {

    // Process parameters
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

    // Load the problem
    GlobalResource::getInstance()->PBSolverPrePOMDPLoad();
    Problem problem (NULL);
    if(p->hardcodedProblem.length() ==0 ) {
        problem = ParserSelector::loadProblem(p->problemName, *p);
    } else {
        cout << "Unknown hard coded problem type : " << p->hardcodedProblem << endl;
        exit(0);
    }

    double pomdpLoadTime = GlobalResource::getInstance()->PBSolverPostPOMDPLoad();
    printf("  loading time : %.2fs \n", pomdpLoadTime);
    GlobalResource::getInstance()->problem = problem;

    // Initialize the solver
    SARSOP* sarsopSolver = NULL;
    BackupAlphaPlaneMOMDP* lbBackup = new BackupAlphaPlaneMOMDP();
    BackupBeliefValuePairMOMDP* ubBackup = new BackupBeliefValuePairMOMDP();

    sarsopSolver = new SARSOP(problem, p);

    lbBackup->problem = problem;
    sarsopSolver->lowerBoundBackup = lbBackup;

    ((BackupAlphaPlaneMOMDP* )(sarsopSolver->lowerBoundBackup))->solver = sarsopSolver;

    ubBackup->problem = problem;
    sarsopSolver->upperBoundBackup = ubBackup;

    // Solve the problem
    sarsopSolver->solve(problem);

    // Load calculated policy
    Policy policy = new AlphaVectorPolicy(problem);
    policy->load(sarsopSolver->getPolicy());

    if (p->useLookahead) {
        cout<<"   action selection : one-step look ahead\n";
    }

    // Initialize controller
    Controller control(problem, policy, p, -1);

    cout << "\nInitialized the controller\n";
    cout << "\nEnter number of the observation or any character to exit."<<endl<<endl;
    cout << "Initial belief: " << (*(control.currBelief())->bvec).ToString() << endl << endl;

    int num, nitems, action;
    short counter = 0;

    // Initialize feedback processor
    FeedbackProcessor *fp = new FeedbackProcessor(problem);

    // Give first observation as dummy observation - to begin the process
    control.nextAction(0, 0);

    // Run controller until it is exited with 'break'
    while (true) {

        // Print possible observations
        cout << "### grabCup=0 grabMilk=1 grabCoffee=2 putbackCup=3 putbackMilk=4 putbackCoff=5 done=6 negative=7 ### ";

        nitems = scanf("%d", &num);
        if (nitems == EOF) {
            cout <<"\n\nExiting...\n";
            break;
        } else if (nitems == 0) {
            cout <<"\n\nExiting...\n";
            break;
        } else {

            // Print name for given observation
            cout << endl << "OBSERVATION  : " << lookupObservation(num) << endl;

            // Check if given input is a normal observation or negative feedback
            if (num == 7) {

                // Handle negative feedback
                cout << "Last action  : " << lookupAction(action) << endl;
                cout << "Beliefs      : " << (*(control.currBelief())->bvec).ToString() << endl;

                // Create feedback object
                Feedback fb = Feedback(control.currBelief(), action, Feedback::NEGATIVE, 0.01);
                cout << "fb: " << fb.toString() << endl;

                // Apply reward changes calculated from feedback to problem
                fp->applyRewardChanges(fp->getRewardChangesForFeedback(fb));

                // Recalculate the policy and load it
                sarsopSolver->solve(problem);
                policy->load(sarsopSolver->getPolicy());

                counter = 0;

            } else {

                // Normal observation
                /*
                counter++;
                cout << "counter=" << counter << endl;

                Feedback fb = Feedback(control.currBelief(), action, Feedback::POSITIVE, 0.01);
                cout << "fb: " << fb.toString() << endl;

                fp->applyRewardChanges(fp->getRewardChangesForFeedback(fb));

                if ((counter % 10) == 0) {
                    cout << "counter is " << counter << " - recalculate policy!" << endl;

                    // Recalculate the policy and load it
                    sarsopSolver->solve(problem);
                    policy->load(sarsopSolver->getPolicy());

                    counter = 0;
                }

                */

                cout << "BELIEF BEFORE: " << (*(control.currBelief())->bvec).ToString() << endl;
                action = control.nextAction(num, 0);
                cout << "CHOSEN ACTION: " << lookupAction(action) << endl << endl;
            }
        }
    }

    // Save the improvements to the filesystem
    if (fp->hasRewardChanges()) {

        vector<RewardChange> rewardChangesInUse = fp->getRewardChangesInUse();

        cout << "RewardChanges in use:" << endl;

        vector<RewardChange>::iterator it;

        for(it = rewardChangesInUse.begin(); it != rewardChangesInUse.end(); ++it) {
            cout << it->toString() << endl;
        }

        // Write all improvements to the pomdpx file
        writeRewardChangesToFile(p->problemName, rewardChangesInUse);
    }

    return 0;
}

void writeRewardChangesToFile(string filename, vector<RewardChange> rewardChanges) {

    TiXmlDocument doc(filename.c_str());

    TiXmlHandle hDoc(&doc); // the handler
    bool loadOkay = doc.LoadFile();

    if (!loadOkay) {
        cerr << "ERROR: Could not load pomdpX file" << endl ;
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

    vector<RewardChange>::iterator it;
    for(it = rewardChanges.begin(); it != rewardChanges.end(); ++it) {

        child = 0;
        while( child = entries->IterateChildren(child) ) {
            //key = child->FirstChild("Instance")->ToElement()->GetText();
            //value = child->FirstChild("ValueTable")->ToElement()->GetText();

            string actionAndState = it->getActionAndStateString();

            if (child->FirstChild("Instance")->ToElement()->GetText() == actionAndState) {
                ss.str("");
                ss.clear();
                ss << it->getNewReward();
                child->FirstChild("ValueTable")->ToElement()->FirstChild()->SetValue(ss.str().c_str());
            }
        }
    }

    doc.SaveFile(filename.append("_new.pomdpx").c_str());
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
	grabCup			#0
	grabMilk		#1
	grabCoffee		#2
	putbackCup		#3
	putbackMilk		#4
	putbackCoff		#5
	done			#6
    */

    switch (observation) {

    case 0:
        returnval = "grabCup";
        break;
    case 1:
        returnval = "grabMilk";
        break;
    case 2:
        returnval = "grabCoffee";
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
