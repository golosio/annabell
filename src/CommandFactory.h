/*
 * CommandFactory.h
 *
 *  Created on: Jan 10, 2016
 *      Author: jpp
 */

#ifndef SRC_COMMANDFACTORY_H_
#define SRC_COMMANDFACTORY_H_

#include <Annabell.h>
#include <Monitor.h>
#include "Command.h"
#include "display.h"

class CommandFactory {
public:
	static Command* newCommand(Annabell* annabell, Monitor* monitor, display* aDisplay, timespec* clk0, timespec* clk1);
};

#endif /* SRC_COMMANDFACTORY_H_ */
