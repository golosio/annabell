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
#include "AnnabellParams.h"
#include "CommandFactory.h"
#include "Command.h"
#include "ParseCommandTests.h"

using namespace std;
using namespace sizes;

int LastGB;
int StoredStActI;

int Interface(Annabell *annabell, Monitor *Mon);
int annabellMain(string param_file);

int ReadArg(int argc, char** argv, bool &isTestMode, string &param_file);

int main(int argc, char** argv)
{
	bool isTestMode;
	string param_file;

	if (ReadArg(argc, argv, isTestMode, param_file)!=0) return 1;

	if (isTestMode) {
		return runAllTests(argc, argv);
	} else {
		return annabellMain(param_file);
	}
}

int annabellMain(string param_file) {
  try {
    AnnabellParams prm;
    Annabell *annabell;
    if (param_file=="") {
      init_randmt(prm.seed);
      annabell = new Annabell();
    }
    else {
      if (prm.LoadFromFile(param_file)!=0) {
	return 1;
      }
      init_randmt(prm.seed);
      annabell = new Annabell(prm);
    }
    Monitor *Mon = new Monitor(annabell);
    annabell->param.Display();
    
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
 
  return 0;
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

int ReadArg(int argc, char** argv, bool &isTestMode, string &param_file)
{
  // Check command line input arguments
  isTestMode = false;
  param_file = "";

  int par=1;
  while(par<argc) {
    string str(argv[par]);
    if (str=="-pf" && par+1<argc) {
      par++;
      param_file=string(argv[par]);
    }
    else if (str=="test") {
      isTestMode = true;
    }
    else {
      cerr << "Wrong input arguments. Usage : " << argv[0] << " [arguments]\n";
      cerr << "Arguments:\n";
      cerr << "  test\t\trun test mode\n";
      cerr << "  -pf filename\tload free parameters from file\n";
      return 1;
    }
    par++;
  }
    
    return 0;
}
