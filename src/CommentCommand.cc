/*
 * CommentCommand.cc
 *
 *  Created on: Feb 21, 2016
 *      Author: jpp
 */

#include "CommentCommand.h"

CommentCommand::CommentCommand() :
		Command() {
}

int CommentCommand::execute() {
	this->Display->Print(this->input_line + "\n");
	return 0;
}
