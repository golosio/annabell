/*
 * QuitCommand.h
 *
 *  Created on: Feb 21, 2016
 *      Author: jpp
 */

#ifndef SRC_QUITCOMMAND_H_
#define SRC_QUITCOMMAND_H_

#include "Command.h"

class QuitCommand: public Command {
public:
	QuitCommand();
	int execute();
};

#endif /* SRC_QUITCOMMAND_H_ */
