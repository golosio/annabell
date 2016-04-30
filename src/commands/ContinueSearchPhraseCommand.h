#ifndef SRC_CONTINUESEARCHPHRASECOMMAND_H_
#define SRC_CONTINUESEARCHPHRASECOMMAND_H_

#include "Command.h"

/**
 * Searches a phrase in the current context of the working phrase
 * starting from the phrase next to the working phrase
 */
class ContinueSearchPhraseCommand: public Command {
public:
	ContinueSearchPhraseCommand();
	int doExecute();
};

#endif /* SRC_CONTINUESEARCHPHRASECOMMAND_H_ */
