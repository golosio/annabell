#ifndef SRC_PUSHGOALCOMMAND_H_
#define SRC_PUSHGOALCOMMAND_H_

#include "Command.h"

/**
 *  Insert the working phrase and the current word group in the top of the goal stack
 */
class PushGoalCommand: public Command {
public:
	PushGoalCommand();
	int execute();
};

#endif /* SRC_PUSHGOALCOMMAND_H_ */
