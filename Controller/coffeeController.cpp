/**
  @brief Controller with reinforcement learning for coffee example
  @author Alexander Lill
  @date 2014-12-19
**/

// Usage: "./coffeeController coffeeproblem.pomdpx"

#include "coffeeController.h"

using namespace std;
using namespace momdp;

int main(int argc, char **argv) {

    // Process parameters
    SolverParams* p = &GlobalResource::getInstance()->solverParams;

    bool parseCorrect = SolverParams::parseCommandLineOption(argc, argv, *p);
    if(!parseCorrect) {
        cout << "Wrong command line options!" << endl; //usage(p->cmdName);
        exit(EXIT_FAILURE);
    }

    GlobalResource::getInstance()->init();
    string baseName = GlobalResource::getInstance()->parseBaseNameWithoutPath(p->problemName);
    GlobalResource::getInstance()->setBaseName(baseName);

    printf("\nLoading the model ...\n  ");

    // Load the problem
    GlobalResource::getInstance()->PBSolverPrePOMDPLoad();
    Problem problem = ParserSelector::loadProblem(p->problemName, *p);
    double pomdpLoadTime = GlobalResource::getInstance()->PBSolverPostPOMDPLoad();
    printf("  loading time : %.2fs \n", pomdpLoadTime);
    GlobalResource::getInstance()->problem = problem;

    // Initialize the solver
    SARSOP* sarsopSolver = new SARSOP(problem, p);

    // Initialize the solvers bounds
    BackupAlphaPlaneMOMDP* lbBackup = new BackupAlphaPlaneMOMDP();
    BackupBeliefValuePairMOMDP* ubBackup = new BackupBeliefValuePairMOMDP();
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
        cout<<"   action selection : one-step look ahead" << endl << endl;
    }

    // Initialize controller
    Controller control(problem, policy, p, -1);

    cout << "Initialized the controller" << endl;
    cout << "Enter number of the observation or any character to exit." << endl << endl;

    // Initialize feedback processor
    FeedbackProcessor *fp = new FeedbackProcessor(problem);

    // Action and observation vars
    int action = -1;
    int observation = -1;

    // Give first observation as dummy observation - to begin the process
    action = control.nextAction(0, 0);

    // Run process
    while (true) {

		printLine();
		printPossibleObservations();

		observation = askForObservation();

		if (observation < 0 || observation > 7) {
			cout << "Invalid observation. Stop." << endl;
			break;
		}

		cout << "BELIEF w/o  O: " << (*(control.currBelief())->bvec).ToString() << endl;
		cout << "OBSERVATION  : " << Obs::ObsNames[observation] << endl;

		if (observation != Obs::NEGATIVE) {

			// NORMAL OBSERVATION
			action = control.nextAction(observation, 0);
			cout << "CHOSEN ACTION: " << Actions::ActionNames[action] << endl;

		} else {

			// NEGATIVE OBSERVATION
			cout << "LAST ACTION  : " << Actions::ActionNames[action] << endl;

			// Create feedback object
			Feedback fb = Feedback(control.currBelief(), action, Feedback::NEGATIVE, 0.01);
			cout << "fb: " << fb.toString() << endl;

			// Apply reward changes calculated from feedback to problem
			fp->applyRewardChanges(fp->getRewardChangesForFeedback(fb));

			// Recalculate the policy and load it
			sarsopSolver->solve(problem);
			policy->load(sarsopSolver->getPolicy());
		}
	}

    // Save the reward changes
    if (fp->hasRewardChanges()) {

    	// Get all reward changes from feedback processor
        vector<RewardChange> rewardChangesInUse = fp->getRewardChangesInUse();

        // Print all reward changes
        printRewardChangesInUse(rewardChangesInUse);

        // Apply all reward changes to the pomdpx file
        writeRewardChangesToFile(p->problemName, rewardChangesInUse);
    }

    return 0;
}

void printRewardChangesInUse(vector<RewardChange> rewardChanges) {

	cout << "RewardChanges in use:" << endl;

	vector<RewardChange>::iterator it;

	for(it = rewardChanges.begin(); it != rewardChanges.end(); ++it) {
		cout << it->toString() << endl;
	}
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

int askForObservation() {

	int observation;

	cout << "Observation=";

	int nitems = scanf("%d", &observation);
	if (nitems == EOF) {
		cout << "EOF!" << endl;
		return -1;
	} else if (nitems == 0) {
		cout << "zero!" << endl;
		return -1;
	} else {
		return observation;
	}
}

void printLine() {
	cout << "----------------------------------------------------------------------------------------------------" << endl;
	//       ### grabCup=0 grabMilk=1 grabCoffee=2 putbackCup=3 putbackMilk=4 putbackCoff=5 done=6 NEGATIVE=7 ###
}

void printPossibleObservations() {

	cout << "### ";
	int numberOfEntries = sizeof(Obs::ObsNames)/sizeof(const char*);

	int i =0;
	while (i < numberOfEntries) {
		cout << Obs::ObsNames[i] << "=" << i << " ";
		i++;
	}

	cout << "###" << endl;
}
