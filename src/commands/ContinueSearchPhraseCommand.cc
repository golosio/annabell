#include "ContinueSearchPhraseCommand.h"

ContinueSearchPhraseCommand::ContinueSearchPhraseCommand() :
		Command() {
}
int ContinueSearchContext(Annabell *annabell, Monitor *Mon, display* Display, string target_phrase);

int ContinueSearchPhraseCommand::doExecute() {
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
	ContinueSearchContext(annabell, Mon, Display, target_phrase);
	//cout << "target_phrase: " << target_phrase << endl;
	//VerboseFlag=false;

	return 0;
}
