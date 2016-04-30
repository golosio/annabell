#ifndef SRC_EXTRACTWORDGROUPCOMMAND_H_
#define SRC_EXTRACTWORDGROUPCOMMAND_H_

#include "Command.h"

/**
 * Suggests a word group to be extracted from the working phrase.
 */
class ExtractWordGroupCommand: public Command {
public:
	ExtractWordGroupCommand();
	int doExecute();
};

#endif /* SRC_EXTRACTWORDGROUPCOMMAND_H_ */
