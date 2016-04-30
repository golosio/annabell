/*
 * FileCommand.h
 *
 *  Created on: Feb 21, 2016
 *      Author: jpp
 */

#ifndef SRC_FILECOMMAND_H_
#define SRC_FILECOMMAND_H_

#include "Command.h"

/**
 * Class CommentCommand represents the action corresponding to when the user enters a file command.
 */
class FileCommand: public Command {
public:
	FileCommand();
	int execute();
};

#endif /* SRC_FILECOMMAND_H_ */
