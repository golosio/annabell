#ifndef SRC_DROPGOALCOMMAND_H_
#define SRC_DROPGOALCOMMAND_H_

#include "Command.h"

/**
 * Removes the goal phrase and the goal word group from the top of the goal stack
 */
class DropGoalCommand: public Command {
public:
	DropGoalCommand();
	int execute();
};

#endif /* SRC_DROPGOALCOMMAND_H_ */
