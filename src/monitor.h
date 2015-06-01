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

#ifndef MONITORH
#define MONITORH
#include <string>
#include <sstream>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include "sizes.h"
#include "sllm.h"
#include "display.h"

class monitor
{
 public:
  display Display;
  static const int ObjNum = 179;
  static const std::string ObjName[ObjNum];
  static const std::string ModeName[ModeNum+1];
  static const std::string ElActName[ElActSize+1];
  static const std::string AcqActName[AcqActSize+1];
  static const std::string RwdActName[RwdActSize+1];
  bool ObjFlag[ModeNum+1][ObjNum];
  bool ModeChangeFlag;
  bool ModeMessageFlag;
  int LastModeIdx;
  int wflag[sizes::WMSize];
  char wlst[sizes::WMSize][sizes::NC];
  sllm *SLLM;
  std::string OutStr[sizes::PhSize];
  std::string OutPhrase;

  monitor(sllm *sllm1);

  int MapW(char *w);
  int PrintWM(std::string name, vssm *wm);
  int PrintPhIM(std::string name, vssm *ph_i_m);
  int PrintPhM(std::string name, vssm *ph_m);
  int PrintSSMidx(std::string name, vssm *ssm1);
  int PrintSSM(std::string name, vssm *ssm1);
  int GetWM(std::string name, vssm *ph_m);
  int GetPhM(std::string name, vssm *ph_m);
  int PrintElAct();
  int PrintElActFL();
  int PrintAcqAct();
  int PrintRwdAct();
  int GetElAct();
  int GetElActFL();
  int ParseCommand(std::vector<std::string> input_token);
  int ModeMessage(std::string msg);
  int Print();
  int SaveWM(FILE *fp);
  int LoadWM(FILE *fp);

};

#endif
