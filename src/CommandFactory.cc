/*
 * CommandFactory.cc
 *
 *  Created on: Jan 10, 2016
 *      Author: jpp
 */

#include "CommandFactory.h"
#include "Command.h"
#include "display.h"

Command* CommandFactory::newCommand(Annabell* annabell, Monitor* monitor, display* aDisplay, timespec* clk0, timespec* clk1) {
	return new Command(annabell, monitor, aDisplay, clk0, clk1);
}
