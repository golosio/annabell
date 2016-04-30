#include "GetGoalPhraseCommand.h"

GetGoalPhraseCommand::GetGoalPhraseCommand() :
		Command() {
}

int GetGoalPhraseCommand::execute() {
	if (input_token.size() > 1) {
		Display->Warning("syntax error.");
		return 1;
	}
	ExecuteAct(annabell, Mon, STORE_ST_A, NULL_ACT, GET_GL_PH);
	ExplorationApprove(annabell, Mon);

	annabell->flags->ExplorationPhaseIdx = 1;
	//annabell->EPhaseI->Clear();

	return 0;
}
