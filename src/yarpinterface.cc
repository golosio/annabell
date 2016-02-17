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

#include "CommandFactory.h"
#include "sensorymotor.h"
using namespace std;

#ifdef USE_YARP

#include <yarp/os/Network.h>
#include <yarp/os/BufferedPort.h>
#include <yarp/os/Time.h>

using namespace yarp::os;
BufferedPort<Bottle> AnnDevInput;  // make a port for reading from yarp device
BufferedPort<Bottle> AnnDevOutput;  // make a port for writing to a yarp device
BufferedPort<Bottle> AnnOutput;  // port for sending output to yarp 
BufferedPort<Bottle> AnnInput; // port for reading from annabell

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
    Display->Print("yarp interface command:");
    for (int i=1; i<phrase_token.size(); i++) {
      Display->Print(" "+phrase_token[i]);
    }
    Display->Print("\n");

    if (phrase_token[1]=="_open") { 
      AnnDevInput.open("/annabell/dev/input");  // give the port a name
      Network::connect("/ann_interface/output","/annabell/dev/input");      
      AnnDevOutput.open("/annabell/dev/output");  // give the port a name
      Network::connect("/annabell/dev/output","/ann_interface/input");

      return 0;
    }
    else if (phrase_token[1]=="_close") { 
      AnnDevInput.close();
      AnnDevOutput.close();
    }

    Bottle &out_bot = AnnDevOutput.prepare(); //
    out_bot.clear();
    for (int i=1; i<phrase_token.size(); i++) {
      // cout << "-" << phrase_token[i] << "-" << endl;
      out_bot.addString(phrase_token[i]);
    }
    AnnDevOutput.write();
    
    Bottle *in_bot = AnnDevInput.read(true);
    
    for (int i=0; i<in_bot->size(); i++) {
      string in_str=in_bot->get(i).toString().c_str();
      
      //-------------------------------------------------send a message
      ss << in_str;
    }
  }
  else {
    Display->Warning("Unrecognized sensorymotor device: " + phrase_token[0]
		     + "\n");
  }
  
  return 0;
}

//////////////////////////////////////////////////////////////////////////////
// Read input from YARP port
//////////////////////////////////////////////////////////////////////////////
int YarpInput(display* Display)
{
  Network yarp;

  // prepare port

  AnnInput.open("/annabell/input");  // give the port a name
  
  do {
    Bottle *in_bot = AnnInput.read(true);

    string in_phrase="";
    for (int i=0; i<in_bot->size(); i++) {
      string in_str=in_bot->get(i).toString().c_str();
      if (i>0) in_phrase=in_phrase+" ";
      in_phrase=in_phrase+in_str;
    }
    // parse and process yarp input
    Command* c = CommandFactory::newCommand(in_phrase);
    int commandResult = c->execute();
    delete c;
    if(commandResult == 2) break;

  } while(true);

  AnnInput.close();

  return 0;	
}

//////////////////////////////////////////////////////////////////////////////
// Sends output to a YARP port
//////////////////////////////////////////////////////////////////////////////
int YarpOutput(string out_phrase, display* Display)
{
  Bottle &out_bot = AnnOutput.prepare(); // prepare the bottle for output
  out_bot.clear();
  out_bot.addString(out_phrase);
  AnnOutput.write();

  return 0;
}

//////////////////////////////////////////////////////////////////////////////
// Initializes YARP output port
//////////////////////////////////////////////////////////////////////////////
bool YarpOutputInit(display* Display)
{
  Network yarp;
  // prepare port
  return AnnOutput.open("/annabell/output");
}

//////////////////////////////////////////////////////////////////////////////
// Closes YARP output port
//////////////////////////////////////////////////////////////////////////////
int YarpOutputClose(display* Display)
{
  AnnOutput.close();

  return 0;
}

//////////////////////////////////////////////////////////////////////////////
// If YARP is supported return true
//////////////////////////////////////////////////////////////////////////////
bool CheckYarp(display* Display)
{  
  return true;
}

#else
//////////////////////////////////////////////////////////////////////////////
// YARP support is not installed
//////////////////////////////////////////////////////////////////////////////

int YarpInterface(vector <string> phrase_token, stringstream &ss,
		 display* Display)
{
  YarpNotSupported(Display);
  
  return 0;
}

int YarpInput(display* Display)
{
  YarpNotSupported(Display);
  
  return 0;
}

int YarpOutput(string out_phrase, display* Display)
{
  YarpNotSupported(Display);
  
  return 0;
}

bool YarpOutputInit(display* Display)
{
  YarpNotSupported(Display);
  
  return false;
}

int YarpOutputClose(display* Display)
{
  YarpNotSupported(Display);

  return 0;
}

bool CheckYarp(display* Display)
{
  YarpNotSupported(Display);
  
  return false;
}

#endif

int YarpNotSupported(display* Display)
{
  Display->Warning("YARP support is not installed\n");
  
  return 0;
}

