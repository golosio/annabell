/*
 * EmptyCommand.h
 *
 *  Created on: Jan 11, 2016
 *      Author: jpp
 */

#ifndef SRC_EMPTYCOMMAND_H_
#define SRC_EMPTYCOMMAND_H_

#include "Command.h"

/**
 * Class EmptyCommand represents the action corresponding to when the user enters an empty string line as a command.
 */
class EmptyCommand: public Command {
public:
	EmptyCommand();
	int execute();
};

#endif /* SRC_EMPTYCOMMAND_H_ */
