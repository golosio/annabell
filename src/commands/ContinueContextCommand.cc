#include "ContinueContextCommand.h"

ContinueContextCommand::ContinueContextCommand() :
		Command() {
}

int ContinueContextCommand::doExecute() {
	if (input_token.size() > 2) {
		Display->Warning("syntax error.");
		return 1;
	}
	annabell->flags->StartContextFlag = false;

	return 0;
}
