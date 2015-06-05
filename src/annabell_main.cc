/*
Copyright (C) 2015 Bruno Golosio

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <set>
#include <stdio.h>
#include <stdlib.h>
#include "sllm.h"
#include "sizes.h"
#include "interface.h"
#include "monitor.h"
#include "rnd.h"
#include "display.h" 
#include <sys/time.h>

using namespace std;
using namespace sizes;

const int TRYLIMIT=10000;
int LastGB;
int ExplorationPhaseIdx;
int StoredStActI;
display Display;
bool VerboseFlag=false;
bool StartContextFlag=true;
bool PeriodFlag=false;
bool SpeakerFlag=false;
bool AnswerTimeFlag=false;
bool AnswerTimeUpdate=false;
std::string AnswerTimePhrase="? do you have any friend -s";
std::string SpeakerName="HUM";
bool AutoSaveLinkFlag=false;
int AutoSaveLinkIndex=0;
long AutoSaveLinkStep=2000000;

struct timespec clk0, clk1;

int GetInputPhrase(sllm *SLLM, monitor *Mon, string input_phrase);
int GetInputPhraseTest(sllm *SLLM, monitor *Mon, string input_phrase);
int BuildAs(sllm *SLLM, monitor *Mon);
int BuildAsTest(sllm *SLLM, monitor *Mon);
int ExplorationApprove(sllm *SLLM, monitor *Mon);
int ExplorationRetry(sllm *SLLM, monitor *Mon);
int Reward(sllm *SLLM, monitor *Mon, int partial_flag, int n_iter);
int RewardTest(sllm *SLLM, monitor *Mon, int partial_flag, int n_iter);
string Exploitation(sllm *SLLM, monitor *Mon, int n_iter);
string ExploitationTest(sllm *SLLM, monitor *Mon, int n_iter);
int ExploitationSlow(sllm *SLLM, monitor *Mon);
int TargetExploration(sllm *SLLM, monitor *Mon, std::string name,
		      std::string target_phrase);
int TargetExplorationTest(sllm *SLLM, monitor *Mon, std::string name,
		      std::string target_phrase);
int SearchContext(sllm *SLLM, monitor *Mon, std::string target_phrase);
int ContinueSearchContext(sllm *SLLM, monitor *Mon, std::string target_phrase);
int WorkingPhraseOut(sllm *SLLM, monitor *Mon);
string SentenceOut(sllm *SLLM, monitor *Mon);
int Interface(sllm *SLLM, monitor *Mon);
int Reset(sllm *SLLM, monitor *Mon);

template <typename T>
std::string to_string(T const& value) {
    stringstream sstr;
    sstr << value;
    return sstr.str();
}

int CheckTryLimit(int i)
{
  if (i>=TRYLIMIT) {
    cout << "Too many attempts\n";
    exit(0);
  }

  return 0;
}

int SetAct(sllm *SLLM, int acq_act, int el_act)
{
  SLLM->AcqAct->Fill((int*)v_acq_act[acq_act]);
  SLLM->ElActFL->Fill((int*)v_el_act[el_act]);
  SLLM->ElAct->Fill((int*)v_el_act[el_act]);

  return 0;
}

int SetAct(sllm *SLLM, int rwd_act, int acq_act, int el_act)
{
  SLLM->RwdAct->Fill((int*)v_rwd_act[rwd_act]);
  SLLM->AcqAct->Fill((int*)v_acq_act[acq_act]);
  SLLM->ElActFL->Fill((int*)v_el_act[el_act]);
  SLLM->ElAct->Fill((int*)v_el_act[el_act]);

  return 0;
}

int SetMode(sllm *SLLM, int imode)
{
  SLLM->ModeFlags->Fill((int*)v_mode[imode]);

  return 0;
}

int ExecuteAct(sllm *SLLM, monitor *Mon, int rwd_act, int acq_act, int el_act)
{
  SetAct(SLLM, rwd_act, acq_act, el_act);
  Mon->Print();
  if (VerboseFlag) Mon->PrintRwdAct();
  SLLM->StActRwdUpdate();
  if (VerboseFlag) {Mon->PrintElActFL(); Mon->PrintElAct();}
  SLLM->Update();

  return 0;
}

bool simplify(sllm *SLLM, monitor *Mon,
	      std::vector<std::string> input_token);

int ParseCommand(sllm *SLLM, monitor *Mon, std::string input_line);

int main()
{
  Display.LogFileFlag=false;
  Display.ConsoleFlag=true;

  init_randmt(12345);

  sllm *SLLM = new sllm();
  monitor *Mon = new monitor(SLLM);
  delete Display.LogFile;
  Display.LogFile = Mon->Display.LogFile;

  SetMode(SLLM, NULL_MODE);

  Interface(SLLM, Mon);

  return 0;
}

int Interface(sllm *SLLM, monitor *Mon)
{
  std::string input_line;
  clock_gettime( CLOCK_REALTIME, &clk0);

  for(;;) {
    std::cout << "Enter command: ";
    std::getline (std::cin, input_line);
    //Display.Print(input_line+"\n");
    if (ParseCommand(SLLM, Mon, input_line)==2) break;
  }
  if (Display.LogFile->is_open()) Display.LogFile->close();

  return 0;
}


//////////////////////////////////////////////////////////////////////
// Read command or input phrase from command line
//////////////////////////////////////////////////////////////////////
int ParseCommand(sllm *SLLM, monitor *Mon, std::string input_line)
{
  std::vector<std::string> input_token;

  std::stringstream ss(input_line); // Insert the line into a stream
  std::string buf, buf1; // buffer strings

  buf = "";
  while (ss >> buf1) { // split line in string tokens
    if (buf1=="an") buf1="a"; // take care of article
    if (buf1=="-es") buf1="-s"; // take care of plural suffix

    buf = buf + buf1;
    if (buf[0]=='/' || buf[0]=='<') {
      int l=buf.size()-1;
      if ((buf[0]=='/' && buf[l]!='/') ||
	  (buf[0]=='<' && buf[l]!='>')) {
	buf = buf + " ";
	continue;
      }
    }
    input_token.push_back(buf);
    buf="";
  }
  if (simplify(SLLM, Mon, input_token)) return 0;

  std::string target_phrase;
  if (input_token.size()==0) {
    Display.Print(input_line+"\n");
    target_phrase = ".end_context";
    GetInputPhrase(SLLM, Mon, target_phrase);
    ExecuteAct(SLLM, Mon, NULL_ACT, MEM_PH, NULL_ACT);
    Display.Print(" >>> End context\n");
    StartContextFlag=true;

    // answer time
    if (AnswerTimeFlag && AnswerTimeUpdate && AnswerTimePhrase!="") {
      struct timespec clk0, clk1;
      clock_gettime( CLOCK_REALTIME, &clk0);

      AnswerTimeUpdate=false;
      ExecuteAct(SLLM, Mon, STORE_ST_A, NULL_ACT, FLUSH_OUT);
      GetInputPhrase(SLLM, Mon, AnswerTimePhrase);
      Exploitation(SLLM, Mon, 1);

      clock_gettime( CLOCK_REALTIME, &clk1);
      double answ_time = clk1.tv_sec - clk0.tv_sec
	+ (double)(clk1.tv_nsec - clk0.tv_nsec)*1e-9;
      double link_num = (double)SLLM->ElActfSt->CountSparseInputLinks();
      FILE *at_fp=fopen("answer_time.dat", "a");
      fprintf(at_fp, "%.3e\t%.3f\n", link_num, answ_time);
      fclose(at_fp);
    }
    // auto save links
    if (AutoSaveLinkFlag) {
      double link_num = (double)SLLM->ElActfSt->CountSparseInputLinks();
      int index = (int)(link_num/AutoSaveLinkStep);
      if (index>AutoSaveLinkIndex) {
	AutoSaveLinkIndex = index;
	char filename[20];
	sprintf(filename, "links_%d.dat", AutoSaveLinkIndex);
	FILE *fp=fopen(filename, "wb");
	Mon->SaveWM(fp);
	if (SLLM->MemPh->HighVect.size()!=1) {
	  Display.Warning("Error on MemPh.");
	  return 1;
	}
	fwrite(&SLLM->MemPh->HighVect[0], sizeof(int), 1, fp);
	if (SLLM->StartPh->HighVect.size()!=1) {
	  Display.Warning("Error on StartPh.");
	  return 1;
	}
	fwrite(&SLLM->StartPh->HighVect[0], sizeof(int), 1, fp);

	SLLM->IW->SaveNr(fp);
	SLLM->IW->SaveInputLinks(fp);
	SLLM->ElActfSt->SaveNr(fp);
	SLLM->ElActfSt->SaveSparseInputLinks(fp);
	SLLM->ElActfSt->SaveOutputLinks(fp);
	SLLM->RemPh->SaveSparseOutputLinks(fp);
	SLLM->RemPhfWG->SaveNr(fp);
	SLLM->RemPhfWG->SaveSparseInputLinks(fp);
	SLLM->RemPhfWG->SaveSparseOutputLinks(fp);

	fclose(fp);
      }
    }
    // added 5/01/2013
    SetAct(SLLM, START_ST_A, NULL_ACT, NULL_ACT);
    SLLM->StActRwdUpdate();
    Mon->Print();
    SLLM->Update();
    ExecuteAct(SLLM, Mon, STORE_ST_A, NULL_ACT, NULL_ACT);

    return 0; // empty line
  }
  buf = input_token[0];
  ////////////////////////////////////////
  if (buf[0]=='#') { // input line is a comment
    Display.Print(input_line+"\n");
    return 0;
  }
  ////////////////////////////////////////
  if (buf[0]!='.' || (buf[1]=='.' && buf[2]=='.')) {
    // if token does not start with "." or if token is "..."
    // input line is a phrase, not a command
    if (SpeakerFlag) Display.Print("*" + SpeakerName + ":\t"); //("*TEA:\t"); 
    Display.Print(input_line+"\n");
    ExecuteAct(SLLM, Mon, STORE_ST_A, NULL_ACT, FLUSH_OUT);
    GetInputPhrase(SLLM, Mon, input_line);
    BuildAs(SLLM, Mon);
    //BuildAsTest(SLLM, Mon);

    return 0;
  }
  Display.Print(input_line+"\n");
  ////////////////////////////////////////
  // Exits the program or the current input file
  ////////////////////////////////////////
  if (buf==".quit" || buf==".q") return 2; // quit
  ////////////////////////////////////////
  else if (buf==".continue_context" || buf==".cctx") { // continue context
    if (input_token.size()>2) {
      Display.Warning("syntax error.");
      return 1;
    }
    StartContextFlag=false;

    return 0;
  }
  ////////////////////////////////////////
  // Suggests a phrase to be retrieved by the association process.
  ////////////////////////////////////////
 else if (buf==".phrase" || buf==".ph") { // suggest a phrase
    if (input_token.size()<2) {
      Display.Warning("a phrase should be provided as argument.");
      return 1;
    }
    target_phrase = input_token[1];
    for(unsigned int itk=2; itk<input_token.size(); itk++) {
      target_phrase = target_phrase + " " + input_token[itk];
    }
    //VerboseFlag=true;
    //TargetExplorationTest(SLLM, Mon, "WkPhB", target_phrase);
    TargetExploration(SLLM, Mon, "WkPhB", target_phrase);
    //cout << "target_phrase: " << target_phrase << endl;
    //VerboseFlag=false;
    return 0;
  }
  ////////////////////////////////////////
  // Suggests a word group to be extracted from the working phrase.
  ////////////////////////////////////////
  else if (buf==".word_group" || buf==".wg") { // suggest a group of words
    if (input_token.size()<2) {
      //Display.Warning("a word group should be provided as argument.");
      //return 1;
      ExecuteAct(SLLM, Mon, STORE_ST_A, NULL_ACT, FLUSH_WG);
      ExecuteAct(SLLM, Mon, STORE_ST_A, NULL_ACT, W_FROM_WK);
      return 0;
    }
    target_phrase = input_token[1];
    for(unsigned int itk=2; itk<input_token.size(); itk++) {
      target_phrase = target_phrase + " " + input_token[itk];
    }
    //VerboseFlag=true;
    //TargetExplorationTest(SLLM, Mon, "WGB", target_phrase);
    TargetExploration(SLLM, Mon, "WGB", target_phrase);
    //cout << "target_phrase: " << target_phrase << endl;
    //VerboseFlag=false;

    return 0;
  }
  ////////////////////////////////////////
  // Searches a phrase in the current context of the working phrase
  // starting from the beginning of the context
  ////////////////////////////////////////
  else if (buf==".search_context" || buf==".sctx") { //search phrase in context 
    if (input_token.size()<2) {
      Display.Warning("a phrase should be provided as argument.");
      return 1;
    }
    target_phrase = input_token[1];
    for(unsigned int itk=2; itk<input_token.size(); itk++) {
      target_phrase = target_phrase + " " + input_token[itk];
    }
    //VerboseFlag=true;
    SearchContext(SLLM, Mon, target_phrase);
    //cout << "target_phrase: " << target_phrase << endl;
    //VerboseFlag=false;

    return 0;
  }
  ////////////////////////////////////////
  // Searches a phrase in the current context of the working phrase
  // starting from the phrase next to the working phrase
  ////////////////////////////////////////
  else if (buf==".continue_search_context" || buf==".csctx") {
    //search phrase in context 
    if (input_token.size()<2) {
      Display.Warning("a phrase should be provided as argument.");
      return 1;
    }
    target_phrase = input_token[1];
    for(unsigned int itk=2; itk<input_token.size(); itk++) {
      target_phrase = target_phrase + " " + input_token[itk];
    }
    //VerboseFlag=true;
    ContinueSearchContext(SLLM, Mon, target_phrase);
    //cout << "target_phrase: " << target_phrase << endl;
    //VerboseFlag=false;

    return 0;
  }
  ////////////////////////////////////////
  // Copies the input phrase to the working phrase.
  ////////////////////////////////////////
  else if (buf==".retrieve_input_phrase" || buf==".rip") { // back to input
    if (input_token.size()>1) {
      Display.Warning("syntax error.");
      return 1;
    }
    ExecuteAct(SLLM, Mon, STORE_ST_A, NULL_ACT, W_FROM_IN);
    //ExecuteAct(SLLM, Mon, STORE_ST_A, MEM_PH, NULL_ACT); // dummy
    ExplorationApprove(SLLM, Mon);
    //Mon->PrintPhM("WkPhB", SLLM->WkPhB);
    //Mon->PrintSSMidx("RemPh", SLLM->RemPh);
    //Mon->PrintSSMidx("MemPh", SLLM->MemPh);
    ExplorationPhaseIdx=1;
    //SLLM->EPhaseI->Clear();

    return 0;
  }
  ////////////////////////////////////////
  // Sends the word group to output and produces a (conclusive) reward
  // for the past state-action sequence.
  ////////////////////////////////////////
  else if (buf==".reward" || buf==".rw") { // reward
    if (input_token.size()>2) {
      Display.Warning("syntax error.");
      return 1;
    }
    int n_iter;
    if (input_token.size()==1) n_iter=SLLM->ElActfSt->K;
    else {
      std::stringstream ss1(input_token[1]);
      ss1 >> n_iter;
      if (!ss1) {
	Display.Warning("Cannot convert token to integer.");
      }
    }
    //int ex_ph = ExplorationPhaseIdx;
    Reward(SLLM, Mon, 0, n_iter);
    //ExplorationPhaseIdx=ex_ph;
    //ExplorationPhaseIdx=0;
    //SLLM->EPhaseI->Clear();

    // added 5/01/2013
    SetAct(SLLM, START_ST_A, NULL_ACT, NULL_ACT);
    SLLM->StActRwdUpdate();
    Mon->Print();
    SLLM->Update();
    ExecuteAct(SLLM, Mon, STORE_ST_A, NULL_ACT, NULL_ACT);
    ExplorationApprove(SLLM, Mon);
    ExplorationPhaseIdx=0;
    AnswerTimeUpdate=true;

    return 0;
  }
  ////////////////////////////////////////
  // Sends the word group to output and produces a partial reward
  // for the past state-action sequence.
  ////////////////////////////////////////
  else if (buf==".partial_reward" || buf==".prw") { // partial reward
    if (input_token.size()>2) {
      Display.Warning("syntax error.");
      return 1;
    }
    int n_iter;
    if (input_token.size()==1) n_iter=SLLM->ElActfSt->K;
    else {
      std::stringstream ss1(input_token[1]);
      ss1 >> n_iter;
      if (!ss1) {
	Display.Warning("Cannot convert token to integer.");
      }
    }
    //int ex_ph = ExplorationPhaseIdx;
    Reward(SLLM, Mon, 1, n_iter);
    //ExplorationPhaseIdx=ex_ph;
    //ExplorationPhaseIdx=0;
    //SLLM->EPhaseI->Clear();

    // added 5/01/2013
    SetAct(SLLM, START_ST_A, NULL_ACT, NULL_ACT);
    SLLM->StActRwdUpdate();
    Mon->Print();
    SLLM->Update();
    ExecuteAct(SLLM, Mon, STORE_ST_A, NULL_ACT, NULL_ACT);
    ExplorationApprove(SLLM, Mon);
    ExplorationPhaseIdx=1; //ex_ph;

    return 0;
  }
  ////////////////////////////////////////
  // Starts an exploitation phase. At the end of this phase, the system
  // is expected to respond to the input phrase with an appropriate output
  // phrase. Without argument, the exploitation is related to the previous
  // input phrase.
  ////////////////////////////////////////
  else if (buf==".exploit" || buf==".x") { // exploitation
    if (input_token.size()>1) {
      target_phrase = input_token[1];
      for(unsigned int itk=2; itk<input_token.size(); itk++) {
	target_phrase = target_phrase + " " + input_token[itk];
      }
      if (AnswerTimePhrase=="")  AnswerTimePhrase = target_phrase;
      ExecuteAct(SLLM, Mon, STORE_ST_A, NULL_ACT, FLUSH_OUT);
      GetInputPhrase(SLLM, Mon, target_phrase);
    }
    //VerboseFlag = true;
    //ExploitationTest(SLLM, Mon, 1);
    Exploitation(SLLM, Mon, 1);
    //VerboseFlag = false;

    return 0;
  }
  ////////////////////////////////////////
  // Clear the goal stack and starts an exploitation phase. At the end of
  // this phase, the system is expected to respond to the input phrase with
  // an appropriate output phrase. Without argument, the exploitation is
  // related to the previous input phrase.
  ////////////////////////////////////////
  else if (buf==".clean_exploit" || buf==".cx") { // clean exploitation
    for (int i=0; i<5; i++)
      ExecuteAct(SLLM, Mon, STORE_ST_A, NULL_ACT, DROP_GL);

    if (input_token.size()>1) {
      target_phrase = input_token[1];
      for(unsigned int itk=2; itk<input_token.size(); itk++) {
	target_phrase = target_phrase + " " + input_token[itk];
      }
      if (AnswerTimePhrase=="")  AnswerTimePhrase = target_phrase;
      ExecuteAct(SLLM, Mon, STORE_ST_A, NULL_ACT, FLUSH_OUT);
      GetInputPhrase(SLLM, Mon, target_phrase);
    }
    //VerboseFlag = true;
    //ExploitationTest(SLLM, Mon, 1);
    Exploitation(SLLM, Mon, 1);
    //VerboseFlag = false;
    
    return 0;
  }
  ////////////////////////////////////////
  // Starts an exploitation phase using a random ordering for the WTA rule.
  // At the end of this phase, the system is expected to respond to the input
  // phrase with an appropriate output phrase. Without argument, the
  // exploitation is related to the previous input phrase.
  ////////////////////////////////////////
  else if (buf==".exploit_random" || buf==".xr") { // random exploitation
    if (input_token.size()>1) {
      target_phrase = input_token[1];
      for(unsigned int itk=2; itk<input_token.size(); itk++) {
	target_phrase = target_phrase + " " + input_token[itk];
      }
      ExecuteAct(SLLM, Mon, STORE_ST_A, NULL_ACT, FLUSH_OUT);
      GetInputPhrase(SLLM, Mon, target_phrase);
    }
    SLLM->RemPhfWG->OrderedWnnFlag = false;
    //VerboseFlag = true;
    //ExploitationTest(SLLM, Mon, 1);
    Exploitation(SLLM, Mon, 1);
    //VerboseFlag = false;
    SLLM->RemPhfWG->OrderedWnnFlag = true;

    return 0;
  }
  ////////////////////////////////////////
  // Starts an exploitation phase.
  // The output phrase is memorized by the system.
  ////////////////////////////////////////
  else if (buf==".exploit_memorize" || buf==".xm") { //exploitation-memorization
    if (input_token.size()>1) {
      target_phrase = input_token[1];
      for(unsigned int itk=2; itk<input_token.size(); itk++) {
	target_phrase = target_phrase + " " + input_token[itk];
      }
      ExecuteAct(SLLM, Mon, STORE_ST_A, NULL_ACT, FLUSH_OUT);
      GetInputPhrase(SLLM, Mon, target_phrase);
    }
    //VerboseFlag = true;
    //ExploitationTest(SLLM, Mon, 1);
    string best_phrase = Exploitation(SLLM, Mon, 1);
    //VerboseFlag = false;
    ExecuteAct(SLLM, Mon, STORE_ST_A, NULL_ACT, FLUSH_OUT);
    GetInputPhrase(SLLM, Mon, best_phrase);
    BuildAs(SLLM, Mon);

    return 0;
  }

 ////////////////////////////////////////
 // Analogous to .x, but it iterates the exploitation process n_iter times
 // and selects the best output phrase.
 ////////////////////////////////////////
  else if (buf==".best_exploit"|| buf== ".bx") { // best exploitation
    if (input_token.size()<3) {
      Display.Warning("n. of iterations and a phrase should be provided");
      Display.Warning("as argument in the current version.");
      return 1;
    }
    int n_iter;
    if (input_token.size()==1) n_iter=20; // will never be exec in curr. vers.
    else {
      std::stringstream ss1(input_token[1]);
      ss1 >> n_iter;
      if (!ss1) {
	Display.Warning("Cannot convert token to integer.");
      }
    }
    if (input_token.size()>2) { // would not be necessary in curr. vers.
      target_phrase = input_token[2];
      for(unsigned int itk=3; itk<input_token.size(); itk++) {
	target_phrase = target_phrase + " " + input_token[itk];
      }
      ExecuteAct(SLLM, Mon, STORE_ST_A, NULL_ACT, FLUSH_OUT);
      GetInputPhrase(SLLM, Mon, target_phrase);
    }
    SLLM->RemPhfWG->OrderedWnnFlag = false;
    //VerboseFlag = true;
    //BestExploitation(SLLM, Mon, n_iter, target_phrase);
    //BestExploitation2(SLLM, Mon, n_iter, target_phrase);
    //ExploitationTest(SLLM, Mon, n_iter);
    Exploitation(SLLM, Mon, n_iter);
    //VerboseFlag = false;
    SLLM->RemPhfWG->OrderedWnnFlag = true;

    return 0;
  }
  ////////////////////////////////////////
  // Insert the working phrase and the current word group in the top of the
  // goal stack
  ////////////////////////////////////////
  else if (buf==".push_goal" || buf==".pg") { // push goal
    if (input_token.size()>1) {
      Display.Warning("syntax error.");
      return 1;
    }
    if (ExplorationPhaseIdx==0) {
      ExecuteAct(SLLM, Mon, START_ST_A, NULL_ACT, NULL_ACT);
      ExecuteAct(SLLM, Mon, STORE_ST_A, NULL_ACT, W_FROM_IN);
      ExplorationPhaseIdx++;
    }
    ExecuteAct(SLLM, Mon, STORE_ST_A, NULL_ACT, PUSH_GL);
    int ex_ph = ExplorationPhaseIdx;
    ExplorationApprove(SLLM, Mon);
    ExplorationPhaseIdx = ex_ph;
    //ExplorationPhaseIdx=0;
    //SLLM->EPhaseI->Clear();

    return 0;
  }
  ////////////////////////////////////////
  // Removes the goal phrase and the goal word group from the top
  // of the goal stack
  ////////////////////////////////////////
  else if (buf==".drop_goal" || buf==".dg") { // drop goal
    if (input_token.size()>1) {
      Display.Warning("syntax error.");
      return 1;
    }
    ExecuteAct(SLLM, Mon, STORE_ST_A, NULL_ACT, DROP_GL);
    int ex_ph = ExplorationPhaseIdx;
    ExplorationApprove(SLLM, Mon);
    ExplorationPhaseIdx = ex_ph;
    //ExplorationPhaseIdx=0;
    //SLLM->EPhaseI->Clear();

    return 0;
  }
  ////////////////////////////////////////
  // Copies the goal phrase from the top of the goal stack to the
  // working phrase
  ////////////////////////////////////////
  else if (buf==".get_goal_phrase" || buf==".ggp") { // get goal phrase
    if (input_token.size()>1) {
      Display.Warning("syntax error.");
      return 1;
    }
    ExecuteAct(SLLM, Mon, STORE_ST_A, NULL_ACT, GET_GL_PH);
    ExplorationApprove(SLLM, Mon);

    ExplorationPhaseIdx=1;
    //SLLM->EPhaseI->Clear();

    return 0;
  }

  ////////////////////////////////////////
  else if (buf==".working_phrase_out" || buf==".wpo") { // working phrase out
    if (input_token.size()>1) {
      Display.Warning("syntax error.");
      return 1;
    }
    WorkingPhraseOut(SLLM, Mon);

    return 0;
  }

  ////////////////////////////////////////
  // Sends to the output the next part of the working phrase, after the
  // current word, and all subsequent phrases of the same context until the
  // end of the context itself.
  ////////////////////////////////////////
  else if (buf==".sentence_out" || buf==".snto") { // sentence out
    if (input_token.size()>1) {
      Display.Warning("syntax error.");
      return 1;
    }
    SentenceOut(SLLM, Mon);

    ExecuteAct(SLLM, Mon, STORE_ST_A, NULL_ACT, FLUSH_WG);
    ExplorationApprove(SLLM, Mon);
    int n_iter=SLLM->ElActfSt->K;
    Reward(SLLM, Mon, 0, n_iter);

    SetAct(SLLM, START_ST_A, NULL_ACT, NULL_ACT);
    SLLM->StActRwdUpdate();
    Mon->Print();
    SLLM->Update();
    ExecuteAct(SLLM, Mon, STORE_ST_A, NULL_ACT, NULL_ACT);
    ExplorationApprove(SLLM, Mon);
    ExplorationPhaseIdx=0;
    AnswerTimeUpdate=true;

    return 0;
  }

  ////////////////////////////////////////
  // Loads phrases and/or commands from the file file_name.
  ////////////////////////////////////////
  else if (buf==".file" || buf==".f") { // read phrases/commands from file
    if (input_token.size()<2) {
      Display.Warning("a file name should be provided as argument.");
      return 1;
    }
    ifstream fs(input_token[1].c_str());
    if (!fs) {
      Display.Warning("Input file not found.");
      return 1;
    }
    while(std::getline (fs, buf))  {
      //Display.Print(buf+"\n");
      if (ParseCommand(SLLM, Mon, buf)==2) break;
    }
    return 0;
  }

  ////////////////////////////////////////
  // Gets the input phrase provided as argument without building the
  // associations between the word groups and the phrase.
  ////////////////////////////////////////
  else if (buf==".get_input_phrase" || buf==".gi") { // get input phrase
    if (input_token.size()<2) {
      Display.Warning("a phrase should be provided as argument.");
      return 1;
    }
    target_phrase = input_token[1];
    for(unsigned int itk=2; itk<input_token.size(); itk++) {
      target_phrase = target_phrase + " " + input_token[itk];
    }
    ExecuteAct(SLLM, Mon, STORE_ST_A, NULL_ACT, FLUSH_OUT);
    GetInputPhrase(SLLM, Mon, target_phrase);
    return 0;
  }
  ////////////////////////////////////////
  // Builds the associations between the word groups and the working phrase.
  ////////////////////////////////////////
  else if (buf==".build_association" || buf==".ba") { // build association
    if (input_token.size()!=1) {
      Display.Warning("no arguments should be provided.");
      return 1;
    }
    BuildAs(SLLM, Mon);

    return 0;
  }
  ////////////////////////////////////////
  else if (buf==".build_association_test" || buf==".bat") { // build association
    if (input_token.size()!=1) {
      Display.Warning("no arguments should be provided.");
      return 1;
    }
    BuildAsTest(SLLM, Mon);

    return 0;
  }
  ////////////////////////////////////////
  // Executes a system update with a NULL action
  ////////////////////////////////////////
  else if (buf==".null" || buf==".n") { // null action
    if (input_token.size()!=1) {
      Display.Warning("syntax error.");
      return 1;
    }
    SetAct(SLLM, NULL_ACT, NULL_ACT);
    SLLM->Update();
    return 0;
  }

  ////////////////////////////////////////
  // Sends a command to the monitor
  ////////////////////////////////////////
  else if (buf==".monitor" || buf==".m") { // send a command to the monitor
    if (input_token.size()<2) {
      Display.Warning("a monitor command should be provided as argument.");
      return 1;
    }
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
  else if (buf==".action" || buf==".a") { // executes single elaboration action
    if (input_token.size()!=2) {
      Display.Warning("syntax error.");
      return 1;
    }
    std::string action_name=input_token[1];
    int iact;
    for (iact=0; iact<ElActSize && Mon->ElActName[iact]!=action_name;
	 iact++);
    if (iact==ElActSize) {
      Display.Warning("unknown action name.");
      return 1;
    }
    SetAct(SLLM, NULL_ACT, iact);
    SLLM->Update();
    Mon->Print();
    return 0;
  }

  ////////////////////////////////////////
  // Displays the execution time
  ////////////////////////////////////////
  else if (buf==".time" || buf==".t") { // time
    if (input_token.size()>1) {
      Display.Warning("syntax error.");
      return 1;
    }

    clock_gettime( CLOCK_REALTIME, &clk1);

    Display.Print("Elapsed time: " + to_string
		  ((double)(clk1.tv_sec - clk0.tv_sec)
		    + (double)(clk1.tv_nsec - clk0.tv_nsec)*1e-9) + "\n");
    Display.Print("StActMem->act_time: " + to_string(SLLM->StActMem->act_time)
		  + "\n");
    Display.Print("StActMem->as_time: " + to_string(SLLM->StActMem->as_time)
		  + "\n");
    Display.Print("ElActfSt->act_time: " + to_string(SLLM->ElActfSt->act_time)
		  + "\n");
    Display.Print("ElActfSt->as_time: " + to_string(SLLM->ElActfSt->as_time)
		  + "\n");
    Display.Print("RemPh->act_time: " + to_string(SLLM->RemPh->act_time)
		  + "\n");
    Display.Print("RemPh->as_time: " + to_string(SLLM->RemPh->as_time) + "\n");
    Display.Print("RemPhfWG->act_time: " + to_string(SLLM->RemPhfWG->act_time)
		  + "\n");
    Display.Print("RemPhfWG->as_time: " + to_string(SLLM->RemPhfWG->as_time)
		  + "\n");
    int nl = 0;
    Display.Print("ElActfSt neurons: " + toStr(SLLM->ElActfSt->NewWnnNum)
		  + "\n");
    for (int i=0; i<SLLM->ElActfSt->NewWnnNum; i++) {
      nl += SLLM->ElActfSt->Nr[i]->NL();
    }
    Display.Print("ElActfSt links: " + toStr(nl) + "\n");

    return 0;
  }

  ////////////////////////////////////////
  // Displays some statistical information
  ////////////////////////////////////////
  else if (buf==".stat") { // statistic
    if (input_token.size()>1) {
      Display.Warning("syntax error.");
      return 1;
    }
    int nw = 0;
    for (int iw=0; iw<WMSize; iw++) {
      if (Mon->wflag[iw]>0.5) nw++;
    }
    Display.Print("Learned Words: " + toStr(nw) + "\n");
    int nph = SLLM->MemPh->HighVect[0];
    Display.Print("Learned Phrases: " + toStr(nph) + "\n");
    int nphwg = SLLM->RemPhfWG->NewWnnNum;
    Display.Print("Learned associations between word groups and phrases: "
		  + toStr(nphwg) + "\n");

    //Display.Print("IW neurons: " + toStr(SLLM->IW->NewWnnNum) + "\n");
    Display.Print("IW input links: " + toStr(SLLM->IW->CountInputLinks())
		  + "\n");
    Display.Print("ElActfSt neurons: " + toStr(SLLM->ElActfSt->NewWnnNum)
		  + "\n");
    Display.Print("ElActfSt input links: " +
		  toStr(SLLM->ElActfSt->CountSparseInputLinks()) + "\n");
    Display.Print("ElActfSt virtual input links: " +
		  toStr(SLLM->ElActfSt->CountVirtualInputLinks()) + "\n");
    Display.Print("ElActfSt output links: " +
		  toStr(SLLM->ElActfSt->CountOutputLinks()) + "\n");
    Display.Print("RemPh output links: " +
		  toStr(SLLM->RemPh->CountSparseOutputLinks()) + "\n");
    Display.Print("RemPh virtual output links: " +
		  toStr(SLLM->RemPh->CountVirtualOutputLinks()) + "\n");
    Display.Print("RemPhfWG neurons: " + toStr(SLLM->RemPhfWG->NewWnnNum)
		  + "\n");
    Display.Print("RemPhfWG input links: " +
		  toStr(SLLM->RemPhfWG->CountSparseInputLinks()) + "\n");
    Display.Print("RemPhfWG virtual input links: " +
		  toStr(SLLM->RemPhfWG->CountVirtualInputLinks()) + "\n");
    Display.Print("RemPhfWG output links: " +
		  toStr(SLLM->RemPhfWG->CountSparseOutputLinks()) + "\n");
    Display.Print("RemPhfWG virtual output links: " +
		  toStr(SLLM->RemPhfWG->CountVirtualOutputLinks()) + "\n");

    return 0;
  }

  ////////////////////////////////////////
  // Starts CUDA version of exploitation mode
  ////////////////////////////////////////
  else if (buf==".cuda" || buf==".cu") { // time
    if (input_token.size()>1) {
      Display.Warning("syntax error.");
      return 1;
    }
#ifdef CUDA
    SLLM->ElActfSt->cuda_CopyInpuLinks();
    SLLM->CudaFlag = true;
    return 0;
#else
    Display.Warning("CUDA is not available.");
    return 1;
#endif

  }

  ////////////////////////////////////////
  // Saves all variable-weight links to a file 
  ////////////////////////////////////////
  else if (buf==".save") { // save links
    if (input_token.size()>2) {
      Display.Warning("syntax error.");
      return 1;
    }
    string filename;
    if (input_token.size()==1) filename = "links.dat";
    else filename = input_token[1];

    FILE *fp=fopen(filename.c_str(), "wb");
    Mon->SaveWM(fp);
    if (SLLM->MemPh->HighVect.size()!=1) {
      Display.Warning("Error on MemPh.");
      return 1;
    }
    fwrite(&SLLM->MemPh->HighVect[0], sizeof(int), 1, fp);
    if (SLLM->StartPh->HighVect.size()!=1) {
      Display.Warning("Error on StartPh.");
      return 1;
    }
    fwrite(&SLLM->StartPh->HighVect[0], sizeof(int), 1, fp);

    SLLM->IW->SaveNr(fp);
    SLLM->IW->SaveInputLinks(fp);
    SLLM->ElActfSt->SaveNr(fp);
    SLLM->ElActfSt->SaveSparseInputLinks(fp);
    SLLM->ElActfSt->SaveOutputLinks(fp);
    SLLM->RemPh->SaveSparseOutputLinks(fp);
    SLLM->RemPhfWG->SaveNr(fp);
    SLLM->RemPhfWG->SaveSparseInputLinks(fp);
    SLLM->RemPhfWG->SaveSparseOutputLinks(fp);

    fclose(fp);

    return 0;
  }
  ////////////////////////////////////////
  // Loads all variable-weight links from a file 
  ////////////////////////////////////////
  else if (buf==".load") { // load links
    if (input_token.size()>2) {
      Display.Warning("syntax error.");
      return 1;
    }
    string filename;
    if (input_token.size()==1) filename = "links.dat";
    else filename = input_token[1];
    FILE *fp;
    if ((fp=fopen(filename.c_str(), "rb")) == NULL) {
      Display.Warning("Links file not found.");
      return 1;
    }
    Mon->LoadWM(fp);
    int i1;
    int nmemb = fread(&i1, sizeof(int), 1, fp);
    if (nmemb!=1) {
      Display.Warning("Error reading MemPh.");
      return 1;
    }
    SLLM->MemPh->Clear();
    SLLM->MemPh->HighVect.push_back(i1);
    SLLM->MemPh->Nr[i1]->O = 1;

    nmemb = fread(&i1, sizeof(int), 1, fp);
    if (nmemb!=1) {
      Display.Warning("Error reading StartPh.");
      return 1;
    }
    SLLM->StartPh->Clear();
    SLLM->StartPh->HighVect.push_back(i1);
    SLLM->StartPh->Nr[i1]->O = 1;

    SLLM->IW->LoadNr(fp);
    SLLM->IW->LoadInputLinks(fp);
    SLLM->ElActfSt->LoadNr(fp);
    SLLM->ElActfSt->LoadSparseInputLinks(fp);
    SLLM->ElActfSt->LoadOutputLinks(fp);
    SLLM->RemPh->LoadSparseOutputLinks(fp);
    SLLM->RemPhfWG->LoadNr(fp);
    SLLM->RemPhfWG->LoadSparseInputLinks(fp);
    SLLM->RemPhfWG->LoadSparseOutputLinks(fp);

    fclose(fp);

    return 0;
  }

  ////////////////////////////////////////
  // Writes the output to a log file
  ////////////////////////////////////////
  else if (buf==".logfile") { // write output to a log file
    if (input_token.size()<2) {
      Display.Warning("a file name or off should be provided as argument.");
      return 1;
    }
    if (input_token[1]=="off" && Display.LogFileFlag) {
      Mon->Display.LogFileFlag = Display.LogFileFlag = false;
      Display.LogFile->close();
      return 0;
    }
    else {
      if (Display.LogFileFlag) {
	Mon->Display.LogFileFlag = Display.LogFileFlag = false;
	Display.LogFile->close();
      }
      Display.LogFile->open(input_token[1].c_str());
      if (!Display.LogFile->is_open()) {
	Display.Warning("cannot open output log file.");
	return 1;
      }
      Mon->Display.LogFileFlag = Display.LogFileFlag = true;
      return 0;
    }
    return 1;
  }
  ////////////////////////////////////////
  // Displays the speaker names using the CHAT format (3 characters)
  ////////////////////////////////////////
  else if (buf==".speaker") { // display the name of the speaker
    if (input_token.size()<2) {
      Display.Warning("a speaker name or off should be provided as argument.");
      return 1;
    }
    if (input_token[1]=="off") {
      SpeakerFlag = false;
      return 0;
    }
    else {
      SpeakerName = input_token[1];
      SpeakerFlag = true;
      return 0;
    }
    return 1;
  }

  ////////////////////////////////////////
  // Records the answer time
  ////////////////////////////////////////
  else if (buf==".answer_time" || buf==".at") { // record answer time
    if (input_token.size()!=1) {
      Display.Warning("syntax error.");
      return 1;
    }
    FILE *at_fp=fopen("answer_time.dat", "w");
    fclose(at_fp);

    AnswerTimeFlag = true;

    return 0;
  }

  ////////////////////////////////////////
  // Evaluates the answer time
  ////////////////////////////////////////
  else if (buf==".evaluate_answer_time" || buf==".eat") {//evaluate answer time
    if (input_token.size()!=1) {
      Display.Warning("syntax error.");
      return 1;
    }

    struct timespec clk0, clk1;
    clock_gettime( CLOCK_REALTIME, &clk0);
    
    ExecuteAct(SLLM, Mon, STORE_ST_A, NULL_ACT, FLUSH_OUT);
    GetInputPhrase(SLLM, Mon, AnswerTimePhrase);
    Exploitation(SLLM, Mon, 1);

    clock_gettime( CLOCK_REALTIME, &clk1);
    double answ_time = clk1.tv_sec - clk0.tv_sec
      + (double)(clk1.tv_nsec - clk0.tv_nsec)*1e-9;
    double link_num = (double)SLLM->ElActfSt->CountSparseInputLinks();
    //FILE *at_fp=fopen("answer_time.dat", "a");
    printf("EAT: %.3e\t%.3f\n", link_num, answ_time);
    //fclose(at_fp);

    return 0;
  }

  ////////////////////////////////////////
  // Periodically save variable-link weights in files with progressive numbers
  ////////////////////////////////////////
  else if (buf==".auto_save_links" || buf==".asl") { // autosave links
    if (input_token.size()!=1) {
      Display.Warning("syntax error.");
      return 1;
    }
    AutoSaveLinkFlag = true;
    double link_num = (double)SLLM->ElActfSt->CountSparseInputLinks();
    AutoSaveLinkIndex = (int)(link_num/AutoSaveLinkStep);

    return 0;
  }

  ////////////////////////////////////////
  // Displays a period at the end of the output sentences
  ////////////////////////////////////////
  else if (buf==".period") { // display a period at the end of output sentences
    if (input_token.size()<2) {
      Display.Warning("on or off should be provided as argument.");
      return 1;
    }
    if (input_token[1]=="off") {
      PeriodFlag = false;
      return 0;
    }
    else if (input_token[1]=="on") {
      PeriodFlag = true;
      return 0;
    }
    else {
      Display.Warning("on or off should be provided as argument.");
      return 1;
    }

    return 1;
  }
  ////////////////////////////////////////
  else {
    Display.Warning("Unknown command.");
    return 1;
  }

  return 1;
}


int GetInputPhraseTest(sllm *SLLM, monitor *Mon, string input_phrase)
{
  int vin[WSize];

  //cout << input_phrase << endl;
  SetMode(SLLM, ACQUIRE);

  for (int ic=0; ic<WSize; ic++) vin[ic]=0;
  SLLM->W->Fill(vin);
  ExecuteAct(SLLM, Mon, STORE_ST_A, NULL_ACT, FLUSH_OUT);

  SetAct(SLLM, ACQ_W, NULL_ACT);

  string buf; // Have a buffer string
  stringstream ss(input_phrase); // Insert the string into a stream
  int i=0;
  while (ss >> buf && i<PhSize) {
    if (buf=="an") buf="a";
    if (buf=="-es") buf="-s";
    interface::w2bin((char*)buf.c_str(),vin);
    Mon->Print();
    SLLM->W->Fill(vin);
    SLLM->Update();
    Mon->MapW((char*)buf.c_str()); //swap with previous line!!!!

    i++;
  }
  for (int ic=0; ic<WSize; ic++) vin[ic]=0;
  SLLM->W->Fill(vin);

  SetAct(SLLM, NULL_ACT, W_FROM_WK);
  Mon->Print();
  SLLM->Update();

  // used to clear PrevWkPh, PrevWG, PrevInEqWGWx, PrevWkEqWGWx, ....
  ExecuteAct(SLLM, Mon, STORE_ST_A, NULL_ACT, FLUSH_WG);
  ExecuteAct(SLLM, Mon, STORE_ST_A, NULL_ACT, FLUSH_WG);

  ExplorationPhaseIdx=0;
  SLLM->EPhaseI->Clear();

  SetMode(SLLM, NULL_MODE);

  return 0;
}

int GetInputPhrase(sllm *SLLM, monitor *Mon, string input_phrase)
{
  int vin[WSize];

  //cout << input_phrase << endl;
  SLLM->StartAcquireFlag->Nr[0]->O = 1;

  SLLM->AcquireUpdate();
  SLLM->ElActFL->ActivOut();
  SLLM->AcqAct->ActivOut();
  SLLM->ElAct->ActivOut();
  Mon->Print();
  SLLM->Update();

  string buf; // Have a buffer string
  stringstream ss(input_phrase); // Insert the string into a stream
  int i=0;
  while (ss >> buf && i<PhSize) {
    if (buf=="an") buf="a";
    if (buf=="-es") buf="-s";
    interface::w2bin((char*)buf.c_str(),vin);
    SLLM->W->Fill(vin);
    SLLM->AcquireUpdate();
    SLLM->ElActFL->ActivOut();
    SLLM->AcqAct->ActivOut();
    SLLM->ElAct->ActivOut();
    Mon->Print();
    SLLM->Update();
    Mon->MapW((char*)buf.c_str()); //swap with previous line!!!!
    i++;
  }
  for (int ic=0; ic<WSize; ic++) vin[ic]=0; // blank word at the end of phrase
  SLLM->W->Fill(vin);
  do {
    SLLM->AcquireUpdate();
    SLLM->ElActFL->ActivOut();
    SLLM->AcqAct->ActivOut();
    SLLM->ElAct->ActivOut();
    Mon->Print();
    SLLM->Update();
  } while (SLLM->EndAcquireFlag->Nr[0]->O==0);

  ExplorationPhaseIdx=0;
  SLLM->EPhaseI->Clear();

  SLLM->AcquireUpdate();
  SetMode(SLLM, NULL_MODE);

  return 0;
}

int BuildAs(sllm *SLLM, monitor *Mon)
{
  //cout << "ok2\n";
  if (StartContextFlag) {
    StartContextFlag = false;
    SLLM->SetStartPhFlag->Nr[0]->O = 1;
  }
  else SLLM->SetStartPhFlag->Nr[0]->O = 0;

  SLLM->StartAssociateFlag->Nr[0]->O = 1;
  do {
    //cout << "okii\n";
    SLLM->AssociateUpdate();
    SLLM->ElActFL->ActivOut();
    SLLM->AcqAct->ActivOut();
    SLLM->ElAct->ActivOut();
    Mon->Print();
    SLLM->Update();
  } while (SLLM->EndAssociateFlag->Nr[0]->O==0);

  SLLM->AssociateUpdate();
  SetMode(SLLM, NULL_MODE);
  //cout << "ok3\n";
  return 0;
}

int BuildAsTest(sllm *SLLM, monitor *Mon)
{
  int PhI;

  SetMode(SLLM, ASSOCIATE);
  if (StartContextFlag) {
    StartContextFlag = false;
    ExecuteAct(SLLM, Mon, NULL_ACT, SET_START_PH, NULL_ACT);
  }
  else ExecuteAct(SLLM, Mon, NULL_ACT, MEM_PH, NULL_ACT);

  int SkipW = 0;
  SetAct(SLLM, NULL_ACT, W_FROM_WK);
  Mon->Print();
  SLLM->Update();
  while (SkipW<PhSize && SLLM->InPhB->RowDefault->Nr[SkipW]->O<0.5) {
    for (PhI=-1; PhI<SkipW; PhI++) {
      SetAct(SLLM, NEXT_AS_W, NULL_ACT);
      Mon->Print();
      //SLLM->ActUpdate(); //just to save time in place of Update
      SLLM->Update();
    }
    ExecuteAct(SLLM, Mon, STORE_ST_A, NULL_ACT, FLUSH_WG);
    for (int WGI=0; WGI<3 && PhI<PhSize
	   && SLLM->InPhB->RowDefault->Nr[PhI]->O<0.5; WGI++) {
      SetAct(SLLM, BUILD_AS, NULL_ACT);
      Mon->Print();
      SLLM->Update();

      SetAct(SLLM, NEXT_AS_W, NULL_ACT);
      Mon->Print();
      SLLM->Update();
      PhI++;
    }
    SkipW++;
    SetAct(SLLM, NULL_ACT, W_FROM_WK);
    Mon->Print();
    SLLM->Update();
  }
  // used to clear PrevWkPh, PrevWG, PrevInEqWGWx, PrevWkEqWGWx, ....
  ExecuteAct(SLLM, Mon, STORE_ST_A, NULL_ACT, FLUSH_WG);
  ExecuteAct(SLLM, Mon, STORE_ST_A, NULL_ACT, FLUSH_WG);

  SetMode(SLLM, NULL_MODE);

  return 0;
}


int MoreRetrAsSlow(sllm *SLLM, monitor *Mon)
{
  int next_act, Nas=20;
  for (int i=0; i<Nas; i++) {
    //cout << "MoreRetrAs " << i << endl;
    SetAct(SLLM, NULL_ACT, RETR_AS);
    SLLM->Update();
    //Mon->Print();
    ExecuteAct(SLLM, Mon, RETR_EL_A, NULL_ACT, NULL_ACT);
    next_act=Mon->GetElActFL();
    if (next_act!=NULL_ACT) break;    
  }
  
  return next_act;
}

int ExploitationSlow(sllm *SLLM, monitor *Mon)
{
  SetMode(SLLM, EXPLOIT);
  //cout << "\nExploitation\n";

  int iac, Nac=300, Nb=60;
  int prev_act, next_act;

  ExecuteAct(SLLM, Mon, START_ST_A, NULL_ACT, NULL_ACT);
  for (iac=0; iac<Nac; iac++) {
    prev_act=Mon->GetElActFL();
    for (int ib=0; ib<Nb; ib++) {    
      ExecuteAct(SLLM, Mon, RETR_EL_A, NULL_ACT, NULL_ACT);
      next_act=Mon->GetElActFL();
      if (next_act==NULL_ACT && prev_act==RETR_AS)
	next_act = MoreRetrAsSlow(SLLM,Mon);

      if (SLLM->ElActfSt->NumWnn()>=5
	  && next_act!=NULL_ACT
	  && (prev_act!=RETR_AS || next_act!=RETR_AS)) break;
      SLLM->ElActfSt->GB++;
      //cout << "next act not found. Trying with GB=" << SLLM->ElActfSt->GB
      //	   << endl;
    }
    LastGB = SLLM->ElActfSt->GB;
    SLLM->ElActfSt->GB=0;
    if (next_act==NULL_ACT || SLLM->ElActfSt->NumWnn()<5) {
      Display.Warning("next act not found with GB=" + toStr(LastGB) +
		      ". Exiting\n");
      break;
    }

    ExecuteAct(SLLM, Mon, STORE_ST_A, NULL_ACT, NULL_ACT);
    if (SLLM->OutFlag->Nr[0]->O>0.5) {
      Mon->GetPhM("OutPhB", SLLM->OutPhB);
      Display.Print(Mon->OutPhrase+"\n");
    }
    //if (SLLM->OutFlag->Nr[0]->O>0.5) {
    if (SLLM->ElAct->Nr[CONTINUE-1]->O>0.5) {
      Display.Print(" ... ");
    }
    if (SLLM->ElAct->Nr[DONE-1]->O>0.5) {
      Display.Print(".\n");
      //Display.Print("Done. Exiting\n");
      //Mon->Print();
      //Mon->GetPhM("OutPhB", SLLM->OutPhB);
      //Display.Print(Mon->OutPhrase+"\n");
      break;
    }
  }
  if (iac==Nac) Display.Warning("Exploitation number of actions >= " +
				toStr(Nac) + ". Exiting\n");
  SetMode(SLLM, NULL_MODE);

  return 0;
}


int MoreRetrAs(sllm *SLLM, monitor *Mon)
{
  int next_act, Nas=20;
  for (int i=0; i<Nas; i++) {
    //cout << "MoreRetrAs " << i << endl;
    SetAct(SLLM, NULL_ACT, RETR_AS);
    SLLM->Update();
    //Mon->Print();
    ExecuteAct(SLLM, Mon, RETR_EL_A, NULL_ACT, NULL_ACT);
    next_act=Mon->GetElActFL();
    if (next_act!=NULL_ACT) break;    
  }
  
  return next_act;
}

int ExplorationRetry(sllm *SLLM, monitor *Mon)
{
  //cout << "\nExplorationRetry (RetrieveStActIdx)\n";

  if (ExplorationPhaseIdx==3) ExplorationPhaseIdx=4;
  else if (ExplorationPhaseIdx==4) ExplorationPhaseIdx=1;

  //cout << "StoredStActI: " << SLLM->StoredStActI->HighVect[0] << endl;
  //cout << "StActI before: " << SLLM->StActI->HighVect[0] << endl;
  //Mon->Print();
  SetAct(SLLM, RETR_SAI, NULL_ACT, NULL_ACT);
  SLLM->StActRwdUpdate();
  //cout << "StActI after: " << SLLM->StActI->HighVect[0] << endl;
  SetAct(SLLM, RETR_ST_A, NULL_ACT, NULL_ACT);
  SLLM->StActRwdUpdate();
  SetAct(SLLM, RETR_SAI, NULL_ACT, NULL_ACT);
  SLLM->StActRwdUpdate();
  //Mon->Print();
  SLLM->RetrStActIFlag->Nr[0]->O=0;
  SLLM->CurrStActIFlag->Nr[0]->O=1;

  return 0;
}

int GetStActIdx(sllm *SLLM, monitor *Mon)
{
  int vi[StActSize];

  for (int ib=0; ib<StActSize; ib++) {
    vi[ib]=SLLM->StActI->Nr[ib]->O;
  } 
  int is=interface::v2i(vi, StActSize);

  return is;
}

int RewardTest(sllm *SLLM, monitor *Mon, int partial_flag, int n_iter)
{
  SetMode(SLLM, REWARD);

  SetAct(SLLM, STORE_ST_A, NULL_ACT, FLUSH_OUT);
  SLLM->StActRwdUpdate();
  Mon->Print();
  SLLM->Update();

  SetAct(SLLM, STORE_ST_A, NULL_ACT, WG_OUT);
  SLLM->StActRwdUpdate();
  Mon->Print();
  SLLM->Update();

  if (partial_flag!=0) SetAct(SLLM, STORE_ST_A, NULL_ACT, CONTINUE);
  else SetAct(SLLM, STORE_ST_A, NULL_ACT, DONE);
  SLLM->StActRwdUpdate();
  Mon->Print();
  SLLM->Update();

  SetAct(SLLM, STORE_ST_A, NULL_ACT, NULL_ACT);
  SLLM->StActRwdUpdate();
  Mon->Print();
  SLLM->Update();
  //Mon->Print();
  int LastStActI = GetStActIdx(SLLM, Mon); //SLLM->StActI
  for (int i=0; i<n_iter; i++) {
    //Mon->ModeMessage("Start State-Action Index\n");
    SetAct(SLLM, START_ST_A, NULL_ACT, NULL_ACT);
    SLLM->StActRwdUpdate();
    Mon->Print();
    SLLM->Update();
    for (int StActI=0; StActI<LastStActI-1; StActI++) {
      //Mon->ModeMessage("Retrieve and reward State-Action\n");
      SetAct(SLLM, RWD_ST_A, NULL_ACT, NULL_ACT);
      SLLM->StActRwdUpdate();
      Mon->Print();
      SLLM->Update();
    }
  }

  SetMode(SLLM, NULL_MODE);
  return 0;
}

int Reward(sllm *SLLM, monitor *Mon, int partial_flag, int n_iter)
{
  int vin[IterSize];
  for (int i=0; i<IterSize; i++) vin[i] = (i==n_iter) ? 1 : 0;
  SLLM->IterNum->Fill(vin);

  SLLM->StartRewardFlag->Nr[0]->O = 1;
  SLLM->PartialFlag->Nr[0]->O = partial_flag;
  do {
    SLLM->RewardUpdate();
    SLLM->ElActFL->ActivOut();
    SLLM->AcqAct->ActivOut();
    SLLM->StActRwdUpdate();
    Mon->Print();
    SLLM->Update();
  } while (SLLM->EndRewardFlag->Nr[0]->O==0);

  SLLM->RewardUpdate();
  SetMode(SLLM, NULL_MODE);

  return 0;
}

string Exploitation(sllm *SLLM, monitor *Mon, int n_iter)
{
  int Nupdate, MaxNupdate=4000;
  int vin[IterSize];
  for (int i=0; i<IterSize-1; i++) vin[i] = (i==n_iter-1) ? 1 : 0;
  SLLM->IterNum->Fill(vin);

  SLLM->StartExploitFlag->Nr[0]->O = 1;
  string OutPhrase="", BestPhrase="";
  int BestDB=1000;
  int IterI=0;
  Nupdate=0;
  do {
    Nupdate++;
    //if (Nupdate%100==0) Display.Print("Number of updates: " + toStr(Nupdate)
    //				      + "\n");
    if (Nupdate>=MaxNupdate) {
      Display.Warning("Exploitation number of updates >= " +
				  toStr(MaxNupdate) + "\n");
      Reset(SLLM, Mon);
      SLLM->EndExploitFlag->Nr[0]->O=1;
      SLLM->ExploitUpdate();
      SetMode(SLLM, NULL_MODE);

      return "";
    }

    SLLM->RewardUpdate();
    SLLM->ExploitUpdate();
    SLLM->RwdAct->ActivOut();
    SLLM->ElActFL->ActivOut();
    SLLM->AcqAct->ActivOut();
    //Mon->PrintRwdAct();

    //if (SLLM->ElAct->Default->Nr[0]->O<0.5)
    Mon->Print();
    if (VerboseFlag) Mon->PrintRwdAct();
    //int prev_act=Mon->GetElAct(); // temporary
    SLLM->StActRwdUpdate();
    //int next_act=Mon->GetElAct(); // temporary
    if (VerboseFlag) Mon->PrintElActFL();
    if (VerboseFlag) Mon->PrintElAct();

    SLLM->Update();

    if (n_iter>1 && SLLM->Exploit_I30->Nr[0]->O>0.5) {
      Display.Print("Trying n. " + toStr(IterI));
      IterI++; 
    }

      /////////////////////// the following has to be checked when possible

      //if (next_act==SNT_OUT) {//SLLM->ElAct->Nr[SNT_OUT-1]->O>0.5) {
    if (SLLM->ElAct->Nr[SNT_OUT-1]->O>0.5 &&
	SLLM->RwdAct->Nr[STORE_ST_A-1]->O>0.5) {
      int DB = SLLM->ElActfSt->DB;
      if (OutPhrase!="") OutPhrase = OutPhrase + " ";
      OutPhrase = OutPhrase + SentenceOut(SLLM, Mon);
      if (n_iter>1) cout << "DB: " << DB << endl;
      else Display.Print(".\n");
      if (DB<BestDB) {
	BestDB = DB;
	BestPhrase = OutPhrase;
      }
      OutPhrase="";
      break;
    }
      /////////////////////// end part to be checked


    if (SLLM->OutFlag->Nr[0]->O>0.5 && SLLM->RwdAct->Nr[STORE_ST_A-1]->O>0.5) {
      Mon->GetPhM("OutPhB", SLLM->OutPhB);
      Display.Print(" -> " + Mon->OutPhrase+"\n");
      if (OutPhrase!="") OutPhrase = OutPhrase + " ";
      OutPhrase = OutPhrase + Mon->OutPhrase;
      Nupdate=0;
    }
    //if (SLLM->OutFlag->Nr[0]->O>0.5) {
    if (SLLM->ElAct->Nr[CONTINUE-1]->O>0.5) {
      Display.Print(" ... ");
    }
    if (SLLM->ElAct->Nr[DONE-1]->O>0.5) {
      int DB = SLLM->ElActfSt->DB;
      if (n_iter>1) cout << "DB: " << DB << endl;
      else Display.Print(".\n");
      if (DB<BestDB) {
	BestDB = DB;
	BestPhrase = OutPhrase;
      }
      OutPhrase="";
    }

  } while (SLLM->EndExploitFlag->Nr[0]->O==0);

  SLLM->ExploitUpdate();
  if (SpeakerFlag) Display.Print("*SYS:\t");
  Display.Print(BestPhrase);
  if (PeriodFlag) Display.Print(" . \n");
  else Display.Print("\n");
  SetMode(SLLM, NULL_MODE);

  return BestPhrase;
}

string ExploitationTest(sllm *SLLM, monitor *Mon, int n_iter)
{
  SetMode(SLLM, EXPLOIT);
  //cout << "\nExploitation\n";

  int iac, Nac=300, Nas=100; // it should be Nas=MaxNRetr in sizes.h
  int prev_act, next_act;
  string OutPhrase="", BestPhrase="";
  int BestDB=1000;

  ExecuteAct(SLLM, Mon, START_ST_A, NULL_ACT, NULL_ACT);
  ExecuteAct(SLLM, Mon, STORE_ST_A, NULL_ACT, NULL_ACT);
  for (int IterI=0; IterI<n_iter; IterI++) {
    ExecuteAct(SLLM, Mon, START_ST_A, NULL_ACT, NULL_ACT);
    if (n_iter>1) {
      Display.Print("Trying n. " + toStr(IterI));
    }
    ExecuteAct(SLLM, Mon, RETR_ST_A, NULL_ACT, NULL_ACT);
    for (iac=0; iac<Nac; iac++) {
      prev_act=Mon->GetElActFL(); //Mon->PrintElAct();
      if (prev_act==RETR_AS) {
	int best_DB = 1000, best_act = NULL_ACT;
	ExecuteAct(SLLM, Mon, STORE_ST_A, NULL_ACT, NULL_ACT);
	for (int i=0; i<Nas; i++) {
	  ExecuteAct(SLLM, Mon, NULL_ACT, NULL_ACT, RETR_AS);
	  ExecuteAct(SLLM, Mon, RETR_EL_A, NULL_ACT, NULL_ACT);
	  next_act=Mon->GetElActFL(); //Mon->PrintElAct();
	  //int n_wnn = SLLM->ElActfSt->NumWnn();
	  //cout << "next act " << next_act << endl;
	  //cout << "NumWnn: " << n_wnn << endl;
	  //cout << "DB: " << SLLM->ElActfSt->DB << endl;
	  int DB = SLLM->ElActfSt->DB;
	  if (next_act!=RETR_AS && next_act!=NULL_ACT
	      && SLLM->ElActfSt->NumWnn()>=5 && DB<best_DB) {
	    best_DB = DB;
	    best_act = next_act;
	    ExecuteAct(SLLM, Mon, CHANGE_ST_A, NULL_ACT, NULL_ACT);
	    //cout << "best DB updated to: " << best_DB << endl;
	  }
	  else {
	    ExecuteAct(SLLM, Mon, NULL_ACT, NULL_ACT, NULL_ACT);
	  }
	}
	next_act = best_act; 
	if (next_act==NULL_ACT) {
	  Display.Warning("next act not found with Nas=" + toStr(Nas) +
			  ". Exiting\n");
	  break;
	}
	ExecuteAct(SLLM, Mon, GET_ST_A, NULL_ACT, NULL_ACT);
	//LastGB = best_DB;
      }
      else {
	ExecuteAct(SLLM, Mon, RETR_EL_A, NULL_ACT, NULL_ACT);
	next_act=Mon->GetElActFL(); // Mon->PrintElAct();
	
	if (next_act==NULL_ACT || SLLM->ElActfSt->NumWnn()<5) {
	  Display.Warning("next act not found with GB="
			  + toStr(SLLM->ElActfSt->DB) + ". Exiting\n");
	  break;
	}
      }


      if (next_act==SNT_OUT) {//SLLM->ElAct->Nr[SNT_OUT-1]->O>0.5) {
	int DB = SLLM->ElActfSt->DB;
	if (OutPhrase!="") OutPhrase = OutPhrase + " ";
	OutPhrase = OutPhrase + SentenceOut(SLLM, Mon);
	if (n_iter>1) cout << "DB: " << DB << endl;
	else Display.Print(".\n");
	if (DB<BestDB) {
	  BestDB = DB;
	  BestPhrase = OutPhrase;
	}
	OutPhrase="";
	break;
      }
      ExecuteAct(SLLM, Mon, STORE_ST_A, NULL_ACT, next_act);
      if (SLLM->OutFlag->Nr[0]->O>0.5) {
	Mon->GetPhM("OutPhB", SLLM->OutPhB);
	Display.Print(" -> " + Mon->OutPhrase+"\n");
	if (OutPhrase!="") OutPhrase = OutPhrase + " ";
	OutPhrase = OutPhrase + Mon->OutPhrase;
      }
      //if (SLLM->OutFlag->Nr[0]->O>0.5) {
      if (SLLM->ElAct->Nr[CONTINUE-1]->O>0.5) {
	Display.Print(" ... ");
      }
      if (SLLM->ElAct->Nr[DONE-1]->O>0.5) {
	int DB = SLLM->ElActfSt->DB;
	if (n_iter>1) cout << "DB: " << DB << endl;
	else Display.Print(".\n");
	if (DB<BestDB) {
	  BestDB = DB;
	  BestPhrase = OutPhrase;
	}
	OutPhrase="";
	break;
      }
      
    }
    if (iac==Nac) Display.Warning("Exploitation number of actions >= " +
				  toStr(Nac) + ". Exiting\n");
  }
  if (SpeakerFlag) Display.Print("*SYS:\t");
  Display.Print(BestPhrase);
  if (PeriodFlag) Display.Print(" . \n");
  else Display.Print("\n");

  SetMode(SLLM, NULL_MODE);
  
  return BestPhrase;
}

int ttt=0;
int TargetExploration(sllm *SLLM, monitor *Mon, std::string name,
		       std::string target_phrase)
{
  int vin[PhSize];

  // temp1 start here
  SLLM->EPhaseI->Clear();
  SLLM->EPhaseI->Nr[ExplorationPhaseIdx]->O = 1;
  // temp1 end here

  if (name=="WGB") SLLM->WGTargetFlag->Nr[0]->O = 1;
  else SLLM->WGTargetFlag->Nr[0]->O = 0;

  SLLM->TargetEqWkPhFlag->Nr[0]->O = 0;
  SLLM->TargetEqWGFlag->Nr[0]->O = 0;
  SLLM->StartExploreFlag->Nr[0]->O = 1;

  int i=0;
  do {
    SLLM->AssociateUpdate();
    SLLM->RewardUpdate();
    SLLM->ExploitUpdate();
    SLLM->ExploreUpdate();
    SLLM->RwdAct->ActivOut();
    SLLM->ElActFL->ActivOut();
    SLLM->AcqAct->ActivOut();

    //if (SLLM->ElAct->Default->Nr[0]->O<0.5)
    Mon->Print();
    if (VerboseFlag) Mon->PrintRwdAct();
    SLLM->StActRwdUpdate();
    if (VerboseFlag) {Mon->PrintElActFL(); Mon->PrintElAct();}

    SLLM->Update();

    if (SLLM->CheckWGFlag->Nr[0]->O>0.5) {
      Mon->GetPhM(name, SLLM->WGB);
      if (target_phrase==Mon->OutPhrase)
	SLLM->TargetEqWGFlag->Nr[0]->O = 1;
    }
    if (SLLM->CheckWkPhFlag->Nr[0]->O>0.5) {
      Mon->GetPhM(name, SLLM->WkPhB);
      if (target_phrase==Mon->OutPhrase)
	SLLM->TargetEqWkPhFlag->Nr[0]->O = 1;
    }
    if (SLLM->GetRndFlag->Nr[0]->O>0.5) {
      int N1, N2;
      N1=rnd_int()%10; //N1=1+seed%5;
      N2=rnd_int()%6; //1+seed%5;
      //cout << N1 << " " << N2 << endl;
      for (int i=0; i<PhSize; i++) vin[i] = (i==N1) ? 1 : 0;
      SLLM->SkipW->Fill(vin);
      for (int i=0; i<PhSize; i++) vin[i] = (i==N2) ? 1 : 0;
      SLLM->ExplWordNum->Fill(vin);
    }
    i++;
  } while (SLLM->EndExploreFlag->Nr[0]->O==0);

  SLLM->ExploreUpdate();

  SetMode(SLLM, NULL_MODE);

  // temp2 start here
  for (int i=0; i<5; i++) {
    if (SLLM->EPhaseI->Nr[i]->O>0.5) {
      ExplorationPhaseIdx = i;
      break;
    }
  }
  // temp2 end here

  return 0;
}

int ExplorationApprove(sllm *SLLM, monitor *Mon)
{
  ExecuteAct(SLLM, Mon, STORE_SAI, NULL_ACT, NULL_ACT);

  if (ExplorationPhaseIdx!=4) ExplorationPhaseIdx++;

  return 0;
}

int TargetExplorationTest(sllm *SLLM, monitor *Mon, std::string name,
			  std::string target_phrase)
{
  //cout << "\nExploration\n";
  SetMode(SLLM, EXPLORE);

  ExecuteAct(SLLM, Mon, NULL_ACT, NULL_ACT, NULL_ACT);
  if (name=="WGB" && (ExplorationPhaseIdx==3 || ExplorationPhaseIdx==4))
    ExplorationPhaseIdx = 1;
  int StartExplorationPhaseIdx = ExplorationPhaseIdx;
  if (ExplorationPhaseIdx==0) StartExplorationPhaseIdx = 1;
  int i = 0;
  while (i<TRYLIMIT) {
    if (ExplorationPhaseIdx==0) {
      ExecuteAct(SLLM, Mon, START_ST_A, NULL_ACT, NULL_ACT);
      ExecuteAct(SLLM, Mon, STORE_ST_A, NULL_ACT, W_FROM_IN);
      ExplorationApprove(SLLM, Mon);
      ExplorationPhaseIdx=1;
    }
    if (ExplorationPhaseIdx==1) {
      int N1, N2;
      N1=1+rnd_int()%10;
      N2=1+rnd_int()%6;
      ExecuteAct(SLLM, Mon, STORE_ST_A, NULL_ACT, W_FROM_WK);
      for (int i=0; i<N1; i++) {
	ExecuteAct(SLLM, Mon, STORE_ST_A, NULL_ACT, NEXT_W);
      }
      ExecuteAct(SLLM, Mon, STORE_ST_A, NULL_ACT, FLUSH_WG);
      for (int i=0; i<N2; i++) {
	ExecuteAct(SLLM, Mon, STORE_ST_A, NULL_ACT, GET_W);
	ExecuteAct(SLLM, Mon, STORE_ST_A, NULL_ACT, NEXT_W);
	Mon->GetWM("CW", SLLM->CW);
	if (Mon->OutStr[0]=="") break;
      }
      if (name=="WGB") {
	ExecuteAct(SLLM, Mon, NULL_ACT, NULL_ACT, NULL_ACT);
	Mon->GetPhM(name, SLLM->WGB);
	if (target_phrase==Mon->OutPhrase) break;
	//else cout << "exp WBG: " << Mon->OutPhrase << endl;
      }
      else { // (name=="WkPhB")
	ExplorationPhaseIdx = 2;
	continue;
      }
    }
    else if (ExplorationPhaseIdx==2) {
      ExecuteAct(SLLM, Mon, STORE_ST_A, NULL_ACT, RETR_AS);
      if (name=="WGB") {
	ExplorationPhaseIdx = 1;
	continue;
      }
      else { // (name=="WkPhB")
	Mon->GetPhM(name, SLLM->WkPhB);
	if (target_phrase==Mon->OutPhrase) break;
      }
    }
    else if (ExplorationPhaseIdx==3) {
      ExecuteAct(SLLM, Mon, STORE_ST_A, NULL_ACT, GET_START_PH);
      Mon->GetPhM(name, SLLM->WkPhB);
      if (target_phrase==Mon->OutPhrase) break;
    }
    else if (ExplorationPhaseIdx==4) {
      ExecuteAct(SLLM, Mon, STORE_ST_A, NULL_ACT, GET_NEXT_PH);
      Mon->GetPhM(name, SLLM->WkPhB);
      if (target_phrase==Mon->OutPhrase) break;
    }
    ExplorationPhaseIdx = StartExplorationPhaseIdx;
    if (ExplorationPhaseIdx==3) StartExplorationPhaseIdx=4;
    if (ExplorationPhaseIdx==4) StartExplorationPhaseIdx=1;
    ExecuteAct(SLLM, Mon, RETR_SAI, NULL_ACT, NULL_ACT);
    ExecuteAct(SLLM, Mon, RETR_ST, NULL_ACT, NULL_ACT);
    ExecuteAct(SLLM, Mon, RETR_SAI, NULL_ACT, NULL_ACT);
    i++;
  }
  CheckTryLimit(i);
  cout << "Good answer after " << i << " iterations\n";
  ExplorationApprove(SLLM, Mon);

  SetMode(SLLM, NULL_MODE);

  return 0;
}

int SearchContext(sllm *SLLM, monitor *Mon, std::string target_phrase)
{
  //cout << "\nSearch context\n";
  SetMode(SLLM, EXPLORE);

  if (ExplorationPhaseIdx==0) {
    ExecuteAct(SLLM, Mon, START_ST_A, NULL_ACT, NULL_ACT);
    ExecuteAct(SLLM, Mon, STORE_ST_A, NULL_ACT, W_FROM_IN);
  }
  ExplorationApprove(SLLM, Mon);
  ExecuteAct(SLLM, Mon, STORE_ST_A, NULL_ACT, GET_START_PH);
  ExplorationPhaseIdx = 3;
  Mon->GetPhM("WkPhB", SLLM->WkPhB);
  cout << " ... " << Mon->OutPhrase << endl;
  if (target_phrase!=Mon->OutPhrase) {
    ExplorationPhaseIdx = 4;
    while(Mon->OutPhrase!=".end_context" && Mon->OutPhrase!="") {
      ExecuteAct(SLLM, Mon, STORE_ST_A, NULL_ACT, GET_NEXT_PH);
      Mon->GetPhM("WkPhB", SLLM->WkPhB);
      cout << " ... " << Mon->OutPhrase << endl;
      if (target_phrase==Mon->OutPhrase) break;
    }
  }
  if (target_phrase!=Mon->OutPhrase) {
    ExecuteAct(SLLM, Mon, RETR_SAI, NULL_ACT, NULL_ACT);
    ExecuteAct(SLLM, Mon, RETR_ST, NULL_ACT, NULL_ACT);
    ExecuteAct(SLLM, Mon, RETR_SAI, NULL_ACT, NULL_ACT);
  }
  else  {
    ExplorationApprove(SLLM, Mon);
    Display.Print(target_phrase + "\n");
  }
    
  SetMode(SLLM, NULL_MODE);

  return 0;
}

int ContinueSearchContext(sllm *SLLM, monitor *Mon, std::string target_phrase)
{
  //cout << "\nSearch context\n";
  SetMode(SLLM, EXPLORE);

  if (ExplorationPhaseIdx==0) {
    ExecuteAct(SLLM, Mon, START_ST_A, NULL_ACT, NULL_ACT);
    ExecuteAct(SLLM, Mon, STORE_ST_A, NULL_ACT, W_FROM_IN);
  }
  ExplorationApprove(SLLM, Mon);

  ExplorationPhaseIdx = 4;
  do {
    ExecuteAct(SLLM, Mon, STORE_ST_A, NULL_ACT, GET_NEXT_PH);
    Mon->GetPhM("WkPhB", SLLM->WkPhB);
    cout << " ... " << Mon->OutPhrase << endl;
    if (target_phrase==Mon->OutPhrase) break;
  } while(Mon->OutPhrase!=".end_context" && Mon->OutPhrase!="");
  if (target_phrase!=Mon->OutPhrase) {
    ExecuteAct(SLLM, Mon, RETR_SAI, NULL_ACT, NULL_ACT);
    ExecuteAct(SLLM, Mon, RETR_ST, NULL_ACT, NULL_ACT);
    ExecuteAct(SLLM, Mon, RETR_SAI, NULL_ACT, NULL_ACT);
  }
  else  {
    ExplorationApprove(SLLM, Mon);
    Display.Print(target_phrase + "\n");
  }
    
  SetMode(SLLM, NULL_MODE);

  return 0;
}

int WorkingPhraseOut(sllm *SLLM, monitor *Mon)
{
  //cout << "\nSend working phrase to output\n";
  SetMode(SLLM, EXPLORE);

  ExecuteAct(SLLM, Mon, STORE_ST_A, NULL_ACT, W_FROM_WK);
  ExecuteAct(SLLM, Mon, STORE_ST_A, NULL_ACT, FLUSH_WG);
  for (int i=0; i<PhSize; i++) {
    ExecuteAct(SLLM, Mon, STORE_ST_A, NULL_ACT, GET_W);
    ExecuteAct(SLLM, Mon, STORE_ST_A, NULL_ACT, NEXT_W);
    // try to restore the following lines
    //Mon->GetWM("CW", SLLM->CW);
    //	if (Mon->OutStr[0]=="") break;
    //}
  }
  SetAct(SLLM, STORE_ST_A, NULL_ACT, FLUSH_OUT);
  SLLM->StActRwdUpdate();
  Mon->Print();
  SLLM->Update();

  SetAct(SLLM, STORE_ST_A, NULL_ACT, WG_OUT);
  SLLM->StActRwdUpdate();
  Mon->Print();
  SLLM->Update();

  ExplorationPhaseIdx = 2;
  ExplorationApprove(SLLM, Mon);

  SetMode(SLLM, NULL_MODE);

  return 0;
}

string SentenceOut(sllm *SLLM, monitor *Mon)
{
  //cout << "\nSend sentence to output\n";
  string out_phrase = "";

  ExecuteAct(SLLM, Mon, STORE_ST_A, NULL_ACT, SNT_OUT);
  if (SLLM->ModeFlags->Nr[EXPLORE]->O>0.5) {
    ExplorationApprove(SLLM, Mon);
    ExplorationPhaseIdx = 2;
  }
  //ExecuteAct(SLLM, Mon, NULL_ACT, NULL_ACT, FLUSH_WG);

  Mon->GetPhM("WkPhB", SLLM->WkPhB);
  while(Mon->OutPhrase!=".end_context" && Mon->OutPhrase!="") {
    ExecuteAct(SLLM, Mon, NULL_ACT, NULL_ACT, FLUSH_WG);
    //while (SLLM->PhI->Nr[PhSize-1]->O<0.5) {
    for(;;) {
      ExecuteAct(SLLM, Mon, NULL_ACT, NULL_ACT, GET_W);
      if (SLLM->PhI->Nr[PhSize-1]->O>0.5) break;
      ExecuteAct(SLLM, Mon, NULL_ACT, NULL_ACT, NEXT_W);
      // try to restore the following lines
      //Mon->GetWM("CW", SLLM->CW);
      //	if (Mon->OutStr[0]=="") break;
      //}
    } 
    ExecuteAct(SLLM, Mon, NULL_ACT, NULL_ACT, FLUSH_OUT);
    ExecuteAct(SLLM, Mon, NULL_ACT, NULL_ACT, WG_OUT);
    Mon->GetPhM("OutPhB", SLLM->OutPhB);
    if (out_phrase!="") Display.Print(" ... ");
    Display.Print("-> " + Mon->OutPhrase+"\n");
    out_phrase = out_phrase + Mon->OutPhrase;
    ExecuteAct(SLLM, Mon, NULL_ACT, NULL_ACT, GET_NEXT_PH);
    Mon->GetPhM("WkPhB", SLLM->WkPhB);
  }
  //Display.Print("\n");

  return out_phrase;
}

int Reset(sllm *SLLM, monitor *Mon)
{
  SetMode(SLLM, NULL_MODE);
  //StartContextFlag=true;

  ExecuteAct(SLLM, Mon, NULL_ACT, FLUSH, NULL_ACT);
  ExecuteAct(SLLM, Mon, NULL_ACT, NULL_ACT, FLUSH_OUT);
  for (int i=0; i<5; i++)
    ExecuteAct(SLLM, Mon, STORE_ST_A, NULL_ACT, DROP_GL);

  int v1[WSize];
  for (int ic=0; ic<WSize; ic++) v1[ic]=0;
  SLLM->W->Fill(v1);

  int v2[PhSize];
  for (int i=0; i<PhSize; i++) v2[i] = 0;
  SLLM->InI->Fill(v2);
  SLLM->PhI->Fill(v2);

  SLLM->Update();
  SLLM->StartExploitFlag->Nr[0]->O = 1;
  SLLM->ExploitUpdate();
  SLLM->EndExploitFlag->Nr[0]->O=1;
  SLLM->ExploitUpdate();

  ExecuteAct(SLLM, Mon, START_ST_A, NULL_ACT, NULL_ACT);
  ExplorationApprove(SLLM, Mon);
  ExplorationPhaseIdx=0;

  return 0;
}
