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

#ifndef SLLMH
#define SLLMH
#include "fssm.h"
#define FAST_SSM       // Comment to use slow version.
//#define FAST_SSM_AS    // Comment to use slow version.
// Really the current "slow" ssm_as version is faster than the "fast" version 
#define FAST_SSM_SORT  // For comparison with slow version. You can comment it


#ifdef FAST_SSM_AS
#define XSSM_AS fssm_as
#else
#define XSSM_AS ssm_as
#endif

#ifdef FAST_SSM
#define XSSM fssm
#define XSSM2d fssm2d
#else
#define XSSM ssm
#define XSSM2d ssm2d
#endif

enum {NULL_ACT, FLUSH_WG, W_FROM_WK, W_FROM_IN, NEXT_W, GET_W,
      RETR_AS, FLUSH_OUT, WG_OUT, GET_NEXT_PH, GET_START_PH,
      CONTINUE, DONE, PUSH_GL, DROP_GL, GET_GL_PH,
      SNT_OUT, EL_ACT_16, EL_ACT_17, EL_ACT_18, EL_ACT_19};
enum {FLUSH=1, ACQ_W, NEXT_AS_W, BUILD_AS, MEM_PH, SET_START_PH};
enum {NULL_MODE, ACQUIRE, ASSOCIATE, EXPLORE, EXPLORE2, EXPLOIT, BEST_EXPLOIT,
      REWARD};
enum {NULL_RWD, STORE_ST_A, START_ST_A, RETR_ST_A, RWD_ST_A, CHANGE_ST_A,
      GET_ST_A, RETR_EL_A, STORE_SAI, RETR_SAI, RETR_ST, RWD_ACT_10,
      RWD_ACT_11, RWD_ACT_12, RWD_ACT_13, RWD_ACT_14};

const int ElActSize=20;
const int AcqActSize=6;
const int ActFlagSize=30;
const int ModeNum=7;
const int RwdActSize=15;
const int RwdFlagSize=10;

const int v_el_act[ElActSize+1][ElActSize] = {
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, // NULL_ACT
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, // FLUSH_WG
  {0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, // W_FROM_WK
  {0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, // W_FROM_IN
  {0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, // NEXT_W
  {0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, // GET_W
  {0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, // RETR_AS
  {0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0}, // FLUSH_OUT
  {0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0}, // WG_OUT
  {0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0}, // GET_NEXT_PH
  {0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0}, // GET_START_PH
  {0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0}, // CONTINUE
  {0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0}, // DONE
  {0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0}, // PUSH_GL
  {0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0}, // DROP_GL
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0}, // GET_GL_PH
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0}, // SNT_OUT
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0}, // EL_ACT_16
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0}, // EL_ACT_17
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0}, // EL_ACT_18
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1}, // EL_ACT_19
};

const int v_acq_act[AcqActSize+1][AcqActSize] = {
  {0,0,0,0,0,0}, // NULL_ACT
  {1,0,0,0,0,0}, // FLUSH
  {0,1,0,0,0,0}, // ACQ_W
  {0,0,1,0,0,0}, // NEXT_AS_W
  {0,0,0,1,0,0}, // BUILD_AS
  {0,0,0,0,1,0}, // MEM_PH
  {0,0,0,0,0,1}  // SET_START_PH
};

const int v_mode[ModeNum+1][ModeNum] = {
  {0,0,0,0,0,0,0}, // NULL_MODE
  {1,0,0,0,0,0,0}, // ACQUIRE
  {0,1,0,0,0,0,0}, // ASSOCIATE
  {0,0,1,0,0,0,0}, // EXPLORE
  {0,0,0,1,0,0,0}, // EXPLORE2
  {0,0,0,0,1,0,0}, // EXPLOIT
  {0,0,0,0,0,1,0}, // BEST_EXPLOIT
  {0,0,0,0,0,0,1}  // REWARD
};

const int v_rwd_act[RwdActSize+1][RwdActSize] = {
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, // NULL_RWD
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, // STORE_ST_A
  {0,1,0,0,0,0,0,0,0,0,0,0,0,0,0}, // START_ST_A
  {0,0,1,0,0,0,0,0,0,0,0,0,0,0,0}, // RETR_ST_A
  {0,0,0,1,0,0,0,0,0,0,0,0,0,0,0}, // RWD_ST_A
  {0,0,0,0,1,0,0,0,0,0,0,0,0,0,0}, // CHANGE_ST_A
  {0,0,0,0,0,1,0,0,0,0,0,0,0,0,0}, // GET_ST_A
  {0,0,0,0,0,0,1,0,0,0,0,0,0,0,0}, // RETR_EL_A
  {0,0,0,0,0,0,0,1,0,0,0,0,0,0,0}, // STORE_SAI
  {0,0,0,0,0,0,0,0,1,0,0,0,0,0,0}, // RETR_SAI
  {0,0,0,0,0,0,0,0,0,1,0,0,0,0,0}, // RETR_ST
  {0,0,0,0,0,0,0,0,0,0,1,0,0,0,0}, // RWD_ACT_10
  {0,0,0,0,0,0,0,0,0,0,0,1,0,0,0}, // RWD_ACT_11
  {0,0,0,0,0,0,0,0,0,0,0,0,1,0,0}, // RWD_ACT_12
  {0,0,0,0,0,0,0,0,0,0,0,0,0,1,0}, // RWD_ACT_13
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,1}, // RWD_ACT_14
};

class sllm
{
 public:
  bool CudaFlag;
  static const float BIGWG=1e20;
  ssm *W;
  ssm *IW;
  XSSM *CW;
  ssm *PhI;
  ssm *InFlag;
  ssm *FlushIn;
  ssm *InI;
  XSSM *InEqW;
  ssm *InEqWC;
  XSSM2d *InPhFL;
  XSSM2d *InPhB;
  //XSSM2d *InWfI;
  XSSM2d *InIfW;
  XSSM2d *InIfWGW1;
  XSSM2d *InIfWGW2;
  XSSM2d *InIfWGW3;
  XSSM2d *InIfWGW4;
  XSSM *InEqWGW1;
  XSSM *InEqWGW2;
  XSSM *InEqWGW3;
  XSSM *InEqWGW4;
  ssm *InEqWGW1C;
  ssm *InEqWGW2C;
  ssm *InEqWGW3C;
  ssm *InEqWGW4C;
  ssm *PrevInEqWGW1FL;
  ssm *PrevInEqWGW2FL;
  ssm *PrevInEqWGW3FL;
  ssm *PrevInEqWGW4FL;
  ssm *PrevInEqWGW1;
  ssm *PrevInEqWGW2;
  ssm *PrevInEqWGW3;
  ssm *PrevInEqWGW4;
  ssm *PrevInEqWGW1C;
  ssm *PrevInEqWGW2C;
  ssm *PrevInEqWGW3C;
  ssm *PrevInEqWGW4C;

  ssm *WkFlag;
  ssm *FlushWk;
  //ssm *WkI;
  XSSM *WkEqW;
  ssm *WkEqWC;
  XSSM2d *WkPhFL;
  XSSM2d *CurrWkPh;
  XSSM2d *WkPhB;
  XSSM2d *WkIfW;
  XSSM2d *WkWfI;
  XSSM2d *WkIfWGW1;
  XSSM2d *WkIfWGW2;
  XSSM2d *WkIfWGW3;
  XSSM2d *WkIfWGW4;
  XSSM *WkEqWGW1;
  XSSM *WkEqWGW2;
  XSSM *WkEqWGW3;
  XSSM *WkEqWGW4;
  ssm *WkEqWGW1C;
  ssm *WkEqWGW2C;
  ssm *WkEqWGW3C;
  ssm *WkEqWGW4C;
  XSSM2d *PrevWkPhFL;
  XSSM2d *PrevWkPh;
  ssm *PrevWkEqWGW1FL;
  ssm *PrevWkEqWGW2FL;
  ssm *PrevWkEqWGW3FL;
  ssm *PrevWkEqWGW4FL;
  ssm *PrevWkEqWGW1;
  ssm *PrevWkEqWGW2;
  ssm *PrevWkEqWGW3;
  ssm *PrevWkEqWGW4;
  ssm *PrevWkEqWGW1C;
  ssm *PrevWkEqWGW2C;
  ssm *PrevWkEqWGW3C;
  ssm *PrevWkEqWGW4C;

  ssm *OutFlag;
  ssm *FlushOut;
  XSSM *OutEqW;
  ssm *OutEqWC;
  XSSM2d *OutPhFL;
  XSSM2d *OutPhB;
  XSSM2d *OutIfW;

  XSSM2d *WkOutPhFL;
  XSSM2d *WkOutPhB;
  ssm *WkOutEqWGW1FL;
  ssm *WkOutEqWGW2FL;
  ssm *WkOutEqWGW3FL;
  ssm *WkOutEqWGW4FL;
  ssm *WkOutEqWGW1;
  ssm *WkOutEqWGW2;
  ssm *WkOutEqWGW3;
  ssm *WkOutEqWGW4;
  ssm *WkOutEqWGW1C;
  ssm *WkOutEqWGW2C;
  ssm *WkOutEqWGW3C;
  ssm *WkOutEqWGW4C;

  ssm *GetFlag;
  ssm *WGI;
  XSSM *WGCW;
  ssm *WGFlag;
  ssm *FlushWG;
  ssm *WGIFL;
  XSSM *WGEqW;
  ssm *WGEqWC;
  XSSM2d *WGFL;
  XSSM2d *WGB;
  XSSM2d *WGIfW;
  XSSM2d *PrevWGFL;
  XSSM2d *PrevWG;

  ssm *RetrAs;
  ssm *BuildAs;

  ssm *StartStActIFlag;
  XSSM *CurrStActI;
  ssm *CurrStActIFlag;
  XSSM *NextStActI;
  ssm *NextStActIFlag;
  XSSM *StActI;
  ssm *StoreStActIFlag;
  XSSM *StoredStActIFL;
  XSSM *StoredStActI;
  XSSM *CurrStoredStActI;
  ssm *RetrStActIFlag;
  XSSM *RetrStActI;

  ssm *RetrStAct;
  ssm *BuildStAct;
  XSSM_AS *StActMem;

  ssm *ElActFL;
  ssm *ElAct;
  ssm *AcqAct;
  ssm *SetNullElAct;

  ssm *ActFlag22;
  ssm *ActFlag23;
  ssm *ActFlag24;
  ssm *ActFlag25;
  ssm *ActFlag26;
  ssm *ActFlag27;
  ssm *ActFlag28;
  ssm *ActFlag29;

  ssm *ActFlags;
  ssm *ActFlagsC;

  ssm *CurrPhI;
  ssm *CurrPhIFlag;
  ssm *NextPhI;
  ssm *NextPhIFlag;

  ssm *CurrWGI;
  ssm *CurrWGIFlag;
  ssm *NextWGI;
  ssm *NextWGIFlag;

  ssm *RetrElActfSt;
  ssm *BuildElActfSt;
  ssm *RewdElActfSt;
  XSSM_AS *ElActfSt;

  XSSM *CurrMemPh;
  XSSM *NextMemPh;
  ssm *NewMemPhFlag;
  XSSM *MemPh;

  ssm *RetrRemPh;
  XSSM *RemPhIL;
  XSSM *CurrRemPh;
  ssm *CurrRemPhFlag;
  XSSM *NextRemPh;
  ssm *NextRemPhFlag;
  XSSM_AS *RemPh;
  XSSM_AS *RemPhfWG;

  ssm *StartPhFlag;
  XSSM *StartPhIL;
  XSSM *StartPh;
  XSSM *CurrStartPh;

  ssm *RemStartPhFlag;
  ssm *CurrStartWkPhFlag;
  XSSM *StartWkPhIL;
  XSSM *CurrStartWkPh;
  XSSM *StartWkPh;
  XSSM *RemStartPh;

  ssm *PrevRetrAsFlag;
  ssm *RepeatRetrAsFlag;
  ssm *BuildRewardFlag;
  ssm *RwdAct;
  ssm *RwdFlag7;
  ssm *RwdFlag8;
  ssm *RwdFlag9;

  ssm *RwdFlags;

  ssm *ModeFlags;
  ssm *Acquire;
  ssm *Associate;
  ssm *Explore;
  ssm *Explore2;
  ssm *Exploit;
  ssm *BestExploit;
  ssm *Reward;

  //Acquire operating mode
  ssm *StartAcquireFlag;
  ssm *BlankWFlag;
  ssm *EndInPhFlag;
  ssm *FlushAcquireFlag;
  ssm *EndAcquireFlag;

  ssm *Acquire_I10;
  ssm *Acquire_I20;
  ssm *Acquire_I30;
  ssm *Acquire_I40;
  ssm *Acquire_I50;
  ssm *Acquire_I60;
  ssm *Acquire_I30goto20;
  ssm *Acquire_Instr;

  //Associate operating mode
  ssm *StartAssociateFlag;
  ssm *StartSkipWFlag;
  ssm *NextSkipWFlag;
  ssm *BlankSkipWFlag;
  ssm *FlushAssociateFlag;
  ssm *EndWordGroupFlag;
  ssm *EndSkipWFlag;
  ssm *EndAssociateFlag;
  ssm *MemPhFlag;
  ssm *SetStartPhFlag;
  ssm *SkipW;
  ssm *CurrSkipW;
  ssm *NextSkipW;
  ssm *BlankSkipW;
  ssm *PhIEqSkipW;
  ssm *PhIEqSkipWFlag;

  ssm *Associate_I10;
  ssm *Associate_I20;
  ssm *Associate_I30;
  ssm *Associate_I40;
  ssm *Associate_I50;
  ssm *Associate_I60;
  ssm *Associate_I70;
  ssm *Associate_I80;
  ssm *Associate_I90;
  ssm *Associate_I100;
  ssm *Associate_I110;
  ssm *Associate_I120;
  ssm *Associate_I130;
  ssm *Associate_I140;
  ssm *Associate_I30goto130;
  ssm *Associate_I50goto40;
  ssm *Associate_I90goto70;
  ssm *Associate_Instr;

  //Reward operating mode
  ssm *StartRewardFlag;
  ssm *StartIterIFlag;
  ssm *NextIterIFlag;
  ssm *FlushRewardFlag;
  ssm *EndIterIFlag;
  ssm *EndStActIFlag;
  ssm *EndRewardFlag;
  ssm *GetStActIFlag;
  ssm *PartialFlag;
  ssm *DoneFlag;
  ssm *ContinueFlag;
  ssm *IterNum;
  ssm *IterI;
  ssm *CurrIterI;
  ssm *NextIterI;
  ssm *EndIterI;
  ssm *LastStActIFL;
  ssm *LastStActI;
  ssm *EndStActI;

  ssm *Reward_I10;
  ssm *Reward_I20;
  ssm *Reward_I30;
  ssm *Reward_I40;
  ssm *Reward_I50;
  ssm *Reward_I60;
  ssm *Reward_I70;
  ssm *Reward_I80;
  ssm *Reward_I90;
  ssm *Reward_I100;
  ssm *Reward_I110;
  ssm *Reward_I90goto80;
  ssm *Reward_I100goto60;
  ssm *Reward_Instr;

  //Exploit operating mode
  ssm *StartExploitFlag;
  ssm *EndExploitFlag;
  ssm *FlushExploitFlag;
  ssm *StartNRetrFlag;
  ssm *NextNRetrFlag;
  ssm *EndNRetrFlag;

  ssm *GetDBFlag;
  ssm *RetrBestActFlag;
  ssm *DB_GE_BestFlag;
  ssm *InitBestDBFlag;
  ssm *MaxStActIFlag;
  //  ssm *GetRetrAsFlag;
  ssm *UpdateBestDBFlag;
  ssm *NoRetrFlag;
  ssm *NoActFlag;
  ssm *MaxStActIWarn;

  ssm *NRetr;
  ssm *CurrNRetr;
  ssm *NextNRetr;

  ssm *BestDBFL;
  ssm *BestDB;
  ssm *CurrBestDB;
  ssm *BestActFL;
  ssm *BestAct;
  ssm *CurrBestAct;
  ssm *RetrBestAct;

  ssm *DBCumul;
  ssm *DB_GE_BestDB;
  // ssm *PrevRetrAsFL;
  // ssm *PrevRetrAs;

  ssm *Exploit_I10;
  ssm *Exploit_I20;
  ssm *Exploit_I30;
  ssm *Exploit_I40;
  ssm *Exploit_I50;
  ssm *Exploit_I60;
  ssm *Exploit_I70;
  ssm *Exploit_I80;
  ssm *Exploit_I90;
  ssm *Exploit_I100;
  ssm *Exploit_I110;
  ssm *Exploit_I120;
  ssm *Exploit_I130;
  ssm *Exploit_I140;
  ssm *Exploit_I150;
  ssm *Exploit_I160;
  ssm *Exploit_I170;
  ssm *Exploit_I180;
  ssm *Exploit_I190;
  ssm *Exploit_I200;
  ssm *Exploit_I210;
  ssm *Exploit_I220;
  ssm *Exploit_I230;
  ssm *Exploit_I240;
  ssm *Exploit_I250;
  ssm *Exploit_I260;
  ssm *Exploit_I270;
  ssm *Exploit_I280;
  ssm *Exploit_I290;
  ssm *Exploit_I50goto200;
  ssm *Exploit_I120goto150;
  ssm *Exploit_I160goto90;
  ssm *Exploit_I170goto270;
  ssm *Exploit_I220goto270;
  ssm *Exploit_I240goto270;
  ssm *Exploit_I250goto50;
  ssm *Exploit_I280goto30;

  ssm *Exploit_Instr;

  //Explore operating mode
  ssm *StartExploreFlag;
  ssm *EndExploreFlag;
  ssm *FlushExploreFlag;
  ssm *MaxIterIFlag;
  ssm *WGTargetFlag;
  ssm *GetRndFlag;
  ssm *EndExplWordFlag;
  ssm *CheckWGFlag;
  ssm *CheckWkPhFlag;
  ssm *TargetEqWGFlag;
  ssm *TargetEqWkPhFlag;
  ssm *EPhaseIFL10Flag;

  ssm *EPhaseI;
  ssm *CurrEPhaseI;
  ssm *StEPhaseI;
  ssm *CurrStEPhaseI;

  ssm *ExplWordNum;
  ssm *EndExplWord;

  ssm *EPhaseIFL10;
  ssm *StEPhaseIFL20;
  ssm *StEPhaseIFL370;
  ssm *EPhaseIFL440;

  ssm *Explore_I10;
  ssm *Explore_I20;
  ssm *Explore_I30;
  ssm *Explore_I40;
  ssm *Explore_I50;
  ssm *Explore_I60;
  ssm *Explore_I70;
  ssm *Explore_I80;
  ssm *Explore_I90;
  ssm *Explore_I100;
  ssm *Explore_I110;
  ssm *Explore_I120;
  ssm *Explore_I130;
  ssm *Explore_I140;
  ssm *Explore_I150;
  ssm *Explore_I160;
  ssm *Explore_I170;
  ssm *Explore_I180;
  ssm *Explore_I190;
  ssm *Explore_I200;
  ssm *Explore_I210;
  ssm *Explore_I220;
  ssm *Explore_I230;
  ssm *Explore_I240;
  ssm *Explore_I250;
  ssm *Explore_I260;
  ssm *Explore_I270;
  ssm *Explore_I280;
  ssm *Explore_I290;
  ssm *Explore_I300;
  ssm *Explore_I310;
  ssm *Explore_I320;
  ssm *Explore_I330;
  ssm *Explore_I340;
  ssm *Explore_I350;
  ssm *Explore_I360;
  ssm *Explore_I370;
  ssm *Explore_I380;
  ssm *Explore_I390;
  ssm *Explore_I400;
  ssm *Explore_I410;
  ssm *Explore_I420;
  ssm *Explore_I430;
  ssm *Explore_I440;
  ssm *Explore_I450;

  ssm *Explore_I130goto120;
  ssm *Explore_I170goto150;
  ssm *Explore_I180goto210;
  ssm *Explore_I200goto440;
  ssm *Explore_I250goto280;
  ssm *Explore_I290goto440;
  ssm *Explore_I330goto440;
  ssm *Explore_I360goto440;
  ssm *Explore_I420goto40;

  ssm *Explore_Instr;

  // goals
  ssm *SetGoalFlag;
  ssm *GetGoalFlag;
  ssm *FlushGoal;
  XSSM2d *GoalWGFL;
  XSSM2d *GoalWG;
  XSSM2d *CurrGoalWG;
  XSSM2d *GoalPhFL;
  XSSM2d *GoalPh;
  XSSM2d *CurrGoalPh;
  XSSM2d *GetGoalPhFL;

  XSSM2d *WkIfGoalWGW1;
  XSSM2d *WkIfGoalWGW2;
  XSSM2d *WkIfGoalWGW3;
  XSSM2d *WkIfGoalWGW4;
  XSSM *WkEqGoalWGW1;
  XSSM *WkEqGoalWGW2;
  XSSM *WkEqGoalWGW3;
  XSSM *WkEqGoalWGW4;
  ssm *WkEqGoalWGW1C;
  ssm *WkEqGoalWGW2C;
  ssm *WkEqGoalWGW3C;
  ssm *WkEqGoalWGW4C;

  XSSM2d *GoalPhIfGoalWGW1;
  XSSM2d *GoalPhIfGoalWGW2;
  XSSM2d *GoalPhIfGoalWGW3;
  XSSM2d *GoalPhIfGoalWGW4;
  XSSM *GoalPhEqGoalWGW1;
  XSSM *GoalPhEqGoalWGW2;
  XSSM *GoalPhEqGoalWGW3;
  XSSM *GoalPhEqGoalWGW4;
  ssm *GoalPhEqGoalWGW1C;
  ssm *GoalPhEqGoalWGW2C;
  ssm *GoalPhEqGoalWGW3C;
  ssm *GoalPhEqGoalWGW4C;

  ssm *GoalI;
  ssm *CurrGoalI;
  ssm *NextGoalI;
  ssm *PrevGoalI;
  ssm *StartGoalIFlag;
  ssm *CurrGoalIFlag;
  ssm *NextGoalIFlag;
  ssm *PrevGoalIFlag;

  ssm *RetrGoal;
  ssm *BuildGoal;
  ssm_as *GoalMem;
  //

  sllm();
  int AddRef();
  int In(int *w, int *phi);
  int PhUpdate();
  int AsUpdate();
  int StActMemUpdate();
  int ActUpdate();
  int ElActfStUpdate();
  int MemPhUpdate();
  int Update();
  int StActRwdUpdate();
  int AcquireArch();
  int AcquireUpdate();
  int AssociateArch();
  int AssociateUpdate();
  int RewardArch();
  int RewardUpdate();
  int ExploitArch();
  int ExploitUpdate();
  int ExploreArch();
  int ExploreUpdate();
  int GoalArch();
  int AddGoalRef();
  int GoalUpdate();

};

#endif
