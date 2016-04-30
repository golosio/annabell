#include "WorkingPhraseOutCommand.h"

WorkingPhraseOutCommand::WorkingPhraseOutCommand() :
		Command() {
}

int WorkingPhraseOutCommand::doExecute() {
	if (input_token.size() > 1) {
		Display->Warning("syntax error.");
		return 1;
	}
	annabell->flags->OutPhrase = WorkingPhraseOut(annabell, Mon);
	annabell->flags->CompleteOutputFlag = true;
	// check if the output is a sensorymotor command
	CheckSensoryMotor(annabell->flags->OutPhrase, annabell, Display);

	return 0;
}
