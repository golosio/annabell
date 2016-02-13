======================================================================
Example of how to communicate with Annabell by speech
using YARP and the "Speech to TCP" Chrome app.
======================================================================

From a terminal start the yarp server:

$ yarpserver --write

In another terminal go to the directory "simple_examples" and start annabell:

$ annabell

 and type:

> .f example1.txt

Send the output to a yarp port:

> .yo

In another terminal go to the directory "yarp_output" and run:

$ ./yarp_output.sh

In the annabell terminal, select automatic exploitation:

> .ax

and take Annabell's input from a yarp port:

> .yi

Start the "Speech to TCP" app.
Type the port of Annabell's input and press start.
Select "Press button to send text"
Say "Tell me a fish" or "Tell me a bird" or "Tell me a reptile"
and press the "Send" button.
And so on...
