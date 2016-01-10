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

#ifndef INTERFACEH
#define INTERFACEH
#include "Annabell.h"
#include "Monitor.h"
#include "sizes.h"

class interface
{
 public:
  static int w2bin(char *w, int *v);
  static int i2bin(int i, int *v, int size);
  static int bin2i(int *v, int size);
  static int i2v(int i, int *v, int size);
  static int v2i(int *v, int size);
};

extern const int TRYLIMIT;
extern int LastGB;
extern int ExplorationPhaseIdx;
extern int StoredStActI;
extern bool VerboseFlag;
extern bool StartContextFlag;

int CheckTryLimit(int i);
int SetAct(Annabell *annabell, int acq_act, int el_act);
int SetAct(Annabell *annabell, int rwd_act, int acq_act, int el_act);
int ExecuteAct(Annabell *annabell, Monitor *Mon, int rwd_act, int acq_act, int el_act);
int GetInputPhrase(Annabell *annabell, Monitor *Mon, std::string input_phrase);


#endif
