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

#include <signal.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <assert.h>

#include "CPTimer.h"
#include "GlobalResource.h"
#include "MOMDP.h"
#include "POMDP.h"
#include "SARSOP.h"
#include "BackupAlphaPlaneMOMDP.h"
#include "BackupBeliefValuePairMOMDP.h"
#include "AlphaVectorPolicy.h"
#include "FullObsUBInitializer.h"
#include "FastInfUBInitializer.h"
#include "solverUtils.h"
#include "ParserSelector.h"
#include "Parser.h"

#include "Controller.h"
#include "RewardChange.h"
#include "Feedback.h"
#include "FeedbackProcessor.h"

#include "tinyxml.h"

namespace Actions {
enum Actions {
	reachCup        = 0,
	addMilk         = 1,
	addCoffee       = 2,
	putbackCup      = 3,
	putbackMilk     = 4,
	putbackCoff     = 5,
	wait            = 6
};

static const char* ActionNames[7] = {
	"reachCup",
	"addMilk",
	"addCoffee",
	"putbackCup",
	"putbackMilk",
	"putbackCoff",
	"wait"
};}

namespace Obs{
enum Obs {
	grabCup			= 0,
	grabMilk		= 1,
	grabCoffee		= 2,
	putbackCup		= 3,
	putbackMilk		= 4,
	putbackCoff		= 5,
	done			= 6,
	NEGATIVE        = 7
};

static const char* ObsNames[8] = {
	"grabCup",
	"grabMilk",
	"grabCoffee",
	"putbackCup",
	"putbackMilk",
	"putbackCoff",
	"done",
	"NEGATIVE"
};}

void writeRewardChangesToFile(string filename, vector<RewardChange> rewardChanges);
int askForObservation();
void printPossibleObservations();
void printRewardChangesInUse(vector<RewardChange> rewardChanges);
void printLine();
