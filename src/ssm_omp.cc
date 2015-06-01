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
#include "ssm.h"
#include <omp.h>
using namespace std;

#define THREAD_MAXNUM omp_get_max_threads()
#define THREAD_IDX omp_get_thread_num()

int ssm_as::OMPActiv()
{
  struct timespec clk0, clk1;
  int N;
  if (WTA_FLAG==T_NEW_WTA || WTA_FLAG==T_NEW_KWTA) N = NewWnnNum;
  else N = NN();

  clock_gettime( CLOCK_REALTIME, &clk0);

  Forced=false;
  int i;
#pragma omp parallel for default(shared) private(i) collapse(1)
  for(i=0; i<N; i++) {
    Nr[i]->Activ();
    Nr[i]->A += GB;
  }

  if (SparseLkFlag) { // unconnected neurons have negative weights
    int n_high=SparseNHighIn();
#pragma omp parallel for default(shared) private(i) collapse(1)
    for(i=0; i<N; i++) {
      Nr[i]->A -= (n_high - Nr[i]->NHighIn());
    }
  }

  clock_gettime( CLOCK_REALTIME, &clk1);
  act_time = act_time
    + clk1.tv_sec - clk0.tv_sec + (double)(clk1.tv_nsec - clk0.tv_nsec)*1e-9;

  return 0;
}

int ssm_as::OMPActivArrInit()
{
  OMPActivArr = new float*[THREAD_MAXNUM];
  for (int i=0; i<THREAD_MAXNUM; i++) {
    OMPActivArr[i] = new float[NN()];
  }
  cout << "Threads max number: " << THREAD_MAXNUM << endl;

  return 0;
}

int ssm_as::OMPSparseActiv()
{
  //float DefaultMinWg = -1; // put in header
  struct timespec clk0, clk1;
  int N;
  if (WTA_FLAG==T_NEW_WTA || WTA_FLAG==T_NEW_KWTA) N = NewWnnNum;
  else N = NN();

  //int n_high=SparseNHighIn();
  float min_in_sign = SparseMinInSign();

#pragma omp parallel for default(shared) collapse(1)
  for(int i=0; i<N; i++) {
    Nr[i]->A = Nr[i]->B + GB + min_in_sign; // first assume ...
    // that all neurons are unconnected.
    // Unconnected neurons have default (negative) weight
    for (int ith=0; ith<THREAD_MAXNUM; ith++) {
      OMPActivArr[ith][i] = 0;
    }
  }
  //for(int i=N; i<NN(); i++) {
  //  Nr[i]->A = Nr[i]->B + GB - n_high;
  //}
  Forced=false;
  clock_gettime( CLOCK_MONOTONIC, &clk0);
#pragma omp parallel for default(shared) collapse(1)
  for (unsigned int issm=0; issm<SparseInSSM.size(); issm++) {
    float wgmin = SparseInMinWeight[issm];
    vssm *ssm1=SparseInSSM[issm];
    for(unsigned int i=0; i<ssm1->HighVect.size(); i++) {
      int inr = ssm1->HighVect[i];
      float in_sign = ssm1->Nr[inr]->O;
      vector<int> *lk_set = &InputLkSet[issm][inr];
      vector<float>::iterator lk_wg_it = InputLkWg[issm][inr].begin();
      for(vector<int>::iterator lk_set_it = lk_set->begin();
	  lk_set_it != lk_set->end(); lk_set_it++) {
	int inr1 = *lk_set_it;
	float wg = *lk_wg_it;
	OMPActivArr[THREAD_IDX][inr1] += in_sign*(wg-wgmin);
	//float f=in_sign*(wg-DefaultMinWg);
	//if (f!=2) {cout << "here " << f << endl; exit(0); }
	// the second term compensates for the original
	// assumption that neurons are unconnected
	// and have default (negative) weights
	lk_wg_it++;
      }
      ////SparseInSSMOut[issm]->OutNr[inr]->DoubleAddActiv();
      //io_nr* nr1 = SparseInSSMOut[issm]->OutNr[inr];
      //for (int i=0; i<nr1->OutNL(); i++) {
      //#pragma omp atomic
      //nr1->OutL[i].Nr->A += nr1->OutL[i].Wg*2;
      //}
    }
  }
  clock_gettime( CLOCK_MONOTONIC, &clk1);

#pragma omp parallel for default(shared) collapse(1)
  for(int i=0; i<N; i++) {
    for (int ith=0; ith<THREAD_MAXNUM; ith++) {
      Nr[i]->A += OMPActivArr[ith][i];
    }
  }

  act_time = act_time
    + clk1.tv_sec - clk0.tv_sec + (double)(clk1.tv_nsec - clk0.tv_nsec)*1e-9;

  return 0;
}
