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

int ExecuteAct(Annabell *annabell, Monitor *Mon, int rwd_act, int acq_act, int el_act);
int GetInputPhrase(Annabell *annabell, Monitor *Mon, string input_phrase);
int BuildAs(Annabell *annabell, Monitor *Mon);
string Exploitation(Annabell *annabell, Monitor *Mon, display* Display, int n_iter);
int CheckSensoryMotor(string out_phrase, Annabell *annabell, display* Display);
int ExplorationApprove(Annabell *annabell, Monitor *Mon);

class Command {
protected:
	Annabell* annabell;
	Monitor* Mon;
	display* Display;
	timespec* clk0;
	timespec* clk1;
	string input_line;

	vector<string> input_token;
	string stringCommand;

public:
	Command();
	void init(Annabell* annabell, Monitor* monitor, display* aDisplay, timespec* clk0, timespec* clk1, string input_line);
	virtual int execute();
	virtual int doExecute();
	virtual ~Command();

	int ParseCommand(Annabell *annabell, Monitor *Mon, display* Display, timespec* clk0, timespec* clk1, string input_line);
};

#endif /* SRC_COMMAND_H_ */
