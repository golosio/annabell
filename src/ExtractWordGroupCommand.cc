#include "ExtractWordGroupCommand.h"

ExtractWordGroupCommand::ExtractWordGroupCommand() :
		Command() {
}

int TargetExploration(Annabell *annabell, Monitor *Mon, string name, string target_phrase);

int ExtractWordGroupCommand::doExecute() {
	if (input_token.size() < 2) {
		//Display->Warning("a word group should be provided as argument.");
		//return 1;
		ExecuteAct(annabell, Mon, STORE_ST_A, NULL_ACT, FLUSH_WG);
		ExecuteAct(annabell, Mon, STORE_ST_A, NULL_ACT, W_FROM_WK);
		return 0;
	}
	string target_phrase;
	target_phrase = input_token[1];
	for (unsigned int itk = 2; itk < input_token.size(); itk++) {
		target_phrase = target_phrase + " " + input_token[itk];
	}
	//VerboseFlag=true;
	//TargetExplorationTest(annabell, Mon, "WGB", target_phrase);
	TargetExploration(annabell, Mon, "WGB", target_phrase);
	//cout << "target_phrase: " << target_phrase << endl;
	//VerboseFlag=false;

	return 0;
}
