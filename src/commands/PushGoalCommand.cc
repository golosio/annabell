#include "PushGoalCommand.h"

PushGoalCommand::PushGoalCommand() :
		Command() {
}

int PushGoalCommand::doExecute() {
	if (input_token.size() > 1) {
		Display->Warning("syntax error.");
		return 1;
	}
	if (annabell->flags->ExplorationPhaseIdx == 0) {
		ExecuteAct(annabell, Mon, START_ST_A, NULL_ACT, NULL_ACT);
		ExecuteAct(annabell, Mon, STORE_ST_A, NULL_ACT, W_FROM_IN);
		annabell->flags->ExplorationPhaseIdx++;
	}
	ExecuteAct(annabell, Mon, STORE_ST_A, NULL_ACT, PUSH_GL);
	int ex_ph = annabell->flags->ExplorationPhaseIdx;
	ExplorationApprove(annabell, Mon);
	annabell->flags->ExplorationPhaseIdx = ex_ph;
	//ExplorationPhaseIdx=0;
	//annabell->EPhaseI->Clear();

	return 0;
}
