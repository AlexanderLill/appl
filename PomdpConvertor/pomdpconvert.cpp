#include "convertor.hpp"
#include "solverUtils.h"
#include "GlobalResource.h"
#include "Parser.h"


int main(int argc, char **argv) {

	Convertor* conv = new Convertor();

	try
	{
	SolverParams* p =&GlobalResource::getInstance()->solverParams;
	bool parseCorrect = SolverParams::parseCommandLineOption(argc, argv, *p);
	if(!parseCorrect)
	{
		conv->print_usage(p->cmdName);
		exit(EXIT_FAILURE);
	}
	Parser* parser = new Parser();
	POMDP* pomdpProblem = parser->parse(p->problemName, p->useFastParser);

	ofstream pomdpxFile((p->problemName.append("x")).c_str());
	conv->convertToPomdpx(pomdpProblem, pomdpxFile);
	pomdpxFile.flush();
	pomdpxFile.close();
	}
	catch(bad_alloc &e)
	{
	if(GlobalResource::getInstance()->solverParams.memoryLimit == 0)
	{
		cout << "Memory allocation failed. Exit." << endl;
	}
	else
	{
		cout << "Memory limit reached. Please try increase memory limit" << endl;
	}

	}
	catch(exception &e)
	{
	cout << "Exception: " << e.what() << endl ;
	}

	return 0;
}
