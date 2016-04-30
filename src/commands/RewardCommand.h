#ifndef SRC_REWARDCOMMAND_H_
#define SRC_REWARDCOMMAND_H_

#include "Command.h"

/**
 * Sends the word group to output and produces a (conclusive) reward
 * for the past state-action sequence.
 */
class RewardCommand: public Command {
public:
	RewardCommand();
	int doExecute();
};

#endif /* SRC_REWARDCOMMAND_H_ */
