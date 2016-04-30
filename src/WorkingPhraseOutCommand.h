#ifndef SRC_WORKINGPHRASEOUTCOMMAND_H_
#define SRC_WORKINGPHRASEOUTCOMMAND_H_

#include "Command.h"

class WorkingPhraseOutCommand: public Command {
public:
	WorkingPhraseOutCommand();
	int execute();
};

#endif /* SRC_WORKINGPHRASEOUTCOMMAND_H_ */
