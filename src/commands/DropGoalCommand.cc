#include "DropGoalCommand.h"

DropGoalCommand::DropGoalCommand() :
		Command() {
}

int DropGoalCommand::doExecute() {
	if (input_token.size() > 1) {
		Display->Warning("syntax error.");
		return 1;
	}
	ExecuteAct(annabell, Mon, STORE_ST_A, NULL_ACT, DROP_GL);
	int ex_ph = annabell->flags->ExplorationPhaseIdx;
	ExplorationApprove(annabell, Mon);
	annabell->flags->ExplorationPhaseIdx = ex_ph;
	//ExplorationPhaseIdx=0;
	//annabell->EPhaseI->Clear();

	return 0;
}
