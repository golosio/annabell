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
#include <string>

using namespace std;

class CommandFactory {
private:
	static Annabell* annabell;
	static Monitor* monitor;
	static display* aDisplay;
	static timespec* clk0;
	static timespec* clk1;

public:
	/**
	 * Set static parameters into this factory. This method must be called prior to newCommand.
	 * @param annabell
	 * @param monitor
	 * @param aDisplay
	 * @param clk0
	 * @param clk1
	 */
	static void init(Annabell* annabell, Monitor* monitor, display* aDisplay, timespec* clk0, timespec* clk1);

	/**
	 * @param input a string line with a command for annabell to process
	 * @return an instance of the sub-type of Command corresponding to the issued string line command.
	 */
	static Command* newCommand(string input);
};

#endif /* SRC_COMMANDFACTORY_H_ */
