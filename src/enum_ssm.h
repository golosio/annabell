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

enum {
  Header_idx,
  IW_idx,CW_idx,PhI_idx,InFlag_idx,FlushIn_idx,InI_idx,InEqW_idx,InEqWC_idx,
  InPhFL_idx,InPhB_idx,InIfW_idx,
  InIfWGW1_idx,InIfWGW2_idx,InIfWGW3_idx,InIfWGW4_idx,
  InEqWGW1_idx,InEqWGW2_idx,InEqWGW3_idx,InEqWGW4_idx,
  InEqWGW1C_idx,InEqWGW2C_idx,InEqWGW3C_idx,InEqWGW4C_idx,
  PrevInEqWGW1FL_idx,PrevInEqWGW2FL_idx,PrevInEqWGW3FL_idx,PrevInEqWGW4FL_idx,
  PrevInEqWGW1_idx,PrevInEqWGW2_idx,PrevInEqWGW3_idx,PrevInEqWGW4_idx,
  PrevInEqWGW1C_idx,PrevInEqWGW2C_idx,PrevInEqWGW3C_idx,PrevInEqWGW4C_idx,
  
  WkFlag_idx,FlushWk_idx,WkEqW_idx,WkEqWC_idx,
  WkPhFL_idx,WkPhB_idx,WkIfW_idx,WkWfI_idx,
  WkIfWGW1_idx,WkIfWGW2_idx,WkIfWGW3_idx,WkIfWGW4_idx,
  WkEqWGW1_idx,WkEqWGW2_idx,WkEqWGW3_idx,WkEqWGW4_idx,
  WkEqWGW1C_idx,WkEqWGW2C_idx,WkEqWGW3C_idx,WkEqWGW4C_idx,
  PrevWkPhFL_idx,PrevWkPh_idx,
  PrevWkEqWGW1FL_idx,PrevWkEqWGW2FL_idx,PrevWkEqWGW3FL_idx,PrevWkEqWGW4FL_idx,
  PrevWkEqWGW1_idx,PrevWkEqWGW2_idx,PrevWkEqWGW3_idx,PrevWkEqWGW4_idx,
  PrevWkEqWGW1C_idx,PrevWkEqWGW2C_idx,PrevWkEqWGW3C_idx,PrevWkEqWGW4C_idx,
  
  OutFlag_idx,FlushOut_idx,OutEqW_idx,OutPhFL_idx,OutPhB_idx,OutIfW_idx,
  
  GetFlag_idx,WGI_idx,WGCW_idx,WGFlag_idx,FlushWG_idx,
  WGIFL_idx,WGEqW_idx,WGEqWC_idx,WGFL_idx,WGB_idx,WGIfW_idx,
  PrevWGFL_idx,PrevWG_idx,
  
  RetrAs_idx,BuildAs_idx,
  
  StartStActIFlag_idx,CurrStActI_idx,CurrStActIFlag_idx,
  NextStActI_idx,NextStActIFlag_idx,StActI_idx,
  RetrStAct_idx,BuildStAct_idx,StActMem_idx,
  
  ElAct_idx,AcqAct_idx,ActFlags_idx,ActFlagsC_idx,
  
  CurrPhI_idx,CurrPhIFlag_idx,NextPhI_idx,NextPhIFlag_idx,
  CurrWGI_idx,CurrWGIFlag_idx,NextWGI_idx,NextWGIFlag_idx,
  
  RetrElActfSt_idx,BuildElActfSt_idx,RewdElActfSt_idx,ElActfSt_idx,
  
  CurrMemPh_idx,NextMemPh_idx,NewMemPhFlag_idx,MemPh_idx,
  RetrRemPh_idx,RemPhIL_idx,CurrRemPh_idx,CurrRemPhFlag_idx,
  NextRemPh_idx,NextRemPhFlag_idx,RemPh_idx,RemPhfWG_idx,
  StartPhFlag_idx,StartPhIL_idx,StartPh_idx,CurrStartPh_idx,
  RemStartPhFlag_idx,CurrStartWkPhFlag_idx,StartWkPhIL_idx,
  CurrStartWkPh_idx,StartWkPh_idx,RemStartPh_idx,
  
  ModeFlags_idx,Acquire_idx,Associate_idx,Explore_idx,Explore2_idx,
  Exploit_idx,BestExploit_idx,Reward_idx,

  SetGoalFlag_idx, GetGoalFlag_idx, FlushGoal_idx,
  GoalWGFL_idx, GoalWG_idx, CurrGoalWG_idx,
  GoalPhFL_idx, GoalPh_idx, CurrGoalPh_idx, GetGoalPhFL_idx,

  WkIfGoalWGW1_idx, WkIfGoalWGW2_idx, WkIfGoalWGW3_idx, WkIfGoalWGW4_idx,
  WkEqGoalWGW1_idx, WkEqGoalWGW2_idx, WkEqGoalWGW3_idx, WkEqGoalWGW4_idx,
  WkEqGoalWGW1C_idx, WkEqGoalWGW2C_idx, WkEqGoalWGW3C_idx, WkEqGoalWGW4C_idx,

  GoalI_idx, CurrGoalI_idx, NextGoalI_idx, PrevGoalI_idx,
  StartGoalIFlag_idx, CurrGoalIFlag_idx, NextGoalIFlag_idx, PrevGoalIFlag_idx,

  RetrGoal_idx, BuildGoal_idx, GoalMem_idx

};
