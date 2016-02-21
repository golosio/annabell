/*
 * QuitCommand.cc
 *
 *  Created on: Feb 21, 2016
 *      Author: jpp
 */

#include "QuitCommand.h"

QuitCommand::QuitCommand() {
	Command();
}

int QuitCommand::execute() {
	return 2;
}
