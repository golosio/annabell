#ifndef SRC_MACROCOMMAND_H_
#define SRC_MACROCOMMAND_H_

#include "Command.h"

/**
 * Class MacroCommand represents the action corresponding to when the user enters a command in a macro form.
 * This command will call, in turn, the individual commands equivalent to the specified macro.
 */
class MacroCommand: public Command {
public:
	MacroCommand();
	int execute();
};

#endif /* SRC_MACROCOMMAND_H_ */
