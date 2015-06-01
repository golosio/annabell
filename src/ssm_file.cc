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
#include <stdio.h>
#include <stdlib.h>
#include "ssm.h"
#include "sizes.h"
#include "monitor.h"

using namespace std;
using namespace sizes;

template <typename T>
int FREAD(T *var, int size, int nmemb, FILE *fp)
{
  int err = fread(var, size, nmemb, fp);
  if (err!=nmemb) {
    cerr << "Error reading file\n";
    exit(0);
  }

  return 0;
}

int vssm::SaveNr(FILE *fp)
{
  fwrite(&NewWnnNum, sizeof(int), 1, fp);
  for (int i=0; i<NN(); i++) {
    fwrite(&Nr[i]->B, sizeof(float), 1, fp);
    fwrite(&Nr[i]->Used, sizeof(bool), 1, fp);
  }

  return 0;
}

int vssm::LoadNr(FILE *fp)
{
  FREAD(&NewWnnNum, sizeof(int), 1, fp);
  for (int i=0; i<NN(); i++) {
    FREAD(&Nr[i]->B, sizeof(float), 1, fp);
    FREAD(&Nr[i]->Used, sizeof(bool), 1, fp);
  }

  return 0;
}

int vssm::SaveSparseInputLinks(FILE *fp)
{
  int Nssm=SparseInSSM.size();
  for (int issm=0; issm<Nssm; issm++) {
    vssm *ssm1=SparseInSSM[issm];
    int Nnr = ssm1->NN();
    for (int inr=0; inr<Nnr; inr++) {
      vector<int> *lk_set = &InputLkSet[issm][inr];
      vector<float> *lk_wg = &InputLkWg[issm][inr];
      int Nlk = lk_set->size();
      fwrite(&Nlk, sizeof(int), 1, fp);
      for (int ilk=0; ilk<Nlk; ilk++) {
	fwrite(&lk_set->at(ilk), sizeof(int), 1, fp); 
	fwrite(&lk_wg->at(ilk), sizeof(float), 1, fp);
      }
    }
  } 

  return 0;
}

int vssm::LoadSparseInputLinks(FILE *fp)
{
  int Nssm=SparseInSSM.size();
  for (int issm=0; issm<Nssm; issm++) {
    vssm *ssm1=SparseInSSM[issm];
    int Nnr = ssm1->NN();
    for (int inr=0; inr<Nnr; inr++) {
      vector<int> *lk_set = &InputLkSet[issm][inr];
      vector<float> *lk_wg = &InputLkWg[issm][inr];
      int Nlk;
      FREAD(&Nlk, sizeof(int), 1, fp);
      lk_set->clear(); 
      lk_wg->clear();  
      for (int ilk=0; ilk<Nlk; ilk++) {
	int nr1;
	float wg;
	FREAD(&nr1, sizeof(int), 1, fp); 
	FREAD(&wg, sizeof(float), 1, fp); 
	lk_set->push_back(nr1); 
	lk_wg->push_back(wg); 
      }
    }
  } 

  return 0;
}

int vssm_io::SaveSparseOutputLinks(FILE *fp)
{
  for (int i=0; i<NN(); i++) {
    io_nr *nr1 = OutNr[i];
    lk_set_t* ls1 = SparseOutLkSet[i];
    int Nlk = ls1->size();
    fwrite(&Nlk, sizeof(int), 1, fp);
    for (int il=0; il<Nlk; il++) {
      pair<int,int>p = ls1->at(il);
      int issm = p.first;
      int inr = p.second;
      fwrite(&issm, sizeof(int), 1, fp);
      fwrite(&inr, sizeof(int), 1, fp);
      float wg = nr1->OutL[il].Wg;
      fwrite(&wg, sizeof(float), 1, fp);
    }
  }

  return 0;
}

int vssm_io::LoadSparseOutputLinks(FILE *fp)
{
  for (int i=0; i<NN(); i++) {
    io_nr *nr1 = OutNr[i];
    lk_set_t* ls1 = SparseOutLkSet[i];
    int Nlk;
    FREAD(&Nlk, sizeof(int), 1, fp);
    nr1->OutL.clear();
    for (int il=0; il<Nlk; il++) {
      int issm, inr;
      float wg;
      FREAD(&issm, sizeof(int), 1, fp);
      FREAD(&inr, sizeof(int), 1, fp);
      FREAD(&wg, sizeof(float), 1, fp);
      pair<int,int>p = make_pair(issm,inr); 
      lk l;
      l.Wg = wg;
      l.Nr = OutSSM[issm]->Nr[inr];
      nr1->OutL.push_back(l);
      ls1->push_back(p);
    }
  }

  return 0;
}

int vssm_io::SaveOutputLinks(FILE *fp)
{
  for (int i=0; i<NN(); i++) {
    io_nr *nr1 = OutNr[i];
    int Nlk =  nr1->OutL.size();
    for (int il=0; il<Nlk; il++) {
      float wg = nr1->OutL[il].Wg;
      fwrite(&wg, sizeof(float), 1, fp);
    }
  }

  return 0;
}

int vssm_io::LoadOutputLinks(FILE *fp)
{
  for (int i=0; i<NN(); i++) {
    io_nr *nr1 = OutNr[i];
    int Nlk =  nr1->OutL.size();
    for (int il=0; il<Nlk; il++) {
      float wg;
      FREAD(&wg, sizeof(float), 1, fp);
      nr1->OutL[il].Wg = wg;
    }
  }

  return 0;
}

int vssm::SaveInputLinks(FILE *fp)
{
  for (int i=0; i<NN(); i++) {
    nr *nr1 = Nr[i];
    int Nlk =  nr1->L.size();
    for (int il=0; il<Nlk; il++) {
      float wg = nr1->L[il].Wg;
      fwrite(&wg, sizeof(float), 1, fp);
    }
  }

  return 0;
}

int vssm::LoadInputLinks(FILE *fp)
{
  for (int i=0; i<NN(); i++) {
    nr *nr1 = Nr[i];
    int Nlk =  nr1->L.size();
    for (int il=0; il<Nlk; il++) {
      float wg;
      FREAD(&wg, sizeof(float), 1, fp);
      nr1->L[il].Wg = wg;
    }
  }

  return 0;
}

int monitor::SaveWM(FILE *fp)
{
  for (int iw=0; iw<WMSize; iw++) {
    fwrite(&wflag[iw], sizeof(int), 1, fp);
    for (int ic=0; ic<NC; ic++) {
      fwrite(&wlst[iw][ic], sizeof(char), 1, fp);
    }
  }

  return 0;
}

int monitor::LoadWM(FILE *fp)
{
  for (int iw=0; iw<WMSize; iw++) {
    FREAD(&wflag[iw], sizeof(int), 1, fp);
    for (int ic=0; ic<NC; ic++) {
      FREAD(&wlst[iw][ic], sizeof(char), 1, fp);
    }
  }

  return 0;
}

///////////////////////////////////////////
// Statistic
///////////////////////////////////////////
long vssm::CountSparseInputLinks()
{
  long NlkTot = 0;
  long Nssm=SparseInSSM.size();
  for (long issm=0; issm<Nssm; issm++) {
    vssm *ssm1=SparseInSSM[issm];
    long Nnr = ssm1->NN();
    for (long inr=0; inr<Nnr; inr++) {
      vector<int> *lk_set = &InputLkSet[issm][inr];
      long Nlk = lk_set->size();
      NlkTot += Nlk;
    }
  } 

  return NlkTot;
}

long vssm::CountVirtualInputLinks()
{
  long NlkTot = 0;
  long Nssm=SparseInSSM.size();
  for (long issm=0; issm<Nssm; issm++) {
    vssm *ssm1=SparseInSSM[issm];
    long Nnr = ssm1->NN();
    NlkTot += Nnr*NN();
  } 

  return NlkTot;
}

long vssm_io::CountSparseOutputLinks()
{
  long NlkTot = 0;
  for (long i=0; i<NN(); i++) {
    //io_nr *nr1 = OutNr[i];
    lk_set_t* ls1 = SparseOutLkSet[i];
    long Nlk = ls1->size();
    NlkTot += Nlk;
  }

  return NlkTot;
}

long vssm_io::CountVirtualOutputLinks()
{
  long NlkTot = 0;
  long Nssm=OutSSM.size();
  for (long issm=0; issm<Nssm; issm++) {
    vssm *ssm1=OutSSM[issm];
    long Nnr = ssm1->NN();
    NlkTot += Nnr*NN();
  } 

  return NlkTot;
}

long vssm_io::CountOutputLinks()
{
  long NlkTot = 0;
  for (long i=0; i<NN(); i++) {
    io_nr *nr1 = OutNr[i];
    long Nlk =  nr1->OutL.size();
    NlkTot += Nlk;
  }

  return NlkTot;
}

long vssm::CountInputLinks()
{
  long NlkTot = 0;
  for (long i=0; i<NN(); i++) {
    nr *nr1 = Nr[i];
    long Nlk =  nr1->L.size();
    NlkTot += Nlk;
  }

  return NlkTot;
}

