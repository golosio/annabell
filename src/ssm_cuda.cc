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
#include <stdlib.h>
#include "ssm.h"
#include "gettime.h"

using namespace std;

double host_time;

void cuda_CopyInputLinks_fn(int Nssm, int *Nnr, int **Nx, int ***lk_nr,
			    int ***lk_nr2, int *** &h_h_lk_nr, int* &dev_Nnr,
			    int** &dev_Nlk, int*** &dev_lk_nr, int NN,
			    float* &dev_activ_arr);
int cuda_SparseActiv_fn(int NRows, int **LkPt, int *Nlk,
			float *in_sign_arr, int NN, float *activ_arr,
			int *dev_Nnr, int ***dev_lk_nr, float *dev_activ_arr);

int ssm_as::cuda_CopyInpuLinks()
{
  int Nssm=SparseInSSM.size();

  int *Nnr;
  int **Nlk;
  int ***lk_nr;
  int ***lk_nr2;

  host_time = 0;
  activ_arr = new float[NN()];
  Nnr = (int*)malloc(Nssm*sizeof(int));
  Nlk = (int**)malloc(Nssm*sizeof(int*));
  lk_nr = (int***)malloc(Nssm*sizeof(int**));
  lk_nr2 = (int***)malloc(Nssm*sizeof(int**));

  for (int issm=0; issm<Nssm; issm++) {
    vssm *ssm1=SparseInSSM[issm];
    Nnr[issm] = ssm1->NN();
    Nlk[issm] = (int*)malloc(Nnr[issm]*sizeof(int));
    lk_nr[issm] = (int**)malloc(Nnr[issm]*sizeof(int*));
    lk_nr2[issm] = (int**)malloc(Nnr[issm]*sizeof(int*));
    for (int inr=0; inr<Nnr[issm]; inr++) {
      vector<int> *lk_set = &InputLkSet[issm][inr];
      Nlk[issm][inr] = lk_set->size();
      lk_nr[issm][inr] = (int*)malloc(Nlk[issm][inr]*sizeof(int));
      lk_nr2[issm][inr] = (int*)malloc(Nlk[issm][inr]*sizeof(int));
      for (int ilk=0; ilk<Nlk[issm][inr]; ilk++) {
	lk_nr[issm][inr][ilk] = lk_set->at(ilk);
      }
    }
  } 

  cuda_CopyInputLinks_fn(Nssm, Nnr, Nlk, lk_nr, lk_nr2, h_h_lk_nr, dev_Nnr, dev_Nlk,
			 dev_lk_nr, NN(), dev_activ_arr);
  /*
  for (int issm=0; issm<Nssm; issm++) {
    for (int inr=0; inr<Nnr[issm]; inr++) {
      for (int ilk=0; ilk<Nlk[issm][inr]; ilk++) {
	cout << issm << " " << inr << " " << ilk << " " 
	     << lk_nr[issm][inr][ilk] << " " << lk_nr2[issm][inr][ilk] << endl;
      }
    }
  } 
  */

  for (int issm=0; issm<Nssm; issm++) {
    for (int inr=0; inr<Nnr[issm]; inr++) {
      free(lk_nr[issm][inr]);
      free(lk_nr2[issm][inr]);
    }
    free (Nlk[issm]);
    free(lk_nr[issm]);
    free(lk_nr2[issm]);
  } 


  free(Nnr);
  free(Nlk);
  free(lk_nr);
  free(lk_nr2);

  return 0;
}

int ssm_as::cuda_SparseActiv()
{
  float DefaultMinWg = -1; // put in header
  int NRows, NCols = 512;
  struct timespec clk0, clk1;
  struct timespec clkh0, clkh1;

  GetMonotonicTime(&clkh0);

  //int n_high=SparseNHighIn();
  float min_in_sign = SparseMinInSign();

  for(int i=0; i<NN(); i++) {
    activ_arr[i] = Nr[i]->B + GB + min_in_sign; // first assume ...
    // that all neurons are unconnected.
    // Unconnected neurons have default (negative) weight
  }
  //cout << "ok0\n";
  //cout << activ_arr[10] << endl;
  //cout << activ_arr[195] << endl;
  //cout << NN() << endl;

  Forced=false;
  GetMonotonicTime(&clk0);
  NRows = 0;
  for (unsigned int issm=0; issm<SparseInSSM.size(); issm++) {
    vssm *ssm1=SparseInSSM[issm];
    for(unsigned int i=0; i<ssm1->HighVect.size(); i++) {
      int inr = ssm1->HighVect[i];
      vector<int> *lk_set = &InputLkSet[issm][inr];
      int nlk = lk_set->size();
      NRows += 1 + nlk/NCols;
    }
  }
  //cout << "NRows: " << NRows << endl;

  int **LkPt = new int*[NRows];
  int *Nlk = new int[NRows];
  float *in_sign_arr = new float[NRows];

  int index=0;
  for (unsigned int issm=0; issm<SparseInSSM.size(); issm++) {
    float wgmin = SparseInMinWeight[issm];
    float wgmax = SparseInMaxWeight[issm];
    vssm *ssm1=SparseInSSM[issm];
    for(unsigned int i=0; i<ssm1->HighVect.size(); i++) {
      int inr = ssm1->HighVect[i];
      float in_sign = ssm1->Nr[inr]->O;
      vector<int> *lk_set = &InputLkSet[issm][inr];
      int nlk = lk_set->size();
      int *lk_pt = h_h_lk_nr[issm][inr];
      while (nlk>=NCols) {
	LkPt[index] = lk_pt;
	Nlk[index] = NCols;
	in_sign_arr[index] = in_sign*(wgmax-wgmin);
	lk_pt += NCols;
	nlk -= NCols;
	index++;
      }
      LkPt[index] = lk_pt;
      Nlk[index] = nlk;
      in_sign_arr[index] = in_sign*(wgmax-wgmin);
      index++;
    }
  }
  //cout << "ok1\n";
  cuda_SparseActiv_fn(NRows, LkPt, Nlk, in_sign_arr, NN(),
		      activ_arr, dev_Nnr, dev_lk_nr, dev_activ_arr);
  //cout << "ok2\n";
  for (int inr=0; inr<NN(); inr++) {
    Nr[inr]->A = activ_arr[inr];
  }
  //cout << "NewWnnNum " << NewWnnNum << endl;
  delete[] LkPt;
  delete[] Nlk;
  delete[] in_sign_arr;

  GetMonotonicTime(&clk1);
  GetMonotonicTime(&clkh1);

  act_time = act_time
    + clk1.tv_sec - clk0.tv_sec + (double)(clk1.tv_nsec - clk0.tv_nsec)*1e-9;
  host_time = host_time
    + clkh1.tv_sec - clkh0.tv_sec + (double)(clkh1.tv_nsec - clkh0.tv_nsec)*1e-9;


  //cout << "host time: " << host_time << endl;

  return 0;
}
