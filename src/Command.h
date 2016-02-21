/*
 * Command.h
 *
 *  Created on: Jan 10, 2016
 *      Author: jpp
 */

#ifndef SRC_COMMAND_H_
#define SRC_COMMAND_H_

#include <string>
#include "display.h"
#include <Annabell.h>
#include <Monitor.h>

using namespace std;

class Command {
protected:
	Annabell* annabell;
	Monitor* Mon;
	display* Display;
	timespec* clk0;
	timespec* clk1;
	string input_line;

public:
	Command();
	void init(Annabell* annabell, Monitor* monitor, display* aDisplay, timespec* clk0, timespec* clk1, string input_line);
	virtual int execute();
	virtual ~Command();
};

#endif /* SRC_COMMAND_H_ */
