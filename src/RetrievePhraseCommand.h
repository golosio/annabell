#ifndef SRC_RETRIEVEPHRASECOMMAND_H_
#define SRC_RETRIEVEPHRASECOMMAND_H_

#include "Command.h"

/**
 * Suggests a phrase to be retrieved by the association process.
 */
class RetrievePhraseCommand: public Command {
public:
	RetrievePhraseCommand();
	int doExecute();
};

#endif /* SRC_RETRIEVEPHRASECOMMAND_H_ */
