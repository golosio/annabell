/*
 Copyright (C) 2015 Bruno Golosio

 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <Annabell.h>
#include <Monitor.h>
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <stdlib.h>
#include "interface.h"
#include "display.h" 
#include "commands/CommandUtils.h"
#include "commands/CommandConstants.h"
#include "commands/CommandFactory.h"
#include "commands/Command.h"

using namespace std;

void simplify(Annabell *annabell, Monitor *monitor, display* Display, timespec* clk0, timespec* clk1,
		vector<string> input_token) {

	string cmd_line; //command_line
	string buf; // buffer string

	bool push_flag = false;

	string cmd = CommandUtils::removeTrailing(input_token[0], '*');
	if ((cmd == WORD_GROUP_CMD || cmd == REWARD_CMD || cmd == PARTIAL_REWARD_CMD)
			&& CommandUtils::endsWith(input_token[0], '*')) {
		push_flag = true;
	}

	buf = input_token[1];

	if (input_token.size() == 2 && CommandUtils::startsWith(input_token[1], '/')) {

		//Entered command is a macro, because it has only two tokens, and the second token starts with the character '/'.
		//For example: .o /mammal/the dog is a mammal/dog/

		unsigned int i = 0;

		string s1 = "";
		string s1b = "";
		string s2 = "";
		string ph = "";

		for (i = 1; i < buf.size(); i++) {
			if (buf[i] == '/' || buf[i] == '&') {
				break;
			}

			s1 = s1 + buf[i];
		}

		if (i >= buf.size()) {
			Display->Warning("Syntax error 1 on simplifying");
			return;
		}

		if (buf[i] == '&') {
			for (i++; i < buf.size(); i++) {
				if (buf[i] == '/') {
					break;
				}

				s1b = s1b + buf[i];
			}
			if (i == buf.size()) {
				Display->Warning("Syntax error 2 on simplifying");
				return;
			}
		}

		for (i++; i < buf.size(); i++) {
			if (buf[i] == '/') {
				break;
			}

			ph = ph + buf[i];
		}

		if (i == buf.size()) {
			Display->Warning("Syntax error 3 on simplifying");
			return;
		}

		for (i++; i < buf.size(); i++) {
			if (buf[i] == '/') {
				break;
			}

			s2 = s2 + buf[i];
		}

		if (i == buf.size()) {
			Display->Warning("Syntax error 4 on simplifying");
			return;
		}

		if (ph == "" && s2 == "") {
			Display->Warning("Syntax error 5 on simplifying");
			return;
		}

		// if working phrase must be stored and retrieved at the end
		if (push_flag) {
			CommandFactory::execute(PUSH_GOAL_CMD_LONG);
			CommandFactory::execute(GET_GOAL_PHRASE_CMD);
		}

		if (s1b != "") { // if there is a second cue
			CommandFactory::execute(string(WORD_GROUP_CMD + " ") + s1b);
			CommandFactory::execute(PUSH_GOAL_CMD_LONG);
			CommandFactory::execute(GET_GOAL_PHRASE_CMD);
		}

		if (s1 != "") {
			CommandFactory::execute(string(WORD_GROUP_CMD + " ") + s1);
		} else {
			monitor->GetPhM("WGB", annabell->WGB);
			s1 = monitor->OutPhrase;
		}

		cmd_line = PHRASE_CMD + " ";

		if (ph == "") {
			cmd_line = cmd_line + s1 + " , " + s2; // standard substitution
		} else {
			cmd_line = cmd_line + ph;
		}

		CommandFactory::execute(cmd_line);

		if (s2 != "") {
			CommandFactory::execute(string(WORD_GROUP_CMD + " ") + s2);
		}

		if (s1b != "") { // drop goal if there was a second cue
			CommandFactory::execute(DROP_GOAL_CMD_LONG);
		}

		// retrieve working phrase if necessary
		if (push_flag) {
			CommandFactory::execute(GET_GOAL_PHRASE_CMD);
			CommandFactory::execute(DROP_GOAL_CMD_LONG);
		}

	} else {

		//command is not a macro, nor a phrase or wordgroup command.
		//because simplify is called only for phrase, wordgroup, reward, partial reward and push commands,
		//it has to be one of reward, partial reward or push.

		if (push_flag) {
			//if push flag was true (.ie the abreviated form of command ending in asterisk was used), lets push the goal
			CommandFactory::execute(PUSH_GOAL_CMD_LONG);

			//get the goal phrase and use it as a working phrase
			CommandFactory::execute(GET_GOAL_PHRASE_CMD);
		}

		//automatically suggest extracting all tokens from the working phrase
		cmd_line = WORD_GROUP_CMD;

		for (unsigned int i = 1; i < input_token.size(); i++) {
			buf = input_token[i];
			cmd_line = cmd_line + " " + buf;
		}

		//execute the word group command
		CommandFactory::execute(cmd_line);
		//if push flasg was true that means the goal was pushed, so remove it from the top of the goal stack
		if (push_flag) {
			CommandFactory::execute(DROP_GOAL_CMD_LONG);
		}
	}

	if (cmd == PARTIAL_REWARD_CMD) {
		//if it was a partial reward, lets execute it
		CommandFactory::execute(PARTIAL_REWARD_CMD2);
	} else if (cmd == REWARD_CMD) {
		//if it was a reward, lets execute it
		CommandFactory::execute(REWARD_CMD2);
	} else if (cmd == PUSH_GOAL_CMD) {
		//if it was a push goal (not an abbreviated one like ".ph*" because that would have lead to push flag being true before), lets execute it
		CommandFactory::execute(PUSH_GOAL_CMD_LONG);
	}
}
