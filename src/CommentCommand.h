/*
 * CommentCommand.h
 *
 *  Created on: Feb 21, 2016
 *      Author: jpp
 */

#ifndef SRC_COMMENTCOMMAND_H_
#define SRC_COMMENTCOMMAND_H_

#include "Command.h"

/**
 * Class CommentCommand represents the action corresponding to when the user enters a comment line as command.
 */
class CommentCommand: public Command {
public:
	CommentCommand();
	int execute();
};

#endif /* SRC_COMMENTCOMMAND_H_ */
