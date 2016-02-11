/*
Copyright (C) 2016 Bruno Golosio

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

#include "sensorymotor.h"

using namespace std;

//////////////////////////////////////////////////////////////////////////////
// Execute sensorymotor command
//////////////////////////////////////////////////////////////////////////////
int SensoryMotor(vector <string> phrase_token, stringstream &ss,
		 display* Display)
{
  // parse sensorymotor device and command
  if (phrase_token[0]=="[testdev:cmd]") { 
    Display->Print("sensorymotor test device: "+phrase_token[0]+"\n");
    Display->Print("command:");
    for (int i=1; i<phrase_token.size(); i++) {
      Display->Print(" "+phrase_token[i]);
    }
    Display->Print("\n");
    
    ss << "[===============]" << endl;
    ss << "[ Sensorymotor  ]" << endl;
    ss << "[ test message  ]" << endl;
    ss << "[ OK            ]" << endl;
    ss << "[===============]" << endl;
  }
  // put here your devices
  // else if (phrase_token[0]=="[mydevice:mycmd]" { // test device command
  // execute here your commands
  // pass messages/commands to ANNABELL below:
  // ss << "Your device messages/commands\n";
  // ss << "to be passed to ANNABELL\n";
  // }
  else if (phrase_token[0].substr(0,6)=="[yarp:") {
    YarpInterface(phrase_token, ss, Display); // call yarp interface
  }
  else {
    Display->Warning("Unrecognized sensorymotor device: " + phrase_token[0]
		     + "\n");
  }
  
  return 0;
}
