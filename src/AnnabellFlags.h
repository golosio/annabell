#ifndef SRC_ANNABELLFLAGS_H_
#define SRC_ANNABELLFLAGS_H_

#include <string>

using namespace std;

/**
 * Utilitary class for grouping all flags and global configurations.
 * This will be likely replaced in the future, but as the refactor is ongoing,
 * it is better to group them here than having them in the global namespace.
 */
class AnnabellFlags {
public:
	AnnabellFlags();
	virtual ~AnnabellFlags();
	bool VerboseFlag;
	bool StartContextFlag;
	bool PeriodFlag;
	bool SpeakerFlag;
	bool AnswerTimeFlag;
	bool AnswerTimeUpdate;
	bool AutoSaveLinkFlag;
	bool YarpOutputFlag;
	bool AutoExploitFlag;

	int AutoSaveLinkIndex;
	long AutoSaveLinkStep;

	string AnswerTimePhrase;
	string SpeakerName;
};

#endif /* SRC_ANNABELLFLAGS_H_ */
