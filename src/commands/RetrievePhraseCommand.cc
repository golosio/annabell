#include "RetrievePhraseCommand.h"

RetrievePhraseCommand::RetrievePhraseCommand() :
		Command() {
}

int TargetExploration(Annabell *annabell, Monitor *Mon, string name, string target_phrase);

int RetrievePhraseCommand::doExecute() {
	if (input_token.size() < 2) {
		Display->Warning("a phrase should be provided as argument.");
		return 1;
	}
	string target_phrase;
	target_phrase = input_token[1];
	for (unsigned int itk = 2; itk < input_token.size(); itk++) {
		target_phrase = target_phrase + " " + input_token[itk];
	}
	//VerboseFlag=true;
	//TargetExplorationTest(annabell, Mon, "WkPhB", target_phrase);
	TargetExploration(annabell, Mon, "WkPhB", target_phrase);
	//cout << "target_phrase: " << target_phrase << endl;
	//VerboseFlag=false;
	return 0;
}
