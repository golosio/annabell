#include <stdio.h>
#include <stdlib.h>

#define MAXSIZE 100000

int main(int argc, char *argv[])
{
  int i, j, k, iter, N;
  long seed = 43627543;
  int arr[MAXSIZE];

  if (argc!=2) {
    printf("Usage: %s number\n", argv[0]);
    exit(0);
  }
  sscanf(argv[1], "%d", &N);
  //printf("%d\n", N);
  srand(seed);

  for(i=0; i<N; i++) {
    arr[i]=i+1;
  }

  for(iter=0; iter<1000000; iter++) {
    i = rand()%N;
    j = rand()%N;
    k = arr[i];
    arr[i] = arr[j];
    arr[j] = k;
  }

  for(i=0; i<N; i++) {
    printf("%d\n", arr[i]);
  }

  return 0;
}
