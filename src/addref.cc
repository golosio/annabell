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

using namespace std;
using namespace sizes;

int sllm::AddRef()
{
#ifdef FAST_SSM
  InPhFL->AddSparseRefRows(IW);
  InPhB->AddSparseRef(InPhB);
  InPhB->AddSparseRef(InPhFL);
  //InWfI->AddSparseRef(InPhB);
  InIfW->AddSparseRef(InPhB);
  //CW->AddSparseRefToRows(InWfI);
  InEqW->AddSparseRefToColumns(InIfW);
  InIfWGW1->AddSparseRef(InPhB);
  InIfWGW2->AddSparseRef(InPhB);
  InIfWGW3->AddSparseRef(InPhB);
  InIfWGW4->AddSparseRef(InPhB);
  InEqWGW1->AddSparseRefToColumns(InIfWGW1);
  InEqWGW2->AddSparseRefToColumns(InIfWGW2);
  InEqWGW3->AddSparseRefToColumns(InIfWGW3);
  InEqWGW4->AddSparseRefToColumns(InIfWGW4);

  WkPhFL->AddSparseRef(InPhB); //WkPhFL->AddSparseRefRows(IW);
  WkPhB->AddSparseRef(CurrWkPh);
  CurrWkPh->AddSparseRef(WkPhB);
  WkPhB->AddSparseRef(WkPhFL);
  WkWfI->AddSparseRef(WkPhB);
  WkIfW->AddSparseRef(WkPhB);
  WkEqW->AddSparseRefToColumns(WkIfW);
  CW->AddSparseRefToRows(WkWfI);
  WkIfWGW1->AddSparseRef(WkPhB);
  WkIfWGW2->AddSparseRef(WkPhB);
  WkIfWGW3->AddSparseRef(WkPhB);
  WkIfWGW4->AddSparseRef(WkPhB);
  WkEqWGW1->AddSparseRefToColumns(WkIfWGW1);
  WkEqWGW2->AddSparseRefToColumns(WkIfWGW2);
  WkEqWGW3->AddSparseRefToColumns(WkIfWGW3);
  WkEqWGW4->AddSparseRefToColumns(WkIfWGW4);
  PrevWkPhFL->AddSparseRef(WkPhB);
  PrevWkPh->AddSparseRef(PrevWkPh);
  PrevWkPh->AddSparseRef(PrevWkPhFL);

  OutPhFL->AddSparseRef(WGB);
  OutPhB->AddSparseRef(OutPhB);
  OutPhB->AddSparseRef(OutPhFL);
  OutIfW->AddSparseRef(OutPhB);
  OutEqW->AddSparseRefToColumns(OutIfW);

  WkOutPhFL->AddSparseRef(WkPhB);
  WkOutPhB->AddSparseRef(WkOutPhB);
  WkOutPhB->AddSparseRef(WkOutPhFL);

  WGCW->AddSparseRef(CW);
  WGFL->AddSparseRefRows(WGCW);
  WGB->AddSparseRef(WGB);
  WGB->AddSparseRef(WGFL);
  WGIfW->AddSparseRef(WGB);
  WGEqW->AddSparseRefToColumns(WGIfW);
  PrevWGFL->AddSparseRef(WGB);
  PrevWG->AddSparseRef(PrevWG);
  PrevWG->AddSparseRef(PrevWGFL);

  CurrStActI->AddSparseRef(StActI);
  NextStActI->AddSparseRef(StActI);
  StActI->AddSparseRef(CurrStActI);
  StActI->AddSparseRef(NextStActI, 1);
  StActI->AddSparseRefNum(0);

  StoredStActIFL->AddSparseRef(StActI); 
  StoredStActI->AddSparseRef(CurrStoredStActI);
  StoredStActI->AddSparseRef(StoredStActIFL);
  CurrStoredStActI->AddSparseRef(StoredStActI);

  RetrStActI->AddSparseRef(StoredStActI);
  StActI->AddSparseRef(RetrStActI);

  CurrMemPh->AddSparseRef(MemPh);
  NextMemPh->AddSparseRef(MemPh);
  MemPh->AddSparseRef(CurrMemPh);
  MemPh->AddSparseRef(NextMemPh, 1);
  
  RemPhIL->AddSparseRef(MemPh);
  CurrRemPh->AddSparseRef(RemPh);
  NextRemPh->AddSparseRef(RemPh);
  
  StartPhIL->AddSparseRef(MemPh);
  CurrStartPh->AddSparseRef(StartPh);
  StartPh->AddSparseRef(StartPhIL);
  StartPh->AddSparseRef(CurrStartPh);
  
  StartWkPhIL->AddSparseRef(StartPh);
  CurrStartWkPh->AddSparseRef(StartWkPh);
  StartWkPh->AddSparseRef(CurrStartWkPh);
  StartWkPh->AddSparseRef(StartWkPhIL);
  
  RemStartPh->AddSparseRef(StartWkPh);
  // uncomment to check null in from FC to flags:
  InPhB->AddSparseRefNum(0);
  WkPhB->AddSparseRefNum(0);
  OutPhFL->AddSparseRefNum(0);
  OutPhB->AddSparseRefNum(0);
  WGCW->AddSparseRefNum(0);
  WGB->AddSparseRefNum(0);
  CurrStActI->AddSparseRefNum(0);
  NextStActI->AddSparseRefNum(0);
  StActI->AddSparseRefNum(0);
  CurrMemPh->AddSparseRefNum(0);
  NextMemPh->AddSparseRefNum(0);
  RemPhIL->AddSparseRefNum(0);
  CurrRemPh->AddSparseRefNum(0);
  NextRemPh->AddSparseRefNum(0);
  StartPhIL->AddSparseRefNum(0);
  CurrStartPh->AddSparseRefNum(0);
  StartWkPhIL->AddSparseRefNum(0);
  CurrStartWkPh->AddSparseRefNum(0);
  RemStartPh->AddSparseRefNum(0);
  StartWkPh->AddSparseRefNum(0);
  // complementary SSMs THEY ARE NOT XSSMs!!!
  //InEqWC->AddSparseRef(InEqW);
  //WkEqWC->AddSparseRef(WkEqW);
  //WGEqWC->AddSparseRef(WGEqW);
  //InEqWGW1C->AddSparseRef(InEqWGW1);
  //InEqWGW2C->AddSparseRef(InEqWGW2);
  //InEqWGW3C->AddSparseRef(InEqWGW3);
  //InEqWGW4C->AddSparseRef(InEqWGW4);
  //WkEqWGW1C->AddSparseRef(WkEqWGW1);
  //WkEqWGW2C->AddSparseRef(WkEqWGW2);
  //WkEqWGW3C->AddSparseRef(WkEqWGW3);
  //WkEqWGW4C->AddSparseRef(WkEqWGW4);

#endif
#ifdef FAST_SSM_AS
  
  ElActfSt->AddSparseFCRef(CW);
  ElActfSt->AddSparseFCRef(CW->Default);
  ElActfSt->AddSparseFCRef(InI);
  ElActfSt->AddSparseFCRef(InI->Default);
  ElActfSt->AddSparseFCRef(InEqW);
  ElActfSt->AddSparseFCRef(InEqWC); //
  ElActfSt->AddSparseFCRef(InPhB);
  ElActfSt->AddSparseFCRef(InPhB->RowDefault);
  //ElActfSt->AddSparseFCRef(WkI); /// MAYBE SHOULD PUT PhI HERE????????
  //ElActfSt->AddSparseFCRef(WkI->Default);
  ElActfSt->AddSparseFCRef(WkEqW);
  ElActfSt->AddSparseFCRef(WkEqWC); //
  ElActfSt->AddSparseFCRef(WkPhB);
  ElActfSt->AddSparseFCRef(WkPhB->RowDefault);
  ElActfSt->AddSparseFCRef(PrevWkPh);
  ElActfSt->AddSparseFCRef(PrevWkPh->RowDefault);
  ElActfSt->AddSparseFCRef(WGIFL);
  ElActfSt->AddSparseFCRef(WGIFL->Default);
  ElActfSt->AddSparseFCRef(WGEqW);
  ElActfSt->AddSparseFCRef(WGEqWC); //
  ElActfSt->AddSparseFCRef(WGB);
  ElActfSt->AddSparseFCRef(WGB->RowDefault);
  ElActfSt->AddSparseFCRef(PrevWG);
  ElActfSt->AddSparseFCRef(PrevWG->RowDefault);
  ElActfSt->AddSparseFCRef(InEqWGW1);
  ElActfSt->AddSparseFCRef(InEqWGW1C); //
  ElActfSt->AddSparseFCRef(InEqWGW2);
  ElActfSt->AddSparseFCRef(InEqWGW2C); //
  ElActfSt->AddSparseFCRef(InEqWGW3);
  ElActfSt->AddSparseFCRef(InEqWGW3C); //
  ElActfSt->AddSparseFCRef(InEqWGW4);
  ElActfSt->AddSparseFCRef(InEqWGW4C); //
  ElActfSt->AddSparseFCRef(PrevInEqWGW1);
  ElActfSt->AddSparseFCRef(PrevInEqWGW1C); //
  ElActfSt->AddSparseFCRef(PrevInEqWGW2);
  ElActfSt->AddSparseFCRef(PrevInEqWGW2C); //
  ElActfSt->AddSparseFCRef(PrevInEqWGW3);
  ElActfSt->AddSparseFCRef(PrevInEqWGW3C); //
  ElActfSt->AddSparseFCRef(PrevInEqWGW4);
  ElActfSt->AddSparseFCRef(PrevInEqWGW4C); //
  ElActfSt->AddSparseFCRef(WkEqWGW1);
  ElActfSt->AddSparseFCRef(WkEqWGW1C); //
  ElActfSt->AddSparseFCRef(WkEqWGW2);
  ElActfSt->AddSparseFCRef(WkEqWGW2C); //
  ElActfSt->AddSparseFCRef(WkEqWGW3);
  ElActfSt->AddSparseFCRef(WkEqWGW3C); //
  ElActfSt->AddSparseFCRef(WkEqWGW4);
  ElActfSt->AddSparseFCRef(WkEqWGW4C); //
  ElActfSt->AddSparseFCRef(PrevWkEqWGW1);
  ElActfSt->AddSparseFCRef(PrevWkEqWGW1C); //
  ElActfSt->AddSparseFCRef(PrevWkEqWGW2);
  ElActfSt->AddSparseFCRef(PrevWkEqWGW2C); //
  ElActfSt->AddSparseFCRef(PrevWkEqWGW3);
  ElActfSt->AddSparseFCRef(PrevWkEqWGW3C); //
  ElActfSt->AddSparseFCRef(PrevWkEqWGW4);
  ElActfSt->AddSparseFCRef(PrevWkEqWGW4C); //
  ElActfSt->AddSparseFCRef(OutPhB);
  ElActfSt->AddSparseFCRef(OutPhB->RowDefault);
  ElActfSt->AddSparseFCRef(OutEqW); //
  ElActfSt->AddSparseFCRef(OutEqWC); //
  ElActfSt->AddSparseFCRef(WkOutPhB);
  ElActfSt->AddSparseFCRef(WkOutPhB->RowDefault);
  ElActfSt->AddSparseFCRef(WkOutEqWGW1);
  ElActfSt->AddSparseFCRef(WkOutEqWGW1C); //
  ElActfSt->AddSparseFCRef(WkOutEqWGW2);
  ElActfSt->AddSparseFCRef(WkOutEqWGW2C); //
  ElActfSt->AddSparseFCRef(WkOutEqWGW3);
  ElActfSt->AddSparseFCRef(WkOutEqWGW3C); //
  ElActfSt->AddSparseFCRef(WkOutEqWGW4);
  ElActfSt->AddSparseFCRef(WkOutEqWGW4C); //
  ElActfSt->AddSparseFCRef(ActFlags);
  ElActfSt->AddSparseFCRef(ActFlagsC); //
  // complementary SSMs
  //ElActfSt->AddSparseFCRef(InEqWC);
  //ElActfSt->AddSparseFCRef(WkEqWC);
  //ElActfSt->AddSparseFCRef(WGEqWC);
  //ElActfSt->AddSparseFCRef(InEqWGW1C);
  //ElActfSt->AddSparseFCRef(InEqWGW2C);
  //ElActfSt->AddSparseFCRef(InEqWGW3C);
  //ElActfSt->AddSparseFCRef(InEqWGW4C);
  //ElActfSt->AddSparseFCRef(WkEqWGW1C);
  //ElActfSt->AddSparseFCRef(WkEqWGW2C);
  //ElActfSt->AddSparseFCRef(WkEqWGW3C);
  //ElActfSt->AddSparseFCRef(WkEqWGW4C);
  //ElActfSt->AddSparseFCRef(ActFlagsC);

  StActMem->AddSparseRef(StActI);

  RemPh->AddSparseRef(RemPhIL);
  RemPh->AddSparseRef(CurrRemPh);
  RemPh->AddSparseRef(NextRemPh, 1);
  RemPh->AddSparseRef(RemStartPh);
  RemPhfWG->AddSparseFCRef(WGB);
  RemPh->AddSparseRefNum(0); // to check null in from FC to FlushAS
  RemPhfWG->AddSparseRefNum(0); // to check null in from FC to FlushAS
#endif

  return 0;
}
