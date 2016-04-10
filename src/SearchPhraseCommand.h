#ifndef SRC_SEARCHPHRASECOMMAND_H_
#define SRC_CONTINUECONTEXTCOMMAND_H_

#include "Command.h"

/**
 * Searches a phrase in the current context of the working phrase
 * starting from the beginning of the context
 */
class SearchPhraseCommand: public Command {
public:
	SearchPhraseCommand();
	int doExecute();
};

#endif /* SRC_SEARCHPHRASECOMMAND_H_ */
