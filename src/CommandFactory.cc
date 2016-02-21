/*
 * CommandFactory.cc
 *
 *  Created on: Jan 10, 2016
 *      Author: jpp
 */

#include <CommandConstants.h>
#include <CommandFactory.h>
#include <CommandUtils.h>
#include <CommentCommand.h>
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

Command* CommandFactory::newCommand(string input) {
	Command* newCommand;

	if (input.empty()) {
		newCommand = new EmptyCommand();

	} else if (CommandUtils::startsWith(input, COMMENT_CMD)) {
		newCommand = new CommentCommand();

	} else {
		newCommand = new Command();
	}

	newCommand->init(CommandFactory::annabell, CommandFactory::monitor, CommandFactory::aDisplay,
			CommandFactory::clk0, CommandFactory::clk1, input);

	return newCommand;
}
