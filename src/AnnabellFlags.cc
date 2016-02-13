#include "AnnabellFlags.h"

AnnabellFlags::AnnabellFlags() {
	VerboseFlag = false;
	StartContextFlag = true;
	PeriodFlag = false;
	SpeakerFlag = false;
	AnswerTimeFlag = false;
	AnswerTimeUpdate = false;
	AutoSaveLinkFlag = false;
	YarpOutputFlag = false;
	AutoExploitFlag = false;

	AutoSaveLinkIndex = 0;
	AutoSaveLinkStep = 2000000;

	AnswerTimePhrase = "? do you have any friend -s";
	SpeakerName = "HUM";
}

AnnabellFlags::~AnnabellFlags() {
	// TODO Auto-generated destructor stub
}

