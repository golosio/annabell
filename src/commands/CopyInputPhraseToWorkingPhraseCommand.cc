#include "CopyInputPhraseToWorkingPhraseCommand.h"

CopyInputPhraseToWorkingPhraseCommand::CopyInputPhraseToWorkingPhraseCommand() :
		Command() {
}

int ExplorationApprove(Annabell *annabell, Monitor *Mon);

int CopyInputPhraseToWorkingPhraseCommand::doExecute() {
	if (input_token.size() > 1) {
		Display->Warning("syntax error.");
		return 1;
	}
	ExecuteAct(annabell, Mon, STORE_ST_A, NULL_ACT, W_FROM_IN);
	//ExecuteAct(annabell, Mon, STORE_ST_A, MEM_PH, NULL_ACT); // dummy
	ExplorationApprove(annabell, Mon);
	//Mon->PrintPhM("WkPhB", annabell->WkPhB);
	//Mon->PrintSSMidx("RemPh", annabell->RemPh);
	//Mon->PrintSSMidx("MemPh", annabell->MemPh);
	annabell->flags->ExplorationPhaseIdx = 1;
	//annabell->EPhaseI->Clear();

	return 0;
}
