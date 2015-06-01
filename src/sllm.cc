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
#include <vector>
#include "sllm.h"
#include "sizes.h"
#include "interface.h"
#include "rnd.h"

using namespace std;
using namespace sizes;

sllm::sllm()
{
  CudaFlag = false;
  W = new ssm(WSize);
  IW = new ssm(WMSize);
  CW = new XSSM(WMSize);
  PhI = new ssm(PhSize);;
  InFlag = new ssm(1);
  FlushIn = new ssm(1);
  InI = new ssm(PhSize);
  InEqW = new XSSM(PhSize);
  InEqWC = new ssm(PhSize);
  InPhFL = new XSSM2d(WMSize,PhSize);
  InPhB = new XSSM2d(WMSize,PhSize);
  //InWfI = new XSSM2d(WMSize,PhSize);
  InIfW = new XSSM2d(WMSize,PhSize);
  InIfWGW1 = new XSSM2d(WMSize,PhSize);
  InIfWGW2 = new XSSM2d(WMSize,PhSize);
  InIfWGW3 = new XSSM2d(WMSize,PhSize);
  InIfWGW4 = new XSSM2d(WMSize,PhSize);
  InEqWGW1 = new XSSM(PhSize);
  InEqWGW2 = new XSSM(PhSize);
  InEqWGW3 = new XSSM(PhSize);
  InEqWGW4 = new XSSM(PhSize);
  InEqWGW1C = new ssm(PhSize);
  InEqWGW2C = new ssm(PhSize);
  InEqWGW3C = new ssm(PhSize);
  InEqWGW4C = new ssm(PhSize);
  PrevInEqWGW1FL = new ssm(PhSize);
  PrevInEqWGW2FL = new ssm(PhSize);
  PrevInEqWGW3FL = new ssm(PhSize);
  PrevInEqWGW4FL = new ssm(PhSize);
  PrevInEqWGW1 = new ssm(PhSize);
  PrevInEqWGW2 = new ssm(PhSize);
  PrevInEqWGW3 = new ssm(PhSize);
  PrevInEqWGW4 = new ssm(PhSize);
  PrevInEqWGW1C = new ssm(PhSize);
  PrevInEqWGW2C = new ssm(PhSize);
  PrevInEqWGW3C = new ssm(PhSize);
  PrevInEqWGW4C = new ssm(PhSize);

  WkFlag = new ssm(1);
  FlushWk = new ssm(1);
  //WkI = new ssm(PhSize);
  WkEqW = new XSSM(PhSize);
  WkEqWC = new ssm(PhSize);
  WkPhFL = new XSSM2d(WMSize,PhSize);
  WkPhB = new XSSM2d(WMSize,PhSize);
  CurrWkPh = new XSSM2d(WMSize,PhSize);
  WkWfI = new XSSM2d(WMSize,PhSize);
  WkIfW = new XSSM2d(WMSize,PhSize);
  WkIfWGW1 = new XSSM2d(WMSize,PhSize);
  WkIfWGW2 = new XSSM2d(WMSize,PhSize);
  WkIfWGW3 = new XSSM2d(WMSize,PhSize);
  WkIfWGW4 = new XSSM2d(WMSize,PhSize);
  WkEqWGW1 = new XSSM(PhSize);
  WkEqWGW2 = new XSSM(PhSize);
  WkEqWGW3 = new XSSM(PhSize);
  WkEqWGW4 = new XSSM(PhSize);
  WkEqWGW1C = new ssm(PhSize);
  WkEqWGW2C = new ssm(PhSize);
  WkEqWGW3C = new ssm(PhSize);
  WkEqWGW4C = new ssm(PhSize);
  PrevWkPhFL = new XSSM2d(WMSize,PhSize);
  PrevWkPh = new XSSM2d(WMSize,PhSize);
  PrevWkEqWGW1FL = new ssm(PhSize);
  PrevWkEqWGW2FL = new ssm(PhSize);
  PrevWkEqWGW3FL = new ssm(PhSize);
  PrevWkEqWGW4FL = new ssm(PhSize);
  PrevWkEqWGW1 = new ssm(PhSize);
  PrevWkEqWGW2 = new ssm(PhSize);
  PrevWkEqWGW3 = new ssm(PhSize);
  PrevWkEqWGW4 = new ssm(PhSize);
  PrevWkEqWGW1C = new ssm(PhSize);
  PrevWkEqWGW2C = new ssm(PhSize);
  PrevWkEqWGW3C = new ssm(PhSize);
  PrevWkEqWGW4C = new ssm(PhSize);

  OutFlag = new ssm(1);
  FlushOut = new ssm(1);
  OutEqW = new XSSM(PhSize);
  OutEqWC = new ssm(PhSize);
  OutPhFL = new XSSM2d(WMSize,PhSize);
  OutPhB = new XSSM2d(WMSize,PhSize);
  OutIfW = new XSSM2d(WMSize,PhSize);

  WkOutPhFL = new XSSM2d(WMSize,PhSize);
  WkOutPhB = new XSSM2d(WMSize,PhSize);
  WkOutEqWGW1FL = new ssm(PhSize);
  WkOutEqWGW2FL = new ssm(PhSize);
  WkOutEqWGW3FL = new ssm(PhSize);
  WkOutEqWGW4FL = new ssm(PhSize);
  WkOutEqWGW1 = new ssm(PhSize);
  WkOutEqWGW2 = new ssm(PhSize);
  WkOutEqWGW3 = new ssm(PhSize);
  WkOutEqWGW4 = new ssm(PhSize);
  WkOutEqWGW1C = new ssm(PhSize);
  WkOutEqWGW2C = new ssm(PhSize);
  WkOutEqWGW3C = new ssm(PhSize);
  WkOutEqWGW4C = new ssm(PhSize);

  WGI = new ssm(PhSize);
  WGCW = new XSSM(WMSize);
  GetFlag = new ssm(1);
  WGFlag = new ssm(1);
  FlushWG = new ssm(1);
  WGIFL = new ssm(PhSize);
  WGEqW = new XSSM(PhSize);
  WGEqWC = new ssm(PhSize);
  WGFL = new XSSM2d(WMSize,PhSize);
  WGB = new XSSM2d(WMSize,PhSize);
  WGIfW = new XSSM2d(WMSize,PhSize);
  PrevWGFL = new XSSM2d(WMSize,PhSize);
  PrevWG = new XSSM2d(WMSize,PhSize);

  // null word
  CW->UseDefault();
  InPhB->UseRowDefault();
  WkPhB->UseRowDefault();
  PrevWkPh->UseRowDefault();
  WGB->UseRowDefault();
  PrevWG->UseRowDefault();
  OutPhB->UseRowDefault();
  WkOutPhB->UseRowDefault();

  InI->UseDefault();
  PhI->UseDefault(); //WkI->UseDefault();
  WGI->UseDefault();
  WGIFL->UseDefault();

  IW->FC(W, -Wgm, Wgm);
  CW->SB(-0.5);

  InI->SC(PhI, 1);
  InI->FC(InFlag, 1);
  InPhFL->SCRows(IW, 1);
  InPhFL->SCColumns(InI, 1);
  InPhFL->SB(-1.5);
  InPhB->SC(InPhB, 1); //self
  InPhB->SC(InPhFL, 1);
  InPhB->FC(FlushIn, -BIGWG);
  InPhB->SB(-0.5);
  // association
  //InWfI->SC(InPhB, 1);
  //InWfI->SCColumns(InI, 1);
  //InWfI->SB(-1.5);
  InIfW->SC(InPhB, 1);
  InIfW->SCRows(CW, 1);
  InIfW->SB(-1.5);
  //CW->SCtoRows(InWfI, 1);
  InEqW->SCtoColumns(InIfW, 1);
  InEqW->SB(-0.5);

  InIfWGW1->SC(InPhB, 1);
  InIfWGW1->SCRows(WGB->Row[0], 1);
  InIfWGW1->SB(-1.5);
  InEqWGW1->SCtoColumns(InIfWGW1, 1);
  InEqWGW1->SB(-0.5);
  InIfWGW2->SC(InPhB, 1);
  InIfWGW2->SCRows(WGB->Row[1], 1);
  InIfWGW2->SB(-1.5);
  InEqWGW2->SCtoColumns(InIfWGW2, 1);
  InEqWGW2->SB(-0.5);
  InIfWGW3->SC(InPhB, 1);
  InIfWGW3->SCRows(WGB->Row[2], 1);
  InIfWGW3->SB(-1.5);
  InEqWGW3->SCtoColumns(InIfWGW3, 1);
  InEqWGW3->SB(-0.5);
  InIfWGW4->SC(InPhB, 1);
  InIfWGW4->SCRows(WGB->Row[3], 1);
  InIfWGW4->SB(-1.5);
  InEqWGW4->SCtoColumns(InIfWGW4, 1);
  InEqWGW4->SB(-0.5);

  PrevInEqWGW1FL->SC(InEqWGW1, 1);
  PrevInEqWGW1FL->FC(FlushWG, 1);
  PrevInEqWGW1FL->SB(-1.5);
  PrevInEqWGW1->SC(PrevInEqWGW1, 1); // self
  PrevInEqWGW1->SC(PrevInEqWGW1FL, 1);
  PrevInEqWGW1->FC(FlushWG, -BIGWG);
  PrevInEqWGW1->SB(-0.5);
  PrevInEqWGW2FL->SC(InEqWGW2, 1);
  PrevInEqWGW2FL->FC(FlushWG, 1);
  PrevInEqWGW2FL->SB(-1.5);
  PrevInEqWGW2->SC(PrevInEqWGW2, 1); // self
  PrevInEqWGW2->SC(PrevInEqWGW2FL, 1);
  PrevInEqWGW2->FC(FlushWG, -BIGWG);
  PrevInEqWGW2->SB(-0.5);
  PrevInEqWGW3FL->SC(InEqWGW3, 1);
  PrevInEqWGW3FL->FC(FlushWG, 1);
  PrevInEqWGW3FL->SB(-1.5);
  PrevInEqWGW3->SC(PrevInEqWGW3, 1); // self
  PrevInEqWGW3->SC(PrevInEqWGW3FL, 1);
  PrevInEqWGW3->FC(FlushWG, -BIGWG);
  PrevInEqWGW3->SB(-0.5);
  PrevInEqWGW4FL->SC(InEqWGW4, 1);
  PrevInEqWGW4FL->FC(FlushWG, 1);
  PrevInEqWGW4FL->SB(-1.5);
  PrevInEqWGW4->SC(PrevInEqWGW4, 1); // self
  PrevInEqWGW4->SC(PrevInEqWGW4FL, 1);
  PrevInEqWGW4->FC(FlushWG, -BIGWG);
  PrevInEqWGW4->SB(-0.5);

  //WkI->SC(PhI, 1);
  //WkI->FC(WkFlag, 1);
  //WkPhFL->SCRows(IW, 1);
  //WkPhFL->SCColumns(WkI, 1);
  WkPhFL->SC(InPhB, 1);
  WkPhFL->FC(WkFlag, 1);
  WkPhFL->SB(-1.5);
  WkPhB->SC(CurrWkPh, 1);
  WkPhB->SC(WkPhFL, 1);
  WkPhB->FC(FlushWk, -BIGWG);
  WkPhB->SB(-0.5);

  CurrWkPh->SC(WkPhB, 1);
  CurrWkPh->FC(WkFlag, -BIGWG);
  CurrWkPh->SB(-0.5);

  // association
  WkWfI->SC(WkPhB, 1);
  WkWfI->SCColumns(PhI, 1); //WkWfI->SCColumns(WkI, 1);
  WkWfI->SB(-1.5);
  WkIfW->SC(WkPhB, 1);
  WkIfW->SCRows(CW, 1);
  WkIfW->SB(-1.5);
  CW->SCtoRows(WkWfI, 1);
  WkEqW->SCtoColumns(WkIfW, 1);
  WkEqW->SB(-0.5);
 
  WkIfWGW1->SC(WkPhB, 1);
  WkIfWGW1->SCRows(WGB->Row[0], 1);
  WkIfWGW1->SB(-1.5);
  WkEqWGW1->SCtoColumns(WkIfWGW1, 1);
  WkEqWGW1->SB(-0.5);
  WkIfWGW2->SC(WkPhB, 1);
  WkIfWGW2->SCRows(WGB->Row[1], 1);
  WkIfWGW2->SB(-1.5);
  WkEqWGW2->SCtoColumns(WkIfWGW2, 1);
  WkEqWGW2->SB(-0.5);
  WkIfWGW3->SC(WkPhB, 1);
  WkIfWGW3->SCRows(WGB->Row[2], 1);
  WkIfWGW3->SB(-1.5);
  WkEqWGW3->SCtoColumns(WkIfWGW3, 1);
  WkEqWGW3->SB(-0.5);
  WkIfWGW4->SC(WkPhB, 1);
  WkIfWGW4->SCRows(WGB->Row[3], 1);
  WkIfWGW4->SB(-1.5);
  WkEqWGW4->SCtoColumns(WkIfWGW4, 1);
  WkEqWGW4->SB(-0.5);

  PrevWkPhFL->SC(WkPhB, 1);
  PrevWkPhFL->FC(FlushWG, 1);
  PrevWkPhFL->SB(-1.5);
  PrevWkPh->SC(PrevWkPh, 1); // self
  PrevWkPh->SC(PrevWkPhFL, 1);
  PrevWkPh->FC(FlushWG, -BIGWG);
  PrevWkPh->SB(-0.5);

  PrevWkEqWGW1FL->SC(WkEqWGW1, 1);
  PrevWkEqWGW1FL->FC(FlushWG, 1);
  PrevWkEqWGW1FL->SB(-1.5);
  PrevWkEqWGW1->SC(PrevWkEqWGW1, 1); // self
  PrevWkEqWGW1->SC(PrevWkEqWGW1FL, 1);
  PrevWkEqWGW1->FC(FlushWG, -BIGWG);
  PrevWkEqWGW1->SB(-0.5);
  PrevWkEqWGW2FL->SC(WkEqWGW2, 1);
  PrevWkEqWGW2FL->FC(FlushWG, 1);
  PrevWkEqWGW2FL->SB(-1.5);
  PrevWkEqWGW2->SC(PrevWkEqWGW2, 1); // self
  PrevWkEqWGW2->SC(PrevWkEqWGW2FL, 1);
  PrevWkEqWGW2->FC(FlushWG, -BIGWG);
  PrevWkEqWGW2->SB(-0.5);
  PrevWkEqWGW3FL->SC(WkEqWGW3, 1);
  PrevWkEqWGW3FL->FC(FlushWG, 1);
  PrevWkEqWGW3FL->SB(-1.5);
  PrevWkEqWGW3->SC(PrevWkEqWGW3, 1); // self
  PrevWkEqWGW3->SC(PrevWkEqWGW3FL, 1);
  PrevWkEqWGW3->FC(FlushWG, -BIGWG);
  PrevWkEqWGW3->SB(-0.5);
  PrevWkEqWGW4FL->SC(WkEqWGW4, 1);
  PrevWkEqWGW4FL->FC(FlushWG, 1);
  PrevWkEqWGW4FL->SB(-1.5);
  PrevWkEqWGW4->SC(PrevWkEqWGW4, 1); // self
  PrevWkEqWGW4->SC(PrevWkEqWGW4FL, 1);
  PrevWkEqWGW4->FC(FlushWG, -BIGWG);
  PrevWkEqWGW4->SB(-0.5);

  OutPhFL->SC(WGB, 1);
  OutPhFL->FC(OutFlag, 1);
  OutPhFL->SB(-1.5);
  OutPhB->SC(OutPhB, 1); //self
  OutPhB->SC(OutPhFL, 1);
  OutPhB->FC(FlushOut, -BIGWG);
  OutPhB->SB(-0.5);
  // association
  OutIfW->SC(OutPhB, 1);
  OutIfW->SCRows(CW, 1);
  OutIfW->SB(-1.5);
  OutEqW->SCtoColumns(OutIfW, 1);
  OutEqW->SB(-0.5);

  WkOutPhFL->SC(WkPhB, 1);
  WkOutPhFL->FC(OutFlag, 1);
  WkOutPhFL->SB(-1.5);
  WkOutPhB->SC(WkOutPhB, 1); //self
  WkOutPhB->SC(WkOutPhFL, 1);
  WkOutPhB->FC(FlushOut, -BIGWG);
  WkOutPhB->SB(-0.5);

  WkOutEqWGW1FL->SC(WkEqWGW1, 1);
  WkOutEqWGW1FL->FC(OutFlag, 1);
  WkOutEqWGW1FL->SB(-1.5);
  WkOutEqWGW1->SC(WkOutEqWGW1, 1); // self
  WkOutEqWGW1->SC(WkOutEqWGW1FL, 1);
  WkOutEqWGW1->FC(FlushOut, -BIGWG);
  WkOutEqWGW1->SB(-0.5);
  WkOutEqWGW2FL->SC(WkEqWGW2, 1);
  WkOutEqWGW2FL->FC(OutFlag, 1);
  WkOutEqWGW2FL->SB(-1.5);
  WkOutEqWGW2->SC(WkOutEqWGW2, 1); // self
  WkOutEqWGW2->SC(WkOutEqWGW2FL, 1);
  WkOutEqWGW2->FC(FlushOut, -BIGWG);
  WkOutEqWGW2->SB(-0.5);
  WkOutEqWGW3FL->SC(WkEqWGW3, 1);
  WkOutEqWGW3FL->FC(OutFlag, 1);
  WkOutEqWGW3FL->SB(-1.5);
  WkOutEqWGW3->SC(WkOutEqWGW3, 1); // self
  WkOutEqWGW3->SC(WkOutEqWGW3FL, 1);
  WkOutEqWGW3->FC(FlushOut, -BIGWG);
  WkOutEqWGW3->SB(-0.5);
  WkOutEqWGW4FL->SC(WkEqWGW4, 1);
  WkOutEqWGW4FL->FC(OutFlag, 1);
  WkOutEqWGW4FL->SB(-1.5);
  WkOutEqWGW4->SC(WkOutEqWGW4, 1); // self
  WkOutEqWGW4->SC(WkOutEqWGW4FL, 1);
  WkOutEqWGW4->FC(FlushOut, -BIGWG);
  WkOutEqWGW4->SB(-0.5);

  WGCW->SC(CW, 1);
  WGCW->FC(GetFlag, 1);
  WGCW->SB(-1.5);
  WGIFL->SC(WGI, 1);
  WGIFL->FC(WGFlag, 1);
  WGFL->SCRows(WGCW, 1);
  WGFL->SCColumns(WGIFL, 1);
  WGFL->SB(-1.5);
  WGB->SC(WGB, 1); //self
  WGB->SC(WGFL, 1);
  WGB->FC(FlushWG, -BIGWG);
  WGB->SB(-0.5);
  // association
  WGIfW->SC(WGB, 1);
  WGIfW->SCRows(CW, 1);
  WGIfW->SB(-1.5);
  WGEqW->SCtoColumns(WGIfW, 1);
  WGEqW->SB(-0.5);

  PrevWGFL->SC(WGB, 1);
  PrevWGFL->FC(FlushWG, 1);
  PrevWGFL->SB(-1.5);
  PrevWG->SC(PrevWG, 1); // self
  PrevWG->SC(PrevWGFL, 1);
  PrevWG->FC(FlushWG, -BIGWG);
  PrevWG->SB(-0.5);

  InI->SB(-1.5);
  //WkI->SB(-1.5);
  WGIFL->SB(-1.5);

  RetrAs = new ssm(1);
  BuildAs = new ssm(1);
  RetrAs->Nr[0]->O = 0;
  BuildAs->Nr[0]->O = 0;
  WGB->RowDefault->FillHighVect=true;
  CW->FC(RetrAs, -BIGWG);

  CurrPhI = new ssm(PhSize);
  NextPhI = new ssm(PhSize);
  CurrPhIFlag = new ssm(1);
  NextPhIFlag = new ssm(1);
  CurrPhI->SC(PhI, 1);
  CurrPhI->FC(CurrPhIFlag, 1);
  CurrPhI->SB(-1.5); 
  NextPhI->SC(PhI, 1);
  NextPhI->FC(NextPhIFlag, 1);
  NextPhI->SB(-1.5);
  PhI->SC(CurrPhI, 1);
  for (int iw=1; iw<PhSize; iw++) {
    PhI->Nr[iw]->CN(NextPhI->Nr[iw-1], 1);
  }
  PhI->Nr[0]->CN(NextPhIFlag, 1); // if PhI=0 and NextPhI is selected => PhI=1
  PhI->Nr[0]->CN(NextPhI, -1);    // if PhI=0 and NextPhI is selected => PhI=1
  PhI->SB(-0.5); // PhI is zero if neither CurrPhI nor NextPhI are selected

  CurrWGI = new ssm(PhSize);
  NextWGI = new ssm(PhSize);
  CurrWGIFlag = new ssm(1);
  NextWGIFlag = new ssm(1);
  CurrWGI->SC(WGI, 1);
  CurrWGI->FC(CurrWGIFlag, 1);
  CurrWGI->SB(-1.5); 
  NextWGI->SC(WGI, 1);
  NextWGI->FC(NextWGIFlag, 1);
  NextWGI->SB(-1.5);
  WGI->SC(CurrWGI, 1);
  for (int iw=1; iw<PhSize; iw++) {
    WGI->Nr[iw]->CN(NextWGI->Nr[iw-1], 1);
  }
  WGI->Nr[0]->CN(NextWGIFlag, 1);//if WGI=0 and NextWGI is selected=>WGI=1
  WGI->Nr[0]->CN(NextWGI, -1);    // if PhI=0 and NextPhI is selected => PhI=1
  WGI->SB(-0.5); // WGI is zero if neither CurrWGI nor NextWGI are selected

  NewMemPhFlag = new ssm(1);
  RetrRemPh = new ssm(1);
  CurrRemPhFlag = new ssm(1);
  NextRemPhFlag = new ssm(1);
  StartPhFlag = new ssm(1);
  RemStartPhFlag = new ssm(1);
  CurrStartWkPhFlag = new ssm(1);

  GoalArch();
  ElActFL = new ssm(ElActSize);
  ElAct = new ssm(ElActSize);
  AcqAct = new ssm(AcqActSize);
  ElActFL->UseDefault();
  ElAct->UseDefault();
  AcqAct->UseDefault();
  ElAct->SC(ElActFL,1);
  ElActFL->SB(-0.5);
  ElAct->SB(-0.5);
  AcqAct->SB(-0.5);

  // Free slots for additionan action flags
  ActFlag22 = new ssm(1);
  ActFlag23 = new ssm(1);
  ActFlag24 = new ssm(1);
  ActFlag25 = new ssm(1);
  ActFlag26 = new ssm(1);
  ActFlag27 = new ssm(1);
  ActFlag28 = new ssm(1);
  ActFlag29 = new ssm(1);

  ActFlags = new ssm();
  ActFlagsC = new ssm(ActFlagSize);

  ActFlags->Nr.push_back(FlushIn->Nr[0]);
  ActFlags->Nr.push_back(FlushWk->Nr[0]);
  ActFlags->Nr.push_back(FlushWG->Nr[0]);
  ActFlags->Nr.push_back(InFlag->Nr[0]);
  ActFlags->Nr.push_back(WkFlag->Nr[0]);
  ActFlags->Nr.push_back(WGFlag->Nr[0]);
  ActFlags->Nr.push_back(GetFlag->Nr[0]);
  ActFlags->Nr.push_back(RetrAs->Nr[0]);
  ActFlags->Nr.push_back(BuildAs->Nr[0]);
  ActFlags->Nr.push_back(CurrPhIFlag->Nr[0]);
  ActFlags->Nr.push_back(NextPhIFlag->Nr[0]);
  ActFlags->Nr.push_back(CurrWGIFlag->Nr[0]);
  ActFlags->Nr.push_back(NextWGIFlag->Nr[0]);
  ActFlags->Nr.push_back(FlushOut->Nr[0]);
  ActFlags->Nr.push_back(OutFlag->Nr[0]);
  ActFlags->Nr.push_back(NewMemPhFlag->Nr[0]);
  ActFlags->Nr.push_back(RetrRemPh->Nr[0]);
  ActFlags->Nr.push_back(CurrRemPhFlag->Nr[0]);
  ActFlags->Nr.push_back(NextRemPhFlag->Nr[0]);
  ActFlags->Nr.push_back(StartPhFlag->Nr[0]);
  ActFlags->Nr.push_back(RemStartPhFlag->Nr[0]);
  ActFlags->Nr.push_back(CurrStartWkPhFlag->Nr[0]);
  ActFlags->Nr.push_back(SetGoalFlag->Nr[0]);
  ActFlags->Nr.push_back(GetGoalFlag->Nr[0]);
  ActFlags->Nr.push_back(FlushGoal->Nr[0]);
  //ActFlags->Nr.push_back(StartGoalIFlag->Nr[0]);
  ActFlags->Nr.push_back(CurrGoalIFlag->Nr[0]);
  ActFlags->Nr.push_back(NextGoalIFlag->Nr[0]);
  ActFlags->Nr.push_back(PrevGoalIFlag->Nr[0]);
  ActFlags->Nr.push_back(RetrGoal->Nr[0]);
  ActFlags->Nr.push_back(BuildGoal->Nr[0]);
  ActFlags->FC(ElAct, 0);
  ActFlags->FC(AcqAct, 0);
  //ActFlags->FC(ElAct->Default, 0);
  InFlag->SC(InFlag,1); //self
  WkFlag->SC(WkFlag,1); //self

  int el_act_matr[ElActSize][ActFlagSize] = {
    // FlsIn FlsWk FlsWG InFlg WkFlg WGFlg GtFlg RtrAs BldAs CurPF
    // NxtPF CurGF NxtGF FlsOu OuFlg NewMm RtrRm CurRm NxtRm StrPh
    // RmStr CurSt SetGL GetGL FlsGL CurGL NxtGL PrvGL RtrGL BldGL
    {   -1,   -1,   +1,   -1,   -1,   -1,   -1,   -1,   -1,   +1, // FLUSH_WG
        -1,   -1,   -1,   -1,   -1,   -1,   -1,   +1,   -1,   -1,
        -1,   +1,   -1,   -1,   -1,   +1,   -1,   -1,   -1,   -1},

    {   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1, // W_FROM_WK
        -1,   +1,   -1,   -1,   -1,   -1,   -1,   +1,   -1,   -1,
        -1,   +1,   -1,   -1,   -1,   +1,   -1,   -1,   -1,   -1},

    {   -1,   -1,   -1,   -1,   +1,   -1,   -1,   -1,   -1,   -1, // W_FROM_IN
        -1,   +1,   -1,   -1,   -1,   +1,   -1,   -1,   -1,   -1,
        -1,   -1,   -1,   -1,   -1,   +1,   -1,   -1,   -1,   -1},

    {   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1, // NEXT_W
        +1,   +1,   -1,   -1,   -1,   -1,   -1,   +1,   -1,   -1,
        -1,   +1,   -1,   -1,   -1,   +1,   -1,   -1,   -1,   -1},

    {   -1,   -1,   -1,   -1,   -1,   +1,   +1,   -1,   -1,   +1, // GET_W
        -1,   -1,   +1,   -1,   -1,   -1,   -1,   +1,   -1,   -1,
        -1,   +1,   -1,   -1,   -1,   +1,   -1,   -1,   -1,   -1},

    {   -1,   -1,   -1,   -1,   -1,   -1,   -1,   +1,   -1,   -1, // RETR_AS
        -1,   +1,   -1,   -1,   -1,   -1,   +1,   +1,   -1,   -1,
        -1,   +1,   -1,   -1,   -1,   +1,   -1,   -1,   -1,   -1},

    {   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   +1, // FLUSH_OUT
        -1,   -1,   -1,   +1,   -1,   -1,   -1,   +1,   -1,   -1,
        -1,   +1,   -1,   -1,   -1,   +1,   -1,   -1,   -1,   -1},

    {   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   +1, // WG_OUT
        -1,   -1,   -1,   -1,   +1,   -1,   -1,   +1,   -1,   -1,
        -1,   +1,   -1,   -1,   -1,   +1,   -1,   -1,   -1,   -1},

    {   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,// GET_NEXT_PH
        -1,   +1,   -1,   -1,   -1,   -1,   +1,   -1,   +1,   -1,
        -1,   +1,   -1,   -1,   -1,   +1,   -1,   -1,   -1,   -1},

    {   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,//GET_START_PH
        -1,   +1,   -1,   -1,   -1,   -1,   +1,   -1,   -1,   -1,
        +1,   +1,   -1,   -1,   -1,   +1,   -1,   -1,   -1,   -1},

    {   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   +1, // CONTINUE
        -1,   +1,   -1,   -1,   -1,   -1,   -1,   +1,   -1,   -1,
        -1,   +1,   -1,   -1,   -1,   +1,   -1,   -1,   -1,   -1},

    {   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   +1, // DONE
        -1,   +1,   -1,   -1,   -1,   -1,   -1,   +1,   -1,   -1,
        -1,   +1,   -1,   -1,   -1,   +1,   -1,   -1,   -1,   -1},

    {   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   +1, // PUSH_GL
        -1,   +1,   -1,   -1,   -1,   -1,   -1,   +1,   -1,   -1,
        -1,   +1,   +1,   -1,   -1,   -1,   +1,   -1,   -1,   +1},

    {   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   +1, // DROP_GL
        -1,   +1,   -1,   -1,   -1,   -1,   -1,   +1,   -1,   -1,
        -1,   +1,   -1,   -1,   -1,   -1,   -1,   +1,   +1,   -1},

    {   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1, // GET_GL_PH
        -1,   +1,   -1,   -1,   -1,   -1,   -1,   +1,   -1,   -1,
        -1,   +1,   -1,   +1,   -1,   +1,   -1,   -1,   -1,   -1},

    {   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   +1, // SNT_OUT
        -1,   +1,   -1,   -1,   -1,   -1,   -1,   +1,   -1,   -1,
        -1,   +1,   -1,   -1,   -1,   +1,   -1,   -1,   -1,   -1},

    {   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   +1, // EL_ACT_16
        -1,   +1,   -1,   -1,   -1,   -1,   -1,   +1,   -1,   -1,
        -1,   +1,   -1,   -1,   -1,   +1,   -1,   -1,   -1,   -1},

    {   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   +1, // EL_ACT_17
        -1,   +1,   -1,   -1,   -1,   -1,   -1,   +1,   -1,   -1,
        -1,   +1,   -1,   -1,   -1,   +1,   -1,   -1,   -1,   -1},

    {   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   +1, // EL_ACT_18
        -1,   +1,   -1,   -1,   -1,   -1,   -1,   +1,   -1,   -1,
        -1,   +1,   -1,   -1,   -1,   +1,   -1,   -1,   -1,   -1},

    {   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   +1, // EL_ACT_19
        -1,   +1,   -1,   -1,   -1,   -1,   -1,   +1,   -1,   -1,
        -1,   +1,   -1,   -1,   -1,   +1,   -1,   -1,   -1,   -1},
  };
  int acq_act_matr[AcqActSize][ActFlagSize] = {
    {   +1,   +1,   +1,   -1,   -1,   -1,   -1,   -1,   -1,   -1, // FLUSH
        -1,   -1,   -1,   -1,   -1,   -1,   -1,   +1,   -1,   -1,
        -1,   +1,   -1,   -1,   -1,   +1,   -1,   -1,   -1,   -1},

    {   -1,   -1,   -1,   +1,   +1,   -1,   -1,   -1,   -1,   -1, // ACQ_W
        +1,   +1,   -1,   -1,   -1,   -1,   -1,   +1,   -1,   -1,
        -1,   +1,   -1,   -1,   -1,   +1,   -1,   -1,   -1,   -1},

    {   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1, // NEXT_AS_W
        +1,   +1,   -1,   -1,   -1,   -1,   -1,   +1,   -1,   -1,
        -1,   +1,   -1,   -1,   -1,   +1,   -1,   -1,   -1,   -1},

    {   -1,   -1,   -1,   -1,   -1,   +1,   +1,   -1,   +1,   +1, // BUILD_AS
        -1,   -1,   +1,   -1,   -1,   -1,   -1,   +1,   -1,   -1,
        -1,   +1,   -1,   -1,   -1,   +1,   -1,   -1,   -1,   -1},

    {   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   +1, // MEM_PH
        -1,   +1,   -1,   -1,   -1,   +1,   -1,   -1,   -1,   -1,
        -1,   -1,   -1,   -1,   -1,   +1,   -1,   -1,   -1,   -1},

    {   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   +1,//SET_START_PH
        -1,   +1,   -1,   -1,   -1,   +1,   -1,   -1,   -1,   +1,
        -1,   -1,   -1,   -1,   -1,   +1,   -1,   -1,   -1,   -1}
  };

  /*
  int null_act_vect[ActFlagSize] =
    // FlsIn FlsWk FlsWG InFlg WkFlg WGFlg GtFlg RtrAs BldAs CurPF
    // NxtPF CurGF NxtGF FlsOu OuFlg NewMm RtrRm CurRm NxtRm StrPh
    // RmStr CurSt Flg22 Flg23 Flg24 Flg25 Flg26 Flg27 Flg28 Flg29
    {   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
        -1,   +1,   -1,   -1,   -1,   -1,   -1,   +1,   -1,   -1,
        -1,   +1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1};
  */

  for (int ia=0; ia<ElActSize; ia++) {
    for (int iaf=0; iaf<ActFlagSize; iaf++) {
      ActFlags->Nr[iaf]->L[ia].Wg = el_act_matr[ia][iaf];
    }
  }
  for (int ia=0; ia<AcqActSize; ia++) {
    for (int iaf=0; iaf<ActFlagSize; iaf++) {
      ActFlags->Nr[iaf]->L[ElActSize+ia].Wg = acq_act_matr[ia][iaf];
    }
  }
  //for (int iaf=0; iaf<ActFlagSize; iaf++) {
  //  ActFlags->Nr[iaf]->L[ElActSize+AcqActSize].Wg = null_act_vect[iaf];
  //}

  for (int iaf=0;  iaf<ActFlagSize; iaf++) {
    ActFlags->Nr[iaf]->Used = 1;
  }
  ActFlags->SB(-0.5); // all flags are zero when no act is selected

  // Set up complementary SSMs
  InEqWC->SC(InEqW, -1);
  InEqWC->SB(+0.5);
  WkEqWC->SC(WkEqW, -1);
  WkEqWC->SB(+0.5);
  OutEqWC->SC(OutEqW, -1);
  OutEqWC->SB(+0.5);
  WGEqWC->SC(WGEqW, -1);
  WGEqWC->SB(+0.5);
  InEqWGW1C->SC(InEqWGW1, -1);
  InEqWGW1C->SB(+0.5);
  InEqWGW2C->SC(InEqWGW2, -1);
  InEqWGW2C->SB(+0.5);
  InEqWGW3C->SC(InEqWGW3, -1);
  InEqWGW3C->SB(+0.5);
  InEqWGW4C->SC(InEqWGW4, -1);
  InEqWGW4C->SB(+0.5);
  PrevInEqWGW1C->SC(PrevInEqWGW1, -1);
  PrevInEqWGW1C->SB(+0.5);
  PrevInEqWGW2C->SC(PrevInEqWGW2, -1);
  PrevInEqWGW2C->SB(+0.5);
  PrevInEqWGW3C->SC(PrevInEqWGW3, -1);
  PrevInEqWGW3C->SB(+0.5);
  PrevInEqWGW4C->SC(PrevInEqWGW4, -1);
  PrevInEqWGW4C->SB(+0.5);
  WkEqWGW1C->SC(WkEqWGW1, -1);
  WkEqWGW1C->SB(+0.5);
  WkEqWGW2C->SC(WkEqWGW2, -1);
  WkEqWGW2C->SB(+0.5);
  WkEqWGW3C->SC(WkEqWGW3, -1);
  WkEqWGW3C->SB(+0.5);
  WkEqWGW4C->SC(WkEqWGW4, -1);
  WkEqWGW4C->SB(+0.5);
  PrevWkEqWGW1C->SC(PrevWkEqWGW1, -1);
  PrevWkEqWGW1C->SB(+0.5);
  PrevWkEqWGW2C->SC(PrevWkEqWGW2, -1);
  PrevWkEqWGW2C->SB(+0.5);
  PrevWkEqWGW3C->SC(PrevWkEqWGW3, -1);
  PrevWkEqWGW3C->SB(+0.5);
  PrevWkEqWGW4C->SC(PrevWkEqWGW4, -1);
  PrevWkEqWGW4C->SB(+0.5);

  WkOutEqWGW1C->SC(WkOutEqWGW1, -1);
  WkOutEqWGW1C->SB(+0.5);
  WkOutEqWGW2C->SC(WkOutEqWGW2, -1);
  WkOutEqWGW2C->SB(+0.5);
  WkOutEqWGW3C->SC(WkOutEqWGW3, -1);
  WkOutEqWGW3C->SB(+0.5);
  WkOutEqWGW4C->SC(WkOutEqWGW4, -1);
  WkOutEqWGW4C->SB(+0.5);

  ActFlagsC->SC(ActFlags, -1);
  ActFlagsC->SB(+0.5);

  // Sm Act from St
  RetrElActfSt = new ssm(1);
  BuildElActfSt = new ssm(1);
  RewdElActfSt = new ssm(1);
  RetrElActfSt->Nr[0]->O = 0;
  BuildElActfSt->Nr[0]->O = 0;
  RewdElActfSt->Nr[0]->O = 1;
  ElActfSt = new XSSM_AS(ElActfStSize, T_NEW_KWTA, BuildElActfSt->Nr[0],
			RetrElActfSt->Nr[0], RewdElActfSt->Nr[0]);
  ElActfSt->K = 5; // K winners, K=5

  ElActfSt->SetSparseIn();
  ElActfSt->SparseFC(CW);
  ElActfSt->SparseFC(CW->Default, false); //
  ElActfSt->SparseFC(InI);
  ElActfSt->SparseFC(InI->Default, false); //
  ElActfSt->SparseFC(InEqW);
  ElActfSt->SparseFC(InEqWC); //
  ElActfSt->SparseFC(InPhB,-3,3);
  ElActfSt->SparseFC(InPhB->RowDefault, -3,3,false); //
  ElActfSt->SparseFC(PhI);//ElActfSt->SparseFC(WkI);
  ElActfSt->SparseFC(PhI->Default, false);
  // ElActfSt->SparseFC(WkI->Default, false);
  ElActfSt->SparseFC(WkEqW);
  ElActfSt->SparseFC(WkEqWC); //
  ElActfSt->SparseFC(WkPhB);
  ElActfSt->SparseFC(WkPhB->RowDefault, false); //
  ElActfSt->SparseFC(PrevWkPh);
  ElActfSt->SparseFC(PrevWkPh->RowDefault, false); //
  ElActfSt->SparseFC(WGI);
  ElActfSt->SparseFC(WGI->Default, false); //
  ElActfSt->SparseFC(WGI);
  ElActfSt->SparseFC(WGI->Default, false); //
  ElActfSt->SparseFC(WGI);
  ElActfSt->SparseFC(WGI->Default, false); //
  ElActfSt->SparseFC(WGEqW);
  ElActfSt->SparseFC(WGEqWC); //
  ElActfSt->SparseFC(WGB);
  ElActfSt->SparseFC(WGB->RowDefault,false); //
  ElActfSt->SparseFC(PrevWG);
  ElActfSt->SparseFC(PrevWG->RowDefault, false); //
  ElActfSt->SparseFC(InEqWGW1);
  ElActfSt->SparseFC(InEqWGW1C); //
  ElActfSt->SparseFC(InEqWGW2);
  ElActfSt->SparseFC(InEqWGW2C); //
  ElActfSt->SparseFC(InEqWGW3);
  ElActfSt->SparseFC(InEqWGW3C); //
  ElActfSt->SparseFC(InEqWGW4);
  ElActfSt->SparseFC(InEqWGW4C); //
  ElActfSt->SparseFC(PrevInEqWGW1);
  ElActfSt->SparseFC(PrevInEqWGW1C); //
  ElActfSt->SparseFC(PrevInEqWGW2);
  ElActfSt->SparseFC(PrevInEqWGW2C); //
  ElActfSt->SparseFC(PrevInEqWGW3);
  ElActfSt->SparseFC(PrevInEqWGW3C); //
  ElActfSt->SparseFC(PrevInEqWGW4);
  ElActfSt->SparseFC(PrevInEqWGW4C); //

  ElActfSt->SparseFC(WkEqWGW1, -3, +3);
  ElActfSt->SparseFC(WkEqWGW1C, -3, +3); //
  ElActfSt->SparseFC(WkEqWGW2, -3, +3);
  ElActfSt->SparseFC(WkEqWGW2C, -3, +3); //
  ElActfSt->SparseFC(WkEqWGW3, -3, +3);
  ElActfSt->SparseFC(WkEqWGW3C, -3, +3); //
  ElActfSt->SparseFC(WkEqWGW4, -3, +3);
  ElActfSt->SparseFC(WkEqWGW4C, -3, +3); //
  // patch: repeat instead of using weight
  /*
  ElActfSt->SparseFC(WkEqWGW1);
  ElActfSt->SparseFC(WkEqWGW1C); //
  ElActfSt->SparseFC(WkEqWGW2);
  ElActfSt->SparseFC(WkEqWGW2C); //
  ElActfSt->SparseFC(WkEqWGW3);
  ElActfSt->SparseFC(WkEqWGW3C); //
  ElActfSt->SparseFC(WkEqWGW4);
  ElActfSt->SparseFC(WkEqWGW4C); //

  ElActfSt->SparseFC(WkEqWGW1);
  ElActfSt->SparseFC(WkEqWGW1C); //
  ElActfSt->SparseFC(WkEqWGW2);
  ElActfSt->SparseFC(WkEqWGW2C); //
  ElActfSt->SparseFC(WkEqWGW3);
  ElActfSt->SparseFC(WkEqWGW3C); //
  ElActfSt->SparseFC(WkEqWGW4);
  ElActfSt->SparseFC(WkEqWGW4C); //
  //
  */
  ElActfSt->SparseFC(PrevWkEqWGW1);
  ElActfSt->SparseFC(PrevWkEqWGW1C); //
  ElActfSt->SparseFC(PrevWkEqWGW2);
  ElActfSt->SparseFC(PrevWkEqWGW2C); //
  ElActfSt->SparseFC(PrevWkEqWGW3);
  ElActfSt->SparseFC(PrevWkEqWGW3C); //
  ElActfSt->SparseFC(PrevWkEqWGW4);
  ElActfSt->SparseFC(PrevWkEqWGW4C); //
  //ElActfSt->SparseFC(CW->Default, false);
  //ElActfSt->SparseFC(InPhB->RowDefault, false);
  //ElActfSt->SparseFC(WkPhB->RowDefault, false);
  //ElActfSt->SparseFC(WGB->RowDefault, false);
  ElActfSt->SparseFC(OutPhB);
  ElActfSt->SparseFC(OutPhB->RowDefault,false);
  ElActfSt->SparseFC(OutEqW);
  ElActfSt->SparseFC(OutEqWC); //
  //ElActfSt->SparseFC(OutPhB);
  //ElActfSt->SparseFC(OutPhB->RowDefault, false);
  //ElActfSt->SparseFC(OutPhB);
  //ElActfSt->SparseFC(OutPhB->RowDefault, false);
  //ElActfSt->SparseFC(OutPhB);
  //ElActfSt->SparseFC(OutPhB->RowDefault, false);
  ElActfSt->SparseFC(WkOutPhB);
  ElActfSt->SparseFC(WkOutPhB->RowDefault, false);
  ElActfSt->SparseFC(WkOutEqWGW1);
  ElActfSt->SparseFC(WkOutEqWGW1C);
  ElActfSt->SparseFC(WkOutEqWGW2);
  ElActfSt->SparseFC(WkOutEqWGW2C);
  ElActfSt->SparseFC(WkOutEqWGW3);
  ElActfSt->SparseFC(WkOutEqWGW3C);
  ElActfSt->SparseFC(WkOutEqWGW4);
  ElActfSt->SparseFC(WkOutEqWGW4C);
  // goals SSM
  ElActfSt->SparseFC(GoalWG);
  ElActfSt->SparseFC(GoalWG->RowDefault,false); //
  ElActfSt->SparseFC(GoalPh);
  ElActfSt->SparseFC(GoalPh->RowDefault,false); //
  ElActfSt->SparseFC(WkEqGoalWGW1, -5, 5);
  ElActfSt->SparseFC(WkEqGoalWGW1C, -5, 5);
  ElActfSt->SparseFC(WkEqGoalWGW2, -5, 5);
  ElActfSt->SparseFC(WkEqGoalWGW2C, -5, 5);
  ElActfSt->SparseFC(WkEqGoalWGW3, -5, 5);
  ElActfSt->SparseFC(WkEqGoalWGW3C, -5, 5);
  ElActfSt->SparseFC(WkEqGoalWGW4, -5, 5);
  ElActfSt->SparseFC(WkEqGoalWGW4C, -5, 5);

  // patch: repeat instead of using weight
  /*
  ElActfSt->SparseFC(WkEqGoalWGW1);
  ElActfSt->SparseFC(WkEqGoalWGW1C);
  ElActfSt->SparseFC(WkEqGoalWGW2);
  ElActfSt->SparseFC(WkEqGoalWGW2C);
  ElActfSt->SparseFC(WkEqGoalWGW3);
  ElActfSt->SparseFC(WkEqGoalWGW3C);
  ElActfSt->SparseFC(WkEqGoalWGW4);
  ElActfSt->SparseFC(WkEqGoalWGW4C);

  ElActfSt->SparseFC(WkEqGoalWGW1);
  ElActfSt->SparseFC(WkEqGoalWGW1C);
  ElActfSt->SparseFC(WkEqGoalWGW2);
  ElActfSt->SparseFC(WkEqGoalWGW2C);
  ElActfSt->SparseFC(WkEqGoalWGW3);
  ElActfSt->SparseFC(WkEqGoalWGW3C);
  ElActfSt->SparseFC(WkEqGoalWGW4);
  ElActfSt->SparseFC(WkEqGoalWGW4C);
  
  ElActfSt->SparseFC(WkEqGoalWGW1);
  ElActfSt->SparseFC(WkEqGoalWGW1C);
  ElActfSt->SparseFC(WkEqGoalWGW2);
  ElActfSt->SparseFC(WkEqGoalWGW2C);
  ElActfSt->SparseFC(WkEqGoalWGW3);
  ElActfSt->SparseFC(WkEqGoalWGW3C);
  ElActfSt->SparseFC(WkEqGoalWGW4);
  ElActfSt->SparseFC(WkEqGoalWGW4C);
  
  ElActfSt->SparseFC(WkEqGoalWGW1);
  ElActfSt->SparseFC(WkEqGoalWGW1C);
  ElActfSt->SparseFC(WkEqGoalWGW2);
  ElActfSt->SparseFC(WkEqGoalWGW2C);
  ElActfSt->SparseFC(WkEqGoalWGW3);
  ElActfSt->SparseFC(WkEqGoalWGW3C);
  ElActfSt->SparseFC(WkEqGoalWGW4);
  ElActfSt->SparseFC(WkEqGoalWGW4C);
  
  ElActfSt->SparseFC(WkEqGoalWGW1);
  ElActfSt->SparseFC(WkEqGoalWGW1C);
  ElActfSt->SparseFC(WkEqGoalWGW2);
  ElActfSt->SparseFC(WkEqGoalWGW2C);
  ElActfSt->SparseFC(WkEqGoalWGW3);
  ElActfSt->SparseFC(WkEqGoalWGW3C);
  ElActfSt->SparseFC(WkEqGoalWGW4);
  ElActfSt->SparseFC(WkEqGoalWGW4C);
  */

  //ElActfSt->SparseFC(WkEqGoalWGW1);
  //ElActfSt->SparseFC(WkEqGoalWGW1C);
  //ElActfSt->SparseFC(WkEqGoalWGW2);
  //ElActfSt->SparseFC(WkEqGoalWGW2C);
  //ElActfSt->SparseFC(WkEqGoalWGW3);
  //ElActfSt->SparseFC(WkEqGoalWGW3C);
  //ElActfSt->SparseFC(WkEqGoalWGW4);
  //ElActfSt->SparseFC(WkEqGoalWGW4C);
  //  
  ElActfSt->SparseFC(GoalPhEqGoalWGW1); //, -2, 2);
  ElActfSt->SparseFC(GoalPhEqGoalWGW1C); //, -2, 2);
  ElActfSt->SparseFC(GoalPhEqGoalWGW2); //, -2, 2);
  ElActfSt->SparseFC(GoalPhEqGoalWGW2C); //, -2, 2);
  ElActfSt->SparseFC(GoalPhEqGoalWGW3); //, -2, 2);
  ElActfSt->SparseFC(GoalPhEqGoalWGW3C); //, -2, 2);
  ElActfSt->SparseFC(GoalPhEqGoalWGW4); //, -2, 2);
  ElActfSt->SparseFC(GoalPhEqGoalWGW4C); //, -2, 2);

  ElActfSt->SparseFC(GoalI);
  ElActfSt->SparseFC(GoalI->Default, false);
  //

  ElActfSt->SparseFC(ActFlags);
  ElActfSt->SparseFC(ActFlagsC); //
  // connect to complementary SSMs
  //ElActfSt->SparseFC(InEqWC);
  //ElActfSt->SparseFC(WkEqWC);
  //ElActfSt->SparseFC(WGEqWC);
  //ElActfSt->SparseFC(InEqWGW1C);
  //ElActfSt->SparseFC(InEqWGW2C);
  //ElActfSt->SparseFC(InEqWGW3C);
  //ElActfSt->SparseFC(InEqWGW4C);
  //ElActfSt->SparseFC(WkEqWGW1C);
  //ElActfSt->SparseFC(WkEqWGW2C);
  //ElActfSt->SparseFC(WkEqWGW3C);
  //ElActfSt->SparseFC(WkEqWGW4C);
  //ElActfSt->SparseFC(ActFlagsC);

  ElActfSt->OutFC(ElActFL,0);
  ElActfSt->UseDynamicBias(-500, 500);
  //ElAct->FC(RetrElActfSt, -BIGWG);
  SetNullElAct = new ssm(1);
  ElAct->FC(SetNullElAct, -BIGWG);

  //st act mem
  StActI = new XSSM(StActSize);
  CurrStActI = new XSSM(StActSize);
  NextStActI = new XSSM(StActSize);
  StartStActIFlag = new ssm(1);
  CurrStActIFlag = new ssm(1);
  NextStActIFlag = new ssm(1);
  CurrStActI->SC(StActI, 1);
  CurrStActI->FC(CurrStActIFlag, 1);
  CurrStActI->SB(-1.5); 
  NextStActI->SC(StActI, 1);
  NextStActI->FC(NextStActIFlag, 1);
  NextStActI->SB(-1.5);
  StActI->FC(StartStActIFlag, -BIGWG);
  StActI->SC(CurrStActI, 1);
  for (int i=1; i<StActSize; i++) {
    StActI->Nr[i]->CN(NextStActI->Nr[i-1], 1);
  }
  StActI->Nr[0]->CN(NextStActIFlag, 1); // if StActI=0 and NextStActI is
  StActI->Nr[0]->CN(NextStActI, -1);    // selected => PhI=1
  StActI->SB(-0.5); // PhI is zero if neither CurrPhI nor NextPhI are selected
  // Provare a semplificare usando il default !!!!!

  StoreStActIFlag = new ssm(1);
  StoredStActIFL = new XSSM(StActSize);
  StoredStActI = new XSSM(StActSize);
  CurrStoredStActI = new XSSM(StActSize);
  RetrStActIFlag = new ssm(1);
  RetrStActI = new XSSM(StActSize);

  StoredStActIFL->SC(StActI, 1);
  StoredStActIFL->FC(StoreStActIFlag, 1);
  StoredStActIFL->SB(-1.5);
  StoredStActI->SC(CurrStoredStActI, 1);
  StoredStActI->SC(StoredStActIFL, 1);
  StoredStActI->SB(-0.5);

  CurrStoredStActI->SC(StoredStActI, 1);
  CurrStoredStActI->FC(StoreStActIFlag, -BIGWG);
  CurrStoredStActI->SB(-0.5);

  RetrStActI->SC(StoredStActI, 1);
  RetrStActI->FC(RetrStActIFlag, 1);
  RetrStActI->SB(-1.5);
  StActI->SC(RetrStActI, 1);
  //CurrWkPh->FC(RetrStActIFlag, -BIGWG); it's enough to put CurrStActIFlag=0





  RetrStAct = new ssm(1);
  BuildStAct = new ssm(1);
  RetrStAct->Nr[0]->O = 0;
  BuildStAct->Nr[0]->O = 0;
  StActMem = new XSSM_AS(StActSize, T_WNN, BuildStAct->Nr[0], RetrStAct->Nr[0]);
  StActMem->SC(StActI, 1); //-Wgm, Wgm);
  StActMem->SetSparseOut();
  StActMem->SparseOutFC(CW);
  StActMem->SparseOutFC(PhI);
  StActMem->SparseOutFC(InI);
  StActMem->SparseOutFC(InEqW);
  StActMem->SparseOutFC(InPhB);
  //StActMem->SparseOutFC(WkI);
  StActMem->SparseOutFC(WkEqW);
  StActMem->SparseOutFC(WkPhB);
  StActMem->SparseOutFC(PrevWkPhFL);
  StActMem->SparseOutFC(PrevWkPh);
  StActMem->SparseOutFC(WGI);
  StActMem->SparseOutFC(WGIFL);
  StActMem->SparseOutFC(WGEqW);
  StActMem->SparseOutFC(WGB);
  StActMem->SparseOutFC(PrevWGFL);
  StActMem->SparseOutFC(PrevWG);
  StActMem->SparseOutFC(InEqWGW1);
  StActMem->SparseOutFC(InEqWGW2);
  StActMem->SparseOutFC(InEqWGW3);
  StActMem->SparseOutFC(InEqWGW4);
  StActMem->SparseOutFC(PrevInEqWGW1FL);
  StActMem->SparseOutFC(PrevInEqWGW2FL);
  StActMem->SparseOutFC(PrevInEqWGW3FL);
  StActMem->SparseOutFC(PrevInEqWGW4FL);
  StActMem->SparseOutFC(PrevInEqWGW1);
  StActMem->SparseOutFC(PrevInEqWGW2);
  StActMem->SparseOutFC(PrevInEqWGW3);
  StActMem->SparseOutFC(PrevInEqWGW4);
  StActMem->SparseOutFC(WkEqWGW1);
  StActMem->SparseOutFC(WkEqWGW2);
  StActMem->SparseOutFC(WkEqWGW3);
  StActMem->SparseOutFC(WkEqWGW4);
  StActMem->SparseOutFC(PrevWkEqWGW1FL);
  StActMem->SparseOutFC(PrevWkEqWGW2FL);
  StActMem->SparseOutFC(PrevWkEqWGW3FL);
  StActMem->SparseOutFC(PrevWkEqWGW4FL);
  StActMem->SparseOutFC(PrevWkEqWGW1);
  StActMem->SparseOutFC(PrevWkEqWGW2);
  StActMem->SparseOutFC(PrevWkEqWGW3);
  StActMem->SparseOutFC(PrevWkEqWGW4);
  StActMem->SparseOutFC(OutEqW);
  StActMem->SparseOutFC(OutPhB);
  StActMem->SparseOutFC(WkOutPhB);
  StActMem->SparseOutFC(WkOutEqWGW1);
  StActMem->SparseOutFC(WkOutEqWGW2);
  StActMem->SparseOutFC(WkOutEqWGW3);
  StActMem->SparseOutFC(WkOutEqWGW4);

  // goals SSM
  StActMem->SparseOutFC(GoalWG);
  StActMem->SparseOutFC(GoalPh);
  StActMem->SparseOutFC(WkEqGoalWGW1);
  StActMem->SparseOutFC(WkEqGoalWGW2);
  StActMem->SparseOutFC(WkEqGoalWGW3);
  StActMem->SparseOutFC(WkEqGoalWGW4);
  StActMem->SparseOutFC(GoalPhEqGoalWGW1);
  StActMem->SparseOutFC(GoalPhEqGoalWGW2);
  StActMem->SparseOutFC(GoalPhEqGoalWGW3);
  StActMem->SparseOutFC(GoalPhEqGoalWGW4);
  StActMem->SparseOutFC(GoalI);
  //
  StActMem->SparseOutFC(ActFlags);
  StActMem->SparseOutFC(ElActFL);
  StActMem->SparseOutFC(ElActfSt);
  //StActMem->SparseOutFC(CW->Default);
  //StActMem->SparseOutFC(InPhB->RowDefault);
  //StActMem->SparseOutFC(WkPhB->RowDefault);
  //StActMem->SparseOutFC(WGB->RowDefault);

  for (int iw=0; iw<StActSize; iw++) {
    StActMem->Nr[iw]->Used = 1;
  }
  StActMem->SB(-0.5);

  //phrase mem
  MemPh = new XSSM(MemPhSize);
  //MemPh->NewWnn(); // set to 1 the first neuron
  MemPh->Nr[0]->O = 1;
  MemPh->NHigh = 1;
  MemPh->HighVect.push_back(0);
  //cout << "nwnn: " << MemPh->Nr[0]->O << endl;
  CurrMemPh = new XSSM(MemPhSize);
  NextMemPh = new XSSM(MemPhSize);
  CurrMemPh->SC(MemPh, 1);
  CurrMemPh->FC(NewMemPhFlag, -1);
  CurrMemPh->SB(-0.5); 
  NextMemPh->SC(MemPh, 1);
  NextMemPh->FC(NewMemPhFlag, 1);
  NextMemPh->SB(-1.5);
  MemPh->SC(CurrMemPh, 1);
  for (int i=1; i<MemPhSize; i++) {
    MemPh->Nr[i]->CN(NextMemPh->Nr[i-1], 1);
  }
  MemPh->SB(-0.5); //MemPh is 0 if neither CurrMemPh nor NextMemPh are selected

  RetrRemPh->Nr[0]->O = 0;
  RemPh = new XSSM_AS(MemPhSize, T_WNN, NewMemPhFlag->Nr[0], RetrRemPh->Nr[0]);
  RemPhIL  = new XSSM(MemPhSize);
  CurrRemPh = new XSSM(MemPhSize);
  NextRemPh = new XSSM(MemPhSize);

  //StActMem->SparseOutFC(CurrRemPh);
  StActMem->SparseOutFC(RemPh);

  RemPhIL->SC(MemPh, 1);
  RemPhIL->FC(NewMemPhFlag, 1);
  RemPhIL->SB(-1.5); 

  CurrRemPh->SC(RemPh, 1);
  CurrRemPh->FC(CurrRemPhFlag, 1);
  CurrRemPh->SB(-1.5); 
  NextRemPh->SC(RemPh, 1);
  NextRemPh->FC(NextRemPhFlag, 1);
  NextRemPh->SB(-1.5);
  RemPh->SC(RemPhIL, 1);
  RemPh->SC(CurrRemPh, 1);
  for (int i=1; i<MemPhSize; i++) {
    RemPh->Nr[i]->CN(NextRemPh->Nr[i-1], 1);
  }
  for (int i=0; i<MemPhSize; i++) {
    RemPh->Nr[i]->Used = 1;  // should implement a function for this
  }
  RemPh->SB(-0.5); // RemPh is 0 if none of Build, Curr or Next are selected
  RemPh->SetSparseOut();
  RemPh->SparseOutFC(WkPhB);


  RemPhfWG = new XSSM_AS(WkPhfWGSize, T_NEW_WTA, BuildAs->Nr[0], RetrAs->Nr[0]);
  //RemPhfWG->OrderedWnnFlag = false;
  RemPhfWG->SetSparseIn();
  RemPhfWG->SetSparseOut();
  RemPhfWG->SparseFC(WGB);
  RemPhfWG->SparseFC(WGB->RowDefault, false);
  RemPhfWG->SparseOutFC(RemPh);

  //starting ph of ctx
  StartPhFlag->Nr[0]->O = 0;
  StartPh = new XSSM(MemPhSize);
  StartPhIL = new XSSM(MemPhSize);
  CurrStartPh = new XSSM(MemPhSize);

  StartPhIL->SC(MemPh, 1);
  StartPhIL->FC(StartPhFlag, 1);
  StartPhIL->SB(-1.5); 

  CurrStartPh->SC(StartPh, 1);
  CurrStartPh->FC(StartPhFlag, -1);
  CurrStartPh->SB(-0.5); 

  StartPh->SC(StartPhIL, 1);
  StartPh->SC(CurrStartPh, 1);
  StartPh->SB(-0.5);

  //retrieve starting ph of ctx
  RemStartPhFlag->Nr[0]->O = 0;
  CurrStartWkPhFlag->Nr[0]->O = 0;

  StartWkPhIL = new XSSM(MemPhSize);
  CurrStartWkPh = new XSSM(MemPhSize);
  StartWkPh = new XSSM(MemPhSize);
  RemStartPh = new XSSM(MemPhSize);

  StartWkPhIL->SC(StartPh, 1);
  StartWkPhIL->FC(NewMemPhFlag,1);
  StartWkPhIL->SB(-1.5); 

  CurrStartWkPh->SC(StartWkPh, 1);
  CurrStartWkPh->FC(CurrStartWkPhFlag, 1);
  CurrStartWkPh->SB(-1.5); 

  StartWkPh->SC(StartWkPhIL, 1);
  StartWkPh->SC(CurrStartWkPh, 1);
  StartWkPh->SB(-0.5);
  RemStartPh->SC(StartWkPh, 1);
  RemStartPh->FC(RemStartPhFlag, 1);
  RemStartPh->SB(-1.5);

  RemPh->SC(RemStartPh, 1);
  RemPh->SparseOutFC(StartWkPh);

  RemPh->FC(FlushWk, -BIGWG);
  StartWkPh->FC(FlushWk, -BIGWG);
  //
  StActMem->SparseOutFC(StartWkPh);
  //

  // reward system actions and flags
  BuildRewardFlag = new ssm(1);
  PrevRetrAsFlag = new ssm(1);
  RepeatRetrAsFlag = new ssm(1);
  PrevRetrAsFlag->Nr[0]->CN(ElActFL->Nr[RETR_AS-1], 1);
  PrevRetrAsFlag->SB(-0.5);
  RepeatRetrAsFlag->Nr[0]->CN(ElActFL->Nr[RETR_AS-1], 1);
  RepeatRetrAsFlag->SC(PrevRetrAsFlag, 1);
  RepeatRetrAsFlag->SB(-1.5);
  BuildElActfSt->SC(BuildRewardFlag, 1);
  BuildElActfSt->SC(RepeatRetrAsFlag, -1);
  BuildElActfSt->SB(-0.5);
  RwdAct = new ssm(RwdActSize);
  RwdAct->UseDefault();
  RwdAct->SB(-0.5);

  // Free slots for additionan action flags
  //RwdFlag10 = new ssm(1);

  RwdFlags = new ssm();

  RwdFlags->Nr.push_back(StartStActIFlag->Nr[0]);
  RwdFlags->Nr.push_back(CurrStActIFlag->Nr[0]);
  RwdFlags->Nr.push_back(NextStActIFlag->Nr[0]);
  RwdFlags->Nr.push_back(RetrStAct->Nr[0]);
  RwdFlags->Nr.push_back(BuildStAct->Nr[0]);
  RwdFlags->Nr.push_back(RetrElActfSt->Nr[0]);
  RwdFlags->Nr.push_back(BuildRewardFlag->Nr[0]);
  RwdFlags->Nr.push_back(StoreStActIFlag->Nr[0]);
  RwdFlags->Nr.push_back(RetrStActIFlag->Nr[0]);
  RwdFlags->Nr.push_back(SetNullElAct->Nr[0]);
  //RwdFlags->Nr.push_back(RwdFlag10->Nr[0]);
  RwdFlags->FC(RwdAct, 0);
  RwdFlags->FC(RwdAct->Default, 0);
  int rwd_act_matr[RwdActSize][RwdFlagSize] = {
    // StrSA CurSA NxtSA RtrSA BldSA RtrAS BldRw StrSI RtrSI NullA
    {   -1,   -1,   +1,   -1,   +1,   -1,   -1,   -1,   -1,   -1}, // STORE_ST_A

    {   +1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1}, // START_ST_A

    {   -1,   -1,   +1,   +1,   -1,   -1,   -1,   -1,   -1,   -1}, // RETR_ST_A

    {   -1,   -1,   +1,   +1,   -1,   -1,   +1,   -1,   -1,   -1}, // RWD_ST_A

    {   -1,   +1,   -1,   -1,   +1,   -1,   -1,   -1,   -1,   -1},//CHANGE_ST_A

    {   -1,   +1,   -1,   +1,   -1,   -1,   -1,   -1,   -1,   -1}, // GET_ST_A

    {   -1,   +1,   -1,   -1,   -1,   +1,   -1,   -1,   -1,   +1}, // RETR_EL_A

    {   -1,   +1,   -1,   -1,   -1,   -1,   -1,   +1,   -1,   -1}, // STORE_SAI

    {   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   +1,   -1}, // RETR_SAI

    {   -1,   -1,   +1,   +1,   -1,   -1,   -1,   -1,   -1,   +1}, // RETR_ST

    {   -1,   +1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1}, //RWD_ACT_10

    {   -1,   +1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1}, //RWD_ACT_11

    {   -1,   +1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1}, //RWD_ACT_12

    {   -1,   +1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1}, //RWD_ACT_13

    {   -1,   +1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1}, //RWD_ACT_14
  };

  for (int ia=0; ia<RwdActSize; ia++) {
    for (int iaf=0; iaf<RwdFlagSize; iaf++) {
      RwdFlags->Nr[iaf]->L[ia].Wg = rwd_act_matr[ia][iaf];
    }
  }

  int null_rwd_act_vect[RwdFlagSize] =
    // StrSA CurSA NxtSA RtrSA BldSA RtrAS BldAS Flg7  Flg8  Flg9
    {   -1,   +1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1};

  for (int iaf=0; iaf<RwdFlagSize; iaf++) {
    RwdFlags->Nr[iaf]->L[RwdActSize].Wg = null_rwd_act_vect[iaf];
  }

  for (int iaf=0;  iaf<RwdFlagSize; iaf++) {
    RwdFlags->Nr[iaf]->Used = 1;
  }
  RwdFlags->SB(-0.5); // all flags are zero when no act is selected

  // Operation Modes
  Acquire = new ssm(1);
  Associate = new ssm(1);
  Explore = new ssm(1);
  Explore2 = new ssm(1);
  Exploit = new ssm(1);
  BestExploit = new ssm(1);
  Reward = new ssm(1);

  ModeFlags = new ssm();
  ModeFlags->Nr.push_back(Acquire->Nr[0]);
  ModeFlags->Nr.push_back(Associate->Nr[0]);
  ModeFlags->Nr.push_back(Explore->Nr[0]);
  ModeFlags->Nr.push_back(Explore2->Nr[0]);
  ModeFlags->Nr.push_back(Exploit->Nr[0]);
  ModeFlags->Nr.push_back(BestExploit->Nr[0]);
  ModeFlags->Nr.push_back(Reward->Nr[0]);

  AcquireArch();
  AssociateArch();
  RewardArch();
  ExploitArch();
  ExploreArch();

  AddRef();
  ElActfSt->OMPActivArrInit();
}

int sllm::In(int *w, int *phi)
{
  W->Fill(w);
  PhI->Fill(phi);

  return 0;
}

int sllm::PhUpdate()
{
  //level 10
  GetGoalPhFL->ActivOut();
  CurrWkPh->ActivOut();
  PrevWkPh->ActivOut();
  PrevWG->ActivOut();
  PrevInEqWGW1->ActivOut();
  PrevInEqWGW2->ActivOut();
  PrevInEqWGW3->ActivOut();
  PrevInEqWGW4->ActivOut();
  PrevWkEqWGW1->ActivOut();
  PrevWkEqWGW2->ActivOut();
  PrevWkEqWGW3->ActivOut();
  PrevWkEqWGW4->ActivOut();

  //level 11
  if (ModeFlags->Nr[0]->O>0.5) // just to save time
    IW->Activ();
  InI->ActivOut();
  //WkI->ActivOut();
  WGIFL->ActivOut();
  if (ModeFlags->Nr[0]->O>0.5) // just to save time
    IW->WTA();
  PrevWkPhFL->ActivOut();
  PrevWGFL->ActivOut();
  PrevInEqWGW1FL->ActivOut();
  PrevInEqWGW2FL->ActivOut();
  PrevInEqWGW3FL->ActivOut();
  PrevInEqWGW4FL->ActivOut();
  PrevWkEqWGW1FL->ActivOut();
  PrevWkEqWGW2FL->ActivOut();
  PrevWkEqWGW3FL->ActivOut();
  PrevWkEqWGW4FL->ActivOut();

  //level 12
  InPhFL->ActivOut();
  //WkPhFL->ActivOut();

  //level 13
  InPhB->ActivOut();
  //WkPhB->ActivOut();

  //level 13b
  WkPhFL->ActivOut(); ////// moved here from l12

  //level 13c
  WkPhB->ActivOut(); ////// moved here from l13

  //level 14
  //InWfI->ActivOut();
  WkWfI->ActivOut();
  //if (PrevWkPh->NHigh>0) {
  //  for (int i=0; i<PrevWkPh->NN(); i++) {
  //    if (PrevWkPh->Nr[i]->O>0.5) cout << "PrevWkPh : " << i << endl;
  //  }
  //}

  //level 15
  CW->ActivOut();

  //level 16
  InIfW->ActivOut();
  WGCW->ActivOut();

  //level 17
  InEqW->ActivOut();
  WGFL->ActivOut();

  //level 18
  WGB->ActivOut();

  //level 19
  WGIfW->ActivOut();
  OutPhFL->ActivOut();
  WkOutPhFL->ActivOut();
  WkOutEqWGW1FL->ActivOut();
  WkOutEqWGW2FL->ActivOut();
  WkOutEqWGW3FL->ActivOut();
  WkOutEqWGW4FL->ActivOut();

  InIfWGW1->ActivOut();
  InIfWGW2->ActivOut();
  InIfWGW3->ActivOut();
  InIfWGW4->ActivOut();
  //if (NewMemPhFlag->Nr[0]->O==1 || NextRemPhFlag->Nr[0]->O==1 ||
  //    RemStartPhFlag->Nr[0]->O==1 || RetrAs->Nr[0]->O==1 ||
  //    FlushWk->Nr[0]->O==1) // just to save time. Check

  MemPhUpdate();
  AsUpdate();

  //level 20
  WGEqW->ActivOut();
  OutPhB->ActivOut();
  WkOutPhB->ActivOut();
  WkOutEqWGW1->ActivOut();
  WkOutEqWGW2->ActivOut();
  WkOutEqWGW3->ActivOut();
  WkOutEqWGW4->ActivOut();

  InEqWGW1->ActivOut();
  InEqWGW2->ActivOut();
  InEqWGW3->ActivOut();
  InEqWGW4->ActivOut();

  //level 21
  WkIfW->ActivOut();
  OutIfW->ActivOut();
  WkIfWGW1->ActivOut();
  WkIfWGW2->ActivOut();
  WkIfWGW3->ActivOut();
  WkIfWGW4->ActivOut();

  //level 22
  OutEqW->ActivOut();
  WkEqW->ActivOut();
  WkEqWGW1->ActivOut();
  WkEqWGW2->ActivOut();
  WkEqWGW3->ActivOut();
  WkEqWGW4->ActivOut();

  //level 23

  return 0;
}

int sllm::AsUpdate()
{
  //level 19
  RemPh->Activ();
  RemPh->Out();
  RemPhfWG->ActivOut();
  RemPh->AsOut();
  // RetrRemPh->Nr[0]->O=0; // Temporary. Remove

  return 0;
}

int sllm::StActMemUpdate()
{
  CurrStoredStActI->ActivOut();
  StoredStActIFL->ActivOut();
  StoredStActI->ActivOut();
  RetrStActI->ActivOut();
  CurrStActI->ActivOut();
  NextStActI->ActivOut();
  StActI->ActivOut();
  //cout << "StActI: ";
  //for (int i=0; i<StActI->NN(); i++) {
  //  if (StActI->Nr[i]->O>0.5) cout << i << " ";
  //}
  //cout << endl;

  StActMem->ActivOut();
  RetrStAct->Nr[0]->O = 0;
  BuildStAct->Nr[0]->O = 0;

  return 0;
}

int sllm::ActUpdate()
{
  ActFlags->ActivOut();
  CurrPhI->ActivOut();
  NextPhI->ActivOut();
  PhI->ActivOut();
  CurrWGI->ActivOut();
  NextWGI->ActivOut();
  WGI->ActivOut();

  return 0;
}

int sllm::ElActfStUpdate()
{
  // just to save time
  if (RetrElActfSt->Nr[0]->O<0.5 && BuildElActfSt->Nr[0]->O<0.5) return 0;

  // complementary SSMs update
  InEqWC->ActivOut();
  WkEqWC->ActivOut();
  OutEqWC->ActivOut();
  WGEqWC->ActivOut();
  InEqWGW1C->ActivOut();
  InEqWGW2C->ActivOut();
  InEqWGW3C->ActivOut();
  InEqWGW4C->ActivOut();
  WkEqWGW1C->ActivOut();
  WkEqWGW2C->ActivOut();
  WkEqWGW3C->ActivOut();
  WkEqWGW4C->ActivOut();
  PrevInEqWGW1C->ActivOut();
  PrevInEqWGW2C->ActivOut();
  PrevInEqWGW3C->ActivOut();
  PrevInEqWGW4C->ActivOut();
  PrevWkEqWGW1C->ActivOut();
  PrevWkEqWGW2C->ActivOut();
  PrevWkEqWGW3C->ActivOut();
  PrevWkEqWGW4C->ActivOut();
  WkOutEqWGW1C->ActivOut();
  WkOutEqWGW2C->ActivOut();
  WkOutEqWGW3C->ActivOut();
  WkOutEqWGW4C->ActivOut();
  WkEqGoalWGW1C->ActivOut();
  WkEqGoalWGW2C->ActivOut();
  WkEqGoalWGW3C->ActivOut();
  WkEqGoalWGW4C->ActivOut();
  GoalPhEqGoalWGW1C->ActivOut();
  GoalPhEqGoalWGW2C->ActivOut();
  GoalPhEqGoalWGW3C->ActivOut();
  GoalPhEqGoalWGW4C->ActivOut();

  ActFlagsC->ActivOut();

  //cout << "elactfs ";
  /*
  if (StActI->HighVect[0]==1 && RetrElActfSt->Nr[0]->O==1) {
    cout << "elactfs ";
    for (unsigned int i=0; i<ElActfSt->SparseInSSM.size(); i++) {
      cout << " " << i <<":";
      vssm *ssm1 = ElActfSt->SparseInSSM[i];
      for (int j=0; j<ssm1->NN(); j++) {
	if (ssm1->Nr[j]->O>0.5) cout << j << ";";
      }
    }
    cout << endl;
  }
  if (RetrElActfSt->Nr[0]->O==1) {
    int nn=0;
    cout << "nn ";
    for (unsigned int i=0; i<ElActfSt->SparseInSSM.size(); i++) {
      vssm *ssm1 = ElActfSt->SparseInSSM[i];
      for (int j=0; j<ssm1->NN(); j++) {
	if (ssm1->Nr[j]->O>0.5) nn++;
      }
    }
    cout << nn << endl;
    
    if (nn==30 || nn==18) {
      cout << "nn " << nn << "  ";
      for (unsigned int i=0; i<ElActfSt->SparseInSSM.size(); i++) {
	cout << " " << i <<":";
	vssm *ssm1 = ElActfSt->SparseInSSM[i];
	int nn1 = 0;
	for (unsigned int j=0; j<ssm1->HighVect.size(); j++) {
	  nn1++;
	}
	cout << nn1 << " ";
      }
      cout << endl;
      cout << "CW:";
      for (unsigned int i=0; i<CW->Nr.size(); i++) {
	if (CW->Nr[i]->O > 0.5) cout << " " << i;
      }
      cout << endl;
      cout << "def " << CW->Default->Nr[0]->O << endl;
    }
    
  }
  */
  //ElActfSt->ActivOut();
#ifdef CUDA
  if (CudaFlag) ElActfSt->cuda_SparseActiv();
  else
#endif
    ElActfSt->OMPSparseActiv();
  ElActfSt->AsOut();
  // test remove ////////////////////
  //if (RetrElActfSt->Nr[0]->O>0.5 &&  BuildElActfSt->Nr[0]->O<0.5) {
  //  ElActfSt->DisplayWnn();
  //}
  ///////////////////////////////////
  RetrElActfSt->Nr[0]->O = 0;
  BuildElActfSt->Nr[0]->O = 0;

  return 0;
}

int sllm::MemPhUpdate()
{
  // level b11
  CurrMemPh->ActivOut();
  NextMemPh->ActivOut();

  // level b12
  MemPh->ActivOut();

  // level b13
  RemPhIL->ActivOut();
  CurrRemPh->ActivOut();
  NextRemPh->ActivOut();
  RemStartPh->ActivOut();

  StartPhIL->ActivOut();
  CurrStartPh->ActivOut();

  // level b14
  StartPh->ActivOut();

  // level b15
  StartWkPhIL->ActivOut();
  CurrStartWkPh->ActivOut();

  // level b16

  StartWkPh->Activ();
  StartWkPh->Out();

  return 0;
}

int sllm::Update()
{
  //TEMPORARY PATCH!!!!
  if (ElAct->Default->Nr[0]->O>0.5 && AcqAct->Default->Nr[0]->O>0.5) return 0;
  ActUpdate();
  PhUpdate();
  GoalUpdate();

  return 0;
}


int sllm::StActRwdUpdate()
{
  //RwdFlags->ActivOut();
  //StActMemUpdate();
  //ElActfStUpdate();

  RwdFlags->ActivOut();
  StActMemUpdate();
  RepeatRetrAsFlag->ActivOut();
  PrevRetrAsFlag->ActivOut();
  BuildElActfSt->ActivOut();
  ElAct->ActivOut();
  ElActfStUpdate();

  return 0;
}

