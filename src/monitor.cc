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
#include <stdio.h>
#include <stdlib.h>
#include "enum_ssm.h"
#include "monitor.h"

using namespace std;
using namespace sizes;

monitor::monitor(sllm *sllm1)
{
  SLLM = sllm1;
  for (int iw=0; iw<WMSize; iw++) wflag[iw]=0;
  for (int imode=0; imode<ModeNum+1; imode++) { 
    for (int iobj=0; iobj<ObjNum; iobj++) {
      ObjFlag[imode][iobj] = false;
    }
  }
  ModeChangeFlag = false;
  ModeMessageFlag = false;
  LastModeIdx = 0;
  Display.ConsoleFlag = true;
}

int monitor::MapW(char *w)
{
  int idx=-1;
  
  for (int jw=0; jw<WMSize; jw++) {
    if (SLLM->IW->Nr[jw]->O>0.5) {
      if (idx >= 0) {
	Display.Warning("Err >1 wnn!!");
	exit(0);
      }
      idx=jw;
    }
  }
  if (idx>=0 && wflag[idx]==0) {
    wflag[idx]=1;
    int ic;
    for (ic=0; ic<NC; ic++) {
      if (w[ic]==0) break;
      wlst[idx][ic] = w[ic];
    }
    for (;ic<NC; ic++) {
      wlst[idx][ic] = 0;
    }

  }
  
  return 0;
}

int monitor::PrintWM(string name, vssm *wm)
{
  int idx=-1;
  
  Display.Print(name + ": ");
  for (int jw=0; jw<WMSize; jw++) {
    if (wm->Nr[jw]->O>0.5) {
      if (idx >= 0) {
	Display.Warning("Err >1 wnn!!\n");
	Display.Warning(toStr(idx)+"\n");
	Display.Warning(toStr(jw)+"\n");
	return 1;
      }
      idx=jw;
    }
  }
  if (idx < 0) {
    Display.Warning("NO Wnn");
  }
  else if (wflag[idx]==0) {
    Display.Warning("Err >1 wnn!!\n");
    Display.Warning(toStr(idx)+"\n");
    return 1;
  }
  Display.Print(std::string(wlst[idx])+", "+toStr(idx)+"\n");
  
  return 0;
}

int monitor::PrintPhIM(string name, vssm *ph_i_m)
{ 
  Display.Print(name+": ");
  for (int iw=0; iw<PhSize; iw++) {
    if (ph_i_m->Nr[iw]->O>0.5) Display.Print("1");
    else Display.Print("0");
  }
  Display.Print("\n");

  return 0;
}

int monitor::PrintPhM(string name, vssm *ph_m)
{
  Display.Print(name+": ");
  for (int iy=0; iy<PhSize; iy++) {  
    int idx=-1;
    for (int ix=0; ix<WMSize; ix++) {
      if (ph_m->Nr[iy*WMSize+ix]->O > 0.5) {
	if (idx >= 0) {
	  Display.Warning("Err >1 wnn!!\n");
	  Display.Warning(toStr(idx)+"\n");
	  Display.Warning(toStr(ix)+"\n");
	  return 1;
	}
	idx=ix;
      }
    }
    if (idx < 0) {
      Display.Print("--- ");
    }
    else if (wflag[idx]==0) {
      Display.Warning("Err >1 wnn!!\n");
      return 1;
    }
    Display.Print(string(wlst[idx])+" ");
  }
  Display.Print("\n");

  return 0;
}

int monitor::PrintSSMidx(string name, vssm *ssm1)
{
  if (ssm1->NN()>1000 && ssm1->FillHighVect==false) {
    Display.Warning("FillHighVect=false for ssm with NN>1000\n");
  }
  Display.Print(name+":");
  if (ssm1->FillHighVect==false) {
    for (int i=0; i<ssm1->NN(); i++) {
      if (ssm1->Nr[i]->O>0.5) {
	Display.Print(" "+toStr(i));
      }
    }
    Display.Print("\n");
  }
  else {
    for(unsigned int i=0; i<ssm1->HighVect.size(); i++) {
      int inr = ssm1->HighVect[i];
      if (ssm1->Nr[inr]->O>0.5) {
	Display.Print(" "+toStr(inr));
      }
    }
    Display.Print("\n");
  }

  return 0;
}

int monitor::PrintSSM(string name, vssm *ssm1)
{
  Display.Print(name+":");

  for (int i=0; i<ssm1->NN(); i++) {
    Display.Print(" "+toStr(ssm1->Nr[i]->O));
  }
  Display.Print("\n");

  return 0;
}

int monitor::GetWM(string name, vssm *ph_m)
{
  int idx=-1;
  for (int ix=0; ix<WMSize; ix++) {
    if (ph_m->Nr[ix]->O > 0.5) {
      if (idx >= 0) {
	Display.Warning("Err >1 wnn!!\n");
	Display.Warning(toStr(idx)+"\n");
	Display.Warning(toStr(ix)+"\n");
	return 1;
      }
      idx=ix;
    }
  }
  if (idx < 0) {
    OutStr[0] = "";
  }
  else if (wflag[idx]==0) {
    Display.Warning("Err >1 wnn!!\n");
    return 1;
  }
  OutStr[0] = wlst[idx];

  return 0;
}

int monitor::GetPhM(string name, vssm *ph_m)
{
  for (int iy=0; iy<PhSize; iy++) {  
    int idx=-1;
    for (int ix=0; ix<WMSize; ix++) {
      if (ph_m->Nr[iy*WMSize+ix]->O > 0.5) {
	if (idx >= 0) {
	  Display.Warning("Err >1 wnn!!\n");
	  Display.Warning(toStr(idx)+"\n");
	  Display.Warning(toStr(ix)+"\n");
	  return 1;
	}
	idx=ix;
      }
    }
    if (idx < 0) {
      OutStr[iy] = "";
    }
    else if (wflag[idx]==0) {
      Display.Warning("Err >1 wnn!!\n");
      return 1;
    }
    else OutStr[iy] = wlst[idx];
  }
  OutPhrase = OutStr[0];
  for (int iy=1; iy<PhSize && OutStr[iy]!=""; iy++)
	 OutPhrase = OutPhrase + " " + OutStr[iy];

  return 0;
}

int monitor::PrintElAct()
{
  Display.Print("Next elaboration action: ");
  int na=-1;
  for (int ia=0; ia<ElActSize; ia++) {
    if (SLLM->ElAct->Nr[ia]->O>0.5) {
      Display.Print(ElActName[ia+1]+"\n");
      if (na>=0) {
	Display.Warning("Error: >1 elaboration action!!\n");
	return 1;
      }
      else na = ia;
    }
  }
  if (na<0) Display.Print("NULL_ACT\n");

  return 0;
}

int monitor::PrintElActFL()
{
  Display.Print("Proposed elaboration action: ");
  int na=-1;
  for (int ia=0; ia<ElActSize; ia++) {
    if (SLLM->ElActFL->Nr[ia]->O>0.5) {
      Display.Print(ElActName[ia+1]+"\n");
      if (na>=0) {
	Display.Warning("Error: >1 elaboration action!!\n");
	return 1;
      }
      else na = ia;
    }
  }
  if (na<0) Display.Print("NULL_ACT\n");

  return 0;
}

int monitor::PrintAcqAct()
{
  Display.Print("Next acquisition action: ");
  int na=-1;
  for (int ia=0; ia<AcqActSize; ia++) {
    if (SLLM->AcqAct->Nr[ia]->O>0.5) {
      Display.Print(AcqActName[ia+1]+"\n");
      if (na>=0) {
	Display.Warning("Error: >1 acquisition action!!\n");
	return 1;
      }
      else na = ia;
    }
  }
  if (na<0) Display.Print("NULL_ACT\n");

  return 0;
}

int monitor::PrintRwdAct()
{
  Display.Print("Reward action: ");
  int na=-1;
  for (int ia=0; ia<RwdActSize; ia++) {
    if (SLLM->RwdAct->Nr[ia]->O>0.5) {
      Display.Print(RwdActName[ia+1]+"\n");
      if (na>=0) {
	Display.Warning("Error: >1 reward action!!\n");
	return 1;
      }
      else na = ia;
    }
  }
  if (na<0) Display.Print("NULL_ACT\n");

  return 0;
}

int monitor::GetElAct()
{

  int na=-1;
  for (int ia=0; ia<ElActSize; ia++) {
    if (SLLM->ElAct->Nr[ia]->O>0.5) {
      if (na>=0) {
	Display.Warning("Err >1 elaboration action!!\n");
	return 1;
      }
      else na = ia;
    }
  }
  na++;

  return na;
}

int monitor::GetElActFL()
{

  int na=-1;
  for (int ia=0; ia<ElActSize; ia++) {
    if (SLLM->ElActFL->Nr[ia]->O>0.5) {
      if (na>=0) {
	Display.Warning("Err >1 proposed elaboration action!!\n");
	return 1;
      }
      else na = ia;
    }
  }
  na++;

  return na;
}

int monitor::ParseCommand(vector<string> input_token)
{
  Display.Print("Monitor: \n");
  if (input_token.size()>1) {
    string comm = input_token[1];
    if (comm=="console") {
      if (input_token.size()!=3) {
	Display.Warning("one argument (on/off) should be provided");
	return 1;
      }
      string val = input_token[2];
      if (val=="on") {
	Display.ConsoleFlag = true;
	return 0;
      }
      else if (val=="off") {
	Display.ConsoleFlag = false;
	return 0;
      }
      else {
	Display.Warning("Unrecognized on/off value.");
	return 1;
      }
    }
    else if (comm=="display") {
      string obj_name, mode_name, val;
      if (input_token.size()>2) {
	obj_name = input_token[2];
	if (obj_name=="mode_change" && input_token.size()==4) {
	  val = input_token[3];
	  if (val=="on") {
	    ModeChangeFlag = true;
	    return 0;
	  }
	  else if (val=="off") {
	    ModeChangeFlag = false;
	    return 0;
	  }
	  else {
	    Display.Warning("Unrecognized on/off value.");
	    return 1;
	  }
	}
	if (obj_name=="mode_message" && input_token.size()==4) {
	  val = input_token[3];
	  if (val=="on") {
	    ModeMessageFlag = true;
	    return 0;
	  }
	  else if (val=="off") {
	    ModeMessageFlag = false;
	    return 0;
	  }
	  else {
	    Display.Warning("Unrecognized on/off value.");
	    return 1;
	  }
	}
	else {
	  int iobj, imode;
	  for (iobj=0; iobj<ObjNum && ObjName[iobj]!=obj_name; iobj++);
	  if (iobj==ObjNum) {
	    Display.Warning("Unrecognized object name.");
	    return 1;
	  }
	  if (input_token.size()==5) {
	    mode_name = input_token[3];
	    val = input_token[4];
	    if (mode_name=="always") {
	      if (val=="on") {
		for (imode=0; imode<ModeNum+1; imode++)
		  ObjFlag[imode][iobj] = true;
		return 0;
	      }
	      else if (val=="off") {
		for (imode=0; imode<ModeNum+1; imode++)
		  ObjFlag[imode][iobj] = false;
		return 0;
	      }
	      else {
		Display.Warning("Unrecognized on/off value.");
		return 1;
	      }
	    }
	    else {
	      for (imode=0; imode<ModeNum+1 && ModeName[imode]!=mode_name;
		   imode++);
	      if (imode==ModeNum+1) {
		Display.Warning("Unrecognized operating mode name.");
		return 1;
	      }
	      if (val=="on") {
		ObjFlag[imode][iobj] = true;
		return 0;
	      }
	      else if (val=="off") {
		ObjFlag[imode][iobj] = false;
		return 0;
	      }
	      else {
		Display.Warning("Unrecognized on/off value.");
		return 1;
	      }
	    }
	  }
	}
      }
      else {
	Print();
	return 0;
      }
    }
  }

  Display.Warning("syntax error!");
  return 1;

}
int monitor::Print()
{
  int imode;
  for (imode=0; imode<ModeNum && SLLM->ModeFlags->Nr[imode]->O<0.5; imode++);
  if (imode==ModeNum) imode = 0;
  else imode++;
  if (imode!=LastModeIdx) {
    LastModeIdx = imode;
    if (ModeChangeFlag)
      Display.Print("Operating mode changed to: "+ModeName[imode]+"\n");
  }

  if (ObjFlag[imode][Header_idx])
    Display.Print("-------------------------------------------------------\n");

  if (ObjFlag[imode][IW_idx]) PrintWM("IW", SLLM->IW);
  if (ObjFlag[imode][CW_idx]) PrintWM("CW", SLLM->CW);
  if (ObjFlag[imode][PhI_idx]) PrintPhIM("PhI", SLLM->PhI);
  if (ObjFlag[imode][InFlag_idx]) PrintSSM("InFlag", SLLM->InFlag);
  if (ObjFlag[imode][FlushIn_idx]) PrintSSM("FlushIn", SLLM->FlushIn);
  if (ObjFlag[imode][InI_idx]) PrintPhIM("InI", SLLM->InI);
  if (ObjFlag[imode][InEqW_idx]) PrintPhIM("InEqW", SLLM->InEqW);
  if (ObjFlag[imode][InEqWC_idx]) PrintPhIM("InEqWC", SLLM->InEqWC);
  if (ObjFlag[imode][InPhFL_idx]) PrintPhM("InPhFL", SLLM->InPhFL);
  if (ObjFlag[imode][InPhB_idx]) PrintPhM("InPhB", SLLM->InPhB);
  //if (ObjFlag[imode][InWfI_idx]) PrintPhM("InWfI", SLLM->InWfI);
  if (ObjFlag[imode][InIfW_idx]) PrintPhM("InIfW", SLLM->InIfW);
  if (ObjFlag[imode][InIfWGW1_idx]) PrintPhM("InIfWGW1", SLLM->InIfWGW1);
  if (ObjFlag[imode][InIfWGW2_idx]) PrintPhM("InIfWGW2", SLLM->InIfWGW2);
  if (ObjFlag[imode][InIfWGW3_idx]) PrintPhM("InIfWGW3", SLLM->InIfWGW3);
  if (ObjFlag[imode][InIfWGW4_idx]) PrintPhM("InIfWGW4", SLLM->InIfWGW4);

  if (ObjFlag[imode][InEqWGW1_idx]) PrintPhIM("InEqWGW1", SLLM->InEqWGW1);
  if (ObjFlag[imode][InEqWGW2_idx]) PrintPhIM("InEqWGW2", SLLM->InEqWGW2);
  if (ObjFlag[imode][InEqWGW3_idx]) PrintPhIM("InEqWGW3", SLLM->InEqWGW3);
  if (ObjFlag[imode][InEqWGW4_idx]) PrintPhIM("InEqWGW4", SLLM->InEqWGW4);
  if (ObjFlag[imode][InEqWGW1C_idx]) PrintPhIM("InEqWGW1C", SLLM->InEqWGW1C);
  if (ObjFlag[imode][InEqWGW2C_idx]) PrintPhIM("InEqWGW2C", SLLM->InEqWGW2C);
  if (ObjFlag[imode][InEqWGW3C_idx]) PrintPhIM("InEqWGW3C", SLLM->InEqWGW3C);
  if (ObjFlag[imode][InEqWGW4C_idx]) PrintPhIM("InEqWGW4C", SLLM->InEqWGW4C);
  if (ObjFlag[imode][PrevInEqWGW1FL_idx])
    PrintPhIM("PrevInEqWGW1FL", SLLM->PrevInEqWGW1FL);
  if (ObjFlag[imode][PrevInEqWGW2FL_idx])
    PrintPhIM("PrevInEqWGW2FL", SLLM->PrevInEqWGW2FL);
  if (ObjFlag[imode][PrevInEqWGW3FL_idx])
    PrintPhIM("PrevInEqWGW3FL", SLLM->PrevInEqWGW3FL);
  if (ObjFlag[imode][PrevInEqWGW4FL_idx])
    PrintPhIM("PrevInEqWGW4FL", SLLM->PrevInEqWGW4FL);
  if (ObjFlag[imode][PrevInEqWGW1_idx])
    PrintPhIM("PrevInEqWGW1", SLLM->PrevInEqWGW1);
  if (ObjFlag[imode][PrevInEqWGW2_idx])
    PrintPhIM("PrevInEqWGW2", SLLM->PrevInEqWGW2);
  if (ObjFlag[imode][PrevInEqWGW3_idx])
    PrintPhIM("PrevInEqWGW3", SLLM->PrevInEqWGW3);
  if (ObjFlag[imode][PrevInEqWGW4_idx])
    PrintPhIM("PrevInEqWGW4", SLLM->PrevInEqWGW4);
  if (ObjFlag[imode][PrevInEqWGW1C_idx])
    PrintPhIM("PrevInEqWGW1C", SLLM->PrevInEqWGW1C);
  if (ObjFlag[imode][PrevInEqWGW2C_idx])
    PrintPhIM("PrevInEqWGW2C", SLLM->PrevInEqWGW2C);
  if (ObjFlag[imode][PrevInEqWGW3C_idx])
    PrintPhIM("PrevInEqWGW3C", SLLM->PrevInEqWGW3C);
  if (ObjFlag[imode][PrevInEqWGW4C_idx])
    PrintPhIM("PrevInEqWGW4C", SLLM->PrevInEqWGW4C);


  if (ObjFlag[imode][WkFlag_idx]) PrintSSM("WkFlag", SLLM->WkFlag);
  if (ObjFlag[imode][FlushWk_idx]) PrintSSM("FlushWk", SLLM->FlushWk);
  //  if (ObjFlag[imode][WkI_idx]) PrintPhIM("WkI", SLLM->WkI);
  if (ObjFlag[imode][WkEqW_idx]) PrintPhIM("WkEqW", SLLM->WkEqW);
  if (ObjFlag[imode][WkEqWC_idx]) PrintPhIM("WkEqWC", SLLM->WkEqWC);
  if (ObjFlag[imode][WkPhFL_idx]) PrintPhM("WkPhFL", SLLM->WkPhFL);
  if (ObjFlag[imode][WkPhB_idx]) PrintPhM("WkPhB", SLLM->WkPhB);
  if (ObjFlag[imode][WkWfI_idx]) PrintPhM("WkWfI", SLLM->WkWfI);
  if (ObjFlag[imode][WkIfW_idx]) PrintPhM("WkIfW", SLLM->WkIfW);
  if (ObjFlag[imode][WkIfWGW1_idx]) PrintPhM("WkIfWGW1", SLLM->WkIfWGW1);
  if (ObjFlag[imode][WkIfWGW2_idx]) PrintPhM("WkIfWGW2", SLLM->WkIfWGW2);
  if (ObjFlag[imode][WkIfWGW3_idx]) PrintPhM("WkIfWGW3", SLLM->WkIfWGW3);
  if (ObjFlag[imode][WkIfWGW4_idx]) PrintPhM("WkIfWGW4", SLLM->WkIfWGW4);
  if (ObjFlag[imode][WkEqWGW1_idx]) PrintPhIM("WkEqWGW1", SLLM->WkEqWGW1);
  if (ObjFlag[imode][WkEqWGW2_idx]) PrintPhIM("WkEqWGW2", SLLM->WkEqWGW2);
  if (ObjFlag[imode][WkEqWGW3_idx]) PrintPhIM("WkEqWGW3", SLLM->WkEqWGW3);
  if (ObjFlag[imode][WkEqWGW4_idx]) PrintPhIM("WkEqWGW4", SLLM->WkEqWGW4);
  if (ObjFlag[imode][WkEqWGW1C_idx]) PrintPhIM("WkEqWGW1C", SLLM->WkEqWGW1C);
  if (ObjFlag[imode][WkEqWGW2C_idx]) PrintPhIM("WkEqWGW2C", SLLM->WkEqWGW2C);
  if (ObjFlag[imode][WkEqWGW3C_idx]) PrintPhIM("WkEqWGW3C", SLLM->WkEqWGW3C);
  if (ObjFlag[imode][WkEqWGW4C_idx]) PrintPhIM("WkEqWGW4C", SLLM->WkEqWGW4C);
  if (ObjFlag[imode][PrevWkPhFL_idx]) PrintPhM("PrevWkPhFL", SLLM->PrevWkPhFL);
  if (ObjFlag[imode][PrevWkPh_idx]) PrintPhM("PrevWkPh", SLLM->PrevWkPh);
  if (ObjFlag[imode][PrevWkEqWGW1FL_idx])
    PrintPhIM("PrevWkEqWGW1FL", SLLM->PrevWkEqWGW1FL);
  if (ObjFlag[imode][PrevWkEqWGW2FL_idx])
    PrintPhIM("PrevWkEqWGW2FL", SLLM->PrevWkEqWGW2FL);
  if (ObjFlag[imode][PrevWkEqWGW3FL_idx])
    PrintPhIM("PrevWkEqWGW3FL", SLLM->PrevWkEqWGW3FL);
  if (ObjFlag[imode][PrevWkEqWGW4FL_idx])
    PrintPhIM("PrevWkEqWGW4FL", SLLM->PrevWkEqWGW4FL);
  if (ObjFlag[imode][PrevWkEqWGW1_idx])
    PrintPhIM("PrevWkEqWGW1", SLLM->PrevWkEqWGW1);
  if (ObjFlag[imode][PrevWkEqWGW2_idx])
    PrintPhIM("PrevWkEqWGW2", SLLM->PrevWkEqWGW2);
  if (ObjFlag[imode][PrevWkEqWGW3_idx])
    PrintPhIM("PrevWkEqWGW3", SLLM->PrevWkEqWGW3);
  if (ObjFlag[imode][PrevWkEqWGW4_idx])
    PrintPhIM("PrevWkEqWGW4", SLLM->PrevWkEqWGW4);
  if (ObjFlag[imode][PrevWkEqWGW1C_idx])
    PrintPhIM("PrevWkEqWGW1C", SLLM->PrevWkEqWGW1C);
  if (ObjFlag[imode][PrevWkEqWGW2C_idx])
    PrintPhIM("PrevWkEqWGW2C", SLLM->PrevWkEqWGW2C);
  if (ObjFlag[imode][PrevWkEqWGW3C_idx])
    PrintPhIM("PrevWkEqWGW3C", SLLM->PrevWkEqWGW3C);
  if (ObjFlag[imode][PrevWkEqWGW4C_idx])
    PrintPhIM("PrevWkEqWGW4C", SLLM->PrevWkEqWGW4C);

  if (ObjFlag[imode][OutFlag_idx]) PrintSSM("OutFlag", SLLM->OutFlag);
  if (ObjFlag[imode][FlushOut_idx]) PrintSSM("FlushOut", SLLM->FlushOut);
  if (ObjFlag[imode][OutEqW_idx]) PrintPhIM("OutEqW", SLLM->OutEqW);
  //if (ObjFlag[imode][OutEqWC_idx]) PrintPhIM("OutEqWC", SLLM->OutEqWC);
  if (ObjFlag[imode][OutPhFL_idx]) PrintPhM("OutPhFL", SLLM->OutPhFL);
  if (ObjFlag[imode][OutPhB_idx]) PrintPhM("OutPhB", SLLM->OutPhB);
  if (ObjFlag[imode][OutIfW_idx]) PrintPhM("OutIfW", SLLM->OutIfW);

  if (ObjFlag[imode][GetFlag_idx]) PrintSSM("GetFlag", SLLM->GetFlag);
  if (ObjFlag[imode][WGI_idx]) PrintPhIM("WGI", SLLM->WGI);
  if (ObjFlag[imode][WGCW_idx]) PrintWM("WGCW", SLLM->WGCW);
  if (ObjFlag[imode][WGFlag_idx]) PrintSSM("WGFlag", SLLM->WGFlag);
  if (ObjFlag[imode][FlushWG_idx]) PrintSSM("FlushWG", SLLM->FlushWG);
  if (ObjFlag[imode][WGIFL_idx]) PrintPhIM("WGIFL", SLLM->WGIFL);
  if (ObjFlag[imode][WGEqW_idx]) PrintPhIM("WGEqW", SLLM->WGEqW);
  if (ObjFlag[imode][WGEqWC_idx]) PrintPhIM("WGEqWC", SLLM->WGEqWC);
  if (ObjFlag[imode][WGFL_idx]) PrintPhM("WGFL", SLLM->WGFL);
  if (ObjFlag[imode][WGB_idx]) PrintPhM("WGB", SLLM->WGB);
  if (ObjFlag[imode][WGIfW_idx]) PrintPhM("WGIfW", SLLM->WGIfW);
  if (ObjFlag[imode][PrevWGFL_idx]) PrintPhM("PrevWGFL", SLLM->PrevWGFL);
  if (ObjFlag[imode][PrevWG_idx]) PrintPhM("PrevWG", SLLM->PrevWG);

  if (ObjFlag[imode][RetrAs_idx]) PrintSSM("RetrAs", SLLM->RetrAs);
  if (ObjFlag[imode][BuildAs_idx]) PrintSSM("BuildAs", SLLM->BuildAs);

  if (ObjFlag[imode][StartStActIFlag_idx])
    PrintSSM("StartStActIFlag", SLLM->StartStActIFlag);
  if (ObjFlag[imode][CurrStActI_idx])
    PrintSSMidx("CurrStActI", SLLM->CurrStActI);
  if (ObjFlag[imode][CurrStActIFlag_idx])
    PrintSSM("CurrStActIFlag", SLLM->CurrStActIFlag);
  if (ObjFlag[imode][NextStActI_idx])
    PrintSSMidx("NextStActI", SLLM->NextStActI);
  if (ObjFlag[imode][NextStActIFlag_idx])
    PrintSSM("NextStActIFlag", SLLM->NextStActIFlag);
  if (ObjFlag[imode][StActI_idx])
    PrintSSMidx("StActI", SLLM->StActI);

  if (ObjFlag[imode][RetrStAct_idx])
    PrintSSM("RetrStAct", SLLM->RetrStAct);
  if (ObjFlag[imode][BuildStAct_idx])
    PrintSSM("BuildStAct", SLLM->BuildStAct);
  if (ObjFlag[imode][StActMem_idx])
    PrintSSMidx("StActMem", SLLM->StActMem);
  //if (ObjFlag[imode][ElAct_idx]) PrintElAct();
  //if (ObjFlag[imode][AcqAct_idx]) PrintSSMidx("AcqAct", SLLM->AcqAct);
  if (ObjFlag[imode][ActFlags_idx])
    PrintSSM("ActFlags", SLLM->ActFlags);
  if (ObjFlag[imode][ActFlags_idx])
    PrintSSM("ActFlagsC", SLLM->ActFlags);

  if (ObjFlag[imode][CurrPhI_idx]) PrintPhIM("CurrPhI", SLLM->CurrPhI);
  if (ObjFlag[imode][CurrPhIFlag_idx])
    PrintSSM("CurrPhIFlag", SLLM->CurrPhIFlag);
  if (ObjFlag[imode][NextPhI_idx]) PrintPhIM("NextPhI", SLLM->NextPhI);
  if (ObjFlag[imode][NextPhIFlag_idx])
    PrintSSM("NextPhIFlag", SLLM->NextPhIFlag);
  if (ObjFlag[imode][CurrWGI_idx]) PrintPhIM("CurrWGI", SLLM->CurrWGI);
  if (ObjFlag[imode][CurrWGIFlag_idx])
    PrintSSM("CurrWGIFlag", SLLM->CurrWGIFlag);
  if (ObjFlag[imode][NextWGI_idx]) PrintPhIM("NextWGI", SLLM->NextWGI);
  if (ObjFlag[imode][NextWGIFlag_idx])
    PrintSSM("NextWGIFlag", SLLM->NextWGIFlag);

  if (ObjFlag[imode][RetrElActfSt_idx])
    PrintSSM("RetrElActfSt", SLLM->RetrElActfSt);
  if (ObjFlag[imode][BuildElActfSt_idx])
    PrintSSM("BuildElActfSt", SLLM->BuildElActfSt);
  if (ObjFlag[imode][RewdElActfSt_idx])
    PrintSSM("RewdElActfSt", SLLM->RewdElActfSt);
  if (ObjFlag[imode][ElActfSt_idx]) PrintSSMidx("ElActfSt", SLLM->ElActfSt);

  if (ObjFlag[imode][CurrMemPh_idx]) PrintSSMidx("CurrMemPh", SLLM->CurrMemPh);
  if (ObjFlag[imode][NextMemPh_idx]) PrintSSMidx("NextMemPh", SLLM->NextMemPh);
  if (ObjFlag[imode][NewMemPhFlag_idx])
    PrintSSM("NewMemPhFlag", SLLM->NewMemPhFlag);
  if (ObjFlag[imode][MemPh_idx]) PrintSSMidx("MemPh", SLLM->MemPh); 

  if (ObjFlag[imode][RetrRemPh_idx]) PrintSSM("RetrRemPh", SLLM->RetrRemPh);
  if (ObjFlag[imode][RemPhIL_idx]) PrintSSMidx("RemPhIL", SLLM->RemPhIL);

  if (ObjFlag[imode][CurrRemPh_idx]) PrintSSMidx("CurrRemPh", SLLM->CurrRemPh);
  if (ObjFlag[imode][CurrRemPhFlag_idx])
    PrintSSM("CurrRemPhFlag", SLLM->CurrRemPhFlag);
  if (ObjFlag[imode][NextRemPh_idx]) PrintSSMidx("NextRemPh", SLLM->NextRemPh);
  if (ObjFlag[imode][NextRemPhFlag_idx])
    PrintSSM("NextRemPhFlag", SLLM->NextRemPhFlag);
  if (ObjFlag[imode][RemPh_idx]) PrintSSMidx("RemPh", SLLM->RemPh); 
  if (ObjFlag[imode][RemPhfWG_idx]) PrintSSMidx("RemPhfWG", SLLM->RemPhfWG); 

  if (ObjFlag[imode][StartPhFlag_idx])
    PrintSSM("StartPhFlag", SLLM->StartPhFlag);
  if (ObjFlag[imode][StartPhIL_idx]) PrintSSMidx("StartPhIL", SLLM->StartPhIL); 
  if (ObjFlag[imode][StartPh_idx]) PrintSSMidx("StartPh", SLLM->StartPh); 
  if (ObjFlag[imode][CurrStartPh_idx])
    PrintSSMidx("CurrStartPh", SLLM->CurrStartPh); 

  if (ObjFlag[imode][RemStartPhFlag_idx])
    PrintSSM("RemStartPhFlag", SLLM->RemStartPhFlag);
  if (ObjFlag[imode][CurrStartWkPhFlag_idx])
    PrintSSM("CurrStartWkPhFlag", SLLM->CurrStartWkPhFlag);
  if (ObjFlag[imode][StartWkPhIL_idx])
    PrintSSMidx("StartWkPhIL", SLLM->StartWkPhIL);
  if (ObjFlag[imode][CurrStartWkPh_idx])
    PrintSSMidx("CurrStartWkPh", SLLM->CurrStartWkPh);
  if (ObjFlag[imode][StartWkPh_idx]) PrintSSMidx("StartWkPh", SLLM->StartWkPh);
  if (ObjFlag[imode][RemStartPh_idx])
    PrintSSMidx("RemStartPh", SLLM->RemStartPh);  

  if (ObjFlag[imode][ModeFlags_idx]) PrintSSM("ModeFlags", SLLM->ModeFlags);
  if (ObjFlag[imode][Acquire_idx]) PrintSSM("Acquire", SLLM->Acquire);
  if (ObjFlag[imode][Associate_idx]) PrintSSM("Associate", SLLM->Associate);
  if (ObjFlag[imode][Explore_idx]) PrintSSM("Explore", SLLM->Explore);
  if (ObjFlag[imode][Explore2_idx]) PrintSSM("Explore2", SLLM->Explore2);
  if (ObjFlag[imode][Exploit_idx]) PrintSSM("Exploit", SLLM->Exploit);
  if (ObjFlag[imode][BestExploit_idx])
    PrintSSM("BestExploit", SLLM->BestExploit);
  if (ObjFlag[imode][Reward_idx]) PrintSSM("Reward", SLLM->Reward);

  if (ObjFlag[imode][SetGoalFlag_idx])
    PrintSSM("SetGoalFlag", SLLM->SetGoalFlag);
  if (ObjFlag[imode][GetGoalFlag_idx])
    PrintSSM("GetGoalFlag", SLLM->GetGoalFlag);
  if (ObjFlag[imode][FlushGoal_idx])
    PrintSSM("FlushGoal", SLLM->FlushGoal);

  if (ObjFlag[imode][GoalWGFL_idx]) PrintPhM("GoalWGFL", SLLM->GoalWGFL);
  if (ObjFlag[imode][GoalWG_idx]) PrintPhM("GoalWG", SLLM->GoalWG);
  if (ObjFlag[imode][CurrGoalWG_idx]) PrintPhM("CurrGoalWG", SLLM->CurrGoalWG);
  if (ObjFlag[imode][GoalPhFL_idx]) PrintPhM("GoalPhFL", SLLM->GoalPhFL);
  if (ObjFlag[imode][GoalPh_idx]) PrintPhM("GoalPh", SLLM->GoalPh);
  if (ObjFlag[imode][CurrGoalPh_idx]) PrintPhM("CurrGoalPh", SLLM->CurrGoalPh);
  if (ObjFlag[imode][GetGoalPhFL_idx])
    PrintPhM("GetGoalPhFL", SLLM->GetGoalPhFL);

  if (ObjFlag[imode][WkIfGoalWGW1_idx])
    PrintPhM("WkIfGoalWGW1", SLLM->WkIfGoalWGW1);
  if (ObjFlag[imode][WkIfGoalWGW2_idx])
    PrintPhM("WkIfGoalWGW2", SLLM->WkIfGoalWGW2);
  if (ObjFlag[imode][WkIfGoalWGW3_idx])
    PrintPhM("WkIfGoalWGW3", SLLM->WkIfGoalWGW3);
  if (ObjFlag[imode][WkIfGoalWGW4_idx])
    PrintPhM("WkIfGoalWGW4", SLLM->WkIfGoalWGW4);
  if (ObjFlag[imode][WkEqGoalWGW1_idx])
    PrintPhIM("WkEqGoalWGW1", SLLM->WkEqGoalWGW1);
  if (ObjFlag[imode][WkEqGoalWGW2_idx])
    PrintPhIM("WkEqGoalWGW2", SLLM->WkEqGoalWGW2);
  if (ObjFlag[imode][WkEqGoalWGW3_idx])
    PrintPhIM("WkEqGoalWGW3", SLLM->WkEqGoalWGW3);
  if (ObjFlag[imode][WkEqGoalWGW4_idx])
    PrintPhIM("WkEqGoalWGW4", SLLM->WkEqGoalWGW4);
  if (ObjFlag[imode][WkEqGoalWGW1C_idx])
    PrintPhIM("WkEqGoalWGW1C", SLLM->WkEqGoalWGW1C);
  if (ObjFlag[imode][WkEqGoalWGW2C_idx])
    PrintPhIM("WkEqGoalWGW2C", SLLM->WkEqGoalWGW2C);
  if (ObjFlag[imode][WkEqGoalWGW3C_idx])
    PrintPhIM("WkEqGoalWGW3C", SLLM->WkEqGoalWGW3C);
  if (ObjFlag[imode][WkEqGoalWGW4C_idx])
    PrintPhIM("WkEqGoalWGW4C", SLLM->WkEqGoalWGW4C);


  if (ObjFlag[imode][GoalI_idx]) PrintSSM("GoalI", SLLM->GoalI);
  if (ObjFlag[imode][CurrGoalI_idx]) PrintSSM("CurrGoalI", SLLM->CurrGoalI);
  if (ObjFlag[imode][NextGoalI_idx]) PrintSSM("NextGoalI", SLLM->NextGoalI);
  if (ObjFlag[imode][PrevGoalI_idx]) PrintSSM("PrevGoalI", SLLM->PrevGoalI);

  if (ObjFlag[imode][StartGoalIFlag_idx])
    PrintSSM("StartGoalIFlag", SLLM->StartGoalIFlag);
  if (ObjFlag[imode][CurrGoalIFlag_idx])
    PrintSSM("CurrGoalIFlag", SLLM->CurrGoalIFlag);
  if (ObjFlag[imode][NextGoalIFlag_idx])
    PrintSSM("NextGoalIFlag", SLLM->NextGoalIFlag);
  if (ObjFlag[imode][PrevGoalIFlag_idx])
    PrintSSM("PrevGoalIFlag", SLLM->PrevGoalIFlag);

  if (ObjFlag[imode][RetrGoal_idx])
    PrintSSM("RetrGoal", SLLM->RetrGoal);
  if (ObjFlag[imode][BuildGoal_idx])
    PrintSSM("BuildGoal", SLLM->BuildGoal);
  if (ObjFlag[imode][GoalMem_idx])
    PrintSSM("GoalMem", SLLM->GoalMem);

  if (ObjFlag[imode][AcqAct_idx]) PrintAcqAct();
  if (ObjFlag[imode][ElAct_idx]) PrintElAct();
  if (ObjFlag[imode][Header_idx])
    Display.Print("-------------------------------------------------------\n");

  return 0;
}

int monitor::ModeMessage(std::string msg)
{
  if (ModeMessageFlag) {
    Display.Print("-------------------------------------------------------\n");
    Display.Print(msg);
  }

  return 0;
}

const std::string monitor::ObjName[] = {
  "Header",
  "IW","CW","PhI","InFlag","FlushIn","InI","InEqW","InEqWC",
  "InPhFL","InPhB","InIfW",
  "InIfWGW1","InIfWGW2","InIfWGW3","InIfWGW4",
  "InEqWGW1","InEqWGW2","InEqWGW3","InEqWGW4",
  "InEqWGW1C","InEqWGW2C","InEqWGW3C","InEqWGW4C",
  "PrevInEqWGW1FL","PrevInEqWGW2FL","PrevInEqWGW3FL","PrevInEqWGW4FL",
  "PrevInEqWGW1","PrevInEqWGW2","PrevInEqWGW3","PrevInEqWGW4",
  "PrevInEqWGW1C","PrevInEqWGW2C","PrevInEqWGW3C","PrevInEqWGW4C",

  "WkFlag","FlushWk","WkEqW","WkEqWC",
  "WkPhFL","WkPhB","WkIfW","WkWfI",
  "WkIfWGW1","WkIfWGW2","WkIfWGW3","WkIfWGW4",
  "WkEqWGW1","WkEqWGW2","WkEqWGW3","WkEqWGW4",
  "WkEqWGW1C","WkEqWGW2C","WkEqWGW3C","WkEqWGW4C",
  "PrevWkPhFL","PrevWkPh",
  "PrevWkEqWGW1FL","PrevWkEqWGW2FL","PrevWkEqWGW3FL","PrevWkEqWGW4FL",
  "PrevWkEqWGW1","PrevWkEqWGW2","PrevWkEqWGW3","PrevWkEqWGW4",
  "PrevWkEqWGW1C","PrevWkEqWGW2C","PrevWkEqWGW3C","PrevWkEqWGW4C",

  "OutFlag","FlushOut","OutEqW","OutPhFL","OutPhB","OutIfW",

  "GetFlag","WGI","WGCW","WGFlag","FlushWG",
  "WGIFL","WGEqW","WGEqWC","WGFL","WGB","WGIfW",
  "PrevWGFL","PrevWG",

  "RetrAs","BuildAs",

  "StartStActIFlag","CurrStActI","CurrStActIFlag",
  "NextStActI","NextStActIFlag","StActI",
  "RetrStAct","BuildStAct","StActMem",

  "ElAct","AcqAct","ActFlags","ActFlagsC",

  "CurrPhI","CurrPhIFlag","NextPhI","NextPhIFlag",
  "CurrWGI","CurrWGIFlag","NextWGI","NextWGIFlag",

  "RetrElActfSt","BuildElActfSt","RewdElActfSt","ElActfSt",

  "CurrMemPh","NextMemPh","NewMemPhFlag","MemPh",
  "RetrRemPh","RemPhIL","CurrRemPh","CurrRemPhFlag",
  "NextRemPh","NextRemPhFlag","RemPh","RemPhfWG",
  "StartPhFlag","StartPhIL","StartPh","CurrStartPh",
  "RemStartPhFlag","CurrStartWkPhFlag","StartWkPhIL",
  "CurrStartWkPh","StartWkPh","RemStartPh",

  "ModeFlags","Acquire","Associate","Explore","Explore2",
  "Exploit","BestExploit","Reward",

  "SetGoalFlag", "GetGoalFlag", "FlushGoal",
  "GoalWGFL", "GoalWG", "CurrGoalWG",
  "GoalPhFL", "GoalPh", "CurrGoalPh", "GetGoalPhFL",

  "WkIfGoalWGW1", "WkIfGoalWGW2", "WkIfGoalWGW3", "WkIfGoalWGW4",
  "WkEqGoalWGW1", "WkEqGoalWGW2", "WkEqGoalWGW3", "WkEqGoalWGW4",
  "WkEqGoalWGW1C", "WkEqGoalWGW2C", "WkEqGoalWGW3C", "WkEqGoalWGW4C",

  "GoalI", "CurrGoalI", "NextGoalI", "PrevGoalI",
  "StartGoalIFlag", "CurrGoalIFlag", "NextGoalIFlag", "PrevGoalIFlag",

  "RetrGoal", "BuildGoal", "GoalMem"
};

const std::string monitor::ModeName[] = {
  "null_mode", "acquire", "associate", "explore", "explore2", "exploit",
  "best_exploit", "reward"
};

const std::string monitor::ElActName[ElActSize+1]={
  "NULL_ACT", "FLUSH_WG", "W_FROM_WK", "W_FROM_IN", "NEXT_W", "GET_W",
  "RETR_AS", "FLUSH_OUT", "WG_OUT", "GET_NEXT_PH", "GET_START_PH",
  "CONTINUE", "DONE", "PUSH_GL", "DROP_GL", "GET_GL_PH",
  "EL_ACT_15", "EL_ACT_16", "EL_ACT_17", "EL_ACT_18", "EL_ACT_19"
};

const std::string monitor::AcqActName[AcqActSize+1]={
  "NULL_ACT", "FLUSH", "ACQ_W", "NEXT_AS_W", "BUILD_AS", "MEM_PH",
  "SET_START_PH"
};

const std::string monitor::RwdActName[RwdActSize+1]={
  "NULL_RWD", "STORE_ST_A", "START_ST_A", "RETR_ST_A", "RWD_ST_A",
  "CHANGE_ST_A", "GET_ST_A", "RETR_EL_A", "STORE_SAI", "RETR_SAI", "RETR_ST",
  "RWD_ACT_10", "RWD_ACT_11", "RWD_ACT_12", "RWD_ACT_13", "RWD_ACT_14"
};

