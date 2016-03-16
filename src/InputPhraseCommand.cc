#include "InputPhraseCommand.h"

InputPhraseCommand::InputPhraseCommand() :
		Command() {
}

int InputPhraseCommand::execute() {
	if (annabell->flags->SpeakerFlag) {
		Display->Print("*" + annabell->flags->SpeakerName + ":\t"); //("*TEA:\t");
	}

	Display->Print(input_line + "\n");
	ExecuteAct(annabell, Mon, STORE_ST_A, NULL_ACT, FLUSH_OUT);
	GetInputPhrase(annabell, Mon, input_line);
	BuildAs(annabell, Mon);
	//BuildAsTest(annabell, Mon);

	// check if automatic-exploitation flag is ON
	if (annabell->flags->AutoExploitFlag) {
		annabell->flags->OutPhrase = Exploitation(annabell, Mon, Display, 1);
		annabell->flags->CompleteOutputFlag = true;

		// check if the output is a sensorymotor command
		CheckSensoryMotor(annabell->flags->OutPhrase, annabell, Display);
	}

	return 0;
}
