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
#include <string>
#include <sstream>
#include <fstream>
#include <set>
#include <stdio.h>
#include <stdlib.h>
#include "sizes.h"
#include "interface.h"
#include "rnd.h"
#include "display.h" 
#include "ann_exception.h"
#include <sys/time.h>
#include "gettime.h" 
#include "AnnabellFlags.h"
#include "CommandFactory.h"
#include "Command.h"

using namespace std;
using namespace sizes;

int LastGB;
int StoredStActI;

int Interface(Annabell *annabell, Monitor *Mon);

int main() {
	try {
		init_randmt(12345);

		Annabell *annabell = new Annabell();
		Monitor *Mon = new Monitor(annabell);


		Interface(annabell, Mon);

		return 0;
	} catch (ann_exception &e) {
		cerr << "Error: " << e.what() << "\n";
		return 1;
	} catch (bad_alloc&) {
		cerr << "Error allocating memory.\n";
		return 1;
	} catch (...) {
		cerr << "Unrecognized error.\n";
		return 1;
	}
}

int Interface(Annabell *annabell, Monitor *mon) {
	struct timespec clk0, clk1;
	display Display;

	delete Display.LogFile;
	Display.LogFile = mon->Display.LogFile;

	string input_line;

	GetRealTime(&clk0);

	CommandFactory::init(annabell, mon, &Display, &clk0, &clk1);

	bool lineRead = true;
	int commandResult = 1;

	while (lineRead && (commandResult != 2)) {
		cout << "Enter command: ";

		lineRead = getline(cin, input_line);

		if (lineRead) {
			Command* c = CommandFactory::newCommand(input_line);
			commandResult = c->execute();
		}
	}

	if (Display.LogFile->is_open()) {
		Display.LogFile->close();
	}

	return 0;
}
