#ifndef SRC_CONTINUECONTEXTCOMMAND_H_
#define SRC_CONTINUECONTEXTCOMMAND_H_

#include "Command.h"

class ContinueContextCommand: public Command {
public:
	ContinueContextCommand();
	int doExecute();
};

#endif /* SRC_CONTINUECONTEXTCOMMAND_H_ */
