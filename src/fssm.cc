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
#include <stdlib.h>
#include "fssm.h"
#include "rnd.h"

using namespace std;

int fssm::SparseActiv()
{
  cout << "Error: SparseActiv not implemented for fssm\n";
  exit(0);

  return 0;
}

int fssm::Activ()
{
  struct timespec clk0, clk1;

  if (SparseLkFlag) return SparseActiv();
  clock_gettime( CLOCK_REALTIME, &clk0);
  Forced=false;
  ActivVect.clear();
  for (unsigned int iv=0; iv<SparseRef.size(); iv++) {
    for (unsigned int i=0; i< SparseRef[iv]->size(); i++) {
      int inr = SparseRef[iv]->at(i) + SparseRefBias[iv];
      if (inr>=0 && inr<NN()) {
	//ActivVect.push_back(inr); // to keep it ordered:
	std::vector<int>::iterator ia=ActivVect.begin();
	while (ia<ActivVect.end() && *ia<=inr) ia++;
	if (ia==ActivVect.begin() || *(ia-1)!=inr) ActivVect.insert(ia,inr);
	Nr[inr]->Activ();
	Nr[inr]->A += GB;
      }
    }
  }

  clock_gettime( CLOCK_REALTIME, &clk1);
  act_time = act_time
    + clk1.tv_sec - clk0.tv_sec + (double)(clk1.tv_nsec - clk0.tv_nsec)*1e-9;

  return 0;
}

int fssm::Out()
{
  struct timespec clk0, clk1;

  clock_gettime( CLOCK_REALTIME, &clk0);
  Clear();

  for(unsigned int i=0; i<ActivVect.size(); i++) {
    int inr = ActivVect[i];
    Nr[inr]->Out();
    if (Nr[inr]->O > 0.5) {
      NHigh++;
      HighVect.push_back(inr);
    }
  }
  SetDefault();

  clock_gettime( CLOCK_REALTIME, &clk1);
  out_time = out_time
    + clk1.tv_sec - clk0.tv_sec + (double)(clk1.tv_nsec - clk0.tv_nsec)*1e-9;

  return 0;
}

int fssm::ActivOut()
{
  Activ();
  Out();

  return 0;
}

int fssm::LowActiv()
{
  Forced=false;
  ActivVect.clear();

  return 0;
}

int fssm::SetActiv(int inr, float aval)
{
  //ActivVect.push_back(inr);
  std::vector<int>::iterator ia=ActivVect.begin();
  while (ia<ActivVect.end() && *ia<=inr) ia++;
  if (ia==ActivVect.begin() || *(ia-1)!=inr) ActivVect.insert(ia,inr);
  Nr[inr]->A = aval;

  return 0;
}

int fssm::AddActiv(int inr, float aval)
{
  //ActivVect.push_back(inr);
  std::vector<int>::iterator ia=ActivVect.begin();
  while (ia<ActivVect.end() && *ia<=inr) ia++;
  if (ia==ActivVect.begin() || *(ia-1)!=inr) {
    ActivVect.insert(ia,inr);
    Nr[inr]->A = aval;
  }
  else Nr[inr]->A += aval;

  return 0;
}

bool fssm::NullIn()
{
  if (SparseLkFlag) return SparseNullIn();

  for (unsigned int iv=0; iv<SparseRef.size(); iv++) {
    for (unsigned int i=0; i<SparseRef[iv]->size(); i++) {
      int inr = SparseRef[iv]->at(i) + SparseRefBias[iv];
      if (inr>=0 && inr<NN()) {
	for(int il=0; il<Nr[inr]->NL(); il++) {
	  if (Nr[inr]->L[il].Nr->O>0.5) {
	    // sort to compare against slow version
	    //cout << inr << " " << il << " " << NN() << endl;
	    return false;
	  }
	}
      }
    }
  }
  
  return true;
}

int fssm::AddSparseRef(vssm *ssm1, int bias)
{
  ssm1->FillHighVect=true;
  SparseRef.push_back(&ssm1->HighVect);
  SparseRefBias.push_back(bias);

  return 0;
}

int fssm::AddSparseRef(vssm *ssm1)
{
  AddSparseRef(ssm1, 0);

  return 0;
}

int fssm::AddSparseRefNum(int num_ref)
{
  std::vector<int> *ref_vect = new std::vector<int>;
  ref_vect->push_back(num_ref);
  SparseRef.push_back(ref_vect);
  SparseRefBias.push_back(0);

  return 0;
}

int fssm::AddSparseRefToRows(vssm2d *ssm1)
{
  ssm1->FillHighVectRow=true;
  SparseRef.push_back(&ssm1->HighVectRow);
  SparseRefBias.push_back(0);

  return 0;
}

int fssm::AddSparseRefToColumns(vssm2d *ssm1)
{
  ssm1->FillHighVectColumn=true;
  SparseRef.push_back(&ssm1->HighVectColumn);
  SparseRefBias.push_back(0);

  return 0;
}

int fssm::Wnn()
{
  float amax = 0;
  int imax = -1;
  float R=0;
  
  Clear();
  if (!NullIn()) {
    for(unsigned int i=0; i<ActivVect.size(); i++) {
      int inr = ActivVect[i];
      if (Nr[inr]->Used==1) {
	Nr[inr]->Out();
	if (Nr[inr]->O > 0.5) {
	  amax = Nr[inr]->A;
	  imax = inr;
	  break;
	}
      }
      else {
	R = rnd();
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
      HighVect.push_back(imax);
    }
  }
  SetDefault();
  
  return imax;
}

int fssm::NumActivWnn(float &amax)
{
  int n_wnn = 0;

  for(unsigned int i=0; i<ActivVect.size(); i++) {
    int inr = ActivVect[i];
    float act = Nr[inr]->A;
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

int fssm::RndActivWnn()
{
  float amax = 0;
  int imax = -1;
  
  Clear();

  int n_wnn=NumActivWnn(amax);
  int k_wnn=rnd_int()%n_wnn;
  int k=0;

  //if (!NullIn()) {
  for(unsigned int i=0; i<ActivVect.size(); i++) {
    int inr = ActivVect[i];
    float act = Nr[inr]->A;
    if (act==amax) {
      if (k==k_wnn) {
	imax=inr;
	break;
      }
      k++;
    }
  }
  //}

  if (imax>=0) {
    Nr[imax]->O = 1;
    NHigh = 1;
    HighVect.push_back(imax);
  }
  SetDefault();
  
  return imax;
}

int fssm::ActivWnn()
{
  float amax = 0;
  int imax = -1;
  
  Clear();
  //if (!NullIn()) {
    for(unsigned int i=0; i<ActivVect.size(); i++) {
      int inr = ActivVect[i];
      float act = Nr[inr]->A;
      if (i==0 || act>amax) {
	amax = act;
	imax = inr;
      }
    }
    //}
  if (imax>=0) {
    Nr[imax]->O = 1;
    NHigh = 1;
    HighVect.push_back(imax);
  }
  SetDefault();
  
  return imax;
}

int fssm::WTA()
{
  cout << "Error: WTA not implemented for fssm\n";
  exit(0);
  
  return 0;
}

int fssm::NewWnn()
{
  int imax = -1;
  Clear();  
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
    HighVect.push_back(imax);
  }
  SetDefault();

  return imax;
}

int fssm::NewWTA()
{
  cout << "Error: NewWTA not implemented for fssm\n";
  exit(0);

}

int fssm::NumWnn()
{
  int n_wnn=0;
 
  Out(); // try to remove
  for(unsigned int i=0; i<HighVect.size(); i++) {
    int inr = HighVect[i];
    if (Nr[inr]->Used==1) {
      if (Nr[inr]->O > 0.5) n_wnn++;
    }
  }
  
  return n_wnn;
}

int fssm::NextWnn()
{
  int iw=-1;
 
  Out(); // try to remove
  for(unsigned int i=0; i<HighVect.size(); i++) {
    int inr = HighVect[i];
    if (Nr[inr]->Used==1 && Nr[inr]->O > 0.5) {
      if (inr>LastWnn && (iw==-1 || inr<iw)) {
	iw = inr;
      }
    }
  }
  if (iw==-1) {
    for(unsigned int i=0; i<HighVect.size(); i++) {
      int inr = HighVect[i];
      if (Nr[inr]->Used==1 && Nr[inr]->O > 0.5) {
	if (iw==-1 || inr<iw) {
	  iw = inr;
	}
      }
    }
  }
  if (iw!=-1) LastWnn = iw;

  return iw;
}

int fssm::RndWnn()
{
  if (OrderedWnnFlag) return NextWnn();
  int iw=-1;
  int n_wnn=NumWnn();
  //cout << "n_wnn: " << n_wnn << endl;
  if (n_wnn==0) return -1;
  int k_wnn=rnd_int()%n_wnn;
  int k=0;
  for(unsigned int i=0; i<HighVect.size(); i++) {
    int inr = HighVect[i];
    if (Nr[inr]->Used==1 && Nr[inr]->O>0.5) {
      if (k==k_wnn) {
	iw=inr;
	break;
      }
      k++;
    }
  }
  Clear();
  if (iw>=0) {
    Nr[iw]->O = 1;
    NHigh = 1;
    HighVect.push_back(iw);
  }
  SetDefault();

  return iw;
}

int fssm::Clear()
{
  NHigh = 0;
  for(unsigned int i=0; i<HighVect.size(); i++) {
    int inr = HighVect[i];
    Nr[inr]->O = 0;
  }
  HighVect.clear();

  return 0;
}

int fssm2d::Activ()
{
  fssm::Activ();
  struct timespec clk0, clk1;

  clock_gettime( CLOCK_REALTIME, &clk0);
  for (unsigned int iv=0; iv<SparseRefRow.size(); iv++) {
    for (unsigned int i=0; i<SparseRefRow[iv]->size(); i++) {
      int ix = SparseRefRow[iv]->at(i);
      if (ix>=0 && ix<Nx()) {
	for (int iy=0; iy<Ny(); iy++) {
	  int inr = Nx()*iy + ix;
	  //ActivVect.push_back(inr);
	  std::vector<int>::iterator ia=ActivVect.begin();
	  while (ia<ActivVect.end() && *ia<=inr) ia++;
	  if (ia==ActivVect.begin() || *(ia-1)!=inr) ActivVect.insert(ia,inr);
	  Nr[inr]->Activ();
	  Nr[inr]->A += GB;
	}
      }
    }
  }
  for (unsigned int iv=0; iv<SparseRefColumn.size(); iv++) {
    for (unsigned int i=0; i<SparseRefColumn[iv]->size(); i++) {
      int iy = SparseRefColumn[iv]->at(i);
      if (iy>=0 && iy<Ny()) {
	for (int ix=0; ix<Nx(); ix++) {
	  int inr = Nx()*iy + ix;
	  //ActivVect.push_back(inr);
	  std::vector<int>::iterator ia=ActivVect.begin();
	  while (ia<ActivVect.end() && *ia<=inr) ia++;
	  if (ia==ActivVect.begin() || *(ia-1)!=inr) ActivVect.insert(ia,inr);
	  Nr[inr]->Activ();
	  Nr[inr]->A += GB;
	}
      }
    }
  }

  clock_gettime( CLOCK_REALTIME, &clk1);
  act_time = act_time
    + clk1.tv_sec - clk0.tv_sec + (double)(clk1.tv_nsec - clk0.tv_nsec)*1e-9;

  return 0;
}

int fssm2d::Out()
{
  struct timespec clk0, clk1;

  clock_gettime( CLOCK_REALTIME, &clk0);
  NHigh = 0;
  for(unsigned int i=0; i<HighVect.size(); i++) {
    int inr = HighVect[i];
    int ix = inr%Nx();
    int iy = inr/Nx();
    Nr[inr]->O = 0;
    Row[iy]->NHigh=0;
    Column[ix]->NHigh=0;
  }
  HighVect.clear();
  HighVectRow.clear();
  HighVectColumn.clear();
  
  for(unsigned int i=0; i<ActivVect.size(); i++) {
    int inr = ActivVect[i];
    int ix = inr%Nx();
    int iy = inr/Nx();
    Nr[inr]->Out();
    if (Nr[inr]->O > 0.5) {
      NHigh++;
      Row[iy]->NHigh++;
      Column[ix]->NHigh++;
      HighVect.push_back(inr);
      HighVectRow.push_back(ix);
      HighVectColumn.push_back(iy);
    }
  }
  SetDefault();
  
  clock_gettime( CLOCK_REALTIME, &clk1);
  out_time = out_time
    + clk1.tv_sec - clk0.tv_sec + (double)(clk1.tv_nsec - clk0.tv_nsec)*1e-9;

  return 0;
}

int fssm2d::ActivOut()
{
  Activ();
  Out();

  return 0;
}

int fssm2d::AddSparseRefRows(vssm *ssm1)
{
  ssm1->FillHighVect=true;
  SparseRefRow.push_back(&ssm1->HighVect);

  return 0;
}

int fssm2d::AddSparseRefColumns(vssm *ssm1)
{
  ssm1->FillHighVect=true;
  SparseRefColumn.push_back(&ssm1->HighVect);

  return 0;
}

