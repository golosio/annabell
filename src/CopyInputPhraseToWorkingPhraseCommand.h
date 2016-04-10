#ifndef SRC_COPYINPUTPHRASETOWORKINGPHRASECOMMAND_H_
#define SRC_COPYINPUTPHRASETOWORKINGPHRASECOMMAND_H_

#include "Command.h"

/**
 * Copies the input phrase to the working phrase.
 */
class CopyInputPhraseToWorkingPhraseCommand: public Command {
public:
	CopyInputPhraseToWorkingPhraseCommand();
	int doExecute();
};

#endif /* SRC_COPYINPUTPHRASETOWORKINGPHRASECOMMAND_H_ */
