#ifndef SRC_PARTIALREWARDCOMMAND_H_
#define SRC_PARTIALREWARDCOMMAND_H_

#include "Command.h"

/**
 * Sends the word group to output and produces a partial reward
 * for the past state-action sequence.
 */
class PartialRewardCommand: public Command {
public:
	PartialRewardCommand();
	int execute();
};

#endif /* SRC_PARTIALREWARDCOMMAND_H_ */
