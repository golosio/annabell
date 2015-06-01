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

#ifndef FSSMH
#define FSSMH

#include "ssm.h"

class fssm : virtual public vssm
{
 public:
  std::vector<std::vector<int>*> SparseRef;
  std::vector<int> SparseRefBias;
  std::vector<int> ActivVect;
  //input_lk_set_t InputLkSet;

  fssm() {Init(); FillHighVect=true;}
  fssm(int size) {Init(size); FillHighVect=true;}

  //int Init();
  virtual int Activ();
  virtual int Out();
  virtual int ActivOut();
  virtual int LowActiv();
  virtual int SetActiv(int inr, float aval);
  virtual int AddActiv(int inr, float aval);
  bool NullIn();

  int AddSparseRef(vssm *ssm1, int bias);
  int AddSparseRef(vssm *ssm1);
  int AddSparseRefNum(int num_ref);
  int AddSparseRefToRows(vssm2d *ssm1);
  int AddSparseRefToColumns(vssm2d *ssm1);
  int Wnn();
  int NumActivWnn(float &amax);
  int RndActivWnn();
  int ActivWnn();
  int WTA();
  int NewWnn();
  int NewWTA();
  int NumWnn();
  int RndWnn();
  int NextWnn();
  int Clear();

  int SparseActiv();
  int SparseFC(vssm *ssm1);
  int SparseFC(vssm *ssm1, bool check_null_flag);
  int AddSparseFCRef(vssm *ssm1);
};

class fssm2d : public vssm2d, public fssm
{
 public:
  std::vector<std::vector<int>*> SparseRefRow;
  std::vector<std::vector<int>*> SparseRefColumn;
  //std::vector<int> ActivVectRow;
  //std::vector<int> ActivVectColumn;
  
 fssm2d(int sizex, int sizey) : fssm() {Init(sizex, sizey);}

  //int Init();
  int Activ();
  int Out();
  int ActivOut();
  int AddSparseRefRows(vssm *ssm1);
  int AddSparseRefColumns(vssm *ssm1);

};    

#endif
