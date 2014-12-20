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
#include "UserFeedback.h"
#include "Improvements.h"

#include "tinyxml.h"

void writeImprovementsToFile(string filename, Improvements improvements);
string lookupAction(int);
string lookupObservation(int);