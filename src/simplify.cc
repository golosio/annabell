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
#include "CommandUtils.h"
#include "CommandConstants.h"

using namespace std;

int ParseCommand(Annabell *annabell, Monitor *Mon, display* Display, timespec* clk0, timespec* clk1, string input_line);

bool simplify(Annabell *annabell, Monitor *monitor, display* Display, timespec* clk0, timespec* clk1, vector<string> input_token) {

	string cmd_line; //command_line
	string buf; // buffer string

	bool push_flag = false;

		string cmd = CommandUtils::removeTrailing(input_token[0], '*');
		if ((cmd == WORD_GROUP_CMD || cmd == REWARD_CMD || cmd == PARTIAL_REWARD_CMD) && CommandUtils::endsWith(input_token[0], '*')) {
			push_flag = true;
		}

		buf = input_token[1];

		if (input_token.size() == 2 && CommandUtils::startsWith(input_token[1], '/')) {

			//Entered command is a macro, because it has only two tokens, and the second token starts with the character '/'.
			//For example: .o /mammal/the dog is a mammal/dog/

			unsigned int i;
			string s1, s1b, s2, ph;
			i = 0;
			s1 = s1b = s2 = ph = "";

			for (i = 1; i < buf.size(); i++) {
				if (buf[i] == '/' || buf[i] == '&') {
					break;
				}

				s1 = s1 + buf[i];
			}

			if (i >= buf.size()) {
				Display->Warning("Syntax error 1 on simplifying");
				return true;
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
					return true;
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
				return true;
			}

			for (i++; i < buf.size(); i++) {
				if (buf[i] == '/') {
					break;
				}

				s2 = s2 + buf[i];
			}

			if (i == buf.size()) {
				Display->Warning("Syntax error 4 on simplifying");
				return true;
			}

			if (ph == "" && s2 == "") {
				Display->Warning("Syntax error 5 on simplifying");
				return true;
			}

			// if working phrase must be stored and retrieved at the end
			if (push_flag && ParseCommand(annabell, monitor, Display, clk0, clk1, PUSH_GOAL_CMD_LONG) == 2) {
				return true;
			}

			if (push_flag && ParseCommand(annabell, monitor, Display, clk0, clk1, GET_GOAL_PHRASE_CMD) == 2) {
				return true;
			}

			if (s1b != "") { // if there is a second cue

				if (ParseCommand(annabell, monitor, Display, clk0, clk1, string(WORD_GROUP_CMD + " ") + s1b) == 2) {
					return true;
				}

				if (ParseCommand(annabell, monitor, Display, clk0, clk1, PUSH_GOAL_CMD_LONG) == 2) {
					return true;
				}

				if (ParseCommand(annabell, monitor, Display, clk0, clk1, GET_GOAL_PHRASE_CMD) == 2) {
					return true;
				}
			}

			if (s1 != "") {
				if (ParseCommand(annabell, monitor, Display, clk0, clk1, string(WORD_GROUP_CMD + " ") + s1) == 2) {
					return true;
				}
			} else {
				monitor->GetPhM("WGB", annabell->WGB);
				s1 = monitor->OutPhrase;
			}

			cmd_line = PHRASE_CMD + " ";

			if (ph == "") {
				cmd_line = cmd_line + s1 + " , " + s2; // standard substitution
			}
			else {
				cmd_line = cmd_line + ph;
			}

			if (ParseCommand(annabell, monitor, Display, clk0, clk1, cmd_line) == 2) {
				return true;
			}

			if (s2 != "" && ParseCommand(annabell, monitor, Display, clk0, clk1, string(WORD_GROUP_CMD + " ") + s2) == 2) {
				return true;
			}

			if (s1b != "") { // drop goal if there was a second cue
				if (ParseCommand(annabell, monitor, Display, clk0, clk1, DROP_GOAL_CMD_LONG) == 2) {
					return true;
				}
			}

			// retrieve working phrase if necessary
			if (push_flag && ParseCommand(annabell, monitor, Display, clk0, clk1, GET_GOAL_PHRASE_CMD) == 2) {
				return true;
			}
			if (push_flag && ParseCommand(annabell, monitor, Display, clk0, clk1, DROP_GOAL_CMD_LONG) == 2) {
				return true;
			}

		} else if (cmd == PHRASE_CMD || cmd == WORD_GROUP_CMD) {
			//command is not a macro, and is a phrase or word group command. return false so it can be normally processed.
			return false;
		}
		else {

			//command is not a macro, nor a phrase or wordgroup command.
			//because simplify is called only for phrase, wordgroup, reward, partial reward and push commands,
			//it has to be one of reward, partial reward or push.

			//if push flag was true (.ie the abreviated form of command ending in asterisk was used), lets push the goal
			if (push_flag && ParseCommand(annabell, monitor, Display, clk0, clk1, PUSH_GOAL_CMD_LONG) == 2) {
				//this only happens if ParseCommand returned 2, which in turn only happens if entered command is .quit,
				//which is like never the case, so this most likely never happens.
				return true;
			}

			//get the goal phrase and use it as a working phrase
			if (push_flag && ParseCommand(annabell, monitor, Display, clk0, clk1, GET_GOAL_PHRASE_CMD) == 2) {
				//this only happens if ParseCommand returned 2, which in turn only happens if entered command is .quit,
				//which is like never the case, so this most likely never happens.
				return true;
			}

			//automatically suggest extracting all tokens from the working phrase
			cmd_line = WORD_GROUP_CMD;

			for (unsigned int i = 1; i < input_token.size(); i++) {
				buf = input_token[i];
				cmd_line = cmd_line + " " + buf;
			}

			//execute the word group command
			if (ParseCommand(annabell, monitor, Display, clk0, clk1, cmd_line) == 2) {
				//this only happens if ParseCommand returned 2, which in turn only happens if entered command is .quit,
				//which is like never the case, so this most likely never happens.
				return true;
			}

			//if push flasg was true that means the goal was pushed, so remove it from the top of the goal stack
			if (push_flag && ParseCommand(annabell, monitor, Display, clk0, clk1, DROP_GOAL_CMD_LONG) == 2) {
				//this only happens if ParseCommand returned 2, which in turn only happens if entered command is .quit,
				//which is like never the case, so this most likely never happens.
				return true;
			}
		}

		if (cmd == PARTIAL_REWARD_CMD) {
			//if it was a partial reward, lets execute it
			ParseCommand(annabell, monitor, Display, clk0, clk1, PARTIAL_REWARD_CMD2);
		}
		else if (cmd == REWARD_CMD) {
			//if it was a reward, lets execute it
			ParseCommand(annabell, monitor, Display, clk0, clk1, REWARD_CMD2);
		}
		else if (cmd == PUSH_GOAL_CMD) {
			//if it was a push goal (not an abbreviated one like ".ph*" because that would have lead to push flag being true before), lets execute it
			ParseCommand(annabell, monitor, Display, clk0, clk1, PUSH_GOAL_CMD_LONG);
		}

		return true;
}
