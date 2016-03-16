#ifndef SRC_INPUTPHRASECOMMAND_H_
#define SRC_INPUTPHRASECOMMAND_H_

#include "Command.h"

/**
 * Class InputPhraseCommand represents the action corresponding to when the user enters simple phrase as input.
 */
class InputPhraseCommand: public Command {
public:
	InputPhraseCommand();
	int execute();
};

#endif /* SRC_INPUTPHRASECOMMAND_H_ */
