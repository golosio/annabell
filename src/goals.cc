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
#include "sllm.h"
#include "sizes.h"
#include "interface.h"

using namespace std;
using namespace sizes;

int sllm::GoalArch()
{
  SetGoalFlag = new ssm(1);
  GetGoalFlag = new ssm(1);
  FlushGoal = new ssm(1);
  GoalWGFL = new XSSM2d(WMSize,PhSize);
  GoalWG = new XSSM2d(WMSize,PhSize);
  GoalWG->UseRowDefault();
  CurrGoalWG = new XSSM2d(WMSize,PhSize);
  GoalPhFL = new XSSM2d(WMSize,PhSize);
  GoalPh = new XSSM2d(WMSize,PhSize);
  GoalPh->UseRowDefault();
  CurrGoalPh = new XSSM2d(WMSize,PhSize);
  GetGoalPhFL = new XSSM2d(WMSize,PhSize);

  WkIfGoalWGW1 = new XSSM2d(WMSize,PhSize);
  WkIfGoalWGW2 = new XSSM2d(WMSize,PhSize);
  WkIfGoalWGW3 = new XSSM2d(WMSize,PhSize);
  WkIfGoalWGW4 = new XSSM2d(WMSize,PhSize);
  WkEqGoalWGW1 = new XSSM(PhSize);
  WkEqGoalWGW2 = new XSSM(PhSize);
  WkEqGoalWGW3 = new XSSM(PhSize);
  WkEqGoalWGW4 = new XSSM(PhSize);
  WkEqGoalWGW1C = new ssm(PhSize);
  WkEqGoalWGW2C = new ssm(PhSize);
  WkEqGoalWGW3C = new ssm(PhSize);
  WkEqGoalWGW4C = new ssm(PhSize);

  GoalPhIfGoalWGW1 = new XSSM2d(WMSize,PhSize);
  GoalPhIfGoalWGW2 = new XSSM2d(WMSize,PhSize);
  GoalPhIfGoalWGW3 = new XSSM2d(WMSize,PhSize);
  GoalPhIfGoalWGW4 = new XSSM2d(WMSize,PhSize);
  GoalPhEqGoalWGW1 = new XSSM(PhSize);
  GoalPhEqGoalWGW2 = new XSSM(PhSize);
  GoalPhEqGoalWGW3 = new XSSM(PhSize);
  GoalPhEqGoalWGW4 = new XSSM(PhSize);
  GoalPhEqGoalWGW1C = new ssm(PhSize);
  GoalPhEqGoalWGW2C = new ssm(PhSize);
  GoalPhEqGoalWGW3C = new ssm(PhSize);
  GoalPhEqGoalWGW4C = new ssm(PhSize);


  GoalWGFL->SC(WGB, 1);
  GoalWGFL->FC(SetGoalFlag, 1);
  GoalWGFL->SB(-1.5);
  GoalWG->SC(CurrGoalWG, 1);
  GoalWG->SC(GoalWGFL, 1);
  GoalWG->FC(FlushGoal, -BIGWG);
  GoalWG->SB(-0.5);

  CurrGoalWG->SC(GoalWG, 1);
  CurrGoalWG->FC(SetGoalFlag, -BIGWG);
  CurrGoalWG->SB(-0.5);

  GoalPhFL->SC(WkPhB, 1);
  GoalPhFL->FC(SetGoalFlag, 1);
  GoalPhFL->SB(-1.5);
  GoalPh->SC(CurrGoalPh, 1);
  GoalPh->SC(GoalPhFL, 1);
  GoalPh->FC(FlushGoal, -BIGWG);
  GoalPh->SB(-0.5);

  CurrGoalPh->SC(GoalPh, 1);
  CurrGoalPh->FC(SetGoalFlag, -BIGWG);
  CurrGoalPh->SB(-0.5);

  GetGoalPhFL->SC(GoalPh, 1);
  GetGoalPhFL->FC(GetGoalFlag, 1);
  GetGoalPhFL->SB(-1.5);
  WkPhB->SC(GetGoalPhFL, 1);

  CurrWkPh->FC(GetGoalFlag, -BIGWG);
  CurrWkPh->SB(-0.5);

  WkIfGoalWGW1->SC(WkPhB, 1);
  WkIfGoalWGW1->SCRows(GoalWG->Row[0], 1);
  WkIfGoalWGW1->SB(-1.5);
  WkEqGoalWGW1->SCtoColumns(WkIfGoalWGW1, 1);
  WkEqGoalWGW1->SB(-0.5);
  WkIfGoalWGW2->SC(WkPhB, 1);
  WkIfGoalWGW2->SCRows(GoalWG->Row[1], 1);
  WkIfGoalWGW2->SB(-1.5);
  WkEqGoalWGW2->SCtoColumns(WkIfGoalWGW2, 1);
  WkEqGoalWGW2->SB(-0.5);
  WkIfGoalWGW3->SC(WkPhB, 1);
  WkIfGoalWGW3->SCRows(GoalWG->Row[2], 1);
  WkIfGoalWGW3->SB(-1.5);
  WkEqGoalWGW3->SCtoColumns(WkIfGoalWGW3, 1);
  WkEqGoalWGW3->SB(-0.5);
  WkIfGoalWGW4->SC(WkPhB, 1);
  WkIfGoalWGW4->SCRows(GoalWG->Row[3], 1);
  WkIfGoalWGW4->SB(-1.5);
  WkEqGoalWGW4->SCtoColumns(WkIfGoalWGW4, 1);
  WkEqGoalWGW4->SB(-0.5);

  WkEqGoalWGW1C->SC(WkEqGoalWGW1, -1);
  WkEqGoalWGW1C->SB(+0.5);
  WkEqGoalWGW2C->SC(WkEqGoalWGW2, -1);
  WkEqGoalWGW2C->SB(+0.5);
  WkEqGoalWGW3C->SC(WkEqGoalWGW3, -1);
  WkEqGoalWGW3C->SB(+0.5);
  WkEqGoalWGW4C->SC(WkEqGoalWGW4, -1);
  WkEqGoalWGW4C->SB(+0.5);

  GoalPhIfGoalWGW1->SC(GoalPh, 1);
  GoalPhIfGoalWGW1->SCRows(GoalWG->Row[0], 1);
  GoalPhIfGoalWGW1->SB(-1.5);
  GoalPhEqGoalWGW1->SCtoColumns(GoalPhIfGoalWGW1, 1);
  GoalPhEqGoalWGW1->SB(-0.5);
  GoalPhIfGoalWGW2->SC(GoalPh, 1);
  GoalPhIfGoalWGW2->SCRows(GoalWG->Row[1], 1);
  GoalPhIfGoalWGW2->SB(-1.5);
  GoalPhEqGoalWGW2->SCtoColumns(GoalPhIfGoalWGW2, 1);
  GoalPhEqGoalWGW2->SB(-0.5);
  GoalPhIfGoalWGW3->SC(GoalPh, 1);
  GoalPhIfGoalWGW3->SCRows(GoalWG->Row[2], 1);
  GoalPhIfGoalWGW3->SB(-1.5);
  GoalPhEqGoalWGW3->SCtoColumns(GoalPhIfGoalWGW3, 1);
  GoalPhEqGoalWGW3->SB(-0.5);
  GoalPhIfGoalWGW4->SC(GoalPh, 1);
  GoalPhIfGoalWGW4->SCRows(GoalWG->Row[3], 1);
  GoalPhIfGoalWGW4->SB(-1.5);
  GoalPhEqGoalWGW4->SCtoColumns(GoalPhIfGoalWGW4, 1);
  GoalPhEqGoalWGW4->SB(-0.5);

  GoalPhEqGoalWGW1C->SC(GoalPhEqGoalWGW1, -1);
  GoalPhEqGoalWGW1C->SB(+0.5);
  GoalPhEqGoalWGW2C->SC(GoalPhEqGoalWGW2, -1);
  GoalPhEqGoalWGW2C->SB(+0.5);
  GoalPhEqGoalWGW3C->SC(GoalPhEqGoalWGW3, -1);
  GoalPhEqGoalWGW3C->SB(+0.5);
  GoalPhEqGoalWGW4C->SC(GoalPhEqGoalWGW4, -1);
  GoalPhEqGoalWGW4C->SB(+0.5);

  //goal mem
  GoalI = new ssm(GoalSize);
  GoalI->UseDefault();
  CurrGoalI = new ssm(GoalSize);
  NextGoalI = new ssm(GoalSize);
  PrevGoalI = new ssm(GoalSize);
  StartGoalIFlag = new ssm(1);
  CurrGoalIFlag = new ssm(1);
  NextGoalIFlag = new ssm(1);
  PrevGoalIFlag = new ssm(1);
  CurrGoalI->SC(GoalI, 1);
  CurrGoalI->FC(CurrGoalIFlag, 1);
  CurrGoalI->SB(-1.5); 
  NextGoalI->SC(GoalI, 1);
  NextGoalI->FC(NextGoalIFlag, 1);
  NextGoalI->SB(-1.5);
  PrevGoalI->SC(GoalI, 1);
  PrevGoalI->FC(PrevGoalIFlag, 1);
  PrevGoalI->SB(-1.5);
  GoalI->FC(StartGoalIFlag, -BIGWG);
  GoalI->SC(CurrGoalI, 1);
  for (int i=1; i<GoalSize; i++) {
    GoalI->Nr[i]->CN(NextGoalI->Nr[i-1], 1);
  }
  for (int i=0; i<GoalSize-1; i++) {
    GoalI->Nr[i]->CN(PrevGoalI->Nr[i+1], 1);
  }
  GoalI->Nr[0]->CN(NextGoalIFlag, 1); // if GoalI=0 and NextGoalI is
  GoalI->Nr[0]->CN(NextGoalI, -1);    // selected => PhI=1
  GoalI->SB(-0.5); // GoalI is zero if neither CurrGoalI nor NextGoalI are
  // selected
  // Provare a semplificare usando il default !!!!!

  RetrGoal = new ssm(1);
  BuildGoal = new ssm(1);
  RetrGoal->Nr[0]->O = 0;
  BuildGoal->Nr[0]->O = 0;
  GoalMem = new ssm_as(GoalSize, T_WNN, BuildGoal->Nr[0], RetrGoal->Nr[0]);
  GoalMem->SC(GoalI, 1);
  GoalMem->SetSparseOut();
  GoalMem->SparseOutFC(GoalWG);
  GoalMem->SparseOutFC(GoalPh);

  CurrGoalPh->FC(GoalI->Default, -BIGWG);
  CurrGoalWG->FC(GoalI->Default, -BIGWG);

  AddGoalRef();

  return 0;
}

int sllm::AddGoalRef()
{
#ifdef FAST_SSM
  GoalPhFL->AddSparseRef(WkPhB);
  GoalPh->AddSparseRef(CurrGoalPh);
  CurrGoalPh->AddSparseRef(GoalPh);
  GoalPh->AddSparseRef(GoalPhFL);

  GoalWGFL->AddSparseRef(WGB);
  GoalWG->AddSparseRef(CurrGoalWG);
  CurrGoalWG->AddSparseRef(GoalWG);
  GoalWG->AddSparseRef(GoalWGFL);

  GetGoalPhFL->AddSparseRef(GoalPh);
  WkPhB->AddSparseRef(GetGoalPhFL);

  WkIfGoalWGW1->AddSparseRef(WkPhB);
  WkIfGoalWGW2->AddSparseRef(WkPhB);
  WkIfGoalWGW3->AddSparseRef(WkPhB);
  WkIfGoalWGW4->AddSparseRef(WkPhB);
  WkEqGoalWGW1->AddSparseRefToColumns(WkIfGoalWGW1);
  WkEqGoalWGW2->AddSparseRefToColumns(WkIfGoalWGW2);
  WkEqGoalWGW3->AddSparseRefToColumns(WkIfGoalWGW3);
  WkEqGoalWGW4->AddSparseRefToColumns(WkIfGoalWGW4);

  GoalPhIfGoalWGW1->AddSparseRef(GoalPh);
  GoalPhIfGoalWGW2->AddSparseRef(GoalPh);
  GoalPhIfGoalWGW3->AddSparseRef(GoalPh);
  GoalPhIfGoalWGW4->AddSparseRef(GoalPh);
  GoalPhEqGoalWGW1->AddSparseRefToColumns(GoalPhIfGoalWGW1);
  GoalPhEqGoalWGW2->AddSparseRefToColumns(GoalPhIfGoalWGW2);
  GoalPhEqGoalWGW3->AddSparseRefToColumns(GoalPhIfGoalWGW3);
  GoalPhEqGoalWGW4->AddSparseRefToColumns(GoalPhIfGoalWGW4);

#endif

  return 0;
}

int sllm::GoalUpdate()
{

  //level 1
  GoalWGFL->ActivOut();
  GoalPhFL->ActivOut();
  CurrGoalWG->ActivOut();
  CurrGoalPh->ActivOut();

  //level 2
  GoalWG->ActivOut();
  GoalPh->ActivOut();

  // level 3
  WkIfGoalWGW1->ActivOut();
  WkIfGoalWGW2->ActivOut();
  WkIfGoalWGW3->ActivOut();
  WkIfGoalWGW4->ActivOut();

  //level 4
  WkEqGoalWGW1->ActivOut();
  WkEqGoalWGW2->ActivOut();
  WkEqGoalWGW3->ActivOut();
  WkEqGoalWGW4->ActivOut();

  // goal memory
  // level 1
  CurrGoalI->ActivOut();
  NextGoalI->ActivOut();
  PrevGoalI->ActivOut();
  //level 2
  GoalI->ActivOut();
  //cout << "GoalI: ";
  //for (int i=0; i<GoalI->NN(); i++) {
  //  if (GoalI->Nr[i]->O>0.5) cout << i << " ";
  //}
  //cout << endl;

  // level 3
  GoalMem->ActivOut();
  RetrGoal->Nr[0]->O = 0;
  BuildGoal->Nr[0]->O = 0;

  // GoalPh-equal-GoalWG vectors
  // level 1
  GoalPhIfGoalWGW1->ActivOut();
  GoalPhIfGoalWGW2->ActivOut();
  GoalPhIfGoalWGW3->ActivOut();
  GoalPhIfGoalWGW4->ActivOut();

  //level 2
  GoalPhEqGoalWGW1->ActivOut();
  GoalPhEqGoalWGW2->ActivOut();
  GoalPhEqGoalWGW3->ActivOut();
  GoalPhEqGoalWGW4->ActivOut();

  return 0;
}
