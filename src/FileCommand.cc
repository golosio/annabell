/*
 * FileCommand.cc
 *
 *  Created on: Feb 21, 2016
 *      Author: jpp
 */

#include <CommandFactory.h>
#include <display.h>
#include <FileCommand.h>
#include <iosfwd>
#include <iostream>
#include <string>
#include <vector>

FileCommand::FileCommand() {
	Command();
}

int FileCommand::execute() {

	vector<string> input_token;

	stringstream ss(input_line); // Insert the line into a stream
	string buf1; // buffer strings

	while (ss >> buf1) {
		// split line into space-separated tokens
		input_token.push_back(buf1);
	}

	if (input_token.size() < 2) {
		Display->Warning("a file name should be provided as argument.");
		return 1;
	}

	ifstream fs(input_token[1].c_str());
	if (!fs) {
		Display->Warning("Input file not found.");
		return 1;
	}

	string fileLineBuffer;
	while (getline(fs, fileLineBuffer)) {
		Command* c = CommandFactory::newCommand(fileLineBuffer);
		int commandResult = c->execute();
		delete c;

		if (commandResult == 2) {
			break;
		}
	}

	return 0;
}

