#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <stdlib.h>
#include "sizes.h"
#include "cuPrintf.cu"
#include "gettime.h"

using namespace std;
using namespace sizes;

double cuda_time;
//#define NCols 512

#define gpuErrchk(ans) { gpuAssert((ans), __FILE__, __LINE__); }
inline void gpuAssert(cudaError_t code, char *file, int line, bool abort=true)
{
  if (code != cudaSuccess) {
    fprintf(stderr,"GPUassert: %s %s %d\n", cudaGetErrorString(code),
      file, line);
    if (abort) exit(code);
  }
}

//The macro CUPRINTF is defined for architectures
//with different compute capabilities.
#if __CUDA_ARCH__ < 200     //Compute capability 1.x architectures
#define CUPRINTF cuPrintf
#else                       //Compute capability 2.x architectures
#define CUPRINTF(fmt, ...) printf("[%d, %d]:\t" fmt, \
        blockIdx.y*gridDim.x+blockIdx.x,\
        threadIdx.z*blockDim.x*blockDim.y+threadIdx.y*blockDim.x+threadIdx.x,\
        __VA_ARGS__)
#endif


__global__ void kernel_SparseActiv_blk1(int NRows, int **LkPt,
    int *Nlk, float *in_sign_arr, float *activ_arr)
{
  //float DefaultMinWg = -1;

  if (blockIdx.x<NRows) {
    int nlk = Nlk[blockIdx.x];
    float in_sign = in_sign_arr[blockIdx.x];
    float sum = in_sign;
    int *lk_pt = LkPt[blockIdx.x];
    if (threadIdx.x<nlk) {
      int inr1 = (*(lk_pt+threadIdx.x));
      atomicAdd(&activ_arr[inr1], sum);
    }
  }
}

__global__ void kernel_SparseActiv_blk2(int NRows, int **LkPt,
    int *Nlk, float *in_sign_arr, float *activ_arr)
{
  //float DefaultMinWg = -1;
  int irow = 65536 + blockIdx.x;

  if (irow<NRows) {
    int nlk = Nlk[irow];
    float in_sign = in_sign_arr[irow];
    float sum = in_sign;
    int *lk_pt = LkPt[irow];
    if (threadIdx.x<nlk) {
      int inr1 = (*(lk_pt+threadIdx.x));
      atomicAdd(&activ_arr[inr1], sum);
    }
  }
}

__global__ void kernel_PrintActiv(int arr_size, int NN, float *activ_arr)
{
  int index = threadIdx.x + blockIdx.x*blockDim.x;
  if (index<10) {
    CUPRINTF("cuda: %d\t%f\n", index, activ_arr[index]);
  }
}

int cuda_CopyInputLinks_fn(int Nssm, int *Nnr, int **Nlk, int ***lk_nr,
    int ***lk_nr2, int*** &h_h_lk_nr, int* &dev_Nnr, int** &dev_Nlk,
    int*** &dev_lk_nr, int NN, float* &dev_activ_arr)
{
  int **h_Nlk;
  //int ***h_h_lk_nr;
  int ***h_d_lk_nr;

  gpuErrchk(cudaMalloc((void**)&dev_activ_arr, NN*sizeof(float)));
 
  h_Nlk = (int**)malloc(Nssm*sizeof(int*));
  h_h_lk_nr = (int***)malloc(Nssm*sizeof(int**));
  h_d_lk_nr = (int***)malloc(Nssm*sizeof(int**));

  gpuErrchk(cudaMalloc( (void**)&dev_Nnr, Nssm*sizeof(int)));
  gpuErrchk(cudaMemcpy(dev_Nnr, Nnr, Nssm*sizeof(int), cudaMemcpyHostToDevice));
  gpuErrchk(cudaMalloc( (void**)&dev_Nlk, Nssm*sizeof(int*)));
  gpuErrchk(cudaMalloc( (void**)&dev_lk_nr, Nssm*sizeof(int**)));

  for (int issm=0; issm<Nssm; issm++) {
    gpuErrchk(cudaMalloc( (void**)&h_Nlk[issm], Nnr[issm]*sizeof(int)));
    gpuErrchk(cudaMemcpy(h_Nlk[issm], Nlk[issm], Nnr[issm]*sizeof(int),
      cudaMemcpyHostToDevice));
    h_h_lk_nr[issm] = (int**)malloc(Nnr[issm]*sizeof(int*));
    for (int inr=0; inr<Nnr[issm]; inr++) {
      gpuErrchk(cudaMalloc( (void**)&h_h_lk_nr[issm][inr],
                       Nlk[issm][inr]*sizeof(int)));
      gpuErrchk(cudaMemcpy(h_h_lk_nr[issm][inr], lk_nr[issm][inr],
        Nlk[issm][inr]*sizeof(int), cudaMemcpyHostToDevice));
    }
    gpuErrchk(cudaMalloc((void**)&h_d_lk_nr[issm], Nnr[issm]*sizeof(int*)));
    gpuErrchk(cudaMemcpy(h_d_lk_nr[issm], h_h_lk_nr[issm],
      Nnr[issm]*sizeof(int*), cudaMemcpyHostToDevice));
  } 
  gpuErrchk(cudaMemcpy(dev_Nlk, h_Nlk, Nssm*sizeof(int*),
    cudaMemcpyHostToDevice));
  gpuErrchk(cudaMemcpy(dev_lk_nr, h_d_lk_nr, Nssm*sizeof(int**),
    cudaMemcpyHostToDevice));

  //for (int issm=0; issm<Nssm; issm++) {
  //  free(h_h_lk_nr[issm]);
  //}
  free(h_Nlk);
  //free(h_h_lk_nr);
  free(h_d_lk_nr);

  cuda_time = 0;
  return 0;
}

int cuda_SparseActiv_fn(int NRows, int **LkPt, int *Nlk,
    float *in_sign_arr, int NN, float *activ_arr, int *dev_Nnr,
    int ***dev_lk_nr, float *dev_activ_arr)
{
  int **dev_LkPt;
  int *dev_Nlk;
  float *dev_in_sign;
  struct timespec clk0, clk1;

  //gpuErrchk( cudaDeviceSynchronize() ); GetMonotonicTime(&clk0);
  gpuErrchk(cudaMalloc((void**)&dev_LkPt, NRows*sizeof(int*)));
  gpuErrchk(cudaMalloc((void**)&dev_Nlk, NRows*sizeof(int)));
  gpuErrchk(cudaMalloc( (void**)&dev_in_sign, NRows*sizeof(float)));

  gpuErrchk(cudaMemcpy(dev_LkPt, LkPt, NRows*sizeof(int*),
    cudaMemcpyHostToDevice));
  gpuErrchk(cudaMemcpy(dev_Nlk, Nlk, NRows*sizeof(int),
    cudaMemcpyHostToDevice));
  gpuErrchk(cudaMemcpy(dev_in_sign, in_sign_arr, NRows*sizeof(float),
    cudaMemcpyHostToDevice));

  gpuErrchk(cudaMemcpy(dev_activ_arr, activ_arr, NN*sizeof(float),
    cudaMemcpyHostToDevice));

  gpuErrchk( cudaDeviceSynchronize() ); GetMonotonicTime(&clk0);
  kernel_SparseActiv_blk1<<< 65535, 512 >>>(NRows, dev_LkPt, dev_Nlk,
    dev_in_sign, dev_activ_arr);
  gpuErrchk( cudaPeekAtLastError() );
  gpuErrchk( cudaDeviceSynchronize() );
  kernel_SparseActiv_blk2<<< 65535, 512 >>>(NRows, dev_LkPt, dev_Nlk,
    dev_in_sign, dev_activ_arr);
  gpuErrchk( cudaPeekAtLastError() );
  gpuErrchk( cudaDeviceSynchronize() );
  gpuErrchk( cudaDeviceSynchronize() ); GetMonotonicTime(&clk1);

  gpuErrchk(cudaMemcpy(activ_arr, dev_activ_arr, NN*sizeof(float),
             cudaMemcpyDeviceToHost));

  gpuErrchk(cudaFree(dev_LkPt));
  gpuErrchk(cudaFree(dev_Nlk));
  gpuErrchk(cudaFree(dev_in_sign));

  //gpuErrchk(cudaDeviceSynchronize()); GetMonotonicTime(&clk1);

  cuda_time = cuda_time
    + clk1.tv_sec - clk0.tv_sec + (double)(clk1.tv_nsec - clk0.tv_nsec)*1e-9;

  //cout << "cuda time: " << cuda_time << endl;
  //cout << "NRows: " << NRows << "NCols: " << NCols << endl;
  // NCols = 67158 after train_all_cuda.txt
  //exit(0);

  return 0;
}
