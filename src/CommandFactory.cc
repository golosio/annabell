/*
 * CommandFactory.cc
 *
 *  Created on: Jan 10, 2016
 *      Author: jpp
 */

#include "CommandFactory.h"
#include "Command.h"
#include "display.h"

Annabell* CommandFactory::annabell;
Monitor* CommandFactory::monitor;
display* CommandFactory::aDisplay;
timespec* CommandFactory::clk0;
timespec* CommandFactory::clk1;

void CommandFactory::init(Annabell* annabell, Monitor* monitor, display* aDisplay, timespec* clk0, timespec* clk1) {
	CommandFactory::annabell = annabell;
	CommandFactory::monitor = monitor;
	CommandFactory::aDisplay = aDisplay;
	CommandFactory::clk0 = clk0;
	CommandFactory::clk1 = clk1;
}

Command* CommandFactory::newCommand(string input) {
	return new Command(CommandFactory::annabell, CommandFactory::monitor, CommandFactory::aDisplay,
			CommandFactory::clk0, CommandFactory::clk1, input);
}
