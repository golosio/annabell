/*
 * Command.cc
 *
 *  Created on: Jan 10, 2016
 *      Author: jpp
 */

#include <ann_exception.h>
#include <AnnabellFlags.h>
#include <Command.h>
#include <CommandUtils.h>
#include <fssm.h>
#include <gettime.h>
#include <interface.h>
#include <rnd.h>
#include <sizes.h>
#include <ssm.h>
#include <cstdio>
#include <ctime>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include "CommandConstants.h"
#include "CommandFactory.h"
#include "sensorymotor.h"

using namespace sizes;

int TargetExploration(Annabell *annabell, Monitor *Mon, string name, string target_phrase);
int ExplorationApprove(Annabell *annabell, Monitor *Mon);
string Reward(Annabell *annabell, Monitor *Mon, int partial_flag, int n_iter);
string WorkingPhraseOut(Annabell *annabell, Monitor *Mon);
string SentenceOut(Annabell *annabell, Monitor *Mon, display* Display);
int Reset(Annabell *annabell, Monitor *Mon);

int SensoryMotor(vector <string> phrase_token, stringstream &ss, display* Display);

/** TEST functions - start */
int BuildAsTest(Annabell *annabell, Monitor *Mon);
int GetInputPhraseTest(Annabell *annabell, Monitor *Mon, string input_phrase);
int TargetExplorationTest(Annabell *annabell, Monitor *Mon, string name, string target_phrase);
string RewardTest(Annabell *annabell, Monitor *Mon, int partial_flag,
		  int n_iter);
int GetStActIdx(Annabell *annabell, Monitor *Mon);
int ExplorationRetry(Annabell *annabell, Monitor *Mon);
/** TEST functions - end */

template <typename T>
string to_string(T const& value) {
    stringstream sstr;
    sstr << value;
    return sstr.str();
}

Command::Command() {
}

void Command::init(Annabell* annabell, Monitor* monitor, display* aDisplay, timespec* clock0, timespec* clock1, string input_line) {
	this->annabell = annabell;
	this->Mon = monitor;
	this->Display = aDisplay;
	this->clk0 = clock0;
	this->clk1 = clock1;
	this->input_line = input_line;
}

Command::~Command() {

}

int Command::execute() {

	stringstream ss(input_line);
	string parsedToken;

	while (ss >> parsedToken) {

		parsedToken = CommandUtils::processArticle(parsedToken);
		parsedToken = CommandUtils::processPlural(parsedToken);

		this->input_token.push_back(parsedToken);
	}

	this->stringCommand = input_token[0];

	Display->Print(input_line+"\n");

	return this->doExecute();
}

int Command::doExecute() {
	// default behaviour, while the refactor is in progress.
	// This will disappear and default to a dummy method to be overriden by subclasses.
	return ParseCommand(this->annabell, this->Mon, this->Display, this->clk0, this->clk1, this->input_line);
}

/**
 * Read command or input phrase from command line.
 * @returns 2 for .quit command.
 */
int Command::ParseCommand(Annabell *annabell, Monitor *Mon, display* Display, timespec* clk0, timespec* clk1, string input_line) {

 ////////////////////////////////////////
 // Analogous to .x, but it iterates the exploitation process n_iter times
 // and selects the best output phrase.
 ////////////////////////////////////////
  if (stringCommand == BEST_EXPLOIT_CMD_LONG|| stringCommand == BEST_EXPLOIT_CMD) { // best exploitation
    if (input_token.size()<3) {
      Display->Warning("n. of iterations and a phrase should be provided");
      Display->Warning("as argument in the current version.");
      return 1;
    }
    int n_iter;
    if (input_token.size()==1) n_iter=20; // will never be exec in curr. vers.
    else {
      stringstream ss1(input_token[1]);
      ss1 >> n_iter;
      if (!ss1) {
	Display->Warning("Cannot convert token to integer.");
      }
    }
    if (input_token.size()>2) { // would not be necessary in curr. vers.
    	string target_phrase;
      target_phrase = input_token[2];
      for(unsigned int itk=3; itk<input_token.size(); itk++) {
	target_phrase = target_phrase + " " + input_token[itk];
      }
      ExecuteAct(annabell, Mon, STORE_ST_A, NULL_ACT, FLUSH_OUT);
      GetInputPhrase(annabell, Mon, target_phrase);
    }
    annabell->RemPhfWG->OrderedWnnFlag = false;
    //VerboseFlag = true;
    //BestExploitation(annabell, Mon, n_iter, target_phrase);
    //BestExploitation2(annabell, Mon, n_iter, target_phrase);
    //string out_phrase=ExploitationTest(annabell, Mon, n_iter);
    annabell->flags->OutPhrase = Exploitation(annabell, Mon, Display, n_iter);
    annabell->flags->CompleteOutputFlag = true;
    //VerboseFlag = false;
    annabell->RemPhfWG->OrderedWnnFlag = true;
    // check if the output is a sensorymotor command
    CheckSensoryMotor(annabell->flags->OutPhrase, annabell, Display);

    return 0;
  }
  ////////////////////////////////////////
  // Insert the working phrase and the current word group in the top of the
  // goal stack
  ////////////////////////////////////////
  else if (stringCommand == PUSH_GOAL_CMD_LONG || stringCommand == PUSH_GOAL_CMD) { // push goal
    if (input_token.size()>1) {
      Display->Warning("syntax error.");
      return 1;
    }
    if (annabell->flags->ExplorationPhaseIdx == 0) {
      ExecuteAct(annabell, Mon, START_ST_A, NULL_ACT, NULL_ACT);
      ExecuteAct(annabell, Mon, STORE_ST_A, NULL_ACT, W_FROM_IN);
      annabell->flags->ExplorationPhaseIdx++;
    }
    ExecuteAct(annabell, Mon, STORE_ST_A, NULL_ACT, PUSH_GL);
    int ex_ph = annabell->flags->ExplorationPhaseIdx;
    ExplorationApprove(annabell, Mon);
    annabell->flags->ExplorationPhaseIdx = ex_ph;
    //ExplorationPhaseIdx=0;
    //annabell->EPhaseI->Clear();

    return 0;
  }
  ////////////////////////////////////////
  // Removes the goal phrase and the goal word group from the top
  // of the goal stack
  ////////////////////////////////////////
  else if (stringCommand == DROP_GOAL_CMD_LONG || stringCommand == DROP_GOAL_CMD) { // drop goal
    if (input_token.size()>1) {
      Display->Warning("syntax error.");
      return 1;
    }
    ExecuteAct(annabell, Mon, STORE_ST_A, NULL_ACT, DROP_GL);
    int ex_ph = annabell->flags->ExplorationPhaseIdx;
    ExplorationApprove(annabell, Mon);
    annabell->flags->ExplorationPhaseIdx = ex_ph;
    //ExplorationPhaseIdx=0;
    //annabell->EPhaseI->Clear();

    return 0;
  }
  ////////////////////////////////////////
  // Copies the goal phrase from the top of the goal stack to the
  // working phrase
  ////////////////////////////////////////
  else if (stringCommand == GET_GOAL_PHRASE_CMD_LONG || stringCommand == GET_GOAL_PHRASE_CMD) { // get goal phrase
    if (input_token.size()>1) {
      Display->Warning("syntax error.");
      return 1;
    }
    ExecuteAct(annabell, Mon, STORE_ST_A, NULL_ACT, GET_GL_PH);
    ExplorationApprove(annabell, Mon);

    annabell->flags->ExplorationPhaseIdx = 1;
    //annabell->EPhaseI->Clear();

    return 0;
  }

  ////////////////////////////////////////
  else if (stringCommand == WORKING_PHRASE_OUT_CMD_LONG || stringCommand == WORKING_PHRASE_OUT_CMD) { // working phrase out
    if (input_token.size()>1) {
      Display->Warning("syntax error.");
      return 1;
    }
    annabell->flags->OutPhrase = WorkingPhraseOut(annabell, Mon);
    annabell->flags->CompleteOutputFlag = true;
    // check if the output is a sensorymotor command
    CheckSensoryMotor(annabell->flags->OutPhrase, annabell, Display);

    return 0;
  }

  ////////////////////////////////////////
  // Sends to the output the next part of the working phrase, after the
  // current word, and all subsequent phrases of the same context until the
  // end of the context itself.
  ////////////////////////////////////////
  else if (stringCommand == SENTENCE_OUT_CMD_LONG || stringCommand == SENTENCE_OUT_CMD) { // sentence out
    if (input_token.size()>1) {
      Display->Warning("syntax error.");
      return 1;
    }
    annabell->flags->OutPhrase = SentenceOut(annabell, Mon, Display);
    annabell->flags->CompleteOutputFlag = true;

    ExecuteAct(annabell, Mon, STORE_ST_A, NULL_ACT, FLUSH_WG);
    ExplorationApprove(annabell, Mon);
    int n_iter=annabell->ElActfSt->K;
    Reward(annabell, Mon, 0, n_iter);

    annabell->SetAct(START_ST_A, NULL_ACT, NULL_ACT);
    annabell->StActRwdUpdate();
    Mon->Print();
    annabell->Update();
    ExecuteAct(annabell, Mon, STORE_ST_A, NULL_ACT, NULL_ACT);
    ExplorationApprove(annabell, Mon);
    annabell->flags->ExplorationPhaseIdx = 0;
    annabell->flags->AnswerTimeUpdate = true;
    // check if the output is a sensorymotor command
    CheckSensoryMotor(annabell->flags->OutPhrase, annabell, Display);

    return 0;
  }
  ////////////////////////////////////////
  // Gets the input phrase provided as argument without building the
  // associations between the word groups and the phrase.
  ////////////////////////////////////////
  else if (stringCommand == GET_INPUT_PHRASE_CMD_LONG || stringCommand == GET_INPUT_PHRASE_CMD) { // get input phrase
    if (input_token.size()<2) {
      Display->Warning("a phrase should be provided as argument.");
      return 1;
    }
    string target_phrase;
    target_phrase = input_token[1];
    for(unsigned int itk=2; itk<input_token.size(); itk++) {
      target_phrase = target_phrase + " " + input_token[itk];
    }
    ExecuteAct(annabell, Mon, STORE_ST_A, NULL_ACT, FLUSH_OUT);
    GetInputPhrase(annabell, Mon, target_phrase);
    return 0;
  }
  ////////////////////////////////////////
  // Builds the associations between the word groups and the working phrase.
  ////////////////////////////////////////
  else if (stringCommand == BUILD_ASSOCIATION_CMD_LONG || stringCommand == BUILD_ASSOCIATION_CMD) { // build association
    if (input_token.size()!=1) {
      Display->Warning("no arguments should be provided.");
      return 1;
    }
    BuildAs(annabell, Mon);

    return 0;
  }
  ////////////////////////////////////////
  else if (stringCommand==".build_association_test" || stringCommand==".bat") { // build association
    if (input_token.size()!=1) {
      Display->Warning("no arguments should be provided.");
      return 1;
    }
    BuildAsTest(annabell, Mon);

    return 0;
  }
  ////////////////////////////////////////
  // Executes a system update with a NULL action
  ////////////////////////////////////////
  else if (stringCommand == NULL_CMD_LONG || stringCommand == NULL_CMD) { // null action
    if (input_token.size()!=1) {
      Display->Warning("syntax error.");
      return 1;
    }
    annabell->SetAct(NULL_ACT, NULL_ACT);
    annabell->Update();
    return 0;
  }

  ////////////////////////////////////////
  // Sends a command to the monitor
  ////////////////////////////////////////
  else if (stringCommand == MONITOR_CMD_LONG || stringCommand == MONITOR_CMD) { // send a command to the monitor
    if (input_token.size()<2) {
      Display->Warning("a monitor command should be provided as argument.");
      return 1;
    }
    string target_phrase;
    target_phrase = input_token[1];
      for(unsigned int itk=2; itk<input_token.size(); itk++) {
	target_phrase = target_phrase + " " + input_token[itk];
      }

    Mon->ParseCommand(input_token);
    return 0;
  }

  ////////////////////////////////////////
  // Executes single elaboration action
  ////////////////////////////////////////
  else if (stringCommand == ACTION_CMD_LONG || stringCommand == ACTION_CMD) { // executes single elaboration action
    if (input_token.size()!=2) {
      Display->Warning("syntax error.");
      return 1;
    }
    string action_name=input_token[1];
    int iact;
    for (iact=0; iact<ElActSize && Mon->ElActName[iact]!=action_name;
	 iact++);
    if (iact==ElActSize) {
      Display->Warning("unknown action name.");
      return 1;
    }
    annabell->SetAct(NULL_ACT, iact);
    annabell->Update();
    Mon->Print();
    return 0;
  }

  ////////////////////////////////////////
  // Displays the execution time
  ////////////////////////////////////////
  else if (stringCommand == TIME_CMD_LONG || stringCommand == TIME_CMD) { // time
    if (input_token.size()>1) {
      Display->Warning("syntax error.");
      return 1;
    }

    GetRealTime(clk1);

    Display->Print("Elapsed time: " + to_string
		  ((double)(clk1->tv_sec - clk0->tv_sec)
		    + (double)(clk1->tv_nsec - clk0->tv_nsec)*1e-9) + "\n");
    Display->Print("StActMem->act_time: " + to_string(annabell->StActMem->act_time)
		  + "\n");
    Display->Print("StActMem->as_time: " + to_string(annabell->StActMem->as_time)
		  + "\n");
    Display->Print("ElActfSt->act_time: " + to_string(annabell->ElActfSt->act_time)
		  + "\n");
    Display->Print("ElActfSt->as_time: " + to_string(annabell->ElActfSt->as_time)
		  + "\n");
    Display->Print("RemPh->act_time: " + to_string(annabell->RemPh->act_time)
		  + "\n");
    Display->Print("RemPh->as_time: " + to_string(annabell->RemPh->as_time) + "\n");
    Display->Print("RemPhfWG->act_time: " + to_string(annabell->RemPhfWG->act_time)
		  + "\n");
    Display->Print("RemPhfWG->as_time: " + to_string(annabell->RemPhfWG->as_time)
		  + "\n");
    int nl = 0;
    Display->Print("ElActfSt neurons: " + toStr(annabell->ElActfSt->NewWnnNum)
		  + "\n");
    for (int i=0; i<annabell->ElActfSt->NewWnnNum; i++) {
      nl += annabell->ElActfSt->Nr[i]->NL();
    }
    Display->Print("ElActfSt links: " + toStr(nl) + "\n");

    return 0;
  }

  ////////////////////////////////////////
  // Displays some statistical information
  ////////////////////////////////////////
  else if (stringCommand == STAT_CMD) { // statistic
    if (input_token.size()>1) {
      Display->Warning("syntax error.");
      return 1;
    }
    int nw = 0;
    for (int iw=0; iw<WMSize; iw++) {
      if (Mon->wflag[iw]>0.5) nw++;
    }
    Display->Print("Learned Words: " + toStr(nw) + "\n");
    int nph = annabell->MemPh->HighVect[0];
    Display->Print("Learned Phrases: " + toStr(nph) + "\n");
    int nphwg = annabell->RemPhfWG->NewWnnNum;
    Display->Print("Learned associations between word groups and phrases: "
		  + toStr(nphwg) + "\n");

    //Display->Print("IW neurons: " + toStr(annabell->IW->NewWnnNum) + "\n");
    Display->Print("IW input links: " + toStr(annabell->IW->CountInputLinks())
		  + "\n");
    Display->Print("ElActfSt neurons: " + toStr(annabell->ElActfSt->NewWnnNum)
		  + "\n");
    Display->Print("ElActfSt input links: " +
		  toStr(annabell->ElActfSt->CountSparseInputLinks()) + "\n");
    Display->Print("ElActfSt virtual input links: " +
		  toStr(annabell->ElActfSt->CountVirtualInputLinks()) + "\n");
    Display->Print("ElActfSt output links: " +
		  toStr(annabell->ElActfSt->CountOutputLinks()) + "\n");
    Display->Print("RemPh output links: " +
		  toStr(annabell->RemPh->CountSparseOutputLinks()) + "\n");
    Display->Print("RemPh virtual output links: " +
		  toStr(annabell->RemPh->CountVirtualOutputLinks()) + "\n");
    Display->Print("RemPhfWG neurons: " + toStr(annabell->RemPhfWG->NewWnnNum)
		  + "\n");
    Display->Print("RemPhfWG input links: " +
		  toStr(annabell->RemPhfWG->CountSparseInputLinks()) + "\n");
    Display->Print("RemPhfWG virtual input links: " +
		  toStr(annabell->RemPhfWG->CountVirtualInputLinks()) + "\n");
    Display->Print("RemPhfWG output links: " +
		  toStr(annabell->RemPhfWG->CountSparseOutputLinks()) + "\n");
    Display->Print("RemPhfWG virtual output links: " +
		  toStr(annabell->RemPhfWG->CountVirtualOutputLinks()) + "\n");

    return 0;
  }

  ////////////////////////////////////////
  // Starts CUDA version of exploitation mode
  ////////////////////////////////////////
  else if (stringCommand == CUDA_CMD_LONG || stringCommand == CUDA_CMD) { // time
    if (input_token.size()>1) {
      Display->Warning("syntax error.");
      return 1;
    }
#ifdef CUDA
    annabell->ElActfSt->cuda_CopyInpuLinks();
    annabell->CudaFlag = true;
    return 0;
#else
    Display->Warning("CUDA is not available.");
    return 1;
#endif

  }

  ////////////////////////////////////////
  // Saves all variable-weight links to a file
  ////////////////////////////////////////
  else if (stringCommand == SAVE_CMD) { // save links
    if (input_token.size()>2) {
      Display->Warning("syntax error.");
      return 1;
    }
    string filename;
    if (input_token.size()==1) filename = "links.dat";
    else filename = input_token[1];

    FILE *fp=fopen(filename.c_str(), "wb");
    Mon->SaveWM(fp);
    if (annabell->MemPh->HighVect.size()!=1) {
      Display->Warning("Error on MemPh.");
      return 1;
    }
    fwrite(&annabell->MemPh->HighVect[0], sizeof(int), 1, fp);
    if (annabell->StartPh->HighVect.size()!=1) {
      Display->Warning("Error on StartPh.");
      return 1;
    }
    fwrite(&annabell->StartPh->HighVect[0], sizeof(int), 1, fp);

    annabell->IW->SaveNr(fp);
    annabell->IW->SaveInputLinks(fp);
    annabell->ElActfSt->SaveNr(fp);
    annabell->ElActfSt->SaveSparseInputLinks(fp);
    annabell->ElActfSt->SaveOutputLinks(fp);
    annabell->RemPh->SaveSparseOutputLinks(fp);
    annabell->RemPhfWG->SaveNr(fp);
    annabell->RemPhfWG->SaveSparseInputLinks(fp);
    annabell->RemPhfWG->SaveSparseOutputLinks(fp);

    fclose(fp);

    return 0;
  }
  ////////////////////////////////////////
  // Loads all variable-weight links from a file
  ////////////////////////////////////////
  else if (stringCommand == LOAD_CMD) { // load links
    if (input_token.size()>2) {
      Display->Warning("syntax error.");
      return 1;
    }
    string filename;
    if (input_token.size()==1) filename = "links.dat";
    else filename = input_token[1];
    FILE *fp;
    if ((fp=fopen(filename.c_str(), "rb")) == NULL) {
      Display->Warning("Links file not found.");
      return 1;
    }
    Mon->LoadWM(fp);
    int i1;
    int nmemb = fread(&i1, sizeof(int), 1, fp);
    if (nmemb!=1) {
      Display->Warning("Error reading MemPh.");
      return 1;
    }
    annabell->MemPh->Clear();
    annabell->MemPh->HighVect.push_back(i1);
    annabell->MemPh->Nr[i1]->O = 1;

    nmemb = fread(&i1, sizeof(int), 1, fp);
    if (nmemb!=1) {
      Display->Warning("Error reading StartPh.");
      return 1;
    }
    annabell->StartPh->Clear();
    annabell->StartPh->HighVect.push_back(i1);
    annabell->StartPh->Nr[i1]->O = 1;

    annabell->IW->LoadNr(fp);
    annabell->IW->LoadInputLinks(fp);
    annabell->ElActfSt->LoadNr(fp);
    annabell->ElActfSt->LoadSparseInputLinks(fp);
    annabell->ElActfSt->LoadOutputLinks(fp);
    annabell->RemPh->LoadSparseOutputLinks(fp);
    annabell->RemPhfWG->LoadNr(fp);
    annabell->RemPhfWG->LoadSparseInputLinks(fp);
    annabell->RemPhfWG->LoadSparseOutputLinks(fp);

    fclose(fp);

    return 0;
  }

  ////////////////////////////////////////
  // Writes the output to a log file
  ////////////////////////////////////////
  else if (stringCommand == LOGFILE_CMD) { // write output to a log file
    if (input_token.size()<2) {
      Display->Warning("a file name or off should be provided as argument.");
      return 1;
    }
    if (input_token[1] == LOGFILE_OFF && Display->LogFileFlag) {
      Mon->Display.LogFileFlag = Display->LogFileFlag = false;
      Display->LogFile->close();
      return 0;
    }
    else {
      if (Display->LogFileFlag) {
	Mon->Display.LogFileFlag = Display->LogFileFlag = false;
	Display->LogFile->close();
      }
      Display->LogFile->open(input_token[1].c_str());
      if (!Display->LogFile->is_open()) {
	Display->Warning("cannot open output log file.");
	return 1;
      }
      Mon->Display.LogFileFlag = Display->LogFileFlag = true;
      return 0;
    }
    return 1;
  }
  ////////////////////////////////////////
  // Displays the speaker names using the CHAT format (3 characters)
  ////////////////////////////////////////
  else if (stringCommand == SPEAKER_CMD) { // display the name of the speaker
    if (input_token.size()<2) {
      Display->Warning("a speaker name or off should be provided as argument.");
      return 1;
    }
    if (input_token[1]=="off") {
    	annabell->flags->SpeakerFlag = false;
      return 0;
    }
    else {
    	annabell->flags->SpeakerName = input_token[1];
    	annabell->flags->SpeakerFlag = true;
      return 0;
    }
    return 1;
  }

  ////////////////////////////////////////
  // Records the answer time
  ////////////////////////////////////////
  else if (stringCommand == ANSWER_TIME_CMD_LONG || stringCommand == ANSWER_TIME_CMD) { // record answer time
    if (input_token.size()!=1) {
      Display->Warning("syntax error.");
      return 1;
    }
    FILE *at_fp=fopen("answer_time.dat", "w");
    fclose(at_fp);

    annabell->flags->AnswerTimeFlag = true;

    return 0;
  }

  ////////////////////////////////////////
  // Evaluates the answer time
  ////////////////////////////////////////
  else if (stringCommand == EVALUATE_ANSWER_TIME_CMD_LONG || stringCommand == EVALUATE_ANSWER_TIME_CMD) {//evaluate answer time
    if (input_token.size()!=1) {
      Display->Warning("syntax error.");
      return 1;
    }

    struct timespec clk0, clk1;
    GetRealTime(&clk0);

    ExecuteAct(annabell, Mon, STORE_ST_A, NULL_ACT, FLUSH_OUT);
    GetInputPhrase(annabell, Mon, annabell->flags->AnswerTimePhrase);
    Exploitation(annabell, Mon, Display, 1);

    GetRealTime(&clk1);
    double answ_time = clk1.tv_sec - clk0.tv_sec
      + (double)(clk1.tv_nsec - clk0.tv_nsec)*1e-9;
    double link_num = (double)annabell->ElActfSt->CountSparseInputLinks();
    //FILE *at_fp=fopen("answer_time.dat", "a");
    printf("EAT: %.3e\t%.3f\n", link_num, answ_time);
    //fclose(at_fp);

    return 0;
  }

  ////////////////////////////////////////
  // Periodically save variable-link weights in files with progressive numbers
  ////////////////////////////////////////
  else if (stringCommand == AUTO_SAVE_LINKS_CMD_LONG || stringCommand == AUTO_SAVE_LINKS_CMD) { // autosave links
    if (input_token.size()!=1) {
      Display->Warning("syntax error.");
      return 1;
    }
    annabell->flags->AutoSaveLinkFlag = true;
    double link_num = (double)annabell->ElActfSt->CountSparseInputLinks();
    annabell->flags->AutoSaveLinkIndex = (int)(link_num/annabell->flags->AutoSaveLinkStep);

    return 0;
  }

  ////////////////////////////////////////
  // Displays a period at the end of the output sentences
  ////////////////////////////////////////
  else if (stringCommand == PERIOD_CMD) { // display a period at the end of output sentences
    if (input_token.size()<2) {
      Display->Warning("on or off should be provided as argument.");
      return 1;
    }
    if (input_token[1] == PERIOD_OFF) {
    	annabell->flags->PeriodFlag = false;
      return 0;
    }
    else if (input_token[1] == PERIOD_ON) {
    	annabell->flags->PeriodFlag = true;
      return 0;
    }
    else {
      Display->Warning("on or off should be provided as argument.");
      return 1;
    }

    return 1;
  }
  ////////////////////////////////////////
  // Takes the input from a YARP port
  ////////////////////////////////////////
  else if (stringCommand == YARP_INPUT_CMD_LONG || stringCommand == YARP_INPUT_CMD) {
    if (input_token.size()>1) {
      Display->Warning("syntax error.");
      return 1;
    }
    YarpInput(Display);

    return 0;
  }
  ////////////////////////////////////////
  // Sends the output to a YARP port
  ////////////////////////////////////////
  else if (stringCommand == YARP_OUTPUT_CMD_LONG || stringCommand == YARP_OUTPUT_CMD) {
    if (input_token.size()==1 ||
	(input_token.size()==2 && input_token[1]=="on")) {
      if ((annabell->flags->YarpOutputFlag=YarpOutputInit(Display))==true)
	return 0;
      else return 1;
    }
    else if (input_token.size()==2 && input_token[1]=="off") {
      YarpOutputClose(Display);
      annabell->flags->YarpOutputFlag = false;
      return 0;
    }
    else {
      Display->Warning("on or off should be provided as argument.");
      return 1;
    }
  }
  ////////////////////////////////////////
  // Set automatic exploitation after every input phrase
  ////////////////////////////////////////
  else if (stringCommand == AUTO_EXPLOIT_CMD_LONG || stringCommand == AUTO_EXPLOIT_CMD) {
    if (input_token.size()==1 ||
	(input_token.size()==2 && input_token[1]=="on")) {
      annabell->flags->AutoExploitFlag=true;
      return 0;
    }
    else if (input_token.size()==2 && input_token[1]=="off") {
      annabell->flags->AutoExploitFlag=false;
      return 0;
    }
    else {
      Display->Warning("on or off should be provided as argument.");
      return 1;
    }
  }
  ////////////////////////////////////////
  // Sends a command to a sensorymotor device
  ////////////////////////////////////////
  else if (stringCommand == SENSORYMOTOR_CMD_LONG || stringCommand == SENSORYMOTOR_CMD) {
    if (input_token.size()<2) {
      Display->Warning("A sensorymotor command should be provided "
		       "as argument.");
      return 1;
    }
    string buffer;
    buffer = input_token[1];
    if (buffer[0]!='[' || buffer[buffer.size()-1] != ']') {
      // phrase is not a sensorymotor command
      Display->Warning("The argument is not a sensorymotor command.");
      return 1;
    }
    input_token.erase(input_token.begin()); // remove the first token
    stringstream in_ss("");
    SensoryMotor(input_token, in_ss, Display); // execute sensorymotor command

    bool ax_tmp=annabell->flags->AutoExploitFlag;
    annabell->flags->AutoExploitFlag=false;
    // parse and process sensorymotor response
    while(getline (in_ss, buffer))  {
      //Display->Print(buf+"\n");
      Command* c = CommandFactory::newCommand(buffer);
      int commandResult = c->execute();
      delete c;
      if(commandResult == 2) break;
    }
    annabell->flags->AutoExploitFlag=ax_tmp;

    return 0;
  }
  ////////////////////////////////////////
  // Reset the system activation state
  ////////////////////////////////////////
  else if (stringCommand == RESET_CMD_LONG || stringCommand == RESET_CMD) {
    if (input_token.size()>1) {
      Display->Warning("syntax error.");
      return 1;
    }
    Reset(annabell, Mon);

    return 0;
  }
  ////////////////////////////////////////
  else {
    Display->Warning("Unknown command.");
    return 1;
  }

  return 1;
}

int GetInputPhrase(Annabell *annabell, Monitor *Mon, string input_phrase)
{
  int vin[WSize];

  //cout << input_phrase << endl;
  annabell->StartAcquireFlag->Nr[0]->O = 1;

  annabell->AcquireUpdate();
  annabell->ElActFL->ActivOut();
  annabell->AcqAct->ActivOut();
  annabell->ElAct->ActivOut();
  Mon->Print();
  annabell->Update();

  string buf; // Have a buffer string
  stringstream ss(input_phrase); // Insert the string into a stream
  int i=0;
  while (ss >> buf && i<PhSize) {
    if (buf=="an") buf="a";
    if (buf=="-es") buf="-s";
    interface::w2bin((char*)buf.c_str(),vin);
    annabell->W->Fill(vin);
    annabell->AcquireUpdate();
    annabell->ElActFL->ActivOut();
    annabell->AcqAct->ActivOut();
    annabell->ElAct->ActivOut();
    Mon->Print();
    annabell->Update();
    Mon->MapW((char*)buf.c_str()); //swap with previous line!!!!
    i++;
  }
  for (int ic=0; ic<WSize; ic++) vin[ic]=0; // blank word at the end of phrase
  annabell->W->Fill(vin);
  do {
    annabell->AcquireUpdate();
    annabell->ElActFL->ActivOut();
    annabell->AcqAct->ActivOut();
    annabell->ElAct->ActivOut();
    Mon->Print();
    annabell->Update();
  } while (annabell->EndAcquireFlag->Nr[0]->O==0);

  annabell->flags->ExplorationPhaseIdx = 0;
  annabell->EPhaseI->Clear();

  annabell->AcquireUpdate();
  annabell->SetMode(NULL_MODE);

  return 0;
}

int ExecuteAct(Annabell *annabell, Monitor *Mon, int rwd_act, int acq_act, int el_act) {
  annabell->SetAct(rwd_act, acq_act, el_act);
  Mon->Print();
  if (annabell->flags->VerboseFlag) Mon->PrintRwdAct();
  annabell->StActRwdUpdate();
  if (annabell->flags->VerboseFlag) {Mon->PrintElActFL(); Mon->PrintElAct();}
  annabell->Update();

  return 0;
}

string Exploitation(Annabell *annabell, Monitor *Mon, display* Display, int n_iter)
{
  int Nupdate, MaxNupdate=4000;
  int vin[IterSize];
  for (int i=0; i<IterSize-1; i++) vin[i] = (i==n_iter-1) ? 1 : 0;
  annabell->IterNum->Fill(vin);

  annabell->StartExploitFlag->Nr[0]->O = 1;
  string out_phrase="", best_phrase="";
  int BestDB=1000;
  int IterI=0;
  Nupdate=0;
  do {
    Nupdate++;
    //if (Nupdate%100==0) Display->Print("Number of updates: " + toStr(Nupdate)
    //				      + "\n");
    if (Nupdate>=MaxNupdate) {
      Display->Warning("Exploitation number of updates >= " +
				  toStr(MaxNupdate) + "\n");
      Reset(annabell, Mon);
      annabell->EndExploitFlag->Nr[0]->O=1;
      annabell->ExploitUpdate();
      annabell->SetMode(NULL_MODE);

      return "";
    }

    annabell->RewardUpdate();
    annabell->ExploitUpdate();
    annabell->RwdAct->ActivOut();
    annabell->ElActFL->ActivOut();
    annabell->AcqAct->ActivOut();
    //Mon->PrintRwdAct();

    //if (annabell->ElAct->Default->Nr[0]->O<0.5)
    Mon->Print();
    if (annabell->flags->VerboseFlag) Mon->PrintRwdAct();
    //int prev_act=Mon->GetElAct(); // temporary
    annabell->StActRwdUpdate();
    //int next_act=Mon->GetElAct(); // temporary
    if (annabell->flags->VerboseFlag) Mon->PrintElActFL();
    if (annabell->flags->VerboseFlag) Mon->PrintElAct();

    annabell->Update();

    if (n_iter>1 && annabell->Exploit_I30->Nr[0]->O>0.5) {
      Display->Print("Trying n. " + toStr(IterI));
      IterI++;
    }

      /////////////////////// the following has to be checked when possible

      //if (next_act==SNT_OUT) {//annabell->ElAct->Nr[SNT_OUT-1]->O>0.5) {
    if (annabell->ElAct->Nr[SNT_OUT-1]->O>0.5 &&
	annabell->RwdAct->Nr[STORE_ST_A-1]->O>0.5) {
      int DB = annabell->ElActfSt->DB;
      if (out_phrase!="") out_phrase = out_phrase + " ";
      out_phrase = out_phrase + SentenceOut(annabell, Mon, Display);
      if (n_iter>1) cout << "DB: " << DB << endl;
      else Display->Print(".\n");
      if (DB<BestDB) {
	BestDB = DB;
	best_phrase = out_phrase;
      }
      out_phrase="";
      break;
    }
      /////////////////////// end part to be checked

    if (annabell->OutFlag->Nr[0]->O>0.5 && annabell->RwdAct->Nr[STORE_ST_A-1]->O>0.5) {
      Mon->GetPhM("OutPhB", annabell->OutPhB);
      Display->Print(" -> " + Mon->OutPhrase+"\n");
      if (out_phrase!="") out_phrase = out_phrase + " ";
      out_phrase = out_phrase + Mon->OutPhrase;
      Nupdate=0;
    }
    //if (annabell->OutFlag->Nr[0]->O>0.5) {
    if (annabell->ElAct->Nr[CONTINUE-1]->O>0.5) {
      Display->Print(" ... ");
    }
    if (annabell->ElAct->Nr[DONE-1]->O>0.5) {
      int DB = annabell->ElActfSt->DB;
      if (n_iter>1) cout << "DB: " << DB << endl;
      else Display->Print(".\n");
      if (DB<BestDB) {
	BestDB = DB;
	best_phrase = out_phrase;
      }
      out_phrase="";
    }
  } while (annabell->EndExploitFlag->Nr[0]->O==0);

  annabell->ExploitUpdate();
  if (annabell->flags->SpeakerFlag) Display->Print("*SYS:\t");
  Display->Print(best_phrase);
  if (annabell->flags->PeriodFlag) Display->Print(" . \n");
  else Display->Print("\n");
  annabell->SetMode(NULL_MODE);

  return best_phrase;
}

int BuildAs(Annabell *annabell, Monitor *Mon)
{
  if (annabell->flags->StartContextFlag) {
	  annabell->flags->StartContextFlag = false;
    annabell->SetStartPhFlag->Nr[0]->O = 1;
  }
  else annabell->SetStartPhFlag->Nr[0]->O = 0;

  annabell->StartAssociateFlag->Nr[0]->O = 1;
  do {
    annabell->AssociateUpdate();
    annabell->ElActFL->ActivOut();
    annabell->AcqAct->ActivOut();
    annabell->ElAct->ActivOut();
    Mon->Print();
    annabell->Update();
  } while (annabell->EndAssociateFlag->Nr[0]->O==0);

  annabell->AssociateUpdate();
  annabell->SetMode(NULL_MODE);

  return 0;
}

int TargetExploration(Annabell *annabell, Monitor *Mon, string name, string target_phrase)
{
  int vin[PhSize];

  // temp1 start here
  annabell->EPhaseI->Clear();
  annabell->EPhaseI->Nr[annabell->flags->ExplorationPhaseIdx]->O = 1;
  // temp1 end here

  if (name=="WGB") annabell->WGTargetFlag->Nr[0]->O = 1;
  else annabell->WGTargetFlag->Nr[0]->O = 0;

  annabell->TargetEqWkPhFlag->Nr[0]->O = 0;
  annabell->TargetEqWGFlag->Nr[0]->O = 0;
  annabell->StartExploreFlag->Nr[0]->O = 1;

  int i=0;
  do {
    annabell->AssociateUpdate();
    annabell->RewardUpdate();
    annabell->ExploitUpdate();
    annabell->ExploreUpdate();
    annabell->RwdAct->ActivOut();
    annabell->ElActFL->ActivOut();
    annabell->AcqAct->ActivOut();

    //if (annabell->ElAct->Default->Nr[0]->O<0.5)
    Mon->Print();
    if (annabell->flags->VerboseFlag) Mon->PrintRwdAct();
    annabell->StActRwdUpdate();
    if (annabell->flags->VerboseFlag) {Mon->PrintElActFL(); Mon->PrintElAct();}

    annabell->Update();

    if (annabell->CheckWGFlag->Nr[0]->O>0.5) {
      Mon->GetPhM(name, annabell->WGB);
      if (target_phrase==Mon->OutPhrase)
	annabell->TargetEqWGFlag->Nr[0]->O = 1;
    }
    if (annabell->CheckWkPhFlag->Nr[0]->O>0.5) {
      Mon->GetPhM(name, annabell->WkPhB);
      if (target_phrase==Mon->OutPhrase)
	annabell->TargetEqWkPhFlag->Nr[0]->O = 1;
    }
    if (annabell->GetRndFlag->Nr[0]->O>0.5) {
      int N1, N2;
      N1=rnd_int()%10; //N1=1+seed%5;
      N2=rnd_int()%6; //1+seed%5;
      //cout << N1 << " " << N2 << endl;
      for (int i=0; i<PhSize; i++) vin[i] = (i==N1) ? 1 : 0;
      annabell->SkipW->Fill(vin);
      for (int i=0; i<PhSize; i++) vin[i] = (i==N2) ? 1 : 0;
      annabell->ExplWordNum->Fill(vin);
    }
    i++;
  } while (annabell->EndExploreFlag->Nr[0]->O==0);

  annabell->ExploreUpdate();

  annabell->SetMode(NULL_MODE);

  // temp2 start here
  for (int i=0; i<5; i++) {
    if (annabell->EPhaseI->Nr[i]->O>0.5) {
      annabell->flags->ExplorationPhaseIdx = i;
      break;
    }
  }
  // temp2 end here

  return 0;
}

int SearchContext(Annabell *annabell, Monitor *Mon, display* Display, string target_phrase)
{
  //cout << "\nSearch context\n";
  annabell->SetMode(EXPLORE);

  if (annabell->flags->ExplorationPhaseIdx == 0) {
    ExecuteAct(annabell, Mon, START_ST_A, NULL_ACT, NULL_ACT);
    ExecuteAct(annabell, Mon, STORE_ST_A, NULL_ACT, W_FROM_IN);
  }
  ExplorationApprove(annabell, Mon);
  ExecuteAct(annabell, Mon, STORE_ST_A, NULL_ACT, GET_START_PH);
  annabell->flags->ExplorationPhaseIdx = 3;
  Mon->GetPhM("WkPhB", annabell->WkPhB);
  cout << " ... " << Mon->OutPhrase << endl;
  if (target_phrase!=Mon->OutPhrase) {
	annabell->flags->ExplorationPhaseIdx = 4;
    while(Mon->OutPhrase!=".end_context" && Mon->OutPhrase!="") {
      ExecuteAct(annabell, Mon, STORE_ST_A, NULL_ACT, GET_NEXT_PH);
      Mon->GetPhM("WkPhB", annabell->WkPhB);
      cout << " ... " << Mon->OutPhrase << endl;
      if (target_phrase==Mon->OutPhrase) break;
    }
  }
  if (target_phrase!=Mon->OutPhrase) {
    ExecuteAct(annabell, Mon, RETR_SAI, NULL_ACT, NULL_ACT);
    ExecuteAct(annabell, Mon, RETR_ST, NULL_ACT, NULL_ACT);
    ExecuteAct(annabell, Mon, RETR_SAI, NULL_ACT, NULL_ACT);
  }
  else  {
    ExplorationApprove(annabell, Mon);
    Display->Print(target_phrase + "\n");
  }

  annabell->SetMode(NULL_MODE);

  return 0;
}

int ContinueSearchContext(Annabell *annabell, Monitor *Mon, display* Display, string target_phrase)
{
  //cout << "\nSearch context\n";
  annabell->SetMode(EXPLORE);

  if (annabell->flags->ExplorationPhaseIdx == 0) {
    ExecuteAct(annabell, Mon, START_ST_A, NULL_ACT, NULL_ACT);
    ExecuteAct(annabell, Mon, STORE_ST_A, NULL_ACT, W_FROM_IN);
  }
  ExplorationApprove(annabell, Mon);

  annabell->flags->ExplorationPhaseIdx = 4;
  do {
    ExecuteAct(annabell, Mon, STORE_ST_A, NULL_ACT, GET_NEXT_PH);
    Mon->GetPhM("WkPhB", annabell->WkPhB);
    cout << " ... " << Mon->OutPhrase << endl;
    if (target_phrase==Mon->OutPhrase) break;
  } while(Mon->OutPhrase!=".end_context" && Mon->OutPhrase!="");
  if (target_phrase!=Mon->OutPhrase) {
    ExecuteAct(annabell, Mon, RETR_SAI, NULL_ACT, NULL_ACT);
    ExecuteAct(annabell, Mon, RETR_ST, NULL_ACT, NULL_ACT);
    ExecuteAct(annabell, Mon, RETR_SAI, NULL_ACT, NULL_ACT);
  }
  else  {
    ExplorationApprove(annabell, Mon);
    Display->Print(target_phrase + "\n");
  }

  annabell->SetMode(NULL_MODE);

  return 0;
}

int ExplorationApprove(Annabell *annabell, Monitor *Mon)
{
  ExecuteAct(annabell, Mon, STORE_SAI, NULL_ACT, NULL_ACT);

  if (annabell->flags->ExplorationPhaseIdx != 4) {
	  annabell->flags->ExplorationPhaseIdx++;
  }

  return 0;
}

string Reward(Annabell *annabell, Monitor *Mon, int partial_flag, int n_iter)
{
  int vin[IterSize];
  for (int i=0; i<IterSize; i++) vin[i] = (i==n_iter) ? 1 : 0;
  annabell->IterNum->Fill(vin);

  annabell->StartRewardFlag->Nr[0]->O = 1;
  annabell->PartialFlag->Nr[0]->O = partial_flag;
  do {
    annabell->RewardUpdate();
    annabell->ElActFL->ActivOut();
    annabell->AcqAct->ActivOut();
    annabell->StActRwdUpdate();
    Mon->Print();
    annabell->Update();
  } while (annabell->EndRewardFlag->Nr[0]->O==0);

  annabell->RewardUpdate();
  annabell->SetMode(NULL_MODE);
  Mon->GetPhM("OutPhB", annabell->OutPhB);
  string out_phrase = Mon->OutPhrase;

  return out_phrase;
}

string WorkingPhraseOut(Annabell *annabell, Monitor *Mon)
{
  //cout << "\nSend working phrase to output\n";
  annabell->SetMode(EXPLORE);

  ExecuteAct(annabell, Mon, STORE_ST_A, NULL_ACT, W_FROM_WK);
  ExecuteAct(annabell, Mon, STORE_ST_A, NULL_ACT, FLUSH_WG);
  for (int i=0; i<PhSize; i++) {
    ExecuteAct(annabell, Mon, STORE_ST_A, NULL_ACT, GET_W);
    ExecuteAct(annabell, Mon, STORE_ST_A, NULL_ACT, NEXT_W);
    // try to restore the following lines
    //Mon->GetWM("CW", annabell->CW);
    //	if (Mon->OutStr[0]=="") break;
    //}
  }
  annabell->SetAct(STORE_ST_A, NULL_ACT, FLUSH_OUT);
  annabell->StActRwdUpdate();
  Mon->Print();
  annabell->Update();

  annabell->SetAct(STORE_ST_A, NULL_ACT, WG_OUT);
  annabell->StActRwdUpdate();
  Mon->Print();
  annabell->Update();
  Mon->GetPhM("OutPhB", annabell->OutPhB);
  string out_phrase = Mon->OutPhrase;

  annabell->flags->ExplorationPhaseIdx = 2;
  ExplorationApprove(annabell, Mon);

  annabell->SetMode(NULL_MODE);

  return out_phrase;
}

string SentenceOut(Annabell *annabell, Monitor *Mon, display* Display)
{
  //cout << "\nSend sentence to output\n";
  string out_phrase = "";

  ExecuteAct(annabell, Mon, STORE_ST_A, NULL_ACT, SNT_OUT);
  if (annabell->ModeFlags->Nr[EXPLORE]->O>0.5) {
    ExplorationApprove(annabell, Mon);
    annabell->flags->ExplorationPhaseIdx = 2;
  }
  //ExecuteAct(annabell, Mon, NULL_ACT, NULL_ACT, FLUSH_WG);

  Mon->GetPhM("WkPhB", annabell->WkPhB);
  while(Mon->OutPhrase!=".end_context" && Mon->OutPhrase!="") {
    ExecuteAct(annabell, Mon, NULL_ACT, NULL_ACT, FLUSH_WG);
    //while (annabell->PhI->Nr[PhSize-1]->O<0.5) {
    for(;;) {
      ExecuteAct(annabell, Mon, NULL_ACT, NULL_ACT, GET_W);
      if (annabell->PhI->Nr[PhSize-1]->O>0.5) break;
      ExecuteAct(annabell, Mon, NULL_ACT, NULL_ACT, NEXT_W);
      // try to restore the following lines
      //Mon->GetWM("CW", annabell->CW);
      //	if (Mon->OutStr[0]=="") break;
      //}
    }
    ExecuteAct(annabell, Mon, NULL_ACT, NULL_ACT, FLUSH_OUT);
    ExecuteAct(annabell, Mon, NULL_ACT, NULL_ACT, WG_OUT);
    Mon->GetPhM("OutPhB", annabell->OutPhB);
    if (out_phrase!="") Display->Print(" ... ");
    Display->Print("-> " + Mon->OutPhrase+"\n");
    out_phrase = out_phrase + Mon->OutPhrase;
    ExecuteAct(annabell, Mon, NULL_ACT, NULL_ACT, GET_NEXT_PH);
    Mon->GetPhM("WkPhB", annabell->WkPhB);
  }
  //Display->Print("\n");

  return out_phrase;
}

int Reset(Annabell *annabell, Monitor *Mon)
{
  annabell->SetMode(NULL_MODE);
  //StartContextFlag=true;

  ExecuteAct(annabell, Mon, NULL_ACT, FLUSH, NULL_ACT);
  ExecuteAct(annabell, Mon, NULL_ACT, NULL_ACT, FLUSH_OUT);
  for (int i=0; i<5; i++)
    ExecuteAct(annabell, Mon, STORE_ST_A, NULL_ACT, DROP_GL);

  int v1[WSize];
  for (int ic=0; ic<WSize; ic++) v1[ic]=0;
  annabell->W->Fill(v1);

  int v2[PhSize];
  for (int i=0; i<PhSize; i++) v2[i] = 0;
  annabell->InI->Fill(v2);
  annabell->PhI->Fill(v2);

  annabell->Update();
  annabell->StartExploitFlag->Nr[0]->O = 1;
  annabell->ExploitUpdate();
  annabell->EndExploitFlag->Nr[0]->O=1;
  annabell->ExploitUpdate();

  ExecuteAct(annabell, Mon, START_ST_A, NULL_ACT, NULL_ACT);
  ExplorationApprove(annabell, Mon);
  annabell->flags->ExplorationPhaseIdx=0;

  return 0;
}


/**
 * TEST functions
 */
int BuildAsTest(Annabell *annabell, Monitor *Mon)
{
  int PhI;

  annabell->SetMode(ASSOCIATE);
  if (annabell->flags->StartContextFlag) {
	  annabell->flags->StartContextFlag = false;
    ExecuteAct(annabell, Mon, NULL_ACT, SET_START_PH, NULL_ACT);
  }
  else ExecuteAct(annabell, Mon, NULL_ACT, MEM_PH, NULL_ACT);

  int SkipW = 0;
  annabell->SetAct(NULL_ACT, W_FROM_WK);
  Mon->Print();
  annabell->Update();
  while (SkipW<PhSize && annabell->InPhB->RowDefault->Nr[SkipW]->O<0.5) {
    for (PhI=-1; PhI<SkipW; PhI++) {
    	annabell->SetAct(NEXT_AS_W, NULL_ACT);
      Mon->Print();
      //annabell->ActUpdate(); //just to save time in place of Update
      annabell->Update();
    }
    ExecuteAct(annabell, Mon, STORE_ST_A, NULL_ACT, FLUSH_WG);
    for (int WGI=0; WGI<3 && PhI<PhSize
	   && annabell->InPhB->RowDefault->Nr[PhI]->O<0.5; WGI++) {
    	annabell->SetAct(BUILD_AS, NULL_ACT);
      Mon->Print();
      annabell->Update();

      annabell->SetAct(NEXT_AS_W, NULL_ACT);
      Mon->Print();
      annabell->Update();
      PhI++;
    }
    SkipW++;
    annabell->SetAct(NULL_ACT, W_FROM_WK);
    Mon->Print();
    annabell->Update();
  }
  // used to clear PrevWkPh, PrevWG, PrevInEqWGWx, PrevWkEqWGWx, ....
  ExecuteAct(annabell, Mon, STORE_ST_A, NULL_ACT, FLUSH_WG);
  ExecuteAct(annabell, Mon, STORE_ST_A, NULL_ACT, FLUSH_WG);

  annabell->SetMode(NULL_MODE);

  return 0;
}

int GetInputPhraseTest(Annabell *annabell, Monitor *Mon, string input_phrase)
{
  int vin[WSize];

  //cout << input_phrase << endl;
  annabell->SetMode(ACQUIRE);

  for (int ic=0; ic<WSize; ic++) vin[ic]=0;
  annabell->W->Fill(vin);
  ExecuteAct(annabell, Mon, STORE_ST_A, NULL_ACT, FLUSH_OUT);

  annabell->SetAct(ACQ_W, NULL_ACT);

  string buf; // Have a buffer string
  stringstream ss(input_phrase); // Insert the string into a stream
  int i=0;
  while (ss >> buf && i<PhSize) {
    if (buf=="an") buf="a";
    if (buf=="-es") buf="-s";
    interface::w2bin((char*)buf.c_str(),vin);
    Mon->Print();
    annabell->W->Fill(vin);
    annabell->Update();
    Mon->MapW((char*)buf.c_str()); //swap with previous line!!!!

    i++;
  }
  for (int ic=0; ic<WSize; ic++) vin[ic]=0;
  annabell->W->Fill(vin);

  annabell->SetAct(NULL_ACT, W_FROM_WK);
  Mon->Print();
  annabell->Update();

  // used to clear PrevWkPh, PrevWG, PrevInEqWGWx, PrevWkEqWGWx, ....
  ExecuteAct(annabell, Mon, STORE_ST_A, NULL_ACT, FLUSH_WG);
  ExecuteAct(annabell, Mon, STORE_ST_A, NULL_ACT, FLUSH_WG);

  annabell->flags->ExplorationPhaseIdx=0;
  annabell->EPhaseI->Clear();

  annabell->SetMode(NULL_MODE);

  return 0;
}

int TargetExplorationTest(Annabell *annabell, Monitor *Mon, string name, string target_phrase) {

	const int TRYLIMIT = 10000;

  //cout << "\nExploration\n";
  annabell->SetMode(EXPLORE);

  ExecuteAct(annabell, Mon, NULL_ACT, NULL_ACT, NULL_ACT);
  if (name == "WGB" && (annabell->flags->ExplorationPhaseIdx == 3 || annabell->flags->ExplorationPhaseIdx == 4)) {
	  annabell->flags->ExplorationPhaseIdx = 1;
  }
  int StartExplorationPhaseIdx = annabell->flags->ExplorationPhaseIdx;
  if (annabell->flags->ExplorationPhaseIdx == 0) {
	  StartExplorationPhaseIdx = 1;
  }
  int i = 0;
  while (i < TRYLIMIT) {
    if (annabell->flags->ExplorationPhaseIdx == 0) {
      ExecuteAct(annabell, Mon, START_ST_A, NULL_ACT, NULL_ACT);
      ExecuteAct(annabell, Mon, STORE_ST_A, NULL_ACT, W_FROM_IN);
      ExplorationApprove(annabell, Mon);
      annabell->flags->ExplorationPhaseIdx = 1;
    }
    if (annabell->flags->ExplorationPhaseIdx==1) {
      int N1, N2;
      N1=1+rnd_int()%10;
      N2=1+rnd_int()%6;
      ExecuteAct(annabell, Mon, STORE_ST_A, NULL_ACT, W_FROM_WK);
      for (int i=0; i<N1; i++) {
	ExecuteAct(annabell, Mon, STORE_ST_A, NULL_ACT, NEXT_W);
      }
      ExecuteAct(annabell, Mon, STORE_ST_A, NULL_ACT, FLUSH_WG);
      for (int i=0; i<N2; i++) {
	ExecuteAct(annabell, Mon, STORE_ST_A, NULL_ACT, GET_W);
	ExecuteAct(annabell, Mon, STORE_ST_A, NULL_ACT, NEXT_W);
	Mon->GetWM("CW", annabell->CW);
	if (Mon->OutStr[0]=="") break;
      }
      if (name=="WGB") {
	ExecuteAct(annabell, Mon, NULL_ACT, NULL_ACT, NULL_ACT);
	Mon->GetPhM(name, annabell->WGB);
	if (target_phrase==Mon->OutPhrase) break;
	//else cout << "exp WBG: " << Mon->OutPhrase << endl;
      }
      else { // (name=="WkPhB")
    annabell->flags->ExplorationPhaseIdx = 2;
	continue;
      }
    }
    else if (annabell->flags->ExplorationPhaseIdx == 2) {
      ExecuteAct(annabell, Mon, STORE_ST_A, NULL_ACT, RETR_AS);
      if (name=="WGB") {
    	  annabell->flags->ExplorationPhaseIdx = 1;
	continue;
      }
      else { // (name=="WkPhB")
	Mon->GetPhM(name, annabell->WkPhB);
	if (target_phrase==Mon->OutPhrase) break;
      }
    }
    else if (annabell->flags->ExplorationPhaseIdx == 3) {
      ExecuteAct(annabell, Mon, STORE_ST_A, NULL_ACT, GET_START_PH);
      Mon->GetPhM(name, annabell->WkPhB);
      if (target_phrase==Mon->OutPhrase) break;
    }
    else if (annabell->flags->ExplorationPhaseIdx == 4) {
      ExecuteAct(annabell, Mon, STORE_ST_A, NULL_ACT, GET_NEXT_PH);
      Mon->GetPhM(name, annabell->WkPhB);
      if (target_phrase==Mon->OutPhrase) break;
    }
    annabell->flags->ExplorationPhaseIdx = StartExplorationPhaseIdx;
    if (annabell->flags->ExplorationPhaseIdx == 3) {
    	StartExplorationPhaseIdx = 4;
    }
    if (annabell->flags->ExplorationPhaseIdx == 4) {
    	StartExplorationPhaseIdx = 1;
    }
    ExecuteAct(annabell, Mon, RETR_SAI, NULL_ACT, NULL_ACT);
    ExecuteAct(annabell, Mon, RETR_ST, NULL_ACT, NULL_ACT);
    ExecuteAct(annabell, Mon, RETR_SAI, NULL_ACT, NULL_ACT);
    i++;
  }

  if (i >= TRYLIMIT) {
      throw ann_exception("Too many attempts\n");
  }

  cout << "Good answer after " << i << " iterations\n";
  ExplorationApprove(annabell, Mon);

  annabell->SetMode(NULL_MODE);

  return 0;
}

string RewardTest(Annabell *annabell, Monitor *Mon, int partial_flag,
		  int n_iter)
{
  annabell->SetMode(REWARD);

  annabell->SetAct(STORE_ST_A, NULL_ACT, FLUSH_OUT);
  annabell->StActRwdUpdate();
  Mon->Print();
  annabell->Update();

  annabell->SetAct(STORE_ST_A, NULL_ACT, WG_OUT);
  annabell->StActRwdUpdate();
  Mon->Print();
  annabell->Update();
  Mon->GetPhM("OutPhB", annabell->OutPhB);
  string out_phrase = Mon->OutPhrase;

  if (partial_flag!=0) annabell->SetAct(STORE_ST_A, NULL_ACT, CONTINUE);
  else annabell->SetAct(STORE_ST_A, NULL_ACT, DONE);
  annabell->StActRwdUpdate();
  Mon->Print();
  annabell->Update();

  annabell->SetAct(STORE_ST_A, NULL_ACT, NULL_ACT);
  annabell->StActRwdUpdate();
  Mon->Print();
  annabell->Update();
  //Mon->Print();
  int LastStActI = GetStActIdx(annabell, Mon); //annabell->StActI
  for (int i=0; i<n_iter; i++) {
    //Mon->ModeMessage("Start State-Action Index\n");
	  annabell->SetAct(START_ST_A, NULL_ACT, NULL_ACT);
    annabell->StActRwdUpdate();
    Mon->Print();
    annabell->Update();
    for (int StActI=0; StActI<LastStActI-1; StActI++) {
      //Mon->ModeMessage("Retrieve and reward State-Action\n");
    	annabell->SetAct(RWD_ST_A, NULL_ACT, NULL_ACT);
      annabell->StActRwdUpdate();
      Mon->Print();
      annabell->Update();
    }
  }

  annabell->SetMode(NULL_MODE);

  return out_phrase;
}

int GetStActIdx(Annabell *annabell, Monitor *Mon)
{
  int vi[StActSize];

  for (int ib=0; ib<StActSize; ib++) {
    vi[ib]=annabell->StActI->Nr[ib]->O;
  }
  int is=interface::v2i(vi, StActSize);

  return is;
}


string ExploitationTest(Annabell *annabell, Monitor *Mon, display* Display, int n_iter)
{
  annabell->SetMode(EXPLOIT);
  //cout << "\nExploitation\n";

  int iac, Nac=300, Nas=100; // it should be Nas=MaxNRetr in sizes.h
  int prev_act, next_act;
  string out_phrase="", best_phrase="";
  int BestDB=1000;

  ExecuteAct(annabell, Mon, START_ST_A, NULL_ACT, NULL_ACT);
  ExecuteAct(annabell, Mon, STORE_ST_A, NULL_ACT, NULL_ACT);
  for (int IterI=0; IterI<n_iter; IterI++) {
    ExecuteAct(annabell, Mon, START_ST_A, NULL_ACT, NULL_ACT);
    if (n_iter>1) {
      Display->Print("Trying n. " + toStr(IterI));
    }
    ExecuteAct(annabell, Mon, RETR_ST_A, NULL_ACT, NULL_ACT);
    for (iac=0; iac<Nac; iac++) {
      prev_act=Mon->GetElActFL(); //Mon->PrintElAct();
      if (prev_act==RETR_AS) {
	int best_DB = 1000, best_act = NULL_ACT;
	ExecuteAct(annabell, Mon, STORE_ST_A, NULL_ACT, NULL_ACT);
	for (int i=0; i<Nas; i++) {
	  ExecuteAct(annabell, Mon, NULL_ACT, NULL_ACT, RETR_AS);
	  ExecuteAct(annabell, Mon, RETR_EL_A, NULL_ACT, NULL_ACT);
	  next_act=Mon->GetElActFL(); //Mon->PrintElAct();
	  //int n_wnn = annabell->ElActfSt->NumWnn();
	  //cout << "next act " << next_act << endl;
	  //cout << "NumWnn: " << n_wnn << endl;
	  //cout << "DB: " << annabell->ElActfSt->DB << endl;
	  int DB = annabell->ElActfSt->DB;
	  if (next_act!=RETR_AS && next_act!=NULL_ACT
	      && annabell->ElActfSt->NumWnn()>=5 && DB<best_DB) {
	    best_DB = DB;
	    best_act = next_act;
	    ExecuteAct(annabell, Mon, CHANGE_ST_A, NULL_ACT, NULL_ACT);
	    //cout << "best DB updated to: " << best_DB << endl;
	  }
	  else {
	    ExecuteAct(annabell, Mon, NULL_ACT, NULL_ACT, NULL_ACT);
	  }
	}
	next_act = best_act;
	if (next_act==NULL_ACT) {
	  Display->Warning("next act not found with Nas=" + toStr(Nas) +
			  ". Exiting\n");
	  break;
	}
	ExecuteAct(annabell, Mon, GET_ST_A, NULL_ACT, NULL_ACT);
	//LastGB = best_DB;
      }
      else {
	ExecuteAct(annabell, Mon, RETR_EL_A, NULL_ACT, NULL_ACT);
	next_act=Mon->GetElActFL(); // Mon->PrintElAct();

	if (next_act==NULL_ACT || annabell->ElActfSt->NumWnn()<5) {
	  Display->Warning("next act not found with GB="
			  + toStr(annabell->ElActfSt->DB) + ". Exiting\n");
	  break;
	}
      }

      if (next_act==SNT_OUT) {//annabell->ElAct->Nr[SNT_OUT-1]->O>0.5) {
	int DB = annabell->ElActfSt->DB;
	if (out_phrase!="") out_phrase = out_phrase + " ";
	out_phrase = out_phrase + SentenceOut(annabell, Mon, Display);
	if (n_iter>1) cout << "DB: " << DB << endl;
	else Display->Print(".\n");
	if (DB<BestDB) {
	  BestDB = DB;
	  best_phrase = out_phrase;
	}
	out_phrase="";
	break;
      }
      ExecuteAct(annabell, Mon, STORE_ST_A, NULL_ACT, next_act);
      if (annabell->OutFlag->Nr[0]->O>0.5) {
	Mon->GetPhM("OutPhB", annabell->OutPhB);
	Display->Print(" -> " + Mon->OutPhrase+"\n");
	if (out_phrase!="") out_phrase = out_phrase + " ";
	out_phrase = out_phrase + Mon->OutPhrase;
      }
      //if (annabell->OutFlag->Nr[0]->O>0.5) {
      if (annabell->ElAct->Nr[CONTINUE-1]->O>0.5) {
	Display->Print(" ... ");
      }
      if (annabell->ElAct->Nr[DONE-1]->O>0.5) {
	int DB = annabell->ElActfSt->DB;
	if (n_iter>1) cout << "DB: " << DB << endl;
	else Display->Print(".\n");
	if (DB<BestDB) {
	  BestDB = DB;
	  best_phrase = out_phrase;
	}
	out_phrase="";
	break;
      }

    }
    if (iac==Nac) Display->Warning("Exploitation number of actions >= " +
				  toStr(Nac) + ". Exiting\n");
  }
  if (annabell->flags->SpeakerFlag) Display->Print("*SYS:\t");
  Display->Print(best_phrase);
  if (annabell->flags->PeriodFlag) Display->Print(" . \n");
  else Display->Print("\n");

  annabell->SetMode(NULL_MODE);

  return best_phrase;
}

int MoreRetrAsSlow(Annabell *annabell, Monitor *Mon)
{
  int next_act, Nas=20;
  for (int i=0; i<Nas; i++) {
    //cout << "MoreRetrAs " << i << endl;
	  annabell->SetAct(NULL_ACT, RETR_AS);
    annabell->Update();
    //Mon->Print();
    ExecuteAct(annabell, Mon, RETR_EL_A, NULL_ACT, NULL_ACT);
    next_act=Mon->GetElActFL();
    if (next_act!=NULL_ACT) break;
  }

  return next_act;
}

int ExploitationSlow(Annabell *annabell, Monitor *Mon, display* Display)
{
  annabell->SetMode(EXPLOIT);
  //cout << "\nExploitation\n";

  int iac, Nac=300, Nb=60;
  int prev_act, next_act;

  ExecuteAct(annabell, Mon, START_ST_A, NULL_ACT, NULL_ACT);
  for (iac=0; iac<Nac; iac++) {
    prev_act=Mon->GetElActFL();
    for (int ib=0; ib<Nb; ib++) {
      ExecuteAct(annabell, Mon, RETR_EL_A, NULL_ACT, NULL_ACT);
      next_act=Mon->GetElActFL();
      if (next_act==NULL_ACT && prev_act==RETR_AS)
	next_act = MoreRetrAsSlow(annabell,Mon);

      if (annabell->ElActfSt->NumWnn()>=5
	  && next_act!=NULL_ACT
	  && (prev_act!=RETR_AS || next_act!=RETR_AS)) break;
      annabell->ElActfSt->GB++;
      //cout << "next act not found. Trying with GB=" << annabell->ElActfSt->GB
      //	   << endl;
    }
    LastGB = annabell->ElActfSt->GB;
    annabell->ElActfSt->GB=0;
    if (next_act==NULL_ACT || annabell->ElActfSt->NumWnn()<5) {
      Display->Warning("next act not found with GB=" + toStr(LastGB) +
		      ". Exiting\n");
      break;
    }

    ExecuteAct(annabell, Mon, STORE_ST_A, NULL_ACT, NULL_ACT);
    if (annabell->OutFlag->Nr[0]->O>0.5) {
      Mon->GetPhM("OutPhB", annabell->OutPhB);
      Display->Print(Mon->OutPhrase+"\n");
    }
    //if (annabell->OutFlag->Nr[0]->O>0.5) {
    if (annabell->ElAct->Nr[CONTINUE-1]->O>0.5) {
      Display->Print(" ... ");
    }
    if (annabell->ElAct->Nr[DONE-1]->O>0.5) {
      Display->Print(".\n");
      //Display->Print("Done. Exiting\n");
      //Mon->Print();
      //Mon->GetPhM("OutPhB", annabell->OutPhB);
      //Display->Print(Mon->OutPhrase+"\n");
      break;
    }
  }
  if (iac==Nac) Display->Warning("Exploitation number of actions >= " +
				toStr(Nac) + ". Exiting\n");
  annabell->SetMode(NULL_MODE);

  return 0;
}


int MoreRetrAs(Annabell *annabell, Monitor *Mon)
{
  int next_act, Nas=20;
  for (int i=0; i<Nas; i++) {
    //cout << "MoreRetrAs " << i << endl;
	  annabell->SetAct(NULL_ACT, RETR_AS);
    annabell->Update();
    //Mon->Print();
    ExecuteAct(annabell, Mon, RETR_EL_A, NULL_ACT, NULL_ACT);
    next_act=Mon->GetElActFL();
    if (next_act!=NULL_ACT) break;
  }

  return next_act;
}

int ExplorationRetry(Annabell *annabell, Monitor *Mon)
{
  //cout << "\nExplorationRetry (RetrieveStActIdx)\n";

  if (annabell->flags->ExplorationPhaseIdx == 3) {
	  annabell->flags->ExplorationPhaseIdx = 4;
  }
  else if (annabell->flags->ExplorationPhaseIdx == 4) {
	  annabell->flags->ExplorationPhaseIdx = 1;
  }

  //cout << "StoredStActI: " << annabell->StoredStActI->HighVect[0] << endl;
  //cout << "StActI before: " << annabell->StActI->HighVect[0] << endl;
  //Mon->Print();
  annabell->SetAct(RETR_SAI, NULL_ACT, NULL_ACT);
  annabell->StActRwdUpdate();
  //cout << "StActI after: " << annabell->StActI->HighVect[0] << endl;
  annabell->SetAct(RETR_ST_A, NULL_ACT, NULL_ACT);
  annabell->StActRwdUpdate();
  annabell->SetAct(RETR_SAI, NULL_ACT, NULL_ACT);
  annabell->StActRwdUpdate();
  //Mon->Print();
  annabell->RetrStActIFlag->Nr[0]->O=0;
  annabell->CurrStActIFlag->Nr[0]->O=1;

  return 0;
}


//////////////////////////////////////////////////////////////////////////////
// check if output phrase is a sensorymotor command
// or if it should be sent to a YARP port
//////////////////////////////////////////////////////////////////////////////
int CheckSensoryMotor(string out_phrase, Annabell *annabell, display* Display)
{
  //cout << "CheckSensoryMotor:" << out_phrase << endl;
  vector<string> phrase_token;

  stringstream ss(out_phrase); // Insert the phrase into a stream
  string buf; // buffer string

  while (ss >> buf) { // split line in string tokens
    phrase_token.push_back(buf);
  }
  if (phrase_token.size()==0) return 0;
  buf=phrase_token[0];
  int l = buf.size() - 1;
  if (buf[0]!='[' || buf[l] != ']') { // phrase is not a sensorymotor command
    if (annabell->flags->YarpOutputFlag) { // send output to a YARP port
      YarpOutput(out_phrase, Display);
    }
    return 0;
  }

  stringstream in_ss("");

  SensoryMotor(phrase_token, in_ss, Display); // execute sensorymotor command

  bool ax_tmp=annabell->flags->AutoExploitFlag;
  annabell->flags->AutoExploitFlag=false;
  // parse and process sensorymotor response
  while(getline (in_ss, buf))  {
    //Display->Print(buf+"\n");
    Command* c = CommandFactory::newCommand(buf);
    int	commandResult = c->execute();
    delete c;
    if(commandResult == 2) break;
  }
  annabell->flags->AutoExploitFlag=ax_tmp;
  
  return 0;
}

