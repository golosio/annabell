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
#include <algorithm>
#include <cmath>
#include <stdlib.h>
#include "ssm.h"
#include "rnd.h"

using namespace std;

const float nr::EPS=0.5;

int nr::NL()
{
  return L.size();
}

int nr::Activ()
{
  A=B;
  for (int i=0; i<NL(); i++) {
    A += L[i].Wg*L[i].Nr->O;
  }

  return 0;
}

float logistic[]={
  0.0066929, 0.0073915, 0.0081626, 0.0090133, 0.0099518,
  0.010987, 0.012128, 0.013387, 0.014774, 0.016302,
  0.017986, 0.01984, 0.021881, 0.024127, 0.026597,
  0.029312, 0.032295, 0.035571, 0.039166, 0.043107,
  0.047426, 0.052154, 0.057324, 0.062973, 0.069138,
  0.075858, 0.083173, 0.091123, 0.09975, 0.1091,
  0.1192, 0.13011, 0.14185, 0.15447, 0.16798,
  0.18243, 0.19782, 0.21417, 0.23148, 0.24974,
  0.26894, 0.28905, 0.31003, 0.33181, 0.35434,
  0.37754, 0.40131, 0.42556, 0.45017, 0.47502,
  0.5, 0.52498, 0.54983, 0.57444, 0.59869,
  0.62246, 0.64566, 0.66819, 0.68997, 0.71095,
  0.73106, 0.75026, 0.76852, 0.78583, 0.80218,
  0.81757, 0.83202, 0.84553, 0.85815, 0.86989,
  0.8808, 0.8909, 0.90025, 0.90888, 0.91683,
  0.92414, 0.93086, 0.93703, 0.94268, 0.94785,
  0.95257, 0.95689, 0.96083, 0.96443, 0.9677,
  0.97069, 0.9734, 0.97587, 0.97812, 0.98016,
  0.98201, 0.9837, 0.98523, 0.98661, 0.98787,
  0.98901, 0.99005, 0.99099, 0.99184, 0.99261,
  0.99331};

int nr::Out()
{
  //const float lim=5;
  #define lim 4.9

  switch(ActivFunc) {
  case HEAVISIDE:
    if (A<=0) O=0;
    else O=1;
    return 0;
  case LOGISTIC:
    if (A<=-lim) O=0;
    else if (A>=lim) O=1;
    else O=logistic[(int)(A*10+50)]; //1./(1.+exp(-A));
    return 0;
  default:
    if (A<=0) O=0;
    else O=1;
  }
  

  return 0;
}


int nr::NHighIn()
{
  int n_high=0;
  for (int i=0; i<NL(); i++) {
    if (L[i].Nr->O > 0.5) n_high++;
  }
  
  return n_high;
}

int nr::CN(nr *nr1, float wg)
{
  lk l;

  l.Wg = wg;
  l.Nr = nr1;
  L.push_back(l);

  return 0;
}

int nr::CN(nr *nr1, float wgmin, float wgmax)
{
  lk l;

  l.Wg = wgmin+rnd()*(wgmax-wgmin);
  l.Nr = nr1;
  L.push_back(l);

  return 0;
}

int nr::CN(vssm *ssm1, float wg)
{
  for(int i=0; i<ssm1->NN(); i++) {
    CN(ssm1->Nr[i], wg);
  }

  return 0;
}

int nr::CN(vssm *ssm1, float wgmin, float wgmax)
{
  for(int i=0; i<ssm1->NN(); i++) {
    CN(ssm1->Nr[i], wgmin, wgmax);
  }
    
  return 0;
}

int nr::SB(float b)
{
  B=b;

  return 0;
}

int nr::Hbbn(float HWpar)
{
  float DW;

  for (int i=0; i<NL(); i++) {
    if ((O-0.5)*(L[i].Nr->O-0.5) > 0) DW=HWpar*(1.-L[i].Wg);
    else DW=HWpar*(-1.-L[i].Wg);
    L[i].Wg += DW;
  }

  return 0;
}

int nr::AdjustB(float HBpar)
{
  Activ();
  if (O>0.5) B += HBpar*(-A+EPS);
  else if (O<=0.5) B += HBpar*(-A-EPS);

  return 0;
}

int nr::HbbnB(float HWpar, float HBpar)
{
  Hbbn(HWpar);
  AdjustB(HBpar);

  return 0;
}

int vssm::NN()
{
  return Nr.size();
}

int vssm::UseDefault()
{
  Default = new ssm(1);
  
  return 0;
}
 
int vssm::SetSparseIn()
{
  SparseLkFlag = true;

  return 0;
}


int vssm::Fill(int *vin)
{
  for(int i=0; i<NN(); i++) {
    Nr[i]->A = 2.*vin[i] - 1;
  }
  Out();

  return 0;
}

int vssm::Fill(float *vin)
{
  for(int i=0; i<NN(); i++) {
    Nr[i]->A = 2.*vin[i] - 1;
  }
  Out();

  return 0;
}

bool vssm::SparseNullIn()
{
  for (unsigned int issm=0; issm<SparseInSSM.size(); issm++) {
    if (CheckNullVect[issm] && SparseInSSM[issm]->NHigh > 0) return false;
  }
  
  return true;
}

int vssm::SparseNHighIn()
{
  int n_high=0;
  for (unsigned int issm=0; issm<SparseInSSM.size(); issm++) {
    n_high += SparseInSSM[issm]->NHigh;
  }
  
  return n_high;
}

/*
float vssm::SparseTotInSign()
{
  float tot_in_sign = 0;

  for (unsigned int issm=0; issm<SparseInSSM.size(); issm++) {
    vssm *ssm1=SparseInSSM[issm];
    for(unsigned int i=0; i<ssm1->HighVect.size(); i++) {
      int inr = ssm1->HighVect[i];
      float in_sign = ssm1->Nr[inr]->O;
      tot_in_sign += in_sign;
    }
  }
  
  return tot_in_sign;
}
*/

float vssm::SparseMinInSign()
{
  float min_in_sign = 0;

  for (unsigned int issm=0; issm<SparseInSSM.size(); issm++) {
    vssm *ssm1 = SparseInSSM[issm];
    float wg = SparseInMinWeight[issm];
    for(unsigned int i=0; i<ssm1->HighVect.size(); i++) {
      int inr = ssm1->HighVect[i];
      float in_sign = ssm1->Nr[inr]->O;
      min_in_sign += wg*in_sign;
    }
  }
  
  return min_in_sign;
}

int vssm::FC(vssm *ssm1, float wg)
{
  if (SparseLkFlag) {
    cout << "Error. SSM cannot have both sparse and ordinary input links\n";
    exit(0);
  }

  for(int i=0; i<NN(); i++) {
    Nr[i]->CN(ssm1, wg);
  }

  return 0;
}

int vssm::FC(vssm *ssm1, float wgmin, float wgmax)
{
  if (SparseLkFlag) {
    cout << "Error. SSM cannot have both sparse and ordinary input links\n";
    exit(0);
  }

  for(int i=0; i<NN(); i++) {
    Nr[i]->CN(ssm1, wgmin, wgmax);
  }

  return 0;
}

int vssm::SparseFC(vssm *ssm1)
{
  SparseFC(ssm1, -1, 1, true);

  return 0;
}

int vssm::SparseFC(vssm *ssm1, bool check_null_flag)
{
  SparseFC(ssm1, -1, 1, check_null_flag);

  return 0;
}

int vssm::SparseFC(vssm *ssm1, float wgmin, float wgmax)
{
  SparseFC(ssm1, wgmin, wgmax, true);

  return 0;
}

int vssm::SparseFC(vssm *ssm1, float wgmin, float wgmax, bool check_null_flag)
{
  if (!SparseLkFlag) {
    cout << "Error. SSM cannot have both ordinary and sparse input links\n";
    exit(0);
  }
  CheckNullVect.push_back(check_null_flag);
  ssm1->FillHighVect=true;
  SparseInSSM.push_back(ssm1);
  SparseInMinWeight.push_back(wgmin);
  SparseInMaxWeight.push_back(wgmax);
  vector<vector<int> > ssm1_lk_set;
  vector<vector<float> > ssm1_lk_wg;
  for (int inr=0; inr<ssm1->NN(); inr++) {
    vector<int> nr_lk_set;
    vector<float> nr_lk_wg;
    ssm1_lk_set.push_back(nr_lk_set);
    ssm1_lk_wg.push_back(nr_lk_wg);
  }
  InputLkSet.push_back(ssm1_lk_set);
  InputLkWg.push_back(ssm1_lk_wg);

  return 0;
}

int vssm::SC(vssm *ssm1, float wg)
{
  if (SparseLkFlag) {
    cout << "Error. SSM cannot have both sparse and ordinary input links\n";
    exit(0);
  }
  if (ssm1->NN() != NN()) {
    cout << "NN must match in SC!\n";
    exit(0);
  }
  for(int i=0; i<NN(); i++) {
    Nr[i]->CN(ssm1->Nr[i], wg);
  }

  return 0;
}

int vssm::SC(vssm *ssm1, float wgmin, float wgmax)
{
  if (SparseLkFlag) {
    cout << "Error. SSM cannot have both sparse and ordinary input links\n";
    exit(0);
  }
  if (ssm1->NN() != NN()) {
    cout << "NN must match in SC!\n";
    exit(0);
  }
  for(int i=0; i<NN(); i++) {
    Nr[i]->CN(ssm1->Nr[i], wgmin, wgmax);
  }

  return 0;
}

int vssm::SCtoRows(vssm2d *ssm1, float wg)
{
  if (SparseLkFlag) {
    cout << "Error. SSM cannot have both sparse and ordinary input links\n";
    exit(0);
  }
  if (ssm1->Nx() != NN()) {
    cout << "Nx and NN must match in SCtoRows!\n";
    exit(0);
  }
  for (int iy=0; iy<ssm1->Ny(); iy++) {
    SC(ssm1->Row[iy], wg);
  }

  return 0;
}

int vssm::SCtoRows(vssm2d *ssm1, float wgmin, float wgmax)
{
  if (SparseLkFlag) {
    cout << "Error. SSM cannot have both sparse and ordinary input links\n";
    exit(0);
  }
  if (ssm1->Nx() != NN()) {
    cout << "Nx and NN must match in SCtoRows!\n";
    exit(0);
  }
  for (int iy=0; iy<ssm1->Ny(); iy++) {
    SC(ssm1->Row[iy], wgmin, wgmax);
  }

  return 0;
}

int vssm::SCtoColumns(vssm2d *ssm1, float wg)
{
  if (SparseLkFlag) {
    cout << "Error. SSM cannot have both sparse and ordinary input links\n";
    exit(0);
  }
  if (ssm1->Ny() != NN()) {
    cout << "Ny and NN must match in SCtoColumns!\n";
    exit(0);
  }
  for (int ix=0; ix<ssm1->Nx(); ix++) {
    SC(ssm1->Column[ix], wg);
  }
  
  return 0;
}

int vssm::SCtoColumns(vssm2d *ssm1, float wgmin, float wgmax)
{
  if (SparseLkFlag) {
    cout << "Error. SSM cannot have both sparse and ordinary input links\n";
    exit(0);
  }
  if (ssm1->Ny() != NN()) {
    cout << "Ny and NN must match in SCtoColumns!\n";
    exit(0);
  }
  for (int ix=0; ix<ssm1->Nx(); ix++) {
    SC(ssm1->Column[ix], wgmin, wgmax);
  }

  return 0;
}

int vssm::SB(float b)
{
  for(int i=0; i<NN(); i++) {
    Nr[i]->SB(b);
  }

  return 0;
}

ssm::ssm()
{
  Init();
}

ssm::ssm(int size)
{
  Init(size);
}

int vssm::Init()
{
  SparseLkFlag=false;
  GB=0;
  Default=NULL;
  HWpar=1;
  HBpar=1;
  FillHighVect=false;
  NewWnnNum=0;
  Forced=false;
  act_time = out_time = 0;
  OrderedWnnFlag=true;
  LastWnn = 0;
  ActivFunc=HEAVISIDE;

  return 0;
}

int vssm::Init(int size)
{
  Init();
  for(int i=0; i<size; i++) {
    nr *nr1=new nr(ActivFunc);
    Nr.push_back(nr1);
  }
  SB(0);

  return 0;
}

int ssm::SparseActiv()
{
  //float DefaultMinWg = -1; // put in header
  struct timespec clk0, clk1;

  //clock_gettime( CLOCK_MONOTONIC, &clk0);

  //int n_high=SparseNHighIn();
  float min_in_sign = SparseMinInSign();
  for(int i=0; i<NN(); i++) {
    Nr[i]->A = Nr[i]->B + GB + min_in_sign; // first assume ...
    // that all neurons are unconnected.
    // Unconnected neurons have default (negative) weight
  }

  Forced=false;
  clock_gettime( CLOCK_MONOTONIC, &clk0);
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
	Nr[inr1]->A += in_sign*(wg-wgmin);
	// the second term compensates for the original
	// assumption that neurons are unconnected
	// and have default (negative) weights
	lk_wg_it++;
      }
    }
  }
  clock_gettime( CLOCK_MONOTONIC, &clk1);

  act_time = act_time
    + clk1.tv_sec - clk0.tv_sec + (double)(clk1.tv_nsec - clk0.tv_nsec)*1e-9;

  return 0;
}

int ssm::Activ()
{
  if (SparseLkFlag) {
    SparseActiv();
    return 0;
  }

  struct timespec clk0, clk1;
  clock_gettime( CLOCK_MONOTONIC, &clk0);
  Forced=false;
  for(int i=0; i<NN(); i++) {
    Nr[i]->Activ();
    Nr[i]->A += GB;
  }

  clock_gettime( CLOCK_MONOTONIC, &clk1);
  act_time = act_time
    + clk1.tv_sec - clk0.tv_sec + (double)(clk1.tv_nsec - clk0.tv_nsec)*1e-9;

  return 0;
}

int ssm::Out()
{
  struct timespec clk0, clk1;
  int N;
  
  clock_gettime( CLOCK_MONOTONIC, &clk0);
  NHigh = 0;
  if (FillHighVect) HighVect.clear();

  if (NewWnnNum!=0) N = NewWnnNum;
  else N = NN();
  for(int i=0; i<N; i++) {
    Nr[i]->Out();
    if (Nr[i]->O > 0.5) {
      NHigh++;
      if (FillHighVect) HighVect.push_back(i);
    }
  }
  SetDefault();

  clock_gettime( CLOCK_MONOTONIC, &clk0);
  out_time = out_time
    + clk1.tv_sec - clk0.tv_sec + (double)(clk1.tv_nsec - clk0.tv_nsec)*1e-9;
  return 0;
}

int vssm::SetDefault()
{
  if (Default!=NULL) {
    if (Default->FillHighVect) Default->HighVect.clear();
    if (NHigh > 0) {
      Default->Nr[0]->O = 0;
      Default->NHigh = 0;
    }
    else {
      if (Default->FillHighVect) Default->HighVect.push_back(0);
      Default->Nr[0]->O = 1;
      Default->NHigh = 1;
    }
  }

  return 0;
}

int ssm::ActivOut()
{
  Activ();
  Out();

  return 0;
}

int ssm::LowActiv()
{
  Forced=false;
  for(int i=0; i<NN(); i++) {
    Nr[i]->A = -1;
  }
  
  return 0;
}

int ssm::SetActiv(int inr, float aval)
{
  Nr[inr]->A = aval;

  return 0;
}

int ssm::AddActiv(int inr, float aval)
{
  Nr[inr]->A += aval;

  return 0;
}

bool ssm::NullIn()
{
  if (SparseLkFlag) return SparseNullIn();
  for (int in=0; in<NN(); in++) {
    for(int il=0; il<Nr[in]->NL(); il++) {
      if (Nr[in]->L[il].Nr->O>0.5) {
	//cout << in << " " << il << " " << NN() << endl;
	return false;
      }
    }
  }

  return true;
}

int ssm::Clear()
{
  NHigh = 0;
  if (FillHighVect) HighVect.clear();
  for(int i=0; i<NN(); i++) Nr[i]->O=0;

  return 0;
}

int ssm::Wnn()
{
  float amax = 0;
  int imax = -1;
  float R=0;
  
  Clear();
  if (!NullIn()) {
    for(int i=0; i<NN(); i++) {
      if (Nr[i]->Used==1) {
	Nr[i]->Out();
	if (Nr[i]->O>0.5) {
	  amax = Nr[i]->A;
	  imax = i;
	  break;
	}
      }
      else {
	if (!SparseLkFlag) R = Nr[i]->A;
	else R = rnd();
	if (imax<0 || R>amax) {
	  amax = R;
	  imax = i;
	}    
      }
    }

    if (imax>=0) {
      Nr[imax]->O = 1;
      Nr[imax]->Used = 1;
      NHigh = 1;
      if (FillHighVect) HighVect.push_back(imax);
    }
  }
  SetDefault();

  return imax;
}

int ssm::NumActivWnn(float &amax)
{
  int n_wnn = 0;
  
  for(int i=0; i<NN(); i++) {
    float act = Nr[i]->A;
    if (i==0 || act>amax) {
      amax = act;
      n_wnn = 1;
    }
    else if (act==amax) {
      n_wnn++;
    }
  }

  return n_wnn;
}

int ssm::RndActivWnn()
{
  float amax = 0;
  int imax = -1;
  
  Clear();

  int n_wnn=NumActivWnn(amax);
  int k_wnn=rnd_int()%n_wnn;
  int k=0;
  //if (!NullIn()) {
  for(int i=0; i<NN(); i++) {
    float act = Nr[i]->A;
    if (act==amax) {
      if (k==k_wnn) {
	imax=i;
	break;
      }
      k++;
    }
  }
  //}

  if (imax>=0) {
    Nr[imax]->O = 1;
    NHigh = 1;
    if (FillHighVect) HighVect.push_back(imax);
  }
  SetDefault();

  return imax;
}

int ssm::ActivWnn()
{
  float amax = 0;
  int imax = -1;
  
  Clear();
  //if (!NullIn()) {
  for(int i=0; i<NN(); i++) {
    float act = Nr[i]->A;
    if (i==0 || act>amax) {
      amax = act;
      imax = i;
    }
  }
  //}
  if (imax>=0) {
    Nr[imax]->O = 1;
    NHigh = 1;
    if (FillHighVect) HighVect.push_back(imax);
  }
  SetDefault();

  return imax;
}

int ssm::NewWnn()
{
  int imax = -1;
  
  Clear();
  //cout << "ni: " << NullIn() << endl; 
  if (!NullIn()) {
    imax = NewWnnNum;
    NewWnnNum++;
    if (NewWnnNum>NN()) {
      cout << "Error: NewWnn overflow\n";
      exit(0);
    }
    Nr[imax]->O = 1;
    Nr[imax]->Used = 1;
    NHigh = 1;
    if (FillHighVect) HighVect.push_back(imax);
  }
  SetDefault();

  return imax;
}

int ssm::NumWnn()
{
  int n_wnn=0;
  
  Out(); // try to remove
  for(int i=0; i<NN(); i++) {
    if (Nr[i]->Used==1) {
      if (Nr[i]->O>0.5) n_wnn++;
    }
  }
  
  return n_wnn;
}

int ssm::NextWnn()
{
  int iw=-1;
  Out(); // try to remove
  for (int j=0; j<NN(); j++) {
    int i = (j + LastWnn + 1)%NN();
    if (Nr[i]->Used==1 && Nr[i]->O>0.5) {
      iw = i;
      LastWnn = iw;
      break;
    }
  }
  
  return iw;
}

int ssm::RndWnn() //semplificare usando rnd()
{
  int iw=-1;
  if (OrderedWnnFlag) iw = NextWnn();
  else {
    int n_wnn=NumWnn();
    //cout << "n_wnn old: " << n_wnn << endl;
    //if (NN()<50) {
    //  for(int i=0; i<NN(); i++) {
    //    cout << Nr[i]->A << endl;
    //  }
    //}
    
    if (n_wnn==0) return -1;
    int k_wnn=rnd_int()%n_wnn;
    int k=0;
    for(int i=0; i<NN(); i++) {
      if (Nr[i]->Used==1 && Nr[i]->O>0.5) {
	if (k==k_wnn) {
	  iw=i;
	  break;
	}
	k++;
      }
    }
  }
  Clear();
  if (iw>=0) {
    Nr[iw]->O = 1;
    NHigh = 1;
    if (FillHighVect) HighVect.push_back(iw);
  }
  SetDefault();

  return iw;
}

int vssm2d::Init(int sizex, int sizey)
{
  RowDefault=NULL;
  ColumnDefault=NULL;

  for(int iy=0; iy<sizey; iy++) {
    ssm *ssm1=new ssm;
    Row.push_back(ssm1);
  }
  for(int ix=0; ix<sizex; ix++) {
    ssm *ssm1=new ssm;
    Column.push_back(ssm1);
  }

  for(int iy=0; iy<sizey; iy++) {
    for(int ix=0; ix<sizex; ix++) {
      nr *nr1=new nr;
      Nr.push_back(nr1);
      Row[iy]->Nr.push_back(nr1);
      Column[ix]->Nr.push_back(nr1);
    }
  }
  SB(0);

  return 0;
}

int vssm2d::Nx()
{
  return Column.size();
}

int vssm2d::Ny()
{
  return Row.size();
}


int vssm2d::SetDefault()
{
  if (Default!=NULL) {
    if (Default->FillHighVect) Default->HighVect.clear();
    if (NHigh > 0) {
      Default->Nr[0]->O = 0;
      Default->NHigh = 0;
    }
    else {
      if (Default->FillHighVect) Default->HighVect.push_back(0);
      Default->Nr[0]->O = 1;
      Default->NHigh = 1;
    }
  }
  if (RowDefault != NULL) {
    if (RowDefault->FillHighVect) RowDefault->HighVect.clear();
    RowDefault->NHigh = 0;
    for(int iy=0; iy<Ny(); iy++) {
      if (Row[iy]->NHigh > 0) RowDefault->Nr[iy]->O = 0;
      else {
	if (RowDefault->FillHighVect) RowDefault->HighVect.push_back(iy);
	RowDefault->Nr[iy]->O = 1;
	RowDefault->NHigh++;
      }
    }
  }
  if (ColumnDefault != NULL) {
    if (ColumnDefault->FillHighVect) ColumnDefault->HighVect.clear();
    ColumnDefault->NHigh = 0;
    for(int ix=0; ix<Nx(); ix++) {
      if (Column[ix]->NHigh > 0) ColumnDefault->Nr[ix]->O = 0;
      else {
	if (ColumnDefault->FillHighVect) ColumnDefault->HighVect.push_back(ix);
	ColumnDefault->Nr[ix]->O = 1;
	ColumnDefault->NHigh++;
      }
    }
  }

  return 0;
}

int vssm2d::UseColumnDefault()
{
  ColumnDefault = new ssm(Nx());

  return 0;
}

int vssm2d::UseRowDefault()
{
  RowDefault = new ssm(Ny());

  return 0;
}

int vssm2d::SCRows(vssm *ssm1, float wg)
{
  if (SparseLkFlag) {
    cout << "Error. SSM cannot have both sparse and ordinary input links\n";
    exit(0);
  }
  if (ssm1->NN() != Nx()) {
    cout << "Nx and NN must match in SCRows!\n";
    exit(0);
  }
  for (int iy=0; iy<Ny(); iy++) {
    Row[iy]->SC(ssm1, wg);
  }

  return 0;
}

int vssm2d::SCRows(vssm *ssm1, float wgmin, float wgmax)
{
  if (SparseLkFlag) {
    cout << "Error. SSM cannot have both sparse and ordinary input links\n";
    exit(0);
  }
  if (ssm1->NN() != Nx()) {
    cout << "Nx and NN must match in SCRows!\n";
    exit(0);
  }
  for (int iy=0; iy<Ny(); iy++) {
    Row[iy]->SC(ssm1, wgmin, wgmax);
  }

  return 0;
}

int vssm2d::SCColumns(vssm *ssm1, float wg)
{
  if (SparseLkFlag) {
    cout << "Error. SSM cannot have both sparse and ordinary input links\n";
    exit(0);
  }
  if (ssm1->NN() != Ny()) {
    cout << "Ny and NN must match in SCColumns!\n";
    exit(0);
  }
  for (int ix=0; ix<Nx(); ix++) {
    Column[ix]->SC(ssm1, wg);
  }

  return 0;
}

int vssm2d::SCColumns(vssm *ssm1, float wgmin, float wgmax)
{
  if (SparseLkFlag) {
    cout << "Error. SSM cannot have both sparse and ordinary input links\n";
    exit(0);
  }
  if (ssm1->NN() != Ny()) {
    cout << "Ny and NN must match in SCColumns!\n";
    exit(0);
  }
  for (int ix=0; ix<Nx(); ix++) {
    Column[ix]->SC(ssm1, wgmin, wgmax);
  }

  return 0;
}

ssm2d::ssm2d(int sizex, int sizey) : ssm()
{
  Init(sizex, sizey);
}

int ssm2d::Out()
{
  struct timespec clk0, clk1;

  clock_gettime( CLOCK_MONOTONIC, &clk0);
  NHigh = 0;
  if (FillHighVect) HighVect.clear();
  if (FillHighVectRow) HighVectRow.clear();
  if (FillHighVectColumn) HighVectColumn.clear();

  for(int iy=0; iy<Ny(); iy++) {
    Row[iy]->NHigh=0;
  }

  for(int ix=0; ix<Nx(); ix++) {
    Column[ix]->NHigh=0;
  }
  
  int k = 0;
  for(int iy=0; iy<Ny(); iy++) {
    for(int ix=0; ix<Nx(); ix++) {
      Nr[k]->Out();
      if (Nr[k]->O > 0.5) {
	NHigh++;
	Row[iy]->NHigh++;
	Column[ix]->NHigh++;
	if (FillHighVect) HighVect.push_back(k);
	if (FillHighVectRow) HighVectRow.push_back(ix);
	if (FillHighVectColumn) HighVectColumn.push_back(iy);
      }
      k++;
    }
  }
  SetDefault();

  clock_gettime( CLOCK_MONOTONIC, &clk1);
  out_time = out_time
    + clk1.tv_sec - clk0.tv_sec + (double)(clk1.tv_nsec - clk0.tv_nsec)*1e-9;

  return 0;
}

int ssm2d::ActivOut()
{
  Activ();
  Out();

  return 0;
}

int io_nr::OutNL()
{
  return OutL.size();
}

int io_nr::ForceActiv(nr *InNr)
{
  if (InNr->O>0.5) {
    for (int i=0; i<OutNL(); i++) {
      OutL[i].Nr->A = OutL[i].Wg;
    }
  }

  return 0;
}

int io_nr::AddActiv(nr *InNr)
{
  if (InNr->O>0.5) {
    for (int i=0; i<OutNL(); i++) {
      OutL[i].Nr->A += OutL[i].Wg;
    }
  }

  return 0;
}

int io_nr::ForceActiv()
{
  for (int i=0; i<OutNL(); i++) {
    OutL[i].Nr->A = OutL[i].Wg;
  }

  return 0;
}

int io_nr::AddActiv()
{
  for (int i=0; i<OutNL(); i++) {
    OutL[i].Nr->A += OutL[i].Wg;
  }

  return 0;
}

int io_nr::ForceActiv(nr *InNr, vector<vssm*> OutSSM, lk_set_t &SparseOutLkSet)
{
  if (InNr->O>0.5) {
    for (int il=0; il<OutNL(); il++) {
      pair<int,int>p = SparseOutLkSet[il];
      int issm = p.first;
      int inr = p.second;
      OutSSM[issm]->SetActiv(inr, OutL[il].Wg); 
      OutSSM[issm]->Forced=true; 
    }
  }
  
  return 0;
}

int io_nr::AddActiv(nr *InNr, vector<vssm*> OutSSM, lk_set_t &SparseOutLkSet)
{
  if (InNr->O>0.5) {
    for (int il=0; il<OutNL(); il++) {
      pair<int,int>p = SparseOutLkSet[il];
      int issm = p.first;
      int inr = p.second;
      OutSSM[issm]->AddActiv(inr, OutL[il].Wg); 
      OutSSM[issm]->Forced=true; 
    }
  }
  
  return 0;
}

int io_nr::OutCN(nr *nr1, float wg)
{
  lk l;

  l.Wg = wg;
  l.Nr = nr1;
  OutL.push_back(l);

  return 0;
}

int io_nr::OutCN(nr *nr1, float wgmin, float wgmax)
{
  lk l;

  l.Wg = wgmin+rnd()*(wgmax-wgmin);
  l.Nr = nr1;
  OutL.push_back(l);

  return 0;
}

int io_nr::OutCN(vssm *ssm1, float wg)
{
  for(int i=0; i<ssm1->NN(); i++) {
    OutCN(ssm1->Nr[i], wg);
  }

  return 0;
}

int io_nr::OutCN(vssm *ssm1, float wgmin, float wgmax)
{
  for(int i=0; i<ssm1->NN(); i++) {
    OutCN(ssm1->Nr[i], wgmin, wgmax);
  }
    
  return 0;
}

int io_nr::SparseOutHbbn(nr *InNr, float OHWpar, vector<vssm*> OutSSM,
			 lk_set_t &SparseOutLkSet)
{
  if (InNr->O > 0.5) {
    SparseOutCreateActiveLks(OutSSM, SparseOutLkSet);
    OutHbbn(InNr, OHWpar);
  }

  return 0;
}

int io_nr::OutHbbn(nr *InNr, float OHWpar)
{
  float DW;
  
  if (InNr->O > 0.5) {
    for (int i=0; i<OutNL(); i++) {
      if (OutL[i].Nr->O > 0.5) DW=OHWpar*(1.-OutL[i].Wg);
      else DW=OHWpar*(-1.-OutL[i].Wg);
      OutL[i].Wg += DW;
    }
  }

  return 0;
}

int io_nr::SparseOutCreateActiveLks(vector<vssm*> OutSSM,
				    lk_set_t &SparseOutLkSet)
{
  for (unsigned int issm=0; issm<OutSSM.size(); issm++) {
    for (int inr=0; inr<OutSSM[issm]->NN(); inr++) {
      pair<int,int>p = make_pair(issm,inr); 
      if (OutSSM[issm]->Nr[inr]->O > 0.5
	&& find(SparseOutLkSet.begin(), SparseOutLkSet.end(), p)
	  ==SparseOutLkSet.end()) {
	lk l;
	l.Wg = 0;
	l.Nr = OutSSM[issm]->Nr[inr];
	OutL.push_back(l);
	SparseOutLkSet.push_back(p);
      }
    }
  }

  return 0;
}

int vssm_io::Init()
{
  SparseOutLkFlag=false;
  OHWpar=1;

  return 0;
}

int vssm_io::Init(int size)
{
  Init();
  for(int i=0; i<size; i++) {
    nr *nr1=new nr(ActivFunc);
    Nr.push_back(nr1);
    io_nr *nr2=new io_nr;
    OutNr.push_back(nr2);
  }
  SB(0);

  return 0;
}

int vssm_io::SetSparseOut()
{
  SparseOutLkFlag = true;
  for(int i=0; i<NN(); i++) {
    lk_set_t* ls1 = new lk_set_t;
    SparseOutLkSet.push_back(ls1);
  }

  return 0;
}

int vssm_io::OutFC(vssm *ssm1, float wg)
{
  if (SparseOutLkFlag) {
    cout << "Error. SSM cannot have both sparse and ordinary output links\n";
    exit(0);
  }
  for(int i=0; i<NN(); i++) {
    OutNr[i]->OutCN(ssm1, wg);
  }
  OutSSM.push_back(ssm1);

  return 0;
}

int vssm_io::OutFC(vssm *ssm1, float wgmin, float wgmax)
{
  if (SparseOutLkFlag) {
    cout << "Error. SSM cannot have both sparse and ordinary output links\n";
    exit(0);
  }
  for(int i=0; i<NN(); i++) {
    OutNr[i]->OutCN(ssm1, wgmin, wgmax);
  }
  OutSSM.push_back(ssm1);

  return 0;
}

int vssm_io::SparseOutFC(vssm *ssm1)
{
  if (!SparseOutLkFlag) {
    cout << "Error. SSM cannot have both ordinary and sparse output links\n";
    exit(0);
  }

  OutSSM.push_back(ssm1);

  return 0;
}

ssm_io::ssm_io() : ssm()
{
  Init();
}

ssm_io::ssm_io(int size) : ssm()
{
  Init(size);
}

int vssm_as::Init(int size, wnn_type wta_flag, nr *build_flag, nr *retr_flag)
{
  ActivFunc = LOGISTIC;
  vssm_io::Init(size);
  UseRewd=false;
  WTA_FLAG = wta_flag;
  BuildFlag = build_flag;
  RetrFlag = retr_flag;
  as_time = 0;

  return 0;
}

int vssm_as::Init(int size, wnn_type wta_flag, nr *build_flag, nr *retr_flag,
		  nr *rewd_flag)
{
  Init(size, wta_flag, build_flag, retr_flag);
  UseRewd=true;
  RewdFlag = rewd_flag;

  return 0;
}

ssm_as::ssm_as(int size, wnn_type wta_flag, nr *build_flag, nr *retr_flag) :
  ssm_io()
{
  Init(size, wta_flag, build_flag, retr_flag);
}

ssm_as::ssm_as(int size, wnn_type wta_flag, nr *build_flag, nr *retr_flag,
               nr *rewd_flag) :
  ssm_io()
{
  Init(size, wta_flag, build_flag, retr_flag, rewd_flag);
  DynamicBiasFlag = false;
}

int ssm_as::ActivOut()
{
  // just to save time:
  if (BuildFlag->O <= 0.5 && RetrFlag->O <= 0.5) return 0;

  Activ();
  AsOut();

  return 0;
}

int ssm_as::AsOut()
{
  struct timespec clk0, clk1;
  int N;
  if (WTA_FLAG==T_NEW_WTA || WTA_FLAG==T_NEW_KWTA) N = NewWnnNum;
  else N = NN();

  clock_gettime( CLOCK_MONOTONIC, &clk0);
  bool null_in=NullIn();
  if (null_in && Forced==false) {
    //cout << "null in\n";
    //cout << NN() << endl;
    Clear();
    return 0;
  }
  if (BuildFlag->O > 0.5 && RetrFlag->O <= 0.5) {
    int iw=-1;
    if (WTA_FLAG==T_NEW_WTA || WTA_FLAG==T_NEW_KWTA) iw = NewWTA();
    else if (WTA_FLAG==T_WTA) iw = WTA();
    else if (WTA_FLAG==T_WNN) iw = Wnn();
    //cout << "build, iw: " << iw << endl;    
    if (iw>=0) {
      if (SparseOutLkFlag) OutNr[iw]->SparseOutHbbn(Nr[iw], OHWpar, OutSSM, 
						    *SparseOutLkSet[iw]);
      else OutNr[iw]->OutHbbn(Nr[iw], OHWpar);
    }
  }
  else if (BuildFlag->O <= 0.5 && RetrFlag->O > 0.5) {
    if (WTA_FLAG==T_NEW_KWTA) KWTA();
    else {
      int iw=RndWnn();
      //cout << "retr, iw: " << iw << endl;
      if (iw>=0) {
	//cout << "i wnn: " << iw << endl; 
	if (SparseOutLkFlag) {
	  for (unsigned int issm=0; issm<OutSSM.size(); issm++) {
	    OutSSM[issm]->LowActiv();
	  }
	  OutNr[iw]->ForceActiv(Nr[iw], OutSSM, *SparseOutLkSet[iw]);
	}
	else OutNr[iw]->ForceActiv(Nr[iw]);
	for (unsigned int issm=0; issm<OutSSM.size(); issm++) {
	  OutSSM[issm]->Out();
	  OutSSM[issm]->SetDefault();
	}
	
	for(int i=0; i<N; i++) {
	  if (i!=iw) Nr[i]->O=0;
	}
      }
    }
  }
  /*
  else if (BuildFlag->O > 0.5 && RetrFlag->O > 0.5 && UseRewd>0) {
    if (RewdFlag->O > 0.5) {
      cout << "Extend HbbnB\n";
      int n_high = 0;
      if (SparseLkFlag) n_high=SparseNHighIn();
      int nw = 0;
      for (int i=0; i<NN(); i++) {
	if (Nr[i]->O > 0.5) {
	  nw++;
	  Nr[i]->Hbbn(HWpar);
	  Nr[i]->Activ();
	  if (SparseLkFlag) { // unconnected neurons have negative weights
	    int n0 = Nr[i]->NHighIn();
	    Nr[i]->A -= (n_high - n0);
	  }
	  Nr[i]->B += HBpar*(-Nr[i]->A+Nr[i]->EPS);
	}
      }
      // cout << "nw: " << nw << endl;
    }
    else { 
      // cout << "Frustr B\n";
      int nw = 0;
      for (int i=0; i<NN(); i++) {
	if (Nr[i]->O > 0.5) {
	  nw++;
	  Nr[i]->B -= 1;
	}
      }
      // cout << "nw: " << nw << endl;
    }
  }
  */

  clock_gettime( CLOCK_MONOTONIC, &clk1);
  as_time = as_time
    + clk1.tv_sec - clk0.tv_sec + (double)(clk1.tv_nsec - clk0.tv_nsec)*1e-9;

  return 0;
}

int ssm_as::KWTA()
{
  int N;
  if (WTA_FLAG==T_NEW_WTA || WTA_FLAG==T_NEW_KWTA) N = NewWnnNum;
  else N = NN();

  if (DynamicBiasFlag) SetDynamicBias();
  Out();
  //if (SparseOutLkFlag) {
  for (unsigned int issm=0; issm<OutSSM.size(); issm++) {
    OutSSM[issm]->LowActiv();
  }
  //}
  
  //bool first_flag = true;
  int n_wnn=0; 
  for(int i=0; i<N; i++) {
    if (Nr[i]->Used==1 && Nr[i]->O > 0.5) {
      n_wnn++;
      //cout << "wnn: " << i;

      if (SparseOutLkFlag)
	OutNr[i]->AddActiv(Nr[i], OutSSM, *SparseOutLkSet[i]);
      //else {
      //if (first_flag==true) {
      //OutNr[inr]->ForceActive(Nr[inr]);
      //first_flag = false;
      //}
      else OutNr[i]->AddActiv(Nr[i]);
    }
  }

  //cout << "Num. wnn: " << n_wnn << endl;
  if (n_wnn<K) return 0;
  for (unsigned int issm=0; issm<OutSSM.size(); issm++) {
    //OutSSM[issm]->Out(); done in NumWnn()
    //OutSSM[issm]->SetDefault(); done in RndWnn()
    //fssm *tmpssm = dynamic_cast<fssm*>(OutSSM[issm]);
    //cout << "av: " << tmpssm->ActivVect.size();

    //int best_act =
    OutSSM[issm]->ActivWnn(); // you can eventually use RndActivWnn()
    //cout  << "Best act: " << best_act << endl;
  }

  return 0;
}
 
int ssm_as::UseDynamicBias(int min_bias, int max_bias)
{
  DynamicBiasFlag = true;
  MinWinnNum = K;
  MinBias=min_bias;
  MaxBias=max_bias;
  BiasNum = (max_bias - min_bias + 1);
  BiasVect.clear();
  BiasVect.resize(BiasNum);
  DynamicBias = new ssm(BiasNum);

  return 0;
}

int ssm_as::SetDynamicBias()
{
  int N;
  if (WTA_FLAG==T_NEW_WTA || WTA_FLAG==T_NEW_KWTA) N = NewWnnNum;
  else N = NN();

  fill(BiasVect.begin(), BiasVect.end(), 0);
  for(int i=0; i<N; i++) {
    if (Nr[i]->Used==1) {
      int iA = (int)ceil(Nr[i]->A) + MaxBias;
      if (iA<0) {
	cerr << "Error: neuron activation state + maximum bias < 0\n";
	exit(0);
      }
      if (iA>BiasNum-1) {
	cerr << "Error: neuron activation state + minimum bias > 0\n";
	exit(0);
      }
      BiasVect[iA]++;
    }
  }
  int n_wnn=0, iA=0;
  while (n_wnn < MinWinnNum) {
    n_wnn += BiasVect[BiasNum-1-iA];
    iA++;
  }
  DB = MinBias + iA; // dynamic bias

  DynamicBias->Clear();
  if (DynamicBias->FillHighVect) DynamicBias->HighVect.push_back(iA);
  DynamicBias->Nr[iA]->O = 1;
  DynamicBias->NHigh = 1;

  for(int i=0; i<N; i++) {
    if (Nr[i]->Used==1) {
      Nr[i]->A += DB;
    }
  }
  //cout << "Dynamic bias: " << DB << endl;

  return 0;
}

//////////////////////////////////
int nr::SparseHbbnB(float HWpar, float HBpar, vector<vssm*> SparseInSSM,
		    vector<float> SparseInMaxWeight,
		    input_lk_set_t &InputLkSet, input_lk_wg_t &InputLkWg,
		    int inr0)
{
  SparseHbbn(HWpar, SparseInSSM, SparseInMaxWeight, InputLkSet, InputLkWg,
	     inr0);
  //AdjustB(HBpar);  used in the old version
  // take care of HBpar: assume that A was calculated in SparseHbbn 
  // and that O should be positive
  B += HBpar*(-A+EPS);

  return 0;
}

int nr::SparseCreateActiveLks(vector<vssm*> SparseInSSM,
			      vector<float> SparseInMaxWeight,
			      input_lk_set_t &InputLkSet,
			      input_lk_wg_t &InputLkWg, int inr0)
{
  //float DefaultMaxWg = 1; // put in header
  A=B; // not present in old version

  for (unsigned int issm=0; issm<SparseInSSM.size(); issm++) {
    float wgmax = SparseInMaxWeight[issm];
    vssm *ssm1=SparseInSSM[issm];
    for(unsigned int i=0; i<ssm1->HighVect.size(); i++) {
      int inr = ssm1->HighVect[i];
      vector<int> *lk_set = &InputLkSet[issm][inr];
      vector<float> *lk_wg = &InputLkWg[issm][inr];
      if (std::find(lk_set->begin(), lk_set->end(), inr0)==lk_set->end()) {
	lk_set->push_back(inr0);
	lk_wg->push_back(wgmax);
	A += wgmax*SparseInSSM[issm]->Nr[inr]->O; //not pres. in old vs.
	//float f= DefaultMaxWg*SparseInSSM[issm]->Nr[inr]->O;
	//if (f!=1) {cout << f <<endl; exit(0);}
      }
    }
  }

  return 0;
}

int nr::SparseHbbn(float HWpar, vector<vssm*> SparseInSSM,
		   vector<float> SparseInMaxWeight,
		   input_lk_set_t &InputLkSet, input_lk_wg_t &InputLkWg,
		   int inr0)
{
  SparseCreateActiveLks(SparseInSSM, SparseInMaxWeight, InputLkSet, InputLkWg,
			inr0);
  //Hbbn(HWpar);  used in the old version
  // now taking care of HWpar in SparseCreateActiveLks 

  return 0;
}

int ssm::WTA()
{
  int iw=Wnn();
  if (iw>=0) {
    if (SparseLkFlag) Nr[iw]->SparseHbbnB(HWpar, HBpar, SparseInSSM,
					  SparseInMaxWeight,
					  InputLkSet, InputLkWg, iw);
    else Nr[iw]->HbbnB(HWpar, HBpar);
  }

  return iw;
}

int ssm::NewWTA()
{
  int iw=NewWnn();
  if (iw>=0) {
    //Nr[iw]->Used = 1; // remove: already done by NewWnn
    //Nr[iw]->O = 1; // remove: already done by NewWnn
    if (SparseLkFlag) Nr[iw]->SparseHbbnB(HWpar, HBpar, SparseInSSM,
					  SparseInMaxWeight,
					  InputLkSet, InputLkWg, iw);
    else Nr[iw]->HbbnB(HWpar, HBpar);
    //for(int i=0; i<NN(); i++) { // remove: already done by NewWnn
    //  if (i!=iw) Nr[i]->O=0; // remove: already done by NewWnn
    //} // remove: already done by NewWnn
  }

  return iw;
}

