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

#include <Annabell.h>
#include <Monitor.h>
#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <set>
#include <stdio.h>
#include <stdlib.h>
#include "sizes.h"
#include "interface.h"
#include "rnd.h"
#include "display.h" 
#include "ann_exception.h"
#include <sys/time.h>
#include "gettime.h" 
#include "CommandUtils.h"

using namespace std;
using namespace sizes;

const int TRYLIMIT = 10000;

int LastGB;
int ExplorationPhaseIdx;
int StoredStActI;
display Display;
bool VerboseFlag = false;
bool StartContextFlag = true;
bool PeriodFlag = false;
bool SpeakerFlag = false;
bool AnswerTimeFlag = false;
bool AnswerTimeUpdate = false;
string AnswerTimePhrase = "? do you have any friend -s";
string SpeakerName = "HUM";
bool AutoSaveLinkFlag = false;
int AutoSaveLinkIndex = 0;
long AutoSaveLinkStep = 2000000;

struct timespec clk0, clk1;

int GetInputPhrase(Annabell *annabell, Monitor *Mon, string input_phrase);
int GetInputPhraseTest(Annabell *annabell, Monitor *Mon, string input_phrase);
int BuildAs(Annabell *annabell, Monitor *Mon);
int BuildAsTest(Annabell *annabell, Monitor *Mon);
int ExplorationApprove(Annabell *annabell, Monitor *Mon);
int ExplorationRetry(Annabell *annabell, Monitor *Mon);
int Reward(Annabell *annabell, Monitor *Mon, int partial_flag, int n_iter);
int RewardTest(Annabell *annabell, Monitor *Mon, int partial_flag, int n_iter);
string Exploitation(Annabell *annabell, Monitor *Mon, int n_iter);
string ExploitationTest(Annabell *annabell, Monitor *Mon, int n_iter);
int ExploitationSlow(Annabell *annabell, Monitor *Mon);
int TargetExploration(Annabell *annabell, Monitor *Mon, string name, string target_phrase);
int TargetExplorationTest(Annabell *annabell, Monitor *Mon, string name, string target_phrase);
int SearchContext(Annabell *annabell, Monitor *Mon, string target_phrase);
int ContinueSearchContext(Annabell *annabell, Monitor *Mon, string target_phrase);
int WorkingPhraseOut(Annabell *annabell, Monitor *Mon);
string SentenceOut(Annabell *annabell, Monitor *Mon);
int Interface(Annabell *annabell, Monitor *Mon);
int Reset(Annabell *annabell, Monitor *Mon);

bool simplify(Annabell *annabell, Monitor *Mon, vector<string> input_token);
int ParseCommand(Annabell *annabell, Monitor *Mon, string input_line);

template <typename T>
string to_string(T const& value) {
    stringstream sstr;
    sstr << value;
    return sstr.str();
}

int main() {

	Display.LogFileFlag = false;
	Display.ConsoleFlag = true;

	try {
		init_randmt(12345);

		Annabell *annabell = new Annabell();
		Monitor *Mon = new Monitor(annabell);

		delete Display.LogFile;
		Display.LogFile = Mon->Display.LogFile;

		Interface(annabell, Mon);

		return 0;
	} catch (ann_exception &e) {
		cerr << "Error: " << e.what() << "\n";
		return 1;
	} catch (bad_alloc&) {
		cerr << "Error allocating memory.\n";
		return 1;
	} catch (...) {
		cerr << "Unrecognized error.\n";
		return 1;
	}
}

int CheckTryLimit(int i) {
  if (i>=TRYLIMIT)
    throw ann_exception("Too many attempts\n");

  return 0;
}

int SetAct(Annabell *annabell, int acq_act, int el_act) {
  annabell->AcqAct->Fill((int*)v_acq_act[acq_act]);
  annabell->ElActFL->Fill((int*)v_el_act[el_act]);
  annabell->ElAct->Fill((int*)v_el_act[el_act]);

  return 0;
}

int SetAct(Annabell *annabell, int rwd_act, int acq_act, int el_act) {
  annabell->RwdAct->Fill((int*)v_rwd_act[rwd_act]);
  annabell->AcqAct->Fill((int*)v_acq_act[acq_act]);
  annabell->ElActFL->Fill((int*)v_el_act[el_act]);
  annabell->ElAct->Fill((int*)v_el_act[el_act]);

  return 0;
}

int ExecuteAct(Annabell *annabell, Monitor *Mon, int rwd_act, int acq_act, int el_act) {
  SetAct(annabell, rwd_act, acq_act, el_act);
  Mon->Print();
  if (VerboseFlag) Mon->PrintRwdAct();
  annabell->StActRwdUpdate();
  if (VerboseFlag) {Mon->PrintElActFL(); Mon->PrintElAct();}
  annabell->Update();

  return 0;
}

int Interface(Annabell *annabell, Monitor *mon) {
  string input_line;
  bool out_flag = false;

  GetRealTime(&clk0);

  while(true) {
    cout << "Enter command: ";

    if (!getline (cin, input_line)) {
    	out_flag=true;
    }

    if (ParseCommand(annabell, mon, input_line) == 2 || out_flag) {
    	break;
    }
  }
  
  if (Display.LogFile->is_open()) {
	  Display.LogFile->close();
  }

  return 0;
}


/**
 * Read command or input phrase from command line.
 * @returns 2 for .quit command.
 */
int ParseCommand(Annabell *annabell, Monitor *Mon, string input_line) {

	vector<string> input_token;

	stringstream ss(input_line); // Insert the line into a stream
	string buf, buf1; // buffer strings

	buf = "";
	while (ss >> buf1) { // split line in string tokens

		buf1 = CommandUtils::processArticle(buf1);

		buf1 = CommandUtils::processPlural(buf1);

		buf = buf + buf1;

		if (CommandUtils::startsWith(buf, '/') && !CommandUtils::endsWith(buf, '/')) {
			buf = buf + " ";
			continue;
		}

		input_token.push_back(buf);
		buf = "";
	}

	if (buf != "") {
		int l = buf.size() - 1;
		if (buf[l] == ' ') {
			buf.erase(l);
		}
		input_token.push_back(buf);
	}

	if (simplify(annabell, Mon, input_token)) {
		return 0;
	}

  string target_phrase;
  if (input_token.size()==0) {
    Display.Print(input_line+"\n");
    target_phrase = ".end_context";
    GetInputPhrase(annabell, Mon, target_phrase);
    ExecuteAct(annabell, Mon, NULL_ACT, MEM_PH, NULL_ACT);
    Display.Print(" >>> End context\n");
    StartContextFlag=true;

    // answer time
    if (AnswerTimeFlag && AnswerTimeUpdate && AnswerTimePhrase!="") {
      struct timespec clk0, clk1;
      GetRealTime(&clk0);

      AnswerTimeUpdate=false;
      ExecuteAct(annabell, Mon, STORE_ST_A, NULL_ACT, FLUSH_OUT);
      GetInputPhrase(annabell, Mon, AnswerTimePhrase);
      Exploitation(annabell, Mon, 1);

      GetRealTime(&clk1);
      double answ_time = clk1.tv_sec - clk0.tv_sec
	+ (double)(clk1.tv_nsec - clk0.tv_nsec)*1e-9;
      double link_num = (double)annabell->ElActfSt->CountSparseInputLinks();
      FILE *at_fp=fopen("answer_time.dat", "a");
      fprintf(at_fp, "%.3e\t%.3f\n", link_num, answ_time);
      fclose(at_fp);
    }
    // auto save links
    if (AutoSaveLinkFlag) {
      double link_num = (double)annabell->ElActfSt->CountSparseInputLinks();
      int index = (int)(link_num/AutoSaveLinkStep);
      if (index>AutoSaveLinkIndex) {
	AutoSaveLinkIndex = index;
	char filename[20];
	sprintf(filename, "links_%d.dat", AutoSaveLinkIndex);
	FILE *fp=fopen(filename, "wb");
	Mon->SaveWM(fp);
	if (annabell->MemPh->HighVect.size()!=1) {
	  Display.Warning("Error on MemPh.");
	  return 1;
	}
	fwrite(&annabell->MemPh->HighVect[0], sizeof(int), 1, fp);
	if (annabell->StartPh->HighVect.size()!=1) {
	  Display.Warning("Error on StartPh.");
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
      }
    }
    // added 5/01/2013
    SetAct(annabell, START_ST_A, NULL_ACT, NULL_ACT);
    annabell->StActRwdUpdate();
    Mon->Print();
    annabell->Update();
    ExecuteAct(annabell, Mon, STORE_ST_A, NULL_ACT, NULL_ACT);

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
    ExecuteAct(annabell, Mon, STORE_ST_A, NULL_ACT, FLUSH_OUT);
    GetInputPhrase(annabell, Mon, input_line);
    BuildAs(annabell, Mon);
    //BuildAsTest(annabell, Mon);

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
    //TargetExplorationTest(annabell, Mon, "WkPhB", target_phrase);
    TargetExploration(annabell, Mon, "WkPhB", target_phrase);
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
      ExecuteAct(annabell, Mon, STORE_ST_A, NULL_ACT, FLUSH_WG);
      ExecuteAct(annabell, Mon, STORE_ST_A, NULL_ACT, W_FROM_WK);
      return 0;
    }
    target_phrase = input_token[1];
    for(unsigned int itk=2; itk<input_token.size(); itk++) {
      target_phrase = target_phrase + " " + input_token[itk];
    }
    //VerboseFlag=true;
    //TargetExplorationTest(annabell, Mon, "WGB", target_phrase);
    TargetExploration(annabell, Mon, "WGB", target_phrase);
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
    SearchContext(annabell, Mon, target_phrase);
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
    ContinueSearchContext(annabell, Mon, target_phrase);
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
    ExecuteAct(annabell, Mon, STORE_ST_A, NULL_ACT, W_FROM_IN);
    //ExecuteAct(annabell, Mon, STORE_ST_A, MEM_PH, NULL_ACT); // dummy
    ExplorationApprove(annabell, Mon);
    //Mon->PrintPhM("WkPhB", annabell->WkPhB);
    //Mon->PrintSSMidx("RemPh", annabell->RemPh);
    //Mon->PrintSSMidx("MemPh", annabell->MemPh);
    ExplorationPhaseIdx=1;
    //annabell->EPhaseI->Clear();

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
    if (input_token.size()==1) n_iter=annabell->ElActfSt->K;
    else {
      stringstream ss1(input_token[1]);
      ss1 >> n_iter;
      if (!ss1) {
	Display.Warning("Cannot convert token to integer.");
      }
    }
    //int ex_ph = ExplorationPhaseIdx;
    Reward(annabell, Mon, 0, n_iter);
    //ExplorationPhaseIdx=ex_ph;
    //ExplorationPhaseIdx=0;
    //annabell->EPhaseI->Clear();

    // added 5/01/2013
    SetAct(annabell, START_ST_A, NULL_ACT, NULL_ACT);
    annabell->StActRwdUpdate();
    Mon->Print();
    annabell->Update();
    ExecuteAct(annabell, Mon, STORE_ST_A, NULL_ACT, NULL_ACT);
    ExplorationApprove(annabell, Mon);
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
    if (input_token.size()==1) n_iter=annabell->ElActfSt->K;
    else {
      stringstream ss1(input_token[1]);
      ss1 >> n_iter;
      if (!ss1) {
	Display.Warning("Cannot convert token to integer.");
      }
    }
    //int ex_ph = ExplorationPhaseIdx;
    Reward(annabell, Mon, 1, n_iter);
    //ExplorationPhaseIdx=ex_ph;
    //ExplorationPhaseIdx=0;
    //annabell->EPhaseI->Clear();

    // added 5/01/2013
    SetAct(annabell, START_ST_A, NULL_ACT, NULL_ACT);
    annabell->StActRwdUpdate();
    Mon->Print();
    annabell->Update();
    ExecuteAct(annabell, Mon, STORE_ST_A, NULL_ACT, NULL_ACT);
    ExplorationApprove(annabell, Mon);
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
      ExecuteAct(annabell, Mon, STORE_ST_A, NULL_ACT, FLUSH_OUT);
      GetInputPhrase(annabell, Mon, target_phrase);
    }
    //VerboseFlag = true;
    //ExploitationTest(annabell, Mon, 1);
    Exploitation(annabell, Mon, 1);
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
      ExecuteAct(annabell, Mon, STORE_ST_A, NULL_ACT, DROP_GL);

    if (input_token.size()>1) {
      target_phrase = input_token[1];
      for(unsigned int itk=2; itk<input_token.size(); itk++) {
	target_phrase = target_phrase + " " + input_token[itk];
      }
      if (AnswerTimePhrase=="")  AnswerTimePhrase = target_phrase;
      ExecuteAct(annabell, Mon, STORE_ST_A, NULL_ACT, FLUSH_OUT);
      GetInputPhrase(annabell, Mon, target_phrase);
    }
    //VerboseFlag = true;
    //ExploitationTest(annabell, Mon, 1);
    Exploitation(annabell, Mon, 1);
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
      ExecuteAct(annabell, Mon, STORE_ST_A, NULL_ACT, FLUSH_OUT);
      GetInputPhrase(annabell, Mon, target_phrase);
    }
    annabell->RemPhfWG->OrderedWnnFlag = false;
    //VerboseFlag = true;
    //ExploitationTest(annabell, Mon, 1);
    Exploitation(annabell, Mon, 1);
    //VerboseFlag = false;
    annabell->RemPhfWG->OrderedWnnFlag = true;

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
      ExecuteAct(annabell, Mon, STORE_ST_A, NULL_ACT, FLUSH_OUT);
      GetInputPhrase(annabell, Mon, target_phrase);
    }
    //VerboseFlag = true;
    //ExploitationTest(annabell, Mon, 1);
    string best_phrase = Exploitation(annabell, Mon, 1);
    //VerboseFlag = false;
    ExecuteAct(annabell, Mon, STORE_ST_A, NULL_ACT, FLUSH_OUT);
    GetInputPhrase(annabell, Mon, best_phrase);
    BuildAs(annabell, Mon);

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
      stringstream ss1(input_token[1]);
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
      ExecuteAct(annabell, Mon, STORE_ST_A, NULL_ACT, FLUSH_OUT);
      GetInputPhrase(annabell, Mon, target_phrase);
    }
    annabell->RemPhfWG->OrderedWnnFlag = false;
    //VerboseFlag = true;
    //BestExploitation(annabell, Mon, n_iter, target_phrase);
    //BestExploitation2(annabell, Mon, n_iter, target_phrase);
    //ExploitationTest(annabell, Mon, n_iter);
    Exploitation(annabell, Mon, n_iter);
    //VerboseFlag = false;
    annabell->RemPhfWG->OrderedWnnFlag = true;

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
      ExecuteAct(annabell, Mon, START_ST_A, NULL_ACT, NULL_ACT);
      ExecuteAct(annabell, Mon, STORE_ST_A, NULL_ACT, W_FROM_IN);
      ExplorationPhaseIdx++;
    }
    ExecuteAct(annabell, Mon, STORE_ST_A, NULL_ACT, PUSH_GL);
    int ex_ph = ExplorationPhaseIdx;
    ExplorationApprove(annabell, Mon);
    ExplorationPhaseIdx = ex_ph;
    //ExplorationPhaseIdx=0;
    //annabell->EPhaseI->Clear();

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
    ExecuteAct(annabell, Mon, STORE_ST_A, NULL_ACT, DROP_GL);
    int ex_ph = ExplorationPhaseIdx;
    ExplorationApprove(annabell, Mon);
    ExplorationPhaseIdx = ex_ph;
    //ExplorationPhaseIdx=0;
    //annabell->EPhaseI->Clear();

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
    ExecuteAct(annabell, Mon, STORE_ST_A, NULL_ACT, GET_GL_PH);
    ExplorationApprove(annabell, Mon);

    ExplorationPhaseIdx=1;
    //annabell->EPhaseI->Clear();

    return 0;
  }

  ////////////////////////////////////////
  else if (buf==".working_phrase_out" || buf==".wpo") { // working phrase out
    if (input_token.size()>1) {
      Display.Warning("syntax error.");
      return 1;
    }
    WorkingPhraseOut(annabell, Mon);

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
    SentenceOut(annabell, Mon);

    ExecuteAct(annabell, Mon, STORE_ST_A, NULL_ACT, FLUSH_WG);
    ExplorationApprove(annabell, Mon);
    int n_iter=annabell->ElActfSt->K;
    Reward(annabell, Mon, 0, n_iter);

    SetAct(annabell, START_ST_A, NULL_ACT, NULL_ACT);
    annabell->StActRwdUpdate();
    Mon->Print();
    annabell->Update();
    ExecuteAct(annabell, Mon, STORE_ST_A, NULL_ACT, NULL_ACT);
    ExplorationApprove(annabell, Mon);
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
    while(getline (fs, buf))  {
      //Display.Print(buf+"\n");
      if (ParseCommand(annabell, Mon, buf)==2) break;
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
    ExecuteAct(annabell, Mon, STORE_ST_A, NULL_ACT, FLUSH_OUT);
    GetInputPhrase(annabell, Mon, target_phrase);
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
    BuildAs(annabell, Mon);

    return 0;
  }
  ////////////////////////////////////////
  else if (buf==".build_association_test" || buf==".bat") { // build association
    if (input_token.size()!=1) {
      Display.Warning("no arguments should be provided.");
      return 1;
    }
    BuildAsTest(annabell, Mon);

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
    SetAct(annabell, NULL_ACT, NULL_ACT);
    annabell->Update();
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
    string action_name=input_token[1];
    int iact;
    for (iact=0; iact<ElActSize && Mon->ElActName[iact]!=action_name;
	 iact++);
    if (iact==ElActSize) {
      Display.Warning("unknown action name.");
      return 1;
    }
    SetAct(annabell, NULL_ACT, iact);
    annabell->Update();
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

    GetRealTime(&clk1);

    Display.Print("Elapsed time: " + to_string
		  ((double)(clk1.tv_sec - clk0.tv_sec)
		    + (double)(clk1.tv_nsec - clk0.tv_nsec)*1e-9) + "\n");
    Display.Print("StActMem->act_time: " + to_string(annabell->StActMem->act_time)
		  + "\n");
    Display.Print("StActMem->as_time: " + to_string(annabell->StActMem->as_time)
		  + "\n");
    Display.Print("ElActfSt->act_time: " + to_string(annabell->ElActfSt->act_time)
		  + "\n");
    Display.Print("ElActfSt->as_time: " + to_string(annabell->ElActfSt->as_time)
		  + "\n");
    Display.Print("RemPh->act_time: " + to_string(annabell->RemPh->act_time)
		  + "\n");
    Display.Print("RemPh->as_time: " + to_string(annabell->RemPh->as_time) + "\n");
    Display.Print("RemPhfWG->act_time: " + to_string(annabell->RemPhfWG->act_time)
		  + "\n");
    Display.Print("RemPhfWG->as_time: " + to_string(annabell->RemPhfWG->as_time)
		  + "\n");
    int nl = 0;
    Display.Print("ElActfSt neurons: " + toStr(annabell->ElActfSt->NewWnnNum)
		  + "\n");
    for (int i=0; i<annabell->ElActfSt->NewWnnNum; i++) {
      nl += annabell->ElActfSt->Nr[i]->NL();
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
    int nph = annabell->MemPh->HighVect[0];
    Display.Print("Learned Phrases: " + toStr(nph) + "\n");
    int nphwg = annabell->RemPhfWG->NewWnnNum;
    Display.Print("Learned associations between word groups and phrases: "
		  + toStr(nphwg) + "\n");

    //Display.Print("IW neurons: " + toStr(annabell->IW->NewWnnNum) + "\n");
    Display.Print("IW input links: " + toStr(annabell->IW->CountInputLinks())
		  + "\n");
    Display.Print("ElActfSt neurons: " + toStr(annabell->ElActfSt->NewWnnNum)
		  + "\n");
    Display.Print("ElActfSt input links: " +
		  toStr(annabell->ElActfSt->CountSparseInputLinks()) + "\n");
    Display.Print("ElActfSt virtual input links: " +
		  toStr(annabell->ElActfSt->CountVirtualInputLinks()) + "\n");
    Display.Print("ElActfSt output links: " +
		  toStr(annabell->ElActfSt->CountOutputLinks()) + "\n");
    Display.Print("RemPh output links: " +
		  toStr(annabell->RemPh->CountSparseOutputLinks()) + "\n");
    Display.Print("RemPh virtual output links: " +
		  toStr(annabell->RemPh->CountVirtualOutputLinks()) + "\n");
    Display.Print("RemPhfWG neurons: " + toStr(annabell->RemPhfWG->NewWnnNum)
		  + "\n");
    Display.Print("RemPhfWG input links: " +
		  toStr(annabell->RemPhfWG->CountSparseInputLinks()) + "\n");
    Display.Print("RemPhfWG virtual input links: " +
		  toStr(annabell->RemPhfWG->CountVirtualInputLinks()) + "\n");
    Display.Print("RemPhfWG output links: " +
		  toStr(annabell->RemPhfWG->CountSparseOutputLinks()) + "\n");
    Display.Print("RemPhfWG virtual output links: " +
		  toStr(annabell->RemPhfWG->CountVirtualOutputLinks()) + "\n");

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
    annabell->ElActfSt->cuda_CopyInpuLinks();
    annabell->CudaFlag = true;
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
    if (annabell->MemPh->HighVect.size()!=1) {
      Display.Warning("Error on MemPh.");
      return 1;
    }
    fwrite(&annabell->MemPh->HighVect[0], sizeof(int), 1, fp);
    if (annabell->StartPh->HighVect.size()!=1) {
      Display.Warning("Error on StartPh.");
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
    annabell->MemPh->Clear();
    annabell->MemPh->HighVect.push_back(i1);
    annabell->MemPh->Nr[i1]->O = 1;

    nmemb = fread(&i1, sizeof(int), 1, fp);
    if (nmemb!=1) {
      Display.Warning("Error reading StartPh.");
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
    GetRealTime(&clk0);
    
    ExecuteAct(annabell, Mon, STORE_ST_A, NULL_ACT, FLUSH_OUT);
    GetInputPhrase(annabell, Mon, AnswerTimePhrase);
    Exploitation(annabell, Mon, 1);

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
  else if (buf==".auto_save_links" || buf==".asl") { // autosave links
    if (input_token.size()!=1) {
      Display.Warning("syntax error.");
      return 1;
    }
    AutoSaveLinkFlag = true;
    double link_num = (double)annabell->ElActfSt->CountSparseInputLinks();
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


int GetInputPhraseTest(Annabell *annabell, Monitor *Mon, string input_phrase)
{
  int vin[WSize];

  //cout << input_phrase << endl;
  annabell->SetMode(ACQUIRE);

  for (int ic=0; ic<WSize; ic++) vin[ic]=0;
  annabell->W->Fill(vin);
  ExecuteAct(annabell, Mon, STORE_ST_A, NULL_ACT, FLUSH_OUT);

  SetAct(annabell, ACQ_W, NULL_ACT);

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

  SetAct(annabell, NULL_ACT, W_FROM_WK);
  Mon->Print();
  annabell->Update();

  // used to clear PrevWkPh, PrevWG, PrevInEqWGWx, PrevWkEqWGWx, ....
  ExecuteAct(annabell, Mon, STORE_ST_A, NULL_ACT, FLUSH_WG);
  ExecuteAct(annabell, Mon, STORE_ST_A, NULL_ACT, FLUSH_WG);

  ExplorationPhaseIdx=0;
  annabell->EPhaseI->Clear();

  annabell->SetMode(NULL_MODE);

  return 0;
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

  ExplorationPhaseIdx=0;
  annabell->EPhaseI->Clear();

  annabell->AcquireUpdate();
  annabell->SetMode(NULL_MODE);

  return 0;
}

int BuildAs(Annabell *annabell, Monitor *Mon)
{
  //cout << "ok2\n";
  if (StartContextFlag) {
    StartContextFlag = false;
    annabell->SetStartPhFlag->Nr[0]->O = 1;
  }
  else annabell->SetStartPhFlag->Nr[0]->O = 0;

  annabell->StartAssociateFlag->Nr[0]->O = 1;
  do {
    //cout << "okii\n";
    annabell->AssociateUpdate();
    annabell->ElActFL->ActivOut();
    annabell->AcqAct->ActivOut();
    annabell->ElAct->ActivOut();
    Mon->Print();
    annabell->Update();
  } while (annabell->EndAssociateFlag->Nr[0]->O==0);

  annabell->AssociateUpdate();
  annabell->SetMode(NULL_MODE);
  //cout << "ok3\n";
  return 0;
}

int BuildAsTest(Annabell *annabell, Monitor *Mon)
{
  int PhI;

  annabell->SetMode(ASSOCIATE);
  if (StartContextFlag) {
    StartContextFlag = false;
    ExecuteAct(annabell, Mon, NULL_ACT, SET_START_PH, NULL_ACT);
  }
  else ExecuteAct(annabell, Mon, NULL_ACT, MEM_PH, NULL_ACT);

  int SkipW = 0;
  SetAct(annabell, NULL_ACT, W_FROM_WK);
  Mon->Print();
  annabell->Update();
  while (SkipW<PhSize && annabell->InPhB->RowDefault->Nr[SkipW]->O<0.5) {
    for (PhI=-1; PhI<SkipW; PhI++) {
      SetAct(annabell, NEXT_AS_W, NULL_ACT);
      Mon->Print();
      //annabell->ActUpdate(); //just to save time in place of Update
      annabell->Update();
    }
    ExecuteAct(annabell, Mon, STORE_ST_A, NULL_ACT, FLUSH_WG);
    for (int WGI=0; WGI<3 && PhI<PhSize
	   && annabell->InPhB->RowDefault->Nr[PhI]->O<0.5; WGI++) {
      SetAct(annabell, BUILD_AS, NULL_ACT);
      Mon->Print();
      annabell->Update();

      SetAct(annabell, NEXT_AS_W, NULL_ACT);
      Mon->Print();
      annabell->Update();
      PhI++;
    }
    SkipW++;
    SetAct(annabell, NULL_ACT, W_FROM_WK);
    Mon->Print();
    annabell->Update();
  }
  // used to clear PrevWkPh, PrevWG, PrevInEqWGWx, PrevWkEqWGWx, ....
  ExecuteAct(annabell, Mon, STORE_ST_A, NULL_ACT, FLUSH_WG);
  ExecuteAct(annabell, Mon, STORE_ST_A, NULL_ACT, FLUSH_WG);

  annabell->SetMode(NULL_MODE);

  return 0;
}


int MoreRetrAsSlow(Annabell *annabell, Monitor *Mon)
{
  int next_act, Nas=20;
  for (int i=0; i<Nas; i++) {
    //cout << "MoreRetrAs " << i << endl;
    SetAct(annabell, NULL_ACT, RETR_AS);
    annabell->Update();
    //Mon->Print();
    ExecuteAct(annabell, Mon, RETR_EL_A, NULL_ACT, NULL_ACT);
    next_act=Mon->GetElActFL();
    if (next_act!=NULL_ACT) break;    
  }
  
  return next_act;
}

int ExploitationSlow(Annabell *annabell, Monitor *Mon)
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
      Display.Warning("next act not found with GB=" + toStr(LastGB) +
		      ". Exiting\n");
      break;
    }

    ExecuteAct(annabell, Mon, STORE_ST_A, NULL_ACT, NULL_ACT);
    if (annabell->OutFlag->Nr[0]->O>0.5) {
      Mon->GetPhM("OutPhB", annabell->OutPhB);
      Display.Print(Mon->OutPhrase+"\n");
    }
    //if (annabell->OutFlag->Nr[0]->O>0.5) {
    if (annabell->ElAct->Nr[CONTINUE-1]->O>0.5) {
      Display.Print(" ... ");
    }
    if (annabell->ElAct->Nr[DONE-1]->O>0.5) {
      Display.Print(".\n");
      //Display.Print("Done. Exiting\n");
      //Mon->Print();
      //Mon->GetPhM("OutPhB", annabell->OutPhB);
      //Display.Print(Mon->OutPhrase+"\n");
      break;
    }
  }
  if (iac==Nac) Display.Warning("Exploitation number of actions >= " +
				toStr(Nac) + ". Exiting\n");
  annabell->SetMode(NULL_MODE);

  return 0;
}


int MoreRetrAs(Annabell *annabell, Monitor *Mon)
{
  int next_act, Nas=20;
  for (int i=0; i<Nas; i++) {
    //cout << "MoreRetrAs " << i << endl;
    SetAct(annabell, NULL_ACT, RETR_AS);
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

  if (ExplorationPhaseIdx==3) ExplorationPhaseIdx=4;
  else if (ExplorationPhaseIdx==4) ExplorationPhaseIdx=1;

  //cout << "StoredStActI: " << annabell->StoredStActI->HighVect[0] << endl;
  //cout << "StActI before: " << annabell->StActI->HighVect[0] << endl;
  //Mon->Print();
  SetAct(annabell, RETR_SAI, NULL_ACT, NULL_ACT);
  annabell->StActRwdUpdate();
  //cout << "StActI after: " << annabell->StActI->HighVect[0] << endl;
  SetAct(annabell, RETR_ST_A, NULL_ACT, NULL_ACT);
  annabell->StActRwdUpdate();
  SetAct(annabell, RETR_SAI, NULL_ACT, NULL_ACT);
  annabell->StActRwdUpdate();
  //Mon->Print();
  annabell->RetrStActIFlag->Nr[0]->O=0;
  annabell->CurrStActIFlag->Nr[0]->O=1;

  return 0;
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

int RewardTest(Annabell *annabell, Monitor *Mon, int partial_flag, int n_iter)
{
  annabell->SetMode(REWARD);

  SetAct(annabell, STORE_ST_A, NULL_ACT, FLUSH_OUT);
  annabell->StActRwdUpdate();
  Mon->Print();
  annabell->Update();

  SetAct(annabell, STORE_ST_A, NULL_ACT, WG_OUT);
  annabell->StActRwdUpdate();
  Mon->Print();
  annabell->Update();

  if (partial_flag!=0) SetAct(annabell, STORE_ST_A, NULL_ACT, CONTINUE);
  else SetAct(annabell, STORE_ST_A, NULL_ACT, DONE);
  annabell->StActRwdUpdate();
  Mon->Print();
  annabell->Update();

  SetAct(annabell, STORE_ST_A, NULL_ACT, NULL_ACT);
  annabell->StActRwdUpdate();
  Mon->Print();
  annabell->Update();
  //Mon->Print();
  int LastStActI = GetStActIdx(annabell, Mon); //annabell->StActI
  for (int i=0; i<n_iter; i++) {
    //Mon->ModeMessage("Start State-Action Index\n");
    SetAct(annabell, START_ST_A, NULL_ACT, NULL_ACT);
    annabell->StActRwdUpdate();
    Mon->Print();
    annabell->Update();
    for (int StActI=0; StActI<LastStActI-1; StActI++) {
      //Mon->ModeMessage("Retrieve and reward State-Action\n");
      SetAct(annabell, RWD_ST_A, NULL_ACT, NULL_ACT);
      annabell->StActRwdUpdate();
      Mon->Print();
      annabell->Update();
    }
  }

  annabell->SetMode(NULL_MODE);
  return 0;
}

int Reward(Annabell *annabell, Monitor *Mon, int partial_flag, int n_iter)
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

  return 0;
}

string Exploitation(Annabell *annabell, Monitor *Mon, int n_iter)
{
  int Nupdate, MaxNupdate=4000;
  int vin[IterSize];
  for (int i=0; i<IterSize-1; i++) vin[i] = (i==n_iter-1) ? 1 : 0;
  annabell->IterNum->Fill(vin);

  annabell->StartExploitFlag->Nr[0]->O = 1;
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
    if (VerboseFlag) Mon->PrintRwdAct();
    //int prev_act=Mon->GetElAct(); // temporary
    annabell->StActRwdUpdate();
    //int next_act=Mon->GetElAct(); // temporary
    if (VerboseFlag) Mon->PrintElActFL();
    if (VerboseFlag) Mon->PrintElAct();

    annabell->Update();

    if (n_iter>1 && annabell->Exploit_I30->Nr[0]->O>0.5) {
      Display.Print("Trying n. " + toStr(IterI));
      IterI++; 
    }

      /////////////////////// the following has to be checked when possible

      //if (next_act==SNT_OUT) {//annabell->ElAct->Nr[SNT_OUT-1]->O>0.5) {
    if (annabell->ElAct->Nr[SNT_OUT-1]->O>0.5 &&
	annabell->RwdAct->Nr[STORE_ST_A-1]->O>0.5) {
      int DB = annabell->ElActfSt->DB;
      if (OutPhrase!="") OutPhrase = OutPhrase + " ";
      OutPhrase = OutPhrase + SentenceOut(annabell, Mon);
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


    if (annabell->OutFlag->Nr[0]->O>0.5 && annabell->RwdAct->Nr[STORE_ST_A-1]->O>0.5) {
      Mon->GetPhM("OutPhB", annabell->OutPhB);
      Display.Print(" -> " + Mon->OutPhrase+"\n");
      if (OutPhrase!="") OutPhrase = OutPhrase + " ";
      OutPhrase = OutPhrase + Mon->OutPhrase;
      Nupdate=0;
    }
    //if (annabell->OutFlag->Nr[0]->O>0.5) {
    if (annabell->ElAct->Nr[CONTINUE-1]->O>0.5) {
      Display.Print(" ... ");
    }
    if (annabell->ElAct->Nr[DONE-1]->O>0.5) {
      int DB = annabell->ElActfSt->DB;
      if (n_iter>1) cout << "DB: " << DB << endl;
      else Display.Print(".\n");
      if (DB<BestDB) {
	BestDB = DB;
	BestPhrase = OutPhrase;
      }
      OutPhrase="";
    }

  } while (annabell->EndExploitFlag->Nr[0]->O==0);

  annabell->ExploitUpdate();
  if (SpeakerFlag) Display.Print("*SYS:\t");
  Display.Print(BestPhrase);
  if (PeriodFlag) Display.Print(" . \n");
  else Display.Print("\n");
  annabell->SetMode(NULL_MODE);

  return BestPhrase;
}

string ExploitationTest(Annabell *annabell, Monitor *Mon, int n_iter)
{
  annabell->SetMode(EXPLOIT);
  //cout << "\nExploitation\n";

  int iac, Nac=300, Nas=100; // it should be Nas=MaxNRetr in sizes.h
  int prev_act, next_act;
  string OutPhrase="", BestPhrase="";
  int BestDB=1000;

  ExecuteAct(annabell, Mon, START_ST_A, NULL_ACT, NULL_ACT);
  ExecuteAct(annabell, Mon, STORE_ST_A, NULL_ACT, NULL_ACT);
  for (int IterI=0; IterI<n_iter; IterI++) {
    ExecuteAct(annabell, Mon, START_ST_A, NULL_ACT, NULL_ACT);
    if (n_iter>1) {
      Display.Print("Trying n. " + toStr(IterI));
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
	  Display.Warning("next act not found with Nas=" + toStr(Nas) +
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
	  Display.Warning("next act not found with GB="
			  + toStr(annabell->ElActfSt->DB) + ". Exiting\n");
	  break;
	}
      }


      if (next_act==SNT_OUT) {//annabell->ElAct->Nr[SNT_OUT-1]->O>0.5) {
	int DB = annabell->ElActfSt->DB;
	if (OutPhrase!="") OutPhrase = OutPhrase + " ";
	OutPhrase = OutPhrase + SentenceOut(annabell, Mon);
	if (n_iter>1) cout << "DB: " << DB << endl;
	else Display.Print(".\n");
	if (DB<BestDB) {
	  BestDB = DB;
	  BestPhrase = OutPhrase;
	}
	OutPhrase="";
	break;
      }
      ExecuteAct(annabell, Mon, STORE_ST_A, NULL_ACT, next_act);
      if (annabell->OutFlag->Nr[0]->O>0.5) {
	Mon->GetPhM("OutPhB", annabell->OutPhB);
	Display.Print(" -> " + Mon->OutPhrase+"\n");
	if (OutPhrase!="") OutPhrase = OutPhrase + " ";
	OutPhrase = OutPhrase + Mon->OutPhrase;
      }
      //if (annabell->OutFlag->Nr[0]->O>0.5) {
      if (annabell->ElAct->Nr[CONTINUE-1]->O>0.5) {
	Display.Print(" ... ");
      }
      if (annabell->ElAct->Nr[DONE-1]->O>0.5) {
	int DB = annabell->ElActfSt->DB;
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

  annabell->SetMode(NULL_MODE);
  
  return BestPhrase;
}

int ttt=0;
int TargetExploration(Annabell *annabell, Monitor *Mon, string name, string target_phrase)
{
  int vin[PhSize];

  // temp1 start here
  annabell->EPhaseI->Clear();
  annabell->EPhaseI->Nr[ExplorationPhaseIdx]->O = 1;
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
    if (VerboseFlag) Mon->PrintRwdAct();
    annabell->StActRwdUpdate();
    if (VerboseFlag) {Mon->PrintElActFL(); Mon->PrintElAct();}

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
      ExplorationPhaseIdx = i;
      break;
    }
  }
  // temp2 end here

  return 0;
}

int ExplorationApprove(Annabell *annabell, Monitor *Mon)
{
  ExecuteAct(annabell, Mon, STORE_SAI, NULL_ACT, NULL_ACT);

  if (ExplorationPhaseIdx!=4) ExplorationPhaseIdx++;

  return 0;
}

int TargetExplorationTest(Annabell *annabell, Monitor *Mon, string name, string target_phrase)
{
  //cout << "\nExploration\n";
  annabell->SetMode(EXPLORE);

  ExecuteAct(annabell, Mon, NULL_ACT, NULL_ACT, NULL_ACT);
  if (name=="WGB" && (ExplorationPhaseIdx==3 || ExplorationPhaseIdx==4))
    ExplorationPhaseIdx = 1;
  int StartExplorationPhaseIdx = ExplorationPhaseIdx;
  if (ExplorationPhaseIdx==0) StartExplorationPhaseIdx = 1;
  int i = 0;
  while (i<TRYLIMIT) {
    if (ExplorationPhaseIdx==0) {
      ExecuteAct(annabell, Mon, START_ST_A, NULL_ACT, NULL_ACT);
      ExecuteAct(annabell, Mon, STORE_ST_A, NULL_ACT, W_FROM_IN);
      ExplorationApprove(annabell, Mon);
      ExplorationPhaseIdx=1;
    }
    if (ExplorationPhaseIdx==1) {
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
	ExplorationPhaseIdx = 2;
	continue;
      }
    }
    else if (ExplorationPhaseIdx==2) {
      ExecuteAct(annabell, Mon, STORE_ST_A, NULL_ACT, RETR_AS);
      if (name=="WGB") {
	ExplorationPhaseIdx = 1;
	continue;
      }
      else { // (name=="WkPhB")
	Mon->GetPhM(name, annabell->WkPhB);
	if (target_phrase==Mon->OutPhrase) break;
      }
    }
    else if (ExplorationPhaseIdx==3) {
      ExecuteAct(annabell, Mon, STORE_ST_A, NULL_ACT, GET_START_PH);
      Mon->GetPhM(name, annabell->WkPhB);
      if (target_phrase==Mon->OutPhrase) break;
    }
    else if (ExplorationPhaseIdx==4) {
      ExecuteAct(annabell, Mon, STORE_ST_A, NULL_ACT, GET_NEXT_PH);
      Mon->GetPhM(name, annabell->WkPhB);
      if (target_phrase==Mon->OutPhrase) break;
    }
    ExplorationPhaseIdx = StartExplorationPhaseIdx;
    if (ExplorationPhaseIdx==3) StartExplorationPhaseIdx=4;
    if (ExplorationPhaseIdx==4) StartExplorationPhaseIdx=1;
    ExecuteAct(annabell, Mon, RETR_SAI, NULL_ACT, NULL_ACT);
    ExecuteAct(annabell, Mon, RETR_ST, NULL_ACT, NULL_ACT);
    ExecuteAct(annabell, Mon, RETR_SAI, NULL_ACT, NULL_ACT);
    i++;
  }
  CheckTryLimit(i);
  cout << "Good answer after " << i << " iterations\n";
  ExplorationApprove(annabell, Mon);

  annabell->SetMode(NULL_MODE);

  return 0;
}

int SearchContext(Annabell *annabell, Monitor *Mon, string target_phrase)
{
  //cout << "\nSearch context\n";
  annabell->SetMode(EXPLORE);

  if (ExplorationPhaseIdx==0) {
    ExecuteAct(annabell, Mon, START_ST_A, NULL_ACT, NULL_ACT);
    ExecuteAct(annabell, Mon, STORE_ST_A, NULL_ACT, W_FROM_IN);
  }
  ExplorationApprove(annabell, Mon);
  ExecuteAct(annabell, Mon, STORE_ST_A, NULL_ACT, GET_START_PH);
  ExplorationPhaseIdx = 3;
  Mon->GetPhM("WkPhB", annabell->WkPhB);
  cout << " ... " << Mon->OutPhrase << endl;
  if (target_phrase!=Mon->OutPhrase) {
    ExplorationPhaseIdx = 4;
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
    Display.Print(target_phrase + "\n");
  }
    
  annabell->SetMode(NULL_MODE);

  return 0;
}

int ContinueSearchContext(Annabell *annabell, Monitor *Mon, string target_phrase)
{
  //cout << "\nSearch context\n";
  annabell->SetMode(EXPLORE);

  if (ExplorationPhaseIdx==0) {
    ExecuteAct(annabell, Mon, START_ST_A, NULL_ACT, NULL_ACT);
    ExecuteAct(annabell, Mon, STORE_ST_A, NULL_ACT, W_FROM_IN);
  }
  ExplorationApprove(annabell, Mon);

  ExplorationPhaseIdx = 4;
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
    Display.Print(target_phrase + "\n");
  }
    
  annabell->SetMode(NULL_MODE);

  return 0;
}

int WorkingPhraseOut(Annabell *annabell, Monitor *Mon)
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
  SetAct(annabell, STORE_ST_A, NULL_ACT, FLUSH_OUT);
  annabell->StActRwdUpdate();
  Mon->Print();
  annabell->Update();

  SetAct(annabell, STORE_ST_A, NULL_ACT, WG_OUT);
  annabell->StActRwdUpdate();
  Mon->Print();
  annabell->Update();

  ExplorationPhaseIdx = 2;
  ExplorationApprove(annabell, Mon);

  annabell->SetMode(NULL_MODE);

  return 0;
}

string SentenceOut(Annabell *annabell, Monitor *Mon)
{
  //cout << "\nSend sentence to output\n";
  string out_phrase = "";

  ExecuteAct(annabell, Mon, STORE_ST_A, NULL_ACT, SNT_OUT);
  if (annabell->ModeFlags->Nr[EXPLORE]->O>0.5) {
    ExplorationApprove(annabell, Mon);
    ExplorationPhaseIdx = 2;
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
    if (out_phrase!="") Display.Print(" ... ");
    Display.Print("-> " + Mon->OutPhrase+"\n");
    out_phrase = out_phrase + Mon->OutPhrase;
    ExecuteAct(annabell, Mon, NULL_ACT, NULL_ACT, GET_NEXT_PH);
    Mon->GetPhM("WkPhB", annabell->WkPhB);
  }
  //Display.Print("\n");

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
  ExplorationPhaseIdx=0;

  return 0;
}
