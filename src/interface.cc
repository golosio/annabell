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
#include "sizes.h"
#include "interface.h"

using namespace std;
using namespace sizes;

int interface::w2bin(char *w, int *v)
{
  char c;
  int k;

  for(k=0; k<NC*CSize; k++) v[k] = 0;
  k = 0;
  for(int ic=0; ic<NC; ic++) {
    c=w[ic];
    if (c==0) break; //end string
    for(int ib=0; ib<CSize; ib++) {
      v[k] = c & 1;
      c >>= 1;
      k++;
    }
  }

  return 0;
}

int interface::i2bin(int i, int *v, int size)
{
  for (int ib=0; ib<size; ib++) {
    v[ib] = i & 1;
    i >>= 1;
  }
  
  return 0;
}

int interface::bin2i(int *v, int size)
{
  int i=0;
  for (int ib=size-1; ib>=0; ib--) {
    i <<= 1;
    i += v[ib] ;
  }
  
  return i;
}

int interface::i2v(int i, int *v, int size)
{
  for (int iv=0; iv<size; iv++) {
    if (iv+1 == i) v[iv] = 1;
    else v[iv] = 0;
  }
  
  return 0;
}

int interface::v2i(int *v, int size)
{
  for (int iv=0; iv<size; iv++) {
    if (v[iv] > 0.5) return iv+1;
  }
  
  return 0;
}

