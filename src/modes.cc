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
int tmp = 0;
//////////////////////////////////////////////////////////////////////
// Acquire operating mode architecture
//////////////////////////////////////////////////////////////////////
int sllm::AcquireArch()
{
  //Flag used to start the acquisition process 
  StartAcquireFlag = new ssm(1);
  BlankWFlag = new ssm(1);
  EndInPhFlag = new ssm(1);
  FlushAcquireFlag = new ssm(1);
  EndAcquireFlag = new ssm(1);

  BlankWFlag->FC(W, 1);
  BlankWFlag->SB(-0.5);
  EndInPhFlag->SC(BlankWFlag, 1);
  EndInPhFlag->Nr[0]->CN(PhI->Nr[PhSize-1], 1);
  EndInPhFlag->SB(-0.5);

  // instructions
  Acquire_I10 = new ssm(1);
  Acquire_I20 = new ssm(1);
  Acquire_I30 = new ssm(1);
  Acquire_I40 = new ssm(1);
  Acquire_I50 = new ssm(1);
  Acquire_I60 = new ssm(1);
  Acquire_I30goto20 = new ssm(1);


  Acquire_Instr = new ssm();
  Acquire_Instr->Nr.push_back(Acquire_I10->Nr[0]);
  Acquire_Instr->Nr.push_back(Acquire_I20->Nr[0]);
  Acquire_Instr->Nr.push_back(Acquire_I30->Nr[0]);
  Acquire_Instr->Nr.push_back(Acquire_I40->Nr[0]);
  Acquire_Instr->Nr.push_back(Acquire_I50->Nr[0]);
  Acquire_Instr->Nr.push_back(Acquire_I60->Nr[0]);

  Acquire_Instr->FC(FlushAcquireFlag, -BIGWG);
  Acquire_I10->Nr[0]->L[0].Wg = 0;

  Acquire_I10->SC(StartAcquireFlag, 1);
  Acquire_I10->SB(-0.5);

  Acquire_I20->SC(Acquire_I10, 1);
  Acquire_I20->SC(Acquire_I30goto20, 1);
  Acquire_I20->SB(-0.5);

  Acquire_I30->SC(Acquire_I20, 1);
  Acquire_I30->SB(-0.5);

  Acquire_I30goto20->SC(Acquire_I30, 1);
  Acquire_I30goto20->SC(EndInPhFlag, 1);
  Acquire_I30goto20->SB(-1.5);

  Acquire_I40->SC(Acquire_I30, 1);
  Acquire_I40->SC(Acquire_I30goto20, -1);
  Acquire_I40->SB(-0.5);

  Acquire_I50->SC(Acquire_I40, 1);
  Acquire_I50->SB(-0.5);

  Acquire_I60->SC(Acquire_I50, 1);
  Acquire_I60->SB(-0.5);

  StartAcquireFlag->SC(StartAcquireFlag, -1);
  StartAcquireFlag->SB(-0.5);

  EndAcquireFlag->SC(Acquire_I60, 1);
  EndAcquireFlag->SB(-0.5);

  Acquire->FC(Acquire_Instr,1);
  Acquire->SC(StartAcquireFlag, 1);
  Acquire->SB(-0.5);

  AcqAct->Nr[FLUSH-1]->CN(Acquire_I10, 1);
  AcqAct->Nr[ACQ_W-1]->CN(Acquire_I20, 1);
  ElActFL->Nr[W_FROM_WK-1]->CN(Acquire_I40, 1);
  ElActFL->Nr[FLUSH_WG-1]->CN(Acquire_I50, 1);
  ElActFL->Nr[FLUSH_WG-1]->CN(Acquire_I60, 1);
  FlushAcquireFlag->FC(StartAcquireFlag, 1);
  FlushAcquireFlag->FC(Acquire, -1);
  FlushAcquireFlag->SB(+0.5);

  return 0;
}

int sllm::AcquireUpdate()
{
  Acquire->ActivOut();
  FlushAcquireFlag->ActivOut();
  
  BlankWFlag->ActivOut();
  EndInPhFlag->ActivOut();

  // level 1
  Acquire_I30->ActivOut();
  Acquire_I60->ActivOut();
  Acquire_I30goto20->ActivOut();

  // level 2
  Acquire_I20->ActivOut();
  Acquire_I50->ActivOut();

  // level 3
  Acquire_I10->ActivOut();
  Acquire_I40->ActivOut();

  StartAcquireFlag->ActivOut();
  EndAcquireFlag->ActivOut();

  return 0;
}

//////////////////////////////////////////////////////////////////////
// Associate operating mode architecture
//////////////////////////////////////////////////////////////////////
int sllm::AssociateArch()
{
  //Flag used to start the build-association process 
  StartAssociateFlag = new ssm(1);

  // Skip-Words counter
  StartSkipWFlag = new ssm(1);
  NextSkipWFlag = new ssm(1);
  BlankSkipWFlag = new ssm(1);
  FlushAssociateFlag = new ssm(1);
  EndWordGroupFlag = new ssm(1);
  EndSkipWFlag = new ssm(1);
  EndAssociateFlag = new ssm(1);
  MemPhFlag = new ssm(1);
  SetStartPhFlag = new ssm(1);
  SkipW = new ssm(PhSize+1);
  CurrSkipW = new ssm(PhSize+1);
  NextSkipW = new ssm(PhSize+1);
  BlankSkipW = new ssm(PhSize);

  CurrSkipW->SC(SkipW, 1);
  CurrSkipW->FC(NextSkipWFlag, -1);
  CurrSkipW->SB(-0.5); 
  NextSkipW->SC(SkipW, 1);
  NextSkipW->FC(NextSkipWFlag, 1);
  NextSkipW->SB(-1.5);
  SkipW->SC(CurrSkipW, 1);
  SkipW->Nr[0]->CN(StartSkipWFlag->Nr[0], 1);
  for (int iw=1; iw<PhSize+1; iw++) {
    SkipW->Nr[iw]->CN(NextSkipW->Nr[iw-1], 1);
    SkipW->Nr[iw]->CN(StartSkipWFlag->Nr[0], -1);
  }
  SkipW->SB(-0.5);
  BlankSkipW->SC(InPhB->RowDefault, 1);
  for (int iw=0; iw<PhSize; iw++) {
    BlankSkipW->Nr[iw]->CN(SkipW->Nr[iw], 1);
  }
  BlankSkipW->SB(-1.5);
  BlankSkipWFlag->FC(BlankSkipW, 1);
  BlankSkipWFlag->SB(-0.5);
  EndSkipWFlag->Nr[0]->CN(SkipW->Nr[PhSize], 1);
  EndSkipWFlag->SC(BlankSkipWFlag, 1);
  EndSkipWFlag->SB(-0.5);
  EndWordGroupFlag->Nr[0]->CN(WGI->Nr[2], 1);
  EndWordGroupFlag->Nr[0]->CN(PhI->Nr[PhSize-1], 1);
  EndWordGroupFlag->SC(CW->Default, 1);
  EndWordGroupFlag->SB(-0.5);

  PhIEqSkipW = new ssm(PhSize);
  PhIEqSkipW->SC(PhI, 1);
  for (int iw=0; iw<PhSize; iw++) {
    PhIEqSkipW->Nr[iw]->CN(SkipW->Nr[iw], 1);
  }
  PhIEqSkipW->SB(-1.5);

  PhIEqSkipWFlag = new ssm(1);
  PhIEqSkipWFlag->FC(PhIEqSkipW, 1);
  PhIEqSkipWFlag->SB(-0.5);

  // instructions
  Associate_I10 = new ssm(1);
  Associate_I20 = new ssm(1);
  Associate_I30 = new ssm(1);
  Associate_I40 = new ssm(1);
  Associate_I50 = new ssm(1);
  Associate_I60 = new ssm(1);
  Associate_I70 = new ssm(1);
  Associate_I80 = new ssm(1);
  Associate_I90 = new ssm(1);
  Associate_I100 = new ssm(1);
  Associate_I110 = new ssm(1);
  Associate_I120 = new ssm(1);
  Associate_I130 = new ssm(1);
  Associate_I140 = new ssm(1);
  Associate_I30goto130 = new ssm(1);
  Associate_I50goto40 = new ssm(1);
  Associate_I90goto70 = new ssm(1);

  Associate_Instr = new ssm();
  Associate_Instr->Nr.push_back(Associate_I10->Nr[0]);
  Associate_Instr->Nr.push_back(Associate_I20->Nr[0]);
  Associate_Instr->Nr.push_back(Associate_I30->Nr[0]);
  Associate_Instr->Nr.push_back(Associate_I40->Nr[0]);
  Associate_Instr->Nr.push_back(Associate_I50->Nr[0]);
  Associate_Instr->Nr.push_back(Associate_I60->Nr[0]);
  Associate_Instr->Nr.push_back(Associate_I70->Nr[0]);
  Associate_Instr->Nr.push_back(Associate_I80->Nr[0]);
  Associate_Instr->Nr.push_back(Associate_I90->Nr[0]);
  Associate_Instr->Nr.push_back(Associate_I100->Nr[0]);
  Associate_Instr->Nr.push_back(Associate_I110->Nr[0]);
  Associate_Instr->Nr.push_back(Associate_I120->Nr[0]);
  Associate_Instr->Nr.push_back(Associate_I130->Nr[0]);
  Associate_Instr->Nr.push_back(Associate_I140->Nr[0]);

  Associate_Instr->FC(FlushAssociateFlag, -BIGWG);
  Associate_I10->Nr[0]->L[0].Wg = 0;

  Associate_I10->SC(StartAssociateFlag, 1);
  Associate_I10->SB(-0.5);

  Associate_I20->SC(Associate_I10, 1);
  Associate_I20->SB(-0.5);

  Associate_I30->SC(Associate_I20, 1);
  Associate_I30->SC(Associate_I120, 1);
  Associate_I30->SB(-0.5);

  Associate_I30goto130->SC(Associate_I30, 1);
  Associate_I30goto130->SC(EndSkipWFlag, 1);
  Associate_I30goto130->SB(-1.5);

  Associate_I50goto40->SC(Associate_I50, 1);
  Associate_I50goto40->SC(PhIEqSkipWFlag,-1);
  Associate_I50goto40->SB(-0.5);

  Associate_I40->SC(Associate_I30, 1);
  Associate_I40->SC(Associate_I30goto130, -1);
  Associate_I40->SC(Associate_I50goto40, 1);
  Associate_I40->SB(-0.5);

  Associate_I50->SC(Associate_I40, 1);
  Associate_I50->SB(-0.5);

  Associate_I60->SC(Associate_I50, 1);
  Associate_I60->SC(Associate_I50goto40, -1);
  Associate_I60->SB(-0.5);

  Associate_I70->SC(Associate_I60, 1);
  Associate_I70->SC(Associate_I90goto70, 1);
  Associate_I70->SB(-0.5);

  Associate_I80->SC(Associate_I70, 1);
  Associate_I80->SB(-0.5);

  Associate_I90->SC(Associate_I80, 1);
  Associate_I90->SB(-0.5);

  Associate_I90goto70->SC(Associate_I90, 1);
  Associate_I90goto70->SC(EndWordGroupFlag, -1);
  Associate_I90goto70->SB(-0.5);

  Associate_I100->SC(Associate_I90, 1);
  Associate_I100->SC(Associate_I90goto70, -1);
  Associate_I100->SB(-0.5);

  Associate_I110->SC(Associate_I100, 1);
  Associate_I110->SB(-0.5);

  Associate_I120->SC(Associate_I110, 1);
  Associate_I120->SB(-0.5);

  Associate_I130->SC(Associate_I30goto130, 1);
  Associate_I130->SB(-0.5);

  Associate_I140->SC(Associate_I130, 1);
  Associate_I140->SB(-0.5);

  StartAssociateFlag->SC(StartAssociateFlag, -1);
  StartAssociateFlag->SB(-0.5);

  EndAssociateFlag->SC(Associate_I140, 1);
  EndAssociateFlag->SB(-0.5);

  Associate->FC(Associate_Instr,1);
  Associate->SC(StartAssociateFlag, 1);
  Associate->SB(-0.5);

  MemPhFlag->SC(Associate_I10, 1);
  MemPhFlag->SC(SetStartPhFlag, -1);
  MemPhFlag->SB(-0.5);
  SetStartPhFlag->SC(Associate_I10, 1);
  SetStartPhFlag->SC(SetStartPhFlag, 1);
  SetStartPhFlag->SB(-1.5);

  AcqAct->Nr[SET_START_PH-1]->CN(SetStartPhFlag, 1);
  AcqAct->Nr[MEM_PH-1]->CN(MemPhFlag, 1);
  ElActFL->Nr[W_FROM_WK-1]->CN(Associate_I20, 1);
  AcqAct->Nr[NEXT_AS_W-1]->CN(Associate_I40, 1);
  ElActFL->Nr[FLUSH_WG-1]->CN(Associate_I60, 1);
  AcqAct->Nr[BUILD_AS-1]->CN(Associate_I70, 1);
  AcqAct->Nr[NEXT_AS_W-1]->CN(Associate_I80, 1);
  ElActFL->Nr[W_FROM_WK-1]->CN(Associate_I110, 1);
  ElActFL->Nr[FLUSH_WG-1]->CN(Associate_I130, 1);
  ElActFL->Nr[FLUSH_WG-1]->CN(Associate_I140, 1);
  NextSkipWFlag->FC(Associate_I100, 1);
  NextSkipWFlag->SB(-0.5);
  StartSkipWFlag->SC(StartAssociateFlag, 1);
  StartSkipWFlag->SB(-0.5);
  FlushAssociateFlag->FC(StartAssociateFlag, 1);
  FlushAssociateFlag->FC(Associate, -1);
  FlushAssociateFlag->SB(+0.5);

  return 0;
}

int sllm::AssociateUpdate()
{
  Associate->ActivOut();
  StartSkipWFlag->ActivOut();
  NextSkipWFlag->ActivOut();
  FlushAssociateFlag->ActivOut();
  
  CurrSkipW->ActivOut();
  NextSkipW->ActivOut();
  SkipW->ActivOut();

  BlankSkipW->ActivOut();
  BlankSkipWFlag->ActivOut();

  PhIEqSkipW->ActivOut();
  PhIEqSkipWFlag->ActivOut();

  EndSkipWFlag->ActivOut();
  EndWordGroupFlag->ActivOut();

  // level 1
  Associate_I30->ActivOut();
  Associate_I50->ActivOut();
  Associate_I90->ActivOut();
  Associate_I30goto130->ActivOut();
  Associate_I50goto40->ActivOut();
  Associate_I90goto70->ActivOut();

  // level 2
  Associate_I20->ActivOut();
  Associate_I40->ActivOut();
  Associate_I80->ActivOut();
  Associate_I100->ActivOut();

  // level 3
  Associate_I70->ActivOut();
  Associate_I110->ActivOut();
  Associate_I140->ActivOut();
  
  // level 4
  Associate_I10->ActivOut();
  Associate_I60->ActivOut();
  Associate_I120->ActivOut();
  Associate_I130->ActivOut();
  
  MemPhFlag->ActivOut();
  SetStartPhFlag->ActivOut();
  StartAssociateFlag->ActivOut();
  EndAssociateFlag->ActivOut();

  return 0;
}

//////////////////////////////////////////////////////////////////////
// Reward operating mode architecture
//////////////////////////////////////////////////////////////////////
int sllm::RewardArch()
{
  //Flag used to start the reward process 
  StartRewardFlag = new ssm(1);

  // iteration counter
  StartIterIFlag = new ssm(1);
  NextIterIFlag = new ssm(1);
  FlushRewardFlag = new ssm(1);
  EndIterIFlag = new ssm(1);
  EndStActIFlag = new ssm(1);
  EndRewardFlag = new ssm(1);
  GetStActIFlag = new ssm(1);
  PartialFlag = new ssm(1);
  DoneFlag = new ssm(1);
  ContinueFlag = new ssm(1);
  IterNum = new ssm(IterSize);
  IterI = new ssm(IterSize);
  CurrIterI = new ssm(IterSize);
  NextIterI = new ssm(IterSize);
  EndIterI = new ssm(IterSize);
  LastStActIFL = new ssm(StActSize);
  LastStActI = new ssm(StActSize);
  EndStActI = new ssm(StActSize);

  CurrIterI->SC(IterI, 1);
  CurrIterI->FC(NextIterIFlag, -1);
  CurrIterI->SB(-0.5); 
  NextIterI->SC(IterI, 1);
  NextIterI->FC(NextIterIFlag, 1);
  NextIterI->SB(-1.5);
  IterI->SC(CurrIterI, 1);
  IterI->Nr[0]->CN(StartIterIFlag->Nr[0], 1);
  for (int i=1; i<IterSize; i++) {
    IterI->Nr[i]->CN(NextIterI->Nr[i-1], 1);
    IterI->Nr[i]->CN(StartIterIFlag->Nr[0], -1);
  }
  IterI->SB(-0.5);

  EndIterI->SC(IterNum, 1);
  EndIterI->SC(IterI, 1);
  EndIterI->SB(-1.5);
  EndIterIFlag->FC(EndIterI, 1);
  EndIterIFlag->SB(-0.5);

  //LastStActIFL->SC(StActI, 1);
  for (int i=0; i<StActSize-2; i++) {
    LastStActIFL->Nr[i]->CN(StActI->Nr[i+2], 1);
  }
  LastStActIFL->FC(GetStActIFlag, 1);
  LastStActIFL->SB(-1.5);

  LastStActI->SC(LastStActI, 1);
  LastStActI->SC(LastStActIFL, 1);
  LastStActI->FC(GetStActIFlag, -BIGWG);
  LastStActI->SB(-0.5);

  EndStActI->SC(LastStActI, 1);
  EndStActI->SC(StActI, 1);
  EndStActI->SB(-1.5);
  EndStActIFlag->FC(EndStActI, 1);
  EndStActIFlag->FC(LastStActI, -1);
  EndStActIFlag->SB(+0.5);

  // instructions
  Reward_I10 = new ssm(1);
  Reward_I20 = new ssm(1);
  Reward_I30 = new ssm(1);
  Reward_I40 = new ssm(1);
  Reward_I50 = new ssm(1);
  Reward_I60 = new ssm(1);
  Reward_I70 = new ssm(1);
  Reward_I80 = new ssm(1);
  Reward_I90 = new ssm(1);
  Reward_I100 = new ssm(1);
  Reward_I110 = new ssm(1);
  Reward_I90goto80 = new ssm(1);
  Reward_I100goto60 = new ssm(1);

  Reward_Instr = new ssm();
  Reward_Instr->Nr.push_back(Reward_I10->Nr[0]);
  Reward_Instr->Nr.push_back(Reward_I20->Nr[0]);
  Reward_Instr->Nr.push_back(Reward_I30->Nr[0]);
  Reward_Instr->Nr.push_back(Reward_I40->Nr[0]);
  Reward_Instr->Nr.push_back(Reward_I50->Nr[0]);
  Reward_Instr->Nr.push_back(Reward_I60->Nr[0]);
  Reward_Instr->Nr.push_back(Reward_I70->Nr[0]);
  Reward_Instr->Nr.push_back(Reward_I80->Nr[0]);
  Reward_Instr->Nr.push_back(Reward_I90->Nr[0]);
  Reward_Instr->Nr.push_back(Reward_I100->Nr[0]);
  Reward_Instr->Nr.push_back(Reward_I110->Nr[0]);

  Reward_Instr->FC(FlushRewardFlag, -BIGWG);
  Reward_I10->Nr[0]->L[0].Wg = 0;

  Reward_I10->SC(StartRewardFlag, 1);
  Reward_I10->SB(-0.5);

  Reward_I20->SC(Reward_I10, 1);
  Reward_I20->SB(-0.5);

  Reward_I30->SC(Reward_I20, 1);
  Reward_I30->SB(-0.5);

  Reward_I40->SC(Reward_I30, 1);
  Reward_I40->SB(-0.5);

  Reward_I50->SC(Reward_I40, 1);
  Reward_I50->SB(-0.5);

  Reward_I60->SC(Reward_I50, 1);
  Reward_I60->SC(Reward_I100goto60, 1);
  Reward_I60->SB(-0.5);

  Reward_I70->SC(Reward_I60, 1);
  Reward_I70->SB(-0.5);

  Reward_I80->SC(Reward_I70, 1);
  Reward_I80->SC(Reward_I90goto80, 1);
  Reward_I80->SB(-0.5);

  Reward_I90->SC(Reward_I80, 1);
  Reward_I90->SB(-0.5);

  Reward_I90goto80->SC(Reward_I90, 1);
  Reward_I90goto80->SC(EndStActIFlag, -1);
  Reward_I90goto80->SB(-0.5);

  Reward_I100->SC(Reward_I90, 1);
  Reward_I100->SC(Reward_I90goto80, -1);
  Reward_I100->SB(-0.5);

  Reward_I100goto60->SC(Reward_I100, 1);
  Reward_I100goto60->SC(EndIterIFlag, -1);
  Reward_I100goto60->SB(-0.5);

  Reward_I110->SC(Reward_I100, 1);
  Reward_I110->SC(Reward_I100goto60, -1);
  Reward_I110->SB(-0.5);

  StartRewardFlag->SC(StartRewardFlag, -1);
  StartRewardFlag->SB(-0.5);

  EndRewardFlag->SC(Reward_I110, 1);
  EndRewardFlag->SB(-0.5);

  Reward->FC(Reward_Instr,1);
  Reward->SC(StartRewardFlag, 1);
  Reward->SB(-0.5);

  DoneFlag->SC(Reward_I30, 1);
  DoneFlag->SC(PartialFlag, -1);
  DoneFlag->SB(-0.5);
  ContinueFlag->SC(Reward_I30, 1);
  ContinueFlag->SC(PartialFlag, 1);
  ContinueFlag->SB(-1.5);

  RwdAct->Nr[STORE_ST_A-1]->CN(Reward_I10, 1);
  ElActFL->Nr[FLUSH_OUT-1]->CN(Reward_I10, 1);
  RwdAct->Nr[STORE_ST_A-1]->CN(Reward_I20, 1);
  ElActFL->Nr[WG_OUT-1]->CN(Reward_I20, 1);
  RwdAct->Nr[STORE_ST_A-1]->CN(Reward_I30, 1);
  ElActFL->Nr[DONE-1]->CN(DoneFlag, 1);
  ElActFL->Nr[CONTINUE-1]->CN(ContinueFlag, 1);
  RwdAct->Nr[STORE_ST_A-1]->CN(Reward_I40, 1);
  RwdAct->Nr[START_ST_A-1]->CN(Reward_I60, 1);
  RwdAct->Nr[RWD_ST_A-1]->CN(Reward_I80, 1);

  FlushRewardFlag->FC(StartRewardFlag, 1);
  FlushRewardFlag->FC(Reward, -1);
  FlushRewardFlag->SB(+0.5);
  StartIterIFlag->SC(StartRewardFlag, 1);
  StartIterIFlag->SB(-0.5);
  GetStActIFlag->FC(Reward_I50, 1);
  GetStActIFlag->SB(-0.5);
  NextIterIFlag->FC(Reward_I70, 1);
  NextIterIFlag->SB(-0.5);

  return 0;
}

int sllm::RewardUpdate()
{
  Reward->ActivOut();
  StartIterIFlag->ActivOut();
  NextIterIFlag->ActivOut();
  FlushRewardFlag->ActivOut();
  
  CurrIterI->ActivOut();
  NextIterI->ActivOut();
  IterI->ActivOut();

  EndIterI->ActivOut();
  EndIterIFlag->ActivOut();

  GetStActIFlag->ActivOut();
  LastStActI->ActivOut();
  LastStActIFL->ActivOut();
  EndStActI->ActivOut();
  EndStActIFlag->ActivOut();

  // level 1
  Reward_I40->ActivOut();
  Reward_I60->ActivOut();
  Reward_I80->ActivOut();

  // level 2
  Reward_I30->ActivOut();
  Reward_I50->ActivOut();
  Reward_I70->ActivOut();
  Reward_I90->ActivOut();
  Reward_I90goto80->ActivOut();

  // level 3
  Reward_I20->ActivOut();
  Reward_I100->ActivOut();
  Reward_I100goto60->ActivOut();

  // level 4
  Reward_I10->ActivOut();
  Reward_I110->ActivOut();

  /*
  for (int i=0; i<Reward_Instr->NN(); i++) {
    cout <<  10*(i+1) << ":" << Reward_Instr->Nr[i]->O << " ";
  }
  cout << endl;
  for (int i=0; i<IterI->NN(); i++) {
    cout << IterI->Nr[i]->O << " ";
  }
  cout << endl;
  */
  StartRewardFlag->ActivOut();
  EndRewardFlag->ActivOut();

  DoneFlag->ActivOut();
  ContinueFlag->ActivOut();

  RwdAct->ActivOut();

  return 0;
}

//////////////////////////////////////////////////////////////////////
// Exploit operating mode architecture
//////////////////////////////////////////////////////////////////////
int sllm::ExploitArch()
{
  //Flag used to start the exploitation process 
  StartExploitFlag = new ssm(1);
  EndExploitFlag = new ssm(1);
  FlushExploitFlag = new ssm(1);

  // Association-retrieval counter
  StartNRetrFlag = new ssm(1);
  NextNRetrFlag = new ssm(1);
  EndNRetrFlag = new ssm(1);

  GetDBFlag = new ssm(1);
  RetrBestActFlag = new ssm(1);
  DB_GE_BestFlag = new ssm(1);
  InitBestDBFlag = new ssm(1);
  MaxStActIFlag = new ssm(1);
  //GetRetrAsFlag = new ssm(1);
  UpdateBestDBFlag = new ssm(1);
  NoRetrFlag = new ssm(1);
  NoActFlag = new ssm(1);
  MaxStActIWarn = new ssm(1);

  NRetr = new ssm(MaxNRetr);
  CurrNRetr = new ssm(MaxNRetr);
  NextNRetr = new ssm(MaxNRetr);

  int BiasNum = ElActfSt->DynamicBias->NN();
  BestDBFL = new ssm(BiasNum);
  BestDB = new ssm(BiasNum);
  CurrBestDB = new ssm(BiasNum);
  BestActFL = new ssm(ElActSize);
  BestAct = new ssm(ElActSize);
  CurrBestAct = new ssm(ElActSize);
  RetrBestAct = new ssm(ElActSize);
  BestAct->UseDefault();
  //DBCumul = new ssm(BiasNum);
  //DB_GE_BestDB = new ssm(BiasNum);
  //PrevRetrAsFL = new ssm(1);
  //PrevRetrAs = new ssm(1);

  CurrNRetr->SC(NRetr, 1);
  CurrNRetr->FC(NextNRetrFlag, -1);
  CurrNRetr->SB(-0.5); 
  NextNRetr->SC(NRetr, 1);
  NextNRetr->FC(NextNRetrFlag, 1);
  NextNRetr->SB(-1.5);
  NRetr->SC(CurrNRetr, 1);
  NRetr->Nr[0]->CN(StartNRetrFlag->Nr[0], 1);
  for (int i=1; i<MaxNRetr; i++) {
    NRetr->Nr[i]->CN(NextNRetr->Nr[i-1], 1);
    NRetr->Nr[i]->CN(StartNRetrFlag->Nr[0], -1);
  }
  NRetr->SB(-0.5);
  EndNRetrFlag->Nr[0]->CN(NRetr->Nr[MaxNRetr-1], 1);
  EndNRetrFlag->SB(-0.5);


  BestDB->FC(InitBestDBFlag, -BIGWG);
  BestDB->Nr[BiasNum-1]->L[0].Wg = 1;
  BestDBFL->SC(ElActfSt->DynamicBias, 1);
  BestDBFL->FC(GetDBFlag, 1);
  BestDBFL->SB(-1.5);
  CurrBestDB->SC(BestDB, 1);
  CurrBestDB->FC(GetDBFlag, -1);
  CurrBestDB->SB(-0.5);
  BestDB->SC(CurrBestDB, 1);
  BestDB->SC(BestDBFL, 1);
  BestDB->SB(-0.5);

  BestActFL->SC(ElActFL, 1);
  BestActFL->FC(GetDBFlag, 1);
  BestActFL->SB(-1.5);
  CurrBestAct->SC(BestAct, 1);
  CurrBestAct->FC(GetDBFlag, -1);
  CurrBestAct->SB(-0.5);
  BestAct->SC(CurrBestAct, 1);
  BestAct->SC(BestActFL, 1);
  BestAct->SB(-0.5);

  RetrBestAct->SC(BestAct, 1);
  RetrBestAct->FC(RetrBestActFlag, 1);
  RetrBestAct->SB(-1.5);
  ElActFL->SC(RetrBestAct, 1);

  //DBCumul->FC(ElActfSt->DynamicBias, 0);
  //for (int i=0; i<BiasNum; i++) {
  //  for (int j=0; j<BiasNum; j++) {
  //    if (j>=i) DBCumul->Nr[i]->L[j].Wg = 1;
  //  }
  //}
  //DBCumul->SB(-0.5);

  //DB_GE_BestDB->SC(DBCumul, 1);
  //DB_GE_BestDB->SC(BestDB, 1);
  //DB_GE_BestDB->SB(-1.5);
  //DB_GE_BestFlag->FC(DB_GE_BestDB, 1);
  //DB_GE_BestFlag->SB(-0.5);
  DB_GE_BestFlag->FC(ElActfSt->DynamicBias, 0);
  DB_GE_BestFlag->FC(BestDB, 0);
  for (int i=0; i<BiasNum; i++) {
    DB_GE_BestFlag->Nr[0]->L[i].Wg = i;
    DB_GE_BestFlag->Nr[0]->L[BiasNum+i].Wg = -i;
  }
  DB_GE_BestFlag->SB(+0.5); // greater or equal

  MaxStActIFlag->Nr[0]->CN(StActI->Nr[StActSize-1], 1);
  MaxStActIFlag->SB(-0.5);

  //PrevRetrAsFL->Nr[0]->CN(ElActFL->Nr[RETR_AS-1], -1);
  //PrevRetrAsFL->FC(GetRetrAs, 1);
  //PrevRetrAsFL->SB(-1.5);
  //PrevRetrAs->SC(PrevRetrAs, 1);
  //PrevRetrAs->SC(PrevRetrAsFL, 1);
  //PrevRetrAs->FC(GetRetrAs, -BIGWG);
  //PrevRetrAs->SB(-0.5);

  UpdateBestDBFlag->Nr[0]->CN(ElActFL->Nr[RETR_AS-1], -1);
  UpdateBestDBFlag->SC(ElActFL->Default, -1);
  UpdateBestDBFlag->SC(DB_GE_BestFlag, -1);
  UpdateBestDBFlag->SB(+0.5);

  // instructions
  Exploit_I10 = new ssm(1);
  Exploit_I20 = new ssm(1);
  Exploit_I30 = new ssm(1);
  Exploit_I40 = new ssm(1);
  Exploit_I50 = new ssm(1);
  Exploit_I60 = new ssm(1);
  Exploit_I70 = new ssm(1);
  Exploit_I80 = new ssm(1);
  Exploit_I90 = new ssm(1);
  Exploit_I100 = new ssm(1);
  Exploit_I110 = new ssm(1);
  Exploit_I120 = new ssm(1);
  Exploit_I130 = new ssm(1);
  Exploit_I140 = new ssm(1);
  Exploit_I150 = new ssm(1);
  Exploit_I160 = new ssm(1);
  Exploit_I170 = new ssm(1);
  Exploit_I180 = new ssm(1);
  Exploit_I190 = new ssm(1);
  Exploit_I200 = new ssm(1);
  Exploit_I210 = new ssm(1);
  Exploit_I220 = new ssm(1);
  Exploit_I230 = new ssm(1);
  Exploit_I240 = new ssm(1);
  Exploit_I250 = new ssm(1);
  Exploit_I260 = new ssm(1);
  Exploit_I270 = new ssm(1);
  Exploit_I280 = new ssm(1);
  Exploit_I290 = new ssm(1);
  Exploit_I50goto200 = new ssm(1);
  Exploit_I120goto150 = new ssm(1);
  Exploit_I160goto90 = new ssm(1);
  Exploit_I170goto270 = new ssm(1);
  Exploit_I220goto270 = new ssm(1);
  Exploit_I240goto270 = new ssm(1);
  Exploit_I250goto50 = new ssm(1);
  Exploit_I280goto30 = new ssm(1);

  Exploit_Instr = new ssm();
  Exploit_Instr->Nr.push_back(Exploit_I10->Nr[0]);
  Exploit_Instr->Nr.push_back(Exploit_I20->Nr[0]);
  Exploit_Instr->Nr.push_back(Exploit_I30->Nr[0]);
  Exploit_Instr->Nr.push_back(Exploit_I40->Nr[0]);
  Exploit_Instr->Nr.push_back(Exploit_I50->Nr[0]);
  Exploit_Instr->Nr.push_back(Exploit_I60->Nr[0]);
  Exploit_Instr->Nr.push_back(Exploit_I70->Nr[0]);
  Exploit_Instr->Nr.push_back(Exploit_I80->Nr[0]);
  Exploit_Instr->Nr.push_back(Exploit_I90->Nr[0]);
  Exploit_Instr->Nr.push_back(Exploit_I100->Nr[0]);
  Exploit_Instr->Nr.push_back(Exploit_I110->Nr[0]);
  Exploit_Instr->Nr.push_back(Exploit_I120->Nr[0]);
  Exploit_Instr->Nr.push_back(Exploit_I130->Nr[0]);
  Exploit_Instr->Nr.push_back(Exploit_I140->Nr[0]);
  Exploit_Instr->Nr.push_back(Exploit_I150->Nr[0]);
  Exploit_Instr->Nr.push_back(Exploit_I160->Nr[0]);
  Exploit_Instr->Nr.push_back(Exploit_I170->Nr[0]);
  Exploit_Instr->Nr.push_back(Exploit_I180->Nr[0]);
  Exploit_Instr->Nr.push_back(Exploit_I190->Nr[0]);
  Exploit_Instr->Nr.push_back(Exploit_I200->Nr[0]);
  Exploit_Instr->Nr.push_back(Exploit_I210->Nr[0]);
  Exploit_Instr->Nr.push_back(Exploit_I220->Nr[0]);
  Exploit_Instr->Nr.push_back(Exploit_I230->Nr[0]);
  Exploit_Instr->Nr.push_back(Exploit_I240->Nr[0]);
  Exploit_Instr->Nr.push_back(Exploit_I250->Nr[0]);
  Exploit_Instr->Nr.push_back(Exploit_I260->Nr[0]);
  Exploit_Instr->Nr.push_back(Exploit_I270->Nr[0]);
  Exploit_Instr->Nr.push_back(Exploit_I280->Nr[0]);
  Exploit_Instr->Nr.push_back(Exploit_I290->Nr[0]);

  Exploit_Instr->FC(FlushExploitFlag, -BIGWG);
  Exploit_I10->Nr[0]->L[0].Wg = 0;

  Exploit_I10->SC(StartExploitFlag, 1);
  Exploit_I10->SB(-0.5);

  Exploit_I20->SC(Exploit_I10, 1);
  Exploit_I20->SB(-0.5);

  Exploit_I30->SC(Exploit_I20, 1);
  Exploit_I30->SC(Exploit_I280goto30, 1);
  Exploit_I30->SB(-0.5);

  Exploit_I40->SC(Exploit_I30, 1);
  Exploit_I40->SB(-0.5);

  Exploit_I50->SC(Exploit_I40, 1);
  Exploit_I50->SC(Exploit_I250goto50, 1);
  Exploit_I50->SB(-0.5);

  Exploit_I50goto200->SC(Exploit_I50, 1);
  Exploit_I50goto200->Nr[0]->CN(ElActFL->Nr[RETR_AS-1], -1);
  Exploit_I50goto200->SB(-0.5);

  Exploit_I60->SC(Exploit_I50, 1);
  Exploit_I60->SC(Exploit_I50goto200, -1);
  Exploit_I60->SB(-0.5);

  Exploit_I70->SC(Exploit_I60, 1);
  Exploit_I70->SB(-0.5);

  Exploit_I80->SC(Exploit_I70, 1);
  Exploit_I80->SB(-0.5);

  Exploit_I90->SC(Exploit_I80, 1);
  Exploit_I90->SC(Exploit_I160goto90, 1);
  Exploit_I90->SB(-0.5);

  Exploit_I100->SC(Exploit_I90, 1);
  Exploit_I100->SB(-0.5);

  Exploit_I110->SC(Exploit_I100, 1);
  Exploit_I110->SB(-0.5);

  Exploit_I120->SC(Exploit_I110, 1);
  Exploit_I120->SB(-0.5);

  Exploit_I120goto150->SC(Exploit_I120, 1);
  Exploit_I120goto150->SC(UpdateBestDBFlag, -1);
  Exploit_I120goto150->SB(-0.5);

  Exploit_I130->SC(Exploit_I120, 1);
  Exploit_I130->SC(Exploit_I120goto150, -1);
  Exploit_I130->SB(-0.5);

  Exploit_I140->SC(Exploit_I130, 1);
  Exploit_I140->SB(-0.5);

  Exploit_I150->SC(Exploit_I140, 1);
  Exploit_I150->SC(Exploit_I120goto150, 1);
  Exploit_I150->SB(-0.5);

  Exploit_I160->SC(Exploit_I150, 1);
  Exploit_I160->SB(-0.5);

  Exploit_I160goto90->SC(Exploit_I160, 1);
  Exploit_I160goto90->SC(EndNRetrFlag, -1);
  Exploit_I160goto90->SB(-0.5);

  Exploit_I170->SC(Exploit_I160, 1);
  Exploit_I170->SC(Exploit_I160goto90, -1);
  Exploit_I170->SB(-0.5);

  Exploit_I170goto270->SC(Exploit_I170, 1);
  Exploit_I170goto270->SC(BestAct->Default, 1);
  Exploit_I170goto270->SB(-1.5);

  Exploit_I180->SC(Exploit_I170, 1);
  Exploit_I180->SC(Exploit_I170goto270, -1);
  Exploit_I180->SB(-0.5);

  Exploit_I190->SC(Exploit_I180, 1);
  Exploit_I190->SB(-0.5);

  Exploit_I200->SC(Exploit_I50goto200, 1);
  Exploit_I200->SB(-0.5);

  Exploit_I210->SC(Exploit_I200, 1);
  Exploit_I210->SB(-0.5);

  Exploit_I220->SC(Exploit_I210, 1);
  Exploit_I220->SB(-0.5);

  Exploit_I220goto270->SC(Exploit_I220, 1);
  Exploit_I220goto270->SC(ElActFL->Default, 1);
  Exploit_I220goto270->SB(-1.5);

  Exploit_I230->SC(Exploit_I190, 1);
  Exploit_I230->SC(Exploit_I220, 1);
  Exploit_I230->SC(Exploit_I220goto270, -1);
  Exploit_I230->SB(-0.5);

  Exploit_I240->SC(Exploit_I230, 1);
  Exploit_I240->SB(-0.5);

  Exploit_I240goto270->SC(Exploit_I240, 1);
  Exploit_I240goto270->Nr[0]->CN(ElActFL->Nr[DONE-1], 1);
  Exploit_I240goto270->SB(-1.5);

  Exploit_I250->SC(Exploit_I240, 1);
  Exploit_I250->SC(Exploit_I240goto270, -1);
  Exploit_I250->SB(-0.5);

  Exploit_I250goto50->SC(Exploit_I250, 1);
  Exploit_I250goto50->SC(MaxStActIFlag, -1);
  Exploit_I250goto50->SB(-0.5);

  Exploit_I260->SC(Exploit_I250, 1);
  Exploit_I260->SC(Exploit_I250goto50, -1);
  Exploit_I260->SB(-0.5);

  Exploit_I270->SC(Exploit_I260, 1);
  Exploit_I270->SC(Exploit_I170goto270, 1);
  Exploit_I270->SC(Exploit_I220goto270, 1);
  Exploit_I270->SC(Exploit_I240goto270, 1);
  Exploit_I270->SB(-0.5);

  Exploit_I280->SC(Exploit_I270, 1);
  Exploit_I280->SB(-0.5);

  Exploit_I280goto30->SC(Exploit_I280, 1);
  Exploit_I280goto30->SC(EndIterIFlag, -1);
  Exploit_I280goto30->SB(-0.5);

  Exploit_I290->SC(Exploit_I280, 1);
  Exploit_I290->SC(Exploit_I280goto30, -1);
  Exploit_I290->SB(-0.5);

  StartExploitFlag->SC(StartExploitFlag, -1);
  StartExploitFlag->SB(-0.5);

  EndExploitFlag->SC(Exploit_I290, 1);
  EndExploitFlag->SB(-0.5);

  Exploit->FC(Exploit_Instr,1);
  Exploit->SC(StartExploitFlag, 1);
  Exploit->SB(-0.5);

  RwdAct->Nr[START_ST_A-1]->CN(Exploit_I10, 1);
  StartIterIFlag->SC(Exploit_I10, 1);
  RwdAct->Nr[STORE_ST_A-1]->CN(Exploit_I20, 1);
  RwdAct->Nr[START_ST_A-1]->CN(Exploit_I30, 1);
  RwdAct->Nr[RETR_ST_A-1]->CN(Exploit_I40, 1);
  InitBestDBFlag->SC(Exploit_I60, 1);
  InitBestDBFlag->SB(-0.5);
  BestAct->FC(Exploit_I60, -BIGWG);
  RwdAct->Nr[STORE_ST_A-1]->CN(Exploit_I70, 1);
  StartNRetrFlag->SC(Exploit_I80, 1);
  StartNRetrFlag->SB(-0.5);
  ElActFL->Nr[RETR_AS-1]->CN(Exploit_I90, 1);
  ElActFL->FC(Exploit_I100, -BIGWG);
  RwdAct->Nr[RETR_EL_A-1]->CN(Exploit_I110, 1);
  GetDBFlag->SC(Exploit_I130, 1);
  GetDBFlag->SB(-0.5);
  RwdAct->Nr[CHANGE_ST_A-1]->CN(Exploit_I140, 1);
  NextNRetrFlag->SC(Exploit_I150, 1);
  NextNRetrFlag->SB(-0.5);
  NoRetrFlag->SC(BestAct->Default, 1);
  NoRetrFlag->SC(Exploit_I170, 1);
  NoRetrFlag->SB(-1.5);
  RwdAct->Nr[GET_ST_A-1]->CN(Exploit_I180, 1);
  RwdAct->Nr[RETR_EL_A-1]->CN(Exploit_I200, 1);
  GetDBFlag->SC(Exploit_I210, 1);
  NoActFlag->SC(ElActFL->Default, 1);
  NoActFlag->SC(Exploit_I220, 1);
  NoActFlag->SB(-1.5);
  RetrBestActFlag->SC(Exploit_I230, 1);
  RetrBestActFlag->SB(-0.5);
  RwdAct->Nr[STORE_ST_A-1]->CN(Exploit_I230, 1);
  MaxStActIWarn->SC(Exploit_I260, 1);
  MaxStActIWarn->SB(-0.5);
  NextIterIFlag->SC(Exploit_I270, 1);

  FlushExploitFlag->FC(StartExploitFlag, 1);
  FlushExploitFlag->FC(Exploit, -1);
  FlushExploitFlag->SB(+0.5);

  return 0;
}

int sllm::ExploitUpdate()
{
  Exploit->ActivOut();
  FlushExploitFlag->ActivOut();
  StartNRetrFlag->ActivOut();
  NextNRetrFlag->ActivOut();
  
  CurrNRetr->ActivOut();
  NextNRetr->ActivOut();
  NRetr->ActivOut();
  EndNRetrFlag->ActivOut();

  InitBestDBFlag->ActivOut();

  //DBCumul->ActivOut();
  //DB_GE_BestDB->ActivOut();
  DB_GE_BestFlag->ActivOut();
  UpdateBestDBFlag->ActivOut();

  MaxStActIFlag->ActivOut();

  // level 1
  Exploit_I100->ActivOut();
  Exploit_I120->ActivOut();
  Exploit_I120goto150->ActivOut();

  // level 2
  Exploit_I90->ActivOut();
  Exploit_I110->ActivOut();
  Exploit_I130->ActivOut();
  Exploit_I220->ActivOut();
  Exploit_I220goto270->ActivOut();

  GetDBFlag->ActivOut();
  BestDBFL->ActivOut();
  CurrBestDB->ActivOut();
  BestDB->ActivOut();
  BestActFL->ActivOut();
  CurrBestAct->ActivOut();
  BestAct->ActivOut();

  // level 3
  Exploit_I50->ActivOut();
  Exploit_I140->ActivOut();
  Exploit_I230->ActivOut();
  Exploit_I50goto200->ActivOut();

  // level 4
  Exploit_I40->ActivOut();
  Exploit_I60->ActivOut();
  Exploit_I150->ActivOut();
  Exploit_I200->ActivOut();
  Exploit_I240->ActivOut();
  Exploit_I240goto270->ActivOut();

  // level 5
  Exploit_I30->ActivOut();
  Exploit_I70->ActivOut();
  Exploit_I160->ActivOut();
  Exploit_I180->ActivOut();
  Exploit_I250->ActivOut();
  Exploit_I210->ActivOut();
  Exploit_I160goto90->ActivOut();
  Exploit_I250goto50->ActivOut();

  // level 6
  Exploit_I20->ActivOut();
  Exploit_I80->ActivOut();
  Exploit_I170->ActivOut();
  Exploit_I190->ActivOut();
  Exploit_I260->ActivOut();
  Exploit_I170goto270->ActivOut();

  // level 7
  Exploit_I10->ActivOut();
  Exploit_I270->ActivOut();

  // level 8
  Exploit_I280->ActivOut();
  Exploit_I280goto30->ActivOut();

  // level 9
  Exploit_I290->ActivOut();

  /*
  cout << "ubdb " << UpdateBestDBFlag->Nr[0]->O << endl;
  cout << "ra " << ElActFL->Nr[RETR_AS-1]->O << endl;
  cout << "df " << ElActFL->Default->Nr[0]->O << endl;
  cout << "DB_GE_BestFlag " << DB_GE_BestFlag->Nr[0]->O << endl;
  cout << "DB";
  for (int i=0; i<ElActfSt->DynamicBias->NN(); i++) {
    if (ElActfSt->DynamicBias->Nr[i]->O==1) cout << " " << i;
  }
  cout << endl;
  cout << "DBCumul";
  for (int i=1; i<DBCumul->NN(); i++) {
    if (DBCumul->Nr[i-1]->O==1 && DBCumul->Nr[i]->O==0) cout << " " << i;
    if (DBCumul->Nr[i-1]->O==0 && DBCumul->Nr[i]->O==1) cout << " Error!";
  }
  cout << endl;
  cout << "DB_GE_BestDB";
  for (int i=0; i<DB_GE_BestDB->NN(); i++) {
    if (DB_GE_BestDB->Nr[i]->O==1) cout << " " << i;
  }
  cout << endl;
  cout << "BestAct";
  for (int i=0; i<BestAct->NN(); i++) {
    if (BestAct->Nr[i]->O==1) cout << " " << i;
  }
  cout << endl;
  cout << "ElActFL";
  for (int i=0; i<ElActFL->NN(); i++) {
    if (ElActFL->Nr[i]->O==1) cout << " " << i;
  }
  cout << endl;


  for (int i=0; i<Exploit_Instr->NN(); i++) {
    cout <<  10*(i+1) << ":" << Exploit_Instr->Nr[i]->O << " ";
  }
  cout << endl;
  */

  RetrBestActFlag->ActivOut();
  RetrBestAct->ActivOut();

  NoRetrFlag->ActivOut();
  NoActFlag->ActivOut();

  StartExploitFlag->ActivOut();
  EndExploitFlag->ActivOut();

  return 0;
}

//////////////////////////////////////////////////////////////////////
// Explore operating mode architecture
//////////////////////////////////////////////////////////////////////
int sllm::ExploreArch()
{
  //Flags
  StartExploreFlag = new ssm(1);
  EndExploreFlag = new ssm(1);
  FlushExploreFlag = new ssm(1);
  MaxIterIFlag = new ssm(1);
  WGTargetFlag = new ssm(1);
  GetRndFlag = new ssm(1);
  EndExplWordFlag = new ssm(1);
  CheckWGFlag = new ssm(1);
  CheckWkPhFlag = new ssm(1);
  TargetEqWGFlag = new ssm(1);
  TargetEqWkPhFlag = new ssm(1);
  EPhaseIFL10Flag = new ssm(1);

  // Exploration phase index
  EPhaseI = new ssm(5);
  CurrEPhaseI = new ssm(5);
  StEPhaseI = new ssm(5);
  CurrStEPhaseI = new ssm(5);

  // Random N1 (words to skip) is SkipW
  // Random N2 (n. of words in a group) is ExplWordNum
  ExplWordNum = new ssm(PhSize);
  EndExplWord = new ssm(PhSize);

  EPhaseIFL10 = new ssm(5);
  StEPhaseIFL20 = new ssm(5);
  StEPhaseIFL370 = new ssm(5);
  EPhaseIFL440 = new ssm(5);

  MaxIterIFlag->Nr[0]->CN(IterI->Nr[MaxExploreIter-1],1);
  MaxIterIFlag->SB(-0.5);

  CurrEPhaseI->SC(EPhaseI, 1);
  CurrEPhaseI->SB(-0.5); 
  EPhaseI->SC(CurrEPhaseI, 1);
  EPhaseI->SB(-0.5);
  EPhaseI->UseDefault();
  CurrStEPhaseI->SC(StEPhaseI, 1);
  CurrStEPhaseI->SB(-0.5); 
  StEPhaseI->SC(CurrStEPhaseI, 1);
  StEPhaseI->SB(-0.5); 

  EndExplWord->SC(WGI, 1);
  EndExplWord->SC(ExplWordNum, 1);
  EndExplWord->SB(-1.5);

  EndExplWordFlag->FC(EndExplWord, 1);
  EndExplWordFlag->Nr[0]->CN(PhI->Nr[PhSize-1], 1);
  EndExplWordFlag->SC(CW->Default, 1);
  EndExplWordFlag->SB(-0.5);

  // instructions
  Explore_I10 = new ssm(1);
  Explore_I20 = new ssm(1);
  Explore_I30 = new ssm(1);
  Explore_I40 = new ssm(1);
  Explore_I50 = new ssm(1);
  Explore_I60 = new ssm(1);
  Explore_I70 = new ssm(1);
  Explore_I80 = new ssm(1);
  Explore_I90 = new ssm(1);
  Explore_I100 = new ssm(1);
  Explore_I110 = new ssm(1);
  Explore_I120 = new ssm(1);
  Explore_I130 = new ssm(1);
  Explore_I140 = new ssm(1);
  Explore_I150 = new ssm(1);
  Explore_I160 = new ssm(1);
  Explore_I170 = new ssm(1);
  Explore_I180 = new ssm(1);
  Explore_I190 = new ssm(1);
  Explore_I200 = new ssm(1);
  Explore_I210 = new ssm(1);
  Explore_I220 = new ssm(1);
  Explore_I230 = new ssm(1);
  Explore_I240 = new ssm(1);
  Explore_I250 = new ssm(1);
  Explore_I260 = new ssm(1);
  Explore_I270 = new ssm(1);
  Explore_I280 = new ssm(1);
  Explore_I290 = new ssm(1);
  Explore_I300 = new ssm(1);
  Explore_I310 = new ssm(1);
  Explore_I320 = new ssm(1);
  Explore_I330 = new ssm(1);
  Explore_I340 = new ssm(1);
  Explore_I350 = new ssm(1);
  Explore_I360 = new ssm(1);
  Explore_I370 = new ssm(1);
  Explore_I380 = new ssm(1);
  Explore_I390 = new ssm(1);
  Explore_I400 = new ssm(1);
  Explore_I410 = new ssm(1);
  Explore_I420 = new ssm(1);
  Explore_I430 = new ssm(1);
  Explore_I440 = new ssm(1);
  Explore_I450 = new ssm(1);

  Explore_I130goto120 = new ssm(1);
  Explore_I170goto150 = new ssm(1);
  Explore_I180goto210 = new ssm(1);
  Explore_I200goto440 = new ssm(1);
  Explore_I250goto280 = new ssm(1);
  Explore_I290goto440 = new ssm(1);
  Explore_I330goto440 = new ssm(1);
  Explore_I360goto440 = new ssm(1);
  Explore_I420goto40 = new ssm(1);

  Explore_Instr = new ssm();
  Explore_Instr->Nr.push_back(Explore_I10->Nr[0]);
  Explore_Instr->Nr.push_back(Explore_I20->Nr[0]);
  Explore_Instr->Nr.push_back(Explore_I30->Nr[0]);
  Explore_Instr->Nr.push_back(Explore_I40->Nr[0]);
  Explore_Instr->Nr.push_back(Explore_I50->Nr[0]);
  Explore_Instr->Nr.push_back(Explore_I60->Nr[0]);
  Explore_Instr->Nr.push_back(Explore_I70->Nr[0]);
  Explore_Instr->Nr.push_back(Explore_I80->Nr[0]);
  Explore_Instr->Nr.push_back(Explore_I90->Nr[0]);
  Explore_Instr->Nr.push_back(Explore_I100->Nr[0]);
  Explore_Instr->Nr.push_back(Explore_I110->Nr[0]);
  Explore_Instr->Nr.push_back(Explore_I120->Nr[0]);
  Explore_Instr->Nr.push_back(Explore_I130->Nr[0]);
  Explore_Instr->Nr.push_back(Explore_I140->Nr[0]);
  Explore_Instr->Nr.push_back(Explore_I150->Nr[0]);
  Explore_Instr->Nr.push_back(Explore_I160->Nr[0]);
  Explore_Instr->Nr.push_back(Explore_I170->Nr[0]);
  Explore_Instr->Nr.push_back(Explore_I180->Nr[0]);
  Explore_Instr->Nr.push_back(Explore_I190->Nr[0]);
  Explore_Instr->Nr.push_back(Explore_I200->Nr[0]);
  Explore_Instr->Nr.push_back(Explore_I210->Nr[0]);
  Explore_Instr->Nr.push_back(Explore_I220->Nr[0]);
  Explore_Instr->Nr.push_back(Explore_I230->Nr[0]);
  Explore_Instr->Nr.push_back(Explore_I240->Nr[0]);
  Explore_Instr->Nr.push_back(Explore_I250->Nr[0]);
  Explore_Instr->Nr.push_back(Explore_I260->Nr[0]);
  Explore_Instr->Nr.push_back(Explore_I270->Nr[0]);
  Explore_Instr->Nr.push_back(Explore_I280->Nr[0]);
  Explore_Instr->Nr.push_back(Explore_I290->Nr[0]);
  Explore_Instr->Nr.push_back(Explore_I300->Nr[0]);
  Explore_Instr->Nr.push_back(Explore_I310->Nr[0]);
  Explore_Instr->Nr.push_back(Explore_I320->Nr[0]);
  Explore_Instr->Nr.push_back(Explore_I330->Nr[0]);
  Explore_Instr->Nr.push_back(Explore_I340->Nr[0]);
  Explore_Instr->Nr.push_back(Explore_I350->Nr[0]);
  Explore_Instr->Nr.push_back(Explore_I360->Nr[0]);
  Explore_Instr->Nr.push_back(Explore_I370->Nr[0]);
  Explore_Instr->Nr.push_back(Explore_I380->Nr[0]);
  Explore_Instr->Nr.push_back(Explore_I390->Nr[0]);
  Explore_Instr->Nr.push_back(Explore_I400->Nr[0]);
  Explore_Instr->Nr.push_back(Explore_I410->Nr[0]);
  Explore_Instr->Nr.push_back(Explore_I420->Nr[0]);
  Explore_Instr->Nr.push_back(Explore_I430->Nr[0]);
  Explore_Instr->Nr.push_back(Explore_I440->Nr[0]);
  Explore_Instr->Nr.push_back(Explore_I450->Nr[0]);

  Explore_Instr->FC(FlushExploreFlag, -BIGWG);
  Explore_I10->Nr[0]->L[0].Wg = 0;

  Explore_I10->SC(StartExploreFlag, 1);
  Explore_I10->SB(-0.5);

  EPhaseIFL10Flag->SC(Explore_I10, 1);
  EPhaseIFL10Flag->SC(WGTargetFlag, 1);
  EPhaseIFL10Flag->SB(-1.5);
  EPhaseIFL10->FC(EPhaseIFL10Flag, 1);
  EPhaseIFL10->SC(EPhaseI, 1);
  EPhaseIFL10->SB(-1.5);
  CurrEPhaseI->FC(EPhaseIFL10Flag, -BIGWG);
  EPhaseI->Nr[0]->CN(EPhaseIFL10->Nr[0], 1);
  EPhaseI->Nr[1]->CN(EPhaseIFL10->Nr[1], 1);
  EPhaseI->Nr[1]->CN(EPhaseIFL10->Nr[3], 1);
  EPhaseI->Nr[1]->CN(EPhaseIFL10->Nr[4], 1);
  EPhaseI->Nr[2]->CN(EPhaseIFL10->Nr[2], 1);

  Explore_I20->SC(Explore_I10, 1);
  Explore_I20->SB(-0.5);
  StEPhaseIFL20->FC(Explore_I20, 1);
  StEPhaseIFL20->SC(EPhaseI, 1);
  StEPhaseIFL20->Nr[0]->CN(EPhaseI->Default->Nr[0], 1);
  StEPhaseIFL20->SB(-1.5);
  CurrStEPhaseI->FC(Explore_I20, -BIGWG);
  StEPhaseI->Nr[1]->CN(StEPhaseIFL20->Nr[0], 1);
  StEPhaseI->Nr[1]->CN(StEPhaseIFL20->Nr[1], 1);
  StEPhaseI->Nr[2]->CN(StEPhaseIFL20->Nr[2], 1);
  StEPhaseI->Nr[3]->CN(StEPhaseIFL20->Nr[3], 1);
  StEPhaseI->Nr[4]->CN(StEPhaseIFL20->Nr[4], 1);

  Explore_I30->SC(Explore_I20, 1);
  Explore_I30->SB(-0.5);
  StartIterIFlag->SC(Explore_I30, 1);

  Explore_I40->SC(Explore_I30, 1);
  Explore_I40->SC(Explore_I420goto40, 1);
  Explore_I40->SB(-0.5);

  Explore_I50->SC(Explore_I40, 1);
  Explore_I50->Nr[0]->CN(EPhaseI->Nr[0], 1);
  Explore_I50->Nr[0]->CN(EPhaseI->Default->Nr[0], 1);
  Explore_I50->SB(-1.5);
  RwdAct->Nr[START_ST_A-1]->CN(Explore_I50, 1);

  Explore_I60->SC(Explore_I50, 1);
  Explore_I60->SB(-0.5);
  ElActFL->Nr[W_FROM_IN-1]->CN(Explore_I60, 1);
  RwdAct->Nr[STORE_ST_A-1]->CN(Explore_I60, 1);

  Explore_I70->SC(Explore_I60, 1);
  Explore_I70->SB(-0.5);

  /////////////////////////////////////////////////
  // Exploration approve:
  RwdAct->Nr[STORE_SAI-1]->CN(Explore_I70, 1);
  //EPhaseIFL70->FC(Explore_I70, 1);
  //EPhaseIFL70->SC(EPhaseI, 1);
  //EPhaseIFL70->SB(-1.5);
  //CurrEPhaseI->FC(Explore_I70, -BIGWG);
  //EPhaseI->Nr[1]->SC(EPhaseIFL70->Nr[0], 1);
  //EPhaseI->Nr[2]->SC(EPhaseIFL70->Nr[1], 1);
  //EPhaseI->Nr[3]->SC(EPhaseIFL70->Nr[2], 1);
  //EPhaseI->Nr[4]->SC(EPhaseIFL70->Nr[3], 1);
  /////////////////////////////////////////////////

  Explore_I80->SC(Explore_I70, 1);
  Explore_I80->SB(-0.5);
  CurrEPhaseI->FC(Explore_I80, -BIGWG);
  EPhaseI->Nr[1]->CN(Explore_I80, 1);

  Explore_I100->SC(Explore_I40, 1);
  Explore_I100->SC(Explore_I80, 1);
  Explore_I100->Nr[0]->CN(EPhaseI->Nr[1], 1);
  Explore_I100->SB(-1.5);
  GetRndFlag->SC(Explore_I100, 1);
  GetRndFlag->SB(-0.5);

  Explore_I110->SC(Explore_I100, 1);
  Explore_I110->SB(-0.5);
  ElActFL->Nr[W_FROM_WK-1]->CN(Explore_I110, 1);
  RwdAct->Nr[STORE_ST_A-1]->CN(Explore_I110, 1);

  Explore_I120->SC(Explore_I110, 1);
  Explore_I120->SC(Explore_I130goto120, 1);
  Explore_I120->SB(-0.5);
  ElActFL->Nr[NEXT_W-1]->CN(Explore_I120, 1);
  RwdAct->Nr[STORE_ST_A-1]->CN(Explore_I120, 1);

  Explore_I130->SC(Explore_I120, 1);
  Explore_I130->SB(-0.5);

  Explore_I130goto120->SC(Explore_I130, 1);
  Explore_I130goto120->SC(PhIEqSkipWFlag,-1);
  Explore_I130goto120->SB(-0.5);

  Explore_I140->SC(Explore_I130, 1);
  Explore_I140->SC(Explore_I130goto120, -1);
  Explore_I140->SB(-0.5);
  ElActFL->Nr[FLUSH_WG-1]->CN(Explore_I140, 1);
  RwdAct->Nr[STORE_ST_A-1]->CN(Explore_I140, 1);

  Explore_I150->SC(Explore_I140, 1);
  Explore_I150->SC(Explore_I170goto150, 1);
  Explore_I150->SB(-0.5);
  ElActFL->Nr[GET_W-1]->CN(Explore_I150, 1);
  RwdAct->Nr[STORE_ST_A-1]->CN(Explore_I150, 1);

  Explore_I160->SC(Explore_I150, 1);
  Explore_I160->SB(-0.5);
  ElActFL->Nr[NEXT_W-1]->CN(Explore_I160, 1);
  RwdAct->Nr[STORE_ST_A-1]->CN(Explore_I160, 1);

  Explore_I170->SC(Explore_I160, 1);
  Explore_I170->SB(-0.5);

  Explore_I170goto150->SC(Explore_I170, 1);
  Explore_I170goto150->SC(EndExplWordFlag, -1);
  Explore_I170goto150->SB(-0.5);

  Explore_I180->SC(Explore_I170, 1);
  Explore_I180->SC(Explore_I170goto150, -1);
  Explore_I180->SB(-0.5);

  Explore_I180goto210->SC(Explore_I180, 1);
  Explore_I180goto210->SC(WGTargetFlag, -1);
  Explore_I180goto210->SB(-0.5);

  Explore_I190->SC(Explore_I180, 1);
  Explore_I190->SC(Explore_I180goto210, -1);
  Explore_I190->SB(-0.5);
  CheckWGFlag->SC(Explore_I190, 1);
  CheckWGFlag->SB(-0.5);

  Explore_I200->SC(Explore_I190, 1);
  Explore_I200->SB(-0.5);

  Explore_I200goto440->SC(Explore_I200, 1);
  Explore_I200goto440->SC(TargetEqWGFlag, 1);
  Explore_I200goto440->SB(-1.5);
  
  Explore_I210->SC(Explore_I180goto210, 1);
  Explore_I210->SB(-0.5);
  CurrEPhaseI->FC(Explore_I210, -BIGWG);
  EPhaseI->Nr[2]->CN(Explore_I210, 1);

  Explore_I220->SC(Explore_I210, 1);
  Explore_I220->SB(-0.5);

  Explore_I240->SC(Explore_I40, 1);
  Explore_I240->Nr[0]->CN(EPhaseI->Nr[2], 1);
  Explore_I240->SB(-1.5);
  ElActFL->Nr[RETR_AS-1]->CN(Explore_I240, 1);
  RwdAct->Nr[STORE_ST_A-1]->CN(Explore_I240, 1);

  Explore_I250->SC(Explore_I240, 1);
  Explore_I250->SB(-0.5);

  Explore_I250goto280->SC(Explore_I250, 1);
  Explore_I250goto280->SC(WGTargetFlag, -1);
  Explore_I250goto280->SB(-0.5);

  Explore_I260->SC(Explore_I250, 1);
  Explore_I260->SC(Explore_I250goto280, -1);
  Explore_I260->SB(-0.5);
  CurrEPhaseI->FC(Explore_I260, -BIGWG);
  EPhaseI->Nr[1]->CN(Explore_I260, 1);

  Explore_I270->SC(Explore_I260, 1);
  Explore_I270->SB(-0.5);

  Explore_I280->SC(Explore_I250goto280, 1);
  Explore_I280->SB(-0.5);
  CheckWkPhFlag->SC(Explore_I280, 1);
  CheckWkPhFlag->SB(-0.5);

  Explore_I290->SC(Explore_I280, 1);
  Explore_I290->SB(-0.5);

  Explore_I290goto440->SC(Explore_I290, 1);
  Explore_I290goto440->SC(TargetEqWkPhFlag, 1);
  Explore_I290goto440->SB(-1.5);
  
  Explore_I310->SC(Explore_I40, 1);
  Explore_I310->Nr[0]->CN(EPhaseI->Nr[3], 1);
  Explore_I310->SB(-1.5);
  ElActFL->Nr[GET_START_PH-1]->CN(Explore_I310, 1);
  RwdAct->Nr[STORE_ST_A-1]->CN(Explore_I310, 1);

  Explore_I320->SC(Explore_I310, 1);
  Explore_I320->SB(-0.5);
  CheckWkPhFlag->SC(Explore_I320, 1);

  Explore_I330->SC(Explore_I320, 1);
  Explore_I330->SB(-0.5);

  Explore_I330goto440->SC(Explore_I330, 1);
  Explore_I330goto440->SC(TargetEqWkPhFlag, 1);
  Explore_I330goto440->SB(-1.5);
  
  Explore_I340->SC(Explore_I40, 1);
  Explore_I340->Nr[0]->CN(EPhaseI->Nr[4], 1);
  Explore_I340->SB(-1.5);
  ElActFL->Nr[GET_NEXT_PH-1]->CN(Explore_I340, 1);
  RwdAct->Nr[STORE_ST_A-1]->CN(Explore_I340, 1);

  Explore_I350->SC(Explore_I340, 1);
  Explore_I350->SB(-0.5);
  CheckWkPhFlag->SC(Explore_I350, 1);

  Explore_I360->SC(Explore_I350, 1);
  Explore_I360->SB(-0.5);

  Explore_I360goto440->SC(Explore_I360, 1);
  Explore_I360goto440->SC(TargetEqWkPhFlag, 1);
  Explore_I360goto440->SB(-1.5);
 
  Explore_I370->SC(Explore_I200, 1);
  Explore_I370->SC(Explore_I200goto440, -1);
  Explore_I370->SC(Explore_I290, 1);
  Explore_I370->SC(Explore_I290goto440, -1);
  Explore_I370->SC(Explore_I330, 1);
  Explore_I370->SC(Explore_I330goto440, -1);
  Explore_I370->SC(Explore_I360, 1);
  Explore_I370->SC(Explore_I360goto440, -1);
  Explore_I370->SB(-0.5);

  StEPhaseIFL370->FC(Explore_I370, 1);
  StEPhaseIFL370->SC(StEPhaseI, 1);
  StEPhaseIFL370->SB(-1.5);
  CurrEPhaseI->FC(Explore_I370, -BIGWG);
  CurrStEPhaseI->FC(Explore_I370, -BIGWG);
  EPhaseI->SC(StEPhaseIFL370, 1);
  StEPhaseI->Nr[1]->CN(StEPhaseIFL370->Nr[0], 1);
  StEPhaseI->Nr[1]->CN(StEPhaseIFL370->Nr[1], 1);
  StEPhaseI->Nr[1]->CN(StEPhaseIFL370->Nr[4], 1);
  StEPhaseI->Nr[2]->CN(StEPhaseIFL370->Nr[2], 1);
  StEPhaseI->Nr[4]->CN(StEPhaseIFL370->Nr[3], 1);

  Explore_I380->SC(Explore_I370, 1);
  Explore_I380->SB(-0.5);
  RwdAct->Nr[RETR_SAI-1]->CN(Explore_I380, 1);

  Explore_I390->SC(Explore_I380, 1);
  Explore_I390->SB(-0.5);
  RwdAct->Nr[RETR_ST-1]->CN(Explore_I390, 1);

  Explore_I400->SC(Explore_I390, 1);
  Explore_I400->SB(-0.5);
  RwdAct->Nr[RETR_SAI-1]->CN(Explore_I400, 1);

  Explore_I410->SC(Explore_I400, 1);
  Explore_I410->SC(Explore_I220, 1);
  Explore_I410->SC(Explore_I270, 1);
  Explore_I410->SB(-0.5);
  NextIterIFlag->SC(Explore_I410, 1);

  Explore_I420->SC(Explore_I410, 1);
  Explore_I420->SB(-0.5);

  Explore_I420goto40->SC(Explore_I420, 1);
  Explore_I420goto40->SC(MaxIterIFlag, -1);
  Explore_I420goto40->SB(-0.5);

  Explore_I430->SC(Explore_I420, 1);
  Explore_I430->SC(Explore_I420goto40, -1);
  Explore_I430->SB(-0.5);

  Explore_I440->SC(Explore_I200goto440, 1);
  Explore_I440->SC(Explore_I290goto440, 1);
  Explore_I440->SC(Explore_I330goto440, 1);
  Explore_I440->SC(Explore_I360goto440, 1);
  Explore_I440->SB(-0.5);
  /////////////////////////////////////////////////
  // Exploration approve:
  RwdAct->Nr[STORE_SAI-1]->CN(Explore_I440, 1);
  EPhaseIFL440->FC(Explore_I440, 1);
  EPhaseIFL440->SC(EPhaseI, 1);
  EPhaseIFL440->SB(-1.5);
  CurrEPhaseI->FC(Explore_I440, -BIGWG);
  EPhaseI->Nr[1]->CN(EPhaseIFL440->Nr[0], 1);
  EPhaseI->Nr[2]->CN(EPhaseIFL440->Nr[1], 1);
  EPhaseI->Nr[3]->CN(EPhaseIFL440->Nr[2], 1);
  EPhaseI->Nr[4]->CN(EPhaseIFL440->Nr[3], 1);
  EPhaseI->Nr[4]->CN(EPhaseIFL440->Nr[4], 1);
  /////////////////////////////////////////////////

  Explore_I450->SC(Explore_I440, 1);
  Explore_I450->SC(Explore_I430, 1);
  Explore_I450->SB(-0.5);

  StartExploreFlag->SC(StartExploreFlag, -1);
  StartExploreFlag->SB(-0.5);

  EndExploreFlag->SC(Explore_I450, 1);
  EndExploreFlag->SB(-0.5);

  Explore->FC(Explore_Instr,1);
  Explore->SC(StartExploreFlag, 1);
  Explore->SB(-0.5);

  FlushExploreFlag->FC(StartExploreFlag, 1);
  FlushExploreFlag->FC(Explore, -1);
  FlushExploreFlag->SB(+0.5);

  return 0;
}


int sllm::ExploreUpdate()
{
  Explore->ActivOut();
  FlushExploreFlag->ActivOut();
  
  MaxIterIFlag->ActivOut();

  EndExplWord->ActivOut();
  EndExplWordFlag->ActivOut();

  // level 1
  Explore_I170->ActivOut();

  // level 1b
  Explore_I170goto150->ActivOut();

  // level 2
  Explore_I160->ActivOut();
  Explore_I180->ActivOut();
  Explore_I200->ActivOut();
  Explore_I290->ActivOut();
  Explore_I330->ActivOut();
  Explore_I360->ActivOut();

  // level 2b
  Explore_I180goto210->ActivOut();
  Explore_I200goto440->ActivOut();
  Explore_I290goto440->ActivOut();
  Explore_I330goto440->ActivOut();
  Explore_I360goto440->ActivOut();

  // level 3
  Explore_I150->ActivOut();
  Explore_I190->ActivOut();
  Explore_I210->ActivOut();
  Explore_I260->ActivOut();
  
  // level 4
  Explore_I220->ActivOut();
  Explore_I270->ActivOut();

  // level 5
  Explore_I20->ActivOut();
  Explore_I410->ActivOut();

  // level 5b
  StEPhaseIFL20->ActivOut();

  // level 6
  Explore_I10->ActivOut();
  Explore_I30->ActivOut();
  Explore_I70->ActivOut();
  Explore_I400->ActivOut();
  Explore_I420->ActivOut();

  //level 6b
  Explore_I420goto40->ActivOut();
  EPhaseIFL10Flag->ActivOut();
  EPhaseIFL10->ActivOut();

  CurrEPhaseI->ActivOut();
  EPhaseI->ActivOut();
  CurrStEPhaseI->ActivOut();
  StEPhaseI->ActivOut();

  // level 7
  Explore_I40->ActivOut();
  Explore_I60->ActivOut();
  Explore_I130->ActivOut();
  Explore_I370->ActivOut();
  Explore_I390->ActivOut();
  Explore_I430->ActivOut();

  // level 7b
  Explore_I130goto120->ActivOut();
  StEPhaseIFL370->ActivOut();

  // level 8
  Explore_I50->ActivOut();
  Explore_I100->ActivOut();
  Explore_I120->ActivOut();
  Explore_I140->ActivOut();
  Explore_I240->ActivOut();
  Explore_I310->ActivOut();
  Explore_I340->ActivOut();
  Explore_I380->ActivOut();
  Explore_I440->ActivOut();

  // level 8 b
  EPhaseIFL440->ActivOut();

  // level 9
  Explore_I80->ActivOut();
  Explore_I110->ActivOut();
  Explore_I250->ActivOut();
  Explore_I290->ActivOut();
  Explore_I320->ActivOut();
  Explore_I350->ActivOut();
  Explore_I450->ActivOut();

  // level 9b
  Explore_I250goto280->ActivOut();

  // level 10
  Explore_I280->ActivOut();
  
  GetRndFlag->ActivOut();
  CheckWGFlag->ActivOut();
  CheckWkPhFlag->ActivOut();

  StartExploreFlag->ActivOut();
  EndExploreFlag->ActivOut();

  /*
  cout << "IterI:";
  for (int i=0; i<IterSize; i++) {
    if (IterI->Nr[i]->O>0.5) cout << " " << i;
  }
  cout << endl;
  cout << MaxIterIFlag->Nr[0]->O << endl;
  cout << Explore_I420goto40->Nr[0]->O << endl;
  cout << Explore_I430->Nr[0]->O << endl;
  for (int i=0; i<Explore_Instr->NN(); i++) {
    cout <<  10*(i+1) << ":" << Explore_Instr->Nr[i]->O << " ";
  }
  cout << endl;
  */

  return 0;
}

