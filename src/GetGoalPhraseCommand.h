#ifndef SRC_GETGOALPHRASECOMMAND_H_
#define SRC_GETGOALPHRASECOMMAND_H_

#include "Command.h"

/**
 * Copies the goal phrase from the top of the goal stack to the working phrase
 */
class GetGoalPhraseCommand: public Command {
public:
	GetGoalPhraseCommand();
	int execute();
};

#endif /* SRC_GETGOALPHRASECOMMAND_H_ */
