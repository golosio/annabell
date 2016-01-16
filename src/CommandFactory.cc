/*
 * CommandFactory.cc
 *
 *  Created on: Jan 10, 2016
 *      Author: jpp
 */

#include <CommandFactory.h>
#include <EmptyCommand.h>
#include <string>

struct timespec;

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

void CommandFactory::pepe() {};

Command* CommandFactory::newCommand(string input) {
	if (input.empty()) {
		//aDisplay->Println("Creating EmptyCommand");
		return new EmptyCommand(CommandFactory::annabell, CommandFactory::monitor, CommandFactory::aDisplay,
				CommandFactory::clk0, CommandFactory::clk1, input);
	} else {
		//aDisplay->Println("Creating base Command");
		return new Command(CommandFactory::annabell, CommandFactory::monitor, CommandFactory::aDisplay,
				CommandFactory::clk0, CommandFactory::clk1, input);
	}
}
