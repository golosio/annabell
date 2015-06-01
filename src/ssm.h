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

#ifndef SSMH
#define SSMH

#include <iostream>
#include <vector>
#include <set>
#include <utility>
#include <stdlib.h>

class nr;
class vssm;
class vssm2d;
class ssm;
class ssm_io;

enum wnn_type {T_WNN, T_WTA, T_NEW_WTA, T_NEW_KWTA};
enum activ_func_type {HEAVISIDE, LOGISTIC};
typedef std::vector<std::pair<int,int> > lk_set_t;
typedef std::vector<std::vector<std::vector<int> > > input_lk_set_t;
typedef std::vector<std::vector<std::vector<float> > > input_lk_wg_t;

class lk
{
 public:
  float Wg;
  nr *Nr;
};

class nr
{
 public:
  float A;
  float B;
  float O;
  bool Used;
  int ActivFunc;
  std::vector<lk> L;
  static const float EPS;

  nr() {A=0; B=0; O=0; Used=0; ActivFunc=HEAVISIDE;}
  nr(float activ_func) {A=0; B=0; O=0; Used=0; ActivFunc=activ_func;}
  int NL();
  int Activ();
  int Out();
  int NHighIn();
  int CN(nr *nr1, float wg);
  int CN(nr *nr1, float wgmin, float wgmax);
  int CN(vssm *ssm1, float wg);
  int CN(vssm *ssm1, float wgmin, float wgmax);
  int SB(float b);
  int Hbbn(float HWpar);
  int HbbnB(float HWpar, float HBpar);
  int AdjustB(float HBpar);
  int SparseHbbnB(float HWpar, float HBpar, std::vector<vssm*> SparseInSSM,
		  std::vector<float> SparseInMaxWeight,
		  input_lk_set_t &InputLkSet, input_lk_wg_t &InputLkWg,
		  int inr0);
  int SparseCreateActiveLks(std::vector<vssm*> SparseInSSM,
			    std::vector<float> SparseInMaxWeight,
			    input_lk_set_t &InputLkSet,
			    input_lk_wg_t &InputLkWg, int inr0);
  int SparseHbbn(float HWpar, std::vector<vssm*> SparseInSSM,
		 std::vector<float> SparseInMaxWeight,
		 input_lk_set_t &InputLkSet, input_lk_wg_t &InputLkWg,
		 int inr0);
};

class vssm
{
 public:
  std::vector<nr*> Nr;
  std::vector<vssm*> SparseInSSM;
  input_lk_set_t InputLkSet;
  input_lk_wg_t InputLkWg;
  std::vector<int> HighVect;
  std::vector<bool> CheckNullVect;
  std::vector<float> SparseInMinWeight;
  std::vector<float> SparseInMaxWeight;
  ssm *Default;

  bool SparseLkFlag;
  float GB;
  int NHigh;
  float HWpar, HBpar;
  bool FillHighVect;
  int NewWnnNum;
  int LastWnn;
  bool Forced;
  bool OrderedWnnFlag;
  double act_time, out_time;
  int ActivFunc;

  int NN();
  int UseDefault();
  int SetSparseIn();
  int Fill(float *vin);
  int Fill(int *vin);

  bool SparseNullIn();
  int SparseNHighIn();
  //float SparseTotInSign();
  float SparseMinInSign();
  int FC(vssm *ssm1, float wg);
  int FC(vssm *ssm1, float wgmin, float wgmax);
  int SparseFC(vssm *ssm1);
  int SparseFC(vssm *ssm1, bool check_null_flag);
  int SparseFC(vssm *ssm1, float wgmin, float wgmax);
  int SparseFC(vssm *ssm1, float wgmin, float wgmax, bool check_null_flag);

  int SC(vssm *ssm1, float wg);
  int SC(vssm *ssm1, float wgmin, float wgmax);
  int SCtoRows(vssm2d *ssm1, float wg);
  int SCtoRows(vssm2d *ssm1, float wgmin, float wgmax);
  int SCtoColumns(vssm2d *ssm1, float wg);
  int SCtoColumns(vssm2d *ssm1, float wgmin, float wgmax);
  int SB(float b);
  int SaveNr(FILE *fp);
  int LoadNr(FILE *fp);
  int SaveSparseInputLinks(FILE *fp);
  int LoadSparseInputLinks(FILE *fp);
  int SaveInputLinks(FILE *fp);
  int LoadInputLinks(FILE *fp);
  long CountInputLinks();
  long CountSparseInputLinks();
  long CountVirtualInputLinks();

  virtual int Init();
  virtual int Init(int size);

  virtual int Activ()=0;
  virtual int Out()=0;
  virtual int SetDefault();
  virtual int ActivOut()=0;
  virtual int LowActiv()=0;
  virtual int SetActiv(int inr, float aval)=0;
  virtual int AddActiv(int inr, float aval)=0;
  virtual int SparseActiv()=0;
  virtual bool NullIn()=0;
  virtual int Clear()=0;

  virtual int Wnn()=0;
  virtual int NumActivWnn(float &amax)=0;
  virtual int RndActivWnn()=0;
  virtual int ActivWnn()=0;
  virtual int WTA()=0;
  virtual int NewWnn()=0;
  virtual int NewWTA()=0;
  virtual int NumWnn()=0;
  virtual int RndWnn()=0;
  virtual int NextWnn()=0;

};

class ssm : virtual public vssm
{
 public:
  ssm();
  ssm(int size);

  //virtual int Init();
  //virtual int Init(int size);

  virtual int Activ();
  virtual int Out();
  //virtual int SetDefault();
  virtual int ActivOut();
  int LowActiv();
  int SetActiv(int inr, float aval);
  int AddActiv(int inr, float aval);
  virtual int SparseActiv();
  bool NullIn();
  int Clear();

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
};

class vssm2d : virtual public vssm
{
 public:
  std::vector<ssm*> Row;
  std::vector<ssm*> Column;
  std::vector<int> HighVectRow;
  std::vector<int> HighVectColumn;
  ssm* RowDefault;
  ssm* ColumnDefault;
  bool FillHighVectRow;
  bool FillHighVectColumn;

  int Init(int sizex, int sizey);
  int Nx();
  int Ny();

  int SetDefault();
  int UseColumnDefault();
  int UseRowDefault();

  int SCRows(vssm *ssm1, float wg);
  int SCRows(vssm *ssm1, float wgmin, float wgmax);
  int SCColumns(vssm *ssm1, float wg);
  int SCColumns(vssm *ssm1, float wgmin, float wgmax);

  virtual int Out()=0;
  virtual int ActivOut()=0;
  
};


class ssm2d : public ssm, public vssm2d
{
 public:
  ssm2d(int sizex, int sizey);
  int Out();
  int ActivOut();
 
};

class io_nr
{
 public:
  std::vector<lk> OutL;

  int OutNL();
  int ForceActiv();
  int ForceActiv(nr *InNr);
  int ForceActiv(nr *InNr, std::vector<vssm*> OutSSM, lk_set_t &SparseOutLkSet);
  int AddActiv();
  int AddActiv(nr *InNr);
  int AddActiv(nr *InNr, std::vector<vssm*> OutSSM, lk_set_t &SparseOutLkSet);
  int OutCN(nr *nr1, float wg);
  int OutCN(nr *nr1, float wgmin, float wgmax);
  int OutCN(vssm *ssm1, float wg);
  int OutCN(vssm *ssm1, float wgmin, float wgmax);
  int SparseOutHbbn(nr *InNr, float OHWpar, std::vector<vssm*> OutSSM,
		    lk_set_t &SparseOutLkSet);
  int OutHbbn(nr *InNr, float OHWpar);
  int SparseOutCreateActiveLks(std::vector<vssm*> OutSSM,
			       lk_set_t &SparseOutLkSet);
};


class vssm_io : virtual public vssm
{
 public:
  std::vector<io_nr*> OutNr;
  std::vector<vssm*> OutSSM;
  std::vector<lk_set_t*> SparseOutLkSet;
  bool SparseOutLkFlag;
  float OHWpar;

  virtual int Init();
  virtual int Init(int size);
  int SetSparseOut();
  int OutFC(vssm *ssm1, float wg);
  int OutFC(vssm *ssm1, float wgmin, float wgmax);
  int SparseOutFC(vssm *ssm1);
  int SaveSparseOutputLinks(FILE *fp);
  int LoadSparseOutputLinks(FILE *fp);
  int SaveOutputLinks(FILE *fp);
  int LoadOutputLinks(FILE *fp);
  long CountOutputLinks();
  long CountSparseOutputLinks();
  long CountVirtualOutputLinks();
};

class ssm_io : public ssm, virtual public vssm_io
{
 public:
  ssm_io();
  ssm_io(int size);
};

class vssm_as : virtual public vssm_io
{
 public:
  nr *BuildFlag;
  nr *RetrFlag;
  nr *RewdFlag;
  wnn_type WTA_FLAG;
  bool UseRewd;
  int K;
  std::vector<int> BiasVect;
  double as_time;

  int Init(int size, wnn_type wta_flag, nr *build_flag, nr *retr_flag);
  int Init(int size, wnn_type wta_flag, nr *build_flag, nr *retr_flag,
	   nr *rewd_flag);
  virtual int AsOut()=0;
  virtual int ActivOut()=0;
  virtual int KWTA()=0;
};

class ssm_as : public ssm_io, public vssm_as
{
 public:
  bool DynamicBiasFlag;
  ssm *DynamicBias;
  int MinWinnNum;
  int MinBias, MaxBias, BiasNum;
  int DB;
  float **OMPActivArr;
  int *dev_Nnr;
  int **dev_Nlk;
  int ***dev_lk_nr;
  int ***h_h_lk_nr;
  float *activ_arr;
  float *dev_activ_arr;

  ssm_as(int size, wnn_type wta_flag, nr *build_flag, nr *retr_flag);
  ssm_as(int size, wnn_type wta_flag, nr *build_flag, nr *retr_flag,
	 nr *rewd_flag);
  int AsOut();
  int ActivOut();
  int KWTA();
  int UseDynamicBias(int min_bias, int max_bias);
  int SetDynamicBias();
  int OMPActiv();
  int OMPSparseActiv();
  int OMPActivArrInit();
  int cuda_CopyInpuLinks();
  int cuda_SparseActiv();
};

   
#endif
