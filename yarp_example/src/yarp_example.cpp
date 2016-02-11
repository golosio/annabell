#include <iostream>
#include <cstring>
#include <cstdlib>

#include <yarp/os/Network.h>
#include <yarp/os/BufferedPort.h>
#include <yarp/os/Time.h>

using namespace yarp::os;
using namespace std;

int main()
{  
  Network yarp;
  
  // prepare ports
  BufferedPort<Bottle> interface_input;  // port for reading from annabell
  interface_input.open("/ann_interface/input");  // give the port a name
  
  BufferedPort<Bottle> interface_output;  // port for writing to annabell
  interface_output.open("/ann_interface/output");  // give the port a name

  do {
    Bottle *in_bot = interface_input.read(true);

    for (int i=0; i<in_bot->size(); i++) {
      string in_str=in_bot->get(i).toString().c_str();
      //-------------------------------------------------send a message
      cout << in_str;	
    }
    //Time::delay(0.5);

    Bottle &out_bot = interface_output.prepare(); //
    out_bot.clear();
    out_bot.addString("test ");
    out_bot.addString("output ");
    out_bot.addString("message\n");
    out_bot.addString("other test ");
    out_bot.addString("output message\n");
    interface_output.write();

  } while(true);
    
  interface_input.close();
  interface_output.close();

  return 0;	
}
