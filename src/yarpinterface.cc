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

#ifdef USE_YARP

#include <yarp/os/Network.h>
#include <yarp/os/BufferedPort.h>
#include <yarp/os/Time.h>

using namespace yarp::os;

//////////////////////////////////////////////////////////////////////////////
// Execute sensorymotor command
//////////////////////////////////////////////////////////////////////////////
int YarpInterface(vector <string> phrase_token, stringstream &ss,
		 display* Display)
{
  Network yarp;

  // parse sensorymotor device and command
  if (phrase_token[0]=="[yarp:test]") { 
    Display->Print("yarp test device: "+phrase_token[0]+"\n");
    Display->Print("command:");
    for (int i=1; i<phrase_token.size(); i++) {
      Display->Print(" "+phrase_token[i]);
    }
    Display->Print("\n");
    
    ss << "[===============]" << endl;
    ss << "[ Yarp          ]" << endl;
    ss << "[ test message  ]" << endl;
    ss << "[ OK            ]" << endl;
    ss << "[===============]" << endl;
  }
  else if (phrase_token[0]=="[yarp:interface]") { 
    Display->Print("yarp test device: "+phrase_token[0]+"\n");
    Display->Print("command:");
    for (int i=1; i<phrase_token.size(); i++) {
      Display->Print(" "+phrase_token[i]);
    }
    Display->Print("\n");

    BufferedPort<Bottle> ann_input;  // make a port for reading from yarp
    ann_input.open("/annabell/input");  // give the port a name
    Network::connect("/ann_interface/output","/annabell/input");
  
    BufferedPort<Bottle> ann_output;  // make a port for writing to yarp
    ann_output.open("/annabell/output");  // give the port a name
    Network::connect("/annabell/output","/ann_interface/input");
  
    Bottle &out_bot = ann_output.prepare(); //
    out_bot.clear();
    for (int i=1; i<phrase_token.size(); i++) {
      if (i>1) out_bot.addString(" ");
      out_bot.addString(phrase_token[i]);
    }
    out_bot.addString("\n");
    ann_output.write();
    
    Bottle *in_bot = ann_input.read(true);
    
    for (int i=0; i<in_bot->size(); i++) {
      string in_str=in_bot->get(i).toString().c_str();
      
      //-------------------------------------------------send a message
      ss << in_str;
    }
    ann_input.close();
    ann_output.close();
  }
  else {
    Display->Warning("Unrecognized sensorymotor device: " + phrase_token[0]
		     + "\n");
  }
  
  return 0;
}

#else

//////////////////////////////////////////////////////////////////////////////
// YARP support is not installed
//////////////////////////////////////////////////////////////////////////////
int YarpInterface(vector <string> phrase_token, stringstream &ss,
		 display* Display)
{
  Display->Warning("YARP support is not installed\n");
  
  return 0;
}

#endif
