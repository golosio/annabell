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
	EmptyCommand(Annabell* annabell, Monitor* monitor, display* aDisplay, timespec* clk0, timespec* clk1,
			string input_line);
	int execute();
};

#endif /* SRC_EMPTYCOMMAND_H_ */
